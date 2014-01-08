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

#include <hwi/include/common/bgq_bitnumbers.h>
#include <hwi/include/common/uci.h>
#include <firmware/include/personality.h>
#include <firmware/include/mailbox.h>
#include "svc_host.h"

#include <hwi/include/bqc/A2_core.h>
#include "firmware/src/Firmware_internals.h" // This is OK since we aren't reaching across components

extern char *basename( const char *path );

extern int svc_getHexLong(char* source, unsigned long* value);

char svc_host_error_msg[SVC_MAX_ERROR_MESSAGE_LEN] = "";
char* svc_host_error_message() { return svc_host_error_msg; }


/* ******************************************************************
 *
 * Assorted global variables (filenames, switches, etc.)
 *
 * ****************************************************************** */

int verbose = 0;
int debug   = 0;

char filename_bl_elf[ SVC_MAX_FILENAME_LEN ]     = "";
char filename_bl_patched[ SVC_MAX_FILENAME_LEN ] = "";
char filename_env[ SVC_MAX_FILENAME_LEN ]        = "";
char filename_example[ SVC_MAX_FILENAME_LEN ]    = "";
char interpret_config[ SVC_MAX_FILENAME_LEN ]    = "";

//char filename_ddrreg[SVC_MAX_FILENAME_LEN ]      = "";
Svc_Firmware_Info_T svc_firmware;

#define SVC_MAX_OPTION_LENGTH 256
#define SVC_MAX_OPTIONS       512

char svc_options[SVC_MAX_OPTIONS][SVC_MAX_OPTION_LENGTH];
int  svc_num_options = 0;

#if 0

_DDR_Chip ddr_chips[] = {
            { "256MBx8",  0x00 },
            { "512MBx8",  0x01 },
            { "1GBx8",    0x02 },
            { "2GBx8",    0x03 },
            { "4GBx8",    0x04 },
            { "256MBx16", 0x08 },
            { "512MBx16", 0x09 },
            { "1GBx16",   0x0A },
            { "2GBx16",   0x0B },
            { "4GBx16",   0x0C },
            { NULL,       0    } };

#endif 


RAS_Verbosity_T ras_verbosity[] = {
    { "Minimal", PERS_RASPOLICY_MINIMAL },
    { "Normal",  PERS_RASPOLICY_NORMAL },
    { "Verbose", PERS_RASPOLICY_VERBOSE },
    { "Extreme", PERS_RASPOLICY_EXTREME }
};

Personality_t Pers_Template = PERSONALITY_DEFAULT();

const char* DDR_PWR_DWN[] = {
    "off",  // 0
    "fast", // 1
    "slow", // 2
};


char svc_Herald[] = "BlueGene/Q svc_host - Version 1.0";

char *EN = "(Enabled)";
char *DIS = "(Disabled)";

Svc_Control_T NodeConfigControls[] = {
  { "Mailbox",         PERS_ENABLE_Mailbox,          "Enable JTAG Mailbox support"                      },
  { "Simulation",      PERS_ENABLE_Simulation,       "Support VHDL Simulation"                          },
  { "FPGA",            PERS_ENABLE_FPGA,             "Running under FPGA"                               },
  { "Mambo",           PERS_ENABLE_Mambo,            "Running under Mambo?"                             },
  { "JTagConsole",     PERS_ENABLE_JTagConsole,      "Route kernel console to JTAG."                    },
  { "JTagLoader",      PERS_ENABLE_JTagLoader,       "JTag Host Kernel Loader Support"                  },
  { "FPU",             PERS_ENABLE_FPU,              "Enable QPX"                                       },
  { "L2Counters",      PERS_ENABLE_L2Counters,       "Enable L2 atomics"                                },
  { "Wakeup",          PERS_ENABLE_Wakeup,           "Enable Sleep-on-Pin unit"                         },
  { "BIC",             PERS_ENABLE_BIC,              "BG/P Interrupt Controller"                        },
  { "DDR",             PERS_ENABLE_DDR,              "Enable DDR Memory usage"                          },
  { "DDRINIT",         PERS_ENABLE_DDRINIT,          "DDR Controllers initialized (not FUSION-DDR)"     },
  { "DDRDynRecal",     PERS_ENABLE_DDRDynamicRecal,  "Enable dynamic memory controller recalibration"   },
  { "DDRNoTerm",       PERS_ENABLE_DDRNoTerm,        "Enable inifnite ohm on termination resistor."     },
  { "DDRBackScrub",    PERS_ENABLE_DDRBackScrub,     "Enable background scrubbing."                     },
  { "DDRDirScrub",     PERS_ENABLE_DDRDirScrub,      "Enable directed scrubbing."                       },
  { "SpecCapDDR",      PERS_ENABLE_SpecCapDDR,       "Allow speculation to DDR memory"                  },
  { "L2Only",          PERS_ENABLE_L2Only,           "Run out of L2 alone (but still requires +DDR)"    },
  { "GlobalInts",      PERS_ENABLE_GlobalInts,       "Global interrupts"                                },
  { "SerDes",          PERS_ENABLE_SerDes,           "SerDes Support"                                   },
  { "UPC",             PERS_ENABLE_UPC,              "Universal Performance Counters"                   },
  { "EnvMon",          PERS_ENABLE_EnvMon,           "Environmental Monitor"                            },
  { "PCIe",            PERS_ENABLE_PCIe,             "PCI Express"                                      },
  { "TimeSync",        PERS_ENABLE_TimeSync,         "Enable A2 timebase synchronization"               },
  { "Diagnostics",     PERS_ENABLE_DiagnosticsMode,  "Enable diagnostics mode"                          },
  { "MMU",             PERS_ENABLE_MMU,              "Enable Memory Management TLBs"                    },
  { "MU",              PERS_ENABLE_MU,               "Enable Messaging Unit"                            },
  { "ND",              PERS_ENABLE_ND,               "Enable Network Device"                            },
  { "IsIoNode",        PERS_ENABLE_IsIoNode,         "Used to indicate an I/O node (simlulation only)"  },
  { "TakeCPU",         PERS_ENABLE_TakeCPU,          "Enable takeCPU mode in firmware"                  },
  { "BeDRAM",          PERS_ENABLE_BeDRAM,           "Enables BeDRAM Unit initialization"               },
  { "ClkStopUnit",     PERS_ENABLE_ClockStop,        "Enables Clock Stop Unit initialization"           },
  { "DrArbiter",       PERS_ENABLE_DrArbiter,        "Enables DR Arbiter Unit initialization"           },
  { "DevBus",          PERS_ENABLE_DevBus,           "Enables Device Bus Unit initialization"           },
  { "L1P",             PERS_ENABLE_L1P,              "Enables L1 Prefetch Unit initialization"          },
  { "L2",              PERS_ENABLE_L2,               "Enables L2 Cache Unit initialization"             },
  { "MSGC",            PERS_ENABLE_MSGC,             "Enables MSGC Unit initialization"                 },
  { "TestInt",         PERS_ENABLE_TestInt,          "Enables Test Interface Unit initialization"       },
  { "NodeRepro",       PERS_ENABLE_NodeRepro,        "Enables single node cycle reproducibility"        },
  { "PartitionRepro",  PERS_ENABLE_PartitionRepro,   "Enables partition-wide cycle reproducibility"     },
  { "DD1Workarounds",  PERS_ENABLE_DD1_Workarounds,  "Enables DD1 hardware workarounds"                 },
  { "AppPreload",      PERS_ENABLE_AppPreload,       "Application is NOT loaded via CIOS."              },
  { "IOServices",      PERS_ENABLE_IOServices,       "Enables CIOS function shipping."                  },
  { "A2Errata",        PERS_ENABLE_A2_Errata,        "Enables assorted A2 workarounds."                 },
  { "A2_IU_LLB",       PERS_ENABLE_A2_IU_LLB,        "Enables the A2's IU Livelock Buster."             },
  { "A2_XU_LLB",       PERS_ENABLE_A2_XU_LLB,        "Enables the A2's XU Livelock Buster."             },
  { "DDRCal",          PERS_ENABLE_DDRCalibration,   "Enables DDR Calibration."                         },
  { "DDRFastInit",     PERS_ENABLE_DDRFastInit,      "Enables DDR Fast Initialization."                 },
  { "DDRCellTest",     PERS_ENABLE_DDRCellTest,      "Enables DDR Cell Test."                           },
  { "DDRAutoSize",     PERS_ENABLE_DDRAutoSize,      "Enables automatic DDR size detection."            },
  { "MaskLinkErrors",  PERS_ENABLE_MaskLinkErrors,   "Masks network link errors (use with caution)."    },
  { "MaskCorrectables",PERS_ENABLE_MaskCorrectables, "Masks correctables after first occurrence."       },
  { NULL,              0,                            NULL                                               }  
};


Svc_Control_T TraceControls[] = {
  { "TraceND",          TRACE_ND,        "Trace Network Device"            },
  { "TraceMU",          TRACE_MU,        "Trace Messaging Unit"            },
  { "TracePCIe",        TRACE_PCIe,      "Trace PCI-Express initialization"},
  { "TraceEntry",       TRACE_Entry,     "Function entry and return"       },
  { "TraceMChk",        TRACE_MChk,      "Machine Check Dispatch"          },
  { "TraceSysCall",     TRACE_SysCall,   "System Calls"                    },
  { "TraceVMM",         TRACE_VMM,       "Virtual Memory Manager"          },
  { "TraceDebug",       TRACE_Debug,     "Debug Events"                    },
  { "TraceSerDes",      TRACE_SerDes,    "SerDes Init"                     },
  { "TraceProcess",     TRACE_Process,   "Process/Thread Setup"            },
  { "TraceExitSummary", TRACE_Exit_Sum,  "Error summary on exit"           },
  { "TraceScheduler",   TRACE_Sched,     "Thread Scheduler Activity"       },
  { "TraceFutex",       TRACE_Futex,     "Futex Activity"                  },
  { "TraceMemAlloc",    TRACE_MemAlloc,  "Memory Allocations"              },
  { "TraceVerbose",     TRACE_Verbose,   "Enable verbose trace output"     },
  { "TraceBeDRAM",      TRACE_BeDRAM,    "Enable BeDRAM trace" },
  { "TraceDevBus",      TRACE_DevBus,    "Enable DevBus trace" },
  { "TraceDDR",         TRACE_DDR,       "" },
  { "TraceL2",          TRACE_L2,        "" },
  { "TraceClockStop",   TRACE_ClockStop, "" },
  { "TraceL1P",         TRACE_L1P,       "" },
  { "TraceAll",         -1,              "" },
  { NULL,               0,               NULL                              }  
};

