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

#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/BIC.h>
#include <hwi/include/bqc/BIC_inlines.h>

#include <hwi/include/bqc/bedram_dcr.h>
#include <hwi/include/bqc/cs_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>
#include <hwi/include/bqc/devbus_dcr.h>
#include <hwi/include/bqc/en_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/l1p_dcr.h>
#include <hwi/include/bqc/l1p_mmio.h>
#include <hwi/include/bqc/l2_dcr.h>
#include <hwi/include/bqc/l2_central_dcr.h>
#include <hwi/include/bqc/l2_counter_dcr.h>
#include <hwi/include/bqc/ms_genct_dcr.h>
#include <hwi/include/bqc/mu_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>
#include <hwi/include/bqc/pe_dcr.h>
#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/upc_c_dcr.h>

FwExt_Barrier_t barrier;

int test_main( void ) {

  int N = BgGetNumThreads();
  int irritatorThread = 1;
  int iterations = 1;

  char* irritator  = fwext_getenv("IRRITATOR");
  char* testId     = fwext_getenv("TEST");
  char* itersStr   = fwext_getenv("ITERATIONS");

  if ( irritator != 0 ) {
      irritatorThread = fwext_strtoul( irritator, 0, 0 );
  }

  if ( itersStr != 0 ) {
      iterations = fwext_strtoul( itersStr, 0, 0 );
  }

  fwext_barrier( &barrier, N );
  
  if ( ProcessorID() == irritatorThread ) {

      char* l2Slice = fwext_getenv("L2SLICE");
      char* mc      = fwext_getenv("MC");
      int   mci = 0;

      if ( mc != 0 ) {
	  mci = fwext_strtoul( mc, 0, 0 );
      }
   
      int i;

      for ( i = 0; i < iterations; i++ ) {

	  if ( i > 0 ) {
	      fwext_udelay( 250 * 1000 );
	  }

	  printf("Test: %s L2Slice:%s MC:%s NumThreads:%d Iter:%d\n", testId, l2Slice, mc ? mc : "?", N, i );
      
	  if ( fwext_strcmp("BeDRAM",testId) == 0 )  {
	      printf("Injecting ...\n");
	      uint64_t inject = BEDRAM_DCR__BEDRAM_INTERRUPT_STATUS__BEDRAM_EDRAM_ECC_set(1);
	      DCRWritePriv( BEDRAM_DCR( BEDRAM_INTERRUPT_STATUS__FORCE ), inject );
	      ppc_msync();
	  }


	  if ( fwext_strcmp("ClkStopUnit",testId) == 0 )  {
	      uint64_t inject = CS_DCR__CLOCKSTOP_INTERRUPT_STATE__STOPPED_set(1);
	      DCRWritePriv( CS_DCR( CLOCKSTOP_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("DrArbiter",testId) == 0 )  {
	      uint64_t inject = DC_ARBITER_DCR__INT_REG__RING_NOT_CLEAN_set(1);
	      DCRWritePriv( DC_ARBITER_DCR( INT_REG__FORCE ), inject );
	      ppc_msync();
	  }


	  if ( fwext_strcmp("DDR",testId) == 0 )  {

	      DCRWritePriv( _DDR_MC_MCFIRS(mci), MCFIR_POWERBUS_READ_BUFFER_SUE );
	      uint64_t inject = DR_ARB_DCR__L2_INTERRUPT_STATE__XSTOP_set(1);
	      DCRWritePriv( DR_ARB_DCR(mci, L2_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("DDR_MARKING_STORE",testId) == 0 )  {
	      DCRWritePriv( _DDR_MC_MCFIRS(mci), MCFIR_MEMORY_ECC_MARKING_STORE_UPDATED );
	      uint64_t inject = DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_set(1);
	      DCRWritePriv( DR_ARB_DCR(mci, L2_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("DDR_Correctable",testId) == 0 )  {
	      //DCRWritePriv( _DDR_MC_MCFIRS(mci), MCFIR_POWERBUS_WRITE_BUFFER_CE);
	      //DCRWritePriv( _DDR_MC_MCFIRS(mci), MCFIR_MEMORY_CE );
	      DCRWritePriv( _DDR_MC_MCFIRS(mci), MCFIR_POWERBUS_READ_BUFFER_SUE );
	      //uint64_t inject = DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_set(1);
	      //DCRWritePriv( DR_ARB_DCR(mci, L2_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("DDR_Threshold",testId) == 0 )  {
	      DCRWritePriv( _DDR_MC_MCFIRS(mci), MCFIR_ECC_ERROR_COUNTER_THRESHOLD_REACHED );
	      uint64_t inject = DR_ARB_DCR__L2_INTERRUPT_STATE__RECOV_ERR_set(1);
	      DCRWritePriv( DR_ARB_DCR(mci, L2_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("DevBus",testId) == 0 )  {
	      uint64_t inject = DEVBUS_DCR__DB_INTERRUPT_STATE__SLAVE_FIFO_PARITY_set(1);
	      DCRWritePriv( DEVBUS_DCR( DB_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("EnvMon",testId) == 0 )  {
	      uint64_t inject = EN_DCR__ENVMON_INTERRUPT_STATE__FSM_CHECKSUM_FAIL_set(1);
	      DCRWritePriv( EN_DCR( ENVMON_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("GEA",testId) == 0 )  {
	      uint64_t inject = GEA_DCR__GEA_INTERRUPT_STATE__DEVBUS_CTL_PERR_set(1);
	      DCRWritePriv( GEA_DCR( GEA_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }


#if 0
	  {
	      uint64_t inject = L1P_DCR__INTERRUPT_INTERNAL_ERROR__BAD_DATA_PARITY_set(1);
	      DCRWritePriv( L1P_DCR(0, INTERRUPT_INTERNAL_ERROR__FORCE ), inject );
	      ppc_msync();
	  }
#endif

	  if ( fwext_strcmp("L1P",testId) == 0 )  {
	  
	      //printf("Irritating L1 on core %d ...\n", irritatorThread);
#if 0
	      BIC_InsertInterruptMap( 0, BIC_MAP_L1P_LANE(0), BIC_MACHINE_CHECK );
	      BIC_InsertInterruptMap( 0, BIC_MAP_L1P_LANE(1), BIC_MACHINE_CHECK );
	      BIC_InsertInterruptMap( 0, BIC_MAP_L1P_LANE(2), BIC_MACHINE_CHECK );
	      BIC_InsertInterruptMap( 0, BIC_MAP_L1P_LANE(3), BIC_MACHINE_CHECK );
#endif

	      int64_t NN ;

	      printf("INJECTING!\n");
	      for ( NN = 0; NN < 10000000ull; NN++ ) {
	      uint64_t inject = 
		  L1P_ESR_err_reload_ecc_x2 |
		  //L1P_ESR_err_si_ecc |
		  //L1P_ESR_err_reload_ecc_ue_x2 |
		  0  ;
	
	      out64_sync((void *)L1P_ESR_INJ_DCR(ProcessorCoreID()), inject );
	      ppc_msync();
	      out64_sync((void *)L1P_ESR_INJ_DCR(ProcessorCoreID()), 0 );
	      ppc_msync();
	      }

	      printf( "*********************************** Injected %ld CEs into L1P ***********************************************\n", NN);

	      //printf("L1P_ESR --> %lx\n", in64( (uint64_t*)L1P_ESR));
	  }

	  if ( fwext_strcmp("L1PBug",testId) == 0 )  {
	  

	      printf("Waiting for 2 seconds ...\n");

	      uint64_t end = GetTimeBase() + 1600ull * 1000ull * 1000ull * 2;

	      while ( GetTimeBase() < end );

	      printf("Irritating L1 on core %d ...\n", irritatorThread);

	      uint64_t inject = 
		  L1P_ESR_err_reload_ecc_x2 |
		  //L1P_ESR_err_si_ecc |
		  //L1P_ESR_err_reload_ecc_ue_x2 |
		  0  ;
	
	      printf("injecting %lx\n", inject);
	      out64_sync((void *)L1P_ESR_INJ_DCR(ProcessorCoreID()), inject );
	      ppc_msync();
	      out64_sync((void *)L1P_ESR_INJ_DCR(ProcessorCoreID()), 0 );
	      ppc_msync();
	  }


	  if ( fwext_strcmp("L2",testId) == 0 )  {
	      uint64_t inject = L2_DCR__L2_INTERRUPT_STATE__DIRB_UE_set(1);
	      unsigned slice = fwext_strtoul( l2Slice, 0, 0 );
	      DCRWritePriv( L2_DCR( slice, L2_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("L2_Correctable",testId) == 0 )  {

	      int II;

	      for ( II=0; II < 111101; II++ ) {
		  //uint64_t inject = L2_DCR__L2_INTERRUPT_STATE__DIRB_CE_set(1);
		  uint64_t inject = L2_DCR__L2_INTERRUPT_STATE__EDR_CE_set(1);
		  unsigned slice = fwext_strtoul( l2Slice, 0, 0 );
		  DCRWritePriv( L2_DCR( slice, L2_INTERRUPT_STATE__FORCE ), inject );
		  ppc_msync();
		  DCRWritePriv( L2_DCR( slice, L2_INTERRUPT_STATE__FORCE ), 0 );
		  ppc_msync();
	      }

	      printf("Issued %d L2 CEs!\n", II);
	  }


	  if ( fwext_strcmp("L2CTR",testId) == 0 )  {
	      uint64_t inject = L2_COUNTER_DCR__L2_INTERRUPT_STATE__BDRY_PAR_ERR_set(1);
	      unsigned counter = fwext_strtoul( l2Slice, 0, 0 );
	      DCRWritePriv( L2_COUNTER_DCR( counter, L2_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }


	  if ( fwext_strcmp("L2Central",testId) == 0 )  {
	      uint64_t inject = L2_CENTRAL_DCR__L2_INTERRUPT_STATE__ECC_UE_set(1);
	      DCRWritePriv( L2_CENTRAL_DCR( L2_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }


	  if ( fwext_strcmp("Msgc",testId) == 0 )     {
	      uint64_t inject = MS_GENCT_DCR__L2_INTERRUPT_STATE__TIMEOUT_E_set(1);
	      DCRWritePriv( MS_GENCT_DCR( L2_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("MU",testId) == 0 )   {
	      // A fatal with simple intinfo:
	      uint64_t inject = MU_DCR__RME_INTERRUPTS0__RME_ERR7_set(1);
	      DCRWritePriv( MU_DCR( RME_INTERRUPTS0__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("MU_Correctable",testId) == 0 ) {
	      // Force a correctable error
	      uint64_t inject = MU_DCR__IMU_ECC_INTERRUPTS__IMU_ECC_CE1_set(1);
	      DCRWritePriv( MU_DCR( IMU_ECC_INTERRUPTS__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("ND",testId) == 0 ) {
	      // A fatal with simple intinfo:
	      uint64_t inject = ND_RESE_DCR__RESE_INTERRUPTS__LOCAL_RING_set(1);
	      DCRWritePriv( ND_RESE_DCR( 7, RESE_INTERRUPTS__FORCE ), inject );
	      ppc_msync();

	      //fwext_getFwInterface()->deprecated.backdoorTest(0);
	  }

	  if ( fwext_strcmp("PCIe",testId) == 0 ) {
	      ppc_msync();
	      uint64_t inject = PE_DCR__PCIE_INTERRUPT_STATE__CFG_PERR_set(1);
	      DCRWritePriv( PE_DCR( PCIE_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("SerDes0",testId) == 0 ) {
	      uint64_t inject = SERDES_LEFT_DCR__SERDES_INTERRUPT_STATE__A_PLLA_LOCK_LOST_set(1);
	      DCRWritePriv( SERDES_LEFT_DCR( SERDES_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }


	  if ( fwext_strcmp("SerDes1",testId) == 0 ) {


#if 0
	      // simulation of issue 1811
	      DCRWritePriv( SERDES_RIGHT_DCR(SERDES_INTERRUPT_STATE_CONTROL_HIGH), SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE_CONTROL_HIGH__D_PLLA_LOCK_LOST_set(2) );
	      ppc_msync();
#endif

	      uint64_t inject = SERDES_RIGHT_DCR__SERDES_INTERRUPT_STATE__D_PLLA_LOCK_LOST_set(1);
	      printf("inject->%016lX\n", inject);
	      DCRWritePriv( SERDES_RIGHT_DCR( SERDES_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

	  if ( fwext_strcmp("TestInt",testId) == 0 ) {
	      uint64_t inject = TESTINT_DCR__TI_INTERRUPT_STATE__INT_PARITY_ERROR_set(1);
	      DCRWritePriv( TESTINT_DCR( TI_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

#ifdef TESTUPC 

#define UPC_C_MMIO ((upc_c_dcr_t*)(PHYMAP_MINADDR_UPC | PHYMAP_PRIVILEGEDOFFSET))
	  {
	      UPC_C_MMIO->upc_c_interrupt_state__force =     
		  UPC_C_DCR__UPC_C_INTERRUPT_STATE__PRING_ERROR_set(1);           // An error was detected on the Processor/L2 UPC Daisy Chain 
	      ppc_msync();
	  }

#endif

#ifdef TESTWakeup
	  {
	      uint64_t inject = _DCR__CLOCKSTOP_INTERRUPT_STATE__STOPPED_set(1);
	      DCRWritePriv( _DCR( CLOCKSTOP_INTERRUPT_STATE__FORCE ), inject );
	      ppc_msync();
	  }

#endif

      }
  }
  else {


      if ( fwext_strcmp("L1PBug",testId) == 0 ) {

	  if ( ( ProcessorID() % 4 ) == 0 ) {
	      uint64_t end = GetTimeBase() + 1600ull * 1000ull * 1000ull * 10;

	      while ( GetTimeBase() < end ) {
		  uint64_t esr = in64( (uint64_t*)L1P_ESR );
		  out64_sync((void *)L1P_ESR, ~esr );
	      }

	      //printf( "shutting down ...\n");
	  }
      }
  }

  
  fwext_barrier( &barrier, N );
  fwext_barrier( &barrier, N );

  exit(0);
}



