/*
 * server_config.c
 *
 *  Created on: Sep 17, 2015
 *      Author: zhaolixiang
 */

#include "server_config.h"
#include "antlib/antlib.h"

//////////////////////////////////////////////////////
void* read_server_config(const char* file_name)
{
	return read_ini_file(file_name);
}
void close_server_config(void* f)
{
	close_ini_file(f);
}


//////////////////////////////////////////////////////
int config_server_id(void* f)
{
	return get_value_int(f,"handlerlib","service_id",1);
}
int config_handlers_count(void* f)
{
	return get_value_int(f,"handlerlib","handler_count",128);
}
int config_tcp_threads_count(void* f)
{
	return get_value_int(f,"tcplib","tcp_threads_num",1);
}
int config_work_threads_count(void* f)
{
	return get_value_int(f,"worklib","work_threads_num",1);
}
int config_log_level(void* f)
{
	return get_value_int(f,"logerlib","log_level",0);
}
const char* config_log_prev(void* f)
{
	return get_value_string(f,"logerlib","log_file_pre","");
}

const char* config_server_name(void* f)
{
	return get_value_string(f,"service","server_name","");
}
int config_server_cache_poolsize(void* f)
{
	return get_value_int(f,"service","cache_pool_size",256);
}


int config_server_user_poolsize(void* f)
{
	return get_value_int(f,"user","user_pool_size",1024);
}
const char* config_server_userkey(void* f)
{
	return get_value_string(f,"user","user_key","happy-ant");
}
const char* config_server_usertable(void* f)
{
	return get_value_string(f,"user","user_dbtable","user");
}
int config_server_userserverid(void* f)
{
	return get_value_int(f,"user","user_serverid",0);
}



int is_config_db(void* f,int no)
{
	char session[32];
	sprintf(session,"dblib%d",no);
	return is_ini_session(f,session);
}
const char* config_db_name(void* f,int no)
{
	char session[32];
	sprintf(session,"dblib%d",no);
	return get_value_string(f,session,"db_name","");
}
const char* config_db_host(void* f,int no)
{
	char session[32];
	sprintf(session,"dblib%d",no);
	return get_value_string(f,session,"db_host","");
}
int config_db_port(void* f,int no)
{
	char session[32];
	sprintf(session,"dblib%d",no);
	return get_value_int(f,session,"db_port",0);
}
const char* config_db_user(void* f,int no)
{
	char session[32];
	sprintf(session,"dblib%d",no);
	return get_value_string(f,session,"db_user","");
}
const char* config_db_pswd(void* f,int no)
{
	char session[32];
	sprintf(session,"dblib%d",no);
	return get_value_string(f,session,"db_pswd","");
}
int config_db_type(void* f,int no)
{
	char session[32];
	sprintf(session,"dblib%d",no);
	return get_value_int(f,session,"db_type",0);
}
int config_db_concount(void* f,int no)
{
	char session[32];
	sprintf(session,"dblib%d",no);
	return get_value_int(f,session,"db_concount",0);
}


//////////////////////////////////////////////////////
int config_tcp_handlers_count(void* f)
{
	return get_value_int(f,"tcplib","tcp_handlers_num",1);
}

int is_config_master(void* f)
{
	return is_ini_session(f,"mster_server");
}
const char* config_master_host(void* f)
{
	return get_value_string(f,"mster_server","server_host","master.shenyv.com");
}
int config_master_port(void* f)
{
	return get_value_int(f,"mster_server","server_port",80);
}
int config_master_protocol(void* f)
{
	return get_value_int(f,"mster_server","server_protocol",0);
}

int is_config_cmd(void* f)
{
	return is_ini_session(f,"tcp_cmd");
}
const char* config_cmd_ip(void* f)
{
	return get_value_string(f,"tcp_cmd","ip","127.0.0.1");
}
int config_cmd_port(void* f)
{
	return get_value_int(f,"tcp_cmd","port",6666);
}

int config_cmd_protocol(void* f)
{
	return get_value_int(f,"tcp_cmd","protocol",0);
}

////////////////////////////////////////////////////////////
int is_config_user(void* f,int no)
{
	char session[32];
	sprintf(session,"port_user%d",no);
	return is_ini_session(f,session);
}
const char* config_user_ip(void* f,int no)
{
	char session[32];
	sprintf(session,"port_user%d",no);
	return get_value_string(f,session,"ip","127.0.0.1");
}
int config_user_port(void* f,int no)
{
	char session[32];
	sprintf(session,"port_user%d",no);
	return get_value_int(f,session,"port",80);
}
int config_user_protocol(void* f,int no)
{
	char session[32];
	sprintf(session,"port_user%d",no);
	return get_value_int(f,session,"protocol",0);
}


