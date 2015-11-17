/*
 * tcp_usermsg_list.c
 *
 *  Created on: Sep 15, 2015
 *      Author: zhaolixiang
 */

#include "tcp_msglist.h"
#include "tcp_stream.h"
#include "http_protocol.h"

struct tcpmsg_field
{
	char _name[32];
	int _min;
	int _max;
	int _type;
};

struct tcpmsg_table {
	char _key1[64];
	char _key2[64];
	do_tcpmsg _domsg_f;
	uint16_t _type;//0 is binary, 1 is string
	uint16_t _n;
	int _id;
	int _max_size;
	int _min_size;
	struct tcpmsg_field _fields[1];
};

struct tcpmsg_table_idlist {
	struct tcpmsg_table* _tables[65536];
	struct str_hash _hash;
};

struct tcpmsg_table_idlist* _g_tcpmsg_list = NULL;

/////////////////////////////////////////////////////////////////

void tcpmsg_libinit()
{
	ASSERT(_g_tcpmsg_list == NULL);
	_g_tcpmsg_list = my_malloc(sizeof(struct tcpmsg_table_idlist));
	memset(_g_tcpmsg_list,0,sizeof(struct tcpmsg_table_idlist));
	init_str_hash(&_g_tcpmsg_list->_hash);
}

void free_msgtable(void* node)
{
	my_free(node);
}

void tcpmsg_libuninit()
{
	ASSERT(_g_tcpmsg_list);
	int i;
	for(i = 0; i < 65536; i++)
	{
		if(_g_tcpmsg_list->_tables[i])
		{
			free_msgtable(_g_tcpmsg_list->_tables[i]);
		}
	}
	clear_str_hash(&_g_tcpmsg_list->_hash,free_msgtable);
	uninit_str_hash(&_g_tcpmsg_list->_hash);
	my_free(_g_tcpmsg_list);
	_g_tcpmsg_list = NULL;
}


struct tcpmsg_table* _tcpmsg_regist(int id,struct pm_tcpmsg_field* fields,int n,do_tcpmsg f)
{
	if(id <= 0 || id >= 65536 || _g_tcpmsg_list->_tables[id] || n < 0 || n > 1024)
		return NULL;
	int i;
	int size = sizeof(struct tcpmsg_table);
	if(n > 0) {
		size += sizeof(struct tcpmsg_field)*(n-1);
	}
	struct tcpmsg_table* t = (struct tcpmsg_table*)my_malloc(size);
	memset(t,0,size);
	t->_domsg_f = f;
	t->_id = id;
	t->_type = 0;
	t->_n = n;
	for(i = 0; i < n; i++)
	{
		t->_fields[i]._type = fields[i]._type;
		t->_fields[i]._min = fields[i]._min;
		t->_fields[i]._max = fields[i]._max;
		t->_min_size += fields[i]._min;
		t->_max_size += fields[i]._max;
		strncpy(t->_fields[i]._name,fields[i]._field,31);
	}
	return t;
}

int tcpmsg_regist_binary(int id,struct pm_tcpmsg_field* fields,int n,do_tcpmsg f)
{
	struct tcpmsg_table* t = _tcpmsg_regist(id,fields,n,f);
	if(t == NULL){
		return -1;
	}
	_g_tcpmsg_list->_tables[id] = t;
	return 0;
}

int tcpmsg_regist_string(const char* key1,const char* key2,int id,struct pm_tcpmsg_field* fields,int n,do_tcpmsg f)
{
	if(NULL == key1 || key1[0] == 0 || NULL==key2 || key2[0] == 0)
	{
		return -1;
	}
	struct tcpmsg_table* t = _tcpmsg_regist(id,fields,n,f);
	if(t == NULL)
	{
		return -1;
	}
	strncpy(t->_key1,key1,63);
	strncpy(t->_key2,key2,63);
	struct str_hash* list = (struct str_hash*)get_str_hash(&_g_tcpmsg_list->_hash,key1);
	if(NULL == list)
	{
		list = my_new(struct str_hash);
		init_str_hash(list);
		set_str_hash(&_g_tcpmsg_list->_hash,t->_key1,list);
	}
	else if(get_str_hash(list,key2))
	{
		my_free(t);
		return -1;
	}
	int i,len;
	for(i = 0; i < n; i++)
	{
		len = strlen(t->_fields[i]._name)+1;
		t->_min_size += len;
		t->_max_size += len;
	}
	if(n > 1) //add '&'
	{
		t->_min_size += (n-1);
		t->_max_size += (n-1);
	}
	_g_tcpmsg_list->_tables[id] = t;
	set_str_hash(list,t->_key2,t);
	return 0;
}

