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
/* -------------------------------------------------------
 *
 * Licensed Materials - Property of IBM.
 * (C) Copyright IBM Corporation 2001, 2008
 * All Rights Reserved.
 *
 * US Government Users Restricted Rights -
 * Use, duplication or disclosure restricted by
 * GSA ADP Schedule Contract with IBM Corporation.
 *
 * Filename : sim_support_code.h
 *
 * Purpose : This file is shared between Mambo and OSes (Linux, K42).
 *
 * ------------------------------------------------------- */

/* Note: Do not include mambo.h here because this file is also used as is by the simulated program */

#ifndef _SIM_SUPPORT_CODE_H_
#define _SIM_SUPPORT_CODE_H_

#include <time.h>


struct CallthruCacheConfig
{
    int dcache_size;                /* bytes */
    int dcache_assoc;
    int dcache_line_size;           /* bytes */
    int dcache_block_size;          /* bytes */

    int icache_size;                /* bytes */
    int icache_assoc;
    int icache_line_size;           /* bytes */
    int icache_block_size;          /* bytes */

    int l2cache_size;               /* bytes */
    int l2cache_assoc;
    int l2cache_line_size;          /* bytes */

    int l3cache_size;               /* bytes */
    int l3cache_assoc;
    int l3cache_line_size;          /* bytes */
};
typedef struct CallthruCacheConfig CallthruCacheConfig;

struct CallthruConfig
{
    int         num_cpus;
    int         cpu_frequency;     /* in Mhz units */
    unsigned long long      mem_size;       /* bytes */
    CallthruCacheConfig cache;
};
typedef struct CallthruConfig CallthruConfig;

enum MachineAttr {
    SimNumbPhysCpusK = 0,
    SimMemorySizeK = 1
};
typedef enum MachineAttr MachineAttr;

/* trace file control codes */
enum TraceFileCtl {
    SimTraceFileCtlOpenK = 1,
    SimTraceFileCtlWriteK = 2,
    SimTraceFileCtlCloseK = 3
};
typedef enum TraceFileCtl TraceFileCtl;

/*
 * Small integers identify type of device to simulator
 */
enum SimDeviceID {
    SimConsoleDeviceID = 0,
    SimDiskDeviceID,
    SimEtherDeviceID,
    SimDeviceIDCount         /* count of valid IDs */
};
typedef enum SimDeviceID SimDeviceID;

struct sim_stats
{
    unsigned long long total_instructions;
    unsigned long long delta_instructions;
    unsigned long long delta_seconds;
    unsigned long long host_time;
    unsigned long long current_cycle;
};
typedef struct sim_stats sim_stats;


enum bogus_disk_operation {
    bd_info_sync   = 0,
    bd_info_status,
    bd_info_blksz,
    bd_info_devsz,
    bd_info_change
};
typedef enum bogus_disk_operation bogus_disk_operation;

/*
 * Support functions that can be invoked by the simulated program
 *
 * Simulated reg 3 contains code indicating which function to perform: */
