/*
 * @Description: 
 * @Author: HJY
 * @Date: 2022-03-24 23:08:17
 * @LastEditTime: 2022-03-26 19:31:41
 * @LastEditors: HJY
 */
#include "system/includes.h"
#include "log.h"
#include "xz_ble_type.h"
#include "xz_ble_port.h"
#include "xz_ble_app_demo.h"

#include "typedef.h"
#include "rtc/rtc_ui.h"
#include "le_trans_data.h"
#include "btstack/bluetooth.h"
#include "sys_time.h"
#include "key_event_deal.h"
#include "rtc/alarm.h"


xz_ble_auth_settings_t xz_ble_auth;
uint16_t call_dp_response_cb(DP_ID dp_id, uint8_t *data, uint16_t len);
uint8_t temp_vn_buff[XZ_NV_ERASE_MIN_SIZE];



void xiao_zhi_ble_dp_data_receive_deal(xiao_zhi_ble_cb_evt_param_t * event)
{
    XZ_BLE_PRINTF("%s\n",__func__);
    XZ_BLE_PRINTF("HJY %d,%d,%d\n",event->dp_id,event->dp_data[0],event->dp_len);
    call_dp_response_cb(event->dp_id,event->dp_data,event->dp_len);
}

#define     APP_IO_DEBUG_0(i,x)       {JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT &= ~BIT(x);}
#define     APP_IO_DEBUG_1(i,x)       {JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT |= BIT(x);}

/**
 * @description: Respond to app after receiving app data
 * @param  {*}
 * @return {*}
 * @param {DP_ID} dp_id
 * @param {uint8_t} acK_value
 */
int16_t xiao_zhi_send_ack(DP_ID dp_id,uint8_t ack_value)
{
    int ret = 0;
    uint8_t ack_buf[RESPOND_ACK_LEN];
    ack_buf[0] = DPID_RET_VAL;
    ack_buf[1] = DT_VALUE;
    ack_buf[2] = 0X00;
    ack_buf[3] = 0X02;
    ack_buf[4] = dp_id;
    ack_buf[5] = ack_value;
    ret = xiao_zhi_ble_dp_data_report(ack_buf,RESPOND_ACK_LEN);
    return ret; 
}

uint16_t dp_switch_machine_response(uint8_t *data, uint16_t len)
{
    int ret = 0;
    XZ_BLE_PRINTF("%s\n",__func__);
    if(data[0]){
        XZ_BLE_PRINTF("APP_IO_DEBUG_0\n");
        APP_IO_DEBUG_0(A,10);
    }else{
        XZ_BLE_PRINTF("APP_IO_DEBUG_1\n");
        APP_IO_DEBUG_1(A,10);
    }
    return XZ_RES_OPT_SUCCESS;

    
}


uint16_t dp_switch_led_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;
    if(data[0]){    //open
        XZ_BLE_PRINTF("CMD_TO_OPEN_BRIGHT\n");
        app_task_put_key_msg(KEY_TO_OPEN_BRIGHT, 0);
    }else{
        XZ_BLE_PRINTF("CMD_TO_CLOSE_BRIGHT\n");
        app_task_put_key_msg(KEY_TO_CLOSE_BRIGHT, 0);  
    }
    return ret;
}

uint16_t dp_led_mode_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;
    XZ_BLE_PRINTF("CMD_TO_ADJUST_BRIGHT\n");
    app_task_put_key_msg(KEY_TO_ADJUST_BRIGHT, 0);    
    return ret;
}

uint16_t dp_scenc_data_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;

    if(data[0]){    //
        printf("CMD_CONTROL_BRIGHTEN_LIGHTING\n");  
        app_task_put_key_msg(KEY_TO_ADJUST_UP_BRIGHT, 0);  
    }else{
        XZ_BLE_PRINTF("KEY_TO_ADJUST_DOWN_BRIGHT\n");    
        app_task_put_key_msg(KEY_TO_ADJUST_DOWN_BRIGHT, 0);
    }   
    return ret;
}

