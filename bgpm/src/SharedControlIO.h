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

#ifdef SHARED_CONTROL_STRUCT_NAME
#undef SHARED_CONTROL_STRUCT_NAME
#undef SHARED_CONTROL_CTR_RANGE
#endif
#define SHARED_CONTROL_STRUCT_NAME SharedControlIO
#define SHARED_CONTROL_CTR_RANGE   UPC_IO_Counter_Range

#ifndef _SHARED_CONTROL_IO_H_  // Prevent multiple inclusion
#define _SHARED_CONTROL_IO_H_

#include "cnk/include/Config.h"
#include "spi/include/upci/upci.h"


namespace bgpm {


#include "SharedControl.h"

void SHARED_CONTROL_STRUCT_NAME::ResetThreshold(unsigned ctr) {
    assert(ctr < SHARED_CONTROL_CTR_RANGE);
    upc_c->iosram_data[ctr] = thresholds[ctr];
}


}
#endif

#ifdef _SHARED_CONTROL_C_
#include "globals.h"
#include "SharedControl.h"

uint64_t bgpm::SHARED_CONTROL_STRUCT_NAME::ResetAndGetIntMask(uint64_t ioIntStatus) {
    uint64_t retMask = 0;

    // build mask with 16 bits for each of the 16 counter groups set in ioIntStatus
    ioIntStatus <<= 64 - UPC_C_DCR__IO_CCG_INT_EN__IE_width;  // left adjust.
    uint64_t mask = 0;
    while (ioIntStatus) {
        unsigned bit = upc_cntlz64(ioIntStatus);
        mask |= 0xFFFF000000000000ULL >> (bit*UPC_C_DCR__IO_CCG_INT_EN__IE_width);
        ioIntStatus &= ~MASK64_bit(bit);
    }
    // mask to leave only those counters which have overflow active
    mask &= ovfMask;

    // Check supported counters for overflow
    while (mask) {
        unsigned maskBit = upc_cntlz64(mask);
        uint64_t count = upc_c->iosram_data[maskBit];
        if (count < BGPM_THRESHOLD_MIN) {
            retMask |= MASK64_bit(maskBit);
            AccumThreshold(maskBit, count);
            ResetThreshold(maskBit);
        }
        mask &= ~MASK64_bit(maskBit);
    }
    return retMask;
}


// Call only within LeaderLatch context (by leading thread)
void bgpm::SHARED_CONTROL_STRUCT_NAME::EnqueIfActiveOvf(uint64_t ioIntStatus) {
    if (ioIntStatus) {
        upc_c->io_ccg_int_status_w1c = ioIntStatus;  // reset interrupt status
        uint64_t ctrStatus = ResetAndGetIntMask(ioIntStatus);
        ovfQue.Enque(ctrStatus);
    }
}

#endif
