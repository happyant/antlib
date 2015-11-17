/*
 * tcp_queue.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TCP_QUEUE_H_
#define TCP_QUEUE_H_

#include "tcp_header.h"


#define TCPQUEUE_NODE 128


struct tcp_queue {
	int _size;
	int _head;
	int _tail;
	int _count;
	void** _queue;
};


static inline void init_tcpqueue(struct tcp_queue* qu)
{
	qu->_size = 0;
	qu->_head = 0;
	qu->_tail = 0;
	qu->_count = 0;
	qu->_queue = NULL;
}

static inline void uninit_tcpqueue(struct tcp_queue* qu)
{
	if(qu->_queue)
	{
		my_free(qu->_queue);
	}
	memset(qu,0,sizeof(struct tcp_queue));
}

static inline void reset_tcpqueue(struct tcp_queue* qu)
{
	qu->_count = 0;
	qu->_head = 0;
	qu->_tail = 0;
	if(qu->_size > TCPQUEUE_NODE)
	{
		ASSERT(qu->_queue);
		my_free(qu->_queue);
		qu->_queue = NULL;
		qu->_size = 0;
	}
	if(NULL == qu->_queue)
	{
		qu->_queue = (void**)my_malloc(sizeof(void*)*TCPQUEUE_NODE);
		qu->_size = TCPQUEUE_NODE;
	}
}

static inline bool is_tcpqueue_empty(struct tcp_queue* qu)
{
	return qu->_count == 0;
}

static inline void push_tcpqueue(struct tcp_queue* qu,void* vl)
{
	ASSERT(qu->_queue && qu->_count < qu->_size);

	int i = qu->_tail++;
	qu->_queue[i] = vl;
	if(++ qu->_count >= qu->_size)
	{
		int n = qu->_size*2;
		ASSERT(qu->_head == qu->_tail);
		ASSERT(n < 1024*1024);
		void** newqu = (void**)my_malloc(sizeof(void*)*n);
		if(qu->_head == 0)
		{
			memcpy(newqu,qu->_queue,sizeof(void*)*qu->_size);
		}
		else
		{
			i = qu->_size-qu->_head;//tail length
			memcpy(newqu,&qu->_queue[qu->_head],sizeof(void*)*i);
			memcpy(&newqu[i],qu->_queue,sizeof(void*)*qu->_head);
		}
		qu->_head = 0;
		qu->_tail = qu->_size;
		qu->_size = n;
		my_free(qu->_queue);
		qu->_queue = newqu;
	}
	if(qu->_tail == qu->_size)
	{
		qu->_tail = 0;
	}
}

static inline void pop_tcpqueue(struct tcp_queue* qu)
{
	if(++qu->_head == TCPQUEUE_NODE)
	{
		qu->_head = 0;
	}
	--qu->_count;
}

static inline void* tcpqueue_head(struct tcp_queue* qu)
{
	return qu->_queue[qu->_head];
}


#endif /* TCP_QUEUE_H_ */
