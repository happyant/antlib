/*
 * handlerlib.c
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#include "handlerlib.h"
#include "thread.h"
#include "handler.h"
#include "timer.h"
#include "thread_data.h"

struct handlerlib {
	struct handlers_list* _pools;
	int _server_id;
	int _list_index;
	int _list_count;
	int _state;
	struct handler _list[1];
};

struct handlerlib* _g_handlerlib = NULL;

///////////////////////////////////////////////////////////////////////

void init_handlerlib(int server_id,int handlers_max)
{
	ASSERT(handlers_max > 0 && handlers_max <= 2048);
	int size = sizeof(struct handlerlib) + (sizeof(struct handler)*(handlers_max-1));
	_g_handlerlib = (struct handlerlib*)my_malloc(size);
	memset(_g_handlerlib,0,size);

	_g_handlerlib->_server_id = server_id;
	_g_handlerlib->_list_count = handlers_max;

	LOCK_ADD(_g_handlerlib->_state,1);

	init_timer();
	create_threads_list();
}

void uninit_handlerlib()
{
	destory_threads_list();
	uninit_timer();

	struct handlers_list* node;;
	while(_g_handlerlib->_pools)
	{
		node = _g_handlerlib->_pools;
		_g_handlerlib->_pools = _g_handlerlib->_pools->_next;
		my_free(node);
	}
	int i;
	for(i = 0; i < _g_handlerlib->_list_count; ++i)
	{
		destory_handler(&_g_handlerlib->_list[i]);
	}

	my_free(_g_handlerlib);
	_g_handlerlib = NULL;
}

struct handlers_list* _get_handler_group(int groupid)
{
	struct handlers_list* node = _g_handlerlib->_pools;
	while(node) {
		if(node->_id == groupid)
			return node;
		node = node->_next;
	}
	return NULL;
}

int create_handlers_group(int groupid,int thread_count)
{
	ASSERT(groupid > 0 && groupid < 31 && thread_count > 0 && thread_count < 1024);
	struct handlers_list* list = _get_handler_group(groupid);
	ASSERT(NULL == list);
	if(list) {
		return -1;
	}
	list = my_new(struct handlers_list);
	init_handlers_list(list,thread_count);
	list->_id = groupid;
	list->_next = _g_handlerlib->_pools;
	_g_handlerlib->_pools = list;
	return 0;
}

uint32_t register_handler(int group_id,const char* handler_name,
		void (*do_msg_fun)(handler_msg*,void*,uint32_t),
		void (*do_handler_fun)(void*,uint32_t),
		void (*free_fun)(void*),void* hdata )
{
	ASSERT(_g_handlerlib->_list_index < _g_handlerlib->_list_count && group_id >= 0 && group_id < 32);
	struct handlers_list* list = _get_handler_group(group_id);
	ASSERT(list);
	int index = _g_handlerlib->_list_index ++;
	struct handler* hl = &_g_handlerlib->_list[index];
	init_handler(hl,handler_name,do_msg_fun,do_handler_fun,free_fun,hdata);

	hl->_handlerid = ((_g_handlerlib->_server_id << 16)|(group_id << 11) | (index&0x7FF));

	add_handler(list,hl);
	return hl->_handlerid;
}

void start_all_handlers()
{
	ASSERT(_g_handlerlib->_state == 1);
	if(LOCK_ADD(_g_handlerlib->_state,1) == 1)
	{
		struct handlers_list* list = _g_handlerlib->_pools;
		while(list)
		{
			crate_handlers_thread(list);
			list = list->_next;
		}
		start_all_threads();
	}
}

void stop_all_handlers()
{
	ASSERT(_g_handlerlib->_state == 2);
	if(LOCK_ADD(_g_handlerlib->_state,1) == 2)
	{
		stop_all_threads();
	}
}

void send_handler_msg(uint32_t handlerid,handler_msg* msg)
{
	ASSERT(_g_handlerlib->_state == 2 || _g_handlerlib->_state == 1);

	int index = (handlerid & 0x7FF);

	ASSERT(index < _g_handlerlib->_list_count);
	struct handler* hl = &_g_handlerlib->_list[index];
	ASSERT(hl->_handlerid > 0);
	add_handler_msg_used(msg,1);
	push_squeue(&hl->_queue,msg);
}

