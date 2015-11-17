/*
 * server_db.h
 *
 *  Created on: Nov 2, 2015
 *      Author: zhaolixiang
 */

#ifndef SERVER_DB_H_
#define SERVER_DB_H_

#include "antlib/antlib.h"

void init_mydb(void* config,int no);

mydb* get_mydb(int no);


#endif /* SERVER_DB_H_ */
