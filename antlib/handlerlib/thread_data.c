/*
 * thread_data.c
 *
 *  Created on: 2015-2-9
 *      Author: lx
 */
#include "thread_data.h"

struct thread_data_list _g_thread_datas = {0,0};

void create_thread_date_list()
{
	memset(&_g_thread_datas,0,sizeof(_g_thread_datas));
	pthread_key_create(&_g_thread_datas._key, NULL);
	set_threadname("main");

}

void destory_thread_data_list()
{
	pthread_key_delete(_g_thread_datas._key);
}

struct thread_data* get_thread_data()
{
	struct thread_data* data = (struct thread_data*)pthread_getspecific(_g_thread_datas._key);
	if(0 == data)
	{
		unsigned int id = LOCK_ADD(_g_thread_datas._index,1);
		assert(id < MAXTHREAD);
		data = &_g_thread_datas._array[id];
		data->_thid = id;
		pthread_setspecific(_g_thread_datas._key,data);
	}
	return data;
}


