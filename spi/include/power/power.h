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
#ifndef _POWER_POWER_H_ // Prevent multiple inclusion.
#define _POWER_POWER_H_

extern void HPM_EMON_Init(void);
extern void HPM_Init(void);
extern void EMON_Init(void);
extern void HPM_Start(char *);
extern void HPM_Stop(char *);
extern void HPM_EMON_Print(void);
extern void HPM_Print(void);
extern void EMON_Print(void);
extern int  EMON_rank_on_card(void );


extern void HPM_EMON_Init_t(int num_threads);
extern void HPM_Init_t(int num_threads);
extern void EMON_Init_t(int num_threads);
extern void HPM_EMON_Print_t(int num_threads);
extern void HPM_Print_t(int num_threads);
extern void EMON_Print_t(int num_threads);


#endif // _POWER_POWER_H_
