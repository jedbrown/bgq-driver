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




#include <firmware/include/personality.h>
#include <firmware/include/mailbox.h>
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <hwi/include/common/uci.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char JTAG_LOC[256];


const char* NC_ENABLES[] = {
"PERS_ENABLE_MMU             ", // FW_BIT(0)   //! Enables use of the MMU.  NOTE: this bit may go away once MMU support stabilizes.
"PERS_ENABLE_IsIoNode        ", // FW_BIT(1)   //! @deprecated Used as a proxy for testint bit
"PERS_ENABLE_TakeCPU         ", // FW_BIT(2)   //! Enables use of the takeCPU service.
"PERS_ENABLE_MU              ", // FW_BIT(3)   //! Enables the Messaging Unit
"PERS_ENABLE_ND              ", // FW_BIT(4)   //! Enables the Network Device
"PERS_ENABLE_Timestamps      ", // FW_BIT(5)   //! Enables automatic embedding of timestamps in mailbox stdout messages
"PERS_ENABLE_BeDRAM          ", // FW_BIT(6)   //! Enables BeDRAM initialization sequence
"PERS_ENABLE_ClockStop       ", // FW_BIT(7)   //! Enables ClockStop Unit initialization
"PERS_ENABLE_DrArbiter       ", // FW_BIT(8)   //! Enables DR Arbiter Unit initialization
"PERS_ENABLE_DevBus          ", // FW_BIT(9)   //! Enables Device Bus Unit initialization
"PERS_ENABLE_L1P             ", // FW_BIT(10)  //! Enables L1 Prefetch Unit initialization
"PERS_ENABLE_L2              ", // FW_BIT(11)  //! Enables L2 Unit initialization (including L2 Central
"PERS_ENABLE_MSGC            ", // FW_BIT(12)  //! Enables MSGC Unit initialization
"PERS_ENABLE_TestInt         ", // FW_BIT(13)  //! Enables Test Interface Unit initialization
"PERS_ENABLE_NodeRepro       ", // FW_BIT(14)  //! Enable single node cycle reproducibility
"PERS_ENABLE_PartitionRepro  ", // FW_BIT(15)  //! Enable partition-wide cycle reproducibility
"PERS_ENABLE_DD1_Workarounds ", // FW_BIT(16)  //! @deprecated Enables DD1 hardware workarounds in firmware.  DD1 hardware support may disappear in the future.
"PERS_ENABLE_A2_Errata       ", // FW_BIT(17)  //! Enables A2 workarounds
"PERS_ENABLE_A2_IU_LLB       ", // FW_BIT(18)  //! Enables the A2 I Unit Livelock Buster
"PERS_ENABLE_A2_XU_LLB       ", // FW_BIT(19)  //! Enables the A2 X Unit Livelock Buster
"PERS_ENABLE_DDRCalibration  ", // FW_BIT(20)  //! Enables DDR Calibration
"PERS_ENABLE_DDRFastInit     ", // FW_BIT(21)  //! Enables DDR Fast Init
"PERS_ENABLE_DDRCellTest     ", //22
"PERS_ENABLE_DDRAutoSize     ", //23
"PERS_ENABLE_MaskLinkErrors  ", //24
"PERS_ENABLE_MaskCorrectables", //25
"PERS_ENABLE_DDRDynamicRecal ", //26
"PERS_ENABLE_DDRNoTerm       ", //27
"PERS_ENABLE_DDRBackScrub    ", //28
"PERS_ENABLE_DirScrub        ", //29
0, //30
0, //31
0, //32
0, //33
0, //34
0, //35
"PERS_ENABLE_AppPreload      ", // FW_BIT(36)  //! Indicates that application loading via CIOS is unecessary
"PERS_ENABLE_IOServices      ", // FW_BIT(37)  //! Enables CIOS function shipping
"PERS_ENABLE_SpecCapDDR      ", // FW_BIT(38)  //! DDR storage is speculation-capable
"PERS_ENABLE_L2Only          ", // FW_BIT(39)  //! Run out of L2, without DDR
"PERS_ENABLE_FPGA            ", // FW_BIT(40)  //! Running in FPGA simulation
"PERS_ENABLE_DDRINIT         ", // FW_BIT(41)  //! Enables the DDR controller initialization
"PERS_ENABLE_Mailbox         ", // FW_BIT(42)  //! Enables the mailbox
"PERS_ENABLE_Simulation      ", // FW_BIT(43)  //! Running in cycle simulation
"PERS_ENABLE_Mambo           ", // FW_BIT(44)  //! Running in Mambo (who uses this ... Linux?)
"PERS_ENABLE_JTagConsole     ", // FW_BIT(45)  //! Routes application console output through jtag (versus function shipped)
"PERS_ENABLE_JTagLoader      ", // FW_BIT(46)  //! Converse with the JTag host to load additional images (kernels and such)
"PERS_ENABLE_FPU             ", // FW_BIT(47)  //! Enable the QPX floating point hardware
"PERS_ENABLE_L2Counters      ", // FW_BIT(48)  //! Enable the L2 atomics (BeDRAM counters always enabled)
"PERS_ENABLE_Wakeup          ", // FW_BIT(49)  //! Enable the Wakeup Unit initialization
"PERS_ENABLE_BIC             ", // FW_BIT(50)  //! Enable Blue Gene Interrupt Controller
"PERS_ENABLE_DDR             ", // FW_BIT(51)  //! Enable DDR controllers
0, // 52
0, // 53
"PERS_ENABLE_GlobalInts      ", // FW_BIT(54)  //! Enable global interrupts embedded in Network
"PERS_ENABLE_SerDes          ", // FW_BIT(55)  //! Enable High-speed Serializer/Deserializer
"PERS_ENABLE_UPC             ", // FW_BIT(56)  //! Enable Universal Performance Counter Unit initializaiton
"PERS_ENABLE_EnvMon          ", // FW_BIT(57)  //! Enable Environmental Monitoring
"PERS_ENABLE_PCIe            ", // FW_BIT(58)  //! Enable PCIExpress
0, // 59
0, // 60
"PERS_ENABLE_TimeSync        ", // 61
0, // 62
"PERS_ENABLE_DiagnosticsMode ", // FW_BIT(63)  //! Enable diagnostics mode

};

