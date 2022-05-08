/*
 * @Author: your name
 * @Date: 2021-04-23 09:49:37
 * @LastEditTime: 2021-04-25 11:54:05
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \SDK\apps\soundbox\log_config\lib_update_config.c
 */

#include "app_config.h"
#include "system/includes.h"
#include "update_loader_download.h"

#ifdef CONFIG_256K_FLASH
const int config_update_mode = UPDATE_UART_EN;
#else
#ifdef CONFIG_SOUNDBOX_FLASH_256K
const int config_update_mode = UPDATE_UART_EN;
#else
#ifdef  CONFIG_AC608N
const int config_update_mode = UPDATE_STORAGE_DEV_EN | UPDATE_UART_EN;
#else
#ifdef   CONFIG_BOARD_AC6083A
const int config_update_mode = UPDATE_STORAGE_DEV_EN | UPDATE_UART_EN;
#else
const int config_update_mode = UPDATE_BT_LMP_EN | UPDATE_STORAGE_DEV_EN;
//| UPDATE_UART_EN | UPDATE_APP_EN;
#endif
#endif
#endif
#endif

//是否采用双备份升级方案:0-单备份;1-双备份
const int support_dual_bank_update_en = 0;

//是否支持外挂flash升级,需要打开Board.h中的TCFG_NOR_FS_ENABLE
const int support_norflash_update_en  = 0;

#if OTA_TWS_SAME_TIME_NEW       //使用新的同步升级流程
const int support_ota_tws_same_time_new =  1;
#else
const int support_ota_tws_same_time_new =  0;
#endif

//是否支持升级之后保留vm数据
#if (((defined CONFIG_CPU_BR25) || (defined CONFIG_CPU_BR23)) && SMART_BOX_EN)
const int support_vm_data_keep = 0;
#else
const int support_vm_data_keep = 0;
#endif

const char log_tag_const_v_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  FALSE;
const char log_tag_const_i_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
const char log_tag_const_d_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  FALSE;
const char log_tag_const_w_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
const char log_tag_const_e_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
