/**
 * \file xz_ble_port.h
 *
 * \brief 
 */
/* 
 */


#ifndef XIAOZHI_LIB_H__
#define XIAOZHI_LIB_H__

extern unsigned char xz_ble_gap_advertising_adv_data_update(unsigned char *p_ad_data);
extern unsigned char  xz_ble_gap_advertising_scan_rsp_data_update(unsigned char *p_ad_data);
extern void xiaozhi_enc_init(void);
extern unsigned char  xz_sent_encode_data(unsigned char *data,unsigned char len);
extern unsigned char  xz_recive_decrypt_data(unsigned char *data,unsigned char len);
#endif