static struct sys_time rtc_time_data;
static _xz_alarm_data alarm_data;
#define cmd_alarm_del(index)                app_alarm_del(index)//后面这个函数由客户自己自定义
#define cmd_alarm_add(type,time)            app_alarm_add(type,time)//do { } while(0)//后面这个函数由客户自己自定义
#define cmd_alarm_del_all()                 app_alarm_del_all()//后面这个函数由客户自己自定义
#define cmd_bluetooth_sync_time(time)       bluetooth_sync_time(time)//do { } while(0)//后面这个函数由客户自己自定义

uint16_t xz_timer_data_2_jl_struct(uint8_t *data, uint16_t len)
{

    memcpy(&alarm_data,data,len);
    rtc_time_data.year = xz_ble_ascii_to_int(alarm_data.year_buf,sizeof(alarm_data.year_buf));
    rtc_time_data.month = xz_ble_ascii_to_int(alarm_data.mouth_buf,sizeof(alarm_data.mouth_buf));
    rtc_time_data.day = xz_ble_ascii_to_int(alarm_data.day_buf,sizeof(alarm_data.day_buf));
    rtc_time_data.hour = xz_ble_ascii_to_int(alarm_data.hour_buf,sizeof(alarm_data.hour_buf));
    rtc_time_data.min = xz_ble_ascii_to_int(alarm_data.min_buf,sizeof(alarm_data.min_buf));
    rtc_time_data.sec = xz_ble_ascii_to_int(alarm_data.sec_buf,sizeof(alarm_data.sec_buf));
    g_printf("alarm_data.sec_buf %d,len:%d\n",rtc_time_data.sec,sizeof(alarm_data.sec_buf));
    put_buf(alarm_data.sec_buf,sizeof(alarm_data.sec_buf));
    XZ_BLE_PRINTF("timer:%d/%d/%d %d:%d:%d\n",rtc_time_data.year,
    rtc_time_data.month,rtc_time_data.day,rtc_time_data.hour,rtc_time_data.min ,rtc_time_data.sec);

    return XZ_RES_OPT_SUCCESS;
    
}


uint16_t dp_alarm_add_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;
    
    if(len != CMD_ALARM_ADD_PACKAGE_LEN){
        return XZ_RES_ERR_LENGTH;
    }

    printf("CMD_ALARM_ADD\n");
    put_buf(data,len);
    uint8_t alarm_index = xz_ble_ascii_to_int(&data[0],1);
    uint8_t alarm_type = xz_ble_ascii_to_int(&data[1],1);
    xz_timer_data_2_jl_struct(&data[2],len-2);
    cmd_alarm_add(alarm_type,&rtc_time_data);
    return ret;
}

uint16_t dp_alarm_del_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;
    uint8_t alarm_index = 0;
    
    printf("CMD_ALARM_DEL\n");
    cmd_alarm_del(data[0]);
    return ret;
}

uint16_t dp_alarm_del_all_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;
    printf("CMD_ALARM_DEL_ALL\n");
    cmd_alarm_del_all();
    return ret;
}

uint16_t dp_sync_timer_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;

    printf("CMD_BLUETOOTH_SYNC_TIME\n");
    xz_timer_data_2_jl_struct(data,len);
    cmd_bluetooth_sync_time(&rtc_time_data);
    return ret;
}

int16_t xz_unbound_opt(void)
{
    int ret = XZ_RES_OPT_SUCCESS;
    memset(&xz_ble_auth,0x00,sizeof(xz_ble_auth_settings_t));
    memcpy(temp_vn_buff,&xz_ble_auth,sizeof(xz_ble_auth_settings_t));
    xz_ble_nv_erase(XZ_BLE_AUTH_FLASH_ADDR,XZ_NV_ERASE_MIN_SIZE);
    xz_ble_nv_write(XZ_BLE_AUTH_FLASH_ADDR,temp_vn_buff,XZ_NV_ERASE_MIN_SIZE);
    return ret;
}

uint16_t dp_bound_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;

    printf("dp_bound_response\n");
    if(data[0]){
        if(xz_ble_auth.bound_flag){
            ret = XZ_RES_ERR_BOUND;
        }else{
            xz_ble_auth.bound_flag = 1;
            memcpy(xz_ble_auth.bound_info,&data[1],sizeof(xz_ble_auth.bound_flag));
            memcpy(temp_vn_buff,&xz_ble_auth,sizeof(xz_ble_auth_settings_t));
            xz_ble_nv_erase(XZ_BLE_AUTH_FLASH_ADDR,XZ_NV_ERASE_MIN_SIZE);
            xz_ble_nv_write(XZ_BLE_AUTH_FLASH_ADDR,temp_vn_buff,XZ_NV_ERASE_MIN_SIZE);
        }
    }else{
        if(!xz_ble_auth.bound_flag){
            ret = XZ_RES_ERR_BOUND;
        }else{
            ret = xz_unbound_opt();
        }
    }

    return ret;
}

