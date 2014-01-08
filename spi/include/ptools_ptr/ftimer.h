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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

************************************************************************
* This file contains type definitions, manifest constants, and	       *
* external subroutine statements for the PTR library.  Other           *
* platform-dependent information is also provided. 		       *
************************************************************************

************************************************************************
* Note to Working Group: This is a draft template for "ftimer.h". It   *
* is only meant to initiate discussion and is not the final version.   *
************************************************************************
********************
* Type definitions *
********************
C  NOTE TO WORKING GROUP: The data type(s) for timer values
C  (e.g., DOUBLE PRECISION) will have to be provided as documentation. 
C  For platforms that provide data on number of rollovers we will have 
C  to declare a type that can store both the quantities, rollover count 
C  and timer value (e.g., REAL*16).  The subroutines will have to store 
C  and extract the values appropriately. (See the argument descriptions 
C  for the subroutines below)
*======================================================================*
************************
* Manifest Constants:  *
************************
C   *------------------------------------------------------------------*
C   * Nominal tick period  (in secs): values supplied by the Vendor    *
C   *------------------------------------------------------------------*
      PARAMETER( PTR_WALL_NOM_TICK	= 1.0D-09)
      PARAMETER( PTR_USR_NOM_TICK	= 1.0D-02)
      PARAMETER( PTR_SYS_NOM_TICK	= 1.0D-02)
C   *------------------------------------------------------------------*
C   * Calibrated tick period (in secs): calibrated values              *
C   *------------------------------------------------------------------*
      PARAMETER( PTR_WALL_CAL_TICK	= 1.0D-09)
      PARAMETER( PTR_USR_CAL_TICK	= 1.0D-02)
      PARAMETER( PTR_SYS_CAL_TICK	= 1.0D-02)
C   *------------------------------------------------------------------*
C   * Resolution (in secs): values supplied by the vendor              *
C   *------------------------------------------------------------------*
      PARAMETER( PTR_WALL_RESOLUTION	= 1.0D-09)
      PARAMETER( PTR_USR_RESOLUTION	= 1.0D-02)
      PARAMETER( PTR_SYS_RESOLUTION	= 1.0D-02)
C   *------------------------------------------------------------------*
C   * Rollover period (in secs): [do not rollover]	               * 
C   *------------------------------------------------------------------*
      PARAMETER( PTR_WALL_ROLLOVER	= .9999999D+09)
      PARAMETER( PTR_USR_ROLLOVER	= .9999999D+09)
      PARAMETER( PTR_SYS_ROLLOVER	= .9999999D+09)
C   *------------------------------------------------------------------*
C   * Overhead (in secs): (approximate values - vary with system load) *
C   *------------------------------------------------------------------*
      PARAMETER( PTR_WALL_OVERHEAD	= 0.25D-06)
      PARAMETER( PTR_USR_OVERHEAD	= 1.5D-06)
      PARAMETER( PTR_SYS_OVERHEAD	= 1.5D-06)
C   *------------------------------------------------------------------*
C   * Status codes returned by the subroutines                         *
C   *------------------------------------------------------------------*
      PARAMETER( PTR_NOT_AVAILABLE	= -1)
      PARAMETER( PTR_ROLLOVER_DETECTED	= -2)
      PARAMETER( PTR_CANNOT_CALIBRATE	= -3)
      PARAMETER( PTR_OTHER_ERROR	= -4)
*======================================================================*
************************
* External Subroutines * 
************************
C  Note: ??? will be replaced by WALL, USR or SYS.
C
C   Subroutines for runtime calibration for the particular platform
C   Calling sequence
C    CALL PTR_INIT_???_TIMER(opt_???_ticksize, init_level, ???_stat)
C     Argument#1 -> PTR_???_NOM_TICK , or 0. 	      [DOUBLE PRECISION]
C     		    If 0 calibrate and return value. 		      
C     Argument#2 -> Time to spend for calibration     [DOUBLE PRECISION]
C			(in secs)    
C     Argument#3 -> Status code; returned by subroutine.       [INTEGER]

      EXTERNAL PTR_INIT_WALL_TIMER
      EXTERNAL PTR_INIT_USR_TIMER
      EXTERNAL PTR_INIT_SYS_TIMER

C   Subroutines to get the opaque timer values 
C   Calling sequence
C    CALL PTR_GET_???_TIME (??_var)
C     Argument#1 -> Variable to hold timer value      [DOUBLE PRECISION?]

      EXTERNAL PTR_GET_WALL_TIME
      EXTERNAL PTR_GET_USR_TIME
      EXTERNAL PTR_GET_SYS_TIME

C   Subroutines for calculating intervals between opaque values  
C   Calling sequence
C    CALL PTR_GET_???_INTERVAL (???_var1, ???_var2, ???_secs, ???_stat)
C     Argument#1 -> First timer value 		      [DOUBLE PRECISION?]
C     Argument#2 -> Second timer value 		      [DOUBLE PRECISION?]
C     Argument#3 -> Interval value returned (in secs) [DOUBLE PRECISION]
C     Argument#4 -> Status code; returned by subroutine	       [INTEGER]

      EXTERNAL PTR_GET_WALL_INTERVAL
      EXTERNAL PTR_GET_USR_INTERVAL
      EXTERNAL PTR_GET_SYS_INTERVAL
