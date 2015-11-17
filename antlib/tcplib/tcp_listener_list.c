/*
 * tcp_listener_list.c
 *
 *  Created on: Feb 15, 2015
 *      Author: zhaolixiang
 */
#include "tcp_listener_list.h"


void init_tcplistener_list(struct tcp_listener_list* l)
{
	l->_head = NULL;
}

void uninit_tcplistener_list(struct tcp_listener_list* l)
{
	struct tcp_listener* node;
	while(l->_head)
	{
		node = l->_head;
		l->_head = l->_head->_next;
		destory_tcplistener(node);
	}
}

struct tcp_listener* add_tcplistener(struct tcp_listener_list* list,const char* ip,int port,create_protocol createf,destory_protocol destoryf)
{
	struct tcp_listener* l = create_tcplistener(ip,port);
	if(l)
	{
		l->_create_f = createf;
		l->_destory_f = destoryf;
		l->_next = list->_head;
		list->_head = l;
	}
	return l;
}

