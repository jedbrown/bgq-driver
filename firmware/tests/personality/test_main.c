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
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TRACE(x) printf x;
//#define TRACE(x)

#define TRACE_ENTRY()
#define TRACE_EXIT()
//#define TRACE_ENTRY() TRACE(("(>) %s() [%s:%d]\n", __func__, __FILE__, __LINE__))
//#define TRACE_EXIT()  TRACE(("(<) %s() [%s:%d]\n", __func__, __FILE__, __LINE__))

#define ASSERT( condition, message ) \
  if ( ! (condition) ) { printf( "[ERROR] : Assertion failed at  %s:%s():%d\n", __FILE__, __func__, __LINE__); \
    printf message ; \
    return -1;	     \
  }

#define ASSERT2( condition, message, rc )					\
  if ( ! (condition) ) { printf( "[ERROR] : Assertion failed at  %s:%s():%d\n", __FILE__, __func__, __LINE__); \
    printf message ; \
    rc =  -1;	     \
  }


typedef int (*Test_t)();


int my_strcmp( char* a, char* b ) {

  int i = 0;

  while ( a[i] != 0 ) {
    if ( a[i] != b[i] )
      return a[i] < b[i] ? -1 : 1;
    i++;
  }
  
  return b[i] == 0 ? 0 : -1;
}

int isEmptyIpAddr( IP_Addr_t ip ) {
  int i;
  for ( i = 0 ; i < sizeof(ip.octet) / sizeof( ip.octet[0] ); i++ ) {
    if ( ip.octet[i] != 0 ) {
      return 0;
    }
  }
  return 1;
}

char* formatIpAddr( IP_Addr_t ip ) {
  return "?";
}

Personality_t DefaultPersonality = PERSONALITY_DEFAULT();

