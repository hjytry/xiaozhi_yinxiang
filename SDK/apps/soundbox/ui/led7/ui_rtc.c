#include "ui/ui_api.h"
#include "system/includes.h"
#include "system/sys_time.h"
#include "rtc/rtc_ui.h"

#include "rtc/alarm.h"

#include "btstack/avctp_user.h"
#include "app_task.h"
#include "ble_user.h"
#include "le_smartbox_module.h"
#include "app_main.h"
#if TCFG_APP_RTC_EN
#if (TCFG_UI_ENABLE&&(CONFIG_UI_STYLE == STYLE_JL_LED7))
extern void set_mode_icon(void *hd);
extern void user_led7_show_bt(void *hd);
extern void open_knob_icon(void *hd);
extern void set_battery_icon(void *hd);
static u8 am_pm_flag = AM_TIME;
extern u8 get_alm_num(void);
void ui_led7_show_rtc_icon(void *hd);
static u32 day_arr[] = {LED7_SUNDAY,LED7_MONDAY,LED7_TUESDAY,LED7_WEDNESDAY,LED7_THURSDAY,LED7_FRIDAY,LED7_SATURDAY};
static u8 week = 0;
extern u8 user_week_flag;
struct rtc_ui_opr {
    void *dev_handle;
    struct ui_rtc_display ui_rtc;
};

static struct rtc_ui_opr *__this = NULL;

void ui_rtc_temp_finsh(u8 menu)//子菜单被打断或者显示超时
{
    switch (menu) {
    default:
        break;
    }
}

void ui_led7_show_week(void *hd)
{
#if RTC_UI_WEEK_DISPLAY_ENABLE
    LCD_API *dis = (LCD_API *)hd;
    // r_printf("user_week_flag = %d\n",user_week_flag);
    if(user_week_flag == 0x55){
        dis->show_icon(day_arr[week]);
    }
#endif
}

static void ui_led7_show_year(void *hd, u16 Year)
{

    LCD_API *dis = (LCD_API *)hd;
    u8 tmp_buf[5] = {0};
    itoa4(Year, (u8 *)&tmp_buf[0]);
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string(tmp_buf);
    open_knob_icon(hd);
    dis->lock(0);
}

static void led7_show_rtc_string(void *hd, const char *buf)
{
    LCD_API *dis = (LCD_API *)hd;

    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string((u8 *)buf);
    open_knob_icon(hd);
    dis->lock(0);
}

static void led7_show_rtc_temperature(void *hd, const char *buf)
{
    LCD_API *dis = (LCD_API *)hd;

    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string((u8 *)buf);
    dis->show_icon(LED7_TEMP);
    set_mode_icon(hd);
    open_knob_icon(hd);
    set_battery_icon(hd);
    dis->lock(0);
}

static void led7_show_rtc_battery(void *hd, const char *buf)
{
#if UI_RTC_BATTERY_SET_ENABLE
    LCD_API *dis = (LCD_API *)hd;

    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string((u8 *)buf);
    dis->show_icon(LED7_BATTERY);
    set_mode_icon(hd);
    open_knob_icon(hd);
    dis->lock(0);
#endif
}


static void led7_flash_char(void *hd, u8 index)
{
    LCD_API *dis = (LCD_API *)hd;
    dis->FlashChar(BIT(index));
}

static void led7_rtc_flash_icon(void *hd, u32 icon)
{
    LCD_API *dis = (LCD_API *)hd;
    dis->flash_icon(icon);
}


static void ui_led7_show_curtime(void *hd, u8 Hour, u8 Min)
{
    LCD_API *dis = (LCD_API *)hd;
    u8 tmp_buf[5] = {0};
    extern u8 hour_flag;
    if(hour_flag == HOUR_12)
    {
        am_pm_flag =  ((Hour>12)||(Hour<=0))?PM_TIME:AM_TIME;
        Hour = (Hour%12)?(Hour%12):12;
    }
    itoa2(Hour, (u8 *)&tmp_buf[0]);
    itoa2(Min, (u8 *)&tmp_buf[2]);

    dis->lock(1);
    dis->setXY(0, 0);
    dis->clear_icon(0xffffffff);
    dis->Clear_FlashChar(BIT(0) | BIT(1) | BIT(2) | BIT(3));
    dis->show_string(tmp_buf);
    dis->flash_icon(LED7_2POINT);
    dis->flash_icon(LED7_DOT);
    ui_led7_show_rtc_icon(hd);
    ui_led7_show_week(hd);
    set_mode_icon(hd);
#if LED7_PA_ENABLE
    if(hour_flag == HOUR_12){
        if(am_pm_flag == PM_TIME)
        {
            dis->show_icon(LED7_PM);
        }else{
            dis->show_icon(LED7_AM);
        }
    }
#endif
    open_knob_icon(hd);
	set_battery_icon(hd);
    dis->lock(0);
}

