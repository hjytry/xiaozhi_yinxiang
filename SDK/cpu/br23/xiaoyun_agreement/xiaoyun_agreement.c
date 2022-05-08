#include "xiaoyun_agreement.h"
extern void bluetooth_sync_time(struct sys_time *time);
extern void app_alarm_add(u8 type,struct sys_time *time);
extern void app_alarm_del(u8 index);
extern void app_alarm_del_all(void);
extern int app_send_user_data(u16 handle, u8 *data, u16 len, u8 handle_type);
#define cmd_alarm_del(index)                app_alarm_del(index)//后面这个函数由客户自己自定义
#define cmd_alarm_add(type,time)            app_alarm_add(type,time)//do { } while(0)//后面这个函数由客户自己自定义
#define cmd_alarm_del_all()                 app_alarm_del_all()//后面这个函数由客户自己自定义
#define cmd_bluetooth_sync_time(time)       bluetooth_sync_time(time)//do { } while(0)//后面这个函数由客户自己自定义
_xiaoyun_alarm_data xiaoyun_alarm_data;
_xiaoyun_time_sync_data xiaoyun_time_sync_data;
struct sys_time rtc_time_data;
static u8 response[RESPOND_BUFF_MAX];
u8 DayOfMon[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
u32 little2big(u32 le) {

    return (le & 0xff) << 24
            | (le & 0xff00) << 8
            | (le & 0xff0000) >> 8
            | (le >> 24) & 0xff;
}
u8 xiaoyun_app_check(u8 *buf,u8 len)
{
    u16 data = 0;
    for(u8 i = 0;i < len;i++)
    {
        data += buf[i];
    }
    printf("xiaoyun_app_check 0x%x\n",(u8)(0x00-data));
    return 0x00-data;
}

void GetDateTimeFromSecond(u32 lSec, struct sys_time *tTime)
{
  u32 i,j,iDay;
  u32 lDay;

  lDay = lSec / SECOND_OF_DAY;
  lSec = lSec % SECOND_OF_DAY;

  i = 1970;
  while(lDay > 365)
  {
   if(((i%4==0)&&(i%100!=0)) || (i%400==0))
    lDay -= 366;
   else
    lDay -= 365;
   i++;
  }
  if((lDay == 365) && !(((i%4==0)&&(i%100!=0)) || (i%400==0)))
  {
   lDay -= 365;
   i++;
  }
  tTime->year = i;
  for(j=0;j<12;j++)
  {
   if((j==1) && (((i%4==0)&&(i%100!=0)) || (i%400==0)))
    iDay = 29;
   else
    iDay = DayOfMon[j];
   if(lDay >= iDay) lDay -= iDay;
   else break;
  }
  tTime->month = j+1;
  tTime->day = lDay+1;
  tTime->hour = ((lSec / 3600)+8)%24;//这里注意，世界时间已经加上北京时间差8，
  tTime->min = (lSec % 3600) / 60;
  tTime->sec = (lSec % 3600) % 60;
  printf("%d %d %d %d %d %d\n",tTime->year,tTime->month,tTime->day,tTime->hour,tTime->min,tTime->sec);
}

int xiaoyun_app_response(u8 cmd_type,u8 cmd, u8 result)
{
    printf("%s[%d %d]\n", __func__, cmd, result);
    memset(response, 0x00, sizeof(response));
    response[0] = HEAD;
    response[1] = cmd_type;
    response[2] = 0x03;
    response[3] = cmd;
    response[4] = result;
    response[5] = xiaoyun_app_check(response,5);
    return app_send_user_data(0x20, response, sizeof(response),1);
}

int xiaoyun_app_data(u8 *buf, u16 len)
{
    u8 response = RESPOND_SUCCESS;
    u32 time_stamp = 0;
    printf("%s[%d]\n", __func__, len);
    put_buf(buf,len);
    if((!buf)||(buf[0] != HEAD)||(xiaoyun_app_check(buf,len-1) != buf[len-1]))
    {
        return -1;
    }
    switch (buf[CMD_TYPE_INDEX])
    {
    case CMD_CONTROL:
        {
            switch (buf[CMD_INDEX])
            {
            case CMD_CONTROL_OPEN_FAN_GEAR_1:
            case CMD_CONTROL_OPEN_FAN_GEAR_2:
            case CMD_CONTROL_OPEN_FAN_GEAR_3:
            case CMD_CONTROL_CLOSE_FAN:
            case CMD_CONTROL_OPEN_SHAKE_HEAD:
            case CMD_CONTROL_CLOSE_SHAKE_HEAD:
                {
                    //没有用到的功能在这里统一应答失败
                    printf("CMD_CONTROL_not_implemented\n");
                    response = RESPOND_FAIL;
                }
                break;
            case CMD_CONTROL_OPEN_LIGHTING:
                {
                    printf("CMD_CONTROL_OPEN_LIGHTING\n");
                    app_task_put_key_msg(KEY_TO_OPEN_BRIGHT, 0);
                }
                break;
            case CMD_CONTROL_SWITCH_LIGHTING:
                {
                    printf("CMD_CONTROL_SWITCH_LIGHTING\n");
                    app_task_put_key_msg(KEY_TO_ADJUST_BRIGHT, 0);
                }
                break;
            case CMD_CONTROL_CLOSE_LIGHTING:
                {
                    printf("CMD_CONTROL_CLOSE_LIGHTING\n");
                    app_task_put_key_msg(KEY_TO_CLOSE_BRIGHT, 0);
                }
                break;
            case CMD_CONTROL_DARKEN_LIGHTING:
                {
                    printf("CMD_CONTROL_DARKEN_LIGHTING\n");
                    app_task_put_key_msg(KEY_TO_ADJUST_DOWN_BRIGHT, 0);
                }
                break;
            case CMD_CONTROL_BRIGHTEN_LIGHTING:
                {
                    printf("CMD_CONTROL_BRIGHTEN_LIGHTING\n");
                    app_task_put_key_msg(KEY_TO_ADJUST_UP_BRIGHT, 0);
                }
                break;
            default:
                break;
            }
            xiaoyun_app_response(buf[CMD_TYPE_INDEX],buf[CMD_INDEX], response);
        }
        break;
    case CMD_ALARM_ADD:
        {
            printf("CMD_ALARM_ADD\n");
            if(len != CMD_ALARM_ADD_PACKAGE_LEN)
            {
                return -1;
            }
            memcpy((u8 *)&xiaoyun_alarm_data,buf,len);
            xiaoyun_alarm_data.timer_l = little2big(xiaoyun_alarm_data.timer_l);
            xiaoyun_alarm_data.timer_h = little2big(xiaoyun_alarm_data.timer_h);
            time_stamp = (xiaoyun_alarm_data.timer_l/1000)+(xiaoyun_alarm_data.timer_h*U32_DIVIDE_1000)+(((xiaoyun_alarm_data.timer_l%1000)+(xiaoyun_alarm_data.timer_h*U32_REMAINDER_1000))/1000);
            GetDateTimeFromSecond(time_stamp,&rtc_time_data);
            if((buf[CMD_ALARM_ADD_DATA_TYPE_INDEX] != APP_ALARM_MODE_EVERY_DAY)&&(buf[CMD_ALARM_ADD_DATA_TYPE_INDEX] != APP_ALARM_MODE_ONCE))
            {
                cmd_alarm_add(buf[CMD_ALARM_ADD_DATA_TYPE_INDEX]<<1,&rtc_time_data);
            }else{
                cmd_alarm_add(E_ALARM_MODE_EVERY_DAY,&rtc_time_data);
            }

        }
        break;
    case CMD_ALARM_DEL:
        {
            printf("CMD_ALARM_DEL\n");
            switch (buf[CMD_ALARM_DEL_DATA_TIME_NUMBER_INDEX])
            {
            case CMD_ALARM_DEL_DATA_TIME_NUMBER_1:
                {
                    cmd_alarm_del(CMD_ALARM_DEL_DATA_TIME_NUMBER_1);
                }
                break;
            case CMD_ALARM_DEL_DATA_TIME_NUMBER_2:
                {
                    cmd_alarm_del(CMD_ALARM_DEL_DATA_TIME_NUMBER_2);
                }
                break;
            default:
                break;
            }

        }
        break;
    case CMD_ALARM_DEL_ALL:
        {
            printf("CMD_ALARM_DEL_ALL\n");
            cmd_alarm_del_all();
        }
        break;
    case CMD_WEATHER:
        break;
    case CMD_BLUETOOTH_CONTROL:
        {
            switch (buf[CMD_INDEX])
            {
            case CMD_BLUETOOTH_CONTROL_MUSIC_PREV:
            case CMD_BLUETOOTH_CONTROL_MUSIC_NEXT:
            case CMD_BLUETOOTH_CONTROL_VOL_UP:
            case CMD_BLUETOOTH_CONTROL_VOL_DOWN:
            case CMD_BLUETOOTH_CONTROL_MUSIC_PLAY:
            case CMD_BLUETOOTH_CONTROL_MUSIC_SUSPEND:
            case CMD_BLUETOOTH_CONTROL_LAST_NO:
                {
                    //没有用到的功能在这里
                    printf("CMD_BLUETOOTH_CONTROL_not_implemented\n");
                }
                break;

            default:
                break;
            }
        }
        break;
    case CMD_BLUETOOTH_SYNC_TIME:
        {
            printf("CMD_BLUETOOTH_SYNC_TIME\n");
            if(len != CMD_BLUETOOTH_SYNC_TIME_PACKAGE_LEN)
            {
                return -1;
            }
            memcpy((u8 *)&xiaoyun_time_sync_data,buf,len);
            xiaoyun_time_sync_data.timer_l = little2big(xiaoyun_time_sync_data.timer_l);
            xiaoyun_time_sync_data.timer_h = little2big(xiaoyun_time_sync_data.timer_h);
            time_stamp = (xiaoyun_time_sync_data.timer_l/1000)+(xiaoyun_time_sync_data.timer_h*U32_DIVIDE_1000)+(((xiaoyun_time_sync_data.timer_l%1000)+(xiaoyun_time_sync_data.timer_h*U32_REMAINDER_1000))/1000);
            GetDateTimeFromSecond(time_stamp,&rtc_time_data);
            cmd_bluetooth_sync_time(&rtc_time_data);
        }
        break;
    case CMD_BLUETOOTH_PHONE:
        break;
    default:
        break;
    }
    return 0;
}
