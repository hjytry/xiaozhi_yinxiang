#include "effect_linein.h"
#include "stream_entry.h"
#include "application/audio_dig_vol.h"
#include "system/includes.h"
#include "audio_config.h"
#include "audio_enc.h"
#include "audio_dec.h"
#include "audio_adc.h"
#include "audio_config.h"
#include "media/includes.h"


struct __effect_linein {
    struct audio_adc_ch  		ch;
    struct audio_adc_output_hdl output;
    s16 						*adc_buf;
    u16 						adc_buf_len;
    void 						*dvol;
    struct __stream_entry 		*stream;
};

int effect_linein_mix_callback(void *priv, s16 *data, u16 len)
{
    struct __effect_linein *linein = (struct __effect_linein *)priv;
    if (linein && linein->adc_buf) {
#if SYS_DIGVOL_GROUP_EN
        if (linein->dvol) {
            audio_dig_vol_run(linein->dvol, linein->adc_buf, len);
        }
#endif/*SYS_DIGVOL_GROUP_EN*/
        int i;
        s32 tmp;
        int points = len >> 1;
        for (i = 0; i < points; i++) {
            data[i] = data_sat_s16(data[i] + linein->adc_buf[i]);
        }
    }
    return len;
}


static void linein_adc_irq_output(void *priv, s16 *data, int len)
{
    struct __effect_linein  *linein = (struct __effect_linein *)priv;
    if (linein) {
        linein->adc_buf = data;
        linein->adc_buf_len = len;
    }
}

struct __effect_linein *effect_linein_open(void)
{
    struct __effect_linein *linein = zalloc(sizeof(struct __effect_linein));
    if (linein) {
        linein->stream = stream_entry_open(linein, effect_linein_mix_callback, 0);
        if (linein->stream) {
            if (audio_linein_open(&linein->ch, MIC_EFFECT_SAMPLERATE, 3) == 0) {
#if SYS_DIGVOL_GROUP_EN
                audio_dig_vol_param linein_digvol_param = {
                    .vol_start = 0,
                    .vol_max = 100,
                    .ch_total = 2,
                    .fade_en = 1,
                    .fade_points_step = 5,
                    .fade_gain_step = 10,
                    .vol_list = NULL,
                };
                linein->dvol = audio_dig_vol_open(&linein_digvol_param);

                if (linein->dvol) {
                    audio_dig_vol_group_add(sys_digvol_group, linein->dvol, "music_linein");
                }
#endif/*SYS_DIGVOL_GROUP_EN*/
                linein->output.handler = linein_adc_irq_output;
                linein->output.priv = linein;
                audio_linein_add_output(&linein->output);
                audio_linein_start(&linein->ch);
                printf("audio_linein_open ok!!!\n ");
                return linein;
            }
        } else {
            effect_linein_close(&linein);
            return NULL;
        }
    }
    return NULL;
}

void effect_linein_close(struct __effect_linein **hdl)
{
    if (hdl && (*hdl))		{
        struct __effect_linein *linein = *hdl;
        stream_entry_close(&linein->stream);
#if SYS_DIGVOL_GROUP_EN
        sys_digvol_group_ch_close("music_linein");
#endif/*SYS_DIGVOL_GROUP_EN*/
        audio_linein_close(&linein->ch, &linein->output);
        local_irq_disable();
        free(linein);
        *hdl = NULL;
        local_irq_enable();
    }
}

struct audio_stream_entry *effect_linein_get_stream_entry(struct __effect_linein *hdl)
{
    if (hdl && hdl->stream) {
        return &(hdl->stream->entry);
    }
    return NULL;
}



