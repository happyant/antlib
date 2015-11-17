/*
 * timer.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "handler_msg.h"


#define TIIMER_MAX (30*24*3600)


void init_timer();

void uninit_timer();

//sec: max is TIIMER_MAX
void add_timer(int sec,uint32_t handlerid,handler_msg* cd_msg);


void do_timer();



#endif /* TIMER_H_ */
