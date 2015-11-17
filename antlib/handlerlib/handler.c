/*
 * handler.c
 *
 *  Created on: Feb 14, 2015
 *      Author: zhaolixiang
 */

#include "handler.h"
#include "thread_proc.h"
#include "thread.h"

void init_handlers_list(struct handlers_list* list,int threads_num)
{
	memset(list,0,sizeof(*list));
	list->_threads_num = threads_num;
}

void uninit_handlers_list(struct handlers_list* list)
{
	if(list->_handlers)
	{
		my_free(list->_handlers);
		list->_handlers = NULL;
	}
}

void init_handler(struct handler* hl,const char* name,
		do_handlermsg_fun domsg_f,
		do_handler_fun dohandler_f,
		free_handlerdata_fun free_f,
		void* data )
{
	ASSERT(hl->_handlerid == 0);
	if(name && name[0]) {
		STRNCPY(hl->_name,name,32);
	}
	hl->_domsg_proc = domsg_f;
	hl->_free_proc = free_f;
	hl->_handler_proc = dohandler_f;
	hl->_hdata = data;
	init_squeue(&hl->_queue);
}

void destory_handler(struct handler* hl)
{
	if(hl->_handlerid == 0)
		return;
	handler_msg* msg;
	while(hl->_queue._count > 0)
	{
		msg = handler_pop_squeue(&hl->_queue);
		ASSERT(msg);
		free_handler_msg(msg);
	}
	if(hl->_free_proc)
	{
		hl->_free_proc(hl->_hdata);
	}
	uninit_squeue(&hl->_queue);
}

void add_handler(struct handlers_list* list,struct handler* hl)
{
	if(list->_index == list->_size)
	{
		void* newlist = my_malloc(sizeof(void*)*(list->_size+32));
		if(list->_handlers) {
			memcpy(newlist,list->_handlers,sizeof(void*)*list->_size);
			my_free(list->_handlers);
		}
		list->_size = list->_size + 32;
		list->_handlers = (struct handler**)newlist;
	}
	ASSERT(list->_index < list->_size);
	list->_handlers[list->_index++] = hl;
}


void crate_handlers_thread(struct handlers_list* list)
{
	int i = 0;
	if(list->_index <= list->_threads_num)
	{
		while(i < list->_index)
		{
			create_thread("handler",thread_proc_handler,list->_handlers[i]);
			++i;
		}
		list->_threads_num = list->_index;
	}
	else
	{
		while(i < list->_threads_num)
		{
			create_thread("handler_lsit",handlers_list_thread_proc,list);
			++i;
		}
	}
	list->_size = list->_index;
	list->_index = 0;
}
