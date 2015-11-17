/*
 * hash.h
 *
 *  Created on: May 18, 2015
 *      Author: zhaolixiang
 */

#ifndef HASH_H_
#define HASH_H_


#define STRHASH_CAP_MAX 80
//a~z 0~9 A~Z
struct str_hash_node
{
	const char* _key;
	void* _vl;
	struct str_hash_node* _next;
};

struct str_hash
{
	int _count;
	struct str_hash_node* _cap[STRHASH_CAP_MAX];
};

void init_str_hash(struct str_hash* h);
void uninit_str_hash(struct str_hash* h);

void* get_str_hash(struct str_hash* h,const char* key);
int set_str_hash(struct str_hash* h,const char* key,void* vl);
void* del_str_hash(struct str_hash* h,const char* key);

void doall_strhash(struct str_hash* h,int (*dof)(void*,void*),void (*freef)(void*,void*),void* data);
void clear_str_hash(struct str_hash* h,void (*free_data)(void*));


#endif /* HASH_H_ */
