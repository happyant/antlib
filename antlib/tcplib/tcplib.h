/*
 * tcplib.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TCPLIB_H_
#define TCPLIB_H_

#include "tcp_header.h"


void init_tcplib(int handler_groupid,int servers_num);

void uninit_tcplib();

int tcp_listenport(const char* ip,int port,create_protocol createf,destory_protocol destoryf);

void tcp_cennectserver(int svrid,uint32_t ip,int port,void* msg,int len,uint64_t userid,uint32_t handlerid,
		tcp_protocol* p,create_protocol createf,destory_protocol destoryf);

void tcp_sendmsg(uint64_t netid,handler_msg* msg);


#endif /* TCPLIB_H_ */
