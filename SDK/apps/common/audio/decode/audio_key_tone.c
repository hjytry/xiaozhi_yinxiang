#include "asm/includes.h"
#include "media/includes.h"
#include "system/includes.h"
#include "app_config.h"
#include "audio_config.h"
#include "audio_dec.h"
#include "tone_player.h"

#if TCFG_KEY_TONE_EN

#define AUDIO_KEY_TONE_TASK_NAME				"key_tone"

#define AUDIO_KEY_TONE_BUF_LEN					256
#define AUDIO_KEY_TONE_SINE_NUM_MAX				3

struct __key_tone_dec {
    struct audio_sin_param sin_parm[AUDIO_KEY_TONE_SINE_NUM_MAX];
    struct audio_sin_param *sin_src;
    u8	sin_num;
    u8	sin_repeat;
    u16	sin_default_sr;
    u32 sin_volume;
    void *sin_maker;
    u32	sample_rate;
    u8	out_ch;
    u32 sin_len;
    u32 sin_total;
    s16 buf[AUDIO_KEY_TONE_BUF_LEN];
    u32 buf_total;
    u32 buf_len;
    struct audio_dac_channel dac_ch;
    struct audio_stream_entry entry;	// 音频流入口
    struct audio_stream *stream;	// 音频流
};

struct __key_tone {
    OS_SEM sem;
    volatile u8	busy;
    volatile u8	start;
    volatile u8	play;
    struct __key_tone_dec *dec;
};

//////////////////////////////////////////////////////////////////////////////

static struct __key_tone *key_tone = NULL;

static const struct audio_sin_param key_tone_sine[] = {
    /*{0, 1000, 0, 100},*/
    {200 << 9, 4000, 0, 100},
};

//////////////////////////////////////////////////////////////////////////////
extern int sin_tone_make(void *_maker, void *data, int len);
extern void *sin_tone_open(void *param, int num, u8 channel, u8 repeat);
extern int sin_tone_points(void *_maker);
extern void sin_tone_close(void *_maker);

extern struct audio_dac_hdl dac_hdl;


//////////////////////////////////////////////////////////////////////////////
static void sine_param_resample(struct __key_tone_dec *dec, struct audio_sin_param *dst, struct audio_sin_param *src, u32 sample_rate)
{
    u32 coef = (sample_rate << 10) / (u32)dec->sin_default_sr;

    dst->freq = (src->freq << 10) / coef;
    dst->points = ((u64)src->points * coef) >> 10;
    dst->win = src->win;
    if (src->win) {
        dst->decay = ((u64)src->decay << 10) / coef;
    } else {
        dst->decay = (((u64)dec->sin_volume * src->decay / 100) << 8) / dst->points;
        if (dst->decay & 0xff) {
            dst->decay = (dst->decay >> 8) + 1;
        } else {
            dst->decay = dst->decay >> 8;
        }
    }
}

static void key_tone_play_stream_resume(void *priv)
{
    struct __key_tone *ktone = (struct __key_tone *)priv;
    if (key_tone && key_tone->start) {
        os_sem_set(&key_tone->sem, 0);
        os_sem_post(&key_tone->sem);
    }
}

static void key_tone_play_data_process_len(struct audio_stream_entry *entry,  int len)
{
    struct __key_tone_dec *dec = container_of(entry, struct __key_tone_dec, entry);
    dec->buf_len += len;
}

static void key_tone_play_close(struct __key_tone *ktone)
{
    if (ktone->dec) {
        y_printf("keytone close \n");
        struct __key_tone_dec *dec = ktone->dec;
        // 发送stop数据流
        struct audio_data_frame frame = {0};
        frame.channel = dec->out_ch;
        frame.sample_rate = dec->sample_rate;
        frame.stop = 1;
        audio_stream_run(&dec->entry, &frame);
        // 关闭dac通道
        audio_dac_free_channel(&dec->dac_ch);
        // 关闭sin句柄
        if (dec->sin_maker) {
            sin_tone_close(dec->sin_maker);
            dec->sin_maker = NULL;
        }
        // 关闭数据流
        if (dec->stream) {
            audio_stream_close(dec->stream);
            dec->stream = NULL;
        }
        free(ktone->dec);
        ktone->dec = NULL;
    }
}

