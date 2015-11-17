/*
 * net_protocol.h
 *
 *  Created on: Oct 26, 2015
 *      Author: zhaolixiang
 */

#ifndef PROTOCOLLIB_H_
#define PROTOCOLLIB_H_


#include "commandlib/alloc.h"

enum netprotocol_type {
	NETPROTOCOL_TYPE_HTTP = 0,
	NETPROTOCOL_TYPE_TCP4 = 1,
	NETPROTOCOL_TYPE_TCP12 =2,
	NETPROTOCOL_TYPE_FLASH = 3,
	NETPROTOCOL_TYPE_HTTP_FLASH =4,
	NETPROTOCOL_TYPE_HTTP_TCP4_FLASH =5,
};


void init_protocollib();
void uninit_protocollib();

int bind_tcpprotocol(const char* ip,int port,int protocol_type);

void close_tcpconnection(uint64_t netid,const char* close_cause);
void bind_tcpconnection_userid(uint64_t netid,uint64_t userid);


#define SYSMSG_CLOSE_CONNECTION 11
#define SYSMSG_SETUSERID 12


#endif /* PROTOCOLLIB_H_ */
