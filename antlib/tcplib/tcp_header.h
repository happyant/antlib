/*
 * tcp_header.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TCP_HEADER_H_
#define TCP_HEADER_H_

#include "commandlib/command_header.h"
#include "handlerlib/handlerlib.h"
#include "commandlib/my_time.h"
#include "handlerlib/handler_msg.h"

struct tcp_connection;

typedef union tcp_connection_expand
{
	void* _ptr;
	uint64_t _e64;
	uint32_t _e32[2];
	uint16_t _e16[4];
	uint8_t  _e8[8];
}tcp_connection_expand;

typedef union tcpid_t
{
	uint64_t _i64;
	struct {
		uint32_t _index;
		uint16_t _arrayid;
		uint8_t _mask; // 0 or TCPID_MASK
		uint8_t _listid;
	}_st;
}tcpid_t;

typedef struct tcp_protocol {
	const char* _protocol_name;
	//msg->_data is struct tcp_connect_data
	void (*_connected)(int result,struct tcp_connection* c,handler_msg* msg);
	void (*_readed_data)(int add,struct tcp_connection* c);
	void (*_do_msg)(handler_msg* msg,struct tcp_connection* c);
	void (*_sended_msg)(handler_msg* msg,struct tcp_connection* c);
	void (*_closed)(struct tcp_connection* c);
} tcp_protocol;


typedef tcp_protocol* (*create_protocol)(char buf[4]);
typedef void (*destory_protocol)(tcp_protocol* p);

typedef struct tcp_connect_data {
	tcp_protocol* _p;
	create_protocol _createf;
	destory_protocol _destoryf;
	uint64_t _user_id;
	uint32_t _user_handlerid;
	uint32_t _ip;
	int _port;
	int _msglen;
	char _msg[1];
} tcp_connect_data;


#define TCPLIB_SYSMSGID_CONNECT 1
#define TCPID_MASK 0x0F
#define TCP_CONNECTION_TIMEOUT 120


#endif /* TCP_HEADER_H_ */
