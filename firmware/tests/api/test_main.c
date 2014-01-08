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


#include <hwi/include/common/bgq_alignment.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/A2_core.h>

#include <firmware/include/mailbox.h>
#include <firmware/include/personality.h>
#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/BeDram_inlines.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

static int beQuiet = 0;

#define TRACE(x)
//#define TRACE(x) if (beQuiet==0) printf x
//#define TRACE(x) printf x

#define TRACE_ENTRY()  TRACE(("(>) %s        [%s:%d]\n", __func__, __FILE__, __LINE__));
#define TRACE_EXIT(rc) TRACE(("(<) %s rc=%d   [%s:%d]\n", __func__, (int)(rc), __FILE__, __LINE__));

#define INFO(x) if (beQuiet==0) printf x


//#define JUST_ONE_THREAD

Firmware_Interface_t* fwtFirmware = 0;
Personality_t         fwtPersonality;

volatile int abort_all_tests = 0;

typedef int (*Test_t)(Firmware_Interface_t* fw);

#define MAX_TESTS 256

volatile uint64_t atomic_counters[MAX_TESTS] ALIGN_QUADWORD = { 0, };

int printf( const char *fmt, ... ) {
  char buffer[256];
    va_list args;
    va_start( args, fmt );
    int len = vsprintf( buffer, fmt, args );
    fwtFirmware->putn(buffer,len);
    va_end(args);
    return len;
}

size_t strlen(const char* s) {
  size_t count = 0;
  while ( *(s++) != 0 )
    count++;
  return count;
}

int puts( const char* s ) {
  return fwtFirmware->putn(s, strlen(s));

}

void fw_memset( char* ptr, char value,  size_t size) {
  int i;
  for (i = 0; i < size; i++) {
    ptr[i] = value;
  }
}


void barrierAllThreads(unsigned test_number, unsigned barrier_count) {

  TRACE_ENTRY();

#if ! defined (JUST_ONE_THREAD)

  Fetch_and_Add((uint64_t *)&(atomic_counters[test_number]), 1);

  while ( ( atomic_counters[test_number] < barrier_count ) && (abort_all_tests == 0) )
    //_delay_cycles(100)
    ;

#endif

  TRACE_EXIT(0);

  return;
}

#define ASSERT( condition, message ) \
  if ( ! (condition) ) { printf( "[ERROR] : Assertion failed at  %s:%s():%d\n", __FILE__, __func__, __LINE__); \
    printf message ; \
    abort_all_tests = 1; \
    TRACE_EXIT(-1); \
    return -1;	     \
  }


    


/* ---------------------------------------------------------------------------
 * performDescriptorSanityCheck() test snoops around the firmware descriptor to see if things
 * look reasonable.
 * --------------------------------------------------------------------------- */

#define IS_BEDRAM_ADDRESS(p) ( ((void*)(p) >= (void*)0x3FFFFFC0000) && ( (void*)(p) < (void*)0x3FFFFFFFFFF) )

int performDescriptorSanityCheck( Firmware_Interface_t* fw) {
  
  TRACE_ENTRY();

  // Check the version
  ASSERT( fw->Version == BGQ_FIRMWARE_VERSION,  ("Firmware version check failed (%d,%d)\n", fw->Version, BGQ_FIRMWARE_VERSION) );

  // Check service addresses to ensure that they look like BeDRAM addresses:
  ASSERT( IS_BEDRAM_ADDRESS(fw->exit),           ("Invalid BeDRAM addres for service  exit           : %p\n", fw->exit           ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->terminate),      ("Invalid BeDRAM addres for service  terminate      : %p\n", fw->terminate      ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->getPersonality), ("Invalid BeDRAM addres for service  getPersonality : %p\n", fw->getPersonality ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->writeRASEvent),  ("Invalid BeDRAM addres for service  writeRASEvent  : %p\n", fw->writeRASEvent  ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->writeRASString), ("Invalid BeDRAM addres for service  writeRASString : %p\n", fw->writeRASString ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->putn),           ("Invalid BeDRAM addres for service  putn           : %p\n", fw->putn           ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->isIoNode),       ("Invalid BeDRAM addres for service  isIoNode       : %p\n", fw->isIoNode       ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->takeCPU),        ("Invalid BeDRAM addres for service  takeCPU        : %p\n", fw->takeCPU        ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->returnCPU),      ("Invalid BeDRAM addres for service  returnCPU      : %p\n", fw->returnCPU      ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->termCheck),      ("Invalid BeDRAM addres for service  termCheck      : %p\n", fw->termCheck      ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->installInterruptVector), ("Invalid BeDRAM addres for service  installInterruptVector      : %p\n", fw->installInterruptVector      ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->getDomainDescriptor),    ("Invalid BeDRAM addres for service  getDomainDescriptor         : %p\n", fw->getDomainDescriptor         ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->getDomainDescriptor),    ("Invalid BeDRAM addres for service  getDomainDescriptor         : %p\n", fw->getDomainDescriptor         ) ) ;
  ASSERT( IS_BEDRAM_ADDRESS(fw->barrier),                ("Invalid BeDRAM addres for service  barrier                     : %p\n", fw->barrier                     ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->get    ),                ("Invalid BeDRAM addres for service  get                         : %p\n", fw->get                         ) );
  ASSERT( IS_BEDRAM_ADDRESS(fw->sendBlockStatus  ),      ("Invalid BeDRAM addres for service  sendBlockStatus             : %p\n", fw->sendBlockStatus             ) );

  TRACE_EXIT(0);
  return 0;
}