#define offsetof(st, m)  ((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))

void showKernelConfig(Personality_t* p) {
  Personality_Kernel_t* k = &(p->Kernel_Config);
  int i;
  printf("%s [%04lX] Kernel_Config -> %p\n", JTAG_LOC, offsetof(Personality_t,Kernel_Config), k);
  printf("%s [%04lX]   UCI         : %016llX\n", JTAG_LOC, offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,UCI), k->UCI);
  printf("%s [%04lX]   NodeConfig  : %016llX\n", JTAG_LOC, offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,NodeConfig), k->NodeConfig);
  for (i=0; i< 64; i++) {
      if ( (k->NodeConfig & (1ull<<(63-i))) != 0 ) {
	  printf("%s [****]     %s\n", JTAG_LOC, NC_ENABLES[i]);
      }
  }
  printf("%s [%04lX]   TraceConfig : %016llX\n", JTAG_LOC, offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,TraceConfig), k->TraceConfig);
  printf("%s [%04lX]   RASPolicy   : %08X\n", JTAG_LOC, offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,RASPolicy), k->RASPolicy);
  printf("%s [%04lX]   FreqMHz     : 0x%08X (%d)\n", JTAG_LOC, offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,FreqMHz), k->FreqMHz, k->FreqMHz);
  printf("%s [%04lX]   ClockStop   : 0x%016llX\n", JTAG_LOC, offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,ClockStop), k->ClockStop);
}

void showDDRConfig(Personality_t* p) {
    // TBD
}

