/*
 * net_protocol.c
 *
 *  Created on: Oct 31, 2015
 *      Author: zhaolixiang
 */

#include "net_protocol.h"
#include "user_list.h"
#include "user_msg.h"


void do_user_netmsg(handler_msg* msg,uint64_t userid,uint64_t netid)
{
	tcp_stream* s = (tcp_stream*)msg->_data;
	char* name = (char*)(s->_vl->_vl[0]);
	if(name && name[0] && name[1])
	{
		pushmsg_byname((const char*)(s->_vl->_vl[0]),msg);
	}
	else
	{
		close_tcpconnection(netid,"name error");
		free_handler_msg(msg);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void init_netprotocol()
{
	// http://url/user/login?name=admin&password=123456&type=1
	struct pm_tcpmsg_field login[3] = {{"name",0,3,32},{"password",0,6,32},{"type",1,1,3}};
	tcpmsg_regist_string("user","login",USERMSG_LOGIN,login,3,do_user_netmsg);

	// http://url/user/check?name=admin
	struct pm_tcpmsg_field check[1] = {{"name",0,3,32}};
	tcpmsg_regist_string("user","check",USERMSG_CHECK,check,1,do_user_netmsg);

	//http://url/user/add?name=admin&password&email=aa@shenyv.com&telphone=12345678901&cards=44a&realname=xiaoming
	struct pm_tcpmsg_field add[6] = {{"name",0,3,32},{"password",0,6,32},{"email",0,0,64},
			{"telphone",0,0,16},{"cards",0,0,32},{"realname",0,0,64}};
	tcpmsg_regist_string("user","add",USERMSG_ADD,add,6,do_user_netmsg);

	// http://url/user/password?name=admin&password=old&newpassword=new
	struct pm_tcpmsg_field password[3] = {{"name",0,3,32},{"password",0,6,32},{"newpassword",0,6,32}};
	tcpmsg_regist_string("user","password",USERMSG_UPDATEPASSWORD,password,3,do_user_netmsg);

	// http://url/user/update?email=&telphone=&cards=&realname=
	struct pm_tcpmsg_field update[4] = {{"email",0,6,64},{"telphone",0,7,16},{"cards",0,16,32},{"realname",0,1,64}};
	tcpmsg_regist_string("user","update",USERMSG_UPDATE,update,4,do_user_netmsg);
}
