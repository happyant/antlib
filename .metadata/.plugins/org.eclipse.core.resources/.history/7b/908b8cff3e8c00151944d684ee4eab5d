/*
 * loglib.c
 *
 *  Created on: 2014-10-10
 *      Author: lx
 */



#include "loger.h"
#include "handlerlib.h"

const char* get_log_date()
{
	return get_mydate();
}
/////////////////////////////////////////////////////////////////////////////////////

#include "commandlib/command_header.h"


#define LOGBUF_MAX 8192

//////////////////////////////////////////////////////////////////////////////////////

inline int open_logfile(const char* name)
{
	return open(name,O_CREAT|O_WRONLY|O_APPEND,0666);
}

inline int wite_logfile(int fd,char* buf,int len)
{
	if(fd == ERROR_FD)
		return -1;
	return write(fd,buf,len);
}

inline void close_logfile(int fd)
{
	if(fd != ERROR_FD)
		close(fd);
}

inline int device_free_space(const char* path,uint64_t* free_bytes)
{
	struct statfs sfs;
	if (0 != statfs(path, &sfs))
		return -1;

	*free_bytes = (uint64_t)sfs.f_bavail * sfs.f_bsize;
	return 0;
}

inline void _create_log_name(const char* pre,char* buf,int len)
{
	time_t t = time(0);
	struct tm l;
	localtime_r(&t,&l);
	int off = sprintf(buf,"%s%d%02d%02d_",pre,l.tm_year+1900,l.tm_mon+1,l.tm_mday);
	int i = 0;
	while(true)
	{
		sprintf(buf+off,"%d.log",i);
		if(i >= 99 || access(buf,0) == -1)
			break;
		++i;
	}
}

//////////////////////////////////////////////////////////////////////////////////////

struct loglib {
	uint32_t _handlerid;
	int _logfd;
	int _level;
	char _logname_pre[16];
};

struct loglib _g_loglib = {0,ERROR_FD,0,""};

//////////////////////////////////////////////////////////////////////////////////////

void log_set_changelog_timer(int sec)
{
	if(sec == 0)
	{
		sec = (int)get_next_hour_sec(0);
		if(sec < 600){sec += 24*3600;}
	}
	handler_msg* msg = create_handler_msg(0);
	msg->_datalen = 0;
	msg->_msgid = SYSMSG_LOGLIB_CHANGEFILE;
	add_timer(sec,_g_loglib._handlerid,msg);
}

void log_change_file()
{
	char buf[256];
	_create_log_name(_g_loglib._logname_pre,buf,sizeof(buf));
	int fd = open_logfile(buf);
	if(fd != ERROR_FD)
	{
		close_logfile(_g_loglib._logfd);
		_g_loglib._logfd = fd;
	}
}

void log_handler_msg(handler_msg* msg,void* data,uint32_t hid)
{
	ASSERT(&_g_loglib == data);
	ASSERT(msg->_datalen <= LOGBUF_MAX);
	if(msg->_msgid == SYSMSG_LOGLIB_WRITELOG)
	{
		wite_logfile(_g_loglib._logfd,(char*)msg->_data,msg->_datalen);
		return;
	}
	ASSERT(msg->_msgid == SYSMSG_LOGLIB_CHANGEFILE);
	log_change_file();
	log_set_changelog_timer(24*3600);
}

///////////////////////////////////////////////////////////////////////////////////////////

void init_loglib(int groupid,int log_level,const char* logname_pre)
{
	ASSERT(_g_loglib._handlerid == 0);
	_g_loglib._handlerid = register_handler(groupid,
			"log_hander",
			log_handler_msg,
			NULL,
			NULL,
			&_g_loglib);
	STRNCPY(_g_loglib._logname_pre,logname_pre,16);
	log_change_file();

	if(log_level >= 0 && log_level < 5)
	{
		_g_loglib._level = log_level;
	}
	ASSERT(_g_loglib._handlerid  > 0 && _g_loglib._logfd != ERROR_FD);
	log_set_changelog_timer(0);
}

void uninit_loglib()
{
	ASSERT(_g_loglib._logfd != ERROR_FD);
	close_logfile(_g_loglib._logfd);
	_g_loglib._logfd = ERROR_FD;
}

int write_log(const char* log,...)
{
	ASSERT(_g_loglib._handlerid > 0);

	handler_msg* msg = create_handler_msg(LOGBUF_MAX);
	char* buf = (char*)msg->_data;
	va_list ap;
	va_start(ap, log);
	int off = vsnprintf(buf,LOGBUF_MAX,log,ap);
	va_end(ap);
	if(off <= 0)
	{
		free_handler_msg(msg);
		return -1;
	}
	if(off > LOGBUF_MAX-1)
	{
		off = LOGBUF_MAX;
		buf[LOGBUF_MAX-1] = '\n';
	}
	msg->_datalen = off;
	msg->_msgid = SYSMSG_LOGLIB_WRITELOG;

	send_handler_msg(_g_loglib._handlerid,msg);
	return 0;
}