enum SimSupportCode {
    SimWriteConsoleCode = 0,         /*  0 */
    SimDiskReadCode,                 /*  1 */
    SimDiskWriteCode,                /*  2 */
    SimDeviceIntRegisterCode,        /*  3 */
    SimDeviceIntDeregisterCode,      /*  4 */
    SimDeviceEnableIntCode,          /*  5 */
    SimDeviceDisableIntCode,         /*  6 */
    SimGetIntStreamForDeviceCode,    /*  7 */
    SimResetIntForStreamCode,        /*  8 */
    SimCharReadCode,                 /*  9 */
    SimCharWriteCode,                /* 10 */
    SimGetInterruptingLevelCode,     /* 11 */
    SimSetExternalLevelMaskCode,     /* 12 */
    SimSetInterruptCode,             /* 13 */
    SimMapLevelCode,                 /* 14 */
    SimResetInterruptCode,           /* 15 */
    SimFileOpenCode,                 /* 16 */
    SimFileCloseCode,                /* 17 */
    SimFileReadCode,                 /* 18 */
    SimFileWriteCode,                /* 19 */
    SimTakeCheckpointCode,           /* 20 */
    SimInterruptEOICode,             /* 21 */
    SimShellCode,                    /* 22 */
    SimBlockIOCode,                  /* 23 */
    SimGetCookieCode,                /* 24 */
    SimDumpStatsCode,                /* 25 */
    SimAnnotateStatsCode,            /* 26 */
    SimReturnStatsCode = 30,         /* 30 */
    SimExitCode,                     /* 31 */
    SimDumpPPCStatsCode = 32,        /* 32 */ /* used in cyclesim + ??? */
    SimClearPPCStatsCode = 33,       /* 33 */ /* used in cyclesim + ??? */
    SimCopyQuickCode = 34,           /* 34 */ /* shortcut a long memory copy */
    /* 35 */ /* bmark under test for power validation in cyclesim */
    SimSetTestNameCode = 35,

    SimDumpSystemStatsCode = 36,           /* 36 */
    SimClearSystemStatsCode = 37,          /* 37 */
    SimSystemMemTraceEnableCode = 38,      /* 38 */
    SimSystemMemTraceDisableCode = 39,     /* 39 */

    SimTCLReadCode = 40,                /* 40 */
    SimTCLWriteCode,                    /* 41 */
    SimGetSimInfo = 42,                 /* 42 */
    SimStatsCreateTagStackCode,         /* 43 */
    SimStatsDeleteTagStackCode,         /* 44 */
    SimStatsDeleteTagPoolCode,          /* 45 */
    SimStatsPushTagCode,                /* 46 */
    SimStatsPopTagCode,                 /* 47 */
    SimStatsTOSTagCode,                 /* 48 */
    SimStatsGetTagStackHandleCode,      /* 49 */
    SimFileLoadedByAIXCode = 50,        /* 50 */
    SimUnloadedFileCode,                /* 51 */
    SimDumpStateCode,                   /* 52 */ /* get a machine attribute */
    /* extended file support  */
    SimFileOpenExtendedCode,            /* 53 */
    SimFileCloseExtendedCode,           /* 54 */
    SimFileReadExtendedCode,            /* 55 */
    SimFileWriteExtendedCode,           /* 56 */
    SimFileSeekCode,                    /* 57 file seek */
    SimFileEOFCode,                     /* 58 file eof */
    SimStatsGetTagStackDepthCode,       /* 59 */

    SimReadConsoleCode = 60,          /* 60 */ /* k42 codes starting at 60 */
    SimThinIPReadCode,                /* 61 */ /* thinIP read */
    SimThinIPWriteCode,               /* 62 */ /* thinIP write */
    SimFastRevMemCopyCode,            /* 63 */ /* fast rev mem cpy */
    SimStartCPUCode,                  /* 64 */ /* start a cpu */
    SimSendIPICode,                   /* 65 */ /* make an IPI */
    SimGetInstrCountCode,             /* 66 */ /* get instr count */
    SimGetNumbPhysProcsCode,          /* 67 */ /* get numb procs */
    SimGetMachAttrCode,               /* 68 */ /* get a machine attribute */
    SimPhysMemCopyCode,               /* 69 */ /* a fast phys to phys mem copy */
    SimGetTimeCode,                   /* 70 */ /* get a time */
    SimPhysMemSetCode,                /* 71 */ /* a fast memory set routine */
    SimTraceFileCtlCode,              /* 72 */ /* allows open, write, close */

    /* 80 */ /* read up to 4096 bytes from a simulator disk */
    SimDiskReadK42Code = 80,

    /* 81 */ /* write up to 4096 bytes to a simulator disk */
    SimDiskWriteK42Code,

