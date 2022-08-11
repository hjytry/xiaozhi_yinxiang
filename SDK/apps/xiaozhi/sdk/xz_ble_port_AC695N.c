/**
 * \file tuya_ble_port.c
 *
 * \brief
 */
/*
 *  Copyright (C) 2014-2019, Tuya Inc., All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of tuya ble sdk
 */

#include "xz_ble_type.h"
#include "xz_ble_port.h"
#include "xz_ble_app_demo.h"

#include "system/includes.h"
#include "le_common.h"


extern __attribute__((weak)) u8 *get_norflash_uuid(void);
extern void get_random_number(u8 *ptr, u8 len);

xz_ble_status_t xz_get_uuid(uint8_t *uuid,uint8_t len)
{
    if(len > 16){
        return -1;
    }
    memcpy(uuid,get_norflash_uuid(),len);
    return 0;
}

xz_ble_status_t xz_get_random_number(uint8_t *buff,uint8_t len)
{
    get_random_number(buff,len);
    return 0;
}


/**
 *@brief
 *@param
 *
 *@note
 *
 * */
xz_ble_status_t xz_ble_gap_addr_get(xz_ble_gap_addr_t *p_addr)
{
    u8 ret;
    XZ_BLE_PRINTF("%s\n", __func__);
    if(p_addr->addr_type == XZ_BLE_ADDRESS_TYPE_PUBLIC){
        ret = le_controller_get_mac(p_addr->addr);
    }else if(p_addr->addr_type == XZ_BLE_ADDRESS_TYPE_RANDOM){
        ret = le_controller_get_random_mac(p_addr->addr);
    }
    return ret ? XZ_BLE_ERR_NOT_FOUND : XZ_BLE_SUCCESS;
}

/**
 *@brief
 *@param
 *
 *@note
 *
 * */
xz_ble_status_t xz_ble_gap_addr_set(xz_ble_gap_addr_t *p_addr)
{
	u8 ret;
    //ble_op_adv_enable(0);
    XZ_BLE_PRINTF("%s\n", __func__);
    XZ_BLE_PRINTF("tuya_ble_gap_addr_set=%d\n",p_addr->addr_type);
    XZ_BLE_HEXDUMP(p_addr->addr,6);
    ble_op_set_own_address_type(p_addr->addr_type);
    if(p_addr->addr_type == XZ_BLE_ADDRESS_TYPE_PUBLIC){
        ret = le_controller_set_mac(p_addr->addr);
    }else if(p_addr->addr_type == XZ_BLE_ADDRESS_TYPE_RANDOM){
        ret = le_controller_set_random_mac(p_addr->addr);
    }
    //ble_op_adv_enable(1);
    return ret ? XZ_BLE_ERR_INVALID_ADDR : XZ_BLE_SUCCESS;
}

extern const u8 *bt_get_mac_addr();
xz_ble_status_t xz_get_edr_mac(uint8_t *buff,uint8_t len)
{
    uint8_t temp_buf[6] = {0};

    XZ_BLE_PRINTF("%s\n", __func__);
    memcpy(temp_buf,bt_get_mac_addr(),6);
    
    XZ_BLE_HEXDUMP(temp_buf,6);
    for(int i=0;i<len;i++){
        buff[i] = temp_buf[len-i-1];
    }
    // memcpy(buff,temp_buf,len);
}



xz_ble_status_t xz_ble_nv_init(void)
{
    return XZ_BLE_SUCCESS;
}
/**
 *@brief
 *@param
 *
 *@note
 *
 * */
 xz_ble_status_t xz_ble_nv_erase(uint32_t addr,uint32_t size)
{
	return XZ_BLE_SUCCESS;
}

