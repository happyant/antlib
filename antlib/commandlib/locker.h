/*
 * locker.h
 *
 *  Created on: Feb 12, 2015
 *      Author: zhaolixiang
 */

#ifndef LOCKER_H_
#define LOCKER_H_

#include "atomic_lock.h"


#define LOCKER struct locker

#define INITLOCK(l) init_wlock(&l)
#define WRITELOCK(l) write_wlock(&l)
#define WRITEUNLOCK(l) write_wunlock(&l)


struct locker
{
	int _write;
};

static inline void init_wlock(struct locker* l)
{
	l->_write = 0;
}

static inline void write_wlock(struct locker* l)
{
	while(LOCK_SET(l->_write,1) == 1)
	{}
}

static inline void write_wunlock(struct locker* l)
{
	LOCK_SET(l->_write,0);
}


#endif /* LOCKER_H_ */
