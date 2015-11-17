/*
 * user_msg.h
 *
 *  Created on: Nov 14, 2015
 *      Author: zhaolixiang
 */

#ifndef USER_MSG_H_
#define USER_MSG_H_

#include "antlib/antlib.h"


int set_serverkey(const char* key,const char* table,int id);

#define USERMSG_LOGIN 	TCPMSGID(1,1)
#define USERMSG_LOGIN_BACK 	TCPMSGID(1,2)

#define USERMSG_CHECK	TCPMSGID(1,3)
#define USERMSG_CHECK_BACK	TCPMSGID(1,4)

#define USERMSG_ADD 	TCPMSGID(1,5)
#define USERMSG_ADD_BACK 	TCPMSGID(1,6)

#define USERMSG_UPDATE 	TCPMSGID(1,7)
#define USERMSG_UPDATE_BACK 	TCPMSGID(1,8)

#define USERMSG_UPDATEPASSWORD 	TCPMSGID(1,9)
#define USERMSG_UPDATEPASSWORD_BACK 	TCPMSGID(1,10)


void handle_usermsg_login(handler_msg* msg,void* user_group);
void handle_usermsg_check(handler_msg* msg,void* user_group);
void handle_usermsg_add(handler_msg* msg,void* user_group);
void handle_usermsg_update(handler_msg* msg,void* user_group);
void handle_usermsg_updatepassword(handler_msg* msg,void* user_group);


#endif /* USER_MSG_H_ */