/* -------------------------------------------------------------------------
 * The getPersonalityTest() performs various tests against said service
 * ensuring that it meets its API contract and doesn't do other devoius
 * things.
 * ------------------------------------------------------------------------- */

int getPersonalityTest( Firmware_Interface_t* fw) {

  TRACE_ENTRY();
  int rc;

  if ( ProcessorID() == 0 ) {
  rc = fw->getPersonality(&fwtPersonality, sizeof(Personality_t)-1 );
  ASSERT ( rc != 0, ( "Illegal personality size test failed. rc=%d \n", (int)rc ) );

  rc = fw->getPersonality(&fwtPersonality, sizeof(Personality_t)+1 );
  ASSERT ( rc != 0, ( "Illegal personality size test failed. rc=%d \n", (int)rc ) );

  rc = fw->getPersonality(&fwtPersonality, sizeof(Personality_t) );

  ASSERT ( rc == 0, ( "Illegal personality size test failed. rc=%d \n", (int)rc ) );
  ASSERT( fwtPersonality.Version == PERSONALITY_VERSION, ( "Version mismatch %d vs %d\n", fwtPersonality.Version, PERSONALITY_VERSION ) );
  ASSERT( fwtPersonality.PersonalitySizeWords == sizeof(Personality_t)/4, ("Personality.PersonalitySizeWords error (%d)\n", fwtPersonality.PersonalitySizeWords ) );
  ASSERT( fwtPersonality.Kernel_Config.FreqMHz == DEFAULT_FREQ_MHZ, ( "Frequency mismatch (%d)\n", fwtPersonality.Kernel_Config.FreqMHz ) );

  unsigned n;
  char* original = (char*)fw->deprecated.personalityPtr;
  char* copy = (char*)&fwtPersonality;

  for ( n = 0 ; n < sizeof(Personality_t); n++ ) {
    ASSERT( original[n] == copy[n], ( "Byte mismatch at offset %d (0x%X) of the personality : %02X vs. %02X\n", n, n, original[n], copy[n]));
  }

  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * This next clever little test ensures that getPersonality() *ONLY* writes 
   * into the personality structure itself.  This is done by carving out a buffer,
   * filling it with known (sentinel) values, and placing a personality structure
   * in the middle.  After calling getPersonality(), the bytes that bookend the
   * structure are checked to ensure they still contain the known values.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

  char buffer[ sizeof(Personality_t) + 256 ];
  char* p;

  fw_memset(buffer, 0xEE, sizeof(buffer) ); // fill up the buffer with known values
  for (p = buffer + 1; (unsigned long)p & 0xF; p++); // advance to the next quadword bondary
  
  rc = fw->getPersonality((Personality_t*)p, sizeof(Personality_t) );
  ASSERT( rc == 0, ( "Unexpected error from getPersonality() rc=%d\n", (int)rc ) );
  ASSERT( p[-1] == 0xEE, ( "Clobbered prefix byte (%x)\n", (int)p[-1] ) );
  ASSERT( p[sizeof(Personality_t)] == 0xEE, ( "Clobbered trailing byte (%x)\n", (int)p[sizeof(Personality_t)]));

  }

  TRACE_EXIT(0);
  return 0;
}


/* -------------------------------------------------------------------------
 * The showPersonality() "test" simply outputs a formatted version of
 * the personality structure.
 * ------------------------------------------------------------------------- */


/*
 * Code Generated by com.ibm.bluegene.tools.bgq.etc.ShowPersonality
 * Location: null
 */

