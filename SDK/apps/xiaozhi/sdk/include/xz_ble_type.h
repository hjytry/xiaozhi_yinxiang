/**
 * \file tuya_ble_type.h
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


#ifndef XZ_BLE_TYPE_H__
#define XZ_BLE_TYPE_H__

#include "xz_ble_stdlib.h"
#include "xz_ble_app_demo.h"


#ifndef NULL
#define NULL 0
#endif




#ifndef __XZ_BLE_ASM
#define __XZ_BLE_ASM               __asm__
#endif

#ifndef __XZ_BLE_INLINE
#define __XZ_BLE_INLINE            inline
#endif

#ifndef __XZ_BLE_WEAK
#define __XZ_BLE_WEAK              __attribute__((weak))
#endif

#ifndef __XZ_BLE_ALIGN
#define __XZ_BLE_ALIGN(n)          __attribute__((aligned(n)))
#endif

#ifndef __XZ_BLE_PACKED
#define __XZ_BLE_PACKED           __attribute__((packed))
#endif

#define XZ_BLE_GET_SP()


typedef enum {
    XZ_BLE_SUCCESS  = 0x00,
    XZ_BLE_ERR_INTERNAL,
    XZ_BLE_ERR_NOT_FOUND,
    XZ_BLE_ERR_NO_EVENT,
    XZ_BLE_ERR_NO_MEM,
    XZ_BLE_ERR_INVALID_ADDR,     // Invalid pointer supplied
    XZ_BLE_ERR_INVALID_PARAM,    // Invalid parameter(s) supplied.
    XZ_BLE_ERR_INVALID_STATE,    // Invalid state to perform operation.
    XZ_BLE_ERR_INVALID_LENGTH,
    XZ_BLE_ERR_DATA_SIZE,
    XZ_BLE_ERR_TIMEOUT,
    XZ_BLE_ERR_BUSY,
    XZ_BLE_ERR_COMMON,
    XZ_BLE_ERR_RESOURCES,
    XZ_BLE_ERR_UNKNOWN,          // other ble sdk errors
} xz_ble_status_t;

typedef enum {
    XZ_RES_OPT_SUCCESS  = 0x00,
    XZ_RES_ERR_NOT_SUPPORTED,
    XZ_RES_ERR_SET_FUNC_FAIL,
    XZ_RES_ERR_PARAM,
    XZ_RES_ERR_LENGTH,
    XZ_RES_ERR_BOUND,
    XZ_RES_ERR_UNKNOWN,          // other ble sdk errors

} xz_res_value_t;

#define MAC_LEN     6
#define RESPOND_ACK_LEN 6
#define BOUND_INFO_LEN      20

#pragma pack(1)//不同平台对齐编译
typedef struct
{
    uint8_t   mac_type;
    uint8_t   mac[MAC_LEN];
    uint8_t   bound_flag;
    uint8_t   bound_info[BOUND_INFO_LEN];

} xz_ble_auth_settings_t;
#pragma pack()

#pragma pack(1)//不同平台对齐编译
typedef struct xiao_zhi_trans_data
{
    uint8_t  dp_id;
    uint8_t  dp_type;
    uint16_t dp_len;
    uint8_t  dp_data[XZ_DATA_MAX_LEN];
}xiao_zhi_ble_cb_evt_param_t;
#pragma pack()

/**
 * tuya ble call back event type.
 * */
typedef enum {
    XZ_BLE_CB_EVT_CONNECTE_STATUS,
    XZ_BLE_CB_EVT_DP_WRITE,          // old version
    XZ_BLE_CB_EVT_DP_QUERY,
    XZ_BLE_CB_EVT_DP_DATA_RECEIVED,  // new version
    XZ_BLE_CB_EVT_OTA_DATA,
    XZ_BLE_CB_EVT_BULK_DATA,
    XZ_BLE_CB_EVT_NETWORK_INFO,
    XZ_BLE_CB_EVT_WIFI_SSID,
    XZ_BLE_CB_EVT_TIME_STAMP,
    XZ_BLE_CB_EVT_TIME_NORMAL,
	XZ_BLE_CB_EVT_APP_LOCAL_TIME_NORMAL,
	XZ_BLE_CB_EVT_TIME_STAMP_WITH_DST,
    XZ_BLE_CB_EVT_DATA_PASSTHROUGH,
    XZ_BLE_CB_EVT_DP_DATA_REPORT_RESPONSE,
    XZ_BLE_CB_EVT_DP_DATA_WTTH_TIME_REPORT_RESPONSE,
    XZ_BLE_CB_EVT_DP_DATA_WITH_FLAG_REPORT_RESPONSE,
    XZ_BLE_CB_EVT_DP_DATA_WITH_FLAG_AND_TIME_REPORT_RESPONSE,
    XZ_BLE_CB_EVT_DP_DATA_SEND_RESPONSE,               // new version
    XZ_BLE_CB_EVT_DP_DATA_WITH_TIME_SEND_RESPONSE,     // new version
    XZ_BLE_CB_EVT_UNBOUND,
    XZ_BLE_CB_EVT_ANOMALY_UNBOUND,
    XZ_BLE_CB_EVT_DEVICE_RESET,
    XZ_BLE_CB_EVT_UPDATE_LOGIN_KEY_VID,
    XZ_BLE_CB_EVT_UNBIND_RESET_RESPONSE,               // Notify the application of the result of the local reset
	XZ_BLE_CB_EVT_WEATHER_DATA_REQ_RESPONSE,	         // received request weather data app response
	XZ_BLE_CB_EVT_WEATHER_DATA_RECEIVED, 		         // received app sync weather data
} xiao_zhi_ble_cb_evt_t;

typedef enum {
    XZ_BLE_ADDRESS_TYPE_PUBLIC, // public address
    XZ_BLE_ADDRESS_TYPE_RANDOM, // random address
} xz_ble_addr_type_t;

typedef struct {
    xz_ble_addr_type_t addr_type;
    uint8_t addr[6];
} xz_ble_gap_addr_t;

#endif
