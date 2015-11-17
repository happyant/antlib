/*
 * squeue.c
 *
 *  Created on: Feb 16, 2015
 *      Author: zhaolixiang
 */

#include "squeue.h"
#include "my_time.h"
#include "alloc.h"
#include "locker.h"


struct squeue_node_pool
{
	struct squeue_node* _head;
	struct squeue_node* _tail;
	LOCKER _locker;
	int _count;
};


struct squeue_node_pool _g_squeue_pool = {NULL,NULL,{0},0};


void init_squeue_node_pool()
{

}

void uninit_squeue_node_pool()
{
	struct squeue_node* node;
	WRITELOCK(_g_squeue_pool._locker);
	while(_g_squeue_pool._head)
	{
		node = _g_squeue_pool._head;
		_g_squeue_pool._head = _g_squeue_pool._head->_next;
		my_free(node);
	}
	_g_squeue_pool._count = 0;
	WRITEUNLOCK(_g_squeue_pool._locker);
}



void free_squeue_node(struct squeue_node* node)
{
	node->_next = NULL;
	node->_free_time = get_mytime()+60;

	WRITELOCK(_g_squeue_pool._locker);
	if(_g_squeue_pool._tail)
	{
		_g_squeue_pool._tail->_next = node;
	}
	else
	{
		_g_squeue_pool._head = node;
	}
	_g_squeue_pool._tail = node;
	++ _g_squeue_pool._count;
	WRITEUNLOCK(_g_squeue_pool._locker);
}

struct squeue_node* get_squeue_node()
{
	struct squeue_node* node = NULL;

	WRITELOCK(_g_squeue_pool._locker);
	if(_g_squeue_pool._head && _g_squeue_pool._head->_free_time < get_mytime())
	{
		node = _g_squeue_pool._head;
		_g_squeue_pool._head = _g_squeue_pool._head->_next;
		-- _g_squeue_pool._count;
	}
	WRITEUNLOCK(_g_squeue_pool._locker);

	if(NULL == node)
	{
		node = my_new(struct squeue_node);
	}
	memset(node,0,sizeof(*node));
	return node;
}