const char* PERS_ENABLES[] = {
    /* 0*/ "PERS_ENABLE_MMU",
    /* 1*/ "PERS_ENABLE_IsIoNode",
    /* 2*/ "PERS_ENABLE_TakeCPU",
    /* 3*/ "PERS_ENABLE_MU",
    /* 4*/ "PERS_ENABLE_ND",
    /* 5*/ "PERS_ENABLE_TimeStamps",
    /* 6*/ "PERS_ENABLE_BeDRAM",
    /* 7*/ "PERS_ENABLE_ClockStop",
    /* 8*/ "PERS_ENABLE_DrArbiter",
    /* 9*/ "PERS_ENABLE_DevBus",
    /* 10*/ "PERS_ENABLE_L1P",
    /* 11*/ "PERS_ENABLE_L2",
    /* 12*/ "PERS_ENABLE_MSGC",
    /* 13*/ "PERS_ENABLE_TestInt",
    /* 14*/ "PERS_ENABLE_NodeRepro",
    /* 15*/ "PERS_ENABLE_PartitionRepro",
    /* 16*/ "PERS_ENABLE_DD1_WorkArounds",
    /* 17*/ "PERS_ENABLE_A2_Errata",
    /* 18*/ "PERS_ENABLE_A2_IU_LLB",
    /* 19*/ "PERS_ENABLE_XU_LLB",
    /* 20*/ "PERS_ENABLE_DDR_Calibration",
    /* 21*/ "PERS_ENABLE_DDRFastInit",
    /* 22*/ 0,
    /* 23*/ 0,
    /* 24*/ 0,
    /* 25*/ 0,
    /* 26*/ 0,
    /* 27*/ 0,
    /* 28*/ 0,
    /* 29*/ 0,
    /* 30*/ 0,
    /* 31*/ 0,
    /* 32*/ 0,
    /* 33*/ 0,
    /* 34*/ 0,
    /* 35*/ 0,
    /* 36*/ "PERS_ENABLE_AppPreload",
    /* 37*/ "PERS_ENABLE_IOServices",
    /* 38*/ "PERS_EANBLE_SpecCapDDR",
    /* 39*/ "PERS_ENABLE_L2Only",
    /* 40*/ "PERS_ENABLE_FPGA",
    /* 41*/ "PERS_ENABLE_DDRInit",
    /* 42*/ "PERS_ENABLE_Mailbox",
    /* 43*/ "PERS_ENABLE_Simulation",
    /* 44*/ "PERS_ENABLE_Mambo",
    /* 45*/ "PERS_ENABLE_StandAlone",
    /* 46*/ "PERS_ENABLE_JTagLoader",
    /* 47*/ "PERS_ENABLE_FPU",
    /* 48*/ "PERS_ENABLE_L2Counters",
    /* 49*/ "PERS_ENABLE_Wakeup",
    /* 50*/ "PERS_ENABLE_BIC",
    /* 51*/ "PERS_ENABLE_DDR",
    /* 52*/ 0,
    /* 53*/ 0,
    /* 54*/ "PERS_ENABLE_GlobalInts",
    /* 55*/ "PERS_ENABLE_SerDes",
    /* 56*/ "PERS_ENABLE_UPC",
    /* 57*/ "PERS_ENABLE_EnvMon",
    /* 58*/ "PERS_ENABLE_PCIe",
    /* 59*/ 0,
    /* 60*/ 0,
    /* 61*/ 0,
    /* 62*/ 0,
    /* 63*/ "PERS_ENABLE_DiagnosticsMode",
};


const char* TRACE_ENABLES[] = {
    /* 0*/ 0,
    /* 1*/ 0,
    /* 2*/ 0,
    /* 3*/ 0,
    /* 4*/ 0,
    /* 5*/ 0,
    /* 6*/ 0,
    /* 7*/ 0,
    /* 8*/ 0,
    /* 9*/ 0,
    /* 10*/ 0,
    /* 11*/ 0,
    /* 12*/ 0,
    /* 13*/ 0,
    /* 14*/ 0,
    /* 15*/ 0,
    /* 16*/ 0,
    /* 17*/ 0,
    /* 18*/ 0,
    /* 19*/ 0,
    /* 20*/ 0,
    /* 21*/ 0,
    /* 22*/ 0,
    /* 23*/ 0,
    /* 24*/ 0,
    /* 25*/ 0,
    /* 26*/ 0,
    /* 27*/ 0,
    /* 28*/ 0,
    /* 29*/ 0,
    /* 30*/ 0,
    /* 31*/ 0,
    /* 32*/ 0,
    /* 33*/ 0,
    /* 34*/ "TRACE_PCIe",
    /* 35*/ 0,
    /* 36*/ "TRACE_Entry",
    /* 37*/ 0,
    /* 38*/ 0,
    /* 39*/ 0,
    /* 40*/ "TRACE_MChk",
    /* 41*/ "TRACE_SysCall",
    /* 42*/ "TRACE_VMM",
    /* 43*/ "TRACE_Debug",
    /* 44*/ 0,
    /* 45*/ 0,
    /* 46*/ 0,
    /* 47*/ "TRACE_SerDes",
    /* 48*/ 0,
    /* 49*/ 0,
    /* 50*/ 0,
    /* 51*/ 0,
    /* 52*/ "TRACE_Process",
    /* 53*/ "TRACE_Exit_Sum",
    /* 54*/ "TRACE_Sched",
    /* 55*/ 0,
    /* 56*/ 0,
    /* 57*/ "TRACE_Futex",
    /* 58*/ "TRACE_MemAlloc",
    /* 59*/ 0,
    /* 60*/ 0,
    /* 61*/ 0,
    /* 62*/ 0,
    /* 63*/ "TRACE_Verbose",
};

