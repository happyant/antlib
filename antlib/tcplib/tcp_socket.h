/*
 * tcp_socket.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

#include "tcp_header.h"

static inline void set_unblock(int s)
{
	int flag = fcntl(s, F_GETFL, 0);
	if ( -1 != flag )
	{
		fcntl(s, F_SETFL, flag | O_NONBLOCK);
	}
}

static inline int tcp_listen(const char* ip,int port,int len)
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {return ERROR_FD;}

	int opt=1;
	setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	uint32_t addr = INADDR_ANY;
	if (ip&&ip[0]) {addr=inet_addr(ip);}

	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = addr;

	if (bind(s, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		close(s);
		return ERROR_FD;
	}
	if (listen(s, len) == -1)
	{
		close(s);
		return ERROR_FD;
	}
	set_unblock(s);

	return s;
}

static inline int tcp_connect(int* s,uint32_t ip,int port)
{
	struct sockaddr_in addr;
	int ns = socket(AF_INET, SOCK_STREAM, 0);
	if(ns != ERROR_FD)
	{
		bzero(&addr,sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr.s_addr = htonl(ip);
		set_unblock(ns);

		if(connect(ns,(struct sockaddr*)&addr,(socklen_t)sizeof(addr)) ==0 )
		{
			*s = ns;
			return 1;
		}
		else if(errno == EINPROGRESS)
		{
			*s = ns;
			return 0;
		}
		close(ns);
	}
	*s = ERROR_FD;
	return -1;
}

static inline void tcp_close(int* s)
{
	if(*s != ERROR_FD)
	{
		close(*s);
		*s = ERROR_FD;
	}
}


static inline int tcp_connected_check(int* s)
{
	int error = 0;
	socklen_t len = sizeof(error);
	int code = getsockopt(*s, SOL_SOCKET, SO_ERROR, &error, &len);
	if (code < 0 || error)
	{
		tcp_close(s);
		return -1;
	}
	return 0;
}

static inline int tcp_accept(int s,uint32_t* ip,int* port)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int news = accept(s,(struct sockaddr*)&addr,&len);
	if(news != ERROR_FD)
	{
		*ip = ntohl(addr.sin_addr.s_addr);
		*port = ntohs(addr.sin_port);
		set_unblock(news);
	}
	return news;
}

static inline int tcp_read(int* s,char* buf,int len)
{
	int off = recv(*s,buf,len,0 );
	if(off > 0)
	{
		return off;
	}
	else if(off == 0 || (errno != EWOULDBLOCK && errno != EAGAIN))
	{
		tcp_close(s);
	}
	return 0;
}

static inline int tcp_write(int* s,char* buf,int len)
{
	int wlen = send(*s,buf,len,0);
	if(wlen > 0)
	{
		return wlen;
	}
	else if(wlen == 0 || (errno != EWOULDBLOCK && errno != EAGAIN))
	{
		tcp_close(s);
	}
	return 0;
}

static inline uint32_t to_intip(const char* str_ip)
{
	return ntohl(inet_addr(str_ip));
}

static inline const char* to_strip(uint32_t ip)
{
	struct in_addr in = {htonl(ip)};
	const char* sip =  inet_ntoa(in);
	if(sip == NULL)
	{
		return "";
	}
	return sip;
}


static inline uint32_t get_hostbyname(const char* url)
{
	if (url==NULL || url[0]==0)
		return 0;

	uint32_t ip = 0;
	struct addrinfo hints;
	struct addrinfo *ptr = NULL;
	struct addrinfo *result = NULL;
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;

	if (0 == getaddrinfo(url, 0, &hints, &result))
	{
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next)
		{
			if (AF_INET == ptr->ai_family)
			{
				struct sockaddr_in  *sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
				ip = ntohl(sockaddr_ipv4->sin_addr.s_addr);
				break;
			}
		}
		freeaddrinfo(result);
	}
	return ip;
}


#endif /* TCP_SOCKET_H_ */
