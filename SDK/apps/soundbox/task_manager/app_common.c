#include "common/app_common.h"
#include "app_task.h"
#include "app_main.h"
#include "key_event_deal.h"
#include "music/music.h"
#include "pc/pc.h"
#include "record/record.h"
#include "linein/linein.h"
#include "fm/fm.h"
#include "btstack/avctp_user.h"
#include "app_power_manage.h"
#include "app_chargestore.h"
#include "usb/otg.h"
#include "usb/host/usb_host.h"
#include <stdlib.h>
#include "bt/bt_tws.h"
#include "audio_config.h"
#include "common/power_off.h"
#include "common/user_msg.h"
#include "audio_config.h"
#include "audio_enc.h"
#include "ui/ui_api.h"
#include "fm_emitter/fm_emitter_manage.h"
#include "common/fm_emitter_led7_ui.h"
#if TCFG_CHARGE_ENABLE
#include "app_charge.h"
#endif
#include "dev_multiplex_api.h"
#include "chgbox_ctrl.h"
#include "device/chargebox.h"
#include "app_online_cfg.h"
#include "soundcard/soundcard.h"
#include "smartbox_bt_manage.h"
#include "bt.h"
#include "common/dev_status.h"
#include "tone_player.h"
#include "ui_manage.h"
#include "soundbox.h"
#include "audio_recorder_mix.h"
#include "bt_emitter.h"

#define LOG_TAG_CONST       APP_ACTION
#define LOG_TAG             "[APP_ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"
#include "smartbox/cmd_user.h"
#include "user_pa.h"
int JL_rcsp_event_handler(struct rcsp_event *rcsp);
int bt_background_event_handler(struct sys_event *event);
extern u32 timer_get_ms(void);
extern int alarm_sys_event_handler(struct sys_event *event);
extern void bt_tws_sync_volume();

extern void pwm_to_close_bright(void);
extern void close_led7_ui(void);
extern void del_user_pa_in_service(void);
extern void add_user_pa_in_service(void);
extern void pwm_to_recovery_bright(void);
extern void add_led7_scan_to_timer(void);
extern void add_led7_ui(void);
extern void del_auto_reduce_bright_timer(void);
//注销 定时器
void user_del_modul(void){
    del_user_pa_in_service();
    pwm_to_close_bright();
    led7_show_null();
    close_led7_ui();
    del_auto_reduce_bright_timer();
}

void user_add_modul(void){
    add_user_pa_in_service();
    add_led7_ui();
    pwm_to_recovery_bright();
}
u8 user_poweroff_cnt = 0;
bool user_poweroff_flag = FALSE;
bool user_poweroff_cnt_flag = FALSE;
void  common_power_on_tone_play_end_callback(void *priv, int flag)
{
    u32 index = (u32)priv;

    // if (APP_IDLE_TASK != app_get_curr_task()) {
    //     log_error("tone callback task out \n");
    //     return;
    // }

    switch (index) {
    case IDEX_TONE_POWER_OFF:
        ///提示音播放结束
        printf(" >>>>> tone \n");
        extern void del_vbat_timer_normal(void);
        del_vbat_timer_normal();
        user_del_modul();
        app_task_switch_to(APP_IDLE_TASK);
        break;
    default:
        break;
    }
}

