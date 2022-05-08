#ifndef __SMARTBOX_CMD_USER_H__
#define __SMARTBOX_CMD_USER_H__

#include "typedef.h"
#include "JL_rcsp_protocol.h"
#define     USER_SET_BRIGHTNESS    0x01
#define     USER_GET_BRIGHTNESS    0x02
#define     USER_SET_VOLUME        0x03
#define     USER_GET_VOLUME        0x04
#define     USER_SET_FM            0x05
#define     USER_REQUEST_FM        0x06
#define     USER_GET_FM            0x07

#define     BRIGHTNESS_0           0x00
#define     BRIGHTNESS_1           0x01
#define     BRIGHTNESS_2           0x02
#define     BRIGHTNESS_3           0x03

#define     FM_PP                  0x00
#define     FM_PLAY                0x01
#define     FM_PAUSE               0x02

#define     FM_NULL_STATE                   0x00
#define     FM_PLAY_STATE                   0x01
#define     FM_PAUSE_STATE                  0x02

//用户自定义数据接收
void smartbox_user_cmd_recieve(void *priv, u8 OpCode, u8 OpCode_SN, u8 *data, u16 len);
//用户自定义数据发送
JL_ERR smartbox_user_cmd_send(u8 *data, u16 len);
extern void user_ctrl_pp_network_fm(u8 cmd);
#endif//__SMARTBOX_CMD_USER_H__
