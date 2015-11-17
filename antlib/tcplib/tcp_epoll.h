/*
 * tcp_epoll.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef TCP_EPOLL_H_
#define TCP_EPOLL_H_

#include "tcp_header.h"

#define TCPEPOLL_EVENTSMAX 255

struct tcp_connection_list;


struct tcp_epoll
{
	int _fd;
	struct epoll_event _events[TCPEPOLL_EVENTSMAX];
};

void init_epoll(struct tcp_epoll* ep);

void uninit_epoll(struct tcp_epoll* ep);

void epoll_run(struct tcp_epoll* ep,struct tcp_connection_list* list);

static inline void epoll_wadd(struct tcp_epoll* ep,int s,uint64_t netid)
{
	struct epoll_event ev;
	ev.events = EPOLLOUT | EPOLLET;
	ev.data.u64 = netid;
	epoll_ctl(ep->_fd, EPOLL_CTL_ADD, s, &ev);
}

static inline void epoll_radd(struct tcp_epoll* ep,int s,uint64_t netid)
{
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.u64 = netid;
	epoll_ctl(ep->_fd, EPOLL_CTL_ADD, s, &ev);
}

static inline void epoll_setw(struct tcp_epoll* ep,int s,uint64_t netid)
{
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
	ev.data.u64 = netid;
	epoll_ctl(ep->_fd, EPOLL_CTL_MOD, s, &ev);
}

static inline void epoll_setunw(struct tcp_epoll* ep,int s,uint64_t netid)
{
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.u64 = netid;
	epoll_ctl(ep->_fd, EPOLL_CTL_MOD, s, &ev);
}

static inline void event_del(struct tcp_epoll* ep,int s)
{
	struct epoll_event ev={0,{0}};
	epoll_ctl(ep->_fd, EPOLL_CTL_DEL, s, &ev);
}


#endif /* TCP_EPOLL_H_ */
