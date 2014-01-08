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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#ifndef _CNK_TIMER_H_ // Prevent multiple inclusion
#define _CNK_TIMER_H_

uint64_t Timer_GetTimer(int sig, uint64_t *remainder, uint64_t *old_interval);
uint64_t Timer_SetTimer(int sig, uint64_t value, uint64_t interval,
			uint64_t *remainder, uint64_t *old_interval);
uint64_t Timer_NanoSleep(struct timespec *req, struct timespec *rem);
void Timer_Interrupt(KThread_t *kthr);
void Timer_Awaken(KThread_t *kthr);
void Timer_disableUDECRwakeup();
void Timer_enableUDECRwakeup(uint64_t current_timebase, uint64_t expiration);
void Timer_enableFutexTimeout(uint64_t current_timebase, uint64_t expiration);

#endif // Add nothing below this line
