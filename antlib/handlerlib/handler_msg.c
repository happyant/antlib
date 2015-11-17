/*
 * handler_msg.c
 *
 *  Created on: Sep 6, 2015
 *      Author: zhaolixiang
 */
#include "handler_msg.h"
#include "commandlib/squeue.h"

struct handler_msg_cache
{
	handler_msg _msg;
	int	_used;
	int _size;
};


///////////////////////////////////////////////////////////////////////////
struct handler_msg_pool
{
	struct squeue _qu;
	int _max;
	int _size;
};

struct handler_msg_pool* _g_handlermsg_chachepool = NULL;

void create_handler_msg_cachepool(int msgsize,int poolsize)
{
	ASSERT(_g_handlermsg_chachepool == NULL);
	_g_handlermsg_chachepool = my_new(struct handler_msg_pool);
	init_squeue(&_g_handlermsg_chachepool->_qu);
	_g_handlermsg_chachepool->_max = poolsize;
	_g_handlermsg_chachepool->_size = msgsize;
}

void destory_handler_msg_cachepool()
{
	if(_g_handlermsg_chachepool == NULL)
		return;
	struct handler_msg_pool* pool = _g_handlermsg_chachepool;
	_g_handlermsg_chachepool = NULL;
	void *cache = pop_squeue(&pool->_qu);
	while(cache)
	{
		my_free(cache);
		cache = pop_squeue(&pool->_qu);
	}
	my_free(pool);
}

#define CREATE_HANDLERMSG_CACHE(cache,size) \
	do{	cache = (struct handler_msg_cache*)my_malloc(sizeof(struct handler_msg_cache)+size);\
		cache->_size = size;\
	}while(0)

inline struct handler_msg_cache* _create_msgcache(int size)
{
	struct handler_msg_cache* cache;
	if(_g_handlermsg_chachepool && size <= _g_handlermsg_chachepool->_size)
	{
		cache = (struct handler_msg_cache*)pop_squeue(&_g_handlermsg_chachepool->_qu);
		if(NULL == cache){
			CREATE_HANDLERMSG_CACHE(cache,size);
		}
	} else {
		CREATE_HANDLERMSG_CACHE(cache,size);
	}
	ASSERT(cache);
	return cache;
}

inline void _destory_msgcache(struct handler_msg_cache* cache)
{
	if(_g_handlermsg_chachepool
	 && _g_handlermsg_chachepool->_qu._count < _g_handlermsg_chachepool->_max
	 && cache->_size == _g_handlermsg_chachepool->_size)
	{
		push_squeue(&_g_handlermsg_chachepool->_qu,cache);
	}
	else
	{
		my_free(cache);
	}
}

///////////////////////////////////////////////////////////////////////////

handler_msg* create_handler_msg(int size)
{
	struct handler_msg_cache* cache = _create_msgcache(size);
	cache->_msg._data = (size > 0)?(cache+1):NULL;
	cache->_used = 0;
	return &cache->_msg;
}

void free_handler_msg(handler_msg* msg)
{
	int n = LOCK_SUB(((struct handler_msg_cache*)msg)->_used,1);
	ASSERT(n >= 0);
	if(n <= 0) {
		_destory_msgcache((struct handler_msg_cache*)msg);
	}
}

void add_handler_msg_used(handler_msg* msg,int add)
{
	LOCK_ADD(((struct handler_msg_cache*)msg)->_used,add);
}

void reset_handler_msg_data(handler_msg* msg)
{
	struct handler_msg_cache* cache = (struct handler_msg_cache*)msg;
	cache->_msg._data = (cache->_size > 0)?(cache+1):NULL;
}



