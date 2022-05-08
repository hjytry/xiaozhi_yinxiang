#include "app_config.h"
#include "key_event_deal.h"
#include "system/includes.h"
#include "tone_player.h"
#include "app_task.h"
#include "tone_player.h"
#include "media/includes.h"
#include "system/sys_time.h"
#include "ui/ui_api.h"
#include "rtc/alarm.h"
#include "rtc/rtc_ui.h"
#include "clock_cfg.h"
#include "ui/ui_style.h"
#include "ui_manage.h"

#include "syscfg_id.h"

/*************************************************************
   此文件函数主要是rtc模式按键处理和事件处理

**************************************************************/


#if TCFG_APP_RTC_EN



#define LOG_TAG_CONST       APP_RTC
#define LOG_TAG             "[APP_RTC]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



#define RTC_SET_MODE  0x55
#define ALM_SET_MODE  0xAA

#define RTC_POS_DEFAULT      RTC_POS_HOUR
#define RTC_ALM_POS_DEFAULT  ALM_POS_HOUR
#define RTC_MODE_DEFAULT     RTC_SET_MODE

#define MAX_YEAR          2099
#define MIN_YEAR          2000

enum {
    RTC_POS_NULL = 0,
    RTC_POS_YEAR,
    RTC_POS_MONTH,
    RTC_POS_DAY,
    RTC_POS_HOUR,
    RTC_POS_MIN,
    //RTC_POS_24_12HOUR,
    RTC_POS_WEEK,
    /* RTC_POS_SEC, */
    RTC_POS_MAX,
    ALM_POS_HOUR,
    ALM_POS_MIN,

    //ALM_POS_ENABLE,
    ALM_POS_RING_SW,
    ALM_POS_MAX,

};

struct rtc_opr {
    void *dev_handle;
    u8  rtc_set_mode;
    u8  rtc_pos;
    u8  alm_enable;
    u8  alm_num;
    struct sys_time set_time;
};

static struct rtc_opr *__this = NULL;



const char *alm_string[] =  {" AL ", " ON ", " OFF"};
const char *alm_select[] =  {"AL-1", "AL-2", "AL-3"};
// const char *alm_select[] =  {"AL-1", "AL-2", "AL-3", "AL-4", "AL-5"};
u8 hour_flag;
u16 rtc_ring_timer = 0;
u8 rtc_ring_tone[] = {IDEX_TONE_ALARM1,IDEX_TONE_ALARM2,IDEX_TONE_ALARM3,IDEX_TONE_ALARM4,IDEX_TONE_ALARM5};
u8 rtc_ring_index[] = {RTC_RING_1,RTC_RING_2,RTC_RING_3,RTC_RING_4,RTC_RING_5};
u8 alarm_ring_flag_1 = RTC_RING_1;
u8 alarm_ring_flag_2 = RTC_RING_5;
int alarm_tone_vol_1 = 0;
int alarm_tone_vol_2 = 0;
u8 alarm_tone_vol_flag = ALARM_NUMBER_NULL;
u8 key_rtc_8_press_falg = FALSE;
u8 key_rtc_7_press_falg = FALSE;
u8 key_rtc_6_press_falg = FALSE;
u8 check_alarm_timer_flag = FALSE;
u8 short_press_falg = FALSE;
// u8 alarm2_temp_sw_flag = FALSE;
static char str_buf[5] = {0};
u8 user_week = 0;
u8 user_week_flag = 0;
u8 user_alarm_set_vm_num = 0;
extern void alarm_delete(u8 index);
extern void itoa2(u8 i, u8 *buf);
extern void user_get_sys_time(struct sys_time *time);//获取时间
void get_rtc_sys_time(struct sys_time *time)//获取时间
{
    if (!__this->dev_handle) {
        return ;
    }
    dev_ioctl(__this->dev_handle, IOCTL_GET_SYS_TIME, (u32)time);
}

u8 get_alm_num(void)
{
    return __this->alm_num;
}

void clear_str_buf(void)
{
    for(u8 i = 0;i<5;i++){str_buf[i] = 0;}
}
void  alarm_tone_vol_to_str(u8 number,u8 *buf)
{
    buf[0] = ' ';
    buf[1] = 'V';
    itoa2(number,&buf[2]);
}

void  clear_rtc_pos(void)
{
    if (!__this) {
        return ;
    }
    __this->rtc_pos = RTC_POS_NULL;
    UI_REFLASH_WINDOW(true);
}

static void ui_set_rtc_timeout(u8 menu)
{
    struct sys_time current_time;
    if (!__this) {
        return ;
    }
    if((__this->rtc_set_mode == RTC_SET_MODE)&&(__this->rtc_pos != RTC_POS_NULL))
    {
#if RTC_UI_WEEK_DISPLAY_ENABLE
        u8 temp_week = 0;
        extern void rtc_calculate_next_few_day(struct sys_time *data_time, u8 days);
        extern u8 rtc_calculate_week_val(struct sys_time *data_time);
        user_get_sys_time(&current_time);
        temp_week = rtc_calculate_week_val(&current_time);
        temp_week = (temp_week <= 0)?7:temp_week;
        if(temp_week < user_week){
            rtc_calculate_next_few_day(&__this->set_time,user_week-temp_week);
        }else if(temp_week > user_week){
            rtc_calculate_next_few_day(&__this->set_time,(user_week-temp_week)+7);
        }
        r_printf("temp_week == %d,user_week == %d\n",temp_week,user_week);
        rtc_update_time_api(&__this->set_time);
        user_week_flag = 0x55;
        //r_printf("user_week_flag = %d\n",user_week_flag);
#endif
        syscfg_write(CFG_USER_RTC_WEEK, &user_week_flag, 1);
    }
    //else if(ALM_POS_ENABLE == __this->rtc_pos)
    //else if(ALM_POS_RING_SW == __this->rtc_pos)
    else if((__this->rtc_set_mode == ALM_SET_MODE)&&(__this->rtc_pos != RTC_POS_NULL))
    {
        T_ALARM temp_alarm = {0};
        T_ALARM alarm = {0};
        alarm.time.hour = __this->set_time.hour;
        alarm.time.min  = __this->set_time.min;
        alarm.time.sec  = 0;
        alarm.sw = 1;
        alarm.index = __this->alm_num;
        alarm.mode  = E_ALARM_MODE_EVERY_DAY;
        if(__this->alm_num == 1){
            user_alarm_delete(3);            
        }else if(__this->alm_num == 2){
            user_alarm_delete(4);    
        }
        user_alarm_delete(__this->alm_num);

        user_alarm_add(&alarm, __this->alm_num);
        if(__this->alm_num == 1)
        {
            syscfg_write(CFG_USER_ALARM_RING_SW_1, &alarm_ring_flag_1, 1);
        }else{
            syscfg_write(CFG_USER_ALARM_RING_SW_2, &alarm_ring_flag_2, 1);
        }
    }
    r_printf("ui_set_rtc_timeout rtc_ring_timer_del\n");
    extern void rtc_ring_timer_del(void);
    rtc_ring_timer_del();
    __this->rtc_pos = RTC_POS_NULL;
    // extern u8 alarm2_temp_sw_flag;
    // alarm2_temp_sw_flag = FALSE;
    key_rtc_8_press_falg = FALSE;
    key_rtc_7_press_falg = FALSE;
    key_rtc_6_press_falg = FALSE;
    check_alarm_timer_flag = FALSE;
    // __this->rtc_set_mode =  RTC_SET_MODE;
    // __this->alm_num = 0;
}

