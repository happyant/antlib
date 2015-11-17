/*
 * tcp_stream.h
 *
 *  Created on: Feb 15, 2015
 *      Author: zhaolixiang
 */

#ifndef TCP_STREAM_H_
#define TCP_STREAM_H_


#include "tcp_protocol_header.h"

#define MSG_KEEPHEAD_SIZE 16
#define MSG_NORMAL_SIZE 8142

uint64_t _ntoh64(char* buf);
int _hton64(char* buf,uint64_t v);

//////////////////////////////////////////////////////////////

#define ntoh16(buf) (ntohs(*((uint16_t*)(buf))))
#define ntoh32(buf) (ntohl(*((uint16_t*)(buf))))
#define ntoh64(buf) _ntoh64(buf)

#define hton16(buf,v16) do{v16=htons(v16);memcpy(buf,&v16,2);}while(0)
#define hton32(buf,v32) do{v32=htonl(v32);memcpy(buf,&v32,4);}while(0)
#define hton64(buf,v64) _hton64(buf,v64)

//////////////////////////////////////////////////////////////

#define MSGINFO_FIELD(msg,index) (((tcp_stream*)msg->_data)->_vl->_vl[index])
#define MSGINFO_NETID(msg)	(((tcp_stream*)msg->_data)->_netid)
#define MSGINFO_IP(msg) (((tcp_stream*)msg->_data)->_ip)

struct tcpmsg_info
{
	void* _vl[1];
};

typedef struct tcp_stream {
	void* _tb;
	struct tcpmsg_info* _vl;
	uint64_t _netid;
	uint32_t _ip;
	int _off;
	int _len;
	char _head[MSG_KEEPHEAD_SIZE];
	char _buf[1];
}tcp_stream;


static inline handler_msg* create_tcpstream(uint64_t userid,int msgid,int msglen,void* tb,int vlnum,uint64_t netid,uint32_t ip)
{
	int size;
	if(MSG_NORMAL_SIZE-sizeof(tcp_stream) < msglen+sizeof(void*)*vlnum)
		size = sizeof(tcp_stream)+msglen+sizeof(void*)*vlnum;
	else
		size = MSG_NORMAL_SIZE;

	handler_msg* msg = create_handler_msg(size);
	msg->_userid = userid;
	msg->_msgid = msgid;
	msg->_datalen = size;

	tcp_stream* stream = (tcp_stream*)msg->_data;
	stream->_tb = tb;
	stream->_netid = netid;
	stream->_ip = ip;
	stream->_len = msglen;
	stream->_off = 0;

	stream->_vl =(struct tcpmsg_info*)(((char*)msg->_data)+sizeof(tcp_stream)+msglen);
	int i;
	for(i =0;i < vlnum;++i)
	{
		stream->_vl->_vl[i] = 0;
	}
	return msg;
}

static inline void reset_tcpstream(tcp_stream* stream,int len)
{
	stream->_off = 0;
	stream->_len = len;
}

static inline void push_tcpstream_end(tcp_stream* stream)
{
	stream->_len = stream->_off;
	stream->_off = 0;
}

static inline void push_tcpstream_int64(tcp_stream* stream,uint64_t vl)
{
	ASSERT(stream->_off + sizeof(uint64_t) <= stream->_len);
	hton64(stream->_buf+stream->_off,vl);
	stream->_off += sizeof(uint64_t);
}

static inline uint64_t pop_tcpstream_int64(tcp_stream* stream)
{
	ASSERT(stream->_off + sizeof(uint64_t) <= stream->_len);
	uint64_t vl = ntoh64(stream->_buf+stream->_off);
	stream->_off += sizeof(uint64_t);
	return vl;
}

static inline void push_tcpstream_int32(tcp_stream* stream,uint32_t vl)
{
	ASSERT(stream->_off + sizeof(uint32_t) <= stream->_len);
	hton32(stream->_buf+stream->_off,vl);
	stream->_off += sizeof(uint32_t);
}


static inline uint32_t pop_tcpstream_int32(tcp_stream* stream)
{
	ASSERT(stream->_off + sizeof(uint32_t) <= stream->_len);
	uint32_t vl = ntoh32(stream->_buf+stream->_off);
	stream->_off += sizeof(uint32_t);
	return vl;
}


static inline void push_tcpstream_int16(tcp_stream* stream,uint16_t vl)
{
	ASSERT(stream->_off + sizeof(uint16_t) <= stream->_len);
	hton16(stream->_buf+stream->_off,vl);
	stream->_off += sizeof(uint16_t);
}


static inline uint16_t pop_tcpstream_int16(tcp_stream* stream)
{
	ASSERT(stream->_off + sizeof(uint16_t) <= stream->_len);
	uint16_t vl = ntoh16(stream->_buf+stream->_off);
	stream->_off += sizeof(uint16_t);
	return vl;
}

static inline void push_tcpstream_int8(tcp_stream* stream,uint8_t vl)
{
	ASSERT(stream->_off + sizeof(uint8_t) <= stream->_len);
	stream->_buf[stream->_off++] = (char)vl;
}

static inline uint8_t pop_tcpstream_int8(tcp_stream* stream)
{
	ASSERT(stream->_off + sizeof(uint8_t) <= stream->_len);
	return (uint8_t)stream->_buf[stream->_off++];
}

static inline void push_tcpstream_str(tcp_stream* stream,const char* vl,int len)
{
	ASSERT(len > 0 && stream->_off + len <= stream->_len);
	memcpy(stream->_buf+stream->_off,vl,len);
	stream->_off += len;
}

static inline void pop_tcpstream_str(tcp_stream* stream,char* buf,int len)
{
	ASSERT(len > 0 && stream->_off + len <= stream->_len);
	memcpy(buf,stream->_buf+stream->_off,len);
	stream->_off += len;
}






#endif /* TCP_STREAM_H_ */
