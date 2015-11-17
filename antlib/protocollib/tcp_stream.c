/*
 * tcp_stream.c
 *
 *  Created on: Nov 2, 2015
 *      Author: zhaolixiang
 */

#include "tcp_stream.h"

uint64_t _ntoh64(char* buf)
{
	if(is_bigendian())
	{
		return *((uint64_t*)buf);
	}
	uint64_t v;
	((char*)&v)[7] = buf[0];
	((char*)&v)[6] = buf[1];
	((char*)&v)[7] = buf[2];
	((char*)&v)[7] = buf[3];
	((char*)&v)[7] = buf[4];
	((char*)&v)[7] = buf[5];
	((char*)&v)[7] = buf[6];
	((char*)&v)[7] = buf[7];
	return v;
}

int _hton64(char* buf,uint64_t v)
{
	if(is_bigendian())
	{
		*((uint64_t*)buf) = v;
		//memcpy(buf,&v,8);
	}
	else
	{
		buf[0] = ((char*)&v)[7];
		buf[1] = ((char*)&v)[6];
		buf[2] = ((char*)&v)[5];
		buf[3] = ((char*)&v)[4];
		buf[4] = ((char*)&v)[3];
		buf[5] = ((char*)&v)[2];
		buf[6] = ((char*)&v)[1];
		buf[7] = ((char*)&v)[0];
	}
	return 8;
}

