#include "includes.h"
#include "ui/ui_api.h"
#include "fm_emitter/fm_emitter_manage.h"
#include "btstack/avctp_user.h"
#if (TCFG_UI_ENABLE&&(CONFIG_UI_STYLE == STYLE_JL_LED7))

#include "app_task.h"
extern void ui_led7_show_rtc_icon(void *hd);
u8 get_bt_connect_status(void);
void set_mode_icon(void *hd)
{
    LCD_API *dis = (LCD_API *)hd;
    if (app_check_curr_task(APP_BT_TASK)){
        if(get_bt_connect_status() == BT_STATUS_WAITINT_CONN){
            dis->flash_icon(LED7_BT);
        }else{
            dis->show_icon(LED7_BT);
        }
    }else if (app_check_curr_task(APP_MUSIC_TASK)){
        dis->show_icon(LED7_SD);
    }
	else if(app_check_curr_task(APP_LINEIN_TASK)){
#if TCFG_APP_LINEIN_EN
        //dis->show_icon(LED7_AUX);
#endif
    }else if(app_check_curr_task(APP_FM_TASK))
    {
#if TCFG_APP_FM_EN
        dis->show_icon(LED7_FM);
#endif
    }
}

void open_knob_icon(void *hd)
{
#if LED7_KNOB_ENABLE
    LCD_API *dis = (LCD_API *)hd;
    dis->show_icon(LED7_KNOB_1|LED7_KNOB_2|LED7_KNOB_3|LED7_KNOB_4);
#endif
}

extern u8  get_cur_battery_level(void);
extern u16 user_bat_val;
//static u32 battery_icon_arr[] = {LED7_BATTERY_2,LED7_BATTERY_2,LED7_BATTERY_2};
void set_battery_icon(void *hd)
{
#if LED7_BATTERY_ICON_ENABLE
    static bool battery_once_flag = TRUE;
    static bool battery_light_flag = TRUE;    
    static u8 cnt_340v_mix = 0;
    static u8 cnt_340v_min = 0;
    LCD_API *dis = (LCD_API *)hd;
    //r_printf("get_cur_battery_level == %d\n",get_cur_battery_level());
    if(user_bat_val >= 340){
        cnt_340v_min = 0;
        if(battery_once_flag){
            battery_light_flag = TRUE;
            dis->show_icon(LED7_BATTERY_2);
        }else{
            if(battery_light_flag){
                dis->show_icon(LED7_BATTERY_2);                
            }else{
                if(cnt_340v_mix++ >= 10){
                    cnt_340v_mix = 0;
                    battery_light_flag = TRUE;
                    dis->show_icon(LED7_BATTERY_2);      
                }
            }
        }
    }else{
        cnt_340v_mix = 0;
        if(battery_once_flag){
            battery_light_flag = FALSE;
        }else{
            if(battery_light_flag){
                if(cnt_340v_min++ >= 10){
                    cnt_340v_min = 0;
                    battery_light_flag = FALSE;
                }else{
                    dis->show_icon(LED7_BATTERY_2);   
                }            
            }           
        }
        
    }
    battery_once_flag = FALSE;
    r_printf("battery_once_flag %d cnt_340v_mix %d cnt_340v_min %d user_bat_val %d",battery_once_flag,cnt_340v_mix,cnt_340v_min,user_bat_val);
#endif
}

static void led7_show_hi(void *hd)
{
    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_icon(0xffffffff);
    dis->show_string((u8 *)"8888");
    set_battery_icon(hd);
    dis->lock(0);
}

static void led7_show_poweroff(void *hd)
{
    r_printf("led7_show_poweroff\n");
    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->clear_icon(0xffffffff);
    dis->show_string((u8 *)" OFF");
    dis->lock(0);
}


static void led7_show_clear(void *hd)
{
    r_printf("led7_show_clear\n");
    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->clear_icon(0xffffffff);
    dis->lock(0);
}


static void led7_show_volume(void *hd, u8 vol)
{
    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_char(' ');
    dis->show_char('V');
    dis->show_number(vol / 10);
    dis->show_number(vol % 10);
    set_mode_icon(hd);
    ui_led7_show_rtc_icon(hd);
    open_knob_icon(hd);
    set_battery_icon(hd);
    dis->lock(0);
}

static void led7_show_wait(void *hd)
{
    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string((u8 *)" Lod");
    open_knob_icon(hd);
    dis->lock(0);
}

static void led7_show_music(void *hd)
{
    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string((u8 *)"nnp3");
    dis->show_icon(LED7_SD);
    open_knob_icon(hd);
    dis->lock(0);
}
extern void itoa4(u16 i, u8 *buf);
static void led7_show_filenumber(void *hd, u16 file_num)
{
    LCD_API *dis = (LCD_API *)hd;
    u8 bcd_number[5] = {0};	    ///<换算结果显示缓存
    itoa4(file_num, (u8 *)bcd_number);

    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    if (file_num > 999 && file_num <= 1999) {
        bcd_number[0] = '1';
    } else {
        bcd_number[0] = 'F';
    }
    dis->show_string(bcd_number);
    ui_led7_show_rtc_icon(hd);
    open_knob_icon(hd);
    set_mode_icon(hd);
    set_battery_icon(hd);
    dis->lock(0);
}
extern void ui_led7_show_week(void *hd);
void user_led7_show_bt(void *hd)
{
#if DISPLAY_BT_STRING_ENABLE
    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string((u8 *)"bLUE");
    set_mode_icon(hd);
    ui_led7_show_rtc_icon(hd);
    ui_led7_show_week(hd);
    open_knob_icon(hd);
    set_battery_icon(hd);
    dis->lock(0);
#endif
}