static void ui_check_mode_rtc_timeout(u8 menu)
{
    if (!__this) {
        return ;
    }
    // extern u8 alarm2_temp_sw_flag;
    // alarm2_temp_sw_flag = FALSE;
    check_alarm_timer_flag = FALSE;
}
void set_alarm_tone_vol(bool flag)
{
    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();
    if (!rtc) {
        return;
    }
    if(__this->alm_num == 1){
        syscfg_read(CFG_USER_ALARM_TONE_VOL_1, &alarm_tone_vol_1, 1);
        if((alarm_tone_vol_1 < 1)||(alarm_tone_vol_1 >30))
        {
            alarm_tone_vol_1 = TONE_MODE_DEFAULE_VOLUME;
        }else{
            if(flag){
                alarm_tone_vol_1 +=1;
                alarm_tone_vol_1 = (alarm_tone_vol_1 >=30)?30:alarm_tone_vol_1;
            }else{
                alarm_tone_vol_1 -=1;
                alarm_tone_vol_1 = (alarm_tone_vol_1 <= 1)?1:alarm_tone_vol_1;
            }
        }
        syscfg_write(CFG_USER_ALARM_TONE_VOL_1, &alarm_tone_vol_1, 1);
        clear_str_buf();
        alarm_tone_vol_to_str(alarm_tone_vol_1,str_buf);
        rtc->str = str_buf;
    }else{
        syscfg_read(CFG_USER_ALARM_TONE_VOL_2, &alarm_tone_vol_2, 1);
        if((alarm_tone_vol_2 < 1)||(alarm_tone_vol_2 >30))
        {
            alarm_tone_vol_2 = TONE_MODE_DEFAULE_VOLUME;
        }else{
            if(flag){
                alarm_tone_vol_2 +=5;
                alarm_tone_vol_2 = (alarm_tone_vol_2 >=30)?30:alarm_tone_vol_2;
            }else{
                alarm_tone_vol_2 -=5;
                alarm_tone_vol_2 = (alarm_tone_vol_2 <= 1)?1:alarm_tone_vol_2;
            }
        }
        syscfg_write(CFG_USER_ALARM_TONE_VOL_2, &alarm_tone_vol_2, 1);
        clear_str_buf();
        alarm_tone_vol_to_str(alarm_tone_vol_2,str_buf);
        r_printf("str_buf = %s\n",str_buf);
        rtc->str = str_buf;
    }
    rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
    UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
}

static void ui_temperature_rtc_timeout(u8 menu)
{
    if (!__this) {
        return ;
    }
}

static void flip_alarm_sw(u8 number)
{
    T_ALARM alarm = {0};

    if (!__this) {
        return ;
    }
    __this->rtc_pos = RTC_POS_NULL;
    user_alarm_get_info(&alarm, number);
    alarm.sw = !alarm.sw;
    r_printf("number %d sw %d\n",number,alarm.sw);
    user_alarm_delete(number);
    alarm.index = number;
    alarm.mode = E_ALARM_MODE_EVERY_DAY;
    user_alarm_add(&alarm, number);
}

struct ui_rtc_display *__attribute__((weak)) rtc_ui_get_display_buf()
{
    return NULL;
}



