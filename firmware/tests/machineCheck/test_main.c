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
#include <hwi/include/bqc/BIC_inlines.h>
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>

extern int _fwext_Vec_MCHK_trampoline ; 
volatile void* vectorAddress;

int test_main( void ) {

  // ----------------------------------------------------------
  // Install the trampoline vector on the primordial thread:
  // ----------------------------------------------------------

  int rc = 0;

  if ( ProcessorThreadID() == 0 ) {
    if ( ( rc = fwext_getFwInterface()->installInterruptVector( &_fwext_Vec_MCHK_trampoline, 0x400 ) ) != 0 ) {
      printf("(E) bad return code (%d) from installInterruptVector service\n", rc);
      Terminate(__LINE__);
    }
    fwext_mapPUEAInterruptLane( BIC_MAP_GEA_LANE(0), 0x1, BIC_MACHINE_CHECK ); // Enable machine checks on all threads
    fwext_mapPUEAInterruptLane( BIC_MAP_L1P_LANE(3), 0x1, BIC_MACHINE_CHECK ); // Enable machine checks on all threads
    //fwext_mapPUEAInterruptLane( BIC_MAP_GEA_LANE(7), 0x4, BIC_MACHINE_CHECK ); // Enable machine checks on thread 2
  }


  // ----------------------------------------------------------    
  // Wait for everyone to arrive here:
  // ----------------------------------------------------------

  BgBarrier( 0, BgGetNumThreads() );

  
  // ---------------------------------------------------------
  //  Force a machine check by scribbling in the GEA debug
  //  register.  If all works properly, this will trigger
  //  a machine check on thread 2.
  // ---------------------------------------------------------

  if (ProcessorID() == 0) {

    DCRWritePriv(
		 GEA_DCR(GEA_DEBUG_CONTROL),
		 GEA_DCR__GEA_DEBUG_CONTROL__DEBUG_EN_set(1) |
		 GEA_DCR__GEA_DEBUG_CONTROL__DEBUG_DATA_set( 0x8000 >> 0 /*lane*/ )
		 );
  }

  /*
   * This really doesnt work on mambo .... so for now we simply allow the exit to happen
   */

  Personality_t personality;


  fwext_getFwInterface()->getPersonality( &personality, sizeof(personality) );

  if ( ( personality.Kernel_Config.NodeConfig & PERS_ENABLE_Mambo ) == 0 ) {

    while (1) 
      /* spin */
      ;
  }

  exit(0);
}



