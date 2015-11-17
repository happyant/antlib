/*
 * loglib.h
 *
 *  Created on: 2014-10-10
 *      Author: lx
 */

#ifndef LOGER_H_
#define LOGER_H_


const char* get_log_date();

#define ERRORLOG(format,...) write_log("[ERROR]%s "format"<%s,%d>\n",get_log_date(),##__VA_ARGS__,__FILE__,__LINE__)
#define WARNLOG(format,...) write_log("[WARN]%s "format"<%s,%d>\n",get_log_date(),##__VA_ARGS__,__FILE__,__LINE__)
#define INFOLOG(format,...) write_log("[INFO]%s "format"<%s,%d>\n",get_log_date(),##__VA_ARGS__,__FILE__,__LINE__)
#define DEBUGLOG(format,...) write_log("[DEBUG]%s "format"<%s,%d>\n",get_log_date(),##__VA_ARGS__,__FILE__,__LINE__)

#define USERLOG(format,...) write_log(format"\n",##__VA_ARGS__)


void init_loglib(int groupid,int log_level,const char* logname_pre);

void uninit_loglib();

int write_log(const char* log,...);


#define SYSMSG_LOGLIB_WRITELOG 	101
#define SYSMSG_LOGLIB_CHANGEFILE 	102


#endif /* LOGER_H_ */