    /* 82 */ /* open simulator disk; return disk size */
    SimDiskOpenK42Code,

    /* 83 */ /* close simulator disk; NOT YET IMPLEMENTED */
    SimDiskCloseK42Code,


    SimGetConfigCode,          /* 84 */ /* read the system configuration */
    /* 85 */ /* print out the stats with a comment and id */
    SimLogStatsCode,


    /* 86 */ /* execute the passed string as a tcl procedure */
    SimCallTCLCode,

    /* For supporting cache coloring - KS */
    SimSetColorCode = 90,
    SimGetColorCode = 91,
    SimInitColorCode = 92,
    SimDumpColorCode = 93,
    SimSetColorEACode = 94,
    SimGetColorEACode = 95,
    SimInitColorEACode = 96,
    SimDumpColorEACode = 97,

    SimPROMCode = 99,              /* 99 */ /* Handle open firmware calls */

    /* mco hooks to linux kernel process creation destruction routines */
    SimPIDCreateCode = 101,         /* 101 */ /* fork a proc */
    SimPIDExecExitCode,             /* 102 */ /* exit() */
    SimPIDExecCode,                 /* 103 */ /* execve() */
    SimPIDResumeCode,               /* 104 */ /* */
    SimPIDKillCode,                 /* 105 */ /* */
    SimKernelThreadCreateCode,      /* 106 */ /* kernel_thread() */
    SimKernelStartedCode,           /* 107 */ /* kernel finished booting */
    SimLinuxLoadShlibCode,          /* 108 */ /* linux load_elf_library() */

    SimOpenConnectionCode = 110,    /* 110 */ /* open connection code */
    SimCloseConnectionCode,         /* 111 */ /* close connection code */
    SimReadConnectionCode,          /* 112 */ /* read connection code */
    SimWriteConnectionCode,         /* 113 */ /* write connection code */
    SimSelectConnectionCode,        /* 114 */ /* select connection code */


    SimPthreadCreateCode,           /* 115 traps into simulator to create/start a pthread */

    SimBogusDiskReadCode,           /* 116 mambo bogus disk block read */
    SimBogusDiskWriteCode,          /* 117 mambo bogus disk block write */
    SimBogusDiskInfoCode,           /* 118 mambo bogus disk info op */
    SimBogusNetProbeCode,           /* 119 mambo bogus net probe op */
    SimBogusNetSendCode,            /* 120 mambo bogus net send op */
    SimBogusNetRecvCode,            /* 121 mambo bogus net recv op */

    SimFileLoadedByOSCode,          /* 122 */

    SimKernelThreadCallCode,        /* 123 */ /* kernel_thread() */

    SimKernelMmapFileCode,          /* 124 */ /* do_mmap from linux */

    SimDumpMemoryCode,              /* 125 */ /* print memory region to file */
    SimBogusHaltCode,               /* 126 */ /* bogus halt for idle */

