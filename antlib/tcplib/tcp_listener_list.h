/*
 * tcp_listener_list.h
 *
 *  Created on: Feb 15, 2015
 *      Author: zhaolixiang
 */

#ifndef TCP_LISTENER_LIST_H_
#define TCP_LISTENER_LIST_H_

#include "tcp_listener.h"


struct tcp_listener_list
{
	struct tcp_listener* _head;
};


void init_tcplistener_list(struct tcp_listener_list* l);


void uninit_tcplistener_list(struct tcp_listener_list* l);


struct tcp_listener* add_tcplistener(struct tcp_listener_list* list,const char* ip,int port,create_protocol createf,destory_protocol destoryf);


static inline void tcp_listeners_run(struct tcp_listener_list* list,struct tcp_connection_list* c)
{
	struct tcp_listener* node = list->_head;
	while(node)
	{
		tcp_accepter_running(node,c);
		node = node->_next;
	}
}


#endif /* TCP_LISTENER_LIST_H_ */
