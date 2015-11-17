/*
 * tcp_protocol.c
 *
 *  Created on: Oct 21, 2015
 *      Author: zhaolixiang
 */
#include "tcp_protocol.h"
#include "protocollib.h"

void tcp_connected(int result,struct tcp_connection* c,handler_msg* msg)
{
	TPD_USERID = 0;
	TPD_MSG = NULL;
	if(result == -1)
	{
		ASSERT(msg);
		struct tcp_connect_data* data = (struct tcp_connect_data*)msg->_data;
		if(data->_user_handlerid > 0) {
			msg->_userid = data->_user_id;
			msg->_msgid = TCPCONNECT_LOST_MSGID;
			send_handler_msg(data->_user_handlerid,msg);
		} else {
			free_handler_msg(msg);
		}
		return;
	}
	else if(msg)
	{
		struct tcp_connect_data* data = (struct tcp_connect_data*)msg->_data;
		msg->_datalen = data->_msglen;
		msg->_data = data->_msg;
		tcpconnection_add_msg(c,msg);
	}
	TPD_MSGBEGIN = 0;
	TPD_MSGOFF = 0;
	//TODO: LOG_INFO("[net]connected,id=%lu,ip=%u:%d,result=%d",c->_id._i64,TPD_IP,TPD_PORT,result);
}

void tcp_sended_msg(handler_msg* msg,struct tcp_connection* c)
{
	free_handler_msg(msg);
}

void tcp_closed(struct tcp_connection* c)
{
	handler_msg* msg = TPD_MSG;
	if(msg)
	{
		free_handler_msg(msg);
	}
	TPD_MSG = NULL;
	//TODO: LOG_INFO("[net]closed,id=%lu,cause=%s",c->_id._i64,TPD_CLOSECAUSE?TPD_CLOSECAUSE:"unknown");
}

int tcp_domsg(handler_msg* msg,struct tcp_connection* c)
{
	switch(msg->_msgid)
	{
	case SYSMSG_CLOSE_CONNECTION:
		tcpconnection_close(c,(const char*)msg->_data);
		return 0;
	case SYSMSG_SETUSERID:
		TPD_USERID = *((uint64_t*)msg->_data);
		return 0;
	}
	return -1;
}


void tcp_resetbuf(struct tcp_connection* c,int begin,int end)
{
	ASSERT(end >= begin && end < TCPCONNECTION_BUFLEN);
	if(end > begin)
	{
		memmove(c->_buf,c->_buf+begin,end-begin);
		c->_offset = end-begin;
	}
	else
	{
		c->_offset = 0;
	}
}