extern u8 key_rtc_4_press_falg;
bool bt_back_mode_falg = FALSE;
extern void get_rtc_sys_time(struct sys_time *time);
#define USER_POWER_OFF_CNT  10
int app_common_key_msg_deal(struct sys_event *event)
{
    int ret = false;
    struct key_event *key = &event->u.key;
    int key_event = event->u.key.event;
    int key_value = event->u.key.value;

    if (key_event == KEY_NULL) {
        return false;
    }
    if (key_is_ui_takeover()) {
        ui_key_msg_post(key_event);
        return false;
    }
#if (TCFG_UI_ENABLE && TCFG_APP_FM_EMITTER_EN)
    if (!ui_fm_emitter_common_key_msg(key_event)) {
        return false;
    }
#endif

    log_info("common_key_event:%d\n", key_event);

    if ((key_event != KEY_POWEROFF) && (key_event != KEY_POWEROFF_HOLD)) {
        extern u8 goto_poweroff_first_flag;
        goto_poweroff_first_flag = 0;
    }

#if (SMART_BOX_EN)
    extern bool smartbox_key_event_filter_before(int key_event);
    if (smartbox_key_event_filter_before(key_event)) {
        return true;
    }
#endif

    switch (key_event) {
#if TCFG_APP_BT_EN

#if TCFG_USER_TWS_ENABLE
    case KEY_TWS_CONN:
        log_info("    KEY_TWS_CONN \n");
        bt_open_tws_conn(0);
        break;
    case KEY_TWS_DISCONN:
        log_info("    KEY_TWS_DISCONN \n");
        bt_disconnect_tws_conn();
        break;
    case KEY_TWS_REMOVE_PAIR:
        log_info("    KEY_TWS_REMOVE_PAIR \n");
        bt_tws_remove_tws_pair();
        break;
    case KEY_TWS_SEARCH_PAIR:
        log_info("    KEY_TWS_SEARCH_PAIR \n");
        bt_tws_start_search_and_pair();
        break;
    case KEY_TWS_SEARCH_REMOVE_PAIR:
        log_info("    KEY_TWS_SEARCH_REMOVE_PAIR \n");
        bt_tws_search_or_remove_pair();
        break;
#endif

    case KEY_BT_DIRECT_INIT:
        bt_direct_init();
        break;
    case KEY_BT_DIRECT_CLOSE:
        bt_direct_close();
        break;
#endif

    case  KEY_POWEROFF:
    case  KEY_POWEROFF_HOLD:
        power_off_deal(event, key_event - KEY_POWEROFF);
        break;

    case KEY_IR_NUM_0:
    case KEY_IR_NUM_1:
    case KEY_IR_NUM_2:
    case KEY_IR_NUM_3:
    case KEY_IR_NUM_4:
    case KEY_IR_NUM_5:
    case KEY_IR_NUM_6:
    case KEY_IR_NUM_7:
    case KEY_IR_NUM_8:
    case KEY_IR_NUM_9:
        break;
    case KEY_USER_POWEROFF:
    {
        r_printf("write timer\n");
        user_poweroff_cnt_flag = TRUE;
        u8 user_power_flag = KEY_POWEROFF;
        struct sys_time current_time;
        get_rtc_sys_time(&current_time);
        syscfg_write(CFG_USER_ALARM_TIMER, &current_time, sizeof(struct sys_time));
        syscfg_write(CFG_USER_POWER_FLAG, &user_power_flag, 1);
    }
        break;
    case KEY_USER_POWEROFF_HOLD:
        puts("KEY_USER_POWEROFF\n");
#if USER_POWEROFF
        if(user_poweroff_cnt_flag){
            if(user_poweroff_cnt < USER_POWER_OFF_CNT){
                user_poweroff_cnt++;
            }else{
                if(!user_poweroff_flag){
                    user_poweroff_flag = TRUE;
                    UI_SHOW_WINDOW(ID_WINDOW_IDLE);
                    UI_SHOW_MENU(MENU_POWER_OFF, 1000, 0, NULL);
                    if(APP_IDLE_TASK != app_get_curr_task()){
                        // user_power_off();

                        if(APP_LINEIN_TASK == app_get_curr_task()){
                            #if TCFG_LINEIN_ENABLE
                            linein_stop();
                            #endif
                        }

                        int err =  tone_play_with_callback_by_name(tone_table[IDEX_TONE_POWER_OFF], 1, common_power_on_tone_play_end_callback, (void *)IDEX_TONE_POWER_OFF);
                        if (err) {
                            printf(" >>>>> tone \n");
                            extern void del_vbat_timer_normal(void);
                            del_vbat_timer_normal();
                            user_del_modul();
                            app_task_switch_to(APP_IDLE_TASK);
                        }
                    }
                }
            }
        }
        r_printf("user_poweroff_cnt %d",user_poweroff_cnt);
#endif
        break;
    case KEY_UP_USER_POWEROFF:
        puts("KEY_UP_USER_POWEROFF\n");
        user_poweroff_cnt = 0;
        user_poweroff_cnt_flag = FALSE;
        break;
    case KEY_CHANGE_MODE:
#if (TCFG_DEC2TWS_ENABLE)
        if (!key->init) {
            break;
        }
#endif
#if TWFG_APP_POWERON_IGNORE_DEV
        if ((timer_get_ms() - app_var.start_time) > TWFG_APP_POWERON_IGNORE_DEV)
#endif//TWFG_APP_POWERON_IGNORE_DEV

        {
            printf("KEY_CHANGE_MODE\n");
            app_task_switch_next();
        }
        break;

    case KEY_VOL_UP:
        log_info("COMMON KEY_VOL_UP\n");
        if (!tone_get_status()) {
            app_audio_volume_up(1);
            printf("common vol+: %d", app_audio_get_volume(APP_AUDIO_STATE_MUSIC));
        }
        if (app_audio_get_volume(APP_AUDIO_CURRENT_STATE) == app_audio_get_max_volume()) {
            if (tone_get_status() == 0) {
#if TCFG_MAX_VOL_PROMPT
                tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 1);
#endif
            }
        }

#if (TCFG_DEC2TWS_ENABLE)
        bt_tws_sync_volume();
#endif
        UI_SHOW_MENU(MENU_MAIN_VOL, 1000, app_audio_get_volume(APP_AUDIO_STATE_MUSIC), NULL);
        break;

    case KEY_VOL_DOWN:
        log_info("COMMON KEY_VOL_DOWN\n");
        if (!tone_get_status()) {
            app_audio_volume_down(1);
            printf("common vol-: %d", app_audio_get_volume(APP_AUDIO_STATE_MUSIC));
        }
        if (app_audio_get_volume(APP_AUDIO_CURRENT_STATE) == 0) {
            if (tone_get_status() == 0) {
#if TCFG_MAX_VOL_PROMPT
                // tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 1);
#endif
            }
        }
#if (TCFG_DEC2TWS_ENABLE)
        bt_tws_sync_volume();
#endif
        UI_SHOW_MENU(MENU_MAIN_VOL, 1000, app_audio_get_volume(APP_AUDIO_STATE_MUSIC), NULL);
        break;

    case  KEY_EQ_MODE:
#if(TCFG_EQ_ENABLE == 1)
        eq_mode_sw();
#endif
        break;
#if (AUDIO_OUTPUT_WAY == AUDIO_OUTPUT_WAY_BT)


    case KEY_BT_EMITTER_RECEIVER_SW:
        printf(" KEY_BT_EMITTER_RECEIVER_SW\n");
        bt_emitter_receiver_sw();
        break;

    case KEY_BT_EMITTER_PAUSE:
        r_printf(" KEY_BT_EMITTER_PAUSE\n");
        bt_emitter_pp(0);
        break;

    case KEY_BT_EMITTER_PLAY:
        r_printf(" KEY_BT_EMITTER_PLAY\n");
        bt_emitter_pp(1);
        break;

    case KEY_BT_EMITTER_SW:
        r_printf("KEY_BT_EMITTER_SW\n");
        {
            extern u8 bt_emitter_stu_sw(void);

            if (bt_emitter_stu_sw()) {
                printf("bt emitter start \n");
            } else {
                printf("bt emitter stop \n");
            }
        }
        break;
#endif


#if (TCFG_CHARGE_BOX_ENABLE)
    case  KEY_BOX_POWER_CLICK:
    case  KEY_BOX_POWER_LONG:
    case  KEY_BOX_POWER_HOLD:
    case  KEY_BOX_POWER_UP:
    case  KEY_BOX_POWER_DOUBLE:
    case  KEY_BOX_POWER_THREE:
    case  KEY_BOX_POWER_FOUR:
    case  KEY_BOX_POWER_FIVE:
        charge_box_key_event_handler(key_event);
        break;
#endif
#if (TCFG_MIC_EFFECT_ENABLE)
    case KEY_REVERB_OPEN:
#if TCFG_USER_TWS_ENABLE
        if (!key->init) {
            break;
        }
#endif
        if (mic_effect_get_status()) {
            mic_effect_stop();
        } else {
            mic_effect_start();
        }
        ret = true;
        break;
#endif
    case KEY_ENC_START:
#if (RECORDER_MIX_EN)
        if (recorder_mix_get_status()) {
            printf("recorder_encode_stop\n");
            recorder_mix_stop();
        } else {
            printf("recorder_encode_start\n");
            recorder_mix_start();
        }
#endif/*RECORDER_MIX_EN*/
        break;
    case KEY_TO_SW_RGB:
    {
        extern void rgb_sw_light(void);
        rgb_sw_light();
    }
    break;
    case  KEY_TO_ADJUST_BRIGHT:
        r_printf("KEY_TO_ADJUST_BRIGHT\n");
        extern void pwm_to_adjust_bright(void);
        pwm_to_adjust_bright();
    break;
    case KEY_TO_CLOSE_BRIGHT:
        r_printf("KEY_TO_CLOSE_BRIGHT\n");
        extern void pwm_to_close_bright(void);
        pwm_to_close_bright();
    break;
    case KEY_TO_OPEN_BRIGHT:
        r_printf("KEY_TO_OPEN_BRIGHT\n");
        extern void pwm_to_open_bright(void);
        pwm_to_open_bright();
    break;
    case  KEY_TO_ADJUST_UP_BRIGHT:
        r_printf("KEY_TO_ADJUST_UP_BRIGHT\n");
        extern void pwm_to_adjust_up_bright(void);
        pwm_to_adjust_up_bright();
    break;
    case  KEY_TO_ADJUST_DOWN_BRIGHT:
        r_printf("KEY_TO_ADJUST_DOWN_BRIGHT\n");
        extern void pwm_to_adjust_down_bright(void);
        pwm_to_adjust_down_bright();
    break;
    case KEY_LIGHTING_SW:
        extern void sw_lighting_mode(void);
        sw_lighting_mode();
    break;
    case KEY_LIGHTING_CLOSE:
        extern void set_lighting_mode(u8 mode);
        set_lighting_mode(LIGHTING_CLOSE_ALL);
    break;
    case KEY_TO_PP_NETWORK_FM:
#if (SMART_BOX_EN)
        extern void user_ctrl_pp_network_fm(u8 cmd);
        if(app_check_curr_task(APP_BT_TASK)){
            user_ctrl_pp_network_fm(FM_PP);
        }else{
            user_ctrl_pp_network_fm(FM_PAUSE);
        }
        break;
#endif
    default:
        ui_key_msg_post(key_event);
#if (SOUNDCARD_ENABLE)
        soundcard_event_deal(event);
#endif
        break;

    }
