/*
 * alloc.h
 *
 *  Created on: Feb 12, 2015
 *      Author: zhaolixiang
 */

#ifndef ALLOC_H_
#define ALLOC_H_

#include "sys_header.h"
#include "my_string.h"

#define _DEBUG_ALLOC 1

#ifdef _DEBUG_ALLOC

	void* _my_malloc(size_t size,const char* file,int line);
	void _my_free(void* p);

	#define my_malloc(size) _my_malloc(size,__FILE__,__LINE__)
	#define my_free(p) _my_free(p);

	void print_unfree_record();

#else

	#define my_malloc(size) malloc(size)
	#define my_free(p) free(p)

	static inline void print_unfree_record(){}
#endif


#define my_new(c) ((c*)my_malloc(sizeof(c)))


#define MEMORY_HEAD_SIZE 128

#endif /* ALLOC_H_ */