#define IPVxFORMAT  "%d.%d.%d.%d"
#define IPVxDATA(a) a.octet[12], a.octet[13], a.octet[14], a.octet[15]

char* bitString( unsigned value, unsigned length, char* buffer) {
  unsigned i;
  for (i = 0; i < length; i++) {
    unsigned mask = 1 << (length-i-1);
    buffer[i] = (value & mask) ? '1' : '0';
  }
  buffer[length] = 0;
  return buffer;
}

char* longBitString( uint64_t value, unsigned length, char* buffer) {
  
  int n = 0;
  int i;
  
  for (i = 0; i < length; i++) {
    if (i > 0) {
      if ( (i % 8) == 0 ) {
	buffer[n++] = ':';
      }
      else if ( (i % 4) == 0 ) {
	buffer[n++] = '-';
      }
    }
    uint64_t mask = 1ul << (length-i-1);
    buffer[n++] = (value & mask) ? '1' : '0';
  }

  buffer[n] = 0;
  return buffer;
}

int showPersonality( Firmware_Interface_t* fw ) {

  TRACE_ENTRY();


  if ( ProcessorID() == 0 ) {

    Personality_t p;
    int n;
    char buffer[128];

    fw->getPersonality(&p, sizeof(p));

    printf("Personality            : size=0x%lX (%d)\n", sizeof(Personality_t), (int)sizeof(Personality_t));
    printf("  CRC                  : 0x%04X\n", p.CRC );
    printf("  Version              : 0x%02X\n", p.Version );
    printf("  PersonalitySizeWords : 0x%02x (%d)\n", p.PersonalitySizeWords, p.PersonalitySizeWords);
    printf("  Kernel_Config        : size=0x%lX offset=0x%lX\n", sizeof(Personality_Kernel_t), offsetof(Personality_t, Kernel_Config));
    printf("    UCI                : 0x%016llX\n", p.Kernel_Config.UCI);
    printf("    NodeConfig         : 0x%016llX = %s\n", p.Kernel_Config.NodeConfig, longBitString(p.Kernel_Config.NodeConfig, 64, buffer));
    
    for (n = 0; n < 64; n++) {
      uint64_t mask = 1ul << (63-n);
      if ( ( p.Kernel_Config.NodeConfig & mask ) != 0 ) {
	printf("        %s = 1\n", PERS_ENABLES[n]);
      }
    }
    
    printf("    TraceConfig        : 0x%016llX\n", p.Kernel_Config.TraceConfig);
 
    for (n = 0; n < 64; n++) {
      uint64_t mask = 1ul << (63-n);
      if ( ( p.Kernel_Config.TraceConfig & mask ) != 0 ) {
	printf("        %s = 1\n", TRACE_ENABLES[n]);
      }
    }
    printf("    RASPolicy          : 0x%08X\n",  p.Kernel_Config.RASPolicy);
    printf("    FreqMHz            : 0x%08X (%d)\n", p.Kernel_Config.FreqMHz, p.Kernel_Config.FreqMHz);
    printf("  DDR_Config           : size=0x%lX offset=0x%lX\n", sizeof(Personality_DDR_t), offsetof(Personality_t,DDR_Config));
    printf("    DDRFlags           : 0x%08X\n", p.DDR_Config.DDRFlags);
    printf("    DDRSizeMB          : 0x%08X\n", p.DDR_Config.DDRSizeMB);
    printf("  Networks_Config      : size=0x%lX offset=0x%lX\n", sizeof(Personality_Networks_t), offsetof(Personality_t,Network_Config));
    printf("    BlockID            : 0x%08X\n", p.Network_Config.BlockID);
    printf("    NetFlags           : 0x%016llX\n", p.Network_Config.NetFlags);

    uint64_t nf = p.Network_Config.NetFlags;

    printf("      ION Link         : %d\n", ND_GET_ENABLE_ION_LINK(nf));
    printf("      Loopback         : (%lld,%lld,%lld,%lld,%lld,%lld)  NOTE: %d=NONE %d=TORUS %d=SERDES\n", 
	   ND_GET_LOOPBACK(0,nf), ND_GET_LOOPBACK(1,nf), ND_GET_LOOPBACK(2,nf), 
	   ND_GET_LOOPBACK(3,nf), ND_GET_LOOPBACK(4,nf), ND_GET_LOOPBACK(5,nf), 
	   ND_LOOPBACK_NONE, ND_LOOPBACK_TORUS_INTERNAL, ND_LOOPBACK_SERDES );
    printf("      Torus/Mesh       : (%lld,%lld,%lld,%lld,%lld) all:%s (0=Mesh, 1=Torus)\n", ND_GET_TORUS(0,nf), ND_GET_TORUS(1,nf), ND_GET_TORUS(2,nf), ND_GET_TORUS(3,nf), ND_GET_TORUS(4,nf), bitString( ND_GET_ALL_TORUS(nf), 5, buffer ) );
    printf("      TorusRanHints    : (%lld,%lld,%lld,%lld,%lld) all:%s\n", ND_GET_TORUS_RAN_HINTS(0,nf), ND_GET_TORUS_RAN_HINTS(1,nf), ND_GET_TORUS_RAN_HINTS(2,nf), ND_GET_TORUS_RAN_HINTS(3,nf), ND_GET_TORUS_RAN_HINTS(4,nf), bitString( ND_GET_ALL_TORUS_RAN_HINTS(nf), 5, buffer) );
    printf("      Rcvr/Inj Ratio   : 0x%01llX\n", ND_GET_SE_ARB_RE_INJ_RATIO(nf) );
    printf("      DetOrderMasks 0  : %s\n", bitString( ND_GET_DET_ORDER_MASK(0,nf), 5, buffer ) );
    printf("      DetOrderMasks 1  : %s\n", bitString( ND_GET_DET_ORDER_MASK(1,nf), 5, buffer ) );
    printf("      DetOrderMasks 2  : %s\n", bitString( ND_GET_DET_ORDER_MASK(2,nf), 5, buffer ) );
    printf("      DetOrderMasks 3  : %s\n", bitString( ND_GET_DET_ORDER_MASK(3,nf), 5, buffer ) );
    printf("      DetOrderMasks 4  : %s\n", bitString( ND_GET_DET_ORDER_MASK(4,nf), 5, buffer ) );

    printf("    Coords             : (%2d,%2d,%2d,%2d,%2d)\n", p.Network_Config.Acoord, p.Network_Config.Bcoord, p.Network_Config.Ccoord, p.Network_Config.Dcoord , p.Network_Config.Ecoord );
    printf("    Size               : (%2d,%2d,%2d,%2d,%2d)\n", p.Network_Config.Anodes, p.Network_Config.Bnodes, p.Network_Config.Cnodes, p.Network_Config.Dnodes , p.Network_Config.Enodes );
    printf("  Ethernet_Config      : size=0x%lX offset=0x%lX\n", sizeof(Personality_Ethernet_t), offsetof(Personality_t,Ethernet_Config));


#if 0
    printf("PORT6           : %016lX = %s\n", ND_ENABLE_PORT6, longBitString( ND_ENABLE_PORT6, 64, buffer ) );
    printf("PORT7           : %s\n", longBitString( ND_ENABLE_PORT7, 64, buffer ) );
    printf("DIM_A_LOOPBACK  : %s\n", longBitString( ND_DIM_A_LOOPBACK(-1), 64, buffer ) );
    printf("DIM(4)_LOOPBACK : %s\n", longBitString( ND_DIM_LOOPBACK(5,-1), 64, buffer ) );
#endif

  }

  TRACE_EXIT(0);
  return 0;
}


