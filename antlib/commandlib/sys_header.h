/*
 * sys_header.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 */

#ifndef SYS_HEADER_H_
#define SYS_HEADER_H_

//base
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
//thread
#include <pthread.h>
#include <signal.h>
//net
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
//type
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <sys/vfs.h>
//debug
#include <errno.h>
#include <assert.h>
#include <stdbool.h>



#define ERROR_FD  (-1)

#define BREAKPOINT() __asm__("int $3")

#define STRNCPY(r,s,n) do{strncpy(r,s,n);r[n-1]=0;}while(0)

#define SNPRINTF(n,buf,len,format,...) \
	do{ assert(len > 1);\
		n = snprintf(buf,len,format,##__VA_ARGS__);\
		if(n <=0 ) {n = 0;} \
		else if(n >= len) {buf[len-1]=0;n =len;}\
	}while(0)


#define ASSERT(b) assert(b)

static inline int is_bigendian()
{
	static uint16_t tmp = 0x0100;
	return ((char*)&tmp)[0] > 0;
}

#endif /* SYS_HEADER_H_ */
