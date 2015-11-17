/*
 * user_msg.c
 *
 *  Created on: Nov 14, 2015
 *      Author: zhaolixiang
 */
#include "user_msg.h"
#include "server_db.h"
#include "user_list.h"


struct user_server
{
	char _key[32];
	char _table[32];
	int _id;
	uint64_t _maxid;
};

struct user_server _g_user_server = {"","",0,0};


int set_serverkey(const char* key,const char* table,int svrid)
{
	memset(&_g_user_server,0,sizeof(_g_user_server));
	strncpy(_g_user_server._key,key,31);
	strncpy(_g_user_server._table,table,31);
	_g_user_server._id = svrid;

	char sql[256];
	int n = sprintf(sql,"select max(id) from %s",_g_user_server._table);
	mydb* db = get_mydb(0);
	mydb_recordset* rset = get_recordset(db,sql,n);
	if(rset)
	{
		if(rset->_row_num > 0)
		{
			_g_user_server._maxid = DBFIELD_UINT64(db,rset,0);
			if((int)(_g_user_server._maxid>>32) != _g_user_server._id)
				return -1;
		}
		back_recordset(db,rset);
	}
	_g_user_server._maxid += 1;
	return 0;
}

struct user_account* _load_user_cache(char* name,void* g)
{
	to_smallwords(name);
	struct user_account* user = get_user_byname(name,g);
	if(NULL == user)
	{
		user = get_user_cache(g);
		strncpy(user->_name,name,31);
		user->_state = -1;

		char sql[256];
		int n = snprintf(sql,sizeof(sql),"select pswd,state,id from %s where name='%s'",_g_user_server._table,name);

		mydb* db = get_mydb(0);
		mydb_recordset* rset = get_recordset(db,sql,n);
		if(rset)
		{
			if(rset->_row_num > 0)
			{
				strncpy(user->_password,DBFIELD_STRING(db,rset,0),32);
				user->_state = (char)DBFIELD_INT(db,rset,1);
				user->_id =  DBFIELD_UINT64(db,rset,2);
			}
			back_recordset(db,rset);
		}
		add_user(user,g);
	}
	return user;
}


//////////////////////////////////////////////////////////////////////////////

void handle_usermsg_login(handler_msg* msg,void* user_group)
{
	char* name = (char*)MSGINFO_FIELD(msg,0);
	char* password = (char*)MSGINFO_FIELD(msg,1);
	tcp_stream* info = (tcp_stream*)msg->_data;
	struct user_account* user = _load_user_cache(name,user_group);
	int result,n;
	uint32_t t = get_mytime();
	char pswd[128]="";
	//0 OK,1 password error,2 user limit used,3 user not exist
	if(user->_state == -1)
	{
		result = 3;
	} else if(user->_time > t) {
		result = 2;
		t = user->_time - t;
	} else {
		MYMD5_ENCRYPT2((uint8_t*)user->_name,strlen(user->_name),(uint8_t*)password,strlen(password),pswd);
		if(memcmp(user->_password,pswd,32) == 0)
		{
			n = sprintf(pswd,"%s:%lu:%u",_g_user_server._key,user->_id,t);
			MD5((uint8_t*)pswd,n,pswd);
			result = 0;
		} else { result = 1; }
	}
	//
	if(result == 0)
	{
		info->_len = sprintf(info->_buf,"{\"msg\":%d,\"result\":0,\"id\":%lu,\"key\":\"%s\",\"time\":%u}",
				USERMSG_LOGIN_BACK,user->_id,pswd,t);
	}
	else
	{
		info->_len = sprintf(info->_buf,"{\"msg\":%d,\"result\":%d,\"id\":0,\"key\":\"\",\"time\":%u}",
				USERMSG_LOGIN_BACK,result,result==2?t:0);
	}
	msg->_msgid = 0;
	USERLOG("%s [login]name=%s,result=%d,ip=%s",get_log_date(),user->_name,result,to_strip(info->_ip));
	tcp_sendmsg(info->_netid,msg);
}



