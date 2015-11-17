/*
 * encry_book.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 * Email: zhaolixiang99@163.com
 */

#ifndef ENCRY_BOOK_H_
#define ENCRY_BOOK_H_

#include "sys_header.h"


struct encry_book
{
	uint8_t _enbook[256];
	uint8_t _debook[256];
	uint64_t _key;
};


void create_encrybooks(struct encry_book* list,int n);

uint64_t create_encrybook_key();

void get_encrybook(struct encry_book* book, uint64_t key);


//encrypt
static inline void encry(struct encry_book* book, uint8_t* datas, int dataslen)
{
	int i;
	for(i = 0; i < dataslen; ++i)
	{
		datas[i] = book->_enbook[(datas[i])];
	}
}

//decrypt
static inline void unencry(struct encry_book* book, uint8_t* datas, int dataslen)
{
	int i;
	for(i = 0; i < dataslen; ++i)
	{
		datas[i] = book->_debook[(datas[i])];
	}
}



#endif /* ENCRY_BOOK_H_ */
