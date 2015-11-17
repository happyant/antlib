/*
 * tcp_listener.c
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#include "tcp_listener.h"


struct tcp_listener* create_tcplistener(const char* ip,int port)
{
	struct tcp_listener* l;
	int s = tcp_listen(ip,port,16);
	if(s == ERROR_FD)
		return NULL;
	l = (struct tcp_listener*)my_malloc(sizeof(struct tcp_listener));
	memset(l,0,sizeof(*l));
	l->_port = port;
	l->_socket = s;
	return l;
}

void destory_tcplistener(struct tcp_listener* l)
{
	struct tcp_accepter_node* node;
	struct tcp_accepter_node* remove;
	tcp_close(&l->_socket);
	if(l->_list)
	{
		node = l->_list->_next;
		l->_list->_next = NULL;
		while(node)
		{
			remove = node;
			node = node->_next;
			my_free(remove);
		}
	}
	my_free(l);
}


void add_tcp_accepter(struct tcp_listener* l,struct tcp_connection_list* a)
{
	struct tcp_accepter_node* node = (struct tcp_accepter_node*)my_malloc(sizeof(struct tcp_accepter_node));
	node->_accepter = a;
	//add into role list
	if(l->_list)
	{
		l->_list->_next = node;
	}
	else
	{
		l->_index = node;
	}
	l->_list = node;

	node->_next = l->_index;
}