int validateDefaultPersonality() {

    if ( ( fwext_getenv("DISABLE.VALIDATE.DEFAULT" ) ) != 0 ) {
	printf("(I) %s disabled\n", __func__);
	return 0;
    }
    
  /*
   * NOTE: This test checks the bits of the default personality.  The test is somewhat brittle but that is partially
   *       by design.  The PERSONALITY_DEFAULT macro is shared across multiple utilities (svchost, control system, firmware)
   *       spanning multiple languages (C, C++).
   */

  int rc = 0;
  int i;
  Personality_t* p = &DefaultPersonality;


  ASSERT2( p->Version == PERSONALITY_VERSION, ("%s mismatch: (actual) %d vs %d (expected)\n", "Version", p->Version, PERSONALITY_VERSION), rc );
  ASSERT2( p->PersonalitySizeWords == (sizeof(Personality_t)/4), ("%s mismatch: (actual) %d vs %ld (expected)\n", "PersonalitySizeWords", p->PersonalitySizeWords, sizeof(Personality_t)/4 ), rc );

  fw_uint64_t nodeConfig =
    PERS_ENABLE_Mailbox |
    PERS_ENABLE_DrArbiter |
    PERS_ENABLE_FPU |					       
    PERS_ENABLE_BIC |					       
    PERS_ENABLE_DDR |		
    PERS_ENABLE_L1P |
    PERS_ENABLE_GlobalInts |				       
    PERS_ENABLE_SpecCapDDR |		   
    PERS_ENABLE_JTagConsole |
    PERS_ENABLE_AppPreload |
      PERS_ENABLE_DiagnosticsMode |
      PERS_ENABLE_TimeSync | 
      PERS_ENABLE_MSGC |
    // <now turned off by default> PERS_ENABLE_DD1_Workarounds |
    // PERS_ENABLE_EnvMon |				       
    /* PERS_ENABLE_MMU | @todo turn this on */		       
    0;
  
  fw_uint64_t traceConfig =
    TRACE_MChk |
    0;

  ASSERT2( p->Kernel_Config.NodeConfig == nodeConfig, ("%s mismatch: (actual) %llX vs %llX (expected)\n", "Kernel_Config.NodeConfig", p->Kernel_Config.NodeConfig, nodeConfig ), rc );
  ASSERT2( p->Kernel_Config.TraceConfig == traceConfig, ("%s mismatch: (actual) %llX vs %llX (expected)\n", "Kernel_Config.TraceConfig", p->Kernel_Config.TraceConfig, traceConfig ), rc );
  ASSERT2( p->Kernel_Config.RASPolicy == 0, ("%s mismatch: (actual) %X vs %X (expected)\n", "Kernel_Config.RASPolicy", p->Kernel_Config.RASPolicy, 0 ), rc );
  ASSERT2( p->Kernel_Config.FreqMHz == DEFAULT_FREQ_MHZ, ("%s mismatch: (actual) %d vs %d (expected)\n", "Kernel_Config.FreqMHz", p->Kernel_Config.FreqMHz, DEFAULT_FREQ_MHZ ), rc );

  ASSERT2( p->DDR_Config.DDRFlags == DDR_POWER_DOWN_FAST, ("%s mismatch: (actual) %d vs %d (expected)\n", "DDR_Config.DDRFlags", p->DDR_Config.DDRFlags, DDR_POWER_DOWN_FAST), rc );
  ASSERT2( p->DDR_Config.DDRSizeMB == (16*1024), ("%s mismatch: (actual) %d vs %d (expected)\n", "DDR_Config.DDRSizeMB", p->DDR_Config.DDRSizeMB, (16*1024) ), rc );


  fw_uint64_t netFlags = 0 |
    ND_ENABLE_ION_LINK | /* simulation only */		       
    ND_DIM_A_LOOPBACK(ND_LOOPBACK_TORUS_INTERNAL) |       
    ND_DIM_B_LOOPBACK(ND_LOOPBACK_TORUS_INTERNAL) |       
    ND_DIM_C_LOOPBACK(ND_LOOPBACK_TORUS_INTERNAL) |       
    ND_DIM_D_LOOPBACK(ND_LOOPBACK_TORUS_INTERNAL) |       
    ND_DIM_E_LOOPBACK(ND_LOOPBACK_TORUS_INTERNAL) |       
    ND_DIM_IO_LOOPBACK(ND_LOOPBACK_TORUS_INTERNAL) |      
    ND_ENABLE_TORUS(0) |                                  
    ND_ENABLE_TORUS(1) |                                  
    ND_ENABLE_TORUS(2) |                                  
    ND_ENABLE_TORUS(3) |                                  
    ND_ENABLE_TORUS(4) |                                  
    ND_ENABLE_TORUS_RAN_HINTS(0) |                        
    ND_ENABLE_TORUS_RAN_HINTS(1) |                        
    ND_ENABLE_TORUS_RAN_HINTS(2) |                        
    ND_ENABLE_TORUS_RAN_HINTS(3) |                        
    ND_ENABLE_TORUS_RAN_HINTS(4) |                        
    ND_SE_ARB_RE_INJ_RATIO(0xF)  |                        
    ND_DET_ORDER_MASK(0,0x10) | /* A first */	       
    ND_DET_ORDER_MASK(1,0x08) | /* B second */	       
    ND_DET_ORDER_MASK(2,0x04) | /* C third */	       
    ND_DET_ORDER_MASK(3,0x02) | /* D fourth */	       
    ND_DET_ORDER_MASK(4,0x01) | /* E fifth */	       
    0;

  ASSERT2( p->Network_Config.NetFlags == netFlags, ("%s mismatch: (actual) %llX vs %llX (expected)\n", "Network_Config.NetFlags", p->Network_Config.NetFlags, netFlags ), rc );
  ASSERT2( p->Network_Config.Anodes == 1, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Anodes", p->Network_Config.Anodes, 1 ), rc );
  ASSERT2( p->Network_Config.Bnodes == 1, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Bnodes", p->Network_Config.Bnodes, 1 ), rc );
  ASSERT2( p->Network_Config.Cnodes == 1, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Cnodes", p->Network_Config.Cnodes, 1 ), rc );
  ASSERT2( p->Network_Config.Dnodes == 1, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Dnodes", p->Network_Config.Dnodes, 1 ), rc );
  ASSERT2( p->Network_Config.Enodes == 1, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Enodes", p->Network_Config.Enodes, 1 ), rc );
  ASSERT2( p->Network_Config.Acoord == 0, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Acoord", p->Network_Config.Acoord, 0 ), rc );
  ASSERT2( p->Network_Config.Bcoord == 0, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Bcoord", p->Network_Config.Bcoord, 0 ), rc );
  ASSERT2( p->Network_Config.Ccoord == 0, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Ccoord", p->Network_Config.Ccoord, 0 ), rc );
  ASSERT2( p->Network_Config.Dcoord == 0, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Dcoord", p->Network_Config.Dcoord, 0 ), rc );
  ASSERT2( p->Network_Config.Ecoord == 0, ("%s mismatch: (actual) %d vs %d (expected)\n", "Network_Config.Ecoord", p->Network_Config.Ecoord, 0 ), rc );

  ASSERT2( 
      p->Network_Config.PrimordialClassRoute.GlobIntUpPortInputs == 0x0001, 
      ("Network_Config.PrimordialClassRoute.GlobIntUpPortInputs mismatch: (actual) %d vs %d (expected)\n", p->Network_Config.PrimordialClassRoute.GlobIntUpPortInputs, 0x0001 ),
      rc );
  ASSERT2( 
      p->Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs == 0, 
      ("Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs mismatch: (actual) %d vs %d (expected)\n", p->Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs, 0 ),
      rc );
  ASSERT2( 
      p->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs == 0, 
      ("Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs mismatch: (actual) %d vs %d (expected)\n", p->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs, 0 ),
      rc );
  ASSERT2( 
      p->Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs == 0, 
      ("Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs mismatch: (actual) %d vs %d (expected)\n", p->Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs, 0 ),
      rc );
 

  for (i = 0; i < NUM_ND_ZONES; i++ ) {
      ASSERT2( p->Network_Config.ZoneRoutingMasks[i] == 0, ("%s[%d] mismatch: (actual) %d vs %d (expected)\n", "Network_Config.ZoneRoutingMasks", i, p->Network_Config.ZoneRoutingMasks[i], 0 ), rc );
  }


  printf("(I) %s complete rc=%d\n", __func__, rc );
  return rc;
}

int validatePersonalizedFirmware() {

  int rc = 0;
  Personality_t* p = fwext_getPersonality();

  if ( ( fwext_getenv("DISABLE.VALIDATE.PERSONALIZED" ) ) != 0 ) {
      printf("(I) %s disabled\n", __func__);
      return 0;
  }

  ASSERT2( p->Version == PERSONALITY_VERSION, ("%s mismatch: (actual) %d vs %d (expected)\n", "Version", p->Version, PERSONALITY_VERSION), rc );
  ASSERT2( p->PersonalitySizeWords == (sizeof(Personality_t)/4), ("%s mismatch: (actual) %d vs %ld (expected)\n", "PersonalitySizeWords", p->PersonalitySizeWords, sizeof(Personality_t)/4 ), rc );

  ASSERT2( ND_GET_LOOPBACK(0,p->Network_Config.NetFlags) == ND_LOOPBACK_TORUS_INTERNAL, ( "Loopback dimension %d mismatch: (actual) %lld vs %d (expected)\n", 0, ND_GET_LOOPBACK(0,p->Network_Config.NetFlags), ND_LOOPBACK_TORUS_INTERNAL ), rc );
  ASSERT2( ND_GET_LOOPBACK(1,p->Network_Config.NetFlags) == ND_LOOPBACK_NONE          , ( "Loopback dimension %d mismatch: (actual) %lld vs %d (expected)\n", 1, ND_GET_LOOPBACK(1,p->Network_Config.NetFlags), ND_LOOPBACK_NONE           ), rc );
  ASSERT2( ND_GET_LOOPBACK(2,p->Network_Config.NetFlags) == ND_LOOPBACK_SERDES,         ( "Loopback dimension %d mismatch: (actual) %lld vs %d (expected)\n", 2, ND_GET_LOOPBACK(0,p->Network_Config.NetFlags), ND_LOOPBACK_SERDES         ), rc );
  

  printf("(I) %s complete rc=%d\n", __func__, rc );

  return rc;
}


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
0, //22
0, //23
0, //24
0, //25
0, //26
0, //27
0, //28
0, //29
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
0, // 52
"PERS_ENABLE_GlobalInts      ", // FW_BIT(54)  //! Enable global interrupts embedded in Network
"PERS_ENABLE_SerDes          ", // FW_BIT(55)  //! Enable High-speed Serializer/Deserializer
"PERS_ENABLE_UPC             ", // FW_BIT(56)  //! Enable Universal Performance Counter Unit initializaiton
"PERS_ENABLE_EnvMon          ", // FW_BIT(57)  //! Enable Environmental Monitoring
"PERS_ENABLE_PCIe            ", // FW_BIT(58)  //! Enable PCIExpress
0, // 59
0, // 60
0, // 61
0, // 62
"PERS_ENABLE_DiagnosticsMode ", // FW_BIT(63)  //! Enable diagnostics mode

};

#define offsetof(st, m)  ((size_t) ( (char *)&((st *)(0))->m - (char *)0 ))

void showKernelConfig(Personality_t* p) {
  Personality_Kernel_t* k = &(p->Kernel_Config);
  int i;
  printf("[%04lX] Kernel_Config -> %p\n", offsetof(Personality_t,Kernel_Config), k);
  printf("[%04lX]   UCI         : %016llX\n", offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,UCI), k->UCI);
  printf("[%04lX]   NodeConfig  : %016llX\n", offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,NodeConfig), k->NodeConfig);
  for (i=0; i< 64; i++) {
      if ( (k->NodeConfig & (1ull<<(63-i))) != 0 ) {
	  printf("[****]     %s\n", NC_ENABLES[i]);
      }
  }
  printf("[%04lX]   TraceConfig : %016llX\n", offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,TraceConfig), k->TraceConfig);
  printf("[%04lX]   RASPolicy   : %08X\n", offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,RASPolicy), k->RASPolicy);
  printf("[%04lX]   FreqMHz     : 0x%08X (%d)\n", offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,FreqMHz), k->FreqMHz, k->FreqMHz);
  printf("[%04lX]   ClockStop   : 0x%016llX\n", offsetof(Personality_t,Kernel_Config) + offsetof(Personality_Kernel_t,ClockStop), k->ClockStop);
}

void showDDRConfig(Personality_t* p) {
    // TBD
}

void showNetConfig(Personality_t* p) {
  
  Personality_Networks_t* n = &(p->Network_Config);
  fw_uint64_t nf  = n->NetFlags;
  fw_uint64_t nf2 = n->NetFlags2;

  printf("[%04lX] Network_Config -> %p\n", offsetof(Personality_t,Network_Config), n);
  printf("[%04lX]   BlockID         : %08X\n",   offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,BlockID), n->BlockID);
  printf("[%04lX]   NetFlags        : %016llX\n", offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,NetFlags), nf);
  printf("[****]     I/O Node Link : %d\n", ND_GET_ENABLE_ION_LINK(nf));
  printf("[****]     Loopback      : (%lld,%lld,%lld,%lld,%lld,%lld) 0=None 1=Torus 2=SerDes 3=HSS\n",  
	 ND_GET_LOOPBACK(0,nf),ND_GET_LOOPBACK(1,nf),ND_GET_LOOPBACK(2,nf),
	 ND_GET_LOOPBACK(3,nf),ND_GET_LOOPBACK(4,nf),ND_GET_LOOPBACK(5,nf));
  printf ("[****]     Torus/Mesh    : (%lld,%lld,%lld,%lld,%lld) (0=Mesh, 1=Torus)\n", 
	  ND_GET_TORUS(0,nf), ND_GET_TORUS(1,nf), ND_GET_TORUS(2,nf), 
	  ND_GET_TORUS(3,nf), ND_GET_TORUS(4,nf) );
  printf ("[****]     RanHints      : (%lld,%lld,%lld,%lld,%lld)\n", 
	  ND_GET_TORUS_RAN_HINTS(0,nf), ND_GET_TORUS_RAN_HINTS(1,nf), ND_GET_TORUS_RAN_HINTS(2,nf), 
	  ND_GET_TORUS_RAN_HINTS(3,nf), ND_GET_TORUS_RAN_HINTS(4,nf) );
  printf ("[****]     Re/Inj Ration : %lld\n", ND_GET_SE_ARB_RE_INJ_RATIO(nf) );
  printf ("[****]     DetOrderMasks : (%02llX,%02llX,%02llX,%02llX,%02llX)\n", ND_GET_DET_ORDER_MASK(0,nf),ND_GET_DET_ORDER_MASK(1,nf),ND_GET_DET_ORDER_MASK(2,nf),ND_GET_DET_ORDER_MASK(3,nf),ND_GET_DET_ORDER_MASK(4,nf));
  printf("[%04lX]   NetFlags2       : %016llX\n", offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,NetFlags2), nf2);

  static char* DIMENSIONS[] = { "A", "B", "C", "D", "E", "IO" };
  static char* DIRECTIONS[] = { "-", "+" };

  printf("[****]       Torus Links Enabled : \n");
  {

    unsigned dimension, direction;

    for ( dimension = 0; dimension < 5; dimension++ ) {
      for ( direction = 0; direction <= 1; direction++ ) {
	if ( ND_GET_LINK_ENABLE(dimension, direction, p->Network_Config.NetFlags2 ) ) {
	  printf("[****]         %s%s\n", DIMENSIONS[dimension], DIRECTIONS[direction]);
	}
      }
    }
  }
  printf("[****]       SerDes Links Enabled :\n");
  {

    unsigned dimension, direction;

    for ( dimension = 0; dimension <= 5; dimension++ ) {
      for ( direction = 0; direction <= 1; direction++ ) {
	if ( SD_GET_LINK_ENABLE(dimension, direction, p->Network_Config.NetFlags2 ) ) {
	    printf("[****]         %s%s\n", DIMENSIONS[dimension], DIRECTIONS[direction]);
	}
      }
    }
  }
  printf("[****]       TIO       : %d\n", nf2 & SD_ENABLE_TIO ? 1 : 0 );
  printf("[****]       HalfSpeed : %d\n", nf2 & SD_ENABLE_HALFSPEED ? 1 : 0 );
  printf("[****]       Use Port 6: %d\n", nf2 & TI_USE_PORT6_FOR_IO ? 1 : 0 );
  printf("[****]       Use Port 7: %d\n", nf2 & TI_USE_PORT7_FOR_IO ? 1 : 0 );
  printf("[****]       Swaps     : (%lld,%lld,%lld,%lld,%lld)\n", TI_GET_TORUS_DIM_REVERSED(0,nf2),TI_GET_TORUS_DIM_REVERSED(1,nf2), TI_GET_TORUS_DIM_REVERSED(2,nf2), TI_GET_TORUS_DIM_REVERSED(3,nf2), TI_GET_TORUS_DIM_REVERSED(4,nf2));
  printf("[%04lX]   Torus           : (%d,%d,%d,%d,%d) / (%d,%d,%d,%d,%d)\n", offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,Anodes), 
	 n->Acoord, n->Bcoord, n->Ccoord, n->Dcoord, n->Ecoord,
	 n->Anodes, n->Bnodes, n->Cnodes, n->Dnodes, n->Enodes
	 );
  
  printf("[%04lX]   PrimmordialClassRoute\n", offsetof(Personality_t, Network_Config) + offsetof(Personality_Networks_t,PrimordialClassRoute));
  printf("[%04lX]     GlintUpPortInputs     : %04X\n", (uint64_t)(&(n->PrimordialClassRoute.GlobIntUpPortInputs)) - (uint64_t)p, n->PrimordialClassRoute.GlobIntUpPortInputs );
  printf("[%04lX]     GlintUpPortOutputs    : %04X\n", (uint64_t)(&(n->PrimordialClassRoute.GlobIntUpPortOutputs)) - (uint64_t)p, n->PrimordialClassRoute.GlobIntUpPortOutputs );
  printf("[%04lX]     CollTypeUpPortInputs  : %04X\n", (uint64_t)(&(n->PrimordialClassRoute.CollectiveTypeAndUpPortInputs)) - (uint64_t)p, n->PrimordialClassRoute.CollectiveTypeAndUpPortInputs );
  printf("[%04lX]     CollTypeUpPortOutputs : %04X\n", (uint64_t)(&(n->PrimordialClassRoute.CollectiveUpPortOutputs)) - (uint64_t)p, n->PrimordialClassRoute.CollectiveUpPortOutputs );
  
  printf("[%04lX]   MuFlags         : %016llX\n", offsetof(Personality_t,Network_Config) + offsetof(Personality_Networks_t,MuFlags), n->MuFlags);
}

