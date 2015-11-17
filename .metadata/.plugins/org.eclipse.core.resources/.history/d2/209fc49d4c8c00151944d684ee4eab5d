/*
 * mysql_con.c
 *
 *  Created on: 2014-10-15
 *      Author: lx
 */


#include "mysql_con.h"
#include "mysql/mysql.h"

struct mysql_con_data
{
	MYSQL _mysql;
	MYSQL_ROW  _row;
	MYSQL_RES* _res_ptr;
	int _field_num;
};

void mysql_db_init(struct mydb_action* a)
{
	a->_create_fun = mysql_create_dbconnection;
	a->_close_fun = mysql_close_dbconnection;
	a->_dosql_fun = mysql_do_dbconnectionsql;
	a->_getvl_fun = mysql_get_dbrecord_value;
	a->_movenext_fun = mysql_move_dbrecord_next;
	a->_free_fun = mysql_free_dbrecord;
}

void _free_dbconnection(struct db_connection* con)
{
	mysql_free_dbrecord(&con->_recordset);
	my_free(con);
}

struct db_connection* mysql_create_dbconnection(struct db_connect_data* connect_data,int type)
{
	int size = sizeof(struct db_connection) + sizeof(struct mysql_con_data);
	struct db_connection* con = (struct db_connection*)my_malloc(size);
	memset(con,0,size);
	struct mysql_con_data* d = (struct mysql_con_data*)(con+1);
	con->_recordset._recod = d;

	if(mysql_init(&d->_mysql) == NULL)
	{
		_free_dbconnection(con);
		return NULL;
	}
	int flag = 1;
	if(mysql_options(&d->_mysql, MYSQL_OPT_RECONNECT, &flag) != 0)
	{
		mysql_close(&d->_mysql);
		_free_dbconnection(con);
		return NULL;
	}
	if (mysql_real_connect(&d->_mysql,connect_data->_dbhost,connect_data->_dbuser,connect_data->_dbpswd,
			connect_data->_dbname,connect_data->_dbport, NULL, 0) == NULL)
	{
		mysql_close(&d->_mysql);
		_free_dbconnection(con);
		return NULL;
	}
	mysql_set_character_set(&d->_mysql,"utf8");
	return con;
}



int mysql_close_dbconnection(struct db_connection* con)
{
	struct mysql_con_data* d = (struct mysql_con_data*)con->_recordset._recod;
	ASSERT(d);
	mysql_free_dbrecord(&con->_recordset);
	mysql_close(&d->_mysql);
	//_free_dbconnection(con);
	return 0;
}


int _mysql_recordset_reset(struct mysql_con_data* d)
{
	int row_num = (unsigned int) mysql_num_rows(d->_res_ptr);
	if (row_num > 0)
	{
		mysql_data_seek(d->_res_ptr, 0);
		d->_row = mysql_fetch_row(d->_res_ptr);
		d->_field_num = (int)mysql_num_fields(d->_res_ptr);
		return row_num;
	}
	else
	{
		d->_row = NULL;
	}
	return 0;
}

int mysql_do_dbconnectionsql(struct db_connection* con,const char* sql,int len)
{
	struct mysql_con_data* d = (struct mysql_con_data*)con->_recordset._recod;
	if(mysql_real_query(&d->_mysql,sql,len) == 0)
	{
		if(sql[0] != 'S' && sql[0] != 's')
		{
			return 0;
		}
		d->_res_ptr = mysql_store_result(&d->_mysql);
		if(d->_res_ptr)
		{
			con->_recordset._row_num = _mysql_recordset_reset(d);
			return 0;
		}
	}
	//mysql_error(&d->_mysql);
	return -1;
}


char* mysql_get_dbrecord_value(mydb_recordset* recordset,int no)
{
	struct mysql_con_data* d = (struct mysql_con_data*)recordset->_recod;
	if(d->_row && no >= 0 && no < d->_field_num)
	{
		return d->_row[no];
	}
	return NULL;
}


int mysql_move_dbrecord_next(mydb_recordset* recordset)
{
	struct mysql_con_data* d = (struct mysql_con_data*)recordset->_recod;
	if(d->_row)
	{
		d->_row = mysql_fetch_row(d->_res_ptr);
		return 0;
	}
	return -1;
}

void mysql_free_dbrecord(mydb_recordset* recordset)
{
	struct mysql_con_data* d = (struct mysql_con_data*)recordset->_recod;
	if(d->_res_ptr)
	{
		mysql_free_result(d->_res_ptr);
		d->_res_ptr = NULL;
	}
	if(d->_row)
	{
		d->_row = NULL;
	}
}
