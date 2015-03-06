/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file compiler/xl/Compiler.h
 * \brief ???
 */

#ifndef __compiler_xl_Compiler_h__
#define __compiler_xl_Compiler_h__

// Here go things specific to this compiler family

#define FP_REGISTER(n) asm("f" #n)

/**
 * \todo I am not convinced this is the right way to handle gcc in xl - too much questionable casting.
 * plus, these XL instrinsics are specific to type and GCC are not. We need to
 * use (e.g.) __fetch_and_addlp() for long and __fetch_and_add() for int.
 * Also, __compare_and_swap() modifies param 2 while GCC does not.
 */

#include "builtins.h"

#define __sync_synchronize() __sync()

#ifdef __64BIT__ // =====================================================================
#define __sync_fetch_and_or(x,y)	__fetch_and_orlp((volatile unsigned long *)x,y)
#define __sync_fetch_and_and(x,y)	__fetch_and_andlp((volatile unsigned long *)x,y)
#define __sync_fetch_and_add(x,y)	__fetch_and_addlp((volatile long *)x,y)
#define __sync_fetch_and_sub(x,y)	__fetch_and_addlp((volatile long *)x,-(y))
#define __sync_fetch_and_xor(x,y)	({		\
	unsigned long _v,_y;				\
	do {						\
		_v = __ldarx((volatile long *)x);       \
	        _y = _v ^ (unsigned long)y;             \
	} while (!__stdcx((volatile long *)x, _y));     \
	_v;						\
})
#define __sync_fetch_and_nand(x,y)	({		\
	unsigned long _v, _y;				\
	do {						\
		_v = __ldarx((volatile long *)x);       \
		_y = ~_v & (unsigned long)y;            \
	} while (!__stdcx((volatile long *)x, _v));     \
	_v;						\
})
#define __sync_fetch_and_swap(x,y)	__fetch_and_swaplp((volatile unsigned long *)x,y)

#define __sync_bool_compare_and_swap(x,y,z) ({		\
	bool _b;					\
	long _y = (long)y;				\
	_b = __compare_and_swaplp((volatile long *)x,&_y,(long)z);\
	_b;						\
})
#define __sync_val_compare_and_swap(x,y,z) (__typeof__(*x))({\
	long _y = (long)y;				\
	__compare_and_swaplp((volatile long *)x,&_y,(long)z);\
	_y;						\
})
#define __sync_add_and_fetch(x,y)	({		\
	long _v;					\
	do {						\
		_v = __ldarx((volatile long *)x);	\
		_v = _v + (long)y;			\
	} while (!__stdcx((volatile long *)x, _v));	\
	_v;						\
})
#define __sync_sub_and_fetch(x,y)	({		\
	long _v;					\
	do {						\
		_v = __ldarx((volatile long *)x);	\
		_v = _v - (long)y;			\
	} while (!__stdcx((volatile long *)x, _v));	\
	_v;						\
})
#define __sync_or_and_fetch(x,y)	({		\
	unsigned long _v;				\
	do {						\
		_v = __ldarx((volatile long *)x);\
		_v = _v | (unsigned long)y;		\
	} while (!__stdcx((volatile long *)x, _v));\
	_v;						\
})
#define __sync_and_and_fetch(x,y)	({		\
	unsigned long _v;				\
	do {						\
		_v = __ldarx((volatile long *)x);\
		_v = _v & (unsigned long)y;		\
	} while (!__stdcx((volatile long *)x, _v));\
	_v;						\
})
#define __sync_xor_and_fetch(x,y)	({		\
	unsigned long _v;				\
	do {						\
		_v = __ldarx((volatile long *)x);\
		_v = _v ^ (unsigned long)y;		\
	} while (!__stdcx((volatile long *)x, _v));\
	_v;						\
})
#define __sync_nand_and_fetch(x,y)	({		\
	unsigned long _v;				\
	do {						\
		_v = __ldarx((volatile long *)x);\
		_v = ~_v & (unsigned long)y;		\
	} while (!__stdcx((volatile long *)x, _v));\
	_v;						\
})
#define __sync_lock_test_and_set(x,y)	__fetch_and_swaplp((long *)x, y)
#define __sync_lock_release(x)		__clear_lockd_mp((const long long *)x,0)

