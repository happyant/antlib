/*
 * hqueue.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef HQUEUE_H_
#define HQUEUE_H_

#include "squeue.h"


//used in handler,must only one thread pop at one time.
static inline void* handler_pop_squeue(struct squeue* qu)
{
	ASSERT(qu->_count > 0);
	void* vl;
	int index = pop_squeuenode(qu->_head,&vl);
	if(index == SQUEUE_NODE_SIZE-1)
	{
		struct squeue_node* node = qu->_head;
		while(node == qu->_tail){}
		qu->_head = qu->_head->_next;
		free_squeue_node(node);
	}
	//just only one thread pops at one time,so is not "index >= SQUEUE_NODE_SIZE".
	LOCK_SUB(qu->_count,1);
	return vl;
}



#endif /* HQUEUE_H_ */
