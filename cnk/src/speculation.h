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

int  Speculation_Init();
int  Speculation_CleanupJob();
int  Speculation_AllocateDomain(unsigned int* domain);
int  Speculation_GetAllocatedDomainCount(uint32_t* domaincount);
int  Speculation_SetAllocatedDomainCount(uint32_t  domaincount);
int  Speculation_CheckJailViolation(Regs_t* context);
void Speculation_CheckJailViolationDefer(Regs_t* context);
int  Speculation_EnterJailMode(bool longRunningSpec);
int  Speculation_ExitJailMode();
void IntHandler_L2Central(int intrp_sum_bitnum);
int  Speculation_EnableFastSpeculationPaths();
int  Speculation_CheckPendingRestart();
int  l1p_init();