Svc_Control_T RASControls[] = {
  { "RASFatalExit",  PERS_RASPOLICY_FATALEXIT,    "Exit on Fatal RAS Event"  },
  { NULL,            0,                                NULL                       }  };


int svc_find_Control( Svc_Control_T* controls, char *name, uint64_t*bits );

int svc_edit_controls( char* p, Personality_t* personality, int flag_enable) {

   uint64_t bits = 0;
   
   if ( svc_find_Control( NodeConfigControls, p, &bits ) ) {
     flag_enable ? (personality->Kernel_Config.NodeConfig |= bits) : (personality->Kernel_Config.NodeConfig &= ~bits);
     return 0;
   }
   else if ( svc_find_Control( TraceControls, p, &bits ) ) {
     flag_enable ? (personality->Kernel_Config.TraceConfig |= bits) : (personality->Kernel_Config.TraceConfig &= ~bits);
     return 0;
   }
   else if ( svc_find_Control( RASControls, p, &bits ) ) {
     flag_enable ? (personality->Kernel_Config.RASPolicy |= bits) : (personality->Kernel_Config.RASPolicy &= ~bits);
     return 0;
   }

   return 1; // not found
}


int netflags_flag_enable;

int svc_parse_net_dimension(char* q, int lower, int upper) {

  char* p = q;

  if ( *p == '(' ) {

    p++;

    int value = -1;

    switch (toupper(*p)) 
      {
      case '0' : value = 0; break;
      case '1' : value = 1; break;
      case '2' : value = 2; break;
      case '3' : value = 3; break;
      case '4' : value = 4; break;
      case '5' : value = 5; break;
      case 'A' : value = 0; break;
      case 'B' : value = 1; break;
      case 'C' : value = 2; break;
      case 'D' : value = 3; break;
      case 'E' : value = 4; break;
      case 'I' : value = 5; break;
      }

    if ( ( value >= lower ) && (value <= upper) ) {

      p++;

      if ( *p == ')' )
	return value;
    }
  }

  fprintf(stderr, "(E) Invalid network dimension specified: %s\n", q );
  return -1;
}

int svc_parse_int_dimension(char** q, int lower, int upper) {

  char* p = *q;

  if ( *p == '(' ) {

    p++;

    int value = 0;

    while ( (toupper(*p) >= '0') && (toupper(*p) <= '9') ) {
      int digit = toupper(*(p++)) - '0';
      value *= 10;
      value += digit;
    }

    if ( ( *p == ')' ) && ( ( value >= lower ) && (value <= upper) ) ) {
      *q = ++p;
      return value;
    }
  }

  fprintf(stderr, "(E) Invalid dimension specified: %s.  Expected range [%d,..,%d]\n", *q, lower, upper );
  return -1;
}



int svc_edit_netflags_ion_link(char* source, Personality_t* pers) {
  
  if (netflags_flag_enable) {
    pers->Network_Config.NetFlags |= ND_ENABLE_ION_LINK;
  }
  else {
    pers->Network_Config.NetFlags &= ~ND_ENABLE_ION_LINK;
  }
  
  return 0;
}


int svc_edit_netflags_loopback(char* source, Personality_t* pers) {
  
  int n = svc_parse_net_dimension(source, 0, 5);

  if ( n >= 0 ) {

    source += 4;
    int value = 0;

    if ( ! strcasecmp( "NONE", source ) ) {
      value = ND_LOOPBACK_NONE;
    }
    else if ( ! strcasecmp( "TORUS", source ) ) {
      value = ND_LOOPBACK_TORUS_INTERNAL;
    }
    else if ( ! strcasecmp( "SERDES", source ) ) {
      value = ND_LOOPBACK_SERDES;
    }
    else if ( ! strcasecmp( "HSS", source ) ) {
      value = ND_LOOPBACK_HSS;
    }
    else {
      fprintf( stderr, "(E) Invalid loopback setting; must be one of NONE, TORUS, SERDES, HSS\n" );
      return -1;
    }
    
    pers->Network_Config.NetFlags &= ~ND_DIM_LOOPBACK(n,-1);
    pers->Network_Config.NetFlags |=  ND_DIM_LOOPBACK(n,value);
 
    return 0;
  }
  
  return -1;
}

int svc_edit_netflags_torus(char* source, Personality_t* pers) {
  
  int n = svc_parse_net_dimension(source, 0, 4);

  if ( n >= 0 ) {
    if (netflags_flag_enable) {
      pers->Network_Config.NetFlags |= ND_ENABLE_TORUS(n);
    }
    else {
      pers->Network_Config.NetFlags &= ~ND_ENABLE_TORUS(n);
    }
    return 0;
  }
  
  return -1;
}

int svc_edit_netflags_det_order(char* source, Personality_t* pers) {

  int n = svc_parse_net_dimension(source, 0, 4);

  if ( n >= 0 ) {

    unsigned order = 0;

    // Advance to the ')'

    while ( *source != 0 ) {
      if ( *(source++) == ')' )
	break;
    }

    if ( sscanf( source, "=0x%x", &order ) != 1 ) {
      fprintf(stderr, "(E) Input error.\n");
      return -1;
    }

    // Deterministic order masks are 5 bit values that should
    // have exactly one bit set:

    if ( (order & ~0x1F) != 0 ) {
      fprintf(stderr,"(E) Illegal value: Deteterministic order routing masks are only 5 bits.");
      return -1;
    }

    unsigned mask = 0x10;
    unsigned oneBits = 0;

    while (mask) {
      oneBits += ( (order & mask) ? 1 : 0 );
      mask >>= 1;
    }

    if (oneBits != 1) {
      fprintf(stderr, "(E) Illegal value:  Deterministic order routing masks should have exactly one bit set.");
      return -1;
    }

    pers->Network_Config.NetFlags &= ~ND_DET_ORDER_MASK(n,-1);
    pers->Network_Config.NetFlags |=  ND_DET_ORDER_MASK(n,order);
    
    return 0;
  }
  
  return -1;
}

int svc_edit_netflags_ran_hints(char* source, Personality_t* pers) {

  int n = svc_parse_net_dimension(source, 0, 4);

  if ( n >= 0 ) {
    if (netflags_flag_enable) {
      pers->Network_Config.NetFlags |= ND_ENABLE_TORUS_RAN_HINTS(n);
    }
    else {
      pers->Network_Config.NetFlags &= ~ND_ENABLE_TORUS_RAN_HINTS(n);
    }
    return 0;
  }
  
  return -1;
}

int svc_edit_mu_flags(char* source, Personality_t* pers) {

  unsigned long flags = 0;

  if ( svc_getHexLong(source, &flags) == 0 ) {
    pers->Network_Config.MuFlags = flags;
    return 0;
  }
  
  return -1;
}

int svc_parse_link(char* source, unsigned* dimension, unsigned* direction ) {

  char* s = source;

  if ( *(s++) != '(' ) {
    fprintf(stderr, "(E) Invalid link specification: %s\n", source);
    return -1;
  }


  char dim = *(s++);

  if ( (dim >= 'A') && ( dim <= 'E' ) ) {
    *dimension = dim - 'A';
  }
  else if ( (dim >= 'a') && ( dim <= 'e' ) ) {
    *dimension = dim - 'a';
  }
  else if ( ( dim == 'I' ) || ( dim == 'i' ) ) {
    *dimension = 5;
    dim = *(s++);
    if ( (dim != 'O') && (dim != 'o') ) {
      fprintf(stderr, "(E) Invalid link specification: Link%s\n", source);
      return -1;
    }
  }

  char dir = *(s++);

  if ( ( dir == '+' ) || ( dir == '1') ) {
    *direction = 1;
  }
  else if ( ( dir == '-' ) || (dir == '0') ) {
    *direction = 0;
  }
  else if ( dir == ')' ) {
    *direction = 0;
  }
  else {
      fprintf(stderr, "(E) Invalid link specification: %s\n", source);
      return -1;
  }

  return 0;
}

int svc_edit_torus_link(char* source, Personality_t* pers) {
  
  // char* s = source;

  unsigned dimension = 0, direction = 0;

  if ( svc_parse_link( source, &dimension, &direction ) != 0 ) {
    return -1;
  }
  
  if ( dimension == 5 ) {
    fprintf(stderr, "(E) Cannot specify IO dimension for Torus link enables.\n");
    return -1;
  }

  if (netflags_flag_enable) {
    ND_SET_LINK_ENABLE( dimension, direction, pers->Network_Config.NetFlags2 );
  }
  else {
    ND_CLEAR_LINK_ENABLE( dimension, direction, pers->Network_Config.NetFlags2 );
  }

  return 0;
}

int svc_edit_serdes_link(char* source, Personality_t* pers) {
  
  // char* s = source;

  unsigned dimension = 0, direction = 0;

  if ( svc_parse_link( source, &dimension, &direction ) != 0 ) {
    return -1;
  }
  
  if (netflags_flag_enable) {
    SD_SET_LINK_ENABLE( dimension, direction, pers->Network_Config.NetFlags2 );
  }
  else {
    SD_CLEAR_LINK_ENABLE( dimension, direction, pers->Network_Config.NetFlags2 );
  }

  return 0;
}

int svc_edit_port6(char* source, Personality_t* pers) {

    if ( netflags_flag_enable ) {
	pers->Network_Config.NetFlags2 |= TI_USE_PORT6_FOR_IO;
    }
    else {
	pers->Network_Config.NetFlags2 &= ~TI_USE_PORT6_FOR_IO;
    }
    return 0;
}

int svc_edit_port7(char* source, Personality_t* pers) {

    if ( netflags_flag_enable ) {
	pers->Network_Config.NetFlags2 |= TI_USE_PORT7_FOR_IO;
    }
    else {
	pers->Network_Config.NetFlags2 &= ~TI_USE_PORT7_FOR_IO;
    }
    return 0;
}

int svc_edit_torus_swap(char* source, Personality_t* pers) {
  
  // char* s = source;

    unsigned dim = svc_parse_net_dimension(source, 0, 4);

    if ( dim >= 0 ) {
	if (netflags_flag_enable) {
	    TI_SET_TORUS_DIM_REVERSED(dim,pers->Network_Config.NetFlags2);
	}
	else {
	    TI_CLEAR_TORUS_DIM_REVERSED(dim,pers->Network_Config.NetFlags2) ;
	}
    }
    else {
	fprintf(stderr, "(E) Illegal dimension specified for torus swap.\n");
	return -1;
    }

  return 0;
}

