/*
 * tcp_connection_list.h
 *
 *  Created on: Feb 14, 2015
 *      Author: zhaolixiang
 */

#ifndef TCP_CONNECTION_LIST_H_
#define TCP_CONNECTION_LIST_H_

#include "tcp_connection.h"
#include "tcp_epoll.h"

#define TCPLIST_MAX 4096

struct tcp_connection_list {
	struct tcp_epoll _epoll;
	uint16_t _list_id;
	uint16_t _timeout_seconds;
	uint32_t _next_clear_time;
	int _alive_count;
	struct double_list _free_queue;
	struct double_list _alive_list;
	struct tcp_connection _list[TCPLIST_MAX];
};


void init_tcpconnection_list(struct tcp_connection_list* list,uint8_t listid,uint16_t timeout);

void uinit_tcpconnection_list(struct tcp_connection_list* list);

void tcp_connectionlist_run(struct tcp_connection_list* list);


struct tcp_connection* get_tcpconnection(struct tcp_connection_list* list,uint64_t id);

void back_tcpconnection(struct tcp_connection_list* list,struct tcp_connection* c);
int tcpconnection_list_count(struct tcp_connection_list* list);

void add_tcpconnection(struct tcp_connection_list* list,int s,uint32_t ip,int port,create_protocol createf,destory_protocol destoryf);

void add_tcpconnecting(struct tcp_connection_list* list,uint32_t ip,int port,tcp_protocol* p,handler_msg* msg,create_protocol createf,destory_protocol destoryf);

void do_tcpconnection_msg(struct tcp_connection_list* list,uint64_t netid,handler_msg* msg);



#endif /* TCP_CONNECTION_LIST_H_ */