void showEthConfig(Personality_t* p) {
 
  Personality_Ethernet_t* n = &(p->Ethernet_Config);
  printf("[%04lX] Ethernet_Config -> %p\n", offsetof(Personality_t,Ethernet_Config), n);
}

int showPersonality() {

  Personality_t* p = fwext_getPersonality();

  printf("        Personality -> %p\n", p);
  printf("[%04lX] CRC       : 0x%X\n", offsetof(Personality_t,CRC), p->CRC );
  printf("[%04lX] Version   : 0x%X\n", offsetof(Personality_t,Version), p->Version );
  printf("[%04lX] SizeWords : 0x%X\n", offsetof(Personality_t,PersonalitySizeWords), p->PersonalitySizeWords );

  showKernelConfig(p);
  showDDRConfig(p);
  showNetConfig(p);
  showEthConfig(p);


  printf("TESTINT_CONFIG1 : %016lX IO:%d Port6:%d Port7:%d Swaps:%02lX\n", DCRReadPriv( TESTINT_DCR(CONFIG1) ), TI_IsIoNode(), TI_UsePort6forIO(), TI_UsePort7forIO(), TI_GetDimensionSwaps() );
  return 0;
}

Test_t TESTS[] = {
    validateDefaultPersonality,
    validatePersonalizedFirmware,
    showPersonality
};

int test_main( void ) {

  if ( ProcessorID() != 0 ) {
    _Bg_Sleep_Forever();
  }

  int t;
  int rc = 0;
  
  for ( t = 0; t < sizeof(TESTS) / sizeof(TESTS[0]) && (rc == 0); t++ ) {
    rc |= TESTS[t]();
  }

  Terminate(rc);
}