int svc_edit_cn_bridge_port6(char* source, Personality_t* pers) {

    if ( netflags_flag_enable ) {
	pers->Network_Config.NetFlags2 |= ND_CN_BRIDGE_PORT_6;
    }
    else {
	pers->Network_Config.NetFlags2 &= ~ND_CN_BRIDGE_PORT_6;
    }
    return 0;
}
int svc_edit_cn_bridge_port7(char* source, Personality_t* pers) {

    if ( netflags_flag_enable ) {
	pers->Network_Config.NetFlags2 |= ND_CN_BRIDGE_PORT_7;
    }
    else {
	pers->Network_Config.NetFlags2 &= ~ND_CN_BRIDGE_PORT_7;
    }
    return 0;
}

int svc_edit_cn_bridge_port10(char* source, Personality_t* pers) {

    if ( netflags_flag_enable ) {
	pers->Network_Config.NetFlags2 |= ND_CN_BRIDGE_PORT_10;
    }
    else {
	pers->Network_Config.NetFlags2 &= ~ND_CN_BRIDGE_PORT_10;
    }
    return 0;
}

int svc_edit_sd_reset(char* source, Personality_t* pers) {
    if ( netflags_flag_enable ) {
	pers->Network_Config.NetFlags2 |= SD_RESET;
    }
    else {
	pers->Network_Config.NetFlags2 &= ~SD_RESET;
    }
    return 0;

}


Svc_SimpleEditor_T Netflags[] = {
  { svc_edit_netflags_ion_link,  "IONLink",    "Enable/disable the compute to IO node link (compute nodes only)",                               "[+|-]",  ""              },
  { svc_edit_netflags_loopback,  "Loopback",   "Set loopback for the specified dimension to (NONE, TORUS SERDES or HSS), e.g. Loopback(A)=TORUS",   "    -",  "(dim)=value"   },
  { svc_edit_netflags_torus,     "Torus",      "Enable torus (or disable, i.e. mesh) in the specified dimension, e.g. +Torus(A) or -Torus(B).", "[+|-]",  "(dim)"         },
  { svc_edit_netflags_ran_hints, "RanHints",   "Ties for +/- choice are broken randomly, e.g. +RanHints(A), -RanHints(B).",                     "[+|-]",  ""              },
  { svc_edit_netflags_det_order, "DetOrder",   "Programmable deterministic order masks.",                                                       "    -",  "(dim)=0xValue" },
  { svc_edit_mu_flags,           "MUFlags",    "Set MU Iniitalization Flags.",                                                                  "    -",  "MUFlags=0xValue" },
  { svc_edit_serdes_link,        "LinkSerDes", "Enables/disables links, e.g. +LinkSerDes(A+), -LinkSerDes(A+), +LinkSerDes(IO).",               "[+|-]",  "(A..E,IO +/-)" },
  { svc_edit_torus_link,         "LinkTorus",  "Enables/disables links, e.g. +Link(A+), -Link(A+).",                                            "[+|-]",  "(A..E +/-)" },
  { svc_edit_torus_link,         "Link",       "Deprecated.  See LinkTorus (above).",                                                           "[+|-]",  "(A..E +/-)" },
  { svc_edit_port6,              "Port6",      "Use Port6 for IO",                                                                              "[+|-]",  "" },
  { svc_edit_port7,              "Port7",      "Use Port7 for IO",                                                                              "[+|-]",  "" },
  { svc_edit_torus_swap,         "SwapTorus",  "Enables/disables torus swap bits, e.g +SwapTorus(A)",                                           "[+|-]",  "(A..E)" },
  { svc_edit_cn_bridge_port6,    "CnBridgePort6",      "Use Port 6 for Compute Node Bridge",                                                    "[+|-]",  "" },
  { svc_edit_cn_bridge_port7,    "CnBridgePort7",      "Use Port 7 for Compute Node Bridge",                                                    "[+|-]",  "" },
  { svc_edit_cn_bridge_port10,   "CnBridgePort10",     "Use Port 10 for Compute Node Bridge",                                                   "[+|-]",  "" },
  { svc_edit_sd_reset,           "SdReset",    "Full reset of SerDes",                                                                          "[+|-]",  "" },
  { NULL, 0, NULL } 
};

int svc_edit_netflags( char* p, Personality_t* personality, int flag_enable) {
  
  int i;

  netflags_flag_enable = flag_enable; // set a global since we dont have a generic arg for the handler (ugh!)

  for ( i = 0 ; Netflags[i].id; i++ ) {
    if ( ! strncasecmp( p, Netflags[i].id, strlen(Netflags[i].id) ) ) {
      return Netflags[i].handler( p + strlen(Netflags[i].id), personality );
    }
    else {
      //printf("NOPE:  %s vs %s\n", p, Netflags[i].id);
    }
  }

  return -1;
}

int svc_getHex(char* source, unsigned* value) {
  if ( sscanf( source, "=0x%x", value ) != 1 ) {
    fprintf(stderr, "(E) Input error.\n");
    return -1;
  }
  return 0;
}

int svc_getHexLong(char* source, unsigned long* value) {
  if ( sscanf( source, "=0x%lx", value ) != 1 ) {
    fprintf(stderr, "(E) Input error.\n");
    return -1;
  }
  return 0;
}


int svc_edit_classroutes_glint_in(char* source, Personality_t* pers) {

    unsigned value = 0;

    if ( svc_getHex(source, &value) == 0 ) {
	pers->Network_Config.PrimordialClassRoute.GlobIntUpPortInputs = value;
	return 0;
    }

    return -1;
}

int svc_edit_classroutes_glint_out(char* source, Personality_t* pers) {

    unsigned value;

    if ( svc_getHex(source, &value) == 0 ) {
	pers->Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs = value;
	return 0;
    }


    return -1;
}

int svc_edit_classroutes_coll_type(char* source, Personality_t* pers) {

    unsigned value;

    if ( svc_getHex(source, &value) == 0 ) {
      ND_CR_SET_COLL_TYPE(value,pers->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs);
      return 0;
    }


  return -1;
}


int svc_edit_classroutes_coll_input(char* source, Personality_t* pers) {

    unsigned value;

    if ( svc_getHex(source, &value) == 0 ) {
	ND_CR_SET_COLL_INPUT(value,pers->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs);
      return 0;
    }

  return -1;
}

int svc_edit_classroutes_coll_output(char* source, Personality_t* pers) {

    unsigned value;

    if ( svc_getHex(source, &value) == 0 ) {
      pers->Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs = value;
      return 0;
    }

  return -1;
}
int svc_edit_zone_mask(char* source, Personality_t* pers) {

  int zone = svc_parse_int_dimension(&source, 0, 3);

  if ( zone >= 0 ) {

    int mask = svc_parse_int_dimension(&source, 0, 4);

    if (mask >= 0) {

      unsigned value;

      if ( svc_getHex(source, &value) == 0 ) {

	// these are 5-bit fields:

	if ( (value & ~0x1F) != 0 ) {
	  fprintf(stderr, "(E) Illegal value: zone routing masks are 5-bit values\n");
	  return -1;
	}

	ND_SET_DYNAMIC_ZONE_MASK(zone, mask, value, &(pers->Network_Config));

	return 0;
      }
    }
  }
  return -1;
}

Svc_SimpleEditor_T ClassRouteEditors[] = {
  { svc_edit_classroutes_glint_in,    "GlintIn",    "Global Interrupt Up Tree Inputs  (11 torus links + 1 internal).",       "-",  "(cr)=0xValue"  },
  { svc_edit_classroutes_glint_out,   "GlintOut",   "Global Interrupt Up Tree Outputs (11 torus links + 1 internal).",       "-",  "(cr)=0xValue"  },
  { svc_edit_classroutes_coll_type,   "CollType",   "Collective type.",                                                      "-",  "(cr)=0xValue"  },
  { svc_edit_classroutes_coll_input,  "CollIn",     "Collective Up Tree Inputs        (11 torus links + 1 internal).",       "-",  "(cr)=0xValue"  },
  { svc_edit_classroutes_coll_output, "CollOut",    "Collective Up Tree Outputs       (11 torus links + 1 internal).",       "-",  "(cr)=0xValue"  },
  { svc_edit_zone_mask,               "ZoneMask",   "Dynamic Zone Masks (4 zones by 5 masks).",                              "-",  "(zone)(mask)=0xValue"  },
  { NULL, 0, NULL }
};

int svc_edit_classroutes( char* p, Personality_t* personality, int flag_enable) {

  int i;

  for ( i = 0 ; ClassRouteEditors[i].id; i++ ) {
    if ( ! strncasecmp( p, ClassRouteEditors[i].id, strlen(ClassRouteEditors[i].id) ) ) {
      return ClassRouteEditors[i].handler( p + strlen(ClassRouteEditors[i].id), personality );
    }
  }
  return -1;
}

int svc_Example( char example_file_name[] ) {

  FILE *fp;
  int i;

  uint64_t node_config  = Pers_Template.Kernel_Config.NodeConfig;
  //uint64_t trace_config = Pers_Template.Kernel_Config.TraceConfig;
  uint32_t ras_config   = Pers_Template.Kernel_Config.RASPolicy;

  if ( (fp = fopen( example_file_name, "w" )) == NULL ) {
    fprintf(stderr,"(E) Unable to open %s for writing.\n", example_file_name );
    return -1;
  }

  fprintf(fp, "#\n# %s\n#\n", svc_Herald );
  fprintf(fp, "# Node controls (use \'-\' to disable, \'+\' to enable)\n");

  for ( i = 0 ; NodeConfigControls[i].name ; i++ ) {
    fprintf(fp, "   %c%-20s# %s\n", (((node_config & NodeConfigControls[i].bits) == NodeConfigControls[i].bits) ? '+' : '-'), NodeConfigControls[i].name, NodeConfigControls[i].help );
  }


  fprintf(fp, "# Kernel Tracing Controls:\n");
  for ( i = 0 ; TraceControls[i].name ; i++ ) {
    fprintf(fp, "   -%-20s# %s\n", TraceControls[i].name, TraceControls[i].help );;
  }


  fprintf(fp, "# RAS Policy Controls:\n");
  for ( i = 0 ; RASControls[i].name ; i++ ) {
    fprintf(fp, "   %c%-20s# %s\n", (((ras_config & RASControls[i].bits) == RASControls[i].bits) ? '+' : '-'), RASControls[i].name, RASControls[i].help );
  }


  fclose( fp );

  return 0;
}

