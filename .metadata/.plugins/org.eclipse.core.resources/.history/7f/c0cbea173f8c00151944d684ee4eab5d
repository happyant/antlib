/*
 * tcp_protocol_flash.c
 *
 *  Created on: May 24, 2015
 *      Author: zhaolixiang
 */

#include "tcp_protocol_flash.h"

struct protocol_falsh_data
{
	char _send_msg[149];
	char _recv_msg[23];
	int _recv_len;
	int _send_len;
};

struct protocol_falsh_data _g_flash_data = {
		"<cross-domain-policy>"\
			"<site-control permitted-cross-domain-policies=\"master-only\"/>"\
			"<allow-access-from domain=\"*\" to-ports=\"*\"/>"\
		"</cross-domain-policy>",

		"<cross-domain-policy/>",
		22,
		149
};

//////////////////////////////////////////////////////////////////////////////////

void flash_connected(int result,struct tcp_connection* c,handler_msg* msg)
{

}
void flash_readed_data(int add,struct tcp_connection* c)
{
	if(c->_offset > _g_flash_data._recv_len)
	{
		tcpconnection_close(c,"flash error");
		return;
	}
	handler_msg* msg;
	c->_offset += add;
	if(c->_offset >= _g_flash_data._recv_len)
	{
		if(strncmp(c->_buf,_g_flash_data._recv_msg,_g_flash_data._recv_len) == 0)
		{
			msg = create_handler_msg(0);
			msg->_datalen = _g_flash_data._send_len;
			msg->_data = (void*)_g_flash_data._send_msg;
			tcpconnection_add_msg(c,msg);
		}
		else
		{
			tcpconnection_close(c,"flash read error");
		}
	}
}
void flash_do_msg(handler_msg* msg,struct tcp_connection* c)
{
	free_handler_msg(msg);
}
void flash_sended_msg(handler_msg* msg,struct tcp_connection* c)
{
	free_handler_msg(msg);
}
void flash_closed(struct tcp_connection* c)
{

}

void flash_protocol_init(tcp_protocol* p)
{
	p->_protocol_name = "flash";
	p->_connected = flash_connected;
	p->_readed_data = flash_readed_data;
	p->_do_msg = flash_do_msg;
	p->_sended_msg = flash_sended_msg;
	p->_closed = flash_closed;
}