handler_msg* tcpmsg_getbyid(int msgid,int msglen,uint64_t userid,uint64_t netid,uint32_t ip)
{
	if(msgid < 0 || msgid > 65535)
	{
		return NULL;
	}
	struct tcpmsg_table* t = _g_tcpmsg_list->_tables[msgid];
	if(t)
	{
		if(msglen < t->_min_size  || msglen > t->_max_size)
			return NULL;
		return	create_tcpstream(userid,msgid,msglen,t,t->_n,netid,ip);
	}
	return NULL;
}

handler_msg* tcpmsg_getbykey(const char* key1,const char* key2,int msglen,uint64_t userid,uint64_t netid,uint32_t ip)
{
	if(key1 == NULL || key1[0] == 0 || key2 == NULL || key2[0] == 0)
	{
		return NULL;
	}
	struct str_hash* list = (struct str_hash*)get_str_hash(&_g_tcpmsg_list->_hash,key1);
	if(NULL == list)
	{
		return NULL;
	}
	struct tcpmsg_table* t = (struct tcpmsg_table*)get_str_hash(list,key2);
	if(NULL == t)
	{
		return NULL;
	}
	if(msglen < t->_min_size  || msglen > t->_max_size)
	{
		return NULL;
	}
	return create_tcpstream(userid,t->_id,msglen,t,t->_n,netid,ip);
}

int tcpmsg_field_index(void* tp,const char* field,void** vl)
{
	struct tcpmsg_table* t = (struct tcpmsg_table*)tp;
	int i = 0;
	while(i < t->_n)
	{
		if(vl[i] == NULL && strcmp(t->_fields[i]._name,field) == 0)
			return i;
		++i;
	}
	return -1;
}

int parse_stringmsg(tcp_stream* s)
{
	struct tcpmsg_table* t = (struct tcpmsg_table*)s->_tb;
	if(t->_n == 0)
	{
		return 0;
	}

	if(-1 == parse_http_values2(s->_buf,s->_len,(void*)(s->_vl->_vl),tcpmsg_field_index,t))
	{
		return -1;
	}

	int i,len;
	for(i = 0; i < t->_n; ++i)
	{
		if(s->_vl->_vl[i] == NULL)
		{
			return -1;
		}
		len = strlen(s->_vl->_vl[i]);
		if(len < t->_fields[i]._min || len >t->_fields[i]._max)
		{
			return -1;
		}
		if(t->_fields[i]._type != 0)
		{
			s->_vl->_vl[i] = (void*)((uint64_t)atol(s->_vl->_vl[i]));
		}
	}
	return t->_n;
}

int tcpmsg_putout_range(handler_msg* msg,char* addmsg,int addlen)
{
	ASSERT(addlen >= 0);
	int n;
	tcp_stream* s = (tcp_stream*)msg->_data;
	if(s->_len - s->_off <= addlen)
	{
		if(addmsg)
		{
			n =  s->_len-s->_off;
			memcpy(s->_buf+s->_off, addmsg, n);
		} else { n = 0;}
		s->_off = 0;
		if(parse_stringmsg(s) == -1)
		{
			return -2;
		}
		((struct tcpmsg_table*)(s->_tb))->_domsg_f(msg,msg->_userid,s->_netid);
		return n;//>=0
	}
	else if(addlen > 0)
	{
		memcpy(s->_buf+s->_off, addmsg, addlen);
		s->_off += addlen;
		return -1;
	}
	return -1;
}

int tcpmsg_type(void* tp)
{
	return ((struct tcpmsg_table*)tp)->_type;
}