/**
 *@brief
 *@param
 *
 *@note
 *
 * */
 xz_ble_status_t xz_ble_nv_write(uint32_t addr,const uint8_t *p_data, uint32_t size)
{

    u8 index;
    switch(addr){
        case XZ_BLE_AUTH_FLASH_ADDR:
            index = CFG_USER_XZ_INFO_AUTH;
            break;
        case XZ_BLE_AUTH_FLASH_BACKUP_ADDR:
            index = CFG_USER_XZ_INFO_AUTH_BK;
            break;
        case XZ_BLE_SYS_FLASH_ADDR:
            index = CFG_USER_XZ_INFO_SYS;
            break;
        case XZ_BLE_SYS_FLASH_BACKUP_ADDR:
            index = CFG_USER_XZ_INFO_SYS_BK;
            break;

        default:
            return XZ_BLE_ERR_INVALID_ADDR;
    }
    if(syscfg_write(index, p_data, size) != size){
        return XZ_BLE_ERR_INVALID_ADDR;
    }

	return XZ_BLE_SUCCESS;
}
/**
 *@brief
 *@param
 *
 *@note
 *
 * */
 xz_ble_status_t xz_ble_nv_read(uint32_t addr,uint8_t *p_data, uint32_t size)
{
    u8 index;
    switch(addr){
        case XZ_BLE_AUTH_FLASH_ADDR:
            index = CFG_USER_XZ_INFO_AUTH;
            break;
        case XZ_BLE_AUTH_FLASH_BACKUP_ADDR:
            index = CFG_USER_XZ_INFO_AUTH_BK;
            break;
        case XZ_BLE_SYS_FLASH_ADDR:
            index = CFG_USER_XZ_INFO_SYS;
            break;
        case XZ_BLE_SYS_FLASH_BACKUP_ADDR:
            index = CFG_USER_XZ_INFO_SYS_BK;
            break;

        default:
            return XZ_BLE_ERR_INVALID_ADDR;
    }
    syscfg_read(index, p_data, size);
	return XZ_BLE_SUCCESS;
}

extern int ex_ble_dp_data_report(u8 *p_data,u32 len);
xz_ble_status_t xiao_zhi_ble_dp_data_report(uint8_t *p_data,uint32_t len)
{
    xz_sent_encode_data(p_data,len);
    return ex_ble_dp_data_report(p_data,len);
}








void xiao_zhi_send_even(void *prev)
{
    struct sys_event e;
    e.type = SYS_DEVICE_EVENT;
    e.arg = (void *)DEVICE_EVENT_XIAO_ZHI;
    e.u.dev.event = XZ_BLE_CB_EVT_DP_DATA_RECEIVED;
    e.u.dev.value = (int) prev;
    sys_event_notify(&e);
}
//------------------------------cbuf-----------------------------------//
#define XZ_TR_BUF_LEN      1024
static u8 xz_tr_cbuf[XZ_TR_BUF_LEN] __attribute__((aligned(4)));//缓存使用,可以扩大几倍
///cbuf缓存多包串口中断接收数据
cbuffer_t xz_cbuft;
#define cbuf_get_space(a) (a)->total_len
typedef struct {
    u8   xz_id;
    u32  rx_len;
}__attribute__((packed)) xz_head_t;

void xz_cbuft_init(void)
{
    cbuf_init(&xz_cbuft, xz_tr_cbuf, sizeof(xz_tr_cbuf));
}

