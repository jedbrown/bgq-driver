/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


/**********************************************************************/
/* This file contains type definitions, manifest constants, and       */
/* subroutine prototypes for the PTR library.  Other platform-        */
/* dependent information is also provided. 			      */ 
/**********************************************************************/
/**********************************************************************/
/* Note to Working Group: This is a draft template for "ctimer.h". It */
/* is only meant to initiate discussion and is not the final version. */
/**********************************************************************/

#ifndef _PTOOLS_PTR_IBM_H_
#define _PTOOLS_PTR_IBM_H_

#ifdef __cplusplus	/* C++ */
extern "C" {
#endif			/* C++ */

#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
    
/********************/
/* Type definitions */
/********************/
typedef unsigned long   PTR_wall_timer_val;	/* Opaque type for Wallclock time  */
typedef struct tms	PTR_usr_timer_val;  /* Opaque type for User CPU time   */
typedef struct tms	PTR_sys_timer_val;  /* Opaque type for System CPU time */

/* NOTE TO WORKING GROUP: For platforms that have information about 
   rollover count these type definitions would be structs that can 
   store both the register value and the number of rollovers. */
/*====================================================================*/

/************************/
/* Manifest Constants:  */
/************************/

/*-----------------------------------------------------------------*/
/* Nominal tick period  (in secs): values supplied by the Vendor   */
/*-----------------------------------------------------------------*/
#define 	PTR_WALL_NOM_TICK	((double)1/(1.6*1000*1000*1000))
#define 	PTR_USR_NOM_TICK	((double)1/sysconf(_SC_CLK_TCK))
#define 	PTR_SYS_NOM_TICK	((double)1/sysconf(_SC_CLK_TCK))

/*-----------------------------------------------------------------*/
/* Calibrated tick period (in secs): calibrated values 	      	   */
/*-----------------------------------------------------------------*/
#define 	PTR_WALL_CAL_TICK	((double)1/(1.6*1000*1000*1000))
#define 	PTR_USR_CAL_TICK	((double)1/sysconf(_SC_CLK_TCK))
#define 	PTR_SYS_CAL_TICK	((double)1/sysconf(_SC_CLK_TCK))

/*-----------------------------------------------------------------*/
/* Resolution (in secs): values supplied by the vendor             */
/*-----------------------------------------------------------------*/
#define 	PTR_WALL_RESOLUTION	PTR_WALL_NOM_TICK
#define 	PTR_USR_RESOLUTION	PTR_USR_NOM_TICK
#define 	PTR_SYS_RESOLUTION	PTR_SYS_NOM_TICK

/*-----------------------------------------------------------------*/
/* Rollover period (in secs): values supplied by the Vendor        */
/*-----------------------------------------------------------------*/
#define 	PTR_WALL_ROLLOVER	.9999999E-09
#define 	PTR_USR_ROLLOVER	.9999999E-09
#define 	PTR_SYS_ROLLOVER	.9999999E-09

/*-----------------------------------------------------------------*/
/* Overhead (in secs): values supplied by the Vendor               */ 
/*-----------------------------------------------------------------*/
#define 	PTR_WALL_OVERHEAD	0.25E-06
#define 	PTR_USR_OVERHEAD	1.5E-06
#define 	PTR_SYS_OVERHEAD	1.5E-06

/*-----------------------------------------------------------------*/
/* Status codes returned by the subroutines 			   */
/*-----------------------------------------------------------------*/
#define		PTR_NOT_AVAILABLE	-1
#define		PTR_ROLLOVER_DETECTED	-2
#define		PTR_CANNOT_CALIBRATE	-3
#define		PTR_OTHER_ERROR		-4
/*====================================================================*/

/*************************/
/* Subroutine prototypes */ 
/*************************/
/*-----------------------------------------------------------------*/
/* The following macro is used to deal with non-ANSI compilers.    */
/*-----------------------------------------------------------------*/
#ifdef __FUNC_PROTO__
#undef __FUNC_PROTO__
#endif
#if defined(__STDC__) || defined(__cplusplus)
#define __FUNC_PROTO__(x) x
#else
#define __FUNC_PROTO__(x) ()
#endif

/*-----------------------------------------------------------------*/
/* Subroutines for runtime calibration for the particular platform */
/* Argument#1 -> PTR_???_NOM_TICK, or 0. If 0 calibrate and return */
/* 		     value. ??? will be replaced by WALL, USR or SYS. */
/* Argument#2 -> Time to spend for calibration (in secs) 	   */
/* Argument#3 -> Status code; returned by subroutine.	      	   */
/*-----------------------------------------------------------------*/
extern void PTR_Init_wall_timer __FUNC_PROTO__(
                         (double *PTR_runtime_wall_ticksize, 
		          double init_level,
		          int *PTR_wall_stat ));

extern void PTR_Init_usr_timer __FUNC_PROTO__(
			(double *PTR_runtime_usr_ticksize, 
		         double init_level,
		         int *PTR_usr_stat ));

extern void PTR_Init_sys_timer __FUNC_PROTO__(
			(double *PTR_runtime_sys_ticksize, 
		         double init_level, 
		         int *PTR_sys_stat ));

/*-----------------------------------------------------------------*/
/* Subroutines to get the opaque timer values                      */
/* Argument#1 -> Variable to hold timer value                      */
/*-----------------------------------------------------------------*/
extern void PTR_Get_wall_time __FUNC_PROTO__(
				(PTR_wall_timer_val *wall_var));

extern void PTR_Get_usr_time __FUNC_PROTO__(
				(PTR_usr_timer_val *usr_var));

extern void PTR_Get_sys_time __FUNC_PROTO__(
				(PTR_sys_timer_val *sys_var));

/*-----------------------------------------------------------------*/
/* Subroutines for calculating intervals between opaque values     */
/* Argument#1 -> First timer value 		      		   */
/* Argument#2 -> Second timer value 				   */
/* Argument#3 -> Interval value returned (in seconds)	           */
/* Argument#4 -> Status code returned by subroutine		   */
/*-----------------------------------------------------------------*/   
extern void PTR_Get_wall_interval __FUNC_PROTO__(
			   (PTR_wall_timer_val *wall_start, 
		            PTR_wall_timer_val *wall_end, 
		            double *wall_secs,
		            int *PTR_wall_stat ));

extern void PTR_Get_usr_interval __FUNC_PROTO__(
			  (PTR_usr_timer_val *usr_start, 
		           PTR_usr_timer_val *usr_end, 
		           double *usr_secs,
		           int *PTR_usr_stat ));

extern void PTR_Get_sys_interval __FUNC_PROTO__(
			  (PTR_sys_timer_val *sys_start, 
		           PTR_sys_timer_val *sys_end, 
		           double *sys_secs,
		           int *PTR_sys_stat ));

#ifdef __cplusplus	/* C++ */
}
#endif			/* C++ */

#endif 			/* _PTOOLS_PTR_IBM_H_ */
