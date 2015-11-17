/*
 * tcp_connection.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TCP_CONNECTION_H_
#define TCP_CONNECTION_H_

#include "tcp_queue.h"
#include "tcp_socket.h"

#define TCPCONNECTION_BUFLEN (8192-8*128)

enum enum_tcpstates
{
	TCPSTATE_CONNECTING = 0,
	TCPSTATE_NOTHNG = 1,
	TCPSTATE_SEND = 2,
};

//~8k
struct tcp_connection
{
	struct double_list_node _listnode;
	tcp_protocol* _protocol;
	create_protocol _pcreate_f;
	destory_protocol _pdestory_f;
	tcpid_t _id;
	int _socket;
	uint32_t _active_time;

	uint16_t _type;
	uint16_t _state;
	uint16_t _offset;
	uint16_t _temp;

	struct tcp_queue _sendqueue;
	union tcp_connection_expand _expands[16];
	char _buf[TCPCONNECTION_BUFLEN];
};


void tcpconnection_init(struct tcp_connection* c,int s,int state,uint32_t ip,int port,
		int type,tcp_protocol* p,create_protocol createf,destory_protocol destoryf);

void tcpconnection_do_sendmsg(struct tcp_connection* c,handler_msg* msg);
void tcpconnection_add_msg(struct tcp_connection* c,handler_msg* msg);

void tcpconnection_recv(struct tcp_connection* c);

void tcpconnection_send(struct tcp_connection* c);

static inline int tcpconnection_is_empty(struct tcp_connection* c)
{
	return c->_sendqueue._count == 0;
}
static inline int tcpconnection_is_timeout(struct tcp_connection* c,uint32_t t)
{
	return c->_active_time < t;
}

void tcpconnection_close(struct tcp_connection* c,const char* cause);

void tcpconnection_free(struct tcp_connection* c);


#endif /* TCP_CONNECTION_H_ */
