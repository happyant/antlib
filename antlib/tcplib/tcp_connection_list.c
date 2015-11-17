/*
 * tcp_connection_list.c
 *
 *  Created on: Oct 12, 2015
 *      Author: zhaolixiang
 */

#include "tcp_connection_list.h"


void init_tcpconnection_list(struct tcp_connection_list* list,uint8_t listid,uint16_t timeout)
{
	int i;
	memset(list,0,sizeof(struct tcp_connection_list));
	init_epoll(&list->_epoll);
	list->_list_id = listid;

	list->_timeout_seconds = timeout;
	list->_next_clear_time = timeout+120;//more later 120 seconds

	struct tcp_connection* c;
	for(i = 0; i < TCPLIST_MAX; i++)
	{
		c = &list->_list[i];
		c->_id._st._arrayid = i;
		c->_id._st._listid = listid;
		c->_id._st._mask = 0;
		c->_socket = ERROR_FD;
		double_list_push(&list->_free_queue,&c->_listnode);
	}
}

void uinit_tcpconnection_list(struct tcp_connection_list* list)
{
	uninit_epoll(&list->_epoll);
	struct double_list_node* node = list->_alive_list._head;
	while(node)
	{
		tcpconnection_close((struct tcp_connection*)node,"by free connections list");
		tcpconnection_free((struct tcp_connection*)node);
		node = node->_next;
	}
	memset(list,0,sizeof(struct tcp_connection_list));
}


struct tcp_connection* get_tcpconnection(struct tcp_connection_list* list,uint64_t id)
{
	tcpid_t netid = {id};
	ASSERT(netid._st._listid == list->_list_id && netid._st._arrayid < TCPLIST_MAX && netid._st._mask == TCPID_MASK);

	struct tcp_connection* c = &list->_list[netid._st._arrayid];
	return (c->_id._i64 == netid._i64 && c->_socket != ERROR_FD) ? c : NULL;
}

void back_tcpconnection(struct tcp_connection_list* list,struct tcp_connection* c)
{
	if(c->_socket == ERROR_FD)
	{
		ASSERT(list->_alive_count > 0);
		--list->_alive_count;
		double_list_remove(&list->_alive_list,&c->_listnode);
		tcpconnection_free(c);
		double_list_push(&list->_free_queue,&c->_listnode);
	}
}


void tcp_connectionlist_run(struct tcp_connection_list* list)
{
	epoll_run(&list->_epoll,list);
	//clear time out
	uint32_t now_time = get_mytime();
	if(list->_next_clear_time > now_time) {
		return;
	}
	now_time -= list->_timeout_seconds;
	struct tcp_connection* rm;
	struct double_list_node* node = list->_alive_list._head;
	while(node)
	{
		if(tcpconnection_is_timeout((struct tcp_connection*)node,now_time)) {
			rm = (struct tcp_connection*)node;
			node = node->_next;
			tcpconnection_close(rm,"by timeout");
			back_tcpconnection(list,rm);
		} else { node = node->_next; }
	}
}

int tcpconnection_list_count(struct tcp_connection_list* list)
{
	return list->_alive_count;
}


void add_tcpconnection(struct tcp_connection_list* list,int s,uint32_t ip,int port,create_protocol createf,destory_protocol destoryf)
{
	struct tcp_connection* c = (struct tcp_connection*)double_list_pop(&list->_free_queue);
	ASSERT(c);
	tcpconnection_init(c,s,TCPSTATE_NOTHNG,ip,port,0,NULL,createf,destoryf);
	double_list_push(&list->_alive_list,&c->_listnode);
	++list->_alive_count;
	epoll_radd(&list->_epoll,s,c->_id._i64);
}

void add_tcpconnecting(struct tcp_connection_list* list,uint32_t ip,int port,tcp_protocol* p,handler_msg* msg,create_protocol createf,destory_protocol destoryf)
{
	struct tcp_connection* c = (struct tcp_connection*)double_list_pop(&list->_free_queue);
	if(NULL == c)
	{
		p->_connected(-1,NULL,msg);
		return;
	}
	int s;
	int re = tcp_connect(&s,ip,port);
	if(re == -1)
	{
		p->_connected(-1,NULL,msg);
		double_list_push(&list->_free_queue,&c->_listnode);
		return;
	}
	if(re == 1)
	{
		tcpconnection_init(c,s,TCPSTATE_NOTHNG,ip,port,1,p,createf,destoryf);
		double_list_push(&list->_alive_list,&c->_listnode);
		++list->_alive_count;
		epoll_radd(&list->_epoll,s,c->_id._i64);
	}
	else
	{
		tcpconnection_init(c,s,TCPSTATE_CONNECTING,ip,port,1,p,createf,destoryf);
		double_list_push(&list->_alive_list,&c->_listnode);
		++list->_alive_count;
		epoll_wadd(&list->_epoll,s,c->_id._i64);
	}
	tcpconnection_add_msg(c,msg);
}

void do_tcpconnection_msg(struct tcp_connection_list* list,uint64_t netid,handler_msg* msg)
{
	struct tcp_connection* c = get_tcpconnection(list,netid);
	if(NULL == c)
	{
		free_handler_msg(msg);
		return;
	}
	tcpconnection_do_sendmsg(c,msg);
	if(c->_state == TCPSTATE_NOTHNG && c->_sendqueue._size > 0)
	{
		epoll_setw(&list->_epoll,c->_socket,c->_id._i64);
		c->_state = TCPSTATE_SEND;
	}
	back_tcpconnection(list,c);
}





