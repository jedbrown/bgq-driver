/* -------------------------------------------------------------- */
/* IBM Confidential                                               */
/*                                                                */
/* Licensed Machine Code Source Materials                         */
/* Blue Gene/Q Licensed Machine Code                              */
/*                                                                */
/* (c) Copyright IBM Corp. 20012, 2012                            */
/*                                                                */
/* The Source code for this program is not published or otherwise */
/* divested of its trade secrets, irrespective of what has been   */
/* deposited with the U.S. Copyright Office.                      */
/* -------------------------------------------------------------- */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <firmware/include/Firmware.h>
#include <firmware/include/mailbox.h>
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>

#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/l2_central_inlines.h>
#include <hwi/include/bqc/BIC.h>
#include <hwi/include/bqc/BIC_inlines.h>
#include <hwi/include/bqc/bedram_dcr.h>
#include <hwi/include/bqc/cs_dcr.h>
#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/devbus_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/l1p_dcr.h>
#include <hwi/include/bqc/l1p_mmio.h>
#include <hwi/include/bqc/upc_c_dcr.h>


#define NUM_THREADS   68
#define MAX_ITERS 100000
#define DIAGS_BQCGENERIC_FATAL_RAS_ID 0x0002003F

volatile uint64_t status[NUM_THREADS] = { 0, };
volatile uint64_t reg0[NUM_THREADS] = { 0, };
volatile uint64_t reg1[NUM_THREADS] = { 0, };
volatile int global_distance;
volatile int shutdown = 0;
volatile int global_source = -1;

void interruptHandler(FwExt_Regs_t* context, uint64_t code);
int pollForShutdown(void);



