/*
 * hash.c
 *
 *  Created on: May 18, 2015
 *      Author: zhaolixiang
 */

#include "hash.h"
#include "alloc.h"
#include "locker.h"

static int _g_is_keylist_init = 0;
static short _g_keylist[256];

void init_hash_master()
{
	if(LOCK_SET(_g_is_keylist_init,1))
	{
		return;
	}
	memset(_g_keylist,0,256);
	int i;
	int n = 1;
	for(i = 'a';i <= 'z'; i++)//26
	{
		_g_keylist[i] = n++;
	}
	for(i = 'A';i <= 'Z'; i++)//26
	{
		_g_keylist[i] = n++;
	}
	for(i = '0';i <= '9'; i++)//10
	{
		_g_keylist[i] = n++;
	}
	//18
	_g_keylist['!'] = n++;
	_g_keylist['@'] = n++;
	_g_keylist['#'] = n++;
	_g_keylist['$'] = n++;
	_g_keylist['%'] = n++;

	_g_keylist['^'] = n++;
	_g_keylist['='] = n++;
	_g_keylist['&'] = n++;
	_g_keylist['*'] = n++;
	_g_keylist['-'] = n++;
	_g_keylist['_'] = n++;

	_g_keylist['.'] = n++;
	_g_keylist['?'] = n++;
	_g_keylist[';'] = n++;
	_g_keylist[':'] = n++;
	_g_keylist[','] = n++;

	_g_keylist[' '] = n++;
	_g_keylist['\t'] = n++;
}//80

short get_strindex(const char* key)
{
	return (key && key[0]) ? _g_keylist[(uint8_t)(key[0])] : 0;
}

//////////////////////////////////////////////////////

void init_str_hash(struct str_hash* h)
{
	init_hash_master();
	memset(h,0,sizeof(*h));
}

void uninit_str_hash(struct str_hash* h)
{
	//memset(h,0,sizeof(*h));
}

void clear_str_hash(struct str_hash* h,void (*free_data)(void*))
{
	struct str_hash_node* rm;
	struct str_hash_node* node;
	int i;
	for(i = 0; i< STRHASH_CAP_MAX; i++)
	{
		node = h->_cap[i];
		while(node)
		{
			rm = node;
			node = node->_next;
			if(free_data){
				free_data(rm->_vl);
			}
			my_free(rm);
		}
	}
	memset(h,0,sizeof(*h));
}


void* get_str_hash(struct str_hash* h,const char* key)
{
	short index = get_strindex(key);
	if(index == -1)
	{
		return NULL;
	}
	struct str_hash_node* node = h->_cap[index];
	while(node)
	{
		if(strcmp(node->_key,key) == 0)
			return node->_vl;
		node = node->_next;
	}
	return NULL;
}

int set_str_hash(struct str_hash* h,const char* key,void* vl)
{
	short index = get_strindex(key);
	if(index == -1 || get_str_hash(h,key))
	{
		return -1;
	}
	struct str_hash_node* node = (struct str_hash_node*)my_malloc(sizeof(struct str_hash_node));
	node->_key = key;
	node->_vl = vl;

	node->_next = h->_cap[index];
	h->_cap[index] = node;
	h->_count ++;
	return 0;
}

void* del_str_hash(struct str_hash* h,const char* key)
{
	short index = get_strindex(key);
	if(index == -1 || NULL == h->_cap[index])
	{
		return NULL;
	}
	void* result;
	struct str_hash_node* rm;

	struct str_hash_node* node = h->_cap[index];
	if(strcmp(node->_key,key) == 0)
	{
		result = node->_vl;
		h->_cap[index] = h->_cap[index]->_next;
		my_free(node);
		return result;
	}
	while(node->_next)
	{
		if(strcmp(node->_next->_key,key) == 0)
		{
			rm = node->_next;
			result = rm->_vl;
			node->_next = node->_next->_next;
			my_free(rm);
			return result;
		}
		node = node->_next;
	}
	return NULL;

}

void doall_strhash(struct str_hash* h,int (*dof)(void*,void*),void (*freef)(void*,void*),void* data)
{
	struct str_hash_node* rm;
	struct str_hash_node* node;
	int i;
	for(i = 0; i< STRHASH_CAP_MAX; ++i)
	{
		if(h->_cap[i] == NULL)
		{
			continue;
		}
		node = h->_cap[i];
		while(node->_next)
		{
			if(dof(node->_next->_vl,data) != 0)
			{
				freef(node->_next->_vl,data);
				rm = node->_next;
				node->_next = node->_next->_next;
				my_free(rm);
			}
			else
			{
				node = node->_next;
			}
		}
		if(dof(h->_cap[i]->_vl,data) != 0)
		{
			rm = h->_cap[i];
			h->_cap[i] = h->_cap[i]->_next;
			freef(rm->_vl,data);
			my_free(rm);
		}
	}

}

