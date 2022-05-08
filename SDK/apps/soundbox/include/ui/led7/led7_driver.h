#ifndef	_LED7_DRV_H_
#define _LED7_DRV_H_


typedef struct _led7_VAR {
    u8  bCoordinateX;
    u8  bFlashChar;
    u8  bShowBuff[4]; //[0] ~ [3]显示的数字/字母显示的所有段
    u32 bShowIcon;    //每1bit对应LED4的1段, maybe icon > 7段
    u32 bFlashIcon;   //闪烁标志
    u8  bShowBuff1[9]; //[i]->第i个管脚, value的每一bit对应一个管脚, scan时使用
} LED7_VAR;

#define LED7_BT  		BIT(0) 		//0: 蓝牙图标
#define LED7_SD 		BIT(1) 		//1: Tf卡图标
#define LED7_TEMP   	BIT(2) 		//2: 温度图标
#define LED7_FM   		BIT(3) 		//3: FM图标
#define LED7_ALARM1 	BIT(4) 		//4: 闹钟1图标
#define LED7_ALARM2     BIT(5) 		//5: 闹钟2图标
#define LED7_AM 		BIT(6) 		//6: 早上图标
#define LED7_PM 		BIT(7) 		//7: 下午图标
#define LED7_MONDAY 	BIT(8) 		//8: 星期一图标
#define LED7_TUESDAY	BIT(9) 		//9: 星期二图标
#define LED7_WEDNESDAY 	BIT(10) 	//10: 星期三图标
#define LED7_THURSDAY 	BIT(11)  	//11: 星期四图标
#define LED7_FRIDAY		BIT(12)  	//11: 星期五图标
#define LED7_SATURDAY 	BIT(13)  	//11: 星期六图标
#define LED7_SUNDAY 	BIT(14)  	//11: 星期日图标
#define LED7_2POINT 	BIT(15) 	//12: 冒号图标
#define LED7_DOT 	    BIT(16) 	//12: 小数点图标
#define LED7_KNOB_1 	BIT(17) 	//12: 旋钮1
#define LED7_KNOB_2 	BIT(18) 	//12: 旋钮2
#define LED7_KNOB_3 	BIT(19) 	//12: 旋钮3
#define LED7_KNOB_4 	BIT(20) 	//12: 旋钮4
#define LED7_BATTERY 	BIT(21) 	//12: 电池

typedef u32 UI_LED7_ICON;

typedef enum  _led7_pin_type {
    LED7_PIN7, //7个引脚
    LED7_PIN12, //12个引脚
    LED7_PIN13, //13个引脚
} LED7_PIN_TYPE;

struct seg2pin {
    u8 pinH;
    u8 pinL;
};

struct icon_seg2pin {
    UI_LED7_ICON icon;
    struct seg2pin seg2pin;
};

struct led7_pin7 {
    u8 pin[8];
};
struct led7_pin12 {
    u8 pin_comh[5];
    u8 pin_segl[7];
};

struct led7_pin13 {
    u8 pin_com[6];
    u8 pin_seg[7];
};


union led7_pin_cfg {
    struct led7_pin7 pin7;
    struct led7_pin12 pin12;
    struct led7_pin13 pin13;
};

#define LED_A   BIT(0)
#define LED_B	BIT(1)
#define LED_C	BIT(2)
#define LED_D	BIT(3)
#define LED_E	BIT(4)
#define LED_F	BIT(5)
#define LED_G	BIT(6)
#define LED_H	BIT(7)


struct led7_platform_data {
    LED7_PIN_TYPE pin_type;
    union led7_pin_cfg pin_cfg;
};

#define LED7_PLATFORM_DATA_BEGIN(data) \
		const struct led7_platform_data data = {

#define LED7_PLATFORM_DATA_END() \
};

//UI LED7 API:
//=================================================================================//
//                        		模块初始化显示接口                    			   //
//=================================================================================//
void *led7_init(const struct led7_platform_data *_data);
const struct ui_display_api *ui_led7_init(void *para);

//=================================================================================//
//                        		设置显示坐标接口                    			   //
/*
  	   ___    ___    ___    ___
	  |___|  |___|  |___|  |___|
	  |___|  |___|  |___|  |___|
	 ---0------1------2------3------> X
*/
//=================================================================================//
void led7_setX(u8 X);

//=================================================================================//
//                        		字符类显示接口                    				   //
//=================================================================================//
void led7_show_char(u8 chardata); 			//显示字符(追加方式)
void led7_flash_char_start(u8 index);       //闪烁单个字符
void led7_flash_char_stop(u8 index);        //取消闪烁单个字符
void led7_show_string(u8 *str);   			//显示字符串(追加方式)
void led7_show_string_reset_x(u8 *str);  	//显示字符串, x从0开始
void led7_show_string_align_right(u8 *str); //led7显示字符串(清屏&右方式)
void led7_show_string_align_left(u8 *str);  //led7显示字符串(清屏&左方式)

//=================================================================================//
//                        		数字类显示接口                    				   //
//=================================================================================//
void led7_show_number(u16 val); //显示数字(清屏&高位显示0)
void led7_show_number2(u16 val); //显示数字(清屏&高位不显示)
void led7_show_number_add(u16 val); //显示数字(追加方式)

//=================================================================================//
//                        		图标类显示接口                    				   //
//=================================================================================//
void led7_show_icon(UI_LED7_ICON icon); //显示单个图标(追加)
void led7_flash_icon(UI_LED7_ICON icon); //闪烁单个图标(追加)

//=================================================================================//
//                        		模式类类显示接口                    			   //
//=================================================================================//

//=================================================================================//
//                        		清屏类显示接口                    				   //
//=================================================================================//
void led7_show_null(void); //清除所有显示(数字,字符串和图标)
void led7_clear_string(void); //清除显示数字和字母
void led7_clear_icon(UI_LED7_ICON icon); //清除显示单个图标
void led7_clear_all_icon(void); //清除显示所有图标
void led7_clear_show_icon(UI_LED7_ICON icon); //清除显示单个图标
void led7_clear_flash_icon(UI_LED7_ICON icon); //清除显示单个图标


#endif	/*	_LED_H_	*/
