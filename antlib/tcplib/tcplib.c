/*
 * tcplib.c
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */


#include "tcplib.h"
#include "tcp_server.h"

struct tcplib {
	int _handler_group_id;
	uint32_t _server_num;
	struct tcp_listener_list _listeners;
	struct tcp_server _servers[1];
};

struct tcplib* _g_tcplib = NULL;



void init_tcplib(int handler_groupid,int servers_num)
{
	ASSERT(_g_tcplib == NULL && servers_num > 0 && servers_num <= 256);
	int size = sizeof(struct tcplib) + (sizeof(struct tcp_server)*(servers_num-1));
	_g_tcplib = (struct tcplib*)my_malloc(size);
	memset(_g_tcplib,0,size);

	_g_tcplib->_handler_group_id = handler_groupid;
	_g_tcplib->_server_num = servers_num;

	init_tcplistener_list(&_g_tcplib->_listeners);

	char server_name[32];
	int i;
	struct tcp_server* s;
	for(i = 0; i < servers_num; i++)
	{
		s = &_g_tcplib->_servers[i];
		create_tcpserver(s,&_g_tcplib->_listeners,(uint16_t)i);
		sprintf(server_name,"tcpserver_%d",i);
		s->_handlerid = register_handler(_g_tcplib->_handler_group_id,server_name,do_tcpserver_msg,do_tcpserver,NULL,s);
		ASSERT(s->_handlerid > 0);
	}
}

void uninit_tcplib()
{
	int i;
	for(i = 0; i < _g_tcplib->_server_num; i++)
	{
		struct tcp_server* s = &_g_tcplib->_servers[i];
		if(s->_handlerid > 0)
			distory_tcpserver(s);
	}
	uninit_tcplistener_list(&_g_tcplib->_listeners);
	my_free(_g_tcplib);
	_g_tcplib = NULL;
}

int tcp_listenport(const char* ip,int port,create_protocol createf,destory_protocol destoryf)
{
	struct tcp_listener* l = add_tcplistener(&_g_tcplib->_listeners,ip,port,createf,destoryf);
	if(NULL == l)
	{
		return -1;
	}
	uint32_t i;
	for(i = 0; i < _g_tcplib->_server_num; i++)
	{
		add_tcp_accepter(l,&_g_tcplib->_servers[i]._connections);
	}
	return 0;
}

void tcp_cennectserver(int serverid,uint32_t ip,int port,void* msg,int len,uint64_t userid,uint32_t handlerid,
		tcp_protocol* p,create_protocol createf,destory_protocol destoryf)
{
	if(serverid < 0)
	{
		static uint32_t mainid = 0;
		serverid = (int)(LOCK_ADD(mainid,1) % _g_tcplib->_server_num);
	}
	ASSERT(serverid >= 0 && serverid < _g_tcplib->_server_num && len >= 0);
	struct tcp_server* s = &_g_tcplib->_servers[serverid];

	int size = sizeof(struct tcp_connect_data)+len;

	handler_msg* hmsg = create_handler_msg(size);
	hmsg->_userid = 0;
	hmsg->_msgid = TCPLIB_SYSMSGID_CONNECT;
	hmsg->_datalen = size;

	struct tcp_connect_data* data = (struct tcp_connect_data*)hmsg->_data;
	data->_ip = ip;
	data->_port = port;
	data->_user_id = userid;
	data->_user_handlerid = handlerid;
	data->_p = p;
	data->_createf = createf;
	data->_destoryf = destoryf;
	data->_msglen = len;
	if(len > 0){
		memcpy(data->_msg,msg,len);
	}
	send_handler_msg(s->_handlerid,hmsg);
}

void tcp_sendmsg(uint64_t netid,handler_msg* msg)
{
	tcpid_t tcpid = {netid};
	ASSERT(tcpid._st._listid < _g_tcplib->_server_num);
	struct tcp_server* s = &_g_tcplib->_servers[tcpid._st._listid];
	msg->_userid = netid;
	send_handler_msg(s->_handlerid,msg);
}


