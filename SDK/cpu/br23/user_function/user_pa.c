#include "user_pa.h"
#include "gpio.h"
#include "app_task.h"
#include "file_decoder.h"
#include "music_player.h"
bool pa_sys_automute;
bool pa_linein_mute;
void set_pa_sys_automute(bool mute)
{
	pa_sys_automute = mute;
}

bool get_pa_sys_automute(void)
{
	return pa_sys_automute;
}
void set_pa_linein_mute(bool mute)
{
	pa_sys_automute = mute;
}

bool get_pa_linein_mute(void)
{
	return pa_sys_automute;
}

void io_strl(u8 cmd)
{
    static bool mute_flag = PA_MUTE;
        switch (cmd) {
        case PA_MUTE:
            if(mute_flag != PA_MUTE)
            {
                r_printf("PA_MUTE\n");
                mute_flag = PA_MUTE;
                gpio_set_pull_up(MUTE_PIN, 0);
                gpio_set_pull_down(MUTE_PIN, 0);
                gpio_set_direction(MUTE_PIN, 0);
                gpio_direction_output(MUTE_PIN,1);
            }
            break;
        case PA_UMUTE:
            if(mute_flag != PA_UMUTE)
            {
                r_printf("PA_UMUTE\n");
                mute_flag = PA_UMUTE;
                gpio_set_pull_up(MUTE_PIN, 0);
                gpio_set_pull_down(MUTE_PIN, 0);
                gpio_set_direction(MUTE_PIN, 0);
                gpio_direction_output(MUTE_PIN,0);
            }
            break;
        case PA_CLASS_D:
            r_printf("PA_CLASS_D\n");
            gpio_set_pull_up(MODE_PIN, 1);
            gpio_set_pull_down(MODE_PIN, 0);
            gpio_set_direction(MODE_PIN, 0);
            gpio_direction_output(MODE_PIN,1);
            break;
        case PA_CLASS_AB:
            r_printf("PA_CLASS_AB\n");
            gpio_set_pull_up(MODE_PIN, 0);
            gpio_set_pull_down(MODE_PIN, 1);
            gpio_set_direction(MODE_PIN, 0);
            gpio_direction_output(MODE_PIN,0);
            break;
        case PA_INIT:
            gpio_set_pull_up(MODE_PIN, 0);
            gpio_set_pull_down(MODE_PIN, 0);
            gpio_set_direction(MODE_PIN, 1);
            gpio_direction_output(MODE_PIN,0);

            gpio_set_pull_up(MUTE_PIN, 0);
            gpio_set_pull_down(MUTE_PIN, 0);
            gpio_set_direction(MUTE_PIN, 0);
            gpio_direction_output(MUTE_PIN,1);
            break;
        default:
            break;
        }
}
#include "app_main.h"
extern APP_VAR app_var;
static u16 user_pa_timer = 0;
void user_pa_in_service(void){
    bool mute;

    //¡Á???mute
    mute = get_pa_sys_automute();

    //?¡Â??????????mute

    if (app_check_curr_task(APP_BT_TASK)){
        extern bool user_bt_dec_runing(void);
        if(user_bt_dec_runing())
        {
            mute = 0;
        }else{
            mute = 1;
        }

    }
    #if TCFG_APP_MUSIC_EN
    else if (app_check_curr_task(APP_MUSIC_TASK)){
        mute = (music_player_get_play_status() == FILE_DEC_STATUS_PLAY)?0:1;
    }
    #endif
	else if(app_check_curr_task(APP_LINEIN_TASK)){
        mute = get_pa_linein_mute();
    }
    else if(app_check_curr_task(APP_FM_TASK)){
        mute = 0;
    }
    else if(app_check_curr_task(APP_POWERON_TASK)){
        mute = 1;
    }

    /*******************************mute*******************************/
    //??????0
    if(!app_var.music_volume){
        // puts(">> L\n");
        mute = 1;
    }

    /*******************************umute*******************************/
    //?¨¢????
    extern int tone_get_status(void);
    if(tone_get_status()){
        mute = 0;
    }

    // r_printf(">>>>>>>>>>>  mute flag %d\n",pa_sys_auto_mute);
    if(mute){
        io_strl(PA_MUTE);
    }else{
        io_strl(PA_UMUTE);
    }

    user_pa_timer = sys_hi_timeout_add(NULL,user_pa_in_service,mute?100:1000);
}

void del_user_pa_in_service(void)
{
    if(user_pa_timer){
        sys_hi_timeout_del(user_pa_timer);
        user_pa_timer = 0;
        io_strl(PA_INIT);
    }
}

void add_user_pa_in_service(void)
{
    io_strl(PA_CLASS_D);
    io_strl(PA_MUTE);
    user_pa_in_service();
}

