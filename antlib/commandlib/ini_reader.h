/*
 * inireader.h
 *
 *  Created on: 2014-10-17
 *      Author: lx
 */

#ifndef INI_READER_H_
#define INI_READER_H_


void* read_ini_file(const char* file_name);
int get_value_int(void* f,const char* session, const char* key,int def);
int is_ini_session(void* f, const char* session);
const char* get_value_string(void* f, const char* session, const char* key,const char* def);
void close_ini_file(void* f);


#endif /* INIREADER_H_ */