//! @todo FIX
#define _bgp_unit_svc_host 0x99

int svc_Usage( char *msg, char *msg2 ) {
  snprintf(
	   svc_host_error_msg, SVC_MAX_ERROR_MESSAGE_LEN, "(E %04d-%04d) svc_host: Usage error: %s%s.\n", 
	   _bgp_unit_svc_host, _svc_error_usage,
	   (msg  ? msg  : "Bad command line"),
	   (msg2 ? msg2 : "") );

  return _svc_error_usage;
}

void svc_Warning( _BGP_Svc_Host_ErrCodes code, char *msg ) {
  printf("(W %04d-%04d) svc_host: Warning: %s.\n",
	 _bgp_unit_svc_host, code,
	 (msg ? msg : "Bad command line") );

  fflush(stdout);
}


#define PRINT16(label, value) printf( label "0x%04x\n", (value) )
#define PRINT32(label, value) printf( label "0x%08x\n", (value) )
#define PRINT64(label, value) printf( label "0x%08x-%08x\n", (unsigned)((value)>>32), (unsigned)(value) )

char* bitString( unsigned value, unsigned length, char* buffer) {
  unsigned i;
  for (i = 0; i < length; i++) {
    unsigned mask = 1 << (length-i-1);
    buffer[i] = (value & mask) ? '1' : '0';
  }
  buffer[length] = 0;
  return buffer;
}


void svc_ShowPersonality( Personality_t *p ) {

  int i;
  char buffer[64];
  char uciStr[64];

  uciStr[0] = 0;

  bg_uci_toString( p->Kernel_Config.UCI, uciStr );

  printf ("Personality: CRC 0x%04x, Version %d, Size %d.\n", p->CRC, p->Version, p->PersonalitySizeWords );
  printf (" Kernel_Config:\n");
  printf( "   UCI:         %016llX (%s)\n", p->Kernel_Config.UCI, uciStr );
  PRINT64("   NodeConfig:  ", p->Kernel_Config.NodeConfig);

  if ( p->Kernel_Config.NodeConfig != 0 ) {
      printf("         ");
      for ( i = 0 ; i < sizeof(NodeConfigControls)/sizeof(NodeConfigControls[0]); i++ ) {
	  if ( ( p->Kernel_Config.NodeConfig & NodeConfigControls[i].bits ) != 0 ) {
	      printf("+%s ", NodeConfigControls[i].name);
	  }
      }
      printf("\n");
  }

  PRINT64("   TraceConfig: ", p->Kernel_Config.TraceConfig);
  PRINT32("   RASPolicy:   ", p->Kernel_Config.RASPolicy );
  PRINT32("   FreqMHz:     ", p->Kernel_Config.FreqMHz );
  PRINT64("   ClockStop:   ", p->Kernel_Config.ClockStop);

  printf (" DDR_Config:\n");
  PRINT32("    DDRFlags:       ", p->DDR_Config.DDRFlags );
  printf ("      DDR Pwr Dwn   %s\n", DDR_PWR_DWN[ DDR_GET_POWER_DOWN(p->DDR_Config.DDRFlags) ] );;
  printf ("    DDRSizeMB:  %08X (%lld bytes)\n", p->DDR_Config.DDRSizeMB, (uint64_t)p->DDR_Config.DDRSizeMB * 1024ull * 1024ull);

  uint64_t nf = p->Network_Config.NetFlags;

  printf (" Network_Config:\n");
  PRINT64("    NetFlags: ", p->Network_Config.NetFlags);
  printf ("      LoopBack        : (%lld,%lld,%lld,%lld,%lld,%lld)  NOTE: %d=NONE %d=TORUS %d=SERDES %d=HSS\n", 
	  ND_GET_LOOPBACK(0,nf), ND_GET_LOOPBACK(1,nf), ND_GET_LOOPBACK(2,nf), 
	  ND_GET_LOOPBACK(3,nf), ND_GET_LOOPBACK(4,nf), ND_GET_LOOPBACK(5,nf), 
	  ND_LOOPBACK_NONE, ND_LOOPBACK_TORUS_INTERNAL, ND_LOOPBACK_SERDES, ND_LOOPBACK_HSS );
  printf ("      Torus/Mesh      : (%lld,%lld,%lld,%lld,%lld) (0=Mesh, 1=Torus)\n", ND_GET_TORUS(0,nf), ND_GET_TORUS(1,nf), ND_GET_TORUS(2,nf), ND_GET_TORUS(3,nf), ND_GET_TORUS(4,nf) );
  printf ("      RanHints        : (%lld,%lld,%lld,%lld,%lld)\n", ND_GET_TORUS_RAN_HINTS(0,nf), ND_GET_TORUS_RAN_HINTS(1,nf), ND_GET_TORUS_RAN_HINTS(2,nf), ND_GET_TORUS_RAN_HINTS(3,nf), ND_GET_TORUS_RAN_HINTS(4,nf) );
  printf ("      Rcvr/Inj Ratio  : 0x%04llX\n", ND_GET_SE_ARB_RE_INJ_RATIO(nf) );
  printf ("      DetOrderMasks 0 : %s\n", bitString( ND_GET_DET_ORDER_MASK(0,nf), 5, buffer ) );
  printf ("      DetOrderMasks 1 : %s\n", bitString( ND_GET_DET_ORDER_MASK(1,nf), 5, buffer ) );
  printf ("      DetOrderMasks 2 : %s\n", bitString( ND_GET_DET_ORDER_MASK(2,nf), 5, buffer ) );
  printf ("      DetOrderMasks 3 : %s\n", bitString( ND_GET_DET_ORDER_MASK(3,nf), 5, buffer ) );
  printf ("      DetOrderMasks 4 : %s\n", bitString( ND_GET_DET_ORDER_MASK(4,nf), 5, buffer ) );

  uint64_t nf2 = p->Network_Config.NetFlags2;

  static char* DIMENSIONS[] = { "A", "B", "C", "D", "E", "IO" };
  static char* DIRECTIONS[] = { "-", "+" };

  PRINT64("    NetFlags2: ", nf2);
  printf("       Torus Links Enabled : ");
  {

    unsigned dimension, direction;

    for ( dimension = 0; dimension < 5; dimension++ ) {
      for ( direction = 0; direction <= 1; direction++ ) {
	if ( ND_GET_LINK_ENABLE(dimension, direction, nf2 ) ) {
	  printf("%s%s ", DIMENSIONS[dimension], DIRECTIONS[direction]);
	}
      }
    }
    printf("\n");
  }
  printf("       SerDes Links Enabled : ");
  {

    unsigned dimension, direction;

    for ( dimension = 0; dimension <= 5; dimension++ ) {
      for ( direction = 0; direction <= 1; direction++ ) {
	if ( SD_GET_LINK_ENABLE(dimension, direction, nf2 ) ) {
	  printf("%s%s ", DIMENSIONS[dimension], DIRECTIONS[direction]);
	}
      }
    }
    printf("\n");
  }

  printf("       Torus Swaps          : (%lld,%lld,%lld,%lld,%lld)\n", 
	 TI_GET_TORUS_DIM_REVERSED(0,nf2), 
	 TI_GET_TORUS_DIM_REVERSED(1,nf2), 
	 TI_GET_TORUS_DIM_REVERSED(2,nf2), 
	 TI_GET_TORUS_DIM_REVERSED(3,nf2), 
	 TI_GET_TORUS_DIM_REVERSED(4,nf2) );
  printf("       Port 6/7             : (%d,%d)\n", (nf2 & TI_USE_PORT6_FOR_IO ? 1 : 0), (nf2 & TI_USE_PORT7_FOR_IO ? 1 : 0) );
  
  printf("       CN Bridge (6,7,10)   : (%d,%d,%d)\n", (nf2 & ND_CN_BRIDGE_PORT_6 ? 1 : 0), (nf2 & ND_CN_BRIDGE_PORT_7 ? 1 : 0), (nf2 & ND_CN_BRIDGE_PORT_10 ? 1 : 0) );
  printf("       SD reset             : %d\n", nf2 & SD_RESET ? 1 : 0 );
  printf ("    Torus:      (%d,%d,%d,%d,%d) dimensions:(%d,%d,%d,%d,%d)\n", 
	  p->Network_Config.Acoord, p->Network_Config.Bcoord, p->Network_Config.Ccoord, p->Network_Config.Dcoord, p->Network_Config.Ecoord,
	  p->Network_Config.Anodes, p->Network_Config.Bnodes, p->Network_Config.Cnodes, p->Network_Config.Dnodes, p->Network_Config.Enodes
	  );
  printf ("    CN Bridge:  (%d,%d,%d,%d,%d)\n",
	  p->Network_Config.cnBridge_A, p->Network_Config.cnBridge_B, p->Network_Config.cnBridge_C, p->Network_Config.cnBridge_D, p->Network_Config.cnBridge_E
	  );

  printf ("    PrimordialClassRoute :\n");

  printf("      [%2d] GLINT I:0x%03X O:0x%03X  Collective T:%d I:0x%03X O:0x%03X\n", i, 
	 p->Network_Config.PrimordialClassRoute.GlobIntUpPortInputs, p->Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs,
	 ND_CR_GET_COLL_TYPE(p->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs),
	 ND_CR_GET_COLL_INPUT(p->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs),
	 p->Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs
      );

  printf ("    ZoneRoutingMasks:\n");
  for (i=0; i < sizeof(p->Network_Config.ZoneRoutingMasks)/sizeof(p->Network_Config.ZoneRoutingMasks[0]); i++) {
    printf("      Zone %d Masks 0x%02llX-%02llX-%02llX-%02llX-%02llX\n",
	   i,
	   ND_GET_DYNAMIC_ZONE_MASK(i, 0, &(p->Network_Config)),
	   ND_GET_DYNAMIC_ZONE_MASK(i, 1, &(p->Network_Config)),
	   ND_GET_DYNAMIC_ZONE_MASK(i, 2, &(p->Network_Config)),
	   ND_GET_DYNAMIC_ZONE_MASK(i, 3, &(p->Network_Config)),
	   ND_GET_DYNAMIC_ZONE_MASK(i, 4, &(p->Network_Config)));
  }

  PRINT64("    MU Flags: ", p->Network_Config.MuFlags);
  printf ("    Other info: tbd\n");
   
  printf (" Ethernet_Config:\n");

}