void dumpBic() {
    printf("GEA map0: %016lX map1: %016lX map2: %016lX map3: %016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_map0, GEA_DCR_PRIV_PTR->gea_interrupt_map1, GEA_DCR_PRIV_PTR->gea_interrupt_map2, GEA_DCR_PRIV_PTR->gea_interrupt_map3 );
    printf("GEA map4: %016lX map5: %016lX map6: %016lX map7: %016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_map4, GEA_DCR_PRIV_PTR->gea_interrupt_map5, GEA_DCR_PRIV_PTR->gea_interrupt_map6, GEA_DCR_PRIV_PTR->gea_interrupt_map7 );
    printf("GEA map8: %016lX map9: %016lX                          \n", GEA_DCR_PRIV_PTR->gea_interrupt_map8, GEA_DCR_PRIV_PTR->gea_interrupt_map9 );
    printf("GEA sta0: %016lX sta1: %016lX sta2:%016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_status0, GEA_DCR_PRIV_PTR->gea_interrupt_status1, GEA_DCR_PRIV_PTR->gea_interrupt_status2 );

    BIC_Device* bic = ((BIC_Device *)BIC_DEVICE_BASE_ADDRESS);
    printf("BIC map : %016llX-%016llX-%016llX-%016llX\n", bic->_map_interrupt[0],bic->_map_interrupt[1],bic->_map_interrupt[2],bic->_map_interrupt[3]);
    printf("BIC EI  : %016llX-%016llX-%016llX-%016llX\n", bic->_ext_int_summary[0],bic->_ext_int_summary[1],bic->_ext_int_summary[2],bic->_ext_int_summary[3]);
    printf("BIC CI  : %016llX-%016llX-%016llX-%016llX\n", bic->_crit_int_summary[0],bic->_crit_int_summary[1],bic->_crit_int_summary[2],bic->_crit_int_summary[3]);
    printf("BIC MC  : %016llX-%016llX-%016llX-%016llX\n", bic->_mach_int_summary[0],bic->_mach_int_summary[1],bic->_mach_int_summary[2],bic->_mach_int_summary[3]);
    printf("BIC ist : %016llX\n", bic->_input_status );

}

int test_main( void ) {

  uint64_t barrierCount = NUM_THREADS;
  int tid = ProcessorID(); // 0..67
  //int core = ProcessorCoreID(); // 0..16
  //int rc = 0;
  Personality_Networks_t* nw = &(fwext_getPersonality()->Network_Config);
  int iAmTheRootNode = (nw->Acoord==0) && (nw->Bcoord==0) && (nw->Ccoord==0) && (nw->Dcoord==0) && (nw->Ecoord==0); // torus node (0,0,0,0,0)
  int iAmTheRootThread = iAmTheRootNode && (tid == 0);

  /*
   * Establish an interrupt handler (see implementation below)
   */

  fwext_setInterruptHandler( interruptHandler );


  /*
   * Issue a "block initialized" message to the control system so that the block leaves 'B' state
   * and the boot does not time out.  This is not necessary for a diagnostic.
   */

  if ( tid == 0 ) {
    fwext_getFwInterface()->sendBlockStatus( JMB_BLOCKSTATE_INITIALIZED, 0, 0 );
  }


  if ( iAmTheRootThread != 0 ) {
      printf("Starting Test ...\n");
  }

  fwext_udelay(1000000);


  // Phase 1: Configuration.  Thread 0 of every core will update its PUEA (BIC) map
  //          to direct L1P lane 1 to critical interrupt.  Also configures L1P_ESR
  //          to mask MMIO privileged condition from the GEA; instead that condition
  //          is enabled for PUEA L1P lane 1 (aka L1P_ESR_BIC_CRITICAL).

  if ( PhysicalThreadID() == 0 ) {

      out64_sync( (void*)L1P_ESR_GEA, in64((void*)L1P_ESR_GEA) & ~L1P_ESR_err_mmio_priv ); // Steal from firmware
      out64_sync( (void*)L1P_ESR_BIC_CRITICAL, L1P_ESR_err_mmio_priv );

      BIC_InsertInterruptMap( 0, BIC_MAP_L1P_LANE(1), BIC_CRITICAL_INTERRUPT );
  }

  if ( (iAmTheRootNode != 0 ) && ( tid == 0 ) ) {
      printf("BIC after initialization:\n");
      dumpBic();
  }

  fwext_udelay(1000000);
  BgBarrier(1, barrierCount);
  barrierCount += NUM_THREADS;


  // Phase 2:  A specific thread on the root node triggers an MMIO
  //           privileged condition by injecting into ESR.

  if ( ( iAmTheRootNode != 0 )  && ( tid == 19 ) ) {

      printf("Injecting error ...\n");
	
      out64_sync((void *)L1P_ESR_INJ_DCR(ProcessorCoreID()), L1P_ESR_err_mmio_priv );
      ppc_msync();
      out64_sync((void *)L1P_ESR_INJ_DCR(ProcessorCoreID()), 0 );
      ppc_msync();

  }

  if ( (iAmTheRootNode != 0 ) && ( tid == 3 ) ) {
      fwext_udelay(5000000);
      printf("BIC after trigger:\n");
      dumpBic();
  }



  /*
   * All threads sync up here.
   */

  BgBarrier(1, barrierCount);
  barrierCount += NUM_THREADS;


  if ( iAmTheRootThread != 0 ) {
      printf("Waiting for shutdown down ...\n");
  }



  BgBarrier(1, barrierCount);
  barrierCount += NUM_THREADS;

  if ( tid == 0 ) {
    int done = 0;
    while ( ( done = pollForShutdown() ) == 0 );
  }

  test_exit(0);       // everybody exit with success
}



void interruptHandler(FwExt_Regs_t* context, uint64_t code) {

  int tid = PhysicalThreadIndex();

  printf("(E) interrupt code=%lX stat: %016llX-%016llX-%016llX-%016llX tid=%d\n", code, BIC_ReadCriticalIntSummary(0), BIC_ReadCriticalIntSummary(1), BIC_ReadCriticalIntSummary(2), BIC_ReadCriticalIntSummary(3), tid );
  dumpBic();

  if ( code == IVO_CI ) { // Critical interrupt

      int i;
      int acked = 0;
      
      for ( i = 0; i < 4; i++ ) {

	  uint64_t stat = BIC_ReadCriticalIntSummary(i);

	  if ( stat & PUEA_INTERRUPT_STATUS_L1P(1) ) {
	      uint64_t esr = in64( (void*)L1P_ESR );
	      printf("(E) L1P error detected on thread %d ESR=%016lX\n", i, esr );

	      if ( esr & L1P_ESR_err_mmio_priv ) {
		  printf("(E) L1P MMIO Priv error detected.\n");
		  out64_sync( (void*)L1P_ESR, L1P_ESR_err_mmio_priv );
		  acked = 1;
	      }
	  }
      }

      if ( acked != 0 ) {
	  return;
      }
  }

  // Should not get here
  test_exit( - __LINE__ );

}



int pollForShutdown() {

  char message_buffer[64];
  fw_uint32_t message_type = 0;

  if ( fwext_getFwInterface()->pollInbox(message_buffer, &message_type, sizeof(message_buffer) ) == FW_EAGAIN ) {
    return 0;
  }

  switch ( message_type ) {

    case JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST :
    {
      MailBoxPayload_ControlSystemRequest_t* msg = (MailBoxPayload_ControlSystemRequest_t*)message_buffer;
      switch ( msg->sysreq_id ) {
        case JMB_CTRLSYSREQ_SHUTDOWN_IO_LINK  :
        {
          fw_uint64_t block_id = msg->details.shutdown_io_link.block_id;
          fwext_getFwInterface()->sendBlockStatus( JMB_BLOCKSTATE_IO_LINK_CLOSED, 1, &block_id );
          //printf("Link for block id %lld closed\n", block_id );
          break;
        }

        case JMB_CTRLSYSREQ_SHUTDOWN :
        {
          fwext_getFwInterface()->sendBlockStatus( JMB_BLOCKSTATE_HALTED, 0, 0 );
          //printf("Block halted!\n");
          return 1;
          break;
        }
      }
      break;
    }
  }

  return 0;
}
