
#include "app_config.h"
#include "system/includes.h"
#include "asm/charge.h"
#include "app_power_manage.h"
#include "update.h"
#include "app_main.h"
#include "app_charge.h"
#include "chgbox_ctrl.h"
#include "update_loader_download.h"


extern void setup_arch();
extern int audio_dec_init();
extern int audio_enc_init();

static void do_initcall()
{
    __do_initcall(initcall);
}

static void do_early_initcall()
{
    __do_initcall(early_initcall);
}

static void do_late_initcall()
{
    __do_initcall(late_initcall);
}

static void do_platform_initcall()
{
    __do_initcall(platform_initcall);
}

static void do_module_initcall()
{
    __do_initcall(module_initcall);
}

void __attribute__((weak)) board_init()
{

}
void __attribute__((weak)) board_early_init()
{

}

int eSystemConfirmStopStatus(void)
{
    /* 系统进入在未来时间里，无任务超时唤醒，可根据用户选择系统停止，或者系统定时唤醒(100ms) */
    //1:Endless Sleep
    //0:100 ms wakeup
    if (get_charge_full_flag()) {
        log_i("Endless Sleep");
        power_set_soft_poweroff();
        return 1;
    } else {
        log_i("100 ms wakeup");
        return 0;
    }

}

static void check_power_on_key(void)
{
    u32 delay_10ms_cnt = 0;
    putchar('!');
    puts("xxxxx_lihongyuan\n");
    while (1) {
        clr_wdt();
        os_time_dly(1);

        extern u8 get_power_on_status(void);
        if (get_power_on_status()) {
            putchar('+');
            delay_10ms_cnt++;
            if (delay_10ms_cnt > 70) {
                return;
            }
        } else {
            putchar('-');
            delay_10ms_cnt = 0;
            puts("enter softpoweroff\n");
            power_set_soft_poweroff();
        }
    }
}

static void app_init()
{
    int update;
    putchar('0');
    do_early_initcall();
    do_platform_initcall();

    board_init();

    do_initcall();

    do_module_initcall();
    do_late_initcall();


    audio_enc_init();
    audio_dec_init();

    if (!UPDATE_SUPPORT_DEV_IS_NULL()) {
        update = update_result_deal();
    }

    app_var.play_poweron_tone = 1;

    if (!get_charge_online_flag()) {
        check_power_on_voltage();

#if TCFG_POWER_ON_NEED_KEY
        /*充电拔出,CPU软件复位, 不检测按键，直接开机*/
#if TCFG_CHARGE_OFF_POWERON_NE
        if ((!update && cpu_reset_by_soft()) || is_ldo5v_wakeup()) {
#else
        if (!update && cpu_reset_by_soft()) {
#endif
            app_var.play_poweron_tone = 0;
        } else {
            extern u8 alarm_active_flag_get(void);
            extern bool reset_of_8sec_flag;
            if((!alarm_active_flag_get())&&(!reset_of_8sec_flag))
            {
                check_power_on_key();
            }
        }
#endif
    }

#if (TCFG_MC_BIAS_AUTO_ADJUST == MC_BIAS_ADJUST_POWER_ON)
    extern u8 power_reset_src;
    u8 por_flag = 0;
    u8 cur_por_flag = 0;
    /*
     *1.update
     *2.power_on_reset(BIT0:上电复位)
     *3.pin reset(BIT4:长按复位)
     */
    if (update || (power_reset_src & BIT(0)) || (power_reset_src & BIT(4))) {
        //log_info("reset_flag:0x%x",power_reset_src);
        cur_por_flag = 0xA5;
    }
    int ret = syscfg_read(CFG_POR_FLAG, &por_flag, 1);
    if ((cur_por_flag == 0xA5) && (por_flag != cur_por_flag)) {
        //log_info("update POR flag");
        ret = syscfg_write(CFG_POR_FLAG, &cur_por_flag, 1);
    }
#endif

#if (TCFG_CHARGE_ENABLE && TCFG_CHARGE_POWERON_ENABLE)
    if (is_ldo5v_wakeup()) { //LDO5V唤醒
        extern u8 get_charge_online_flag(void);
        if (get_charge_online_flag()) { //关机时，充电插入

        } else { //关机时，充电拔出
            power_set_soft_poweroff();
        }
    }
#endif

#if(TCFG_CHARGE_BOX_ENABLE)
    /* clock_add_set(CHARGE_BOX_CLK); */
    chgbox_init_app();
#endif
}
extern extern void set_gpio_r_to_pwm(u32 gpio);
static void app_task_handler(void *p)
{
    set_gpio_r_to_pwm(IO_PORTC_05);
    set_gpio_r_to_pwm(IO_PORTC_04);
    set_gpio_r_to_pwm(IO_PORTC_03);
    set_gpio_r_to_pwm(IO_PORTC_02);
    set_gpio_r_to_pwm(IO_PORTA_10);
    set_gpio_r_to_pwm(IO_PORTA_09);
    set_gpio_r_to_pwm(IO_PORTA_01);
    set_gpio_r_to_pwm(IO_PORTA_00);
    app_init();
    app_main();
}

__attribute__((used)) int *__errno()
{
    static int err;
    return &err;
}

int main()
{
    gpio_set_pull_up(IO_PORTB_02, 0);
    gpio_set_pull_down(IO_PORTB_02, 0);
    gpio_set_direction(IO_PORTB_02, 1);
    gpio_set_die(IO_PORTB_02, 0);
    gpio_set_dieh(IO_PORTB_02, 0);

    gpio_set_pull_up(IO_PORTB_03, 0);
    gpio_set_pull_down(IO_PORTB_03, 0);
    gpio_set_direction(IO_PORTB_03, 1);
    gpio_set_die(IO_PORTB_03, 0);
    gpio_set_dieh(IO_PORTB_03, 0);

    gpio_set_pull_up(IO_PORT_PR_00, 0);
    gpio_set_pull_down(IO_PORT_PR_00, 0);
    gpio_set_direction(IO_PORT_PR_00, 1);
    gpio_set_die(IO_PORT_PR_00, 0);
    gpio_set_dieh(IO_PORT_PR_00, 0);

    gpio_set_pull_up(IO_PORT_PR_01, 0);
    gpio_set_pull_down(IO_PORT_PR_01, 0);
    gpio_set_direction(IO_PORT_PR_01, 1);
    gpio_set_die(IO_PORT_PR_01, 0);
    gpio_set_dieh(IO_PORT_PR_01, 0);
    wdt_close();

    os_init();

    setup_arch();

    board_early_init();

    task_create(app_task_handler, NULL, "app_core");

    os_start();

    local_irq_enable();

    while (1) {
        asm("idle");
    }

    return 0;
}

