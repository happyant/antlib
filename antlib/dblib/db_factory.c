/*
 * db_factory.c
 *
 *  Created on: 2014-10-15
 *      Author: lx
 */


#include "db_factory.h"
#include "mysql_con.h"

int create_mydb(mydb* db,int type)
{
	switch(type)
	{
	case MYSQL_DB: mysql_db_init(&db->_data);
		return 0;
	default: break;
	}
	return -1;
}
