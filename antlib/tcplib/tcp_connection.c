/*
 * tcp_connection.c
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#include "tcp_connection.h"
#include "tcp_epoll.h"
#include "tcp_socket.h"


void tcpconnection_init(struct tcp_connection* c,int s,int state,uint32_t ip,int port,
		int type,tcp_protocol* p,create_protocol createf,destory_protocol destoryf)
{
	static uint32_t g_mainid = 0;
	c->_id._st._index = LOCK_ADD(g_mainid,1);
	c->_id._st._mask = TCPID_MASK;

	c->_pcreate_f = createf;
	c->_pdestory_f = destoryf;
	c->_protocol = p;

	c->_active_time = get_mytime();
	c->_socket = s;
	c->_type = (uint16_t)type;
	c->_state = (uint16_t)state;
	c->_offset = 0;
	reset_tcpqueue(&c->_sendqueue);
	c->_expands[0]._e32[0] = ip;
	c->_expands[0]._e32[1] = port;
}

void tcpconnection_do_sendmsg(struct tcp_connection* c,handler_msg* msg)
{
	ASSERT(c->_protocol);
	c->_protocol->_do_msg(msg,c);
}


void tcpconnection_add_msg(struct tcp_connection* c,handler_msg* msg)
{
	if(c->_type == 0 && c->_sendqueue._count >= TCPQUEUE_NODE)
	{
		tcpconnection_close(c,"by send queue full");
		free_handler_msg(msg);
	}
	else
	{
		msg->_userid = 0; //used for send length
		push_tcpqueue(&c->_sendqueue,msg);
		//
		add_handler_msg_used(msg,1);
	}
}


void tcpconnection_recv(struct tcp_connection* c)
{
	if(c->_socket == ERROR_FD)
		return;

	int n = tcp_read(&c->_socket,c->_buf,TCPCONNECTION_BUFLEN);
	if(NULL == c->_protocol)
	{
		if(n < 4)
		{
			tcpconnection_close(c,"first receive error");
			return;
		}
		c->_protocol = c->_pcreate_f(c->_buf);
		if(NULL == c->_protocol)
		{
			tcpconnection_close(c,"set protocol error");
			return;
		}
		else
		{
			c->_protocol->_connected(0,c,NULL);
		}
	}
	while(n > 0 && c->_socket != ERROR_FD)
	{
		c->_protocol->_readed_data(n,c);
		if(c->_socket != ERROR_FD)
		{
			ASSERT(c->_offset < TCPCONNECTION_BUFLEN);
			n = tcp_read(&c->_socket,c->_buf+c->_offset,TCPCONNECTION_BUFLEN-c->_offset);
		}
	}
	if(c->_socket == ERROR_FD)
	{
		tcpconnection_close(c,"receive error");
	}
	else
	{
		c->_active_time = get_mytime();
	}
}

int _tcpconnection_send_msg(struct tcp_connection* c)
{
	if(c->_sendqueue._size == 0) {
		return 0;
	}
	int len,off;
	handler_msg* msg = (handler_msg*)tcpqueue_head(&c->_sendqueue);
	while(msg && c->_socket != ERROR_FD)
	{
		off = (int)(msg->_userid);//used for send length
		len = tcp_write(&c->_socket,((char*)msg->_data)+off,msg->_datalen-off);

		if(c->_socket == ERROR_FD) {
			tcpconnection_close(c,"send net error");
			return 0;
		}
		if(len == 0 ) {
			break;
		}
		if(len + off == msg->_datalen)
		{
			c->_protocol->_sended_msg(msg,c);
			pop_tcpqueue(&c->_sendqueue);
			msg = (handler_msg*)tcpqueue_head(&c->_sendqueue);
		}
		else
		{
			msg->_userid += len;//add send length
			break;
		}
	}
	return 1;
}

void tcpconnection_send(struct tcp_connection* c)
{
	if(c->_socket == ERROR_FD)
		return;
	if(c->_state == TCPSTATE_CONNECTING)
	{
		ASSERT(c->_sendqueue._count > 0);
		handler_msg* msg = (handler_msg*)tcpqueue_head(&c->_sendqueue);
		pop_tcpqueue(&c->_sendqueue);
		if(0 != tcp_connected_check(&c->_socket))
		{
			c->_protocol->_connected(-1,c,msg);
			tcpconnection_close(c,"connect lost");
			return;
		}
		c->_protocol->_connected(0,c,msg);
		c->_state = TCPSTATE_NOTHNG;
		c->_active_time = get_mytime();
	}
	if(_tcpconnection_send_msg(c) > 0)
	{
		c->_active_time = get_mytime();
	}
}


void tcpconnection_close(struct tcp_connection* c,const char* cause)
{
	tcp_close(&(c->_socket));
	c->_expands[15]._ptr = (void*)cause;
}

void tcpconnection_free(struct tcp_connection* c)
{
	ASSERT(c->_socket == -1);
	if(c->_protocol)
	{
		c->_protocol->_closed(c);
		if(c->_pdestory_f){
			c->_pdestory_f(c->_protocol);
		}
		c->_protocol = NULL;
	}
	handler_msg* msg = (handler_msg*)tcpqueue_head(&c->_sendqueue);
	while(msg)
	{
		free_handler_msg(msg);
		pop_tcpqueue(&c->_sendqueue);
		msg = (handler_msg*)tcpqueue_head(&c->_sendqueue);
	}
	c->_id._st._mask = 0;
	if(c->_type != 0)
	{
		uninit_tcpqueue(&c->_sendqueue);
	}
}
