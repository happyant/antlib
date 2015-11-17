/*
 * tcp_protocol.h
 *
 *  Created on: Oct 21, 2015
 *      Author: zhaolixiang
 */

#ifndef TCP_PROTOCOL_H_
#define TCP_PROTOCOL_H_

#include "tcp_protocol_header.h"


enum tcpprotocol_type
{
	TCPPROTOCOL_HTTP = 0x01,
	TCPPROTOCOL_FLASH = 0x02,
	TCPPROTOCOL_TCP4 = 	0x04,
	TCPPROTOCOL_TCP12 = 0x08,
};


#define TCPMSG_SENDOUT 0
#define TCPCONNECT_MSGID 1
#define TCPCONNECT_LOST_MSGID 2


void tcp_connected(int result,struct tcp_connection* c,handler_msg* msg);
void tcp_sended_msg(handler_msg* msg,struct tcp_connection* c);
void tcp_closed(struct tcp_connection* c);
int tcp_domsg(handler_msg* msg,struct tcp_connection* c);

void tcp_resetbuf(struct tcp_connection* c,int begin,int end);



#endif /* TCP_PROTOCOL_H_ */
