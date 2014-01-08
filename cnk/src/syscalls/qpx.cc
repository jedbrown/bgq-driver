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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

// Includes
#include "Kernel.h" 

uint64_t sc_GETAXUCR0(SYSCALL_FCN_ARGS)
{
    AppState_t *app = GetMyAppState();
    uint64_t mask;

    if (app->Active_Processes <= 16)
    {
	// This process owns the whole core.  Allow access to stochastic
	// rounding mode and to all thread signalling control bits.
	mask = AXUCR0_LFSR_RESET | AXUCR0_SR_ENABLE | AXUCR0_ENAB_IND_ALL(-1);
    }
    else
    {
	// This process does not own the whole core.  Restrict access to
	// just this thread's signalling control bits.
	mask = AXUCR0_ENAB_IND(ProcessorThreadID(), -1);
    }

    return CNK_RC_SPI(mfspr(SPRN_AXUCR0) & mask);
}

uint64_t sc_SETAXUCR0(SYSCALL_FCN_ARGS)
{
    uint64_t value  = (uint64_t) r3;
    AppState_t *app = GetMyAppState();
    uint64_t mask;

    if (app->Active_Processes <= 16)
    {
	// This process owns the whole core.  Allow changes to stochastic
	// rounding mode and to all thread signalling control bits.
	mask = AXUCR0_LFSR_RESET | AXUCR0_SR_ENABLE | AXUCR0_ENAB_IND_ALL(-1);
    }
    else
    {
	// This process does not own the whole core.  Restrict changes to
	// just this thread's signalling control bits.
	mask = AXUCR0_ENAB_IND(ProcessorThreadID(), -1);
    }

    if ((value & ~mask) != 0)
    {
	// Trying to set disallowed bits.
	return CNK_RC_SPI(EINVAL);
    }

    mtspr(SPRN_AXUCR0, (mfspr(SPRN_AXUCR0) & ~mask) | value);

    return CNK_RC_SPI(0);
}