void handle_usermsg_check(handler_msg* msg,void* user_group)
{
	char* name = (char*)MSGINFO_FIELD(msg,0);
	tcp_stream* info = (tcp_stream*)msg->_data;
	struct user_account* user = _load_user_cache(name,user_group);
	info->_len = sprintf(info->_buf,"{\"msg\":%d,\"result\":%d}",USERMSG_CHECK_BACK,user->_state == -1?0:1);
	msg->_msgid = 0;
	tcp_sendmsg(info->_netid,msg);
}


void handle_usermsg_add(handler_msg* msg,void* user_group)
{
	char* name = (char*)MSGINFO_FIELD(msg,0);
	char* password = (char*)MSGINFO_FIELD(msg,1);
	char pswd[33];
	tcp_stream* info = (tcp_stream*)msg->_data;

	int result = 1;
	struct user_account* user = _load_user_cache(name,user_group);
	if(user->_state == -1)
	{
		char sql[1024];
		uint64_t id = LOCK_ADD(_g_user_server._maxid,1);
		MYMD5_ENCRYPT2((uint8_t*)name,strlen(name),(uint8_t*)password,strlen(password),pswd);
		int n = snprintf(sql,sizeof(sql),"insert into %s (id,name,pswd,state) values (%lu,\'%s\',\'%s\',0)",
				_g_user_server._table,id,name,pswd);

		mydb* db = get_mydb(0);
		if(0 == do_sql(db,sql,n))
		{
			user->_state = 0;
			user->_time = 0;
			user->_count = 0;
			strncpy(user->_password,pswd,32);
			result = 0;
		}
		if(result == 0)
		{
			char* email = (char*)MSGINFO_FIELD(msg,2);
			char* telphone = (char*)MSGINFO_FIELD(msg,3);
			char* cards = (char*)MSGINFO_FIELD(msg,4);
			char* realname = (char*)MSGINFO_FIELD(msg,5);

			n = snprintf(sql,sizeof(sql),"insert into %s_data (id,email,telphone,cards,realname) values (%lu,\'%s\',\'%s\',\'%s\',\'%s\')",
					_g_user_server._table,id,email,telphone,cards,realname);
			do_sql(db,sql,n);
		}
	}
	USERLOG("%s [add]name=%s,result=%d,ip=%s",get_log_date(),user->_name,result,to_strip(info->_ip));

	info->_len = sprintf(info->_buf,"{\"msg\":%d,\"result\":%d}",USERMSG_ADD_BACK,result);
	msg->_msgid = 0;
	tcp_sendmsg(info->_netid,msg);
}

void handle_usermsg_update(handler_msg* msg,void* user_group)
{

}

void handle_usermsg_updatepassword(handler_msg* msg,void* user_group)
{
	char* name = (char*)MSGINFO_FIELD(msg,0);
	char* password = (char*)MSGINFO_FIELD(msg,1);
	char* newpassword = (char*)MSGINFO_FIELD(msg,2);
	char pswd[33];
	int result = 1;
	tcp_stream* info = (tcp_stream*)msg->_data;
	struct user_account* user = _load_user_cache(name,user_group);
	MYMD5_ENCRYPT2((uint8_t*)name,strlen(name),(uint8_t*)password,strlen(password),pswd);
	if(memcmp(user->_password,pswd,32) == 0)
	{
		char sql[256];
		MYMD5_ENCRYPT2((uint8_t*)name,strlen(name),(uint8_t*)newpassword,strlen(newpassword),pswd);
		int n = snprintf(sql,sizeof(sql),"update %s set pswd=\'%s\' where name=\'%s\';",
					_g_user_server._table,pswd,name);
		mydb* db = get_mydb(0);
		if(0 == do_sql(db,sql,n))
		{
			strncpy(user->_password,pswd,32);
			result = 0;
		}
	}
	info->_len = sprintf(info->_buf,"{\"msg\":%d,\"result\":%d}",USERMSG_UPDATEPASSWORD_BACK,result);
	msg->_msgid = 0;
	tcp_sendmsg(info->_netid,msg);
}