static u16 temp_r_arr[] = {
    18744,17584,16504,15497,14559,13683,12866,12103,11391,10724,10102,9519,8974,8463,7985,7537,7117,6722,6353,6006,5680,5374,5086,4815,4561,4322,4096,3884,3684,3496,\
    3318,\
    3151,2993,2844,2703,2570,2444,2325,2213,2107,2007,1912,1822,1736,1656,1579,1507,1438,1373,1311,1252,1196,1144,1093,1045,1000,957,916,877,839,804,\
    770,738,708,679,651,624,599,575,552,530,509,489,470,452,434,417,401,386,372,358,344,332,319,308,296,286,275,265,256,247,\
    238,230,222,214,207,199,193,186,180,174,168,180,174,168,162,157,152,147,142,137,133,129,125,121,117,113,110,106,103,100,97,94,91,\
    88,85,83,80,78,76,74,71,69,67,65,64,62,60,58,57,55,54,52,51,49,48,47,45,44,43,42,41,40,39,34,
};

extern void user_sd_set_power(u8 enable);

void temperature_init(void)
{
#if GET_TEMPERATURE_ENABLE||TCFG_APP_LINEIN_EN
    extern void user_sd_set_power(u8 enable);
    user_sd_set_power(1);
#endif
#if GET_TEMPERATURE_ENABLE
    adc_add_sample_ch(GET_TEMPERATURE_PORT_CH);
    gpio_set_die(GET_TEMPERATURE_PORT, 0);
    gpio_set_direction(GET_TEMPERATURE_PORT, 1);
    gpio_set_pull_down(GET_TEMPERATURE_PORT, 0);
    gpio_set_pull_up(GET_TEMPERATURE_PORT, 0);
#endif
}

int get_current_temperature(void)
{
#if GET_TEMPERATURE_ENABLE
    u32 temp_r = 0;
    int i = 0;
    u8 j = 0;
    u16 get_dac_val = 0;
    for(j = 0;j < 20;j++){
        get_dac_val = adc_get_value(AD_CH_DP);
        if(get_dac_val <= 0){continue;}
        temp_r += ((1000*get_dac_val)/(1024-get_dac_val));
        delay(100);
    }
    temp_r /= 20;
    r_printf("get_dac_val = %d,temp_r =%d\n",get_dac_val,temp_r);
    for(i = 0;i<155;i++)
    {
        if(temp_r>temp_r_arr[i])
        {
            r_printf("current temperature == %d\n",i-30);
            return (i-30);
            break;
        }
    }
#endif
    return (155-30);
}


void bule_no_connect_to_rtc(void *priv)
{
    app_task_switch_to(APP_POWERON_TASK);
}

static u8 rgb_flag = FALSE;

void rgb_on(void)
{
#if USER_RGB_ENABLE
    rgb_flag = TRUE;
    M_PA_IO_H_LEVEL(USER_RGB_PORT);
#endif
}

void rgb_off(void)
{
#if USER_RGB_ENABLE
    rgb_flag = FALSE;
    M_PA_IO_L_LEVEL(USER_RGB_PORT);
#endif
}

void rgb_init(void)
{
#if USER_RGB_ENABLE
    rgb_on();
#endif
}

void rgb_sw_light(void)
{
#if USER_RGB_ENABLE   
    if(rgb_flag == FALSE)
    {
        rgb_on();
    }else{
        rgb_off();
    }
#endif
}

static void set_gpio_high(u32 gpio)
{
#if LIGHTING_ENABLE
    gpio_set_pull_up(gpio, 0);
    gpio_set_pull_down(gpio, 0);
    gpio_set_direction(gpio, 0);
    gpio_direction_output(gpio,1);
#endif
}

static void set_gpio_low(u32 gpio)
{
#if LIGHTING_ENABLE
    gpio_set_pull_up(gpio, 0);
    gpio_set_pull_down(gpio, 0);
    gpio_set_direction(gpio, 0);
    gpio_direction_output(gpio,0);
#endif
}
static u8 lighting_mode = LIGHTING_CLOSE_ALL;

void set_lighting_mode(u8 mode)
{
#if LIGHTING_ENABLE
    switch (mode)
    {
    case LIGHTING_CLOSE_ALL:
    case LIGHTING_INIT:
        set_gpio_low(LIGHT_LEFT_PORT);
        set_gpio_low(LIGHT_RIGHT_PORT);
        break;
    case LIGHTING_LIGHT_ALL:
        set_gpio_high(LIGHT_LEFT_PORT);
        set_gpio_high(LIGHT_RIGHT_PORT);
        break;
    case LIGHTING_LIGHT_LEFT:
        set_gpio_high(LIGHT_LEFT_PORT);
        set_gpio_low(LIGHT_RIGHT_PORT);
        break;
    case LIGHTING_LIGHT_RIGHT:
        set_gpio_low(LIGHT_LEFT_PORT);
        set_gpio_high(LIGHT_RIGHT_PORT);
        break;
    default:
        break;
    }
#endif
}

void sw_lighting_mode(void)
{
#if LIGHTING_ENABLE
    if((lighting_mode < LIGHTING_LIGHT_ALL)&&(lighting_mode >= LIGHTING_MAX)){return;}
    set_lighting_mode(lighting_mode);
    lighting_mode = (lighting_mode >= (LIGHTING_MAX-1))?LIGHTING_LIGHT_ALL:lighting_mode+1;
#endif
}