int svc_find_Control( Svc_Control_T* controls, char *name, uint64_t*bits )  {

  int i;

  *bits = 0;

  for ( i = 0 ; controls[i].name ; i++ )  {
    if ( ! strcasecmp( name, controls[i].name ) ) {
      *bits = controls[i].bits;
      return 1;
    }
  }

  return 0;
}



int find_ras_verbosity( char *str, uint32_t *policy, char *msg ) {

  int i;

  for ( i = 0 ; i < sizeof(ras_verbosity)/sizeof(ras_verbosity[0]); i++ ) {
    if ( !strcasecmp( str, ras_verbosity[i].spec ) ) {
      *policy &= ~PERS_RASPOLICY_VERBOSITY(-1);
      *policy |= ras_verbosity[i].value;
      return _svc_error_none;
    }
  }

  return svc_Usage( msg, NULL );
}


int find_ddr_power_down( char *str, Personality_DDR_t* ddrConfig, char *msg ) {

  int i;

  for ( i = 0 ; i < sizeof(DDR_PWR_DWN)/sizeof(DDR_PWR_DWN[0]); i++ ) {
    if ( !strcasecmp( str, DDR_PWR_DWN[i] ) ) {
	DDR_SET_POWER_DOWN( ddrConfig->DDRFlags, i );
	return _svc_error_none;
    }
  }

  return svc_Usage( msg, NULL );
}

int svc_getString( char *source, char *target, int maximum_length, char *error_message ) {
  
  if ( ! *source ) {
    return svc_Usage( error_message, NULL );
  }
  
  strncpy( target, source, maximum_length );

  return _svc_error_none; 
}


int  svc_getInt8( char *str, uint8_t *var, uint8_t vmin, uint8_t vmax, char *fmsg, char *rmsg ) {

  uint32_t tmp = 0;
  int rc = sscanf( str, "%d", &tmp );

  if ( rc != 1 ) {
    return svc_Usage( fmsg, NULL );
  }
  else if ( (tmp < vmin) || (tmp > vmax) ) {
    return svc_Usage( rmsg, NULL );
  }

  *var = tmp;

  return _svc_error_none;
}


int svc_getInt16( char *str, uint16_t *var, uint16_t vmin, uint16_t vmax, char *fmsg, char *rmsg ) {

  uint32_t tmp = 0;
  int rc = sscanf( str, "%d", &tmp );

  if ( rc != 1 ) {
    return svc_Usage( fmsg, NULL );
  }
  else if ( (tmp < vmin) || (tmp > vmax) ) {
    return svc_Usage( rmsg, NULL );
  }

  *var = tmp;
   
  return _svc_error_none;
}

int svc_getInt32( char *str, uint32_t *var, uint32_t vmin, uint32_t vmax, char *fmsg, char *rmsg ) {
  
  uint32_t tmp = 0;
  int rc = sscanf( str, "%d", &tmp );

  if ( rc != 1 ) {
    return svc_Usage( fmsg, NULL );
  }
  else if ( (tmp < vmin) || (tmp > vmax) ) {
    return svc_Usage( rmsg, NULL );
  }

  *var = tmp;

  return _svc_error_none;
}

int svc_GetInt64( char *str, uint64_t *var, uint64_t vmin, uint64_t vmax, char *fmsg, char *rmsg ) {

  uint64_t tmp = 0;
  char *p = str;

  while( *p && isspace(*p) ) {
    p++;
  }

  if ( !*p ) {
    return svc_Usage( fmsg, NULL );
  }

  tmp = strtoull( p, NULL, 0 );

  if ( (tmp < vmin) || (tmp > vmax) ) {
    return svc_Usage( rmsg, NULL );
  }

  *var = tmp;

  return _svc_error_none;
}


int svc_GetIP( char *str, IP_Addr_t *var, char *msg )  {

  int a = 0, b = 0, c = 0, d = 0;
  int rc = sscanf( str, "%d.%d.%d.%d", &a, &b, &c, &d );

  if ( rc != 4 ) {
    return svc_Usage( msg, NULL );
  }

  memset( var, 0, sizeof(IP_Addr_t) );
  var->octet[0] = a;
  var->octet[1] = b;
  var->octet[2] = c;
  var->octet[3] = d;
  
  return _svc_error_none;
}


int svc_getMAC( char *str, uint8_t *var, char *msg ) {

  uint32_t a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
  int rc = sscanf( str, "%02x:%02x:%02x:%02x:%02x:%02x", &a, &b, &c, &d, &e, &f );

  if ( rc != 6 ) {
    return svc_Usage( msg, NULL );
  }

  *var++ = (a & 0xFF);
  *var++ = (b & 0xFF);
  *var++ = (c & 0xFF);
  *var++ = (d & 0xFF);
  *var++ = (e & 0xFF);
  *var++ = (f & 0xFF);
  
  return _svc_error_none;
}


int svc_MakeRank_Compute( int x, int y, int z, int X, int Y, int Z ) {
  return( x + (y * X) + (z * X * Y) );
}

int svc_MakeRank_IO( int X, int Y, int Z, int i ) {
  return( (X * Y * Z) + i );
}



int svc_bloader_PatchCRC( Svc_Firmware_Info_T *firmware ) {

  Svc_Elf_Info_T *efi = &(firmware->elfInfo);
  int rc;
  uint32_t crc_start = 0; //! @todo FIX ME (firmware->crcStartAddress - _BGP_VA_SRAM);
  uint32_t crc_len   = (firmware->crcStopAddress - firmware->crcStartAddress);
  uint16_t tCrc, *pCrc;

  if ( !efi->imageIsLoaded || !firmware->canBeCRCd ) {
    if ( debug ) {
      printf("(?) svc_bloader_PatchCRC: Image not loaded or cannot be CRC'd.\n");
    }
    return( _svc_error_none );
  }

  if ( (rc = svc_makeFirmwareImage( firmware )) )  {
    if ( debug ) {
      printf("(?) svc_bloader_PatchCRC: svc_makeFirmwareImage rc = %d.\n", rc );
    }
    return( rc );
  }

  tCrc = _bgp_Crc16n( JMB_INITIAL_CRC,
		      &(firmware->BeDRAM_Image[crc_start]),
		      crc_len );


  if ( verbose )
    printf("(I) svc_bloader_PatchCRC: CRC from 0x%08lx to 0x%08lx is 0x%04x\n",
	   firmware->crcStartAddress,
	   firmware->crcStopAddress,
	   tCrc );

  pCrc = (efi->mappedAddress + firmware->crcFilePos);

  *pCrc = htons( tCrc );

  return( _svc_error_none );
}



int svc_bloader_GetPersonality( Svc_Firmware_Info_T *firmware, Personality_t* personality ) {

  Svc_Elf_Info_T *elfInfo = &(firmware->elfInfo);
  Personality_t *elf_personality = (Personality_t *)(elfInfo->mappedAddress + firmware->personalityFilePosition);
  svc_pers_CopySwapCrc( elf_personality, personality );

  return 0;
}

int svc_bloader_SetPersonality( Svc_Firmware_Info_T *firmware, Personality_t* personality ) {
  Svc_Elf_Info_T *elfInfo = &(firmware->elfInfo);
  Personality_t *elf_personality = (Personality_t *)(elfInfo->mappedAddress + firmware->personalityFilePosition);
  svc_pers_CopySwapCrc( personality, elf_personality );

  return 0;
}


// node_pers was already filled out and, if necessary, byte swapped.

int svc_bloader_PatchPersonality( Svc_Firmware_Info_T *firmware, Personality_t *node_pers ) {

  Svc_Elf_Info_T *elfInfo = &(firmware->elfInfo);

  Personality_t *elf_personality = (Personality_t *)(elfInfo->mappedAddress + firmware->personalityFilePosition);

  if ( debug )  {

    printf(
	   "(D) PatchPers: elf_personality = 0x%p, ema=0x%p, pfp=0x%08lx\n",
	   elf_personality,
	   elfInfo->mappedAddress,
	   firmware->personalityFilePosition
	   );
  }

  if ( ! elfInfo->imageIsLoaded || ! firmware->canBePatched ) {
    printf("(W) svc_bloader_PatchCRC: Image not loaded or cannot be personalized.\n");
    return( _svc_error_none );
  }

  memcpy( elf_personality, node_pers, sizeof(Personality_t) );

  return  _svc_error_none;
}


void svc_dumpPersonalityOffsets() {
  printf("Personality Offset Information:\n");
  printf("  %04lx : %s\n", offsetof(Personality_t,CRC),                  "CRC");
  printf("  %04lx : %s\n", offsetof(Personality_t,Version),              "Version");
  printf("  %04lx : %s\n", offsetof(Personality_t,PersonalitySizeWords), "PersonalitySizeWords");
  printf("  %04lx : %s\n", offsetof(Personality_t,Kernel_Config),        "Kernel_Config");
  printf("  %04lx : %s\n", offsetof(Personality_t,DDR_Config),           "DDR_Config");
  printf("  %04lx : %s\n", offsetof(Personality_t,Network_Config),       "Network_Config");
  printf("  %04lx : %s\n", offsetof(Personality_t,Ethernet_Config),      "Ethernet_Config");
}

int svc_doFirmware( Svc_Firmware_Info_T   *firmware,
                       char                *bl_in_fn,
                       char                *bl_out_fn )
{
  int rc;
  int i;
  Personality_t personality;

  if ( debug ) {
    svc_dumpPersonalityOffsets();
  }

  if ( ( rc = svc_loadFirmware( firmware, bl_in_fn, bl_out_fn ) ) ) {
    fprintf(stderr,"(E) Non-zero return code (%d) from svc_loadFirmware ... aborting.\n", rc);
    return rc;
  }

  if ( debug ) {
    printf("(D) BootLoader loaded from %s.\n", bl_in_fn );
  }


  /*
   * Fetch the personality from the elf image, apply all options & switches, and
   * then put it back
   */

  svc_bloader_GetPersonality( firmware, &personality);


  for (i = 0; i < svc_num_options; i++) {

    if (debug) {
      printf("(D) apply option \"%s\"\n", svc_options[i]);
    }

    if ( (rc = svc_set_pers( svc_options[i], &personality)) != 0 ) {
      return rc;
    }
  }

  if (verbose) {
    svc_ShowPersonality(&personality);
  }

  svc_bloader_SetPersonality( firmware, &personality);
  
  if ( (rc = svc_bloader_PatchCRC( firmware )) ) {
    return rc;
  }

  // done with the bootloader elf

  return svc_unloadElfFile( &(firmware->elfInfo) );
}


