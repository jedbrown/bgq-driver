/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/testint_inlines.h>
#include "Firmware_internals.h"

int fw_fpu_init( void ) {

  unsigned long zeroes[4] = { 0, 0, 0, 0 };

  mtmsr( mfmsr() | MSR_FP );
  isync();

#if 0
  // Per Krishnan, this workaround is no longer necessary (it was a "DD2"
  // workaround that has been fixed in DD2)

  if(!FW_DD1_WORKAROUNDS_ENABLED()) {  
      mtspr(SPRN_IUCR2, _BN(37));      /* Issue BQCBUGS issue 1448: Disable Clock gating in axu issue */
      isync();
  }
#endif
  
  asm volatile ( "qvlfdx 0,0,%0;"
		 "isync;" /* required to work around bugspray issue */
		 "sync;" /* required to work around bugspray issue */
		 : : "r" (zeroes) : "memory" );
  asm volatile ( "qvfmr  1,0;" 
		 "qvfmr  2,0;"
		 "qvfmr  3,0;"
		 "qvfmr  4,0;"
		 "qvfmr  5,0;"
		 "qvfmr  6,0;"
		 "qvfmr  7,0;"
		 "qvfmr  8,0;"
		 "qvfmr  9,0;"
		 "qvfmr 10,0;"
		 "qvfmr 11,0;"
		 "qvfmr 12,0;"
		 "qvfmr 13,0;"
		 "qvfmr 14,0;"
		 "qvfmr 15,0;"
		 "qvfmr 16,0;"
		 "qvfmr 17,0;"
		 "qvfmr 18,0;"
		 "qvfmr 19,0;"
		 "qvfmr 20,0;"
		 "qvfmr 21,0;"
		 "qvfmr 22,0;"
		 "qvfmr 23,0;"
		 "qvfmr 24,0;"
		 "qvfmr 25,0;"
		 "qvfmr 26,0;"
		 "qvfmr 27,0;"
		 "qvfmr 28,0;"
		 "qvfmr 29,0;"
		 "qvfmr 30,0;"
		 "qvfmr 31,0;"
		 );
  return 0;
}