static int key_tone_play_run(struct __key_tone *ktone)
{
    int res = os_sem_pend(&ktone->sem, 0);
    if (res) {
        return -1;
    }
    if (!ktone->start) {
        return -1;
    }
    if (ktone->play) {
        ktone->play = 0;
        key_tone_play_close(ktone);
        ktone->dec = zalloc(sizeof(struct __key_tone_dec));
        ASSERT(ktone->dec);
        struct __key_tone_dec *dec = ktone->dec;

        // dec数据流
        dec->entry.data_process_len = key_tone_play_data_process_len;
        // dac通道
        audio_dac_new_channel(&dac_hdl, &dec->dac_ch);
        struct audio_dac_channel_attr attr;
        attr.delay_time = 50;
        attr.write_mode = WRITE_MODE_BLOCK;
        audio_dac_channel_set_attr(&dec->dac_ch, &attr);
        // 声道和采样率
        dec->out_ch = audio_output_channel_num();
        dec->sample_rate = audio_dac_get_sample_rate(&dac_hdl);
        if (dec->sample_rate == 0) {
            dec->sample_rate = 16000;
        }
        // sin初始化
        dec->sin_src = key_tone_sine;
        dec->sin_num = ARRAY_SIZE(key_tone_sine);
        if (dec->sin_num > AUDIO_KEY_TONE_SINE_NUM_MAX) {
            dec->sin_num = AUDIO_KEY_TONE_SINE_NUM_MAX;
        }
        dec->sin_default_sr = 16000;
        dec->sin_volume = 26843546;
        for (int i = 0; i < dec->sin_num; i++) {
            sine_param_resample(dec, &dec->sin_parm[i], dec->sin_src + i, dec->sample_rate);
        }
        dec->sin_maker = sin_tone_open(dec->sin_parm, dec->sin_num, dec->out_ch, dec->sin_repeat);
        ASSERT(dec->sin_maker);
        dec->sin_total = sin_tone_points(dec->sin_maker) * 2;

        // 数据流串联
        struct audio_stream_entry *entries[8] = {NULL};
        u8 entry_cnt = 0;
        entries[entry_cnt++] = &dec->entry;
        entries[entry_cnt++] = &dec->dac_ch.entry;
        dec->stream = audio_stream_open(ktone, key_tone_play_stream_resume);
        audio_stream_add_list(dec->stream, entries, entry_cnt);
        y_printf("keytone ch:%d, sr:%d \n", dec->out_ch, dec->sample_rate);
    }

    if (ktone->dec) {
        struct __key_tone_dec *dec = ktone->dec;
        while (1) {
            struct audio_data_frame frame = {0};
            frame.channel = dec->out_ch;
            frame.sample_rate = dec->sample_rate;
            if (dec->buf_len < dec->buf_total) {
                // 发送数据
                frame.data = &dec->buf[dec->buf_len / 2];
                frame.data_len = dec->buf_total - dec->buf_len;
                /* putchar('['); */
                audio_stream_run(&dec->entry, &frame);
                /* putchar(']'); */
                if (dec->buf_len < dec->buf_total) {
                    /* putchar('!'); */
                    // 输出不了，退出
                    return 0;
                }
                dec->sin_len += dec->buf_total;
                if (dec->sin_len >= dec->sin_total) {
                    // 数据全部播放完
                    key_tone_play_close(ktone);
                    break;
                }
            }
            int rlen = sin_tone_make(dec->sin_maker, dec->buf, sizeof(dec->buf));
            /* y_printf("sin rl:%d \n", rlen); */
            if (rlen == 0) {
                // 没有数据了
                key_tone_play_close(ktone);
                break;
            }
            dec->buf_total = rlen;
            dec->buf_len = 0;
        }
    }

    return 0;
}

static void key_tone_task_deal(void *p)
{
    int res = 0;
    struct __key_tone *ktone = (struct __key_tone *)p;
    ktone->start = 1;
    ktone->busy = 1;
    while (1) {
        res = key_tone_play_run(ktone);
        if (res) {
            ///等待删除线程
            key_tone_play_close(ktone);
            ktone->busy = 0;
            while (1) {
                os_time_dly(10000);
            }
        }
    }
}

void audio_key_tone_destroy(void)
{
    if (!key_tone) {
        return ;
    }

    key_tone->start = 0;

    os_sem_set(&key_tone->sem, 0);
    os_sem_post(&key_tone->sem);

    while (key_tone->busy) {
        os_time_dly(1);
    }

    task_kill(AUDIO_KEY_TONE_TASK_NAME);
    os_sem_del(&key_tone->sem, 0);

    local_irq_disable();
    free(key_tone);
    key_tone = NULL;
    local_irq_enable();
}

int audio_key_tone_init(void)
{
    int err = 0;
    if (key_tone) {
        return true;
    }
    struct __key_tone *ktone = zalloc(sizeof(struct __key_tone));
    os_sem_create(&ktone->sem, 0);
    err = task_create(key_tone_task_deal, (void *)ktone, AUDIO_KEY_TONE_TASK_NAME);
    if (err != OS_NO_ERR) {
        printf("%s creat fail %x\n", __FUNCTION__,  err);
        free(ktone);
        return false;
    }
    key_tone = ktone;
    return true;
}

void audio_key_tone_play(void)
{
    if (key_tone && key_tone->start) {
        key_tone->play = 1;
        os_sem_set(&key_tone->sem, 0);
        os_sem_post(&key_tone->sem);
    }
}

#endif

