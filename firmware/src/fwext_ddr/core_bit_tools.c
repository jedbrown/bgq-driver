/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#include <firmware/include/fwext/sd_init_auto.h>

// args
// 1 - ibm bit number
// returns single bit value
inline uint64_t  get_ibm_bit64 (uint64_t val) 
{
	return (0x8000000000000000 >> val);
}

// args
// 1 - ibm bit number
// returns single bit value
inline uint16_t  get_std_bit16 (uint16_t val) 
{
	return (1 << val);
}

// args
// 1 - value
// 2 - rightmost bit number, ibm notation
// returns value shifted up to rightmost bit position
inline uint64_t ibm_move64_upto (uint64_t val, int bit) 
{
	return val << (63-bit);
}

// args
// 1 - value
// 2 - rightmost bit number, std notation
// returns value shifted up to rightmost bit position
inline uint64_t std_move_upto (uint64_t val, int bit) 
{
	return val << (bit);
}

//1 = size
//ret mask
inline uint64_t get_mask64 (int size) 
{
	int i;
	uint64_t v=0;

	for (i=0; i<size; i++) {
		v <<= 1;
		v  |= 1;
	}
	return v;
}

// args
// 1 - size
// 2 - ibm bit number (rightmost)
inline uint64_t get_ibm_mask64 (int size, int bitnum) 
{
	return ibm_move64_upto(get_mask64(size), bitnum);
}

//1 = value
//2 = from
//3 = to
inline uint64_t get_ibm_range64 (uint64_t val, int from, int to) 
{
	int size;
	int shift;
	uint64_t mask;
	
	size=to-from+1;
	shift=63-to;
	mask=get_mask64(size);

	if (size > 32) {
		Terminate(1);
	}

	val >>= shift;
	val  &= mask;
	return val;
}

//64bit always (tuned for dcr)
//Arg
// 1 - array (mask size, mask position in ibm notation)
inline uint64_t quickmask (int *array) 
{
	return get_ibm_mask64(array[0], array[1]);
}

//64bit always (tuned for dcr)
//Arg
// 1 - array (mask size, mask position in ibm notation)
inline uint64_t quickmask1 (int *array) 
{
	return get_ibm_bit64(array[1]);
}

//64bit always (tuned for dcr)
//Arg
// 1 - value
// 2 - array (mask size, mask position in ibm notation)
inline uint64_t quickval (uint64_t val, int *array) 
{
	return ibm_move64_upto(val, array[1]);
}

//64bit always (tuned for dcr)
//Arg
// 1 - value
// 2 - array (mask size, mask position in ibm notation)
inline uint64_t quickget (uint64_t val, int *array) 
{
	return get_mask64(array[0]) & (val >> (63-array[1]));
}
