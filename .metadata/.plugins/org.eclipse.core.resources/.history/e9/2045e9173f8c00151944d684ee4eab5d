/*
 * tcp_usermsg_list.h
 *
 *  Created on: Sep 14, 2015
 *      Author: zhaolixiang
 */

#ifndef TCP_MSGLIST_H_
#define TCP_MSGLIST_H_

#include "tcp_protocol_header.h"
#include "tcp_stream.h"

#define TCPMSGID(mianid,secondid) (((mianid&0x3F)<<10)|(secondid&0x3FF))

typedef void (*do_tcpmsg)(handler_msg* msg,uint64_t userid,uint64_t netid);

void tcpmsg_libinit();

void tcpmsg_libuninit();


struct pm_tcpmsg_field
{
	const char* _field;
	int _type;
	int _min;
	int _max;
};

int tcpmsg_regist_binary(int id,struct pm_tcpmsg_field* fields,int n,do_tcpmsg f);
int tcpmsg_regist_string(const char* key1,const char* key2,int id,struct pm_tcpmsg_field* fields,int n,do_tcpmsg f);

handler_msg* tcpmsg_getbyid(int msgid,int msglen,uint64_t userid,uint64_t netid,uint32_t ip);
handler_msg* tcpmsg_getbykey(const char* key1,const char* key2,int msglen,uint64_t userid,uint64_t netid,uint32_t ip);
int tcpmsg_putout_range(handler_msg* msg,char* addmsg,int addlen);

int tcpmsg_type(void* tp);

#endif /* TCP_MSGLIST_H_ */
