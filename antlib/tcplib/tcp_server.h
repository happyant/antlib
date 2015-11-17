/*
 * tcp_server.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_


#include "tcp_connection_list.h"
#include "tcp_listener_list.h"


typedef struct tcp_server
{
	uint32_t _handlerid;
	struct tcp_connection_list _connections;
	struct tcp_listener_list* _listeners;
}tcp_server;


static inline void create_tcpserver(tcp_server* s,struct tcp_listener_list* l,uint16_t serverid)
{
	memset(s,0,sizeof(*s));
	s->_listeners = l;
	init_tcpconnection_list(&s->_connections,serverid,TCP_CONNECTION_TIMEOUT);
}

static void distory_tcpserver(tcp_server* s)
{
	uinit_tcpconnection_list(&s->_connections);
}

//////////////////////////////////////////////////////////////////////////////////

static void do_tcpserver_msg(handler_msg* msg,void* hdata,uint32_t handlerid)
{
	tcp_server* s = (tcp_server*)hdata;
	if(msg->_userid > 0) {
		do_tcpconnection_msg(&s->_connections,msg->_userid,msg);
	} else {
		ASSERT(msg->_msgid == TCPLIB_SYSMSGID_CONNECT);
		struct tcp_connect_data* data = (struct tcp_connect_data*)msg->_data;
		add_tcpconnecting(&s->_connections,data->_ip,data->_port,data->_p,msg,data->_createf,data->_destoryf);
	}
}

static void do_tcpserver(void* hdata,uint32_t handlerid)
{
	tcp_server* s = (tcp_server*)hdata;
	tcp_listeners_run(s->_listeners,&s->_connections);
	tcp_connectionlist_run(&s->_connections);
}


#endif /* TCP_SERVER_H_ */