static void led7_show_aux(void *hd)
{

    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string((u8 *)"L1NE");
    ui_led7_show_rtc_icon(hd);
    //dis->show_icon(LED7_AUX);
    open_knob_icon(hd);
    set_battery_icon(hd);
    dis->lock(0);
}


#if TCFG_APP_FM_EMITTER_EN
static void led7_fm_ir_set_freq(void *hd, u16 freq)
{

    LCD_API *dis = (LCD_API *)hd;
    u8 bcd_number[5] = {0};	  ///<换算结果显示缓存
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    sprintf((char *)bcd_number, "%4d", freq);
    /* itoa4(freq,bcd_number); */
    if (freq > 1080) {
        dis->show_string((u8 *)" Err");
    } else if (freq >= 875) {
        dis->show_string(bcd_number);
        /* os_time_dly(100); */
        fm_emitter_manage_set_fre(freq);
        UI_REFLASH_WINDOW(TRUE);//设置回主页
    } else {
        dis->FlashChar(BIT(0) | BIT(1) | BIT(2) | BIT(3)); //设置闪烁
        dis->show_string(bcd_number);
    }
    dis->lock(0);

}
#endif

static void led7_fm_show_station(void *hd, u32 arg)
{
    u8 bcd_number[5] = {0};
    LCD_API *dis = (LCD_API *)hd;
    u16 freq = arg;

    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    sprintf((char *)bcd_number, "P %02d", arg);
    dis->show_string(bcd_number);
    set_mode_icon(hd);
    ui_led7_show_rtc_icon(hd);
    open_knob_icon(hd);
    set_battery_icon(hd);
    dis->lock(0);
}

static void led7_fm_set_freq(void *hd, u32 arg)
{
    u8 bcd_number[5] = {0};
    LCD_API *dis = (LCD_API *)hd;
    u16 freq = 0;
    freq = arg;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->FlashChar(BIT(0) | BIT(1) | BIT(2) | BIT(3)); //设置闪烁
    itoa4(freq, (u8 *)bcd_number);
    if (freq > 999 && freq <= 1999) {
        bcd_number[0] = '1';
    } else {
        bcd_number[0] = ' ';
    }
    dis->show_string(bcd_number);
    set_mode_icon(hd);
    ui_led7_show_rtc_icon(hd);
    open_knob_icon(hd);
    set_battery_icon(hd);
    dis->lock(0);
}

static void led7_fm_show_freq(void *hd, u32 arg)
{
    u8 bcd_number[5] = {0};
    LCD_API *dis = (LCD_API *)hd;
    u16 freq = arg;

    if (freq > 1080 && freq <= 1080 * 10) {
        freq = freq / 10;
    }
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    itoa4(freq, (u8 *)bcd_number);
    if (freq > 999 && freq <= 1999) {
        bcd_number[0] = '1';
    } else {
        bcd_number[0] = ' ';
    }
    dis->show_string(bcd_number);
    //dis->show_icon(LED7_DOT);
    set_mode_icon(hd);
    ui_led7_show_rtc_icon(hd);
    open_knob_icon(hd);
    set_battery_icon(hd);
    dis->lock(0);
}

static void led7_show_repeat_mode(void *hd, u32 val)
{
    if (!val) {
        return ;
    }
    u8 mode = (u8)val - 1;

    const u8 playmodestr[][5] = {
        " ALL",
        " ONE",
        "Fold",
        " rAn",
    };

    if (mode >= sizeof(playmodestr) / sizeof(playmodestr[0])) {
        printf("rpt mode display err !!\n");
        return ;
    }

    LCD_API *dis = (LCD_API *)hd;
    dis->lock(1);
    dis->clear();
    dis->setXY(0, 0);
    dis->show_string((u8 *)playmodestr[mode]);
    set_mode_icon(hd);
    open_knob_icon(hd);
    dis->lock(0);
}

void ui_common(void *hd, void *private, u8 menu, u32 arg)//公共显示
{

    u16 fre = 0;

    if (!hd) {
        return;
    }

    switch (menu) {
    case MENU_POWER_UP:
        led7_show_hi(hd);
        break;
    case MENU_MAIN_VOL:
        led7_show_volume(hd, arg & 0xff);
        break;
    case MENU_WAIT:
        led7_show_wait(hd);
        break;
    case MENU_MUSIC:
        led7_show_music(hd);
        break;
    case MENU_FILENUM:
        led7_show_filenumber(hd,arg);
        break;
    case MENU_AUX:
        led7_show_aux(hd);
        break;
    case MENU_BT:
        user_led7_show_bt(hd);
        break;
    case MENU_IR_FM_SET_FRE:
#if TCFG_APP_FM_EMITTER_EN
        led7_fm_ir_set_freq(hd, arg);
#endif
        break;
    case MENU_FM_SET_FRE:
    {
        extern u16 fm_manage_get_fre();
#if TCFG_APP_FM_EN
        u16 fre = fm_manage_get_fre();
        if (fre != 0) {
            led7_fm_show_freq(hd, fre);
        }
#endif
    }
// #if TCFG_APP_FM_EMITTER_EN
//         fre = fm_emitter_manage_get_fre();
//         led7_fm_set_freq(hd, arg);
// #endif
        break;
    case MENU_FM_STATION:
        led7_fm_show_station(hd, arg);
        break;
    case MENU_MUSIC_REPEATMODE:
        led7_show_repeat_mode(hd, arg);
        break;
    case MENU_POWER_OFF:
        led7_show_poweroff(hd);
		break;
    case MENU_CLEAR:
        led7_show_clear(hd);
        break;
    default:
        break;
    }
}

#endif
