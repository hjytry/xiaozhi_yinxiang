#include "system/includes.h"
#include "media/includes.h"
#include "app_config.h"
#include "tone_player.h"
#include "asm/charge.h"
#include "app_charge.h"
#include "app_main.h"
#include "ui_manage.h"
#include "vm.h"
#include "app_chargestore.h"
#include "user_cfg.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "key_event_deal.h"
#include "smartbox/cmd_user.h"

#define LOG_TAG_CONST       APP_IDLE
#define LOG_TAG             "[APP_IDLE]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"


static void  lcd_ui_power_on_timeout(void *p)
{
#if (TCFG_SPI_LCD_ENABLE)
    /* sys_key_event_enable(); */
    /* logo_time = timer_get_ms(); */
    /* while (timer_get_ms() - logo_time <= 2 * 1000) { //显示开机logo */
    /* os_time_dly(10); */
    /* } */

    UI_HIDE_WINDOW(ID_WINDOW_POWER_ON);
    UI_SHOW_WINDOW(ID_WINDOW_MAIN);
#if TCFG_APP_BT_EN
    app_task_switch_to(APP_BT_TASK);
#else
    app_task_switch_to(APP_MUSIC_TASK);
#endif
#endif

}

static void  lcd_ui_power_on()
{
#if (TCFG_SPI_LCD_ENABLE)
    int logo_time = 0;
    UI_SHOW_WINDOW(ID_WINDOW_POWER_ON);
    sys_timeout_add(NULL, lcd_ui_power_on_timeout, 1000);
#endif
}
extern bool idle_powerlow_flag;
extern bool idle_to_poweron_tone_end;
static int power_on_init(void)
{
    if(idle_powerlow_flag)
    {
        idle_to_poweron_tone_end = TRUE;
    }
    ///有些需要在开机提示完成之后再初始化的东西， 可以在这里初始化
#if (TCFG_SPI_LCD_ENABLE)
    lcd_ui_power_on();//由ui决定切换的模式
    return 0;
#endif

#if TCFG_APP_BT_EN
    //app_task_switch_to(APP_RTC_TASK);
#else

#if TCFG_USB_APPLE_DOCK_EN //苹果iap协议使用pc模式
    app_task_switch_to(APP_PC_TASK);
#else
    app_task_switch_to(APP_SLEEP_TASK);
    /* app_task_switch_to(APP_PC_TASK); */
    /* app_task_switch_to(APP_MUSIC_TASK); */
    /* app_task_switch_to(APP_IDLE_TASK); */
    /* app_task_switch_to(APP_LINEIN_TASK);//如果带检测，设备不在线，则不跳转 */
#endif

#endif

    return 0;
}

static int power_on_unint(void)
{

    tone_play_stop();
    UI_HIDE_CURR_WINDOW();
    return 0;
}






static int poweron_sys_event_handler(struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        break;
    case SYS_BT_EVENT:
        break;
    case SYS_DEVICE_EVENT:
        break;
    default:
        return false;
    }
    return false;
}


static void  tone_play_end_callback(void *priv, int flag)
{
    int index = (int)priv;

    if (APP_POWERON_TASK != app_get_curr_task()) {
        log_error("tone callback task out \n");
        return;
    }

    switch (index) {
    case IDEX_TONE_POWER_ON:
        power_on_init();
        break;
    }
}
extern u8 alarm_power_on_flag;
u8 user_power_on_flag = TRUE;
extern u32 dev_manager_get_total(u8 valid);
void wait_dev_online(void)
{
    if(app_check_curr_task(APP_POWERON_TASK))
    {
        if(dev_manager_get_total(1)&&(!idle_powerlow_flag)&&(!alarm_power_on_flag)){
            app_task_switch_to(APP_MUSIC_TASK);
        }else{
            sys_key_event_enable();
            if(alarm_power_on_flag){
                power_on_init();
            }else{
                // int err =  tone_play_with_callback_by_name(tone_table[IDEX_TONE_RTC], 1, tone_play_end_callback, (void *)IDEX_TONE_POWER_ON);
                // if (err) { //提示音没有,播放失败，直接init流程 */
                //     power_on_init();
                // }
                app_task_switch_to(APP_BT_TASK);
            }
        }
    }
}

void app_poweron_task()
{
    extern void rgb_init(void);
    rgb_init();
    int msg[32];
#if (SMART_BOX_EN)
    extern void  user_get_network_fm_state_buf(void);
    user_get_network_fm_state_buf();
#endif
    UI_SHOW_WINDOW(ID_WINDOW_POWER_ON);
    if(user_power_on_flag)
    {
        user_power_on_flag = FALSE;
        UI_SHOW_MENU(MENU_POWER_UP, 2000, 0, NULL);
        sys_timeout_add(NULL,wait_dev_online,1000);
    }else{
        sys_key_event_enable();
        if(!idle_powerlow_flag){
            int err =  tone_play_with_callback_by_name(tone_table[IDEX_TONE_RTC], 1, tone_play_end_callback, (void *)IDEX_TONE_POWER_ON);
            if (err) { //提示音没有,播放失败，直接init流程 */
                power_on_init();
            }
        }else{
            power_on_init();
        }
    }

    while (1) {
        app_task_get_msg(msg, ARRAY_SIZE(msg), 1);
        switch (msg[0]) {
        case APP_MSG_SYS_EVENT:
            if (poweron_sys_event_handler((struct sys_event *)(msg + 1)) == false) {
                app_default_event_deal((struct sys_event *)(&msg[1]));    //由common统一处理
            }
            break;
        default:
            break;
        }

        if (app_task_exitting()) {
            power_on_unint();
            return;
        }
    }
}
