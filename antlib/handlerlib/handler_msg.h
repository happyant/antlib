/*
 * handler_msg.h
 *
 *  Created on: 2015-2-10
 *      Author: lx
 */

#ifndef HANDLER_MSG_H_
#define HANDLER_MSG_H_

#include "commandlib/command_header.h"

typedef struct handler_msg {
	uint64_t _userid;
	int _msgid;
	int _datalen;
	void*  _data;
}handler_msg;

handler_msg* create_handler_msg(int size);

void free_handler_msg(handler_msg* msg);

void add_handler_msg_used(handler_msg* msg,int add);

void reset_handler_msg_data(handler_msg* msg);

//cache size need 40 bytes for head(8K is 8192-40=8142)
void create_handler_msg_cachepool(int cachesize,int poolsize);
void destory_handler_msg_cachepool();

#endif /* HANDLER_MSG_H_ */