static void ui_led7_show_RTC_time(void *hd, u8 Hour, u8 Min)
{
    LCD_API *dis = (LCD_API *)hd;
    u8 tmp_buf[5] = {0};
    r_printf("Hour = %d,Min = %d\n",Hour,Min);
    extern u8 hour_flag;
    if(hour_flag == HOUR_12)
    {
        am_pm_flag =  ((Hour>12)||(Hour<=0))?PM_TIME:AM_TIME;
        Hour = (Hour%12)?(Hour%12):12;
    }
    itoa2(Hour, (u8 *)&tmp_buf[0]);
    itoa2(Min, (u8 *)&tmp_buf[2]);

    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->clear_icon(0xffffffff);
    dis->show_string(tmp_buf);
    dis->show_icon(LED7_2POINT);
    dis->show_icon(LED7_DOT);
    ui_led7_show_week(hd);
    set_mode_icon(hd);
#if LED7_PA_ENABLE
    if(hour_flag == HOUR_12){
        if(am_pm_flag == PM_TIME)
        {
            dis->show_icon(LED7_PM);
        }else{
            dis->show_icon(LED7_AM);
        }
    }
#endif
    set_battery_icon(hd);
    open_knob_icon(hd);
    dis->lock(0);
}

static void ui_led7_show_date(void *hd, u16 Year, u8 Month, u8 Day)
{

    LCD_API *dis = (LCD_API *)hd;
    u8 tmp_buf[5] = {0};
    itoa2(Month, (u8 *)&tmp_buf[0]);
    itoa2(Day, (u8 *)&tmp_buf[2]);
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string(tmp_buf);
    ui_led7_show_week(hd);
    open_knob_icon(hd);
    dis->lock(0);
}

extern u8 alarm_number_flag;
void ui_led7_show_rtc_icon(void *hd)
{
    T_ALARM alarm = {0};
    LCD_API *dis = (LCD_API *)hd;
    if(alarm_number_flag == ALARM_NUMBER_NULL)
    {
        if (user_alarm_get_info(&alarm, 1) == 0) {
            // r_printf("alarm_get_info(&alarm, 1) == 0\n");
            // r_printf("alarm.sw == %d\n",alarm.sw);
            if(alarm.sw == TRUE)
            {
                dis->show_icon(LED7_ALARM1);
            }
        }
        if (user_alarm_get_info(&alarm, 2) == 0) {
            // r_printf("alarm_get_info(&alarm, 2) == 0\n");
            // r_printf("alarm.sw == %d\n",alarm.sw);
            if(alarm.sw == TRUE)
            {
                dis->show_icon(LED7_ALARM2);
            }
        }
    }else if(alarm_number_flag == ALARM_NUMBER_1){
        dis->flash_icon(LED7_ALARM1);
        if (user_alarm_get_info(&alarm, 2) == 0) {
            //r_printf("alarm_get_info(&alarm, 2) == 0\n");
            //r_printf("alarm.sw == %d\n",alarm.sw);
            if(alarm.sw == TRUE)
            {
                dis->show_icon(LED7_ALARM2);
            }
        }
    }else if(alarm_number_flag == ALARM_NUMBER_2){
        dis->flash_icon(LED7_ALARM2);
        if (user_alarm_get_info(&alarm, 1) == 0) {
            // r_printf("alarm_get_info(&alarm, 1) == 0\n");
            // r_printf("alarm.sw == %d\n",alarm.sw);
            if(alarm.sw == TRUE)
            {
                dis->show_icon(LED7_ALARM1);
            }
        }
    }
}


