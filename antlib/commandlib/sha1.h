/*
 * sha1.h
 *
 *  Created on: Sep 24, 2015
 *      Author: lx
 */

#ifndef SHA1_H_
#define SHA1_H_

#include "sys_header.h"

uint8_t* SHA1(void *buf,uint64_t size,uint8_t result[20]);


#endif /* SHA1_H_ */
