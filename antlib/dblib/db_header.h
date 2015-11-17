/*
 * db_header.h
 *
 *  Created on: 2014-10-11
 *      Author: lx
 */

#ifndef DB_HEADER_H_
#define DB_HEADER_H_

#include "commandlib/alloc.h"
#include "commandlib/double_list.h"
#include "commandlib/sys_header.h"
#include "commandlib/atomic_lock.h"
#include "commandlib/my_time.h"

enum enum_dbtype
{
	MYSQL_DB = 0,
};

typedef struct mydb_recordset
{
	void* _recod;
	struct db_connection* _conptr;
	int _row_num;
}mydb_recordset;

struct db_connection
{
	struct double_list_node _node;
	int _used;
	int _state;
	uint32_t _last_used_time;
	mydb_recordset _recordset;
};

enum enum_dbconnection_state
{
	DNCON_STATE_COLSE = 0,
	DNCON_STATE_CONNECT,
};


enum enum_dbconnection_type
{
	DNCON_TYPE_LONG = 0,
	DNCON_TYPE_SHORT,
};

struct db_connect_data
{
	char _dbhost[128];
	char _dbuser[32];
	char _dbpswd[32];
	char _dbname[32];
	int _dbport;
};


typedef struct db_connection* (*create_dbconnection)(struct db_connect_data* data,int type);

typedef int (*close_dbconnection)(struct db_connection* con);

typedef int (*do_dbconnectionsql)(struct db_connection* con,const char* sql,int len);

typedef char* (*get_dbrecord_value)(mydb_recordset* recordset,int no);

typedef int (*move_dbrecord_next)(mydb_recordset* recordset);

typedef void (*free_dbrecord)(mydb_recordset* recordset);

struct mydb_action
{
	create_dbconnection _create_fun;
	close_dbconnection _close_fun;
	do_dbconnectionsql _dosql_fun;
	get_dbrecord_value _getvl_fun;
	move_dbrecord_next _movenext_fun;
	free_dbrecord _free_fun;
};



#endif /* DB_HEADER_H_ */
