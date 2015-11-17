/*
 * http_protocol.c
 *
 *  Created on: Sep 18, 2015
 *      Author: zhaolixiang
 */

#include "http_protocol.h"
#include <stdlib.h>

#define IS_HTTPURL_OK(c) (_g_http_path_words[((unsigned char)c)] > 0)
#define IS_HTTPVALUE_OK(c) (_g_http_path_words[((unsigned char)c)] > 1)
#define IS_HTTPPATH_OK(c) (_g_http_path_words[((unsigned char)c)] > 3)
#define IS_HTTP_ENESCAPE(c) (_g_http_path_words[((unsigned char)c)]<3)
#define IS_HTTP_DISESCAPE(c) (_g_http_path_words[((unsigned char)c)]>4)

static char HEX_STR[] = "0123456789ABCDEF";

char _g_http_path_words[256] = {
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		1,  3,  0,  0,  3,  2,  3,  0,  3,  3,  3,  3,  3,  3,  3,  4,
	   48, 48, 48, 48, 48, 48, 48, 48, 48, 48,  3,  3,  0,  3,  0,  1,
	    3, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
	   65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,  3,  3,  3,  3,  4,
	    0, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,
	   97, 97, 97, 97, 97, 97, 97, 97, 97, 97, 97,  3,  0,  3,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

void init_http_path_words()
{
	memset(_g_http_path_words,0,sizeof(_g_http_path_words));
	int i;
	//path
	for(i = 'a'; i <= 'z'; ++i){
		_g_http_path_words[i] = 'a';
	}
	for(i = 'A'; i <= 'Z'; ++i){
		_g_http_path_words[i] = 'A';
	}
	for(i = '0'; i <= '9'; ++i){
		_g_http_path_words[i] = '0';
	}
	_g_http_path_words['/'] = 4;
	_g_http_path_words['_'] = 4;

	_g_http_path_words['&'] = 3;
	_g_http_path_words['-'] = 3;
	_g_http_path_words['+'] = 3;
	_g_http_path_words['='] = 3;
	_g_http_path_words['^'] = 3;
	_g_http_path_words['*'] = 3;
	_g_http_path_words['@'] = 3;
	_g_http_path_words['('] = 3;
	_g_http_path_words[')'] = 3;
	_g_http_path_words['['] = 3;
	_g_http_path_words[']'] = 3;
	_g_http_path_words['{'] = 3;
	_g_http_path_words['}'] = 3;
	_g_http_path_words['!'] = 3;
	_g_http_path_words[':'] = 3;
	_g_http_path_words[','] = 3;
	_g_http_path_words[';'] = 3;
	_g_http_path_words['.'] = 3;
	_g_http_path_words['$'] = 3;
	_g_http_path_words['\\'] =3;
	//value
	_g_http_path_words['%'] = 2;
	//session
	_g_http_path_words['?'] = 1;
	_g_http_path_words['\n'] =1;
	_g_http_path_words['\r'] =1;
	_g_http_path_words[' '] = 1;
}

//////////////////////////////////////////////////////////////////////////////

int escape2str(const char* str,int len,char* res,int res_len)
{
	int i = 0,j = 0;
	while(i < len)
	{
		if(*str != '%')
		{
			res[j] = *str;
			++i;
		}
		else
		{
			if(i >= len-2 || j >= res_len)
				return -1;
			i += 3;
			++str;
			if(!IS_HTTP_DISESCAPE(*str))
				return -1;
			res[j] = ((*str - _g_http_path_words[(unsigned char)*str])<<4);

			++str;
			if(!IS_HTTP_DISESCAPE(*str))
				return -1;
			res[j] |= (*str-_g_http_path_words[(unsigned char)*str]);
		}
		++str;
		++j;
	}
	if(j < res_len)
	{
		res[j] = 0;
		return j;
	}
	return -1;
}

int str2escape(const char* str,int len,char* res,int res_len)
{
	int i = 0,j = 0;
	for(;i < len; ++i)
	{
		if(IS_HTTP_ENESCAPE(*str))
		{
			if(j+3 >= res_len)
				return -1;
			res[j] = '%';
			res[j++] = HEX_STR[(*str & 0xF0)>>4];
			res[j++] = HEX_STR[(*str & 0x0F)];
		}
		else
		{
			if(j+1 >= res_len)
				return -1;
			res[j++] = *str;
		}
		++str;
	}
	if(j < res_len)
	{
		res[j] = 0;
		return j;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////
int parse_http_header_len(char* buf,uint16_t len,uint16_t* off)
{
	uint16_t i = *off;
	while(i < len)
	{
		if(!IS_HTTPURL_OK(buf[i]))
			return -1;
		if(buf[i] == '\n')
		{
			//there is not '\n' between buf[0] to buf[12]';
			if(i < 13)
			{
				return -1;
			}

			if(buf[i-2] == '\n')
			{
				if(buf[i-3] != '\r' || buf[i-1] != '\r')
					return -1;
				//http head length i+1
				*off = i+1;
				return i+1;
			}
		}
		++i;
	}
	*off = i;
	return 0;
}

int parse_http_get(char* buf,int len,char** path1,char** path2,char** values)
{
	if(buf[3] != ' ' || buf[4] != '/')
	{
		return -1;
	}
	//path
	*path1 = buf+5;
	int i = 5;
	while(++i < len)
	{
		if(buf[i] == '?')
		{
			buf[i] = 0;
			break;
		}
		else if(buf[i] == '/')
		{
			buf[i] = 0;
			*path2 = buf+i+1;
			continue;
		}
		if(!IS_HTTPPATH_OK(buf[i]))
		{
			return -1;
		}
	}
	//values
	int j = i;
	*values = buf+i+1;
	while(++i < len)
	{
		if(buf[i] == ' ')
		{
			buf[i] = 0;
			break;
		}
		if(!IS_HTTPVALUE_OK(buf[i]))
		{
			return -1;
		}
	}
	return str2escape(*values,i-j-1,*values,i-j);
}

int set_http_get(char* buf,int len,char* path,char* host,char* values,int value_len)
{
	int n = snprintf(buf,len,"GET /%s?",path);
	if(value_len == -1)
	{
		value_len = strlen(values);
	}
	if(n <= 0 || n+value_len >= len)
	{
		return -1;
	}
	int m = str2escape(values,value_len,buf+n,len-n);
	if(m == -1)
	{
		return -1;
	}
	n += m;
	m = snprintf(buf+n,len-m," HTTP/1.1\r\nHost: %s\r\n\r\n",host);
	if(m <= 0 || n+m >= len)
	{
		return -1;
	}
	return n+m;
}

/////////////////////////////////////////////////////////////////////////////////

int parse_http_post(char* buf,int len,char** path1,char** path2,int64_t* content_len)
{
	if(buf[4] != ' ' || buf[5] != '/')
		return -1;
	*path1 = buf+6;//POST /
	int i = 6;
	while(++i < len)
	{
		if(buf[i] == ' ')
		{
			buf[i] = 0;
			break;
		}
		else if(buf[i] == '/')
		{
			buf[i] = 0;
			*path2 = buf+i+1;
			continue;
		}
		if(!IS_HTTPPATH_OK(buf[i]))
		{
			return -1;
		}
	}
	while(++i < len)
	{
		if(buf[i] == 'C' && buf[i-1] == '\n' && i+16 < len
		&& strncmp(buf+i,"Content-Length: ",16) == 0)
		{
			*content_len = atol(buf+i+16);
			if(*content_len < 0)
				return -1;
		}
	}
	return 0;
}


int set_http_post(char* buf,int len,char* path,char* host,char* values,int values_len)
{
	ASSERT(len < 1024*1024*1024 && values_len < len);

	const char* request_str = "POST %s HTTP/1.1\r\nContent-Length: %d\r\nHost: %s\r\n\r\n";
	int n = snprintf(buf,len,request_str,path,values_len,host);
	ASSERT(n > 0 && n+values_len < len);
	memcpy(buf+n,values,values_len);
	return n+values_len;
}

int create_http_response_head(char* buf,int len,const char* result,char* server,int content_len)
{
	const char* responce_str = "HTTP/1.1 %s\r\nContent-Length: %d\r\nServer: %s\r\n\r\n";
	int n = snprintf(buf,len,responce_str,result,content_len,server);
	ASSERT(n > 0 && n < len);
	return n;
}


int parse_http_response(char* buf,int len,int64_t* value_len,int* result)
{
	if(len < 9){
		return -1;
	}
	int i = 9;//HTTP/1.1
	*result = atoi(buf+i);
	while(++i < len)
	{
		if(buf[i-1] == '\n' && buf[i] == 'C' && strncmp(buf+i,"Content-Length:",15) == 0)
		{
			*value_len = atol(buf+i+15);
			return 0;
		}
	}
	return -1;
}


int _strkey_get(const char** keys_list,char** values,int n,char* key)
{
	int i = 0;
	while(i < n)
	{
		if(values[i] == NULL && strcmp(keys_list[i],key) == 0)
			return i;
		++i;
	}
	return -1;
}

int parse_http_values(char* buf,int len,const char** keys,char** values,int n)
{
	int i,no = -1;
	char* key = buf;
	for(i = 1;i< len; ++i)
	{
		switch(buf[i-1])
		{
		case '=':
			buf[i-1] = 0;
			no = _strkey_get(keys,values,n,key);
			if(no != -1)
			{
				values[no] = buf+i;
			}
			break;
		case '&':
			if(no != -1)
			{
				buf[i-1] = 0;
			}
			key = buf+i;
			break;
		}
	}
	return 0;
}

int parse_http_values2(char* buf,int len,void** values,int (*get_index_f)(void*,const char*,void**),void* tb)
{
	int i,no = -1;
	char* key = buf;
	for(i = 1;i< len; ++i)
	{
		switch(buf[i-1])
		{
		case '=':
			buf[i-1] = 0;
			no = get_index_f(tb,(const char*)key,values);
			if(no != -1)
			{
				values[no] = buf+i;
			}
			break;
		case '&':
			if(no != -1)
			{
				buf[i-1] = 0;
			}
			key = buf+i;
			break;
		}
	}
	return 0;
}