//*----------------------------------------------------------------------------*/
/**@brief    rtc 闹钟设置 \ 时间设置转换
   @param    无
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void set_rtc_sw()
{
    if ((!__this) || (!__this->dev_handle)) {
        return;
    }
    dev_ioctl(__this->dev_handle, IOCTL_GET_SYS_TIME, (u32)&__this->set_time);
    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();
    if (!rtc) {
        return;
    }
    rtc->time.Hour = __this->set_time.hour;
    rtc->time.Min = __this->set_time.min;
    if(__this->rtc_set_mode == RTC_SET_MODE)
    {
        __this->rtc_set_mode = ALM_SET_MODE;
    }
    __this->rtc_pos = RTC_POS_NULL;
    switch (__this->rtc_set_mode) {
    case RTC_SET_MODE:
        // __this->rtc_set_mode = ALM_SET_MODE;
        // __this->alm_num = 0;
        // rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
        // rtc->str = alm_select[__this->alm_num];
        // rtc->rtc_menu = UI_RTC_ACTION_SHOW_TIME;
        // UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
        break;

    case ALM_SET_MODE:
        __this->alm_num++;
        // __this->rtc_pos = RTC_POS_NULL;
        if (__this->alm_num >= sizeof(alm_select) / sizeof(alm_select[0])) {
            __this->rtc_set_mode = RTC_SET_MODE;
            __this->alm_num = 0;
            UI_REFLASH_WINDOW(true);
            break;
        }
        // rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
        // rtc->str = alm_select[__this->alm_num];
        rtc->rtc_menu = UI_RTC_ACTION_SHOW_TIME;
        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
        break;

    }
}

void user_display_rtc_mode(u8 cmd)
{
    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();
    T_ALARM alarm = {0};
    if (!rtc) {
        return;
    }

    switch (cmd) {
    case ALARM_NUMBER_1:
        user_alarm_get_info(&alarm, 1);
        rtc->time.Hour = alarm.time.hour;
        rtc->time.Min = alarm.time.min;
        __this->alm_num = 1;
        __this->rtc_set_mode = ALM_SET_MODE;
        rtc->rtc_menu = UI_ALARM_ACTION_SHOW_TIME;
        UI_SHOW_MENU(MENU_RTC_SET, 2*1000, 0, ui_temperature_rtc_timeout);
        break;
    case ALARM_NUMBER_2:
        user_alarm_get_info(&alarm, 2);
        rtc->time.Hour = alarm.time.hour;
        rtc->time.Min = alarm.time.min;
        __this->alm_num = 2;
        __this->rtc_set_mode = ALM_SET_MODE;
        rtc->rtc_menu = UI_ALARM_ACTION_SHOW_TIME;
        UI_SHOW_MENU(MENU_RTC_SET, 2*1000, 0, ui_temperature_rtc_timeout);
        break;

    }   
}

static void user_sw_rtc_mode(u8 cmd)
{
    T_ALARM alarm = {0};
    if ((!__this) || (!__this->dev_handle)) {
        return;
    }
    dev_ioctl(__this->dev_handle, IOCTL_GET_SYS_TIME, (u32)&__this->set_time);
    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();
    if (!rtc) {
        return;
    }
    __this->rtc_pos = RTC_POS_NULL;
    switch (cmd) {
    case USER_RTC_TIME:
        UI_REFLASH_WINDOW(true);
        break;
    case USER_ALARM_1:
        user_alarm_get_info(&alarm, 1);
        rtc->time.Hour = alarm.time.hour;
        rtc->time.Min = alarm.time.min;
        __this->alm_num = 1;
        __this->rtc_set_mode = ALM_SET_MODE;
        rtc->rtc_menu = UI_ALARM_ACTION_SHOW_TIME;
        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_check_mode_rtc_timeout);
        break;
    case USER_ALARM_2:
        user_alarm_get_info(&alarm, 2);
        rtc->time.Hour = alarm.time.hour;
        rtc->time.Min = alarm.time.min;
        __this->alm_num = 2;
        __this->rtc_set_mode = ALM_SET_MODE;
        rtc->rtc_menu = UI_ALARM_ACTION_SHOW_TIME;
        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_check_mode_rtc_timeout);
        break;

    }
}

extern u8 get_vbat_percent(void);
#if UI_RTC_BATTERY_SET_ENABLE
static void ui_battery_rtc_timeout(u8 menu)
{
    if (!__this) {
        return ;
    }
}
#endif

void set_rtc_battery(void)
{
#if UI_RTC_BATTERY_SET_ENABLE
    int temp = 0;
    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();
    if (!rtc) {
        return;
    }
    temp = get_vbat_percent();
    clear_str_buf();
    str_buf[0] = ' ';
    str_buf[1] = ' ';
    temp = (temp < 0)?(-temp):temp;
    itoa3(temp,&str_buf[1]);
    rtc->str = str_buf;
    rtc->rtc_menu = UI_RTC_BATTERY_SET;
    UI_SHOW_MENU(MENU_RTC_SET, 2 * 1000, 0, ui_battery_rtc_timeout);
#endif
}

void rtc_ring_timer_del(void)
{
    r_printf("rtc_ring_timer_del\n");
    if(tone_get_status()){
        tone_play_stop();
    }
    alarm_tone_vol_flag = ALARM_NUMBER_NULL;
    if(rtc_ring_timer){
        sys_timeout_del(rtc_ring_timer);
        rtc_ring_timer = 0;
    }
}

extern int get_current_temperature(void);
extern void itoa3(u16 i, u8 *buf);
void set_rtc_sw_temperature(void)
{
    r_printf("set_rtc_sw_temperature\n");
    int temp = 0;
    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();
    if (!rtc) {
        return;
    }
    temp = get_current_temperature();
    clear_str_buf();
    if(temp < 0){str_buf[0] = '-'; temp = -temp;}
    else{str_buf[0] = ' ';}
    itoa3(temp,&str_buf[1]);
    rtc->str = str_buf;
    rtc->rtc_menu = UI_RTC_TEMPERATURE_SET;
    UI_SHOW_MENU(MENU_RTC_SET, 2*1000, 0, ui_temperature_rtc_timeout);
}

static void alarm_tone_to_play_list(char *num, u32 *lst,u8 alarm_number)
{
    u8 i = 0;
    lst[i++] = (u32)TONE_REPEAT_BEGIN(-1);
    if(alarm_number == 1)
    {
        alarm_tone_vol_flag = ALARM_NUMBER_1;
        lst[i++] = (u32)(tone_table[IDEX_TONE_NONE]);
        lst[i++] = (u32)(tone_table[rtc_ring_tone[alarm_ring_flag_1-RTC_RING_1]]);
        lst[i++] = (u32)(tone_table[IDEX_TONE_NONE]);
    }else if(alarm_number == 2){
        alarm_tone_vol_flag = ALARM_NUMBER_2;
        lst[i++] = (u32)(tone_table[IDEX_TONE_NONE]);
        lst[i++] = (u32)(tone_table[rtc_ring_tone[alarm_ring_flag_2-RTC_RING_1]]);
        lst[i++] = (u32)(tone_table[IDEX_TONE_NONE]);
    }
    lst[i++] = (u32)TONE_REPEAT_END();
    lst[i++] = (u32)NULL;
}


static void poweroff_tone_to_play_list(u32 *lst)
{
    u8 i = 0;
    lst[i++] = (u32)(tone_table[IDEX_TONE_MAX_VOL]);
    lst[i++] = (u32)(tone_table[IDEX_TONE_MAX_VOL]);
    lst[i++] = (u32)(tone_table[IDEX_TONE_MAX_VOL]);
    lst[i++] = (u32)NULL;
}
void poweroff_tone_play_start(void)
{
    char *len_lst[34];
    poweroff_tone_to_play_list(len_lst);
    tone_file_list_play(len_lst, 1);
}


void alarm_tone_play_start(u8 alarm_number)
{
    char *len_lst[34];
    alarm_tone_to_play_list(NULL, len_lst,alarm_number);
    tone_file_list_play(len_lst, 1);
}

void rtc_ring_play(void)
{
    if(get_alm_num() == 1)
    {
        alarm_tone_play_start(1);
    }else if(get_alm_num() == 2)
    {
        alarm_tone_play_start(2);
    }
    if(rtc_ring_timer){
        sys_timeout_del(rtc_ring_timer);
        rtc_ring_timer = 0;
    }
    rtc_ring_timer = sys_timeout_add(NULL, rtc_ring_timer_del, (20*1000));
}

#if RTC_SW_24_12H_ENABLE
static void  rtc_sw_24_12h_timeout(u8 menu)
{
    if (!__this) {
        return ;
    }
}
#endif

void rtc_sw_24_12h(void)
{
#if RTC_SW_24_12H_ENABLE
    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();

    if (!rtc) {
        return;
    }

    syscfg_read(CFG_USER_DEFINE_12_24_HOUR, &hour_flag, 1);
    r_printf("XXX1 %d\n",hour_flag);
    if((hour_flag != HOUR_12)&&(hour_flag != HOUR_24))
    {
        hour_flag = HOUR_24;
        syscfg_write(CFG_USER_DEFINE_12_24_HOUR, &hour_flag, 1);
    }
    r_printf("XXX2 %d\n",hour_flag);
    rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
    if (hour_flag == HOUR_12) {
        hour_flag = HOUR_24;
        rtc->str = "24H ";
    } else {
        hour_flag = HOUR_12;
        rtc->str = "12H ";
    }
    r_printf("XXX3 %d\n",hour_flag);
    syscfg_write(CFG_USER_DEFINE_12_24_HOUR, &hour_flag, 1);
    UI_SHOW_MENU(MENU_RTC_SET, 2 * 1000, 0, rtc_sw_24_12h_timeout);
#endif
}


void  rtc_ring_sw(u8 number,u8 *buf)
{
    buf[0] = ' ';
    itoa2(number,&buf[1]);
}


void  rtc_week_sw(u8 number,u8 *buf)
{
    buf[0] = ' ';
    itoa2(number,&buf[1]);
}
//*----------------------------------------------------------------------------*/
/**@brief    rtc 设置调整时钟的位置
   @param    无
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void set_rtc_pos()
{
    T_ALARM alarm = {0};
    struct sys_time current_time;
    if ((!__this) || (!__this->dev_handle)) {
        return;
    }

    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();

    if (!rtc) {
        return;
    }

    switch (__this->rtc_set_mode) {
    case RTC_SET_MODE:

        if (__this->rtc_pos == RTC_POS_NULL) {
            __this->rtc_pos = RTC_POS_DEFAULT;
            dev_ioctl(__this->dev_handle, IOCTL_GET_SYS_TIME, (u32)&__this->set_time);
        } else {
            __this->rtc_pos++;
            if (__this->rtc_pos == RTC_POS_MAX) {
                __this->rtc_pos = RTC_POS_DEFAULT;
                // break;
            }
        }
        // if(RTC_POS_24_12HOUR == __this->rtc_pos){
        //     syscfg_read(CFG_USER_DEFINE_12_24_HOUR, &hour_flag, 1);
        //     if((hour_flag != HOUR_12)&&(hour_flag != HOUR_24))
        //     {
        //         hour_flag = HOUR_24;
        //         syscfg_write(CFG_USER_DEFINE_12_24_HOUR, &hour_flag, 1);
        //     }
        //     rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
        //     if (hour_flag == HOUR_12) {
        //         rtc->str = "12H ";
        //     } else {
        //         rtc->str = "24H ";
        //     }

        // }else
        if(RTC_POS_WEEK == __this->rtc_pos){
#if RTC_UI_WEEK_DISPLAY_ENABLE
            rtc->rtc_menu = UI_RTC_ACTION_WEEK_SET;
            extern u8 rtc_calculate_week_val(struct sys_time *data_time);
            user_get_sys_time(&current_time);
            user_week = rtc_calculate_week_val(&current_time);
            r_printf("user_week == %d\n",user_week);
            user_week = (user_week <= 0)?7:user_week;
            clear_str_buf();
            rtc_ring_sw(user_week,str_buf);
            rtc->str = str_buf;
#endif
        }else{
            rtc->rtc_menu = UI_RTC_ACTION_YEAR_SET + (__this->rtc_pos - RTC_POS_YEAR);

            rtc->time.Year = __this->set_time.year;
            rtc->time.Month = __this->set_time.month;
            rtc->time.Day = __this->set_time.day;
            rtc->time.Hour = __this->set_time.hour;
            rtc->time.Min = __this->set_time.min;
            rtc->time.Sec = __this->set_time.sec;
        }

        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
        break;

    case ALM_SET_MODE:
        if (__this->rtc_pos == RTC_POS_NULL) {
            __this->rtc_pos = RTC_ALM_POS_DEFAULT;
            if (user_alarm_get_info(&alarm, __this->alm_num) != 0) {
                log_error("alarm_get_info \n");
            }

            __this->set_time.hour = alarm.time.hour;
            __this->set_time.min = alarm.time.min;
            //__this->alm_enable = alarm.sw;

        } else {
            __this->rtc_pos++;
            if (__this->rtc_pos == ALM_POS_MAX) {
                __this->rtc_pos = RTC_ALM_POS_DEFAULT;
                // break;
            }
        }

        // if (ALM_POS_ENABLE == __this->rtc_pos) {
        //     rtc_ring_timer_del();
        //     if(__this->alm_num == 1)
        //     {
        //         syscfg_write(CFG_USER_ALARM_RING_SW_1, &alarm_ring_flag_1, 1);
        //     }else{
        //         syscfg_write(CFG_USER_ALARM_RING_SW_2, &alarm_ring_flag_1, 1);
        //     }

        //     rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
        //     if (__this->alm_enable) {
        //         rtc->str = " ON ";
        //     } else {
        //         rtc->str = " OFF";
        //     }
        // }else
        if(ALM_POS_RING_SW == __this->rtc_pos){
            rtc->rtc_menu = UI_RTC_TONE_SET;
            syscfg_read(CFG_USER_ALARM_RING_SW_1, &alarm_ring_flag_1, 1);
            if((alarm_ring_flag_1 < RTC_RING_1)||(alarm_ring_flag_1 > (RTC_RING_MAX-1)))
            {
                alarm_ring_flag_1 = RTC_RING_1;
                syscfg_write(CFG_USER_ALARM_RING_SW_1, &alarm_ring_flag_1, 1);
            }
            syscfg_read(CFG_USER_ALARM_RING_SW_2, &alarm_ring_flag_2, 1);
            if((alarm_ring_flag_2 < RTC_RING_1)||(alarm_ring_flag_2 > (RTC_RING_MAX-1)))
            {
                alarm_ring_flag_2 = RTC_RING_5;
                syscfg_write(CFG_USER_ALARM_RING_SW_2, &alarm_ring_flag_2, 1);
            }
            clear_str_buf();
            if(__this->alm_num == 1){
                rtc_ring_sw(alarm_ring_flag_1,str_buf);
            }else{
                rtc_ring_sw(alarm_ring_flag_2,str_buf);
            }
            rtc->str = str_buf;
            rtc_ring_timer_del();
            rtc_ring_play();
        }else {
            rtc_ring_timer_del();
            rtc->rtc_menu = UI_ALARM_ACTION_HOUR_SET + (__this->rtc_pos - ALM_POS_HOUR);
            rtc->time.Year = __this->set_time.year;
            rtc->time.Month = __this->set_time.month;
            rtc->time.Day = __this->set_time.day;
            rtc->time.Hour = __this->set_time.hour;
            rtc->time.Min = __this->set_time.min;
            rtc->time.Sec = __this->set_time.sec;
        }

        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
        break;

    }
}

//*----------------------------------------------------------------------------*/
/**@brief    rtc 调整时钟 加时间
   @param    无
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void set_rtc_up()
{

    if ((!__this) || (!__this->dev_handle)) {
        return;
    }

    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();

    if (!rtc) {
        return;
    }

    if (__this->rtc_pos == RTC_POS_NULL) {
        return ;
    }


    switch (__this->rtc_set_mode) {
    case RTC_SET_MODE:
        switch (__this->rtc_pos) {
        case RTC_POS_YEAR:
            __this->set_time.year++;
            if (__this->set_time.year > MAX_YEAR) {
                __this->set_time.year = MIN_YEAR;
            }
            break;
        case RTC_POS_MONTH:
            if (++__this->set_time.month > 12) {
                __this->set_time.month = 1;
            }
            break;
        case RTC_POS_DAY:
            if (++__this->set_time.day > month_for_day(__this->set_time.month, __this->set_time.year)) {
                __this->set_time.day = 1;
            }
            break;
        case RTC_POS_HOUR:
            if (++__this->set_time.hour >= 24) {
                __this->set_time.hour = 0;
            }
            break;

        case RTC_POS_MIN:
            if (++__this->set_time.min >= 60) {
                __this->set_time.min = 0;
            }
            break;
        // case RTC_POS_24_12HOUR:
        //     hour_flag = (HOUR_12==hour_flag)?HOUR_24:HOUR_12;
        //     break;
        case RTC_POS_WEEK:
#if RTC_UI_WEEK_DISPLAY_ENABLE
            user_week = (user_week >= 7)?1:user_week+1;
#endif
            break;
            /* case RTC_POS_SEC: */

            /* break; */
        }

        // if(RTC_POS_24_12HOUR == __this->rtc_pos){
        //     rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
        //     if (hour_flag == HOUR_12) {
        //         rtc->str = "12H ";
        //     } else {
        //         rtc->str = "24H ";
        //     }

        // }else
        if(RTC_POS_WEEK == __this->rtc_pos){
#if RTC_UI_WEEK_DISPLAY_ENABLE
            rtc->rtc_menu = UI_RTC_ACTION_WEEK_SET;
            clear_str_buf();
            rtc_ring_sw(user_week,str_buf);
            rtc->str = str_buf;
#endif
        }else{
            rtc->rtc_menu = UI_RTC_ACTION_YEAR_SET + (__this->rtc_pos - RTC_POS_YEAR);
            rtc->time.Year = __this->set_time.year;
            rtc->time.Month = __this->set_time.month;
            rtc->time.Day = __this->set_time.day;
            rtc->time.Hour = __this->set_time.hour;
            rtc->time.Min = __this->set_time.min;
            rtc->time.Sec = __this->set_time.sec;
        }
        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
        break;
    case ALM_SET_MODE:

        switch (__this->rtc_pos) {
        case ALM_POS_HOUR:
            if (++__this->set_time.hour >= 24) {
                __this->set_time.hour = 0;
            }
            break;

        case ALM_POS_MIN:
            if (++__this->set_time.min >= 60) {
                __this->set_time.min = 0;
            }
            break;
        case ALM_POS_RING_SW:
            if(__this->alm_num == 1)
            {
                alarm_ring_flag_1 = (alarm_ring_flag_1>=(RTC_RING_MAX-1))?RTC_RING_1:(alarm_ring_flag_1+1);
            }else{
                alarm_ring_flag_2 = (alarm_ring_flag_2>=(RTC_RING_MAX-1))?RTC_RING_1:(alarm_ring_flag_2+1);
            }
            break;
        // case ALM_POS_ENABLE:
        //     __this->alm_enable = !__this->alm_enable;
        //     break;
        }

        // if (ALM_POS_ENABLE == __this->rtc_pos) {
        //     rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
        //     if (__this->alm_enable) {
        //         rtc->str = " ON ";
        //     } else {
        //         rtc->str = " OFF";
        //     }
        // }else
        if(ALM_POS_RING_SW == __this->rtc_pos){
            rtc->rtc_menu = UI_RTC_TONE_SET;
            clear_str_buf();
            if(__this->alm_num == 1){
                rtc_ring_sw(alarm_ring_flag_1,str_buf);
            }else{
                rtc_ring_sw(alarm_ring_flag_2,str_buf);
            }
            rtc->str = str_buf;
            rtc_ring_timer_del();
            rtc_ring_play();
        }else {
            rtc->rtc_menu = UI_ALARM_ACTION_HOUR_SET + (__this->rtc_pos - ALM_POS_HOUR);
            rtc->time.Year = __this->set_time.year;
            rtc->time.Month = __this->set_time.month;
            rtc->time.Day = __this->set_time.day;
            rtc->time.Hour = __this->set_time.hour;
            rtc->time.Min = __this->set_time.min;
            rtc->time.Sec = __this->set_time.sec;
        }

        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
        break;

    default:
        break;
    }
}

