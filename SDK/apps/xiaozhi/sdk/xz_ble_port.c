/**
 * \file xz_ble_port.c
 *
 * \brief 
 */
/*

 */

#include "xz_ble_port.h"
#include "xz_ble_type.h"


/**
 * \file xz_ble_port.c
 *
 * \brief 
 */
/*

 */

#include "xz_ble_port.h"
#include "xz_ble_type.h"


__XZ_BLE_WEAK xz_ble_status_t xz_get_uuid(uint8_t *uuid,uint8_t len)
{
    return XZ_BLE_SUCCESS;
}

__XZ_BLE_WEAK xz_ble_status_t xz_get_random_number(uint8_t *buff,uint8_t len)
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
__XZ_BLE_WEAK xz_ble_status_t xz_ble_gap_addr_get(xz_ble_gap_addr_t *p_addr)
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
__XZ_BLE_WEAK xz_ble_status_t xz_ble_gap_addr_set(xz_ble_gap_addr_t *p_addr)
{

}



__XZ_BLE_WEAK xz_ble_status_t xz_ble_nv_init(void)
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
__XZ_BLE_WEAK xz_ble_status_t xz_ble_nv_erase(uint32_t addr,uint32_t size)
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
__XZ_BLE_WEAK xz_ble_status_t xz_ble_nv_write(uint32_t addr,const uint8_t *p_data, uint32_t size)
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
__XZ_BLE_WEAK xz_ble_status_t xz_ble_nv_read(uint32_t addr,uint8_t *p_data, uint32_t size)
{
    return XZ_BLE_SUCCESS;
}


__XZ_BLE_WEAK xz_ble_status_t xiao_zhi_ble_dp_data_report(uint8_t *p_data,uint32_t len)
{
    return XZ_BLE_SUCCESS;
}