#if (SMART_BOX_EN)
    extern int smartbox_common_key_event_deal(int key_event, int ret);
    ret = smartbox_common_key_event_deal(key_event, ret);
#endif
    extern int rtc_key_event_opr(struct sys_event *event);
    rtc_key_event_opr(event);
    return ret;
}

int app_power_user_event_handler(struct device_event *dev)
{
#if(TCFG_SYS_LVD_EN == 1)
    switch (dev->event) {
    case POWER_EVENT_POWER_WARNING:
        ui_update_status(STATUS_LOWPOWER);
        //tone_play_by_path(tone_table[IDEX_TONE_LOW_POWER], 1);
        /* return 0; */
    }
#if (SOUNDCARD_ENABLE)
    soundcard_power_event(dev);
#endif
#endif
    return app_power_event_handler(dev);
}

static void app_common_device_event_handler(struct sys_event *event)
{
    int ret = 0;
    const char *logo = NULL;
    const char *usb_msg = NULL;
    u8 app  = 0xff ;
    u8 alarm_flag = 0;
    switch ((u32)event->arg) {
#if TCFG_CHARGE_ENABLE
    case DEVICE_EVENT_FROM_CHARGE:
        app_charge_event_handler(&event->u.dev);
        break;
#endif//TCFG_CHARGE_ENABLE

#if TCFG_ONLINE_ENABLE
    case DEVICE_EVENT_FROM_CI_UART:
        ci_data_rx_handler(CI_UART);
        break;

#if TCFG_USER_TWS_ENABLE
    case DEVICE_EVENT_FROM_CI_TWS:
        ci_data_rx_handler(CI_TWS);
        break;
#endif//TCFG_USER_TWS_ENABLE
#endif//TCFG_ONLINE_ENABLE

    case DEVICE_EVENT_FROM_POWER:
        app_power_user_event_handler(&event->u.dev);
        break;

#if TCFG_CHARGESTORE_ENABLE || TCFG_TEST_BOX_ENABLE
    case DEVICE_EVENT_CHARGE_STORE:
        app_chargestore_event_handler(&event->u.chargestore);
        break;
#endif//TCFG_CHARGESTORE_ENABLE || TCFG_TEST_BOX_ENABLE

#if(TCFG_CHARGE_BOX_ENABLE)
    case DEVICE_EVENT_FROM_CHARGEBOX:
        charge_box_ctrl_event_handler(&event->u.chargebox);
        break;
#endif


    case DEVICE_EVENT_FROM_OTG:
        ///先分析OTG设备类型
        usb_msg = (const char *)event->u.dev.value;
        if (usb_msg[0] == 's') {
            ///是从机
#if TCFG_PC_ENABLE
            ret = pc_device_event_handler(event);
            if (ret == true) {
                app = APP_PC_TASK;
            }
#endif
            break;
        } else if (usb_msg[0] == 'h') {
            ///是主机, 统一于SD卡等响应主机处理，这里不break
        } else {
            log_e("unknow otg devcie !!!\n");
            break;
        }
    case DRIVER_EVENT_FROM_SD0:
    case DRIVER_EVENT_FROM_SD1:
    case DRIVER_EVENT_FROM_SD2:
#if TCFG_APP_MUSIC_EN
        ret = dev_status_event_filter(event);///解码设备上下线， 设备挂载等处理
        if (ret == true) {
            if (event->u.dev.event == DEVICE_EVENT_IN) {
                ///设备上线， 非解码模式切换到解码模式播放
                if (app_get_curr_task() != APP_MUSIC_TASK) {
                    app = APP_MUSIC_TASK;
                }
            }
        }
#endif
        break;

#if TCFG_APP_LINEIN_EN
    case DEVICE_EVENT_FROM_LINEIN:
        ret = linein_device_event_handler(event);
        if (ret == true) {
            app = APP_LINEIN_TASK;
        }
        break;
#endif//TCFG_APP_LINEIN_EN

#if TCFG_APP_RTC_EN
    case DEVICE_EVENT_FROM_ALM:
        ret = alarm_sys_event_handler(event);
        // if (ret == true) {
        //     alarm_flag = 1;
        //     app = APP_RTC_TASK;
        // }
        break;
#endif//TCFG_APP_RTC_EN

    default:
        /* printf("unknow SYS_DEVICE_EVENT!!, %x\n", (u32)event->arg); */
        break;
    }

#if (SMART_BOX_EN)
    extern void smartbox_update_dev_state(u32 event);
    smartbox_update_dev_state((u32)event->arg);
#endif

    if (app != 0xff) {
        if ((true != app_check_curr_task(APP_PC_TASK)) || alarm_flag) {

            //PC 不响应因为设备上线引发的模式切换
#if TWFG_APP_POWERON_IGNORE_DEV
//            if ((timer_get_ms() - app_var.start_time) > TWFG_APP_POWERON_IGNORE_DEV)
            extern u8 alarm_power_on_flag;
            extern bool idle_powerlow_flag;
            if((!alarm_power_on_flag)&&(!idle_powerlow_flag))
#endif//TWFG_APP_POWERON_IGNORE_DEV
            {
#if (TCFG_CHARGE_ENABLE && (!TCFG_CHARGE_POWERON_ENABLE))
                extern u8 get_charge_online_flag(void);
                if (get_charge_online_flag()) {

                } else
#endif
                {

                    printf(">>>>>>>>>>>>>%s %d \n", __FUNCTION__, __LINE__);
                    app_task_switch_to(app);
                }
            }
        }
    }
}


