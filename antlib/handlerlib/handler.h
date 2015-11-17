/*
 * handler.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 *
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include "commandlib/hqueue.h"
#include "handler_msg.h"
#include "thread_data.h"

#define HANDLERPOOL_MAX 8192


#define HANDLERID_CREATE(harbor,id) (((harbor&0x7FFFF)<<13)|(id&0x1FFF))
#define HANDLERID_HARBOR(handlerid) ((handlerid>>13)&&0x7FFFF)
#define HANDLERID_ID(handlerid) (handlerid&0x1FFF)


typedef void (*do_handlermsg_fun)(handler_msg*,void*,uint32_t handlerid);
typedef void (*free_handlerdata_fun)(void*);
typedef void (*do_handler_fun)(void* data,uint32_t handlerid);


struct handler
{
	char _name[32];
	struct squeue _queue;

	do_handlermsg_fun _domsg_proc;
	do_handler_fun _handler_proc;
	free_handlerdata_fun _free_proc;
	void* _hdata;

	uint32_t _handlerid;
	uint32_t _locker;
};


struct handlers_list
{
	struct double_list_node _link_node;
	struct handler** _handlers;
	int _id;
	uint32_t _size;
	uint32_t _index;
	uint32_t _threads_num;
	struct handlers_list* _next;
};


void init_handlers_list(struct handlers_list* list,int threads_num);

void uninit_handlers_list(struct handlers_list* list);


void init_handler(struct handler* hl,const char* name,
		do_handlermsg_fun domsg_f,
		do_handler_fun dohandler_f,
		free_handlerdata_fun free_f,
		void* data );


void destory_handler(struct handler* hl);

void add_handler(struct handlers_list* list,struct handler* hl);

void crate_handlers_thread(struct handlers_list* list);

//handler_pop_squeue(struct squeue* qu)
static inline int do_handler(struct handler* hl)
{
	handler_msg* msg;
	int i = 0;
	int count = hl->_queue._count;
	set_handlerid(hl->_handlerid);
	while(i < count)
	{
		msg = handler_pop_squeue(&hl->_queue);
		ASSERT(msg);
		hl->_domsg_proc(msg,hl->_hdata,hl->_handlerid);
		free_handler_msg(msg);
		++i;
	}
	if(hl->_handler_proc)
	{
		hl->_handler_proc(hl->_hdata,hl->_handlerid);
	}
	return count;
}


#endif /* HANDLER_H_ */
