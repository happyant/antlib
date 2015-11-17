/*
 * thread_data.h
 *
 *  Created on: 2015-2-9
 *      Author: lx
 */

#ifndef THREAD_DATA_H_
#define THREAD_DATA_H_

#include "commandlib/command_header.h"


#define MAXTHREAD 1024

struct thread_data
{
	const char* _name;
	void* _error;
	unsigned int _thid;
	unsigned int _handlerid;
};

struct thread_data_list
{
	pthread_key_t _key;
	unsigned int _index;
	struct thread_data _array[MAXTHREAD];
};


void create_thread_date_list();

void destory_thread_data_list();


struct thread_data* get_thread_data();


static inline unsigned int get_handlerid()
{
	return get_thread_data()->_handlerid;
}

static inline void set_handlerid(unsigned int hid)
{
	get_thread_data()->_handlerid = hid;
}


static inline unsigned int get_threadid()
{
	return get_thread_data()->_thid;
}

static inline const char* get_threadname()
{
	return get_thread_data()->_name;
}

static inline void set_threadname(const char* name)
{
	get_thread_data()->_name = name;
}


static inline void* get_error()
{
	return get_thread_data()->_error;
}

static inline void set_error(void* e)
{
	get_thread_data()->_error = e;
}


#endif /* THREAD_DATA_H_ */
