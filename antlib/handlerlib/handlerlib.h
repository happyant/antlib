/*
 * handlerlib.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef HANDLERLIB_H_
#define HANDLERLIB_H_


#include "handler_msg.h"
#include "timer.h"

void init_handlerlib(int server_id,int handlers_max);


void uninit_handlerlib();

/**
 * @groupid: group id 1~31
 * @thread_count:the threads number that work for this group
**/
int create_handlers_group(int groupid,int thread_count);


uint32_t register_handler(int group_id,const char* handler_name,
		void (*do_msg_fun)(handler_msg*,void*,uint32_t),
		void (*do_handler_fun)(void*,uint32_t),
		void (*free_fun)(void*),
		void* hdata );


void start_all_handlers();


void stop_all_handlers();


void send_handler_msg(uint32_t handlerid,handler_msg* msg);


#endif /* HANDLERLIB_H_ */
