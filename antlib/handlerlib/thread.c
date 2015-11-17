/*
 * thread.c
 *
 *  Created on: 2015-2-10
 *      Author: lx
 */
#include "thread.h"
#include "thread_data.h"
#include "commandlib/double_list.h"

struct thread
{
	struct double_list_node _linknode;
	char _name[THREAD_NAME_LEN];
	pthread_t _fd;
	int _running;
	void (*_thread_fun)(void*);
	void* _data;
};

struct threads_list
{
	int _state;
	int _thread_count;
	struct double_list _threadlist;
};


struct threads_list _g_threads_list = {0,0,{0,0}};

////////////////////////////////////////////////////////////////////

void create_threads_list()
{
	create_thread_date_list();
}

void destory_threads_list()
{
	struct thread* th;
	struct double_list_node* node = _g_threads_list._threadlist._head;
	while(th)
	{
		th = (struct thread*)node;
		node = node->_next;
		my_free(th);
	}
	destory_thread_data_list();
}


static void* _thread_proc(void* data)
{
	struct thread* th = (struct thread*)data;
	set_threadname(th->_name);
	while(th->_running)
	{
		th->_thread_fun(th->_data);
		usleep(1000);
	}
	return 0;
}


int create_thread(const char* thname,void (*th_proc)(void*),void* data)
{
	struct thread* th = my_new(struct thread);
	memset(th,0,sizeof(*th));
	STRNCPY(th->_name,thname,THREAD_NAME_LEN);
	th->_thread_fun = th_proc;
	th->_data = data;
	double_list_add_tail(&_g_threads_list._threadlist,&th->_linknode);
	_g_threads_list._thread_count ++;
	return 0;
}

void start_all_threads()
{
	struct thread* th;
	struct double_list_node* node = _g_threads_list._threadlist._head;
	while(node)
	{
		th = (struct thread*)node;
		th->_running = 1;
		pthread_create(&th->_fd,NULL,_thread_proc,th);
		node = node->_next;
	}
}

void stop_all_threads()
{
	struct thread* th;
	struct double_list_node* node = _g_threads_list._threadlist._head;
	while(node)
	{
		th = (struct thread*)node;
		th->_running = 0;
		node = node->_next;
	}
	node = _g_threads_list._threadlist._head;
	while(node)
	{
		th = (struct thread*)node;
		pthread_join(th->_fd,NULL);
		node = node->_next;
	}
}