void showNetConfig(Personality_t* p) {
  
  Personality_Networks_t* n = &(p->Network_Config);
  fw_uint64_t nf  = n->NetFlags;
  fw_uint64_t nf2 = n->NetFlags2;

  printf("%s [%04lX] Network_Config -> %p\n", JTAG_LOC, offsetof(Personality_t,Network_Config), n);
  printf("%s [%04lX]   BlockID         : %08X\n",   JTAG_LOC, offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,BlockID), n->BlockID);
  printf("%s [%04lX]   NetFlags        : %016llX\n", JTAG_LOC, offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,NetFlags), nf);
  printf("%s [****]     I/O Node Link : %d\n", JTAG_LOC, ND_GET_ENABLE_ION_LINK(nf));
  printf("%s [****]     Loopback      : (%lld,%lld,%lld,%lld,%lld,%lld) 0=None 1=Torus 2=SerDes 3=HSS\n",  JTAG_LOC, 
	 ND_GET_LOOPBACK(0,nf),ND_GET_LOOPBACK(1,nf),ND_GET_LOOPBACK(2,nf),
	 ND_GET_LOOPBACK(3,nf),ND_GET_LOOPBACK(4,nf),ND_GET_LOOPBACK(5,nf));
  printf ("%s [****]     Torus/Mesh    : (%lld,%lld,%lld,%lld,%lld) (0=Mesh, 1=Torus)\n", JTAG_LOC, 
	  ND_GET_TORUS(0,nf), ND_GET_TORUS(1,nf), ND_GET_TORUS(2,nf), 
	  ND_GET_TORUS(3,nf), ND_GET_TORUS(4,nf) );
  printf ("%s [****]     RanHints      : (%lld,%lld,%lld,%lld,%lld)\n", JTAG_LOC, 
	  ND_GET_TORUS_RAN_HINTS(0,nf), ND_GET_TORUS_RAN_HINTS(1,nf), ND_GET_TORUS_RAN_HINTS(2,nf), 
	  ND_GET_TORUS_RAN_HINTS(3,nf), ND_GET_TORUS_RAN_HINTS(4,nf) );
  printf ("%s [****]     Re/Inj Ration : %lld\n", JTAG_LOC, ND_GET_SE_ARB_RE_INJ_RATIO(nf) );
  printf ("%s [****]     DetOrderMasks : (%02llX,%02llX,%02llX,%02llX,%02llX)\n", JTAG_LOC, ND_GET_DET_ORDER_MASK(0,nf),ND_GET_DET_ORDER_MASK(1,nf),ND_GET_DET_ORDER_MASK(2,nf),ND_GET_DET_ORDER_MASK(3,nf),ND_GET_DET_ORDER_MASK(4,nf));
  printf("%s [%04lX]   NetFlags2       : %016llX\n", JTAG_LOC, offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,NetFlags2), nf2);

  static char* DIMENSIONS[] = { "A", "B", "C", "D", "E", "IO" };
  static char* DIRECTIONS[] = { "-", "+" };

  printf("%s [****]       Torus Links Enabled : ", JTAG_LOC );
  {

    unsigned dimension, direction;

    for ( dimension = 0; dimension < 5; dimension++ ) {
      for ( direction = 0; direction <= 1; direction++ ) {
	if ( ND_GET_LINK_ENABLE(dimension, direction, p->Network_Config.NetFlags2 ) ) {
	  printf("%s%s ", DIMENSIONS[dimension], DIRECTIONS[direction]);
	}
      }
    }
  }
  printf("\n");
  printf("%s [****]       SerDes Links Enabled : ", JTAG_LOC );
  {

    unsigned dimension, direction;

    for ( dimension = 0; dimension <= 5; dimension++ ) {
      for ( direction = 0; direction <= 1; direction++ ) {
	if ( SD_GET_LINK_ENABLE(dimension, direction, p->Network_Config.NetFlags2 ) ) {
	    printf("%s%s ", DIMENSIONS[dimension], DIRECTIONS[direction]);
	}
      }
    }
  }
  printf("\n");

  printf("%s [****]       TIO       : %d\n", JTAG_LOC, nf2 & SD_ENABLE_TIO ? 1 : 0 );
  printf("%s [****]       HalfSpeed : %d\n", JTAG_LOC, nf2 & SD_ENABLE_HALFSPEED ? 1 : 0 );
  printf("%s [****]       Use Port 6: %d\n", JTAG_LOC, nf2 & TI_USE_PORT6_FOR_IO ? 1 : 0 );
  printf("%s [****]       Use Port 7: %d\n", JTAG_LOC, nf2 & TI_USE_PORT7_FOR_IO ? 1 : 0 );
  printf("%s [****]       Swaps     : (%lld,%lld,%lld,%lld,%lld)\n", JTAG_LOC, TI_GET_TORUS_DIM_REVERSED(0,nf2),TI_GET_TORUS_DIM_REVERSED(1,nf2), TI_GET_TORUS_DIM_REVERSED(2,nf2), TI_GET_TORUS_DIM_REVERSED(3,nf2), TI_GET_TORUS_DIM_REVERSED(4,nf2));
  printf("%s [%04lX]   Torus           : (%d,%d,%d,%d,%d) / (%d,%d,%d,%d,%d)\n", JTAG_LOC, offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,Anodes), 
	 n->Acoord, n->Bcoord, n->Ccoord, n->Dcoord, n->Ecoord,
	 n->Anodes, n->Bnodes, n->Cnodes, n->Dnodes, n->Enodes
	 );
  
  printf("%s [%04lX]   PrimordialClassRoute\n", JTAG_LOC, offsetof(Personality_t, Network_Config) + offsetof(Personality_Networks_t,PrimordialClassRoute));
  printf("%s [%04lX]     GlintUpPortInputs     : %04X\n", JTAG_LOC, (uint64_t)(&(n->PrimordialClassRoute.GlobIntUpPortInputs)) - (uint64_t)p, n->PrimordialClassRoute.GlobIntUpPortInputs );
  printf("%s [%04lX]     GlintUpPortOutputs    : %04X\n", JTAG_LOC, (uint64_t)(&(n->PrimordialClassRoute.GlobIntUpPortOutputs)) - (uint64_t)p, n->PrimordialClassRoute.GlobIntUpPortOutputs );
  printf("%s [%04lX]     CollTypeUpPortInputs  : %04X\n", JTAG_LOC, (uint64_t)(&(n->PrimordialClassRoute.CollectiveTypeAndUpPortInputs)) - (uint64_t)p, n->PrimordialClassRoute.CollectiveTypeAndUpPortInputs );
  printf("%s [%04lX]     CollTypeUpPortOutputs : %04X\n", JTAG_LOC, (uint64_t)(&(n->PrimordialClassRoute.CollectiveUpPortOutputs)) - (uint64_t)p, n->PrimordialClassRoute.CollectiveUpPortOutputs );
  printf("%s [%04lX]   ZoneRoutingMasks        : ", JTAG_LOC, offsetof(Personality_t, Network_Config) + offsetof(Personality_Networks_t,ZoneRoutingMasks));
  { 
      unsigned zone;
      for ( zone = 0; zone < NUM_ND_ZONES; zone++ ) {
	  printf(" %016X", n->ZoneRoutingMasks[zone]);
      }
  }
  printf("\n");
  
  printf("%s [%04lX]   MuFlags         : %016llX\n", JTAG_LOC, offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,MuFlags), n->MuFlags);
  printf("%s [%04lX]   CN Bridge       : (%d,%d,%d,%d,%d)\n", JTAG_LOC, offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,cnBridge_A), 
	 n->cnBridge_A, n->cnBridge_B, n->cnBridge_C, n->cnBridge_D, n->cnBridge_E );
  printf("%s [%04lX]   latencyFromRoot : %d\n", JTAG_LOC, offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,latencyFromRoot), n->latencyFromRoot);
}