///公共事件处理， 各自模式没有处理的事件， 会统一在这里处理
void app_default_event_deal(struct sys_event *event)
{
    int ret;
    SYS_EVENT_HANDLER_SPECIFIC(event);
    switch (event->type) {
    case SYS_DEVICE_EVENT:
        /*默认公共设备事件处理*/
        /* printf(">>>>>>>>>>>>>%s %d \n", __FUNCTION__, __LINE__); */
        app_common_device_event_handler(event);
        break;
#if TCFG_APP_BT_EN
    case SYS_BT_EVENT:
        if (true != app_check_curr_task(APP_BT_TASK)) {
            /*默认公共BT事件处理*/
            bt_background_event_handler(event);
        }
        break;
#endif
    case SYS_KEY_EVENT:
        app_common_key_msg_deal(event);
        break;
    default:
        printf("unknow event\n");
        break;
    }
}


#if 0
extern int key_event_remap(struct sys_event *e);
extern const u16 bt_key_ad_table[KEY_AD_NUM_MAX][KEY_EVENT_MAX];
u8 app_common_key_var_2_event(u32 key_var)
{
    u8 key_event = 0;
    u8 key_value = 0;
    struct sys_event e = {0};
#if TCFG_ADKEY_ENABLE
    for (; key_value < KEY_AD_NUM_MAX; key_value++) {
        for (key_event = 0; key_event < KEY_EVENT_MAX; key_event++) {
            if (bt_key_ad_table[key_value][key_event] == key_var) {
                e.type = SYS_KEY_EVENT;
                e.u.key.type = KEY_DRIVER_TYPE_AD;
                e.u.key.event = key_event;
                e.u.key.value = key_value;
                /* e.u.key.tmr = timer_get_ms(); */
                e.arg  = (void *)DEVICE_EVENT_FROM_KEY;
                /* printf("key2event:%d %d %d\n", key_var, key_value, key_event); */
                if (key_event_remap(&e)) {
                    sys_event_notify(&e);
                    return true;
                }
            }
        }
    }
#endif
    return false;
}

#else

u8 app_common_key_var_2_event(u32 key_var)
{
    struct sys_event e = {0};
    e.type = SYS_KEY_EVENT;
    e.u.key.type = KEY_DRIVER_TYPE_SOFTKEY;
    e.u.key.event = key_var;
    e.arg  = (void *)DEVICE_EVENT_FROM_KEY;
    sys_event_notify(&e);
    return true;
}
#endif

