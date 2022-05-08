/**
 * \file xz_ble_port.h
 *
 * \brief
 */
/*
 */



#ifndef XZ_BLE_PORT_H__
#define XZ_BLE_PORT_H__

#include "xz_ble_type.h"

#define XZ_LOG_EN       1
#if XZ_LOG_EN
#define XZ_BLE_PRINTF    			            printf
#define XZ_BLE_HEXDUMP                           put_buf
#else
#define XZ_BLE_PRINTF    			            printf
#define XZ_BLE_HEXDUMP                           put_buf
#endif


xz_ble_status_t xz_get_uuid(uint8_t *uuid,uint8_t len);
xz_ble_status_t xz_get_random_number(uint8_t *buff,uint8_t len);
xz_ble_status_t xz_ble_gap_addr_get(xz_ble_gap_addr_t *p_addr);
xz_ble_status_t xz_ble_gap_addr_set(xz_ble_gap_addr_t *p_addr);
xz_ble_status_t xz_ble_nv_init(void);
xz_ble_status_t xz_ble_nv_erase(uint32_t addr,uint32_t size);
xz_ble_status_t xz_ble_nv_write(uint32_t addr,const uint8_t *p_data, uint32_t size);
xz_ble_status_t xz_ble_nv_read(uint32_t addr,uint8_t *p_data, uint32_t size);
xz_ble_status_t xiao_zhi_ble_dp_data_report(uint8_t *p_data,uint32_t len);
#endif

