/*
 * timer.c
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#include "timer.h"
#include "handlerlib.h"
#include "handler.h"

struct timer_node {
	struct timer_node *_next;
	handler_msg* _msg;
	uint32_t _handlerid;
	uint32_t _expire;
};

struct timer_list {
	struct timer_node* _head;
	struct timer_node* _tail;
};

struct timer {
	struct timer_list _near[65536];//1M
	struct timer_list _far[65536]; //1M
};

inline void add_timer_link(struct timer_list* list,struct timer_node* node)
{
	node->_next = NULL;
	if(list->_tail)
		list->_tail->_next = node;
	else
		list->_head = node;
	list->_tail = node;
}

inline void clear_timer_link(struct timer_list* list)
{
	struct timer_node* node = list->_head;
	while(node) {
		my_free(node);
		node = node->_next;
	}
	list->_head = NULL;
	list->_tail = NULL;
}

///////////////////////////////////////////////////////////////////////////

struct timer_line {
	struct squeue _queue;
	time_t _systime;
	uint32_t _times;
	int _is_running;
	struct timer _timer;
};

struct timer_line _g_timerline;

void init_timer()
{
	memset(&_g_timerline,0,sizeof(_g_timerline));
	init_squeue(&_g_timerline._queue);
	time(&_g_timerline._systime);
}

void uninit_timer()
{
	int i = 0;
	struct timer_list* list;
	while(i < 65536)
	{
		list = &_g_timerline._timer._near[i];
		while(list->_head)
		{
			free_handler_msg(list->_head->_msg);
			list->_head = list->_head->_next;
		}
		clear_timer_link(list);

		list = &_g_timerline._timer._far[i];
		while(list->_head)
		{
			free_handler_msg(list->_head->_msg);
			list->_head = list->_head->_next;
		}
		clear_timer_link(list);

		++i;
	}
	struct timer_node* node = (struct timer_node*)handler_pop_squeue(&_g_timerline._queue);
	while(node)
	{
		free_handler_msg(node->_msg);
		my_free(node);
		node = (struct timer_node*)handler_pop_squeue(&_g_timerline._queue);
	}
	uninit_squeue(&_g_timerline._queue);
}

//sec: max is TIIMER_MAX
void add_timer(int sec,uint32_t handlerid,handler_msg* cd_msg)
{
	if(sec <= 0)
	{
		send_handler_msg(handlerid,cd_msg);
		return;
	}
	ASSERT(sec <= TIIMER_MAX);
	struct timer_node* node = my_new(struct timer_node);
	node->_expire = _g_timerline._times+(uint32_t)sec;
	node->_handlerid = handlerid;
	node->_msg = cd_msg;
	//add queue
	push_squeue(&_g_timerline._queue,node);
}


void do_timers()
{
	int near = _g_timerline._times&0xFFFF;
	struct timer_list* list = &_g_timerline._timer._near[near];
	struct timer_node* node = list->_head;
	while(node)
	{
		send_handler_msg(node->_handlerid,node->_msg);
		node = node->_next;
	}
	clear_timer_link(list);

	if(near == 0xFFFF)
	{
		list = &_g_timerline._timer._far[(_g_timerline._times >> 16)&0xFFFF];
		node = list->_head;
		while(node)
		{
			near = node->_expire&0xFFFF;
			add_timer_link(&_g_timerline._timer._near[near],node);
			node = node->_next;
		}
		clear_timer_link(list);
	}
	_g_timerline._times ++;
}

void do_timer_event()
{
	int i;
	struct timer_node* node;
	int count = _g_timerline._queue._count;
	for(i = 0; i < count; ++i)
	{
		node = (struct timer_node*)handler_pop_squeue(&_g_timerline._queue);
		ASSERT(node);
		if(node->_expire <= _g_timerline._times)
		{
			send_handler_msg(node->_handlerid,node->_msg);
			my_free(node);
		}
		else if((node->_expire|0xFFFF) == (_g_timerline._times|0xFFFF))
		{
			add_timer_link(&_g_timerline._timer._near[node->_expire&0xFFFF],node);
		}
		else
		{
			add_timer_link(&_g_timerline._timer._far[(node->_expire>>16)&0xFFFF],node);
		}
	}
}

void do_timer()
{
	time_t t = time(0);
	if(t == _g_timerline._systime)
		return;
	if(t > _g_timerline._systime)
	{
		int n = (int)((t-_g_timerline._systime)&0x03);//max 3 seconds
		while( n-- >= 0)
		{
			do_timers();
		}
		set_mytime(_g_timerline._times,t);
	}
	do_timer_event();
	_g_timerline._systime = t;
}
