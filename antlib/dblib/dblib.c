/*
 * dblib.c
 *
 *  Created on: 2014-10-14
 *      Author: lx
 */


#include "dblib.h"
#include "db_factory.h"


struct db_connection* _get_dbconnection(mydb* db)
{
	uint32_t now_time = get_mytime();
	struct db_connection* con;
	struct double_list_node* node = db->_con_list._head;
	while(node)
	{
		con = (struct db_connection*)node;
		if(LOCK_SET(con->_used,1) == 0)
		{
			if(con->_last_used_time + db->_max_unused_time < now_time)
			{
				db->_data._close_fun(con);
			}
			else
			{
				con->_last_used_time = now_time;
				return con;
			}
		}
		node = node->_next;
	}
	return NULL;
}

void _back_dbconnection(mydb* db,struct db_connection* con)
{
	if(	db->_contype == DNCON_TYPE_LONG && con->_state == DNCON_STATE_CONNECT)
	{
		db->_data._free_fun(&con->_recordset);
	}
	else
	{
		db->_data._close_fun(con);
	}
	ASSERT(con->_used == 1);
	LOCK_ZERO(con->_used);
}

struct db_connection* _create_dbconnection(mydb* db)
{
	struct db_connection* con = db->_data._create_fun(&db->_connect_data,db->_contype);
	if(con)
	{
		con->_used = 0;
		con->_last_used_time = get_mytime();
		con->_recordset._conptr = con;
	}
	return con;
}

/////////////////////////////////////////////////////////////////////////
mydb* open_mydb(int dbtype,const char* sip,int port,const char* dbname,
		const char* user,const char* pswd,int con_count,int con_type)
{
	mydb* db = (mydb*)my_malloc(sizeof(mydb));
	memset(db,0,sizeof(*db));
	if(create_mydb(db,dbtype) != 0)
	{
		my_free(db);
		return NULL;
	}
	strncpy(db->_connect_data._dbhost,sip,sizeof(db->_connect_data._dbhost)-1);
	strncpy(db->_connect_data._dbname,dbname,sizeof(db->_connect_data._dbname)-1);
	strncpy(db->_connect_data._dbuser,user,sizeof(db->_connect_data._dbuser)-1);
	strncpy(db->_connect_data._dbpswd,pswd,sizeof(db->_connect_data._dbpswd)-1);
	db->_connect_data._dbport = port;
	db->_contype = con_type;
	db->_max_unused_time = 3600;

	int i = 0;
	struct db_connection* con;
	while(i++ < con_count)
	{
		con = _create_dbconnection(db);
		ASSERT(con);
		con->_used = 0;
		double_list_push(&db->_con_list,&con->_node);
	}
	return db;
}

void close_mydb(mydb* db)
{
	struct db_connection* con;
	ASSERT(db);
	con = (struct db_connection*)double_list_pop(&db->_con_list);
	while(con)
	{
		db->_data._close_fun(con);
		my_free(con);
		con = (struct db_connection*)double_list_pop(&db->_con_list);
	}
	my_free(db);
}

int do_sql(mydb* db,const char* sql,int len)
{
	int result = -1;
	struct db_connection* con = _get_dbconnection(db);
	if(con)
	{
		result = db->_data._dosql_fun(con,sql,len);
		_back_dbconnection(db,con);
	}
	return result;
}

mydb_recordset* get_recordset(mydb* db,const char* sql,int len)
{
	struct db_connection* con = _get_dbconnection(db);
	if(con)
	{
		if(0 == db->_data._dosql_fun(con,sql,len))
			return &con->_recordset;
		_back_dbconnection(db,con);
	}
	return NULL;
}

void back_recordset(mydb* db,mydb_recordset* recordset)
{
	ASSERT(recordset->_conptr);
	_back_dbconnection(db,recordset->_conptr);
}

char* get_recordset_value(mydb* db,mydb_recordset* recordset,int no)
{
	return db->_data._getvl_fun(recordset,no);
}

int move_recordset_next(mydb* db,mydb_recordset* recordset)
{
	return db->_data._movenext_fun(recordset);
}
