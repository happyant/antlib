/*
 * thread.h
 *
 *  Created on: 2015-2-10
 *      Author: lx
 */

#ifndef THREAD_H_
#define THREAD_H_


#define THREAD_NAME_LEN 16


void create_threads_list();

void destory_threads_list();

int create_thread(const char* thname,void (*th_proc)(void*),void* data);

void start_all_threads();

void stop_all_threads();


#endif /*THREAD_H_*/
