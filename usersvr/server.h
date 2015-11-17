/*
 * server.h
 *
 *  Created on: Sep 16, 2015
 *      Author: zhaolixiang
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "net_protocol.h"

int init_server(int* running);
void uninit_server();

void start_server();
void stop_server();
void server_timer();

#endif /* SERVER_H_ */