//*----------------------------------------------------------------------------*/
/**@brief    rtc 调整时钟 减时间
   @param    无
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void set_rtc_down()
{

    if ((!__this) || (!__this->dev_handle)) {
        return;
    }
    struct ui_rtc_display *rtc = rtc_ui_get_display_buf();

    if (!rtc) {
        return;
    }

    if (__this->rtc_pos == RTC_POS_NULL) {
        return ;
    }

    switch (__this->rtc_set_mode) {
    case RTC_SET_MODE:
    {
        switch (__this->rtc_pos) {
        case RTC_POS_YEAR:
            __this->set_time.year--;
            if (__this->set_time.year < MIN_YEAR) {
                __this->set_time.year = MAX_YEAR;
            }
            break;
        case RTC_POS_MONTH:

            if (__this->set_time.month == 1) {
                __this->set_time.month = 12;
            } else {
                __this->set_time.month--;
            }

            break;
        case RTC_POS_DAY:

            if (__this->set_time.day == 1) {
                __this->set_time.day = month_for_day(__this->set_time.month, __this->set_time.year);
            } else {
                __this->set_time.day --;
            }

            break;
        case RTC_POS_HOUR:
            if (__this->set_time.hour == 0) {
                __this->set_time.hour = 23;
            } else {
                __this->set_time.hour--;
            }
            break;
        case RTC_POS_MIN:
            if (__this->set_time.min == 0) {
                __this->set_time.min = 59;
            } else {
                __this->set_time.min--;
            }
            break;
        // case RTC_POS_24_12HOUR:
        //     hour_flag = (HOUR_12==hour_flag)?HOUR_24:HOUR_12;
        //     break;
        case RTC_POS_WEEK:
#if RTC_UI_WEEK_DISPLAY_ENABLE
            user_week = (user_week <= 1 )?7:user_week-1;
#endif
            break;
        }
        // if(RTC_POS_24_12HOUR == __this->rtc_pos){
        //     rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
        //     if (hour_flag == HOUR_12) {
        //         rtc->str = "12H ";
        //     } else {
        //         rtc->str = "24H ";
        //     }

        // }else
        if(RTC_POS_WEEK == __this->rtc_pos){
            rtc->rtc_menu = UI_RTC_ACTION_WEEK_SET;
            clear_str_buf();
            rtc_ring_sw(user_week,str_buf);
            rtc->str = str_buf;
        }else{
            rtc->rtc_menu = UI_RTC_ACTION_YEAR_SET + (__this->rtc_pos - RTC_POS_YEAR);
            rtc->time.Year = __this->set_time.year;
            rtc->time.Month = __this->set_time.month;
            rtc->time.Day = __this->set_time.day;
            rtc->time.Hour = __this->set_time.hour;
            rtc->time.Min = __this->set_time.min;
            rtc->time.Sec = __this->set_time.sec;
        }
        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
    }
        break;

    case ALM_SET_MODE:
    {
        switch (__this->rtc_pos) {
        case ALM_POS_HOUR:
            if (__this->set_time.hour == 0) {
                __this->set_time.hour = 23;
            } else {
                __this->set_time.hour--;
            }
            break;

        case ALM_POS_MIN:
            if (__this->set_time.min == 0) {
                __this->set_time.min = 59;
            } else {
                __this->set_time.min--;
            }
            break;
        case ALM_POS_RING_SW:
            if(__this->alm_num == 1)
            {
                alarm_ring_flag_1 = (alarm_ring_flag_1<=RTC_RING_1)?(RTC_RING_MAX-1):(alarm_ring_flag_1-1);
            }else{
                alarm_ring_flag_2 = (alarm_ring_flag_2<=RTC_RING_1)?(RTC_RING_MAX-1):(alarm_ring_flag_2-1);
            }
            break;
        // case ALM_POS_ENABLE:
        //     __this->alm_enable = !__this->alm_enable;
        //     break;
        }

        // if (ALM_POS_ENABLE == __this->rtc_pos) {
        //     rtc->rtc_menu = UI_RTC_ACTION_STRING_SET;
        //     if (__this->alm_enable) {
        //         rtc->str = " ON ";
        //     } else {
        //         rtc->str = " OFF";
        //     }
        // }else
        if(ALM_POS_RING_SW == __this->rtc_pos){
            rtc->rtc_menu = UI_RTC_TONE_SET;
            clear_str_buf();
            if(__this->alm_num == 1){
                rtc_ring_sw(alarm_ring_flag_1,str_buf);
            }else{
                rtc_ring_sw(alarm_ring_flag_2,str_buf);
            }
            rtc->str = str_buf;
            rtc_ring_timer_del();
            rtc_ring_play();
        }else {
            rtc->rtc_menu = UI_ALARM_ACTION_HOUR_SET + (__this->rtc_pos - ALM_POS_HOUR);
            rtc->time.Year = __this->set_time.year;
            rtc->time.Month = __this->set_time.month;
            rtc->time.Day = __this->set_time.day;
            rtc->time.Hour = __this->set_time.hour;
            rtc->time.Min = __this->set_time.min;
            rtc->time.Sec = __this->set_time.sec;
        }
        UI_SHOW_MENU(MENU_RTC_SET, RTC_TIMEOUT_TIME, 0, ui_set_rtc_timeout);
    }
        break;

    default:

        break;
    }

}

static void rtc_app_init()
{
    if (!__this) {
        __this = zalloc(sizeof(struct rtc_opr));
        ASSERT(__this, "%s %di \n", __func__, __LINE__);
        __this->dev_handle = dev_open("rtc", NULL);
        if (!__this->dev_handle) {
            ASSERT(0, "%s %d \n", __func__, __LINE__);
        }
    }
    __this->rtc_set_mode =  RTC_SET_MODE;
    __this->rtc_pos = RTC_POS_NULL;

    // ui_update_status(STATUS_RTC_MODE);
    clock_idle(RTC_IDLE_CLOCK);
    // UI_SHOW_WINDOW(ID_WINDOW_CLOCK);
    // sys_key_event_enable();
    syscfg_read(CFG_USER_DEFINE_12_24_HOUR, &hour_flag, 1);
    if((hour_flag != HOUR_12)&&(hour_flag != HOUR_24))
    {
        hour_flag = HOUR_24;
        syscfg_write(CFG_USER_DEFINE_12_24_HOUR, &hour_flag, 1);
    }
}


//*----------------------------------------------------------------------------*/
/**@brief    rtc 退出
   @param    无
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void rtc_task_close()
{
#if SMART_BOX_EN
    extern void smartbox_rtc_mode_exit(void);
    smartbox_rtc_mode_exit();
#endif
    UI_HIDE_CURR_WINDOW();
    if (__this) {
        if (__this->dev_handle) {
            dev_close(__this->dev_handle);
            __this->dev_handle = NULL;
        }
        free(__this);
        __this = NULL;
    }
}


//*----------------------------------------------------------------------------*/
/**@brief    rtc 按键消息入口
   @param    无
   @return   1、消息已经处理，不需要发送到common  0、消息发送到common处理
   @note
*/
/*----------------------------------------------------------------------------*/
int rtc_key_event_opr(struct sys_event *event)
{
    int ret = true;
    int err = 0;
#if (TCFG_SPI_LCD_ENABLE)
    extern int key_is_ui_takeover();
    if (key_is_ui_takeover()) {
        return false;
    }
#endif
    if (!__this) {
        __this = zalloc(sizeof(struct rtc_opr));
        ASSERT(__this, "%s %di \n", __func__, __LINE__);
        __this->dev_handle = dev_open("rtc", NULL);
        if (!__this->dev_handle) {
            ASSERT(0, "%s %d \n", __func__, __LINE__);
        }
    }
    int key_event = event->u.key.event;
    int key_value = event->u.key.value;//
    static u8 temperature_flag = FALSE;
    log_info("key_event:%d \n", key_event);
    if (__this && __this->dev_handle) {
        switch (key_event) {
        case  KEY_RTC_UP:
            log_info("KEY_RTC_UP \n");
            set_rtc_up();
            break;

        case KEY_RTC_DOWN:
            log_info("KEY_RTC_DOWN \n");
            set_rtc_down();
            break;

        case KEY_RTC_SW:
            log_info("KEY_RTC_SW \n");
            set_rtc_sw();
            break;

        case KEY_RTC_SW_POS:
            log_info("KEY_RTC_SW_POS \n");
            set_rtc_pos();
            break;
        case KEY_RTC_0_SHORT:
            r_printf("KEY_RTC_0_SHORT\n");
            static u8 o_data[] = {0x18, 0x01, 0x00, 0x01, 0x00};
            
            extern unsigned char  xz_sent_encode_data(unsigned char *data,unsigned char len);
            xz_sent_encode_data(o_data,sizeof(o_data));
            for(int i=0;i<sizeof(o_data);i++){
                printf("data[%d]:%d\t",i,o_data[i]);
            }  
            printf("\n"); 
            break;
            if((key_rtc_8_press_falg == FALSE)&&(key_rtc_7_press_falg == FALSE)&&(key_rtc_6_press_falg == FALSE)){
                if(app_check_curr_task(APP_FM_TASK)){
                    app_task_put_key_msg(KEY_FM_SCAN_ALL_UP, 0);
                }else{
                    app_task_put_key_msg(KEY_MUSIC_PP, 0);
                }
            }else{
                // ui_set_rtc_timeout(UI_RTC_ACTION_STRING_SET);
                UI_REFLASH_WINDOW(true);
            }
            break;
        case KEY_RTC_1_SHORT:
            {
                r_printf("KEY_RTC_1_SHORT\n");
                if((key_rtc_8_press_falg == FALSE)&&(key_rtc_7_press_falg == FALSE)&&(key_rtc_6_press_falg == FALSE)){
                    if(app_check_curr_task(APP_FM_TASK)){
                        app_task_put_key_msg(KEY_FM_NEXT_STATION, 0);
                    }else{
                        app_task_put_key_msg(KEY_MUSIC_NEXT, 0);
                    }
                }else{
                    log_info("KEY_RTC_UP \n");
                    set_rtc_up();
                }
            }
            break;
        case KEY_RTC_1_LONG_PRESS:
        {
            r_printf("KEY_RTC_1_LONG_PRESS\n");
            if((key_rtc_8_press_falg == FALSE)&&(key_rtc_7_press_falg == FALSE)&&(key_rtc_6_press_falg == FALSE)&&(__this->rtc_pos == ALM_POS_RING_SW)){
                #if KEY_VOLUME_SET_ENABLE
                set_alarm_tone_vol(TRUE);
                #endif
            }
        }
            break;
        case KEY_RTC_1_HOLD_PRESS:
            {
                r_printf("KEY_RTC_1_HOLD_PRESS\n");
                if((key_rtc_8_press_falg == FALSE)&&(key_rtc_7_press_falg == FALSE)&&(key_rtc_6_press_falg == FALSE)){
                    #if KEY_VOLUME_SET_ENABLE
                    app_task_put_key_msg(KEY_VOL_UP, 0);
                    #endif
                }else{
                    if(__this->rtc_pos != ALM_POS_RING_SW)
                    {
                        log_info("KEY_RTC_UP \n");
                        set_rtc_up();
                    }else{
                        #if KEY_VOLUME_SET_ENABLE
                        set_alarm_tone_vol(TRUE);
                        #endif
                    }
                }
            }
            break;
        case KEY_RTC_2_SHORT:
            {
                r_printf("KEY_RTC_2_SHORT\n");
                if((key_rtc_8_press_falg == FALSE)&&(key_rtc_7_press_falg == FALSE)&&(key_rtc_6_press_falg == FALSE)){
                    if(app_check_curr_task(APP_FM_TASK)){
                        app_task_put_key_msg(KEY_FM_PREV_STATION, 0);
                    }else{
                        app_task_put_key_msg(KEY_MUSIC_PREV, 0);
                    }
                }else{
                    log_info("KEY_RTC_DOWN \n");
                    set_rtc_down();
                }
            }
            break;
        case KEY_RTC_2_LONG_PRESS:
        {
            r_printf("KEY_RTC_2_LONG_PRESS\n");
            if((key_rtc_8_press_falg == FALSE)&&(key_rtc_7_press_falg == FALSE)&&(key_rtc_6_press_falg == FALSE)&&(__this->rtc_pos == ALM_POS_RING_SW)){
                #if KEY_VOLUME_SET_ENABLE
                set_alarm_tone_vol(FALSE);
                #endif
            }
        }
        break;
        case KEY_RTC_2_HOLD_PRESS:
            {
                r_printf("KEY_RTC_2_HOLD_PRESS\n");
                if((key_rtc_8_press_falg == FALSE)&&(key_rtc_7_press_falg == FALSE)&&(key_rtc_6_press_falg == FALSE)){
                   #if KEY_VOLUME_SET_ENABLE
                   app_task_put_key_msg(KEY_VOL_DOWN, 0);
                   #endif
                }else{
                    if(__this->rtc_pos != ALM_POS_RING_SW)
                    {
                        log_info("KEY_RTC_DOWN \n");
                        set_rtc_down();
                    }else{
                        #if KEY_VOLUME_SET_ENABLE
                        set_alarm_tone_vol(FALSE);
                        #endif
                    }
                }
            }
            break;
        case KEY_RTC_3_SHORT:
            {
                r_printf("KEY_RTC_3_SHORT\n");
                if((key_rtc_8_press_falg == FALSE)&&(key_rtc_7_press_falg == FALSE)&&(key_rtc_6_press_falg == FALSE)){
                    app_task_put_key_msg(KEY_CHANGE_MODE, 0);
                }else{
                    log_info("SET_RTC_POS \n");
                    set_rtc_pos();
                }
            }
            break;
        case KEY_RTC_6_SHORT:
            {
                r_printf("KEY_RTC_6_SHORT\n");
                if(key_rtc_6_press_falg == TRUE){
                    set_rtc_pos();
                }else{
                    check_alarm_timer_flag = TRUE;
                    // if(alarm2_temp_sw_flag == FALSE)
                    // {
                    //     alarm2_temp_sw_flag = TRUE;
                        user_sw_rtc_mode(USER_ALARM_2);
                    // }else{
                    //     alarm2_temp_sw_flag = FALSE;
                    //     set_rtc_sw_temperature();
                    // }
                }
            }
            break;
        case KEY_RTC_6_LONG_PRESS:
            if(!check_alarm_timer_flag)
            {
                r_printf("KEY_RTC_6_LONG_PRESS\n");
                if(key_rtc_6_press_falg == FALSE){
                    key_rtc_6_press_falg = TRUE;
                    __this->alm_num = 2;
                    __this->rtc_set_mode = ALM_SET_MODE;
                    __this->rtc_pos = RTC_POS_NULL;
                    set_rtc_pos();
                }else{
                    key_rtc_6_press_falg = FALSE;
                    flip_alarm_sw(2);
                    UI_REFLASH_WINDOW(true);
                }
            }
            break;
        case KEY_RTC_7_SHORT:
            r_printf("KEY_RTC_7_SHORT\n");
            if(key_rtc_7_press_falg == TRUE){
                set_rtc_pos();
            }else{
                check_alarm_timer_flag = TRUE;
                user_sw_rtc_mode(USER_ALARM_1);
            }
            break;
        case KEY_RTC_7_LONG_PRESS:
            r_printf("KEY_RTC_7_LONG_PRESS\n");
            if(!check_alarm_timer_flag)
            {
                if(key_rtc_7_press_falg == FALSE){
                    key_rtc_7_press_falg = TRUE;
                    __this->alm_num = 1;
                    __this->rtc_set_mode = ALM_SET_MODE;
                    __this->rtc_pos = RTC_POS_NULL;
                    set_rtc_pos();
                }else{
                    key_rtc_7_press_falg = FALSE;
                    flip_alarm_sw(1);
                    UI_REFLASH_WINDOW(true);
                }
            }
            break;
        case KEY_RTC_8_SHORT:
            {
                r_printf("KEY_RTC_8_SHORT\n");
                if(key_rtc_8_press_falg == TRUE){
                    set_rtc_pos();
                }else{
                    user_sw_rtc_mode(USER_RTC_TIME);
                }
            }
            break;
        case KEY_RTC_8_LONG_PRESS:
            if(!check_alarm_timer_flag)
            {
            r_printf("KEY_RTC_8_LONG_PRESS\n");
            if(key_rtc_8_press_falg == FALSE){
                key_rtc_8_press_falg = TRUE;
                __this->alm_num = 0;
                __this->rtc_set_mode = RTC_SET_MODE;
                __this->rtc_pos = RTC_POS_NULL;
                set_rtc_pos();
            }
            }
            break;
        case KEY_RTC_SW_24_12H:
            rtc_sw_24_12h();
            break;
        default :
            ret = false;
            break;
        }
    }
    return ret;
}


