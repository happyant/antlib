/*
 * dblib.h
 *
 *  Created on: 2014-9-29
 *      Author: lx
 */

#ifndef DBLIB_H_
#define DBLIB_H_

#include "db_header.h"

typedef struct mydb
{
	struct db_connect_data _connect_data;
	int _state;
	int _contype;
	uint32_t _max_unused_time;
	struct double_list _con_list;
	struct mydb_action _data;
}mydb;


mydb* open_mydb(int dbtype,const char* sip,int port,const char* dbname,
		const char* user,const char* pswd,int con_count,int con_type);

void close_mydb(mydb* db);

int do_sql(mydb* db,const char* sql,int len);

mydb_recordset* get_recordset(mydb* db,const char* sql,int len);

void back_recordset(mydb* db,mydb_recordset* recordset);

char* get_recordset_value(mydb* db,mydb_recordset* recordset,int no);

int move_recordset_next(mydb* db,mydb_recordset* recordset);


#define DBFIELD_INT(db,set,no) (atoi(get_recordset_value(db,set,no)))
#define DBFIELD_UINT32(db,set,no) ((uint32_t)atoi(get_recordset_value(db,set,no)))
#define DBFIELD_UINT64(db,set,no) ((uint64_t)atoll(get_recordset_value(db,set,no)))
#define DBFIELD_STRING(db,set,no) (get_recordset_value(db,set,no))


#endif /* DBLIB_H_ */
