/*
 * net_protocol.c
 *
 *  Created on: Oct 26, 2015
 *      Author: zhaolixiang
 */
#include "protocollib.h"
#include "tcp_msglist.h"
#include "tcp_protocol_http.h"
#include "tcp_protocol4.h"
#include "tcp_protocol12.h"
#include "tcp_protocol_flash.h"
#include "tcplib/tcplib.h"



tcp_protocol _g_protocol[4];

void init_protocollib()
{
	tcpmsg_libinit();

	memset(_g_protocol,0,sizeof(_g_protocol));
	http_protocol_init(&_g_protocol[0]);
	tcp4_protocol_init(&_g_protocol[1]);
	tcp12_protocol_init(&_g_protocol[2]);
	flash_protocol_init(&_g_protocol[3]);
}

void uninit_protocollib()
{
	tcpmsg_libuninit();
}

void tcpprotocol_destory(tcp_protocol* p)
{

}

tcp_protocol* tcpprotocol_create_http(char buf[4])
{
	return &_g_protocol[0];
}

tcp_protocol* tcpprotocol_create_tcp4(char buf[4])
{
	return &_g_protocol[1];
}

tcp_protocol* tcpprotocol_create_tcp12(char buf[4])
{
	return &_g_protocol[2];
}

tcp_protocol* tcpprotocol_create_flash(char buf[4])
{
	return &_g_protocol[3];
}

tcp_protocol* tcpprotocol_create_httpflash(char buf[4])
{
	switch(buf[0])
	{
	case 'G': //GET
		if(buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ')
		{
			return &_g_protocol[0];
		}
		break;
	case 'P': //POST
		if(buf[1] == 'O' && buf[2] == 'S' && buf[3] == 'T')
		{
			return &_g_protocol[0];
		}
		break;
	case '<'://"<cross-domain-policy/>",
		if(buf[1] == 'c' && buf[2] == 'r' && buf[3] == 'o')
		{
			return &_g_protocol[3];
		}
		break;
	}
	return NULL;
}

//"GET POST<cro4TCPCTCP"
//47 45 54 20 50 4f 53 54 3c 63 72 6f 34 54 43 50 43 54 43 50 0
tcp_protocol* tcpprotocol_create_user(char buf[4])
{
	switch(buf[0])
	{
	case 'G': //GET
		if(buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ')
		{
			return &_g_protocol[0];
		}
		break;
	case 'P': //POST
		if(buf[1] == 'O' && buf[2] == 'S' && buf[3] == 'T')
		{
			return &_g_protocol[0];
		}
		break;
	case '<'://"<cross-domain-policy/>",
		if(buf[1] == 'c' && buf[2] == 'r' && buf[3] == 'o')
		{
			return &_g_protocol[3];
		}
		break;
	}
	return &_g_protocol[1];
}

int bind_tcpprotocol(const char* ip,int port,int protocol_type)
{
	int reault = -1;
	switch(protocol_type)
	{
	case NETPROTOCOL_TYPE_HTTP:
		reault = tcp_listenport(ip,port,tcpprotocol_create_http,tcpprotocol_destory);
		break;
	case NETPROTOCOL_TYPE_TCP4:
		reault = tcp_listenport(ip,port,tcpprotocol_create_tcp4,tcpprotocol_destory);
		break;
	case NETPROTOCOL_TYPE_TCP12:
		reault = tcp_listenport(ip,port,tcpprotocol_create_tcp12,tcpprotocol_destory);
		break;
	case NETPROTOCOL_TYPE_FLASH:
		reault = tcp_listenport(ip,port,tcpprotocol_create_flash,tcpprotocol_destory);
		break;
	case NETPROTOCOL_TYPE_HTTP_FLASH:
		reault = tcp_listenport(ip,port,tcpprotocol_create_httpflash,tcpprotocol_destory);
		break;
	case NETPROTOCOL_TYPE_HTTP_TCP4_FLASH:
		reault = tcp_listenport(ip,port,tcpprotocol_create_user,tcpprotocol_destory);
		break;
	}
	return reault;
}


void close_tcpconnection(uint64_t netid,const char* close_cause)
{
	handler_msg* msg = create_handler_msg(0);
	msg->_datalen = 0;
	msg->_msgid = SYSMSG_CLOSE_CONNECTION;
	msg->_data = (void*)close_cause;
	tcp_sendmsg(netid,msg);
}

void bind_tcpconnection_userid(uint64_t netid,uint64_t userid)
{
	handler_msg* msg = create_handler_msg(sizeof(uint64_t));
	msg->_datalen = sizeof(uint64_t);
	msg->_msgid = SYSMSG_SETUSERID;
	*((uint64_t*)msg->_data) = userid;
	tcp_sendmsg(netid,msg);
}



