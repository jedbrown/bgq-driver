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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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
#ifndef _L1P_FLUSH_H_ // Prevent multiple inclusion.
#define _L1P_FLUSH_H_

#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

#include <sys/types.h>

/*!
 * \brief Cause the L1P to flush all pending load and store requests to the
 *        switch.
 *
 * The cheapest way to accomplish this function is to issue a store via a
 * TLB entry that has the U1 bit set.  Our primary MMIO mapping has the
 * required property.  We just need an MMIO location for which stores have
 * no undesirable side effects.
 */

/*
 * This implementation uses a location in the MU.  Stores to this location
 * have no functional side effects.  Heavy use of this interface can
 * possibly affect MU performance however, so the implementation may change
 * to another location at some point.
 */
#include <hwi/include/bqc/MU_Macros.h>
__INLINE__ void L1P_FlushRequests()
{
    volatile uint64_t *dummy_mu_reg =
	(uint64_t *) ((BGQ_MU_STATUS_CONTROL_REGS_START_OFFSET(0,0) -
		       PHYMAP_PRIVILEGEDOFFSET) + 0x030);
    *dummy_mu_reg = 0;
}

__END_DECLS

#endif // Add nothing below this line.
