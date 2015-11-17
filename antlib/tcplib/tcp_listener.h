/*
 * tcp_listener.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TCP_LISTENER_H_
#define TCP_LISTENER_H_

#include "tcp_connection_list.h"
#include "tcp_socket.h"

struct tcp_accepter_node
{
	struct tcp_connection_list* _accepter;
	struct tcp_accepter_node* _next;
};


struct tcp_listener
{
	struct tcp_listener* _next;
	create_protocol _create_f;
	destory_protocol _destory_f;

	int _socket;
	int _port;
	//role list
	struct tcp_accepter_node* _index;
	struct tcp_accepter_node* _list;
};


struct tcp_listener* create_tcplistener(const char* ip,int port);

void destory_tcplistener(struct tcp_listener* l);

void add_tcp_accepter(struct tcp_listener* l,struct tcp_connection_list* a);

static inline void tcp_accepter_running(struct tcp_listener* l,struct tcp_connection_list* ap)
{
	if(ap == l->_index->_accepter)
	{
		uint32_t ip;
		int port,s;
		while(tcpconnection_list_count(ap) < TCPLIST_MAX)
		{
			s = tcp_accept(l->_socket,&ip,&port);
			if(s == ERROR_FD)
				break;
			add_tcpconnection(ap,s,ip,port,l->_create_f,l->_destory_f);
		}
		l->_index = l->_index->_next;
	}
}


#endif /* TCP_LISTENER_H_ */