Personality_t _PERS_; // Used to compute sizes below

#define FREQ_HELP     "Sets the core clock frequency (in megaherz)."
#define COORD_HELP    "Sets the associated 5-D torus coordinate."
#define DIM_HELP      "Sets the associated 5-D torus dimension."
//#define I_HELP      "Sets the number of I/O nodes in the partition."
#define DDR_SIZE_HELP "Sets the DDR size (in MB)."
#define THROTTLE_HELP "Sets the DDR throttling value."


Svc_FieldEditor_T svc_field_editors[] = {
  { "Frequency=",     offsetof(Personality_t,Kernel_Config.FreqMHz),     sizeof(_PERS_.Kernel_Config.FreqMHz),    FREQ_HELP,       100,  3000 },
  { "Acoord=",        offsetof(Personality_t,Network_Config.Acoord),     sizeof(_PERS_.Network_Config.Acoord),    COORD_HELP,        0,   255 },
  { "Bcoord=",        offsetof(Personality_t,Network_Config.Bcoord),     sizeof(_PERS_.Network_Config.Bcoord),    COORD_HELP,        0,   255 },
  { "Ccoord=",        offsetof(Personality_t,Network_Config.Ccoord),     sizeof(_PERS_.Network_Config.Ccoord),    COORD_HELP,        0,   255 },
  { "Dcoord=",        offsetof(Personality_t,Network_Config.Dcoord),     sizeof(_PERS_.Network_Config.Dcoord),    COORD_HELP,        0,   255 },
  { "Ecoord=",        offsetof(Personality_t,Network_Config.Ecoord),     sizeof(_PERS_.Network_Config.Ecoord),    COORD_HELP,        0,   255 },
  { "Anodes=",        offsetof(Personality_t,Network_Config.Anodes),     sizeof(_PERS_.Network_Config.Anodes),    DIM_HELP,          0,   255 },
  { "Bnodes=",        offsetof(Personality_t,Network_Config.Bnodes),     sizeof(_PERS_.Network_Config.Bnodes),    DIM_HELP,          0,   255 },
  { "Cnodes=",        offsetof(Personality_t,Network_Config.Cnodes),     sizeof(_PERS_.Network_Config.Cnodes),    DIM_HELP,          0,   255 },
  { "Dnodes=",        offsetof(Personality_t,Network_Config.Dnodes),     sizeof(_PERS_.Network_Config.Dnodes),    DIM_HELP,          0,   255 },
  { "Enodes=",        offsetof(Personality_t,Network_Config.Enodes),     sizeof(_PERS_.Network_Config.Enodes),    DIM_HELP,          0,   255 },
  { "cnBridge_A=",    offsetof(Personality_t,Network_Config.cnBridge_A), sizeof(_PERS_.Network_Config.cnBridge_A),COORD_HELP,        0,   255 },
  { "cnBridge_B=",    offsetof(Personality_t,Network_Config.cnBridge_B), sizeof(_PERS_.Network_Config.cnBridge_B),COORD_HELP,        0,   255 },
  { "cnBridge_C=",    offsetof(Personality_t,Network_Config.cnBridge_C), sizeof(_PERS_.Network_Config.cnBridge_C),COORD_HELP,        0,   255 },
  { "cnBridge_D=",    offsetof(Personality_t,Network_Config.cnBridge_D), sizeof(_PERS_.Network_Config.cnBridge_D),COORD_HELP,        0,   255 },
  { "cnBridge_E=",    offsetof(Personality_t,Network_Config.cnBridge_E), sizeof(_PERS_.Network_Config.cnBridge_E),COORD_HELP,        0,   255 },
  //{ "I=",             offsetof(Personality_t,Network_Config.IOnodes),    sizeof(_PERS_.Network_Config.IOnodes),   I_HELP,            0,  2048 }, //! @todo obsolete?
  //{ "IONodeNum=",     offsetof(Personality_t,Network_Config.IOnodeRank), sizeof(_PERS_.Network_Config.IOnodeRank),   0,  2048 }, //! @todo obsolete?
  { "DDRSize=",       offsetof(Personality_t,DDR_Config.DDRSizeMB),      sizeof(_PERS_.DDR_Config.DDRSizeMB),     DDR_SIZE_HELP,     0,  32 * 1024 },
};

int svc_edit_field( char* p, Personality_t* personality ) {

  int i;
  int rc;
  Svc_FieldEditor_T* ed;

  for ( i = 0, ed = svc_field_editors; i < sizeof(svc_field_editors)/sizeof(svc_field_editors[0]); i++, ed++ ) {
    
    if ( ! strncasecmp( p, ed->id, strlen(ed->id) ) ) {
      
      switch ( ed->width ) {

      case sizeof(uint8_t) : {
	uint8_t* ptr = (uint8_t*)( (char*)personality + ed->offset );
	rc = svc_getInt8( p + strlen(ed->id), ptr,  ed->lower_bound, ed->upper_bound, "Missing Value", "Out of Range");
	break;
      }

      case sizeof(uint16_t) : {
	uint16_t* ptr = (uint16_t*)( (char*)personality + ed->offset );
	rc = svc_getInt16( p + strlen(ed->id), ptr,  ed->lower_bound, ed->upper_bound, "Missing Value", "Out of Range");
	break;
      }

      case sizeof(uint32_t) : {
	uint32_t* ptr = (uint32_t*)( (char*)personality + ed->offset );
	rc = svc_getInt32( p + strlen(ed->id), ptr,  ed->lower_bound, ed->upper_bound, "Missing Value", "Out of Range");
	break;
      }

      case sizeof(uint64_t) : {
	uint64_t* ptr = (uint64_t*)( (char*)personality + ed->offset );
	rc = svc_GetInt64( p + strlen(ed->id), ptr,  ed->lower_bound, ed->upper_bound, "Missing Value", "Out of Range");
	break;
      }

      default: {
	rc =  -1;
	break;
      }

      }

      if (rc != 0) {
	fprintf(stderr,"(E) Error processing \"%s\"\n", p);
	return -1;
      }

      return 0;
    }
  }

  return 1; // not found
}

Svc_IPAddrEditor_T svc_ip_editors[] = {
};

int svc_edit_ip_address( char* p, Personality_t* personality ) {

  int i;
  int rc;
  Svc_IPAddrEditor_T* ed;

  for ( i = 0, ed = svc_ip_editors; i < sizeof(svc_ip_editors)/sizeof(svc_ip_editors[0]); i++, ed++ ) {
    
    if ( ! strncasecmp( p, ed->id, strlen(ed->id) ) ) {

      IP_Addr_t* ip = (IP_Addr_t*)( (void*)personality + ed->offset );
      rc = svc_GetIP( p + strlen(ed->id), ip, "Bad or missing IP address" );

      if ( rc != 0 ) {
	fprintf(stderr,"(E) error setting IP address \"%s\"\n", ed->id);
	return -1;
      }

      return 0;
    }
  }

  return 1; // not found
}

int svc_edit_ras_verbosity(char* source, Personality_t* personality) {
  return find_ras_verbosity( source, &(personality->Kernel_Config.RASPolicy), "Missing or invalid RAS Verbosity type" );
}

int svc_edit_ddr_powerdwn(char* source, Personality_t* personality) {
    return find_ddr_power_down( source, &(personality->DDR_Config), "Missing or invalid DDR Power Down type" );
}


int svc_edit_binary(char* source, Personality_t* personality) {
  uint32_t offset;
  uint32_t value;
       
  if ( sscanf( source, "%x=0x%x", &offset, &value ) != 1 ) {
    return svc_Usage("Missing or bad value for personality offset", NULL );
  }
  
  if( (offset+4) >= sizeof(Personality_t) ) {
    return svc_Usage("Personality offset out of range", NULL );
  }
      
  *((uint32_t*)(((char*)personality) + offset)) = value;

  return 0;
}

int svc_edit_entry_point(char* source, Personality_t* personality) {


    /*
     * Entry point editing is not supported in the "direct" mode
     */

    if ( filename_bl_elf[0] == 0 ) {
	fprintf(stderr, "(W) Entry point setting is being ignored.\n");
	return 0;
    }

  unsigned long ep;

  if ( svc_getHexLong(source, &ep) == 0 ) {

    unsigned long 
      ep_vaddr = 0, 
      ep_size = 0,
      ep_filepos = 0;

#if 0
    svc_getElfSymbolByName( &(svc_firmware.elfInfo), "fw_mailbox_launch_address", &ep_vaddr, &ep_size, &ep_filepos );
    unsigned long* ptr = (unsigned long*)(svc_firmware.elfInfo.mappedAddress + ep_filepos );
#endif

    // NOTE: the entry point is now hosted in the domain structure.

    if ( svc_getElfSymbolByName( &(svc_firmware.elfInfo), "FW_InternalState", &ep_vaddr, &ep_size, &ep_filepos ) != 0 ) {
      printf("(E) could not locate Firmware internal state.\n");
      return -1;
    }


    FW_InternalState_t* state = (FW_InternalState_t*)(svc_firmware.elfInfo.mappedAddress + ep_filepos );

    state->nodeState.domain[0].entryPoint = swap64(ep);

    printf("Entry point set to %lX\n", ep );


    return 0;
  }

  printf("BLUGH\n");
  return -1;
}


int svc_parse_domain_index(char** q) {

  char* orig = *q;

  if ( **q == '[' ) {

    (*q)++;

    int value = **q - '0';

    if ( (value >= 0) && (value <= 1) ) {

      (*q)++;

      if ( **q == ']' ) {
	(*q)++;
	return value;
      }
    }
  }

  fprintf(stderr, "(E) Invalid domain dimension specified: %s\n", orig );
  return -1;
}

