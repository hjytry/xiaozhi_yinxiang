#include "system/includes.h"
#include "rtc/alarm.h"
#include "common/app_common.h"
#include "system/timer.h"
#include "app_main.h"
#include "tone_player.h"
#include "app_task.h"

#include "syscfg_id.h"
#include "sys_time.h"
#include "key_event_deal.h"
#include "sys_time.h"
#include "stdlib.h"
#include "app_power_manage.h"
#if TCFG_APP_RTC_EN

#define ALARM_RING_MAX 120
volatile u16 g_alarm_ring_cnt = 0;
static u16 g_ring_playing_timer = 0;

/*************************************************************
   此文件函数主要是用户主要修改的文件

void set_rtc_default_time(struct sys_time *t)
设置默认时间的函数

void alm_wakeup_isr(void)
闹钟到达的函数

void alarm_event_handler(struct sys_event *event, void *priv)
监听按键消息接口,应用于随意按键停止闹钟


int alarm_sys_event_handler(struct sys_event *event)
闹钟到达响应接口

void alarm_ring_start()
闹钟铃声播放接口


void alarm_stop(void)
闹钟铃声停止播放接口
**************************************************************/




void set_rtc_default_time(struct sys_time *t)
{
    t->year = 2020;
    t->month = 1;
    t->day = 1;
    t->hour = 0;
    t->min = 0;
    t->sec = 0;
}


__attribute__((weak))
u8 rtc_app_alarm_ring_play(u8 alarm_state)
{
    return 0;
}

bool idle_powerlow_flag = FALSE;
bool idle_to_poweron_tone_end = FALSE;
static u16 to_set_alm_timer = 0;
void to_set_alm(void)
{
    if(idle_to_poweron_tone_end){
        struct sys_event e;
        e.type = SYS_DEVICE_EVENT;
        e.arg  = (void *)DEVICE_EVENT_FROM_ALM;
        e.u.dev.event = DEVICE_EVENT_IN;
        e.u.dev.value = 0;
        sys_event_notify(&e);
        idle_to_poweron_tone_end = FALSE;
    }else{
        to_set_alm_timer = sys_timeout_add(NULL,to_set_alm,200);
    }
}


#define USER_LOW_POWER_IN_IDLE    1
extern void idle_app_open_module();
extern bool user_of_idle_to_poweron_flag;
void alm_wakeup_isr(void)
{
    if (!is_sys_time_online()) {
        alarm_active_flag_set(true);
    } else {
        alarm_active_flag_set(true);
        if(app_check_curr_task(APP_IDLE_TASK)&&(user_of_idle_to_poweron_flag)){
            idle_powerlow_flag = TRUE;
            #if USER_LOW_POWER_IN_IDLE
                idle_app_open_module();
            #endif

            #if TWFG_APP_POWERON_IGNORE_DEV
                app_task_switch_to(APP_POWERON_TASK);
            #endif       
            to_set_alm_timer = sys_timeout_add(NULL,to_set_alm,100);     
        }else{
            struct sys_event e;
            e.type = SYS_DEVICE_EVENT;
            e.arg  = (void *)DEVICE_EVENT_FROM_ALM;
            e.u.dev.event = DEVICE_EVENT_IN;
            e.u.dev.value = 0;
            sys_event_notify(&e);            
        }

    }
}

void alarm_ring_cnt_clear(void)
{
    g_alarm_ring_cnt = 0;
}
extern u8 rtc_ring_tone[];
extern u8 alarm_ring_flag_1;
extern u8 alarm_ring_flag_2;
extern u8 alarm_tone_vol_flag;

u8 alarm_number_flag = ALARM_NUMBER_NULL;
extern u8 alarm_power_on_flag;
u16 __alarm_ring_play_timer = 0;
u16 __alarm_ring_stop_timer = 0;
void alarm_stop(void)
{
    printf("ALARM_STOP !!!\n");
    if(tone_get_status()){
        tone_play_stop();
    }
    alarm_active_flag_set(0);
    alarm_ring_cnt_clear();
    // rtc_app_alarm_ring_play(0);
    if(alarm_power_on_flag == TRUE)
    {
        alarm_power_on_flag = FALSE;
        power_event_to_user(POWER_EVENT_POWER_LOW);
    }
    if(idle_powerlow_flag){
        idle_powerlow_flag = FALSE;
        power_event_to_user(POWER_EVENT_POWER_LOW);      
    }
    if(__alarm_ring_stop_timer){
        sys_timeout_del(__alarm_ring_stop_timer);
        __alarm_ring_stop_timer = 0;

    }
    alarm_number_flag = ALARM_NUMBER_NULL;	
}