#define OUTBOX_SIZE (16 * 1024)
char bigString[OUTBOX_SIZE] = { [0 ... (16*1024-1)] = '*'   };

/* ---------------------------------------------------------------------------
 * putnTest() validates the API of the putn service including boundary
 * conditions.
 * --------------------------------------------------------------------------- */

int putnTest( Firmware_Interface_t* fw) {

  TRACE_ENTRY();

  int rc;
  char* str = "This is a test of the EBS.";

  if ( ProcessorID() != 0 ) {
    TRACE_EXIT(0);
    return 0;
  }

  /* --------------------------------------
   * putn() of length zero should return 0
   * -------------------------------------- */

  rc = fw->putn( (const char*)0, 0 );
  ASSERT( (rc == 0), ("Unexpected rc (%d) from putn(NULL,0)\n", rc));

  /* --------------------------------------
   * Even if the pointer is completely
   * bogus:
   * -------------------------------------- */

  rc = fw->putn( (const char*)0x123456789abcdeful, 0 );
  ASSERT( (rc == 0), ("Unexpected rc (%d) from putn(BOGUS,0)\n", rc));

  /* -----------------------------------------
   * How about something simple:
   * ----------------------------------------- */

  rc = fw->putn( "hello", 5 );

  /* -----------------------------------------
   * Really big operations will fail.  Note 
   * that this only guarantees to fail on
   * the mailbox path:
   * ----------------------------------------- */
  if ( fwtPersonality.Kernel_Config.NodeConfig & PERS_ENABLE_Mailbox ) {
    rc = fw->putn( (const char*)0, OUTBOX_SIZE - sizeof(MailBoxHeader_t) + 1 );
    ASSERT( (rc == FW_TOO_BIG), ("Unexpected rc (%d) from putn(0,%lu) ... should be FW_TOO_BIG (%d)\n", rc, OUTBOX_SIZE - sizeof(MailBoxHeader_t) + 1, FW_TOO_BIG));
  }

  
  /* -----------------------------------------
   * putn() of one characater should return 1
   * ----------------------------------------- */

  rc = fw->putn( str, 1 );
  ASSERT( (rc == 1), ("Unexpected rc (%d) from putn(\"%s\",1)\n", rc, str));
  fw->putn("\n",1); // get the new line in there

  TRACE_EXIT(0);
  return 0;
}