    SimGetCycleCountCode,           /* 127 */ /* get cycle count */
    SimGetCycleCountAndPrintCode,   /* 128 prints the number of cycles since the last getcyclecount call */
    SimStartTrackingPIDCode,        /* 129 */ /* start tracking the given PID */
    SimStopTrackingPIDCode,         /* 130 */ /* stop tracking the given PID */
    SimDelayCode,                   /* 131 */ /* stall the processor */
    SimGateNewCode,                 /* 132 */ /* create a user level gate */
    SimGateFreeCode,                /* 133 */ /* free a user level gate */
    SimGateEnterCode,               /* 134 */ /* enter a user level gate */
    SimGateLeaveCode,               /* 135 */ /* leave a user level gate */
    SimToggleCacheStatsCode,        /* 136 toggles cache stats gathering on/off*/
    SimUserLockInitCode,            /* 137 */ /* init a user level lock */
    SimUserBarrierWaitCode,         /* 138 */ /* wait at a barrier */
    SimUserUnlockCode,              /* 139 */ /* unlock a user level lock */
    SimUserLockCode,                /* 140 */ /* lock a user level lock */
    SimUserLockTryCode,             /* 141 */ /* try to get a lock */
    SimStartStatsCode,              /* 142 */ /* Tell emitter reader to take stats */
    SimStopStatsCode,               /* 143 */ /* Tell emitter reader to stop stats */
    SimKernelLockTryCode,           /* 144 */ /* try to get a kernel lock */
    SimKernelLockCode,              /* 145 */ /* lock a kernel level lock */
    SimKernelUnlockCode,            /* 146 */ /* unlock a kernel level lock */
    SimKernelLockInitCode,          /* 147 */ /* init a kernel level lock */
    SimUserBarrierInitCode,         /* 148 */ /* init a user level barrier */
    SimStatsFilenameCode,           /* 149 */ /* tell emitter the output file name */
    SimGetCPUClockFreqCode,         /* 150 */ /* Get CPU clock frequency */
    SimDebugStringCode,             /* 151 */ /* Output Debug String to Mambo Terminal (Not console) */
    SimNMPOPCode,                   /* 152 */ /* NMP op code */
    SimAMOOPCode,                   /* 153 */ /* AMO op code */
    SimStartEnergyCode,             /* 154 */
    SimEndEnergyCode,               /* 155 */
    SimSetPowerTableCode,           /* 156 */
    SimChangeCoreFrequencyCode,     /* 157 */
    SimChangePowerSamplingPeriodCode, /* 158 */
    SimSetPowerModeCode,              /* 159 */
    SimGetCycleCountCodePtr,        /* 160 */ /* For 32-bit apps, this returns the 64-bit clock through a longlong ptr*/
    SimBogusNetFreeBufCode,         /* 161 */ /* Used by linux bogus net driver to free DMA buffers */
    SimBogusSocketSendCode,         /* 162 */
    SimBogusSocketRecvCode,         /* 163 */
    SimFlushCacheForARangeCode,     /* 164 */
    SimGetNextPacketCode,           /* 165 */
    SimTimePrintCode,               /* 166 */ /* lets emitter put a fake records in when time is to be printed */
    SimSyncStartCode,               /* 167 */
    SimSyncEndCode,                 /* 168 */
    SimBogusHaltDisableCode,        /* 169 */ /* used to disable bogus halt */
    SimCacheCoreDumpCode,           /* 170 */

    SimLookupTVUObjectByNameCode,   /* 171 */
    SimEnableTVUObjectCode,         /* 172 */
    SimDisableTVUObjectCode,        /* 173 */
    SimEnableAllTVUObjectsCode,     /* 174 */
    SimDisableAllTVUObjectsCode,    /* 175 */

    SimBpStatsResetCode,            /* 176 */  /* Reset BP Stats */
    SimBpStatsDumpCode,             /* 177 */  /* Print BP Stats */

    SimPrintCode,                   /* 178 */  /* Print */

    SimSetLocalityCode,             /* 179 */  /* Set locality */

    SimBogusNetDMASendCode,         /* 180 mambo bogus net send with DMA op */
    SimBogusNetDMARecvCode,         /* 181 mambo bogus net recv with DMA op */

    SimBogusSocketProbeCode,        /* 182 */
    SimBogusSocketNewSendCode,      /* 183 */
    SimBogusSocketNewRecvCode,      /* 184 */
    SimBogusSocketMemcpyCode,       /* 185 */
    SimBogusSocketMemsetCode,       /* 186 */

    SimSetHugePageCode,             /* 187 */  /* Set Huge Page Range */

    SimPrintIntCode,                /* 188 */  /* Print integer (64-bits) */

    SimLastCode /* This must be last in this list */
};
typedef enum SimSupportCode SimSupportCode;

#endif /* #ifndef _SIM_SUPPORT_CODE_H_ */
