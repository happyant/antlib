/*
 * tcp_protocol_http.c
 *
 *  Created on: May 24, 2015
 *      Author: zhaolixiang
 */

#include "tcp_protocol_http.h"
#include "http_protocol.h"
#include "tcp_protocol.h"
#include "tcp_msglist.h"

int _http_readed_get(struct tcp_connection* c)
{
	ASSERT(TPD_MSG == NULL);
	int result = parse_http_header_len(c->_buf+TPD_MSGBEGIN,c->_offset-TPD_MSGBEGIN,&TPD_MSGOFF);
	if(result == -1 || TPD_MSGOFF >= TCPCONNECTION_BUFLEN)
	{
		return -1;
	}
	if(result == 0)
	{
		return 0;
	}
	char* path1=NULL;char* path2=NULL;char* values="";
	result = parse_http_get(c->_buf+TPD_MSGBEGIN,(int)TPD_MSGOFF,&path1,&path2,&values);
	if(result == -1)
	{
		return -1;
	}
	handler_msg* msg = tcpmsg_getbykey((const char*)path1,(const char*)path2,result,TPD_USERID,c->_id._i64,TPD_IP);
	if(msg == NULL)
	{
		return -1;
	}
	if(-2 == tcpmsg_putout_range(msg,values,result))
	{
		free_handler_msg(msg);
		return -1;
	}
	TPD_MSGBEGIN += TPD_MSGOFF;
	TPD_MSGOFF = 0;
	return 1;
}

int _htt_readed_post(struct tcp_connection* c)
{
	int result;
	handler_msg* msg = (handler_msg*)TPD_MSG;
	if(NULL == msg)
	{
		result = parse_http_header_len(c->_buf+TPD_MSGBEGIN,c->_offset-TPD_MSGBEGIN,&TPD_MSGOFF);
		if(result == -1 || TPD_MSGOFF >= TCPCONNECTION_BUFLEN)
		{
			return -1;
		}
		if(result == 0)
		{
			return 0;
		}
		char* path1 = NULL; char* path2 = NULL; int64_t len = 0;
		int result = parse_http_post(c->_buf+TPD_MSGBEGIN,(int)TPD_MSGOFF,&path1,&path2,&len);
		if(result == -1 || len >= 0x7FFFFFFF)//max size is 2G
		{
			return -1;
		}
		msg = tcpmsg_getbykey(path1,path2,(int)len,TPD_USERID,c->_id._i64,TPD_IP);
		if(msg == NULL)
		{
			return -1;
		}
		TPD_MSGBEGIN += TPD_MSGOFF;
		TPD_MSGOFF = 0;
		TPD_MSG = msg;
	}
	result = tcpmsg_putout_range(msg,c->_buf+TPD_MSGBEGIN,c->_offset-TPD_MSGBEGIN);
	if(result >= 0)
	{
		TPD_MSGBEGIN += result;
		TPD_MSG = 0;
		return 1;
	}
	else if(result == -2)
	{
		return -1;
	}
	TPD_MSGBEGIN = c->_offset;
	return 0;
}


/////////////////////////////////////////////////////////////////////////

void http_readed_data(int add,struct tcp_connection* c)
{
	int i;
	c->_offset += add;
	do {
		switch(c->_buf[TPD_MSGBEGIN])
		{
		case 'G':i = _http_readed_get(c);
			break;
		case 'P':i = _htt_readed_post(c);
			break;
		default: i = -1;
			break;
		}
	} while(i > 0 && TPD_MSGBEGIN < c->_offset);

	//check parse result
	if(i == -1)
	{
		tcpconnection_close(c,"http read error message");
	}
	else if(TPD_MSGBEGIN > 0)
	{
		tcp_resetbuf(c,TPD_MSGBEGIN,c->_offset);
	}

	if(TPD_MSGBEGIN > 0)
	{
		TPD_MSGBEGIN = 0;
	}
}

void http_do_msg(handler_msg* msg,struct tcp_connection* c)
{
	if(tcp_domsg(msg,c) == 0)
	{
		return;
	}
	tcp_stream* s = (tcp_stream*)msg->_data;
	//create and send http head
	handler_msg* headmsg = create_handler_msg(1024);
	headmsg->_datalen = create_http_response_head((char*)headmsg->_data,1024,HTTP_RESULT_OK,"Ant net",s->_len);
	tcpconnection_add_msg(c,headmsg);

	//send http body
	msg->_datalen = s->_len;
	msg->_data = s->_buf;
	tcpconnection_add_msg(c,msg);
}


void http_protocol_init(tcp_protocol* p)
{
	p->_protocol_name = "http";
	p->_connected = tcp_connected;

	p->_readed_data = http_readed_data;
	p->_do_msg = http_do_msg;

	p->_sended_msg = tcp_sended_msg;
	p->_closed = tcp_closed;
}