int writeRASEventTest( Firmware_Interface_t* fw ) {

  TRACE_ENTRY();

  if ( ProcessorID() != 0 ) {
    TRACE_EXIT(0);
    return 0;
  }

  {

    int rc;
    uint64_t details[4];
    details[0] = 0x1010101010101010;
    details[1] = 0x1111111111111111;
    details[2] = 0x2222222222222222;
    details[3] = 0x3333333333333333;
    rc = fw->writeRASEvent( 0x12345678, sizeof(details)/sizeof(details[0]), (fw_uint64_t *)details );
    ASSERT( rc == 0, ( "Unexpected return code from writeRASEvent: rc=%d (expected zero)\n", rc) );
  }
  
  /* +--------------------------------------------------------------+
   * | NOTE: the big RAS event test currently only works whwen we   |
   * |       are in mailbox mode (UART code isnt this smart).       |
   * |       Rather than fixing the non-strategic UART path, we'll  |
   * |       just skip the test if we aren't in mailbox mode.       |
   * +--------------------------------------------------------------+
   */

  if ( fwtPersonality.Kernel_Config.NodeConfig & PERS_ENABLE_Mailbox ) {

    uint16_t tooManyDetails = (OUTBOX_SIZE - sizeof(MailBoxHeader_t) - sizeof(MailBoxPayload_RAS_t)) / sizeof(uint64_t) + 1;
    int rc;
    uint64_t details[1];

    rc = fw->writeRASEvent(0, tooManyDetails, (fw_uint64_t *)details);
    ASSERT( rc < 0, ( "Unexpected return code from writeRASEvent: rc=%d (expected negative)\n", rc) );
  }
  else {
    INFO(("(I) Big RAS event test skipped in UART mode [%s():%s:%d]\n", __FILE__, __func__, __LINE__));
  }

  TRACE_EXIT(0);

  return 0;
}

int writeRASStringTest( Firmware_Interface_t* fw ) {

  TRACE_ENTRY();

  if ( ProcessorID() != 0 ) {
    TRACE_EXIT(0);
    return 0;
  }

  int rc = fw->writeRASString( 0x12345678, "Hello RAS world!");
  ASSERT( rc == 0, ( "Unexpected return code from writeRASString: rc=%d (expected zero)\n", rc) );

#if 0
  /* +--------------------------------------------------------------+
   * | NOTE: the big RAS event test currently only works whwen we   |
   * |       are in mailbox mode (UART code isnt this smart).       |
   * |       Rather than fixing the non-strategic UART path, we'll  |
   * |       just skip the test if we aren't in mailbox mode.       |
   * +--------------------------------------------------------------+
   */

  if ( fwtPersonality.Kernel_Config.NodeConfig & PERS_ENABLE_Mailbox ) {

    uint16_t tooManyDetails = (OUTBOX_SIZE - sizeof(MailBoxHeader_t) - sizeof(MailBoxPayload_RAS_t)) / sizeof(uint64_t) + 1;
    int rc;
    uint64_t details[1];

    rc = fw->writeRASEvent(0, tooManyDetails, details);
    ASSERT( rc < 0, ( "Unexpected return code from writeRASEvent: rc=%d (expected negative)\n", rc) );
  }
  else {
    INFO(("(I) Big RAS event test skipped in UART mode [%s():%s:%d]\n", __FILE__, __func__, __LINE__));
  }
#endif

  TRACE_EXIT(0);

  return 0;
}

