/*
 * alloc.c
 *
 *  Created on: Feb 12, 2015
 *      Author: zhaolixiang
 */

#include "alloc.h"
#include "double_list.h"
#include "locker.h"
#include "my_time.h"

#ifdef _DEBUG_ALLOC

//32 bytes
struct alloc_debug_node
{
	struct double_list_node _link_node;
	const char* _file;
	int _line;
	int _size;
};


struct alloc_debug_list
{
	int _count;
	LOCKER _locker;
	struct double_list _link;
};

struct alloc_debug_list _g_alloc_debug_list = {0,{0},{NULL,NULL}};


void* _my_malloc(size_t size,const char* file,int line)
{
	struct alloc_debug_node* m = (struct alloc_debug_node*)malloc(sizeof(struct alloc_debug_node)+size);
	m->_file = file;
	m->_line = line;
	m->_size = (int)size;

	WRITELOCK(_g_alloc_debug_list._locker);
	double_list_add_tail(&_g_alloc_debug_list._link,&m->_link_node);
	_g_alloc_debug_list._count ++;
	WRITEUNLOCK(_g_alloc_debug_list._locker);

	return m+1;
}

void _my_free(void* p)
{
	struct alloc_debug_node* m = (struct alloc_debug_node*)p;
	--m;

	WRITELOCK(_g_alloc_debug_list._locker);
	double_list_remove(&_g_alloc_debug_list._link,&m->_link_node);
	_g_alloc_debug_list._count --;
	WRITEUNLOCK(_g_alloc_debug_list._locker);

	free(m);
}


void _write_alloc_debug_title(int fd,int count)
{
	int len;
	char buf[1024];
	SNPRINTF(len,buf,sizeof(buf),"========= %s not free count is %d =========\n",get_mydate(),count);
	write(fd,buf,len);
}

void _write_alloc_debug_log(int fd,const char* file,int line,int size)
{
	int len;
	char buf[1024];
	SNPRINTF(len,buf,sizeof(buf),"%s %d ,size %d\n",file,line,size);
	write(fd,buf,len);
}

void print_unfree_record()
{
	struct alloc_debug_node* m;
	struct double_list_node* node;
	int fd;
	WRITELOCK(_g_alloc_debug_list._locker);
	if(_g_alloc_debug_list._count > 0)
	{
		fd = open("alloc_debug.log",O_CREAT|O_WRONLY|O_APPEND,0666);
		assert(fd != -1);
		_write_alloc_debug_title(fd,_g_alloc_debug_list._count);
		node = (struct double_list_node*)_g_alloc_debug_list._link._head;
		while(node)
		{
			m = (struct alloc_debug_node*)node;
			_write_alloc_debug_log(fd,m->_file,m->_line,m->_size);
			node = node->_next;
		}
		close(fd);
	}
	WRITEUNLOCK(_g_alloc_debug_list._locker);
}

#endif
