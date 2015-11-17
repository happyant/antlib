/*
 * squeue.h
 *
 *  Created on: Feb 16, 2015
 *      Author: zhaolixiang
 */

#ifndef SQUEUE_H_
#define SQUEUE_H_

#include "sys_header.h"
#include "locker.h"
#include "alloc.h"

#define SQUEUE_NODE_SIZE 1021

// 8k
struct squeue_node
{
	void* _queue[SQUEUE_NODE_SIZE];
	int _head;
	int _tail;
	uint32_t _free_time;
	struct squeue_node* _next;
};


void init_squeue_node_pool();

void uninit_squeue_node_pool();

void free_squeue_node(struct squeue_node* node);

struct squeue_node* get_squeue_node();

static inline int push_squeuenode(struct squeue_node* node,void* vl)
{
	int index = LOCK_ADD(node->_head,1);
	if(index < SQUEUE_NODE_SIZE)
	{
		node->_queue[index] = vl;
	}
	return index;
}

static inline int pop_squeuenode(struct squeue_node* node,void** vl)
{
	int index = LOCK_ADD(node->_tail,1);
	if(index < SQUEUE_NODE_SIZE)
	{
		while(NULL == node->_queue[index]){/*sched_yield();*/}
		*vl = node->_queue[index];
	}
	return index;
}


/////////////////////////////////////////////////////////


struct squeue
{
	int _count;
	struct squeue_node* _free_ptr;
	struct squeue_node* _tail;
	struct squeue_node* _head;
};


static inline void init_squeue(struct squeue* qu)
{
	memset(qu,0,sizeof(*qu));
	qu->_head = get_squeue_node();
	qu->_tail = qu->_head;
	qu->_free_ptr = qu->_head;
}

static inline void uninit_squeue(struct squeue* qu)
{
	struct squeue_node* node;
	while(qu->_head)
	{
		node = qu->_head;
		qu->_head = qu->_head->_next;
		my_free(node);
	}
	memset(qu,0,sizeof(*qu));
}


static inline void push_squeue(struct squeue* qu,void* vl)
{
	int index;
	assert(vl);

	do{	index  = push_squeuenode(qu->_tail,vl);
		if(index < SQUEUE_NODE_SIZE-1)
		{
			break;
		}
		else if(index == SQUEUE_NODE_SIZE-1)
		{
			qu->_tail->_next = get_squeue_node();
			qu->_tail = qu->_tail->_next;
			break;
		}
	}while(true);

	LOCK_ADD(qu->_count,1);
}


static inline void* pop_squeue(struct squeue* qu)
{
	void* vl;
	int index;
	if(qu->_count < 1)
	{
		return NULL;
	}
	if(LOCK_SUB(qu->_count,1) < 1)
	{
		LOCK_ADD(qu->_count,1);
		return NULL;
	}

	do{	index = pop_squeuenode(qu->_head,&vl);
		if(index < SQUEUE_NODE_SIZE-1)
		{
			break;
		}
		else if(index == SQUEUE_NODE_SIZE-1)
		{
			struct squeue_node* rm;
			while(qu->_free_ptr != qu->_head)
			{
				rm = qu->_free_ptr;
				qu->_free_ptr = qu->_free_ptr->_next;
				free_squeue_node(rm);
			}
			while(qu->_head == qu->_tail){}
			qu->_head = qu->_head->_next;
			break;
		}
	}while(true);

	LOCK_SUB(qu->_count,1);
	return vl;
}


#endif /* SQUEUE_H_ */