int getDomainDescriptor( Firmware_Interface_t* fw ) {

    if ( ProcessorID() > 0 ) 
	return 0;

    Firmware_DomainDescriptor_t dd;
    int rc;
    int isMambo =  ( fwtPersonality.Kernel_Config.NodeConfig & PERS_ENABLE_Mambo ) ? 1 : 0;

    printf("calling getDomainDescriptor (%p)\n", fw);
    rc = fw->getDomainDescriptor(&dd);
    printf("rc=%d\n", rc);

    ASSERT( rc == FW_OK, ( "Unexpected return code from getDomainDescriptor: rc=%d (expected %d)\n", rc, FW_OK) );

    printf("Domain Descriptor:\n");
    printf("  core mask : %X\n", dd.coreMask );
    printf("  DDR origin  : %llX\n", dd.ddrOrigin );
    printf("  DDR end     : %llX (%dMB)\n", dd.ddrEnd, (int)((dd.ddrEnd+1)/1024/1024) );
    printf("  entry point : 0x%llX\n", dd.entryPoint );

    fw_uint64_t ddrEnd = (fwtPersonality.DDR_Config.DDRSizeMB * 1024ull * 1024ull) - 1; 

    if ( isMambo ) {
      // Mambo is single core by default
      ASSERT( dd.coreMask == 0x1, ("coreMask miscompare: (actual) %X vs. %X (expected)\n", dd.coreMask, 0x1 ) );
    }
    else {
      ASSERT( dd.coreMask == 0x1FFFF, ("coreMask miscompare: (actual) %X vs. %X (expected)\n", dd.coreMask, 0x1FFFF ) );
    }

    ASSERT( dd.ddrOrigin == 0, ("ddrOrigin miscompare: (actual) %llX vs. %llX (expected)\n", dd.ddrOrigin, 0x0ull ) );
    ASSERT( dd.ddrEnd == ddrEnd, ("ddrEnd miscompare: (actual) %llX vs. %llX (expected) [EXPECTED VALUE ASSUMES DD1!!!] \n", dd.ddrEnd, ddrEnd ) );
    ASSERT( dd.entryPoint == 0x10, ("entryPoint miscompare: (actual) %llX vs. %llX (expected)\n", dd.entryPoint, 0x10ull ) );

    return 0;
}

int barrierTest( Firmware_Interface_t* fw ) {

    if (ProcessorID() == 0) {
	
	int rc;

	printf("(I) Invoking barrier (%p) ...\n", fw->barrier);
 	rc = fw->barrier(0);
	printf("(I)   alive!  rc=%d\n", rc);
	
	ASSERT( rc == FW_OK, ( "Unexpected return code from barrier: %d\n", rc ) );
    }

    return 0;
}

int crashTest( Firmware_Interface_t* fw ) {


  TRACE_ENTRY();

  if ( ProcessorID() != 1 )
    return 0;


#ifdef TEST_CRASH
  fw->crash(-2);
  ASSERT( 0, ("Should never get here!\n") );

#else
  INFO(("(I) test of Firmware crash service is being skipped [%s():%s:%d]\n", __func__, __FILE__, __LINE__));
#endif
  
  TRACE_EXIT(0);
  return 0;

}


int sendBlockStatusTest( Firmware_Interface_t* fw ) {

    if ( ProcessorID() == 0 ) {

	int rc;

	printf("(I) Sending block booted (%X) ...\n", JMB_BLOCKSTATE_INITIALIZED);
 	rc = fw->sendBlockStatus( JMB_BLOCKSTATE_INITIALIZED, 0, 0 );
	ASSERT( rc == FW_OK, ( "Unxpected return code from sendBlockStatus: %d\n", rc ));

	fw_uint64_t blockid = 1234;

	rc = fw->sendBlockStatus( JMB_BLOCKSTATE_IO_LINK_CLOSED, 1, &blockid );
	ASSERT( rc == FW_OK, ( "Unxpected return code from sendBlockStatus: %d\n", rc ));
	
    }

    return 0;
}


typedef struct _TestList_t {
  Test_t test;
  const char* name;
} TestList_t;

