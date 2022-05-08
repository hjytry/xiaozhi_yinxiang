#ifndef _XIAOYUN_AGREEMENT_H
#define _XIAOYUN_AGREEMENT_H
#include "typedef.h"
#include "rtc/rtc_ui.h"
#include "le_trans_data.h"
#include "btstack/bluetooth.h"
#include "sys_time.h"
#include "key_event_deal.h"
#include "rtc/alarm.h"
#pragma pack(1)//不平台对齐编译
typedef struct{
    u8 head;
    u8 cmd;
    u8 len;
    u8 index;
    u32 timer_h;
    u32 timer_l;
    u8  check;
}_xiaoyun_alarm_data;
#pragma pack()
#pragma pack(1)//不平台对齐编译
typedef struct{
    u8 head;
    u8 cmd;
    u8 len;
    u32 timer_h;
    u32 timer_l;
    u8  check;
}_xiaoyun_time_sync_data;
#pragma pack()
enum {
    APP_ALARM_MODE_ONCE            = 0x00,
    APP_ALARM_MODE_EVERY_DAY       = 0x7F,
    APP_ALARM_MODE_EVERY_MONDAY    = 0x01,
    APP_ALARM_MODE_EVERY_TUESDAY   = 0x02,
    APP_ALARM_MODE_EVERY_WEDNESDAY = 0x04,
    APP_ALARM_MODE_EVERY_THURSDAY  = 0x08,
    APP_ALARM_MODE_EVERY_FRIDAY    = 0x10,
    APP_ALARM_MODE_EVERY_SATURDAY  = 0x20,
    APP_ALARM_MODE_EVERY_SUNDAY    = 0x40,
};

#define U32_DIVIDE_1000 4294967
#define U32_REMAINDER_1000  296
#define SECOND_OF_DAY   86400

#define READ_BUFF_MAX   512
#define RESPOND_BUFF_MAX    6

#define HEAD_INDEX                              0
#define CMD_TYPE_INDEX                          1
#define DATA_LEN_INDEX                          2
#define DATA_INDEX                              3
#define CMD_INDEX                               3
#define CMD_ALARM_ADD_DATA_TYPE_INDEX           3
#define CMD_ALARM_DEL_DATA_TIME_NUMBER_INDEX    3
#define CMD_ALARM_ADD_DATA_TIME_STAMP_INDEX     4

#define HEAD                                0xA5
#define CMD_CONTROL                         0xBB //除了0xBB其他都不需要发送应答
#define CMD_ALARM_ADD                       0x20
#define CMD_ALARM_DEL                       0x21
#define CMD_ALARM_DEL_ALL                   0x22
#define CMD_WEATHER                         0xA0
#define CMD_BLUETOOTH_CONTROL               0xB0
#define CMD_BLUETOOTH_SYNC_TIME             0xC0
#define CMD_BLUETOOTH_PHONE                 0xD0

#define RESPOND_SUCCESS                     0x01 //应答成功
#define RESPOND_FAIL                        0x00 //应答失败


#define CMD_CONTROL_PACKAGE_LEN                 5
#define CMD_ALARM_ADD_PACKAGE_LEN               13
#define CMD_TIME_STAMP_LEN                      8
#define CMD_ALARM_DEL_PACKAGE_LEN               5
#define CMD_ALARM_DEL_ALL_PACKAGE_LEN           4
#define CMD_BLUETOOTH_SYNC_TIME_PACKAGE_LEN     12

#define CMD_CONTROL_OPEN_FAN_GEAR_1         0x11 //打开风扇一档
#define CMD_CONTROL_OPEN_FAN_GEAR_2         0x12 //打开风扇二档
#define CMD_CONTROL_OPEN_FAN_GEAR_3         0x13 //打开风扇三档
#define CMD_CONTROL_CLOSE_FAN               0x14 //关闭风扇
#define CMD_CONTROL_OPEN_SHAKE_HEAD         0x15 //打开摇头
#define CMD_CONTROL_CLOSE_SHAKE_HEAD        0x16 //关闭摇头
#define CMD_CONTROL_OPEN_LIGHTING           0x17 //打开灯光
#define CMD_CONTROL_SWITCH_LIGHTING         0x18 //切换灯光
#define CMD_CONTROL_CLOSE_LIGHTING          0x19 //关闭灯光
#define CMD_CONTROL_DARKEN_LIGHTING         0x1A //灯光变暗
#define CMD_CONTROL_BRIGHTEN_LIGHTING       0x1B //灯光变亮

#define CMD_ALARM_DEL_DATA_TIME_NUMBER_1    0x01 //闹钟1
#define CMD_ALARM_DEL_DATA_TIME_NUMBER_2    0x02 //闹钟2


#define CMD_BLUETOOTH_CONTROL_MUSIC_PREV    0x01  //上一曲
#define CMD_BLUETOOTH_CONTROL_MUSIC_NEXT    0x02  //下一曲
#define CMD_BLUETOOTH_CONTROL_VOL_UP        0x03  //音量加
#define CMD_BLUETOOTH_CONTROL_VOL_DOWN      0x04  //音量减
#define CMD_BLUETOOTH_CONTROL_MUSIC_PLAY    0x05  //播放
#define CMD_BLUETOOTH_CONTROL_MUSIC_SUSPEND 0x06  //暂停
#define CMD_BLUETOOTH_CONTROL_LAST_NO       0x07  //回拨电话

extern int xiaoyun_app_data(u8 *buf, u16 len);
#endif