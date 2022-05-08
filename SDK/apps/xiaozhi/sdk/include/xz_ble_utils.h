/**
 * \file xz_ble_utils.h
 *
 * \brief 
 */
/*
 *  Copyright (C) 2014-2019, xz Inc., All Rights Reserved
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
 *  This file is part of xz ble sdk 
 */

#ifndef XZ_BLE_UTILS_H_
#define XZ_BLE_UTILS_H_

#include "xz_ble_stdlib.h"


#define BSWAP_16(x)   \
            (uint16_t)((((uint16_t)(x) & 0x00ff) << 8) | \
            (((uint16_t)(x) & 0xff00) >> 8) )
             
       
#define BSWAP_32(x)   \
            (uint32_t)((((uint32_t)(x) & 0xff000000) >> 24) | \
            (((uint32_t)(x) & 0x00ff0000) >> 8) | \
            (((uint32_t)(x) & 0x0000ff00) << 8) | \
            (((uint32_t)(x) & 0x000000ff) << 24)) 
    
int32_t xz_ble_rand_number(int32_t min, int32_t max);
  
int32_t xz_ble_count_bits(uint32_t data);	

void xz_ble_inverted_array(uint8_t *array,uint16_t length);

bool xz_ble_buffer_value_is_all_x(uint8_t *buffer,uint16_t len,uint8_t value);

uint8_t xz_ble_check_sum(uint8_t *pbuf,uint16_t len);

uint8_t xz_ble_check_num(uint8_t *buf,uint8_t num);

void xz_ble_hextoascii(uint8_t *hexbuf,uint8_t len,uint8_t *ascbuf);

void xz_ble_hextostr(uint8_t *hexbuf,uint8_t len ,uint8_t *strbuf);

void xz_ble_asciitohex(uint8_t *ascbuf,uint8_t *hexbuf);

uint8_t xz_ble_char_2_ascii(uint8_t data);

void xz_ble_str_to_hex(uint8_t *str_buf,uint8_t str_len,uint8_t *hex_buf);

bool xz_ble_is_word_aligned_xz(void const* p);

uint16_t xz_ble_crc16_compute(uint8_t * p_data, uint16_t size, uint16_t * p_crc);

uint32_t xz_ble_crc32_compute(uint8_t const * p_data, uint32_t size, uint32_t const * p_crc);

void xz_ble_device_id_20_to_16(uint8_t *in,uint8_t *out);

void xz_ble_device_id_16_to_20(uint8_t *in,uint8_t *out);

char xz_ble_hexstr2hex(uint8_t *hexstr,int32_t len,uint8_t *hex);

/**
 * @brief   Function for search character/symbol index from input string    
 *
 * @param
 *          [in]data     : pointer to input data
 *          [in]data     : Number of bytes of data 
 *          [in]symbol   : Search symbol or character, Such as ':' or ','
 *          [out]index[] : Array of indexs 
 *
 * @return  The number of matched character/symbol
 *
 * @note    
 *
 * */
int32_t xz_ble_search_symbol_index(char *data, uint16_t len, char symbol, uint8_t index[]);

int32_t xz_ble_ascii_to_int(char *ascii, uint16_t len);

/**
 * @brief   Function for pem format ecc key to hex foramt
 *
 * @param
 *          [in]pem     : pointer to pem data
 *          [out]key    : pointer to output data, hex format ecc key
 *          [out]key_len: Number of bytes of key.      
 *
 * @return  0 - failed, 1 - success
 *
 * @note    developer could use xz_ble_ecc_key_pem2hex_example() function to test 
 *
 * */
int32_t xz_ble_ecc_key_pem2hex(const char *pem, uint8_t *key, uint16_t *key_len);

/**
 * @brief   Function for extract r+s from der from ecdsa sign
 *
 * @param
 *          [in]der        : pointer to der data
 *          [out]raw_rs    : pointer to output data, the raw data[r+s] of der
 *
 * @return  0 - failed, 1 - success
 *
 * @note    
 *
 * */
int32_t xz_ble_ecc_sign_secp256r1_extract_raw_from_der(const char *der, uint8_t *raw_rs);

#endif /* XZ_UTILS_H_ */
