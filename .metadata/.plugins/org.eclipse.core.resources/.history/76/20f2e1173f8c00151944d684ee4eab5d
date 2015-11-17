/*
 * http_protocol.h
 *
 *  Created on: Sep 18, 2015
 *      Author: zhaolixiang
 */

#ifndef HTTP_PROTOCOL_H_
#define HTTP_PROTOCOL_H_

#include "tcp_protocol_header.h"

#define POST_MAX_SIZE 0xFFFFFFFF

//*off > 2
int parse_http_header_len(char* buf,uint16_t len,uint16_t* off);

int parse_http_get(char* buf,int len,char** path1,char** path2,char** values);
int set_http_get(char* buf,int len,char* path,char* host,char* values,int value_len);

int parse_http_post(char* buf,int len,char** path1,char** path2,int64_t* content_len);
int set_http_post(char* buf,int len,char* path,char* host,char* values,int value_len);


#define HTTP_RESULT_OK  "200 OK"
#define HTTP_RESULT_ERROR "404 Not Found"

int create_http_response_head(char* buf,int len,const char* result,char* server,int content_len);
int parse_http_response(char* buf,int len,int64_t* value_len,int* result);


int parse_http_values(char* buf,int len,const char** keys,char** values,int n);

int parse_http_values2(char* buf,int len,void** values,int (*get_index_f)(void*,const char*,void**),void* tb);

#endif /* HTTP_PROTOCOL_H_ */