int svc_edit_domain(char* source, Personality_t* personality) {

    /*
     * Domain editing is not supported in the "direct" mode
     */

    if ( filename_bl_elf[0] == 0 ) {
	fprintf(stderr, "(W) Domain specification data is being ignored.\n");
	return 0;
    }

  int domain = svc_parse_domain_index( &source );

  if ( domain >= 0 ) {

    unsigned long 
      ep_vaddr = 0, 
      ep_size = 0,
      ep_filepos = 0;

    if ( svc_getElfSymbolByName( &(svc_firmware.elfInfo), "FW_InternalState", &ep_vaddr, &ep_size, &ep_filepos ) != 0 ) {
      printf("(E) could not locate Firmware internal state.\n");
      return -1;
    }


    FW_InternalState_t* state = (FW_InternalState_t*)(svc_firmware.elfInfo.mappedAddress + ep_filepos );

    if ( strncmp( source, ".cores", strlen(".cores") ) == 0 ) {

      source += strlen(".cores");

      unsigned mask = 0;

      if ( svc_getHex(source,&mask) != 0 ) {
	printf("(E) illegal core mask\n");
	return -1;
      }

      state->nodeState.domain[domain].coreMask = swap32(mask);
    }
    else if ( strncmp( source, ".ddrEnd=", strlen(".ddrEnd=") ) == 0 ) {

      source += strlen(".ddrEnd=");

      uint64_t ddrEnd;

      if ( svc_getHexLong( source, &ddrEnd ) != 0 ) {
	printf("(E) illegal DDR end\n");
	return -1;
      }

      state->nodeState.domain[domain].ddrEnd = swap64(ddrEnd);
    }
    else if ( strncmp( source, ".entry", strlen(".entry") ) == 0 ) {

      source += strlen(".entry");

      uint64_t entry;

      if ( svc_getHexLong(source, &entry) != 0 ) {
	printf("(E) illegal entry point\n");
	return -1;
      }

      state->nodeState.domain[domain].entryPoint = swap64(entry);
    }
    else if ( strncmp( source, ".ddrOrigin", strlen(".ddrOrigin") ) == 0 ) {

      source += strlen(".ddrOrigin");

      uint64_t ddrOrigin;

      if ( svc_getHexLong(source, &ddrOrigin) != 0 ) {
	printf("(E) illegal DDR origin address\n");
	return -1;
      }

      state->nodeState.domain[domain].ddrOrigin = swap64(ddrOrigin);
    }
  }

  return 0;
}

uint8_t svc_getHexChar(char* source) {

  //printf("(D) %s source=\"%s\"\n", __func__, source);

  uint8_t result = 0;
  unsigned i;

  for (i = 0; i < 2; i++) {
    result <<= 4;
    
    if (( source[i] >= '0') && (source[i] <= '9')) {
      result |= (source[i] - '0');
    }
    else if ( (source[i] >= 'a') && (source[i] <= 'f') ) {
      result |= (source[i] - 'a' + 10);
    }
    else if ( (source[i] >= 'A') && (source[i] <= 'F') ) {
      result |= (source[i] - 'A' + 10);
    }
    else {
      printf("(E) illegal hex digit -> \"%c\"\n", source[i]);
    }
  }
  return result;
  
}

int svc_edit_clockstop(char* source, Personality_t* personality) {

  unsigned long clockstop = 0;

  if ( svc_getHexLong(source, &clockstop) == 0 ) {
    personality->Kernel_Config.ClockStop = clockstop;
    return 0;
  }

  return -1;
}

int svc_edit_uci(char* source, Personality_t* personality) {
  uint64_t uci;

  if ( (source[0] == '=') && bg_string_to_uci(source+1, &uci) == 0 ) {
    // OK
  }
  else if ( svc_getHexLong(source, &uci ) != 0 ) {
    printf("(E) illegal UCI value.\n");
    return -1;
  }
  personality->Kernel_Config.UCI = uci;
  return 0;
}


int svc_show_pers(char* source, Personality_t* personality) {
  svc_ShowPersonality(personality);
  return 0;
}

#define RAS_VERBOSITY_HELP  "-RASVerbosity=string         : Specify one of: Minimal, Normal, Verbose, Extreme."
#define CLOCKSTOP_HELP      "-ClockStop=0xValue           : Specify a clock stop value (hexadecimal)."
#define MAC_HELP            "-MAC=xx:xx:xx:xx:xx:xx       : Sets the MAC address of the ethernet adapter."
#define NFS_HELP            "-NFSExportDir=path           : Sets the path name of the exported NFS."
#define BINARY_EDIT_HELP    "-Offset0xXX=0xYY             : Performs a 32-bit binary edit of the word at offset XX (sets that word to value YY)."
#define ENTRY_POINT_HELP    "-EntryPoint=0xXXXXXX         : Sets the entry point for the kernel.  This setting is potentially overridden by control system"
#define DOMAIN_HELP         "-Domain[n].cores=0xXXXX      : Sets the core mask for the specified domain.\n" \
                          "  -Domain[n].ddrOrigin=0xXXXX  : Specifies the base (physical) address of the specified domain's DDR region\n"  \
                          "  -Domain[n].ddrEnd=0xXXXX     : Specifies the end address of DDR in the specified domain.\n" \
                          "  -Domain[n].entry=0xXXXX      : Specifies the entry point of the specified domain."
#define INTERFACE_HELP      "-Interface[n]=name           : Sets the name (string) of the network interface."
#define IFID_HELP           "-IFID[n]=xxxxxxxxxxxx        : Sets the interface ID."
#define UCI_HELP            "-UCI=0xXXXXXXXXXXXXXXXX      : Sets the Universal Component Identifier"
#define DDR_POWER_DOWN_HELP "-DDRPowerDown={Fast,Slow,Off}"

Svc_SimpleEditor_T svc_assorted_editors[] = {
  { svc_edit_ras_verbosity, "RASVerbosity=", RAS_VERBOSITY_HELP },
  { svc_edit_clockstop,     "ClockStop",     CLOCKSTOP_HELP },
  { svc_edit_binary,        "Offset0x",      BINARY_EDIT_HELP },
  { svc_edit_entry_point,   "EntryPoint",    ENTRY_POINT_HELP },
  { svc_edit_domain,        "Domain",        DOMAIN_HELP },
  { svc_edit_uci,           "UCI",           UCI_HELP },
  { svc_edit_ddr_powerdwn,  "DDRPowerDown=", DDR_POWER_DOWN_HELP },
  { svc_show_pers,          "Show",          0 },
};

int svc_edit_assorted(char* p, Personality_t* personality) {
  
  int i;
  Svc_SimpleEditor_T* ed = svc_assorted_editors;
  for (i = 0; i < sizeof(svc_assorted_editors) / sizeof(svc_assorted_editors[0]); i++, ed++) {
	 
    //printf("?%s\n?%s\n-->%d\n", p, ed->id, strncasecmp( p, ed->id, strlen(ed->id)));
    if ( ! strncasecmp( p, ed->id, strlen(ed->id) ) ) {
      return ed->handler( p + strlen(ed->id), personality );
    }
  }
  return 1; // not found
}


char* svc_legacy_options[] = {

  // -------------------------------------------------------------------------------
  // These are options from BG/P that we no longer support.  This tool will issue
  // warnings (not errors) when these options are encountered.
  // -------------------------------------------------------------------------------

  "BlindStacks",
  "CNKMalloc",
  "Collective",
  "Core",     // CoreN
  "DDRChips=",
  "DDRScrub",
  "DMA",
  "Environment=",
  "I=",
  "L1D",
  "L1I",
  "L1Recovery",
  "L1SWOA",
  "L1WriteThru",
  "L2Bug824Lumpy",
  "L2Bug824Smooth",
  "L2D",
  "L2I",
  "L2PF",
  "L3BankSelect=",
  "L3CacheSelect=",
  "L3D",
  "L3I",
  "L3PFI",
  "L3Scramble",
  "L3PreFetchBy2",
  "NonCoherentStacks",
  "LockBox",
  "Process=",
  "ReservedMemory=",
  "Scratch=",
  "Snoop",
  "Standalone",
  "Timestamps",
  "TLB=",
  "TLBMisses",
  "TraceTorus",
  "TraceTree",
  //"Torus",
  "Verbose", // we dont support +Verbose from inside a .svchost file
  "WatchDog",
  "X=",
  "Y=",
  "Z=",
  "XM=",
  "YM=",
  "ZM=",
  "Xcoord=",
  "Ycoord=",
  "Zcoord=",
  "-",
};

int svc_set_pers( char *p, Personality_t *pers ) {

  int i;
  char *q;
  int flag_enable = 0;

  if (debug)
    printf("(D) %s p=\"%s\"\n", __func__, p );

  q = p;

  if ( *p == '+' ) {
    flag_enable = 1;
  }
  else if ( *p == '-' ) {
    flag_enable = 0;
  }
  else {
    return svc_Usage("Unknown argument.", p );
  }

  p++;

  if ( svc_edit_controls( p, pers, flag_enable ) == 0 ) {
    return 0;
  }

  if ( svc_edit_field( p, pers ) == 0 ) {
    return 0;
  }

  if ( svc_edit_netflags( p, pers, flag_enable ) == 0 ) {
    return 0;
  }

  if ( svc_edit_classroutes( p, pers, flag_enable ) == 0 ) {
    return 0;
  }

  if ( svc_edit_ip_address( p, pers ) == 0 ) {
    return 0;
  }

  if ( svc_edit_assorted( p, pers ) == 0 ) {
    return 0;
  }

  // Is it an old option?  
   
  for (i = 0; i < sizeof(svc_legacy_options)/sizeof(svc_legacy_options[0]); i++) {
    if ( ! strncasecmp( p, svc_legacy_options[i], strlen(svc_legacy_options[i]) ) ) {
      printf("(W) Option \"%c%s\" is no longer supported and is ignored.\n", (flag_enable ? '+' : '-'), p );
      return 0;
    }
  }
   

  return svc_Usage("Unrecognized or illegal option: ", p );
}

int svc_help_handler(char* not_used, char* also_not_used, int foo, char* bar) {
  svc_help();
  return _svc_error_help;
}

int svc_debug_handler(char* not_used, char* also_not_used, int foo, char* bar) {
  debug++;
  return 0;
}

int svc_verbose_handler(char* not_used, char* also_not_used, int foo, char* bar) {
  verbose++;
  return 0;
}


int svc_StoreOption(char* source, char* na0, int na1, char* na2) {

  if (svc_num_options >= SVC_MAX_OPTIONS) {
    fprintf(stderr,"(E) too many arguments ... ignoring \"%s\"\n", source);
    return -1;
  }

  if ( (strlen(source)+1) >= SVC_MAX_OPTION_LENGTH) {
    fprintf(stderr,"(E) option \"%s\" is too long ... ignoring\n", source);
    return -1;
  }

  if (debug) {
    printf("(D) option[%d] = \"%s\"\n", svc_num_options, source);
  }

  strcpy(svc_options[svc_num_options++], source);
  return 0;
}


