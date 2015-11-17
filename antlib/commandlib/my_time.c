/*
 * my_time.c
 *
 *  Created on: Feb 13, 2015
 *      Author: zhaolixiang
 */

#include "my_time.h"

time_t _g_system_time = 0;
uint32_t _g_running_secends = 0;
char _g_date_buf[256] = "";
const char* _g_date = "000-00-00 00:00:00";

///////////////////////////////////////////////////////////////

//time
uint64_t get_sys_mstime()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (uint64_t)(t.tv_sec*1000)+(t.tv_usec/1000);
}


uint32_t get_next_hour_sec(int hour)
{
	time_t t = time(0);
	struct tm l;
	localtime_r(&t,&l);
	if(hour < l.tm_hour)
	{
		return (23+hour-l.tm_hour)*3600 +(59-l.tm_min)*60 + l.tm_sec;
	}
	return (23-l.tm_hour)*3600 +(59-l.tm_min)*60 + l.tm_sec;
}

time_t get_systime()
{
	return _g_system_time;
}


uint32_t get_mytime()
{
	return _g_running_secends;
}


void set_mytime(uint32_t t,time_t st)
{
	if(_g_running_secends == 0) {
		memset(_g_date_buf,0,sizeof(_g_date_buf));
	}
	_g_system_time = st;
	_g_running_secends = t;
	//update date
	struct tm l;
	localtime_r(&st,&l);
	uint32_t i = _g_running_secends%12;
	char* buf = &_g_date_buf[i*20];
	sprintf(buf,"%02d-%02d-%02d %02d:%02d:%02d",l.tm_year+1900,l.tm_mon+1,l.tm_mday,l.tm_hour,l.tm_min,l.tm_sec);
	_g_date = buf;
}


const char* get_mydate()
{
	return _g_date;
}


