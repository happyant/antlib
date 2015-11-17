/*
 * thread_proc.h
 *
 *  Created on: 2015-2-10
 *      Author: lx
 */

#ifndef THREAD_PROC_H_
#define THREAD_PROC_H_

#include "handler.h"


static inline void handlers_list_thread_proc(void* data)
{
	uint32_t n,i;
	struct handler* hl;
	struct handlers_list* list = (struct handlers_list*)data;
	n  = 0;
	while(++n < 8)
	{
		i = LOCK_ADD(list->_index,1)%list->_size;
		hl = list->_handlers[i];
		if(LOCK_SET(hl->_locker,1) == 0)
		{
			do_handler(hl);
			LOCK_ZERO(hl->_locker);
		}
	}
}


static inline void thread_proc_handler(void* data)
{
	do_handler((struct handler*)data);
}


#endif /* THREAD_PROC_H_ */