//*----------------------------------------------------------------------------*/
/**@brief    rtc 按键消息入口
   @param    无
   @return   1、消息已经处理，不需要发送到common  0、消息发送到common处理
   @note
*/
/*----------------------------------------------------------------------------*/
static int rtc_sys_event_handler(struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        return rtc_key_event_opr(event);
    case SYS_DEVICE_EVENT:
        return false;
    default:
        return false;
    }
    return false;
}


//*----------------------------------------------------------------------------*/
/**@brief   rtc 启动
   @param    无
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void rtc_task_start()
{
    struct sys_time current_time;
    rtc_app_init();
#if RTC_UI_WEEK_DISPLAY_ENABLE
    extern u8 rtc_calculate_week_val(struct sys_time *data_time);
    user_get_sys_time(&current_time);
    user_week = rtc_calculate_week_val(&current_time);
    user_week = (user_week <= 0)?7:user_week;
#endif
    // if (alarm_active_flag_get()) {
    //     alarm_ring_start();
    // }
}

//*----------------------------------------------------------------------------*/
/**@brief    rtc 模式提示音播放结束处理
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void  rtc_tone_play_end_callback(void *priv, int flag)
{
    u32 index = (u32)priv;

    if (APP_RTC_TASK != app_get_curr_task()) {
        log_error("tone callback task out \n");
        return;
    }

    switch (index) {
    case IDEX_TONE_RTC:
        ///提示音播放结束， 启动播放器播放
        break;
    default:
        break;
    }
}

//*----------------------------------------------------------------------------*/
/**@brief    rtc 主任务
   @param    无
   @return   无
   @note
*/
/*----------------------------------------------------------------------------*/
void app_rtc_task()
{
    int res;
    int msg[32];
#if (SMART_BOX_EN)
    extern u8 smartbox_rtc_ring_tone(void);
    if (smartbox_rtc_ring_tone()) {
        tone_play_with_callback_by_name(tone_table[IDEX_TONE_RTC], 1, rtc_tone_play_end_callback, (void *)IDEX_TONE_RTC);
    }
#else
    UI_SHOW_WINDOW(UI_RTC_MENU_MAIN);
    sys_key_event_enable();
    tone_play_with_callback_by_name(tone_table[IDEX_TONE_RTC], 1, rtc_tone_play_end_callback, (void *)IDEX_TONE_RTC);
#endif
    rtc_task_start();

    while (1) {
        app_task_get_msg(msg, ARRAY_SIZE(msg), 1);

        switch (msg[0]) {
        case APP_MSG_SYS_EVENT:
            if (rtc_sys_event_handler((struct sys_event *)(&msg[1])) == false) {
                app_default_event_deal((struct sys_event *)(&msg[1]));    //由common统一处理
            }
            break;
        default:
            break;
        }

        if (app_task_exitting()) {
            rtc_task_close();
            return;
        }
    }
}

#else


void app_rtc_task()
{

}

#endif




