#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

/*
 *  板级配置选择
 */

//#define CONFIG_BOARD_AC695X_DEMO
// #define CONFIG_BOARD_AC6955F_HEADSET_MONO
// #define CONFIG_BOARD_AC6952E_LIGHTER
// #define CONFIG_BOARD_AC695X_CHARGING_BIN
// #define CONFIG_BOARD_AC695X_BTEMITTER
// #define CONFIG_BOARD_AC695X_TWS_BOX
// #define CONFIG_BOARD_AC695X_TWS
// #define CONFIG_BOARD_AC695X_MULTIMEDIA_CHARGING_BIN
// #define CONFIG_BOARD_AC695X_SOUNDCARD
// #define CONFIG_BOARD_AC6954A_DEMO
#define CONFIG_BOARD_AC695X_SMARTBOX
// #define CONFIG_BOARD_AC695X_LCD

// #define CONFIG_BOARD_AC6951G
// #define CONFIG_BOARD_AC6083A
// #define CONFIG_BOARD_AC6083A_IAP


#define DAC_OUTPUT_MONO_L                  0    //左声道
#define DAC_OUTPUT_MONO_R                  1    //右声道
#define DAC_OUTPUT_LR                      2    //立体声
#define DAC_OUTPUT_MONO_LR_DIFF            3    //单声道差分输出

#define DAC_OUTPUT_DUAL_LR_DIFF            6    //双声道差分
#define DAC_OUTPUT_FRONT_LR_REAR_L         7    //三声道单端输出 前L+前R+后L (不可设置vcmo公共端)
#define DAC_OUTPUT_FRONT_LR_REAR_R         8    //三声道单端输出 前L+前R+后R (可设置vcmo公共端)
#define DAC_OUTPUT_FRONT_LR_REAR_LR        9    //四声道单端输出(不可设置vcmo公共端)

#include "board_ac6954a_demo/board_ac6954a_demo_cfg.h"  //SPDIF-HDMI-IIS测试板
#include "board_ac695x_demo/board_ac695x_demo_cfg.h"
#include "board_ac6952e_lighter/board_ac6952e_lighter_cfg.h"       //蓝牙点烟器
#include "board_ac6955f_headset_mono/board_ac6955f_headset_mono_cfg.h" //蓝牙带卡耳机
#include "board_ac695x_charging_bin/board_ac695x_charging_bin.h"   //智能充电仓
#include "board_ac695x_btemitter/board_ac695x_btemitter.h"   //蓝牙发射器
#include "board_ac695x_tws_box/board_ac695x_tws_box.h"   //对箱
#include "board_ac695x_tws/board_ac695x_tws.h"   //纯对箱
#include "board_ac695x_multimedia_charging_bin/board_ac695x_multimedia_charging_bin.h"   //多媒体智能充电仓发射器
#include "board_ac695x_soundcard/board_ac695x_soundcard.h"   //声卡
#include "board_ac695x_smartbox/board_ac695x_smartbox.h"   //杰理之家智能音箱
#include "board_ac695x_lcd/board_ac695x_lcd_cfg.h"//

#include "board_ac6951g/board_ac6951g_cfg.h"

#include "board_ac6083a/board_ac6083a_cfg.h"
#include "board_ac6083a_iap/board_ac6083a_iap_cfg.h"


#define  DUT_AUDIO_DAC_LDO_VOLT   							DACVDD_LDO_2_90V

#define RTC_UI_WEEK_DISPLAY_ENABLE         1 //星期符号显示
#define USER_POWEROFF   1       //假关机
#define USER_SD_POWER_SWITCH_EN 0//SD pg 引脚为sd供电
#define USER_SD_POWER_IO IO_PORTB_11//与sd pg引脚绑定的引脚 没有选NO_CONFIG_PORT
#define UI_RTC_BATTERY_SET_ENABLE   0//剩余电量显示
#define LED7_BATTERY_ICON_ENABLE    0 //电量符号显示
#define LED7_KNOB_ENABLE            0 //外部音量调节背关
#define KEY_VOLUME_SET_ENABLE       1 //音量设置功能
#define LED7_PA_ENABLE				1 //AM PM符号显示
#define KEY_SLEEP_ENABLE            1 //贪睡模式
#define DISPLAY_BT_STRING_ENABLE    1   //显示蓝牙字符串

#define ALARM_SLEEP_ADD_TIME        10  //贪睡时间，单位分钟
#define AUDIO_VOLUME_INIT_FIXED         0  //开机音量是否固定
#define AUDIO_VOLUME_INIT_FIXED_VALUE   30 //固定音量等级
#define AUDIO_VOLUME_INIT_DEFAULT       25 //默认音量
#define RTC_SW_24_12H_ENABLE            1

#define GET_TEMPERATURE_ENABLE      1 //温度检测
#define GET_TEMPERATURE_PORT        IO_PORT_DP
#define GET_TEMPERATURE_PORT_CH     AD_CH_DP

#define USER_RGB_ENABLE                 0           
#define USER_RGB_PORT                   IO_PORT_DM

#define LIGHTING_ENABLE             0 //台灯
#define LIGHT_LEFT_PORT             IO_PORT_DP
#define LIGHT_RIGHT_PORT            IO_PORT_DM

#endif
