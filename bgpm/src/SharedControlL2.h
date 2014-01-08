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
#define SHARED_CONTROL_STRUCT_NAME SharedControlL2
#define SHARED_CONTROL_CTR_RANGE   UPC_L2_NUM_COUNTERS



#ifndef _SHARED_CONTROL_L2_H_  // Prevent multiple inclusion
#define _SHARED_CONTROL_L2_H_

#include "cnk/include/Config.h"
#include "spi/include/upci/upci.h"


namespace bgpm {


#include "SharedControl.h"

void SHARED_CONTROL_STRUCT_NAME::ResetThreshold(unsigned ctr) {
    assert(ctr < SHARED_CONTROL_CTR_RANGE);
    upc_c->data16.grp[UPC_C_SRAM_BASE_L2_GROUP].counter[ctr] = thresholds[ctr];
}


}
#endif

#ifdef _SHARED_CONTROL_C_
#include "globals.h"
#include "SharedControl.h"

uint64_t bgpm::SHARED_CONTROL_STRUCT_NAME::ResetAndGetIntMask(uint64_t upccIntStatus) {
    uint64_t retMask = 0;

    if (upccIntStatus & MASK64_bit(UPC_C_SRAM_BASE_L2_GROUP)) {
        uint64_t mask = ovfMask;
        while (mask) {
            unsigned maskBit = upc_cntlz64(mask);
            uint64_t count = upc_c->data16.grp[UPC_C_SRAM_BASE_L2_GROUP].counter[maskBit];
            if (count < BGPM_THRESHOLD_MIN) {
                retMask |= MASK64_bit(maskBit);
                AccumThreshold(maskBit, count);
                ResetThreshold(maskBit);
            }
            mask &= ~MASK64_bit(maskBit);
        }
    }
    return retMask;
}



// Call only within LeaderLatch context (by leading thread)
void bgpm::SHARED_CONTROL_STRUCT_NAME::EnqueIfActiveOvf(uint64_t upccIntStatus) {
    if (upccIntStatus) {
        upc_c->ccg_int_status_w1c = upccIntStatus;  // reset interrupt status
        uint64_t ctrStatus = ResetAndGetIntMask(upccIntStatus);
        ovfQue.Enque(ctrStatus);
    }
}

#endif
