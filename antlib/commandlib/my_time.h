/*
 * my_time.h
 *
 *  Created on: Feb 13, 2015
 *      Author: zhaolixiang
 */

#ifndef MY_TIME_H_
#define MY_TIME_H_

#include "sys_header.h"


//time
uint64_t get_sys_mstime();


uint32_t get_next_hour_sec(int hour);


time_t get_systime();


uint32_t get_mytime();


void set_mytime(uint32_t t,time_t st);


const char* get_mydate();



#endif /* MY_TIME_H_ */