static void ui_led7_show_alarm_icon(void *hd,u8 flag)
{
    LCD_API *dis = (LCD_API *)hd;
    // r_printf("get_alm_num = %d,key_rtc_4_press_falg = %d\n",get_alm_num(),key_rtc_4_press_falg);
    if(get_alm_num() == 1)
    {
        if(flag){
            dis->flash_icon(LED7_ALARM1);
        }
         else{
            dis->show_icon(LED7_ALARM1);
        }
    }else if(get_alm_num() == 2)
    {
        if(flag){
            dis->flash_icon(LED7_ALARM2);
        }
        else{
            dis->show_icon(LED7_ALARM2);
        }
    }
}

static int ui_led7_show_RTC_user(void *hd, struct ui_rtc_display *rtc)
{
    int ret = true;
    if (rtc == NULL) {
        return false;
    }

    switch (rtc->rtc_menu) {

    case UI_RTC_ACTION_SHOW_TIME:
        ui_led7_show_RTC_time(hd,rtc->time.Hour, rtc->time.Min);
        ui_led7_show_rtc_icon(hd);
    break;
    case UI_ALARM_ACTION_SHOW_TIME:
        ui_led7_show_RTC_time(hd,rtc->time.Hour, rtc->time.Min);
        ui_led7_show_alarm_icon(hd,1);
    break;
    case UI_RTC_ACTION_SHOW_DATE:
        ui_led7_show_date(hd, rtc->time.Year, rtc->time.Month, rtc->time.Day);
        break;

    case UI_RTC_ACTION_YEAR_SET:
        ui_led7_show_year(hd, rtc->time.Year);
        led7_flash_char(hd, 0);
        led7_flash_char(hd, 1);
        led7_flash_char(hd, 2);
        led7_flash_char(hd, 3);
        ui_led7_show_rtc_icon(hd);
        break;

    case UI_RTC_ACTION_MONTH_SET:
        ui_led7_show_date(hd, rtc->time.Year, rtc->time.Month, rtc->time.Day);
        led7_flash_char(hd, 0);
        led7_flash_char(hd, 1);
        ui_led7_show_rtc_icon(hd);
        break;

    case UI_RTC_ACTION_DAY_SET:
        ui_led7_show_date(hd, rtc->time.Year, rtc->time.Month, rtc->time.Day);
        led7_flash_char(hd, 2);
        led7_flash_char(hd, 3);
        ui_led7_show_rtc_icon(hd);
        break;

    case UI_RTC_ACTION_HOUR_SET:
        ui_led7_show_RTC_time(hd, rtc->time.Hour, rtc->time.Min);
        led7_flash_char(hd, 0);
        led7_flash_char(hd, 1);
        ui_led7_show_rtc_icon(hd);
        break;

    case UI_RTC_ACTION_MINUTE_SET:
        ui_led7_show_RTC_time(hd, rtc->time.Hour, rtc->time.Min);
        led7_flash_char(hd, 2);
        led7_flash_char(hd, 3);
        ui_led7_show_rtc_icon(hd);
        break;
    case UI_ALARM_ACTION_HOUR_SET:
        ui_led7_show_RTC_time(hd, rtc->time.Hour, rtc->time.Min);
        led7_flash_char(hd, 0);
        led7_flash_char(hd, 1);
        ui_led7_show_alarm_icon(hd,1);
        break;

    case UI_ALARM_ACTION_MINUTE_SET:
        ui_led7_show_RTC_time(hd, rtc->time.Hour, rtc->time.Min);
        led7_flash_char(hd, 2);
        led7_flash_char(hd, 3);
        ui_led7_show_alarm_icon(hd,1);
        break;
    case UI_RTC_ACTION_STRING_SET:
        led7_show_rtc_string(hd, rtc->str);
        break;
    case UI_RTC_ACTION_WEEK_SET:
        led7_show_rtc_string(hd, rtc->str);
        led7_flash_char(hd, 0);
        led7_flash_char(hd, 1);
        led7_flash_char(hd, 2);
        led7_flash_char(hd, 3);
        break;
    case UI_RTC_TEMPERATURE_SET:
        led7_show_rtc_temperature(hd, rtc->str);
        ui_led7_show_week(hd);
        ui_led7_show_rtc_icon(hd);
        break;
    case UI_RTC_BATTERY_SET:
        led7_show_rtc_battery(hd, rtc->str);
        break;
    case UI_RTC_TONE_SET:
        led7_show_rtc_string(hd, rtc->str);
        set_mode_icon(hd);
        set_battery_icon(hd);
        led7_flash_char(hd, 1);
        led7_flash_char(hd, 2);
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}




struct ui_rtc_display *rtc_ui_get_display_buf()
{
    if (__this) {
        return &(__this->ui_rtc);
    } else {
        return NULL;
    }
}



static void *ui_open_rtc(void *hd)
{

    struct sys_time current_time;
    if (!__this) {
        __this =  zalloc(sizeof(struct rtc_ui_opr));
    }
    __this->dev_handle = dev_open("rtc", NULL);

    if (!__this->dev_handle) {
        free(__this);
        __this = NULL;
        return NULL;
    }

    ui_set_auto_reflash(500);//设置主页500ms自动刷新
    dev_ioctl(__this->dev_handle, IOCTL_GET_SYS_TIME, (u32)&current_time);
    printf("rtc_read_sys_time: %d-%d-%d %d:%d:%d\n",
           current_time.year,
           current_time.month,
           current_time.day,
           current_time.hour,
           current_time.min,
           current_time.sec);


    if (hd) {
        LCD_API *dis = (LCD_API *)hd;
        dis->lock(1);
        dis->clear();
        dis->setXY(0, 0);
        dis->lock(0);
    }
    return NULL;
}


static void ui_close_rtc(void *hd, void *private)
{
    LCD_API *dis = (LCD_API *)hd;
    if (!dis) {
        return;
    }
    if (__this) {
        free(__this);
        __this = NULL;
    }
}

static void ui_rtc_main(void *hd, void *private) //主界面显示
{
    static u8 cnt = 0;
    u16 sec_sum = 0;
    if (!hd) {
        return;
    }
    extern u8 get_bt_connect_status(void);
        
    struct sys_time current_time;
    dev_ioctl(__this->dev_handle, IOCTL_GET_SYS_TIME, (u32)&current_time);
    #if 1
        printf("rtc_read_sys_time: %d-%d-%d %d:%d:%d\n",
            current_time.year,
            current_time.month,
            current_time.day,
            current_time.hour,
            current_time.min,
            current_time.sec);
    #endif
    #if RTC_UI_WEEK_DISPLAY_ENABLE
        extern u8 rtc_calculate_week_val(struct sys_time *data_time);
        week = rtc_calculate_week_val(&current_time);
    #endif
    if((app_check_curr_task(APP_BT_TASK))&&((get_bt_connect_status() == BT_STATUS_WAITINT_CONN)&&(get_ble_work_state() != BLE_ST_NOTIFY_IDICATE))){
        // user_led7_show_bt(hd);
        extern void sys_auto_shut_down_enable(void);
        if(!app_var.auto_shut_down_timer){
            sys_auto_shut_down_enable();
        }
    }
    // else
    {
        if (app_check_curr_task(APP_BT_TASK)&&(get_ble_work_state() == BLE_ST_NOTIFY_IDICATE))
        {
            extern void sys_auto_shut_down_disable(void);
            if(app_var.auto_shut_down_timer){
                sys_auto_shut_down_disable();
            }
        }
        extern void set_rtc_sw_temperature(void);
        if((current_time.sec%12 >= 10)&&(++cnt <= 2))
        {
            cnt = 0;
            set_rtc_sw_temperature();
        }else{
            ui_led7_show_curtime(hd, current_time.hour, current_time.min);
        }
        // if((!(current_time.sec%5))&&(++cnt <= 2))
        // {
        //     cnt = 0;
        //     set_rtc_sw_temperature();
        // }else{
        //     ui_led7_show_curtime(hd, current_time.hour, current_time.min);
        // }
    }
}


static int ui_rtc_user(void *hd, void *private, u8 menu, u32 arg)//子界面显示 //返回true不继续传递 ，返回false由common统一处理
{
    int ret = true;
    LCD_API *dis = (LCD_API *)hd;
    if (!dis) {
        return false;
    }
    switch (menu) {

    case MENU_ALM_SET:
    case MENU_RTC_SET:
        ui_led7_show_RTC_user(hd, &(__this->ui_rtc));
        break;
    default:
        ret = false;
        break;
    }

    return ret;

}



const struct ui_dis_api rtc_main = {
    .ui      = UI_RTC_MENU_MAIN,
    .open    = ui_open_rtc,
    .ui_main = ui_rtc_main,
    .ui_user = ui_rtc_user,
    .close   = ui_close_rtc,
};


#endif
#endif
