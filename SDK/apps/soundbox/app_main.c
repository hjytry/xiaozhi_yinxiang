#include "system/includes.h"
#include "app_config.h"
#include "asm/pwm_led.h"
#include "tone_player.h"
#include "ui_manage.h"
#include "app_main.h"
#include "app_task.h"
#include "asm/charge.h"
#include "app_power_manage.h"
#include "app_charge.h"
#include "user_cfg.h"
#include "power_on.h"
#include "bt.h"
#include "soundcard/peripheral.h"

#include "user_pa.h"
#include "key_event_deal.h"

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[APP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



APP_VAR app_var;


void app_entry_idle()
{
    app_task_switch_to(APP_IDLE_TASK);
}



void app_task_loop()
{
    while (1) {
        switch (app_curr_task) {
        case APP_POWERON_TASK:
            log_info("APP_POWERON_TASK \n");
            app_poweron_task();
            break;
        case APP_POWEROFF_TASK:
            log_info("APP_POWEROFF_TASK \n");
            app_poweroff_task();
            break;
        case APP_BT_TASK:
            log_info("APP_BT_TASK \n");
            app_bt_task();
            break;
        case APP_MUSIC_TASK:
            log_info("APP_MUSIC_TASK \n");
            app_music_task();
            break;
        case APP_FM_TASK:
            log_info("APP_FM_TASK \n");
            app_fm_task();
            break;
        case APP_RECORD_TASK:
            log_info("APP_RECORD_TASK \n");
            app_record_task();
            break;
        case APP_LINEIN_TASK:
            log_info("APP_LINEIN_TASK \n");
            app_linein_task();
            break;
        case APP_RTC_TASK:
            log_info("APP_RTC_TASK \n");
            app_rtc_task();
            break;
        case APP_PC_TASK:
            log_info("APP_PC_TASK \n");
            app_pc_task();
            break;
        case APP_SPDIF_TASK:
            log_info("APP_SPDIF_TASK \n");
            app_spdif_task();
            break;
        case APP_IDLE_TASK:
            log_info("APP_IDLE_TASK \n");
            app_idle_task();
            break;
        case APP_SLEEP_TASK:
            log_info("APP_SLEEP_TASK \n");
            app_sleep_task();
            break;
        }
        app_task_clear_key_msg();//清理按键消息
    }
}
extern bool reset_of_8sec_flag;
extern void io_strl(u8 cmd);
extern void user_pa_in_service(void);
extern void user_del_modul(void);
void app_main()
{
    log_info("app_main\n");

    app_var.start_time = timer_get_ms();
    //r_printf("JL_TIMER0->CON = %x ,JL_TIMER1->CON = %x ,JL_TIMER2->CON = %x ,JL_TIMER3->CON = %x\n",JL_TIMER0->CON,JL_TIMER1->CON,JL_TIMER2->CON,JL_TIMER3->CON);
    if (get_charge_online_flag()) {

        app_var.poweron_charge = 1;

#if (TCFG_SYS_LVD_EN == 1)
        vbat_check_init();
#endif
        app_curr_task = APP_IDLE_TASK;
    } else {

#if SOUNDCARD_ENABLE
        soundcard_peripheral_init();
#endif
        /* endless_loop_debug_int(); */
        ui_update_status(STATUS_POWERON);
        u8 user_power_flag = 0;
        syscfg_read(CFG_USER_POWER_FLAG, &user_power_flag, 1); 
        if(reset_of_8sec_flag){
            if(user_power_flag == KEY_POWEROFF){
                user_del_modul();
                app_curr_task = APP_IDLE_TASK;                
            }else{
                app_curr_task = APP_POWERON_TASK;
                io_strl(PA_CLASS_D);
                user_pa_in_service();
            }
            reset_of_8sec_flag = FALSE;
            user_power_flag = 0;
            syscfg_write(CFG_USER_POWER_FLAG, &user_power_flag, 1); 
        }else{
            app_curr_task = APP_POWERON_TASK;
            io_strl(PA_CLASS_D);
            user_pa_in_service();
        }

    }

#if TCFG_CHARGE_BOX_ENABLE
    app_curr_task = APP_IDLE_TASK;
#endif
    app_task_loop();
}



