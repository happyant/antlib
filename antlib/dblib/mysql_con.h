/*
 * mysql_con.h
 *
 *  Created on: 2014-10-15
 *      Author: lx
 */

#ifndef MYSQL_CON_H_
#define MYSQL_CON_H_

#include "db_header.h"

void mysql_db_init(struct mydb_action* a);

struct db_connection* mysql_create_dbconnection(struct db_connect_data* data,int type);

int mysql_close_dbconnection(struct db_connection* con);

int mysql_do_dbconnectionsql(struct db_connection* con,const char* sql,int len);

char* mysql_get_dbrecord_value(mydb_recordset* recordset,int no);

int mysql_move_dbrecord_next(mydb_recordset* recordset);

void mysql_free_dbrecord(mydb_recordset* recordset);

#endif /* MYSQL_CON_H_ */
