/*
 * tcp_protocol_header.h
 *
 *  Created on: Jul 21, 2015
 *      Author: zhaolixiang
 */

#ifndef TCP_PROTOCOL_HEADER_H_
#define TCP_PROTOCOL_HEADER_H_

#include "handlerlib/handler_msg.h"
#include "tcplib/tcp_header.h"
#include "tcplib/tcp_connection.h"
#include "commandlib/hash.h"
#include "commandlib/alloc.h"


struct protocol4_head {
	uint16_t _bodylen;//just body length
	uint16_t _msgid;
};

struct protocol12_head {
	uint64_t _userid;
	uint16_t _bodylen;//just body length
	uint16_t _msgid;
};

//tcp protocol data
#define TPD_IP			(c->_expands[0]._e32[0])
#define TPD_PORT		(c->_expands[0]._e32[1])
#define TPD_USERID 		(c->_expands[1]._e64)

#define TPD_MSG 		(c->_expands[2]._ptr)
#define TPD_MSGBEGIN	(c->_expands[3]._e16[0])
#define TPD_MSGOFF 		(c->_expands[3]._e16[1])

#define TPD_CLOSECAUSE ((const char*)(c->_expands[15]._ptr))

/* @net_stream buffer size,max size of one net message
 * */
#define NETSTREAM_BUFLEN 	8000

/* @connection receive buffer size
 * */
#define TCPRECVBUFFER_SIZE 	 4000



#endif /* TCP_PROTOCOL_HEADER_H_ */