void showEthConfig(Personality_t* p) {
 
  Personality_Ethernet_t* n = &(p->Ethernet_Config);
  printf("%s [%04lX] Ethernet_Config -> %p\n", JTAG_LOC, offsetof(Personality_t,Ethernet_Config), n);
}

int showPersonality() {

  Personality_t* p = fwext_getPersonality();

  bg_uci_toString( p->Kernel_Config.UCI, JTAG_LOC );

  printf("%s      Personality -> %p\n", JTAG_LOC, p);
  printf("%s [%04lX] CRC       : 0x%X\n", JTAG_LOC, offsetof(Personality_t,CRC), p->CRC );
  printf("%s [%04lX] Version   : 0x%X\n", JTAG_LOC, offsetof(Personality_t,Version), p->Version );
  printf("%s [%04lX] SizeWords : 0x%X\n", JTAG_LOC, offsetof(Personality_t,PersonalitySizeWords), p->PersonalitySizeWords );

  showKernelConfig(p);
  showDDRConfig(p);
  showNetConfig(p);
  showEthConfig(p);


  printf("%s TESTINT_CONFIG1 : %016lX IO:%d Port6:%d Port7:%d Swaps:%02lX\n", JTAG_LOC, DCRReadPriv( TESTINT_DCR(CONFIG1) ), TI_IsIoNode(), TI_UsePort6forIO(), TI_UsePort7forIO(), TI_GetDimensionSwaps() );
  return 0;
}