void alarm_play_timer_del(void)
{
    if (g_ring_playing_timer) {
        sys_timeout_del(g_ring_playing_timer);
        g_ring_playing_timer = 0;
    }
}
extern void alarm_tone_play_start(u8 alarm_number);
static void  __alarm_ring_play(void *p)
{
    if (!tone_get_status()) {
            // if (!rtc_app_alarm_ring_play(1)) {
                if(alarm_number_flag == ALARM_NUMBER_1)
                {
                    alarm_tone_play_start(1);
                }else{
                    alarm_tone_play_start(2);
                }
            // }
    }else{
        if(__alarm_ring_play_timer){
        sys_timeout_del(__alarm_ring_play_timer);
        __alarm_ring_play_timer = 0;
        }
        __alarm_ring_play_timer = sys_timeout_add(NULL, __alarm_ring_play, 500);
    }
}

void alarm_ring_start()
{
    r_printf("alarm_ring_start\n");
    __alarm_ring_play(NULL);
    __alarm_ring_stop_timer = sys_timeout_add(NULL, alarm_stop, (240*1000));
}


extern void alarm_delete(u8 index);
extern void user_get_sys_time(struct sys_time *time);
extern T_ALARM alarm_tab[M_MAX_ALARM_NUMS];
static void user_add_alarm(u8 index)
{
    T_ALARM alarm = {0};
    struct sys_time set_time;
    user_get_sys_time(&set_time);
    set_time.min += ALARM_SLEEP_ADD_TIME;
    if(set_time.min>=60){
        set_time.min %= 60;
        set_time.hour++;
    }
    if(set_time.hour >= 24)
    {
        set_time.hour %= 24;
    }
    user_alarm_get_info(&alarm, index);
    alarm.time.hour = set_time.hour;
    alarm.time.min  = set_time.min;
    alarm.time.sec  = 0;
    alarm.sw = 1;
    alarm.index = index;
    alarm.mode  = E_ALARM_MODE_ONCE;
    user_alarm_delete(index);
    user_alarm_add(&alarm, index);
    r_printf("set_alarm_%d\n",index);
}

void alarm_event_handler(struct sys_event *event, void *priv)
{
    if(app_check_curr_task(APP_IDLE_TASK)&&(!user_of_idle_to_poweron_flag)){
        event->consumed = 1;//接管按键消息,app应用不会收到消息
        if(alarm_active_flag_get()){
            alarm_active_flag_set(0);
        }
        return;
    }
    u8 index = 0;
    switch (event->type) {
    case SYS_KEY_EVENT:
        if (alarm_active_flag_get()) {
            event->consumed = 1;//接管按键消息,app应用不会收到消息
#if KEY_SLEEP_ENABLE
            if(event->u.key.event == KEY_TO_ALARM_SLEEP)
            {
                g_printf("1alarm_event_handler alarm_number_flag = %d\n",alarm_number_flag);
                if(alarm_number_flag ==  ALARM_NUMBER_1)
                {
                    user_add_alarm(3);
                }else if(alarm_number_flag ==  ALARM_NUMBER_2)
                {
                    user_add_alarm(4);
                }else{
                    user_add_alarm(3);
                }
                alarm_stop();
            }else if(event->u.key.event == KEY_TO_ALARM_UNSLEEP){
                g_printf("2alarm_event_handler alarm_number_flag = %d\n",alarm_number_flag);
                if(alarm_number_flag ==  ALARM_NUMBER_1)
                {
                    user_alarm_delete(3);
                }else if(alarm_number_flag ==  ALARM_NUMBER_2)
                {
                    user_alarm_delete(4);
                }
                alarm_stop();
            }
#else
            alarm_stop();
#endif
        // r_printf("event->u.key.value = %d,event->u.key.event = %d,index = %d\n",event->u.key.value,event->u.key.event,index);
        }
        break;
    default:
        break;
    }
}

SYS_EVENT_HANDLER(SYS_KEY_EVENT, alarm_event_handler, 2);
extern void get_rtc_sys_time(struct sys_time *time);
extern void clear_rtc_pos(void);
int alarm_sys_event_handler(struct sys_event *event)
{
    struct application *cur;
    struct sys_time current_time;
    if ((u32)event->arg == DEVICE_EVENT_FROM_ALM) {
        if (event->u.dev.event == DEVICE_EVENT_IN) {
            printf("alarm_sys_event_handler\n");
            alarm_update_info_after_isr();
            r_printf("alarm_number_flag = %d\n",alarm_number_flag);
//           u8 app = app_get_curr_task();
            clear_rtc_pos();
            if((!app_check_curr_task(APP_IDLE_TASK))||(user_of_idle_to_poweron_flag)){
                alarm_ring_start();
            }
//           if (app == APP_RTC_TASK) {
//                return false;
//          }
            return true;
        }
    }

    return false;
}

#endif

