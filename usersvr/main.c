/*
 * main.c
 *
 *  Created on: 2014-9-26
 *      Author: lx
 */
#include "server.h"
#include <unistd.h>

int main(int argc,char** argv)
{
	int is_running = 1;
	if(0 != init_server(&is_running))
	{
		return 0;
	}
	init_netprotocol();
	start_server();
	while(is_running)
	{
		server_timer();
		usleep(10000);//10ms
	}
	stop_server();
	uninit_server();
	return 0;
}

