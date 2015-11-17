/*
 * server_config.h
 *
 *  Created on: Sep 17, 2015
 *      Author: zhaolixiang
 */

#ifndef SERVER_CONFIG_H_
#define SERVER_CONFIG_H_


//////////////////////////////////////////////////////
void* read_server_config(const char* file_name);
void close_server_config(void*);



//////////////////////////////////////////////////////
int config_server_id(void*);
int config_handlers_count(void*);

int config_tcp_threads_count(void*);
int config_work_threads_count(void*);

int config_log_level(void*);
const char* config_log_prev(void*);


const char* config_server_name(void*);
int config_server_cache_poolsize(void*);
int config_server_user_poolsize(void*);
const char* config_server_userkey(void*);
const char* config_server_usertable(void*);
int config_server_userserverid(void*);



int is_config_db(void*,int no);
const char* config_db_name(void*,int no);
const char* config_db_host(void*,int no);
int config_db_port(void*,int no);
const char* config_db_user(void*,int no);
const char* config_db_pswd(void*,int no);
int config_db_type(void*,int no);
int config_db_concount(void*,int no);

//////////////////////////////////////////////////////
int config_tcp_handlers_count(void*);

int is_config_master(void*);
const char* config_master_host(void*);
int config_master_port(void*);
int config_master_protocol(void*);


int is_config_cmd(void*);
const char* config_cmd_ip(void*);
int config_cmd_port(void*);
int config_cmd_protocol(void*);


int is_config_user(void*,int no);
const char* config_user_ip(void*,int no);
int config_user_port(void*,int no);
int config_user_protocol(void*,int no);


#endif /* SERVER_CONFIG_H_ */