TestList_t ALL_TESTS[] = {
  { performDescriptorSanityCheck,        "performDescriptorSanityCheck" },
  { getPersonalityTest, "getPersonalityTest" },
  { showPersonality,    "showPersonality" },
  { putnTest,           "putnTest" },
  { getDomainDescriptor, "getDomainDescriptorTest" },
  { barrierTest,         "barrierTest" },
  { sendBlockStatusTest, "sendBlockStatusTest" },
  //DELETEME { putTest,            "putTest" },
  //DELETEME { putsTest,           "putsTest" },
  //DELETEME { printfTest,         "printfTest" },
  //{ writeRASEventTest,  "writeRASEventTest" },
  //{ writeRASStringTest,  "writeRASStringTest" },
  //{ crashTest,          "crashTest" },
};




int run_tests() {

  TRACE(("(>) %s [%s:%d]\n", __func__, __FILE__, __LINE__));

  int t;
  int rc = 0;

  for (t = 0; (t < sizeof(ALL_TESTS) / sizeof(ALL_TESTS[0])) && (rc == 0); t++) {

    if ( ALL_TESTS[t].test == 0 ) 
      break;

    barrierAllThreads( t, 4 ); // 4 threads for now

    if (abort_all_tests != 0) {
      return -1;
    }


    if ( ProcessorID() == 0 ) {
      INFO(("--- Test %d : %s ---\n", t, ALL_TESTS[t].name ));
    }

    rc |= ALL_TESTS[t].test( fwtFirmware );
  }

  barrierAllThreads(t,4);
  return rc;
}

int test_main( void ) {
  
  TRACE(("(>) %s [%s:%d]\n", __func__, __FILE__, __LINE__));

#if defined (JUST_ONE_THREAD)
  if ( ProcessorID() != 0 ) {
    printf(">>>> DISABLING MYSELF (%d)\n", ProcessorID());
    mtspr(SPRN_TENC,1 << ProcessorID() );
  }
#endif

  int rc = 0;

  rc = run_tests();

  TRACE(("(<) %s rc=%d [%s:%d]\n", __func__, rc, __FILE__, __LINE__));
  
  rc ? fwtFirmware->terminate(rc) : fwtFirmware->exit(rc);
    
  return 0;

}

#define STACK_SIZE (32*1024)

char Stacks[17][4][STACK_SIZE] ALIGN_L2_CACHE /* = { 0, } */ ;


__C_LINKAGE void __NORETURN crash( int status ) {
  printf("I am here unexpetedly -> %s:%s:%d\n", __func__, __FILE__, __LINE__);
  fwtFirmware->terminate(-1);
  while (1);
#if 0
   do {
      Kernel_Crash(status);
      }
      while(1);
#endif
}



//
// The entry point for C code (via start.S via firmware)
//
//  Note: (1) We're still running on the Firmware's 1KB stack, be careful.
//        (2) Firmware owns interrupt vectors (IVPR) until we're ready to handle interrupts.
//

__C_LINKAGE void __NORETURN _test_entry( Firmware_Interface_t *fw_interface ) {

    int threadID       = ProcessorID(); // 0..67
    int processorIndex = ProcessorCoreID(); // 0..16
    int threadIndex    = ProcessorThreadID();    // 0..3

    if ( 0 == threadID ) {
      fwtFirmware = fw_interface;
      beQuiet = (fw_interface->deprecated.personalityPtr->Kernel_Config.NodeConfig & PERS_ENABLE_Simulation) ? 1 : 0;
      BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_KERNEL_INIT);
    }
    else { 

      // block until primordial thread is initialized

      while ( ! BeDRAM_Read(BeDRAM_LOCKNUM_KERNEL_INIT) )
	/* spin */
	;
    }

    // Relaunch threads on their own stack

    do {
      
      OPD_Entry_t *opd_main = (OPD_Entry_t *)test_main;
      OPD_Entry_t *opd_exit = (OPD_Entry_t *)crash;

      size_t pStk = (size_t)((size_t)&(Stacks[processorIndex][threadIndex][STACK_SIZE-64]));

      mtspr( SPRN_SRR0_IP,   opd_main->Entry_Addr );
      mtspr( SPRN_SRR1_MSR,  MSR_KERNEL );
      isync();
      
      asm volatile (
		     "mtlr  %2;"
                     "li    0,0;"
                     "mr    1,%0;"
                     "mr    2,%1;"
                     "li    3,0;"
                     "rfi;"
                     "nop;"
                     :
                     : "r" (pStk),
                     "r" (opd_main->TOC_Addr),
                     "r" (opd_exit->Entry_Addr)
                     : "r0", "r1", "r2", "r3", "lr", "cc", "memory" );
    } while (1);
}


