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

#ifndef _FIRMWARE_SIMSUPPORT_H_ // Prevent multiple inclusion
#define _FIRMWARE_SIMSUPPORT_H_

#ifndef __KERNEL__
#define __KERNEL__
#endif

#if !defined(__ASSEMBLY__)

#include <firmware/include/personality.h>

// The following routines are intended for Simulation
extern void __NORETURN Terminate( int status ); // silently and immediately terminate the simulation run.

extern void __NORETURN Sleep_Forever( void );   // stop the calling thread forever: all interrupts disabled and MSR_WE set.

extern int printf_unlocked( const char *fmt, ... );

extern int put( const char *str );

extern int  sim_puts( const char *str );
extern void uart_puts( const char *str );
extern void uart_putc( int c );
extern void  __NORETURN sim_exit( int status );

extern Personality_t *PersonalityPtr( void );

#define SPECFW_ConvertToSpeculativeAddress(x) ((void*) (((uint64_t)x) | 0x8000000000000000LL))
#define SPECFW_ConvertToAlias(x, aliasid) ((void*) (0x1040000000LL * (aliasid) + (uint64_t)(x)))

#endif // __ASSEMBLY__

#endif // Add nothing below this line.
