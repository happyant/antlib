/*
 * double_list.h
 *
 *  Created on: Feb 12, 2015
 *      Author: zhaolixiang
 */

#ifndef DOUBLE_LIST_H_
#define DOUBLE_LIST_H_

#include "sys_header.h"


struct double_list_node
{
	struct double_list_node* _next;
	struct double_list_node* _prev;
};

struct double_list
{
	struct double_list_node* _head;
	struct double_list_node* _tail;
};


static inline void double_list_add_head(struct double_list* list,struct double_list_node* node)
{
	node->_prev = NULL;
	node->_next = list->_head;
	if(list->_head)
	{
		list->_head->_prev = node;
	}
	else
	{
		ASSERT(list->_tail == NULL);
		list->_tail = node;
	}
	list->_head = node;
}

static inline void double_list_add_tail(struct double_list* list,struct double_list_node* node)
{
	node->_next = NULL;
	node->_prev = list->_tail;
	if(list->_tail)
	{
		list->_tail->_next = node;
	}
	else
	{
		ASSERT(list->_head == NULL);
		list->_head = node;
	}
	list->_tail = node;
}

static inline void double_list_add_back(struct double_list* list,struct double_list_node* prev,struct double_list_node* node)
{
	if(prev == NULL)
	{
		double_list_add_head(list,node);
	}
	else if(prev == list->_tail)
	{
		double_list_add_tail(list,node);
	}
	else
	{
		node->_prev = prev;
		node->_next = prev->_next;
		prev->_next->_prev = node;
		prev->_next = node;
	}
}

static inline void double_list_add_front(struct double_list* list,struct double_list_node* next,struct double_list_node* node)
{
	if(next == NULL)
	{
		double_list_add_tail(list,node);
	}
	else if(next == list->_head)
	{
		double_list_add_head(list,node);
	}
	else
	{
		node->_next = next;
		node->_prev = next->_prev;
		next->_prev->_next = node;
		next->_prev = node;
	}
}

static inline void double_list_remove(struct double_list* list,struct double_list_node* node)
{
	if(node->_next)
	{
		ASSERT(node->_next->_prev == node);
		node->_next->_prev = node->_prev;
	}
	else
	{
		ASSERT(list->_tail == node);
		list->_tail = node->_prev;
	}
	if(node->_prev)
	{
		ASSERT(node->_prev->_next == node);
		node->_prev->_next = node->_next;
	}
	else
	{
		ASSERT(list->_head == node);
		list->_head = node->_next;
	}
}

static inline struct double_list_node* double_list_pop(struct double_list* list)
{
	struct double_list_node* node = list->_head;
	if(node)
	{
		double_list_remove(list,node);
	}
	return node;
}

static inline void double_list_push(struct double_list* list,struct double_list_node* node)
{
	double_list_add_tail(list,node);
}


#endif /* DOUBLE_LIST_H_ */