#else	// 32-bit ====================================================================

#define __sync_fetch_and_or(x,y)	__fetch_and_or((volatile unsigned int *)x,y)
#define __sync_fetch_and_and(x,y)	__fetch_and_and((volatile unsigned int *)x,y)
#define __sync_fetch_and_add(x,y)	__fetch_and_add((volatile int *)x,y)
#define __sync_fetch_and_sub(x,y)	__fetch_and_add((volatile int *)x,-(y))
#define __sync_fetch_and_xor(x,y)	({		\
	unsigned int _v,_y;				\
	do {						\
		_v = __lwarx((volatile int *)x);        \
		_y = _v ^ (unsigned int)y;	        \
	} while (!__stwcx((volatile int *)x, _y));      \
	_v;						\
})
#define __sync_fetch_and_nand(x,y)	({		\
	unsigned int _v,_y;				\
	do {						\
		_v = __lwarx((volatile int *)x);        \
		_y = ~_v & (unsigned int)y;             \
	} while (!__stwcx((volatile int *)x, _v));      \
	_v;						\
})
#define __sync_fetch_and_swap(x,y)	__fetch_and_swap((volatile int *)x,y)
#define __sync_bool_compare_and_swap(x,y,z) ({		\
	bool _b;					\
	int _y = (int)y;				\
	_b = __compare_and_swap((volatile int *)x,&_y,(int)z);\
	_b;						\
})
#define __sync_val_compare_and_swap(x,y,z) (__typeof__(*x))({\
	int _y = (int)y;				\
	__compare_and_swap((volatile int *)x,&_y,(int)z);\
	_y;						\
})
#define __sync_add_and_fetch(x,y)	({		\
	int _v;						\
	do {						\
		_v = __lwarx((volatile int *)x);	\
		_v = _v + (int)y;			\
	} while (!__stwcx((volatile int *)x, _v));	\
	_v;						\
})
#define __sync_sub_and_fetch(x,y)	({		\
	int _v;						\
	do {						\
		_v = __lwarx((volatile int *)x);	\
		_v = _v - (int)y;			\
	} while (!__stwcx((volatile int *)x, _v));	\
	_v;						\
})
#define __sync_or_and_fetch(x,y)	({		\
	unsigned int _v;				\
	do {						\
		_v = __lwarx((volatile int *)x);\
		_v = _v | (unsigned int)y;		\
	} while (!__stwcx((volatile int *)x, _v));\
	_v;						\
})
#define __sync_and_and_fetch(x,y)	({		\
	unsigned int _v;				\
	do {						\
		_v = __lwarx((volatile int *)x);\
		_v = _v & (unsigned int)y;		\
	} while (!__stwcx((volatile int *)x, _v));\
	_v;						\
})
#define __sync_xor_and_fetch(x,y)	({		\
	unsigned int _v;				\
	do {						\
		_v = __lwarx((volatile int *)x);\
		_v = _v ^ (unsigned int)y;		\
	} while (!__stwcx((volatile int *)x, _v));\
	_v;						\
})
#define __sync_nand_and_fetch(x,y)	({		\
	unsigned int _v;				\
	do {						\
		_v = __lwarx((volatile int *)x);\
		_v = ~_v & (unsigned int)y;		\
	} while (!__stwcx((volatile int *)x, _v));\
	_v;						\
})
#define __sync_lock_test_and_set(x,y)	__fetch_and_swap((volatile int *)x, y)
#define __sync_lock_release(x)		__clear_lock_mp((int *)x,0)

#endif	// 32/64-bit ==================================================================

#endif // __pami_compiler_xl_h__
