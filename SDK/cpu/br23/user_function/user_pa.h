#ifndef USER_PA_H
#define USER_PA_H
#include "typedef.h"
#include "board_config.h"
#define MUTE_PIN    IO_PORTB_00
#define MODE_PIN    IO_PORTB_05
enum {
    PA_INIT,
    PA_POWER_OFF,
    PA_CLASS_AB,
    PA_CLASS_D,
    PA_MUTE,
    PA_UMUTE,
};

enum {
    LIGHTING_INIT,
    LIGHTING_LIGHT_ALL,
    LIGHTING_LIGHT_LEFT,
    LIGHTING_LIGHT_RIGHT,
    LIGHTING_CLOSE_ALL,
    LIGHTING_MAX,
};

#define M_PA_IO_H_LEVEL(GPIO)                   do{ \
                                                    gpio_set_direction(GPIO,0);\
                                                    gpio_set_pull_down(GPIO,0);\
                                                    gpio_set_pull_up(GPIO,0);\
                                                    gpio_set_die(GPIO,1);\
                                                    gpio_set_output_value(GPIO,1);\
                                                } while(0)

#define M_PA_IO_L_LEVEL(GPIO)                   do{ \
                                                    gpio_set_direction(GPIO,0);\
                                                    gpio_set_pull_down(GPIO,0);\
                                                    gpio_set_pull_up(GPIO,0);\
                                                    gpio_set_die(GPIO,1);\
                                                    gpio_set_output_value(GPIO,0);\
                                                } while(0)
#define M_PA_IO_R_LEVEL(GPIO)                   do{ \
                                                    gpio_set_direction(GPIO,1);\
                                                    gpio_set_pull_down(GPIO,0);\
                                                    gpio_set_pull_up(GPIO,0);\
                                                    gpio_set_die(GPIO,1);\
                                                    gpio_set_output_value(GPIO,0);\
                                                } while(0)
#endif