/*
 * atomic_lock.h
 *
 *  Created on: 2014-10-1
 *      Author: happy_ant
 */

#ifndef ATOMIC_LOCK_H_
#define ATOMIC_LOCK_H_



/* lock{
 * old =	l;
 * l	+=	n;
 * return old;}
 * */
#define LOCK_ADD(l,n) 	__sync_fetch_and_add(&l,n)

/*lock{
 * old	=	l;
 * l	-=	n;
 * return old; }
 * */
#define LOCK_SUB(l,n) 	__sync_fetch_and_sub(&l,n)

/*lock {
 * old = l;
 * l 	= n;
 * return old;}
 * */
#define LOCK_SET(l,n) 	__sync_lock_test_and_set(&l,n)


//lock{l = 0;}
#define LOCK_ZERO(l) 	__sync_lock_release(&l)

/*lock {
 * if (l==o"){l = n;return true;};
 * else {return false;} }
 * */
#define LOCK_SWAP(l,o,n)	__sync_bool_compare_and_swap(&l,o,n)





#endif /* ATOMIC_LOCK_H_ */
