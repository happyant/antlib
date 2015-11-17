/*
 * tcp_protocol4.c
 *
 *  Created on: May 24, 2015
 *      Author: zhaolixiang
 */

#include "tcp_protocol4.h"
#include "tcp_protocol.h"
#include "tcp_stream.h"
#include "tcp_msglist.h"

int _parse_tcp4msg(struct tcp_connection* c)
{
	handler_msg* msg = (handler_msg*)TPD_MSG;
	if(msg == NULL)
	{
		//parse head
		if(TPD_MSGBEGIN+4 > c->_offset)
		{
			return -1;
		}
		char* buf = c->_buf + TPD_MSGBEGIN;
		uint16_t msgid = ntoh16(buf);
		uint16_t msglen = ntoh16(buf+2);
		//check message
		msg = tcpmsg_getbyid(msgid,msglen,TPD_USERID,c->_id._i64,TPD_IP);
		if(msg == NULL)
		{
			tcpconnection_close(c,"tcp4 error message");
			return -1;
		}
		TPD_MSGBEGIN += 4;
		//put out ,if only message head,
		if(msglen == 0)
		{
			if(-2 == tcpmsg_putout_range(msg,NULL,0))
			{
				tcpconnection_close(c,"tcp4 parser error");
				return -1;
			}
			return 0;
		}
		//record
		TPD_MSG = msg;
	}
	//add message body range
	if(msg && c->_offset > TPD_MSGBEGIN)
	{
		char* buf = c->_buf + TPD_MSGBEGIN;
		int n = tcpmsg_putout_range(msg,buf,c->_offset-TPD_MSGBEGIN);
		if(n >= 0)
		{
			TPD_MSG = NULL;
			TPD_MSGBEGIN += n;
			return 0;
		}
		else if(n == -2)
		{
			tcpconnection_close(c,"tcp4 parser error2");
			return -1;
		}
		TPD_MSGBEGIN = c->_offset;
	}
	return -1;
}

///////////////////////////////////////////////////////////////

void tcp4_readed_data(int add,struct tcp_connection* c)
{
	c->_offset += add;
	while(_parse_tcp4msg(c) == 0)
	{ }
	tcp_resetbuf(c,TPD_MSGBEGIN,c->_offset);
}

void tcp4_do_msg(handler_msg* msg,struct tcp_connection* c)
{
	if(tcp_domsg(msg,c) == 0)
	{
		return;
	}
	tcp_stream* s = (tcp_stream*)msg->_data;
	hton16(s->_head+12,msg->_msgid);
	hton16(s->_head+14,s->_len);
	msg->_datalen = s->_len +4;
	msg->_data = s->_head + 12;
	tcpconnection_add_msg(c,msg);
}


////////////////////////////////////////////////////////////////
void tcp4_protocol_init(tcp_protocol*p)
{
	p->_protocol_name = "tcp4";
	p->_connected = tcp_connected;
	p->_readed_data = tcp4_readed_data;
	p->_do_msg = tcp4_do_msg;
	p->_sended_msg = tcp_sended_msg;
	p->_closed = tcp_closed;
}