int svc_ArgFile(char* file,  char* na1, int na2, char* na3) {

  int rc = 0;
  char buff[1024];
  char* savePtr = 0;

  if (debug) 
      printf("(D) opening svchost file %s ...\n", file);

  FILE* fp = fopen(file, "r");

  if ( fp == 0 ) {
    fprintf(stderr,"(E) couldnt open argument file \"%s\"\n", file);
    return -1;
  }


  int lineNo = 1;

  while ( fgets(buff, sizeof(buff), fp) > 0 ) {

      char* buffPointer = buff;
      char* token;

      if ( debug ) {
	  printf("(D) %d : %s", lineNo, buff);
      }

      while ( ( token = strtok_r( buffPointer, " \t\n\r", &savePtr ) ) != 0 ) {

	  if ( token[0] == '#' ) {
	      break;
	  }
	  
	  if ( debug ) {
	      printf("(D) %d :   > %s\n", lineNo, token);
	  }

	  rc = svc_StoreOption(token, 0, 0, 0);
	  buffPointer = 0;
      }

      lineNo++;
  }

  fclose(fp);

  return rc;
}

int svc_NotSupported(char* source,  char* option, int rc, char* na3) {
  if (rc == 0) {
    printf("(W) The option \"%s%s\" is no longer supported and will be ignored.\n", option, source);
  }
  else {
    //fprintf(stderr,"(E) The option \"%s%s\" is no longer supported.  Aborting.\n", option, source);
    char buff[256];
    sprintf(buff, "%s%s", option, source);
    svc_Usage("Unsupported option from BG/P : ", buff);
  }
  return rc;
}


#define HELP_HELP_MSG       " : Print this help message."
#define EXAMPLE_ERR_MSG     "Missing Example filename."
#define EXAMPLE_HELP_MSG    "filename : Generate an example svchost file."
#define DEBUG_HELP_MSG      " : Enable debugging output for this utility."
#define VERBOSE_HELP_MSG    " : Enable verbose output for this utility."
#define FW_ERR_MSG          "Missing BootLoader file name"
#define FW_HELP_MSG         "filename : Specifies the path of the bootloader ELF file (usually required)."
#define FW_PATCHED_ERR_MSG  "Missing PatchedBootLoader file name."
#define FW_PATCHED_HELP_MSG "filename : Specifies the path of the patched bootloader ELF file (output)."
#define PERS_ERR_MSG        "Missing Personality file name."
#define PERS_HELP_MSG       "filename : Specifies the path of the personality stem."
#define ARGFILE_HELP_MSG    "filename : Specifies the path of a file containing options (i.e. an .svchost file)."
#define SHOW_HELP_MSG       " : Dumps the personality to stdout."


Svc_HostArg_T svc_host_args[] = {
  { svc_help_handler,       "-Help",               0,                   0,                     0,                  HELP_HELP_MSG       },
  { svc_getString,          "-Example=",           filename_example,    SVC_MAX_FILENAME_LEN,  EXAMPLE_ERR_MSG,    EXAMPLE_HELP_MSG    },
  { svc_NotSupported,       "-Interpret=",         "-Interpret=",       -1,                    0,                  0                   },
  { svc_debug_handler,      "-Debug",              0,                   0,                     0,                  DEBUG_HELP_MSG      },
  { svc_verbose_handler,    "-Verbose",            0,                   0,                     0,                  VERBOSE_HELP_MSG    },
  { svc_getString,          "-BootLoader=",        filename_bl_elf,     SVC_MAX_FILENAME_LEN,  FW_ERR_MSG,         FW_HELP_MSG         },
  { svc_getString,          "-PatchedBootLoader=", filename_bl_patched, SVC_MAX_FILENAME_LEN,  FW_PATCHED_ERR_MSG, FW_PATCHED_HELP_MSG },
  { svc_ArgFile,            "-ArgFile=",           0,                   0,                     0,                  ARGFILE_HELP_MSG    },
  { svc_StoreOption,        "",                    0,                   0,                     0,                  0                   } // This must be LAST!
};
    



int svc_host( int argc, char *argv[], char **envp ) {

  int   rc = 0;
  int   i, j;
  char* p;

  printf("(I) %s\n", svc_Herald );   // Identify this utility

  for (j = 1; j < argc; j++) {

    p = argv[j];

    if ( debug ) {
      printf("(D) program argument-> \"%s\"\n", p );
    }

    int found = 0;

    for ( i = 0; (i < sizeof(svc_host_args)/sizeof(svc_host_args[0])) && (found==0); i++) {
      if ( ! strncasecmp( p, svc_host_args[i].id, strlen(svc_host_args[i].id) ) ) {
	rc = svc_host_args[i].handler( 
				      p + strlen(svc_host_args[i].id),
				      svc_host_args[i].target,
				      svc_host_args[i].maximum_length,
				      svc_host_args[i].error_message
				      );
	found = 1;
      }
    }

  if ( debug ) {
    printf("(D) sizeof(Personality_t) = %ld\n", sizeof(Personality_t) );
    printf("(D) sizeof(fw_uint64_t)   = %ld\n", sizeof(fw_uint64_t));
    printf("(D) sizeof(fw_uint32_t)   = %ld\n", sizeof(fw_uint32_t));
    printf("(D) sizeof(fw_uint164_t)  = %ld\n", sizeof(fw_uint16_t));
    printf("(D) sizeof(fw_uint8_t)    = %ld\n", sizeof(fw_uint8_t));
  }

  if ( sizeof(fw_uint64_t) != 8 ) {
    printf("(E) sizeof(fw_uint64_t)=%ld (should be 8)\n", sizeof(fw_uint64_t));
    return -1;
  }
  if ( sizeof(fw_uint32_t) != 4 ) {
    printf("(E) sizeof(fw_uint32_t)=%ld (should be 4)\n", sizeof(fw_uint32_t));
    return -1;
  }
  if ( sizeof(fw_uint16_t) != 2 ) {
    printf("(E) sizeof(fw_uint16_t)=%ld (should be 2)\n", sizeof(fw_uint16_t));
    return -1;
  }
  if ( sizeof(fw_uint8_t) != 1 ) {
    printf("(E) sizeof(fw_uint8_t)=%ld (should be 1)\n", sizeof(fw_uint8_t));
    return -1;
  }

  if ( rc != 0 ) {
      return rc;  
  }
  } // rof argc

  if ( filename_example[0] ) {
    return svc_Example( filename_example );
  }

  if ( !filename_bl_elf[0] ) {
    return svc_Usage("Missing required bootloader elf filename", NULL );
  }

  if ( !filename_bl_patched[0] ) {
    printf("(W) patched bootloader not specified.\n");
  }

  rc = svc_doFirmware( &svc_firmware,
			  filename_bl_elf,
			  filename_bl_patched
			  );

  return rc;
}


void svc_help( void ) {

  char buff[256];
  int i;

  printf( "%s\n", svc_Herald );
  printf( "\n" );
  printf( " Usage: svc_host [controls] [flags]\n");


  printf( "\n" );
  printf( " Flags Controlling this program:\n" );
  for (i = 0; i < sizeof(svc_host_args)/sizeof(svc_host_args[0]); i++) {
    if ( strlen(svc_host_args[i].id) > 0 ) {
      printf("  %s%s\n", svc_host_args[i].id, svc_host_args[i].help_message);
    }
  }
  printf( "\n" );


  printf( " Node controls: Use \'+\' to enable an option and \'-\' to disable an option.\n" );
  printf( "            E.g., \"+Mailbox\" to enable the mailbox.\n" );

  for (i = 0; NodeConfigControls[i].name != 0; i++) {
    printf("  [+|-]%-16s : %s\n", NodeConfigControls[i].name, NodeConfigControls[i].help );
  }
  printf( "\n" );

  printf( " Trace controls:  Use \'+\' to enable a trace option and \'-\' to disable it.\n" );
  for (i = 0; TraceControls[i].name; i++ ) {
    printf("  [+|-]%-16s : %s\n", TraceControls[i].name, TraceControls[i].help);
  }
  printf("\n");

  printf(" Assorted fields: These are specified using \'-name=value\'\n");
  for (i = 0; i < sizeof(svc_field_editors) / sizeof(svc_field_editors[0]); i++) {
    sprintf(buff, "%svalue", svc_field_editors[i].id);
    printf("  -%-18s : %s (range = [%d,%d])\n", buff, svc_field_editors[i].help, svc_field_editors[i].lower_bound, svc_field_editors[i].upper_bound);
  }
  printf("\n");

  printf(" Network fields and flags.  Many of these are indexed by network dimension, which may be\n");
  printf(" expressed symbolically (e.g. +Torus(A), +Torus(B), etc.) or numerically (e.g. +Torus(0),\n");
  printf(" +Torus(1), etc.).  Most dimensioned fields are 5-D for BGQ and hence accept 0..4 or A..E\n");
  printf(" as valid dimension indices.  However a few (such as Loopback) have an additional IO\n");
  printf(" dimension, which may be specified as 'I' or '5'.\n");
  
  for (i = 0; i < sizeof(Netflags)/sizeof(Netflags[0]); i++) {
    if ( Netflags[i].id ) {
      sprintf(buff, "%s%s%s", Netflags[i].help_prefix, Netflags[i].id, Netflags[i].help_suffix);
      printf("  %-24s : %s\n", buff, Netflags[i].help_message);
    }
  }
  printf("\n");

  printf(" Class route fields. \n");
  for (i = 0; i < sizeof(ClassRouteEditors)/sizeof(ClassRouteEditors[0]); i++) {
    if ( ClassRouteEditors[i].id ) {
      sprintf(buff, "%s%s%s", ClassRouteEditors[i].help_prefix, ClassRouteEditors[i].id, ClassRouteEditors[i].help_suffix);
      printf("  %-24s : %s\n", buff, ClassRouteEditors[i].help_message);
    }
  }
  printf("\n");

  printf(" Assorted IP address fields: These are specified using \'-name=a.b.c.d\'\n");
  for (i = 0; i < sizeof(svc_ip_editors) / sizeof(svc_ip_editors[0]); i++) {
    sprintf(buff, "%sa.b.c.d", svc_ip_editors[i].id);
    printf("  -%-20s : %s\n", buff, svc_ip_editors[i].help );
  }
  printf("\n");

  printf( " Various and sundry options:\n");
  for (i = 0; i < sizeof(svc_assorted_editors) / sizeof(svc_assorted_editors[0]); i++ ) {
    printf("  %s\n", svc_assorted_editors[i].help_message);
  }
  printf( "\n" );
  printf( "\n" );
}

