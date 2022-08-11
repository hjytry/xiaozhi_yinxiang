/*
 * @Description: 
 * @Author: HJY
 * @Date: 2022-05-08 12:12:28
 * @LastEditTime: 2022-07-11 09:36:30
 * @LastEditors: HJY
 */
#ifndef SYS_TIME_H
#define SYS_TIME_H

#include "typedef.h"


struct sys_time {
    // u16 crc;
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
    u16 crc;
};

#if 0
struct tm {
    int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
    int tm_min;			/* Minutes.	[0-59] */
    int tm_hour;			/* Hours.	[0-23] */
    int tm_mday;			/* Day.		[1-31] */
    int tm_mon;			/* Month.	[0-11] */
    int tm_year;			/* Year	- 1900.  */
    int tm_wday;			/* Day of week.	[0-6] */
    int tm_yday;			/* Days in year.[0-365]	*/
    int tm_isdst;			/* DST.		[-1/0/1]*/

# ifdef	__USE_MISC
    long int tm_gmtoff;		/* Seconds east of UTC.  */
    const char *tm_zone;		/* Timezone abbreviation.  */
# else
    long int __tm_gmtoff;		/* Seconds east of UTC.  */
    const char *__tm_zone;	/* Timezone abbreviation.  */
# endif
};

#endif












#endif
