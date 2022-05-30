/*
 * @Descripttion:
 * @version:
 * @Author: hu jian ye
 * @Date: 2021-12-01 19:48:25
 * @LastEditors: HJY
 * @LastEditTime: 2022-03-26 18:08:59
 */
#ifndef XZ_BLE_APP_DEMO_H_
#define XZ_BLE_APP_DEMO_H_


#ifdef __cplusplus
extern "C" {
#endif


#ifndef XZ_NV_START_ADDR
#define XZ_NV_START_ADDR              0
#endif
#ifndef XZ_NV_ERASE_MIN_SIZE
#define XZ_NV_ERASE_MIN_SIZE         (0x100)
#endif

#define	    XZ_BLE_AUTH_FLASH_ADDR             (XZ_NV_START_ADDR)
#define	    XZ_BLE_AUTH_FLASH_BACKUP_ADDR      (XZ_NV_START_ADDR+XZ_NV_ERASE_MIN_SIZE)

#define	    XZ_BLE_SYS_FLASH_ADDR              (XZ_NV_START_ADDR+XZ_NV_ERASE_MIN_SIZE*2)
#define	    XZ_BLE_SYS_FLASH_BACKUP_ADDR       (XZ_NV_START_ADDR+XZ_NV_ERASE_MIN_SIZE*3)

#define XZ_DATA_MAX_LEN     20
#define CMD_ALARM_ADD_PACKAGE_LEN               16

typedef enum {
    DPID_SWITCH_MACHINE = 1,    //开关机
    DPID_MUSIC_PP = 2,          //暂停播放
    DPID_FUNCTION_MODE = 3,     //功能模式
    DPID_VOL_SET = 4,           //音量
    DPID_REVERB_MIC = 6,        //麦克风混响
    DPID_PLAY_MODE = 7,         //播放模式
    DPID_QUIET = 8,             //静音
    DPID_SUBWOOFER = 9,          //重低音
    DPID_FAST_FORWARD = 10,      //快进
    DPID_FAST_BACKWARD = 11,     //快退
    DPID_AUTO_SCAN_STATION = 16, //自动搜索电台
    DPID_EQ_SWITCH = 20,         //EQ开关
    DPID_EQ_MODE = 21,           //eq模式
    DPID_EQ_DATA = 22,           //EQ参数
    DPID_SWITCH_LED = 23,        //灯光开关
    DPID_LIGHT_MODE = 24,        //灯光模式
    DPID_SCENE_DATA_RAW = 26,    //灯光参数
    DPID_BATTERY_PERCENTAGE = 27,//电池电量
    DPID_COUNT_DOWN = 28,        //倒计时
    DPID_MUSIC_LIGHT = 30,       //音乐灯
    DPID_BATTERY_STATUS = 31,    //电池状态
    DPID_MUSIC_PREV_TRACK = 32,  //上一曲
    DPID_MUSIC_NEXT_TRACK = 32,  //下一曲
    DPID_ALARM_ADD = 40,         //添加闹钟
    DPID_ALARM_DEL = 41,         //删除闹钟
    DPID_ALARM_DEL_ALL = 42,     //删除所有闹钟
    DPID_SYNC_TIMER = 43,        //同步时间

    DPID_MAX,
    DPID_SWITCH,                //开关(新界面不用)
    DPID_CHANGE_KEY = 0XF0,     //修改密钥
    DPID_BOUND = 0XF1,          //绑定
    DPID_BOUND_STATUS = 0XF2,   //绑定状态
    DPID_RET_VAL = 0XFF,        //返回值


}DP_ID;

struct dp_data_income_do {
    DP_ID dp_id;
    uint16_t (*response_cb)(uint8_t *data, uint16_t len);//相应dp id的处理，注：当app需要查询耳机数据时，data传null
};

enum {
    TONE_PLAY_INTERRUPT,//打断
    TONE_PLAY_DROP,     //叠加
    TONE_PLAY_OFF,      //关闭
};

typedef enum {
    DT_RAW,     //raw 类型；
    DT_BOOL,    //布尔类型；
    DT_VALUE,   //数值类型，其范围在 iot 平台注册时指定；
    DT_STRING,  //字符串类型；
    DT_ENUM,    //枚举类型；
    DT_BITMAP,  //位映射类型；
}DP_TYPE;

typedef enum {
    BT_DISCONNECT,
    BT_CONNECTING,
    BT_CONNECTED,
};

#pragma pack(1)//不平台对齐编译
typedef struct{
    uint8_t year_buf[4];
    uint8_t mouth_buf[2];
    uint8_t day_buf[2];
    uint8_t hour_buf[2];
    uint8_t min_buf[2];
    uint8_t sec_buf[2];
}_xz_alarm_data;
#pragma pack()


#ifdef __cplusplus
}
#endif

#endif //