uint16_t dp_bound_status_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;
    printf("dp_bound_response\n");
    ret = xz_ble_auth.bound_flag;
    return ret;
}
uint16_t dp_change_key_response(uint8_t *data, uint16_t len)
{
    int ret = XZ_RES_OPT_SUCCESS;
    printf("dp_change_key_response\n");
    return ret;
}

struct dp_data_income_do dp_data_response_tb[] = {
    {DPID_SWITCH_MACHINE, dp_switch_machine_response},
    {DPID_SWITCH_LED, dp_switch_led_response},
    {DPID_LIGHT_MODE, dp_led_mode_response},
    {DPID_SCENE_DATA_RAW, dp_scenc_data_response},
    {DPID_ALARM_ADD, dp_alarm_add_response},
    {DPID_ALARM_DEL, dp_alarm_del_response},
    {DPID_ALARM_DEL_ALL, dp_alarm_del_all_response},
    {DPID_SYNC_TIMER, dp_sync_timer_response},

    {DPID_CHANGE_KEY, dp_change_key_response},
    {DPID_BOUND_STATUS, dp_bound_status_response},
    {DPID_BOUND, dp_bound_response},

};

uint16_t call_dp_response_cb(DP_ID dp_id, uint8_t *data, uint16_t len)
{
    int ret = 0;
    for(uint8_t i = 0; i < ARRAY_SIZE(dp_data_response_tb); i++){
        if(dp_data_response_tb[i].dp_id == dp_id && dp_data_response_tb[i].response_cb){
            ret = dp_data_response_tb[i].response_cb(data, len);
            xiao_zhi_send_ack(dp_id,ret);
            return ret;
        }
    }

    if(dp_id >= DPID_MAX){
        xiao_zhi_send_ack(dp_id,XZ_RES_ERR_NOT_SUPPORTED);
    }else{
        xiao_zhi_send_ack(dp_id,XZ_RES_ERR_SET_FUNC_FAIL);
    }

    return 0;
}

void (*xiao_zhi_event_callback)(void *priv) = NULL;


void reg_xiao_zhi_event_callback_callback(void (*cb)(void *))
{
    xiao_zhi_event_callback = cb;
}


//涂鸦流程初始化
void xz_ble_app_init(void)
{
    int ret = 0;
    
    xz_ble_gap_addr_t gap_addr;

    
    XZ_BLE_PRINTF("%s",__func__);

    ret = xz_ble_nv_read(XZ_BLE_AUTH_FLASH_ADDR,temp_vn_buff,XZ_NV_ERASE_MIN_SIZE);     //读配置
    if(!ret){
        xz_ble_auth.mac_type = XZ_BLE_ADDRESS_TYPE_PUBLIC;
        xz_get_random_number(xz_ble_auth.mac,MAC_LEN);
        xz_ble_auth.bound_flag = 0;
        memcpy(temp_vn_buff,&xz_ble_auth,sizeof(xz_ble_auth));
        xz_ble_nv_erase(XZ_BLE_AUTH_FLASH_ADDR,XZ_NV_ERASE_MIN_SIZE);
        xz_ble_nv_write(XZ_BLE_AUTH_FLASH_ADDR,temp_vn_buff,XZ_NV_ERASE_MIN_SIZE);

    }else{
        memcpy(&xz_ble_auth,temp_vn_buff,sizeof(xz_ble_auth));
    }

    xiaozhi_enc_init();

    gap_addr.addr_type = xz_ble_auth.mac_type;
    memcpy(gap_addr.addr,xz_ble_auth.mac,MAC_LEN);

    xz_ble_gap_addr_set(&gap_addr);

    reg_xiao_zhi_event_callback_callback(xiao_zhi_ble_dp_data_receive_deal);
    // xiao_zhi_event_callback = xiao_zhi_ble_dp_data_receive_deal;


}