static int xz_cbuf_is_write_able(u32 len)
{
    u32 wlen, buf_space;
    /* OS_ENTER_CRITICAL(); */
    /* cbuf_is_write_able() */
    buf_space = cbuf_get_space(&xz_cbuft) - cbuf_get_data_size(&xz_cbuft);
    if (buf_space < len) {
        wlen = 0;
    }else {
        wlen = len;
    }
    /* OS_EXIT_CRITICAL(); */
    XZ_BLE_PRINTF("%s[%d %d %d]", __func__, len, buf_space, wlen);
    return wlen;
}
int xz_ibuf_to_cbuf(u8 *buf, u32 len)
{
    int ret = 0;
    u16 wlen = 0;
    u16 head_size = sizeof(xz_head_t);
    xz_head_t rx_head;
    rx_head.xz_id = 1; //use uart1
    OS_ENTER_CRITICAL();
    if (xz_cbuf_is_write_able(len + head_size)) {
        rx_head.rx_len = len;
        wlen = cbuf_write(&xz_cbuft, &rx_head, head_size);
        wlen += cbuf_write(&xz_cbuft, buf, rx_head.rx_len);
        if (wlen != (rx_head.rx_len + head_size)) {
            ret = -1;
        } else {
            ret = wlen;
        }
    }else{
        ret = -2;
    }
    OS_EXIT_CRITICAL();
    log_e("%s[%d %d %d]", __func__, len, head_size, ret);
    return ret;
}
static int xz_cbuf_read(u8 *buf, u32 *len)
{
    int ret = 0;
    u16 rlen = 0;
    u16 head_size = sizeof(xz_head_t);
    xz_head_t rx_head;
    OS_ENTER_CRITICAL();
    if (0 == cbuf_get_data_size(&xz_cbuft)) {
        /* OS_EXIT_CRITICAL(); */
        ret =  -1;
    }else{
        rlen = cbuf_read(&xz_cbuft, &rx_head, sizeof(xz_head_t));
        if(rlen != sizeof(xz_head_t)){
            ret =  -2;
        }
        else if(rx_head.rx_len) {
            rlen = cbuf_read(&xz_cbuft, buf, rx_head.rx_len);
            if(rlen != rx_head.rx_len){
                ret =  -3;
            }else{
                *len = rx_head.rx_len;
            }
        }
    }
    OS_EXIT_CRITICAL();
    log_d("%s[%d 0x%x %d]", __func__, ret, len, rx_head.rx_len);
    return ret;
}

extern void (*xiao_zhi_event_callback)(void *priv);


#define DATA_HAED       4

extern int16_t xiao_zhi_send_ack(DP_ID dp_id,uint8_t ack_value);

//设备事件响应demo
static void xiao_zhi_event_handler(struct sys_event *e)
{
    int ret = 0;
    u32 rlen = 0;
    u8 receiced_buf[XZ_DATA_MAX_LEN];

    //struct xiao_zhi_trans_data *m_mic_data = xiao_zhi_receiced_buf;
    struct xiao_zhi_trans_data xz_data;
    ret = xz_cbuf_read(receiced_buf, &rlen);//栈变量取值可能会死机
    if(ret < 0) return;
    // memcpy(&xz_data,xiao_zhi_receiced_buf,sizeof(struct xiao_zhi_trans_data));
    //put_buf(xiao_zhi_receiced_buf,sizeof(xiao_zhi_receiced_buf));
    xz_data.dp_id = receiced_buf[0];
    xz_data.dp_type = receiced_buf[1];
    xz_data.dp_len = (receiced_buf[2]<<8) | receiced_buf[3];
    // xz_data.dp_data = &receiced_buf[4];
    if(rlen != (xz_data.dp_len + DATA_HAED)){
        xiao_zhi_send_ack(DPID_RET_VAL,4);
        return;
    }
    memcpy(xz_data.dp_data,&receiced_buf[4],xz_data.dp_len);
    // XZ_BLE_PRINTF("id:%d,type:%d,len:%d,data:%d\n",xz_data.dp_id,xz_data->dp_type,xz_data->dp_len,xz_data->dp_data[0]);
    XZ_BLE_PRINTF("id:%d,type:%d,len:%d,data:%d\n", xz_data.dp_id,xz_data.dp_type,xz_data.dp_len,xz_data.dp_data[0]);
    if ((u32)e->arg == DEVICE_EVENT_XIAO_ZHI) {
         xiao_zhi_event_callback(&xz_data);
    }


}
SYS_EVENT_HANDLER(SYS_DEVICE_EVENT, xiao_zhi_event_handler, 0);