#define FW_BASE_ADDRESS  0x03fffffc0000UL  // NOTE: This must match the linker script!

void dumpBeDRAM() {
    uint64_t* w = (uint64_t*)FW_BASE_ADDRESS;
    unsigned i;

    for ( i = 0; i < 640; i+=4 ) {
	printf( "%p : %016lX %016lX %016lX %016lX\n", w + i, w[i], w[i+1], w[i+2], w[i+3] );
    }
    
}

void waitForShutdown() {

    char msgbuff[128];
    fw_uint32_t message_type = 0;

    // Indicate that we have completed booting:
  fwext_getFwInterface()->sendBlockStatus( JMB_BLOCKSTATE_INITIALIZED, 0, 0 );

  for ( ;; ) {

      if ( fwext_getFwInterface()->pollInbox(msgbuff, &message_type, sizeof(msgbuff) ) == FW_EAGAIN ) {
	  fwext_udelay(1000);
      }
      else {
	  switch ( message_type ) {

	  case JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST : 
	  {
	      MailBoxPayload_ControlSystemRequest_t* msg = (MailBoxPayload_ControlSystemRequest_t*)msgbuff;
	      switch ( msg->sysreq_id ) {
	      case JMB_CTRLSYSREQ_SHUTDOWN_IO_LINK  : 
	      {
		  fw_uint64_t block_id = msg->details.shutdown_io_link.block_id;
		  fwext_getFwInterface()->sendBlockStatus( JMB_BLOCKSTATE_IO_LINK_CLOSED, 1, &block_id );
		  break;
	      }

	      case JMB_CTRLSYSREQ_SHUTDOWN : 
	      {
		  fwext_getFwInterface()->sendBlockStatus( JMB_BLOCKSTATE_HALTED, 0, 0 );
		  return;
		  break;
	      }
	    
	    
	      }
	      break;
	  }
	    
	  default : {
	      break;
	  }
	  }
      }
  }


}

int test_main( void ) {

  if ( ProcessorID() != 0 ) {
    _Bg_Sleep_Forever();
  }

  showPersonality();

  if ( ( fwext_getPersonality()->Kernel_Config.TraceConfig & TRACE_Verbose ) != 0 ) {
      dumpBeDRAM();
  }

  waitForShutdown();
  Terminate(0);
}



