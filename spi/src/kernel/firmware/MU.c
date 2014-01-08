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

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <hwi/include/common/compiler_support.h>
#include <firmware/include/personality.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <spi/include/kernel/debug.h>
#include <spi/include/kernel/process.h>


#define DEBUG(s) printf s
//#define DEBUG(s)



typedef struct _KERNEL_MU_TermCheck_t {
    uint32_t base;        // Unit's DCR base address
    uint32_t offset;      // DCR offset
    uint32_t stride;      // Stride length (for units with multiple instances)
    uint32_t range;       // The number of unit instances
    uint32_t count;       // The number of DCRs in the array
    uint32_t rasEvent;    // The RAS event id associated with the termination check
    uint64_t mask;        // 
    int (*selector)( Personality_t*, uint32_t instance );
} Kernel_MU_TermCheck_t;


#define ND_500_BASE       ND_500_DCR_base
#define ND_500_OFFSET(d)  ND_500_DCR__##d##_offset
#define ND_500_STRIDE     0
#define ND_500_RANGE      1

#define ND_RESE_BASE      ND_RESE_DCR_base
#define ND_RESE_OFFSET(d) ND_RESE_DCR__##d##_offset
#define ND_RESE_STRIDE    ND_RESE_DCR_stride
#define ND_RESE_RANGE     ND_RESE_DCR_num

#define ND_X2_BASE      ND_X2_DCR_base
#define ND_X2_OFFSET(d) ND_X2_DCR__##d##_offset


/**
 * @brief Performs the check on a single data point.  Any violations
 *        are reported as a RAS event.
 */

int _KERNEL_PerformTermCheck( Kernel_MU_TermCheck_t* tc, Personality_t* p ) {

    DEBUG(("(>) %s\n", __func__));

    unsigned i, j, n = 0, rc = 0;
    uint64_t details[128];

    for ( i = 0; i < tc->range; i++ ) {

	// Skip the instance if it's selector inhibits the check:
	if ( ( tc->selector != 0 ) && (tc->selector(p,i) == 0) )
	    continue;

	for ( j = 0; j < tc->count; j++ ) {

	    // Read the DCR and report a violation if any bits are on (modulo the
	    // mask) are on

	    uint32_t dcrAddress = tc->base + ( i * tc->stride ) + tc->offset + j;
	    uint64_t value =  DCRReadPriv( dcrAddress );

	    DEBUG(( "(*) DCR %08X : %016lX : %016lX : %s\n", dcrAddress, value, tc->mask, (value & ~tc->mask) ? "DIRTY" : "CLEAN" ));


	    if ( ( value & ~tc->mask ) != 0 ) {
		details[n++] = dcrAddress;
		details[n++] = value;
	    }

	    // Flush the RAS buffer if it is full:
	    
	    if ( n >= sizeof(details)/sizeof(details[0]) ) {
		Kernel_InjectRAWRAS( tc->rasEvent, n, details );
		rc = -1;
		n = 0;
	    }
	}
    }

    if ( n > 0 ) {
	Kernel_InjectRAWRAS( tc->rasEvent, n, details );
	rc = -1;
    }
  
    DEBUG(("(<) %s rc=%d\n", __func__, rc));
    return rc;
}


int _KERNEL_SenderIsEnabled( Personality_t* p, unsigned i ) {
    return ND_GET_LINK_ENABLE( (i>>1), (i&1), p->Network_Config.NetFlags2 ) ? 1 : 0;
}

 /* 
   <rasevent 
     id="000D0001"
     category="Message_Unit"
     component="SPI"
     severity="WARN"
     message="MU non-fatal error has been detected in the Network Device Hardware: $(DETAILS)"
     description="This error indicates that a fatal error exists in the network hardware."
     service_action="$(Diagnostics)"
     relevant_diags="processor,torus"
     decoder="fw_ND_machineCheckDecoder"
     />

   <rasevent 
     id="000D0002"
     category="Message_Unit"
     component="SPI"
     severity="FATAL"
     message="MU fatal error has been detected in the Network Device Hardware: $(DETAILS)"
     description="This error indicates that a fatal error exists in the network hardware. "
     service_action="$(Diagnostics)"
     relevant_diags="processor,torus"
     decoder="fw_ND_machineCheckDecoder"
     />
 */


Kernel_MU_TermCheck_t TERM_CHECKS[] = {
    //  BASE               OFFSET
    { ND_500_BASE,   ND_500_OFFSET(CE_CNT),            0,                1,              ND_500_DCR__CE_CNT_range,    0x000D0001,  0, 0 },
    { ND_RESE_BASE,  ND_RESE_OFFSET(RE_LINK_ERR_CNT),  ND_RESE_STRIDE,   ND_RESE_RANGE,  1,                           0x000D0001,  0, _KERNEL_SenderIsEnabled },
    { ND_RESE_BASE,  ND_RESE_OFFSET(SE_RETRANS_CNT),   ND_RESE_STRIDE,   ND_RESE_RANGE,  1,                           0x000D0001,  0, _KERNEL_SenderIsEnabled },
    { ND_RESE_BASE,  ND_RESE_OFFSET(CE_COUNT),         ND_RESE_STRIDE,   ND_RESE_RANGE,  ND_RESE_DCR__CE_COUNT_range, 0x000D0001,  0, _KERNEL_SenderIsEnabled },
    { ND_500_BASE,   ND_500_OFFSET(FATAL_ERR0),        0,                1,              6,                           0x000D0002,  0, 0 },
    { ND_RESE_BASE,  ND_RESE_OFFSET(FATAL_ERR),        ND_RESE_STRIDE,   ND_RESE_RANGE,  1,                           0x000D0002,  ND_RESE_DCR__FATAL_ERR__SE_DROPPED_PKT_set(1), _KERNEL_SenderIsEnabled },
    { ND_X2_BASE,    ND_X2_OFFSET(FATAL_ERR0),         0,                1,              1,                           0x000D0002,  0, 0 },
    { ND_X2_BASE,    ND_X2_OFFSET(FATAL_ERR1),         0,                1,              1,                           0x000D0002,  0, 0 },
};

int32_t Kernel_MU_TermCheck( void ) {

    int32_t rc = 0;
    Personality_t pers;
    unsigned i;

    rc |= Kernel_GetPersonality( &pers, sizeof(pers) );


    if ( rc == 0 ) {
	for ( i = 0; i < sizeof(TERM_CHECKS)/sizeof(TERM_CHECKS[0]); i++ ) {
	    rc |= _KERNEL_PerformTermCheck( TERM_CHECKS+i, &pers );
	}
    }

    return rc;
}
