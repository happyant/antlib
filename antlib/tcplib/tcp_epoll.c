/*
 * tcp_epoll.c
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#include "tcp_epoll.h"
#include "tcp_connection_list.h"

void init_epoll(struct tcp_epoll* ep)
{
	ep->_fd = epoll_create(255);
	assert(ep->_fd != ERROR_FD);
}


void uninit_epoll(struct tcp_epoll* ep)
{
	close(ep->_fd);
	ep->_fd = ERROR_FD;
}



void epoll_run(struct tcp_epoll* ep,struct tcp_connection_list* list)
{
	struct tcp_connection* con;
	int i;
	int n = epoll_wait(ep->_fd,ep->_events,TCPEPOLL_EVENTSMAX,0);
	for(i = 0; i < n; i++)
	{
		con = get_tcpconnection(list,ep->_events[i].data.u64);
		if(NULL == con){
			continue;
		}
		if((ep->_events[i].events&EPOLLERR) > 0)
		{
			tcpconnection_close(con,"net event error");
			back_tcpconnection(list,con);
			continue;
		}
		if((ep->_events[i].events&EPOLLIN) > 0)
		{
			tcpconnection_recv(con);
		}
		if((ep->_events[i].events&EPOLLOUT) > 0)
		{
			tcpconnection_send(con);
			if(tcpconnection_is_empty(con) && con->_socket != ERROR_FD)
			{
				epoll_setunw(ep,con->_socket,con->_id._i64);
				con->_state = TCPSTATE_NOTHNG;
			}
		}
		back_tcpconnection(list,con);
	}
}
