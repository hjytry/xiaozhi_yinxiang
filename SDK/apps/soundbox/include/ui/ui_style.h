/*
 * @Author: your name
 * @Date: 2020-12-01 14:42:16
 * @LastEditTime: 2021-04-27 11:12:03
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \SDK_garbage\apps\soundbox\include\ui\ui_style.h
 */
#ifndef UI_STYLE_H
#define UI_STYLE_H

#include "ui/style_led7.h"//led7



#if(CONFIG_UI_STYLE == STYLE_JL_WTACH)
#include "ui/style_jl01.h"//彩屏//
#define ID_WINDOW_MAIN         PAGE_3
#define ID_WINDOW_BT           PAGE_0
#define ID_WINDOW_MUSIC        PAGE_1
#define ID_WINDOW_LINEIN       PAGE_1
// #define ID_WINDOW_FM           PAGE_2
#define ID_WINDOW_CLOCK        PAGE_0
// #define ID_WINDOW_BT_MENU      PAGE_3
#define ID_WINDOW_POWER_ON     PAGE_4
#define ID_WINDOW_POWER_OFF    PAGE_5
#define CONFIG_UI_STYLE_JL_ENABLE
#endif



#if(CONFIG_UI_STYLE == STYLE_JL_SOUNDBOX)
#include "ui/style_jl02.h"//点阵//
#define ID_WINDOW_MAIN     PAGE_0
#define ID_WINDOW_BT       PAGE_1
#define ID_WINDOW_FM       PAGE_2
#define ID_WINDOW_CLOCK    PAGE_3
#define ID_WINDOW_MUSIC    PAGE_4
#define ID_WINDOW_LINEIN   PAGE_0
#define ID_WINDOW_POWER_ON     PAGE_5
#define ID_WINDOW_POWER_OFF    PAGE_6
#define ID_WINDOW_SYS      PAGE_7
#define ID_WINDOW_PC         (-1)
#define ID_WINDOW_IDLE       (-1)
#define ID_WINDOW_REC        (-1)
#endif



#if(CONFIG_UI_STYLE == STYLE_JL_LED7)//led7 显示
#define ID_WINDOW_BT           UI_RTC_MENU_MAIN//UI_BT_MENU_MAIN
#define ID_WINDOW_FM           UI_RTC_MENU_MAIN
#define ID_WINDOW_CLOCK        UI_RTC_MENU_MAIN
#define ID_WINDOW_MUSIC        UI_RTC_MENU_MAIN//UI_MUSIC_MENU_MAIN
#define ID_WINDOW_LINEIN       UI_RTC_MENU_MAIN//UI_AUX_MENU_MAIN
#define ID_WINDOW_PC           UI_PC_MENU_MAIN
#define ID_WINDOW_REC          UI_RECORD_MENU_MAIN
#define ID_WINDOW_POWER_ON     UI_RTC_MENU_MAIN//UI_IDLE_MENU_MAIN
#define ID_WINDOW_POWER_OFF    UI_IDLE_MENU_MAIN
#define ID_WINDOW_SPDIF        UI_IDLE_MENU_MAIN
#define ID_WINDOW_IDLE         UI_RTC_MENU_MAIN
#endif


#endif
