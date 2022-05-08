#include "smartbox/cmd_user.h"
#include "syscfg_id.h"
#include "ble_user.h"
#include "le_smartbox_module.h"

//*----------------------------------------------------------------------------*/
/**@brief    smartbox自定义命令数据接收处理
   @param    priv:全局smartbox结构体， OpCode:当前命令， OpCode_SN:当前的SN值， data:数据， len:数据长度
   @return
   @note	 二次开发需要增加自定义命令，通过JL_OPCODE_CUSTOMER_USER进行扩展，
  			 不要定义这个命令以外的命令，避免后续SDK更新导致命令冲突
*/
/*----------------------------------------------------------------------------*/
extern u8 bright_index;
extern u8 auto_reduce_bright_flag;
extern u8 bright_arr[];
extern int alarm_tone_vol_1;
extern int alarm_tone_vol_2;
extern void led8_set_pwm_duty(u32 duty,bool en);
u8 brightness_send_buf[] = {0x02, USER_SET_BRIGHTNESS, 0x00};
u8 volume_send_buf[] = {0x03, USER_SET_VOLUME, 0x00,0x00};
u8 network_fm_pp_buf[] = {0x02,USER_SET_FM,0x00};
u8 get_network_fm_state_buf[] = {0x01,USER_REQUEST_FM};
u8 network_fm_state_buf[] = {0x02,USER_GET_FM,0x00};

u8 last_fm_state_play = FALSE;
#if (SMART_BOX_EN)
void smartbox_user_cmd_recieve(void *priv, u8 OpCode, u8 OpCode_SN, u8 *data, u16 len)
{
    //自定义数据接收
    printf("%s:", __FUNCTION__);
    put_buf(data, len);
    switch (data[1])
    {
    case USER_SET_BRIGHTNESS:
        {
            printf("USER_SET_BRIGHTNESS");
            switch (data[2])
            {
            case BRIGHTNESS_0:
            case BRIGHTNESS_1:
            case BRIGHTNESS_2:
            case BRIGHTNESS_3:
                bright_index = data[2];
                auto_reduce_bright_flag = FALSE;
                led8_set_pwm_duty(bright_arr[bright_index],0);
                break;
            default:
                break;
            }
        }
        break;
    case USER_GET_BRIGHTNESS:
        {
            printf("USER_GET_BRIGHTNESS");
            // syscfg_read(CFG_USER_BRIGHTNESS,&brightness,1);
            extern u8 bright_cur_index;
            brightness_send_buf[2] = bright_cur_index;
            smartbox_user_cmd_send(brightness_send_buf, sizeof(brightness_send_buf));
        }
        break;
    case USER_SET_VOLUME:
        {
            printf("USER_SET_VOLUME");
            if(data[2] == 0x00){
                alarm_tone_vol_1 = (u8)(((float)data[3]/100.0)*30);
                syscfg_write(CFG_USER_ALARM_TONE_VOL_1, &alarm_tone_vol_1, 1);
            }else if(data[2] == 0x01){
                alarm_tone_vol_2 = (u8)(((float)data[3]/100.0)*30);
                syscfg_write(CFG_USER_ALARM_TONE_VOL_2, &alarm_tone_vol_2, 1);
            }
        }
        break;
    case USER_GET_VOLUME:
        {
            printf("USER_GET_VOLUME");
            if(data[2] == 0x00){
                // syscfg_read(CFG_USER_ALARM_TONE_VOL_1, &alarm_tone_vol_1, 1);
                volume_send_buf[2] = 0x00;
                volume_send_buf[3] = (u8)(((float)alarm_tone_vol_1/30.0)*100);
            }else if(data[2] == 0x01){
                // syscfg_read(CFG_USER_ALARM_TONE_VOL_1, &alarm_tone_vol_1, 1);
                volume_send_buf[2] = 0x01;
                volume_send_buf[3] = (u8)(((float)alarm_tone_vol_2/30.0)*100);
            }
            smartbox_user_cmd_send(volume_send_buf, sizeof(volume_send_buf));
        }
        break;
    case USER_GET_FM:
        {
            printf("USER_GET_VOLUME");
            if(data[2] == FM_PLAY_STATE){
                user_ctrl_pp_network_fm(FM_PAUSE);
                last_fm_state_play = TRUE;
            }else{
                last_fm_state_play = FALSE;
            }            
        }
        break;
    default:
        break;
    }
#if 0
    ///以下是发送测试代码
    smartbox_user_cmd_send(test_send_buf, sizeof(test_send_buf));
#endif

    JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, NULL, 0);

}

void  user_send_brightness_state(void)
{

    printf("%s",__func__);
    if(get_ble_work_state() == BLE_ST_NOTIFY_IDICATE){

        extern u8 bright_cur_index;
        // syscfg_read(CFG_USER_BRIGHTNESS,&brightness,1);
        brightness_send_buf[2] = bright_cur_index;
        smartbox_user_cmd_send(brightness_send_buf, sizeof(brightness_send_buf));
    }
}

void  user_get_network_fm_state_buf(void)
{

    printf("%s",__func__);
    if(get_ble_work_state() == BLE_ST_NOTIFY_IDICATE){
        if(!last_fm_state_play){
            smartbox_user_cmd_send(get_network_fm_state_buf, sizeof(get_network_fm_state_buf));
        }
    }
}

void recovery_network_fm_state(void)
{
    printf("%s",__func__);
    if(get_ble_work_state() == BLE_ST_NOTIFY_IDICATE){
        if(last_fm_state_play){
            user_ctrl_pp_network_fm(FM_PLAY);
        }
    }    
    last_fm_state_play = FALSE;
}


void user_ctrl_pp_network_fm(u8 cmd)
{
    printf("%s %d",__func__,cmd);
    if(get_ble_work_state() == BLE_ST_NOTIFY_IDICATE){
        switch (cmd)
        {
        case FM_PP:
        case FM_PLAY:
        case FM_PAUSE:
            network_fm_pp_buf[2] = cmd;
            smartbox_user_cmd_send(network_fm_pp_buf, sizeof(network_fm_pp_buf));
            break;
        
        default:
            break;
        }

    }    
}    


//*----------------------------------------------------------------------------*/
/**@brief    smartbox自定义命令数据发送接口
   @param    data:数据， len:数据长度
   @return
   @note	 二次开发需要增加自定义命令，通过JL_OPCODE_CUSTOMER_USER进行扩展，
  			 不要定义这个命令以外的命令，避免后续SDK更新导致命令冲突
*/
/*----------------------------------------------------------------------------*/
JL_ERR smartbox_user_cmd_send(u8 *data, u16 len)
{
    //自定义数据接收
    printf("%s:", __FUNCTION__);
    put_buf(data, len);
    return JL_CMD_send(JL_OPCODE_CUSTOMER_USER, data, len, 0);
}
#endif

