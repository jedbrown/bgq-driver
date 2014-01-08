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


#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"
#include "../../test_utils.h"


#define PRINTERR(...) fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }





// Test Data
#define N 1048576
#if 0
static double   a[N],
                b[N],
                c[N];
#endif
static double sum = 0;


// Shared User data used to aggregate event counts from each thread.
uint64_t evtSums[24];  // Use to sum events from all thread

unsigned evtList[] = {      // List of performance events to collect

#if 1
        PEVT_LSU_COMMIT_LD_MISSES, // 0
        PEVT_LSU_COMMIT_CACHEABLE_LDS, // 1
        //PEVT_L1P_BAS_MISS, // 2
        PEVT_CYCLES,
        PEVT_INST_XU_ALL, // 3
        PEVT_INST_QFPU_ALL, // 4
        PEVT_INST_QFPU_FPGRP1, // 5
#else
    //PEVT_XU_PPC_COMMIT,
    //PEVT_AXU_INSTR_COMMIT,
    //PEVT_IU_IS1_STALL_CYC,
    //PEVT_IU_IS2_STALL_CYC,
    //PEVT_IU_AXU_FXU_DEP_HIT_CYC,
    //PEVT_IU_AXU_DEP_HIT_CYC,
    PEVT_IU_FXU_DEP_HIT,  // key
    //PEVT_IU_FXU_DEP_HIT_CYC,  // key
    PEVT_IU_BARRIER_OP_STALL_CYC,
    //PEVT_IU_BARRIER_OP_STALL_CYC,
    //PEVT_IU_SYNC_DEP_HIT_CYC,
    //PEVT_IU_SLOW_SPR_ACCESS,
    //PEVT_IU_SPR_DEP_HIT,
    //PEVT_IU_SPR_DEP_HIT_CYC,
    //PEVT_IU_RAW_DEP_HIT,
    //PEVT_IU_RAW_DEP_HIT_CYC,
    //PEVT_IU_WAW_DEP_HIT,
    //PEVT_IU_WAW_DEP_HIT_CYC,
    //PEVT_XU_INTS_TAKEN,
    //PEVT_IU_FXU_ISSUE_COUNT,
    //PEVT_XU_PROC_BUSY,
    //PEVT_XU_BR_MISPRED_COMMIT_CORE,
    PEVT_XU_BR_MISPRED_COMMIT,
    PEVT_XU_BR_TAKEN_COMMIT,
    PEVT_CYCLES,
    PEVT_INST_ALL,
#endif
};
int numEvts = sizeof(evtList) / sizeof(int);
unsigned cyclesIdx = 0;

uint64_t resultCounts[64][24];  // max possible results [threads][ctrs]
int phyThd[64];                 // match to omp thread.
uint64_t tb_elapsed[64];


int main(int argc, char *argv[])
{
    if (Kernel_ProcessorID() == 0) {
        printf("Verify punit results from each thread are consistent\n");
    }

    if (argc < 2) {
        DIE("Pass in number of threads per process\n");
    }
    int numThreads = atoi(argv[1]);
    int numProcs = Kernel_ProcessCount();

    if (Kernel_ProcessorID() == 0) {
        printf("numProcs=%d, numThreads=%d, numEvts=%d\n", numProcs, numThreads, numEvts);
    }

    InitCriticalLock();

    int hwThd = Kernel_ProcessorID();
    fprintf(stderr, _AT_ " Kernel_ProcessorID()=%d, core=%d, thd=%d\n", hwThd, hwThd/4, hwThd%4);


    int i,j;
    sum = 0;
    memset(evtSums,0,sizeof(evtSums));  // init

    //Kernel_Upci_Wait4MailboxEmpty();

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(sum,evtSums,evtList,numEvts,stderr,resultCounts,phyThd,tb_elapsed,cyclesIdx)    \
        private(i,j)
    {
        //Upci_DumpKDebug(0);

        volatile double   la[N],
                 lb[N],
                 lc[N];

        uint64_t tb_start;
        uint64_t tb_end;
        // Configure and start hw counters.
        // Bgpm_Init just inits this thread use the hardware.
        // This happens once per thread.

        Bgpm_Init(BGPM_MODE_SWDISTRIB);


        int hEvtSet = Bgpm_CreateEventSet();   // get container for events.
        Bgpm_AddEventList(hEvtSet, evtList, numEvts);
        cyclesIdx = Bgpm_GetEventIndex(hEvtSet,PEVT_CYCLES,0);

        Bgpm_Apply(hEvtSet);        // apply to hardware

        Bgpm_Punit_Handles_t hCtrs;
        Bgpm_Punit_GetHandles(hEvtSet, &hCtrs);

        #if 0
        if (Kernel_ProcessorID() == 0) {
            Upci_A2PC_Val_t a2qry;
            Kernel_Upci_A2PC_GetRegs(&a2qry);
            Upci_A2PC_DumpRegs(&a2qry);
            UPC_P_Dump_State(Kernel_ProcessorCoreID());
        }
        #endif

        #pragma omp barrier
        //Delay(2000);

        //Bgpm_ResetStart(hEvtSet);   // start counting.
        tb_start = GetTimeBase();
        //Bgpm_Punit_Start(hCtrs.allCtrMask);
        Bgpm_Start(hEvtSet);   // start counting.

#if 0
        // Target code to measure
        int pclks = 4000;
        while( pclks-- )
        {
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
            asm volatile("nop;");
        }
#else
        for (j=0; j<N; j++) {
            la[j] = 1.0;
            lb[j] *= la[j];
            lc[j] /= la[j];
        }
#endif
        Bgpm_Stop(hEvtSet);   // stop counting - once per thread.
        //Bgpm_Punit_Stop(hCtrs.allCtrMask);
        tb_end = GetTimeBase();

        //Delay(2000);
        //Upci_Delay(800);  // min low-level delay before reading.

        #pragma omp barrier


        // Gather sum for each event.
        #pragma omp critical (SumEvts)
        {
            for (i=0; i<numEvts; i++) {
                uint64_t val;
                Bgpm_ReadEvent(hEvtSet, i, &val);
                resultCounts[omp_get_thread_num()][i] = val;
                evtSums[i] += val;
            }
            phyThd[omp_get_thread_num()] = Kernel_ProcessorID();
            tb_elapsed[omp_get_thread_num()] = tb_end - tb_start;
        }
    }


    ShmCriticalLock();

#if 0
    printf("After OMP for, sum is: %lf\n", sum);
    printf("The event counts for %d threads are:\n", numThreads);
    for (i=0; i<numEvts; ++i) {
        printf("   %s     = %ld\n", Bgpm_GetEventIdLabel(evtList[i]), evtSums[i]);
        printf("   %s_avg = %ld\n", Bgpm_GetEventIdLabel(evtList[i]), evtSums[i]/numThreads);
    }
#endif


    printf("Individual thread counts:\n");

    char ostrg[2048];
    int lblWidth = 0;
    for (i=0; i<numEvts; ++i) {
        lblWidth = MAX(lblWidth, strlen(Bgpm_GetEventIdLabel(evtList[i])));
    }
    lblWidth = MAX(lblWidth, strlen("thd(phyid):"));
    lblWidth = MAX(lblWidth, strlen("tb_cycles-CYCLES"));

    int thd;
    char lblFormat[128];
    sprintf(lblFormat, "%%-%ds", lblWidth);
    sprintf(ostrg, lblFormat, "thd(phyid):");
    for (thd = 0; thd<numThreads; ++thd) {
        int pos = strlen(ostrg);
        sprintf(&ostrg[pos], "   %02d(%02d) ", thd, phyThd[thd]);
    }
    printf("%s\n", ostrg);


    for (i=0; i<numEvts; ++i) {
        int pos = sprintf(ostrg, lblFormat, Bgpm_GetEventIdLabel(evtList[i]));
        for (thd = 0; thd<numThreads; ++thd) {
            pos += sprintf(&ostrg[pos], " %9ld", resultCounts[thd][i]);
        }
        printf("%s\n", ostrg);
    }

    int pos = sprintf(ostrg, lblFormat, "tb_cycles");
    for (thd = 0; thd<numThreads; ++thd) {
        pos += sprintf(&ostrg[pos], " %9ld", tb_elapsed[thd]);
    }
    printf("%s\n", ostrg);

    pos = sprintf(ostrg, lblFormat, "tb_cycles-CYCLES");
    for (thd = 0; thd<numThreads; ++thd) {
        pos += sprintf(&ostrg[pos], " %9ld", (tb_elapsed[thd] - resultCounts[thd][cyclesIdx]));
    }
    printf("%s\n", ostrg);


#if 0
    Upci_A2PC_Val_t a2qry;
    Kernel_Upci_A2PC_GetRegs(&a2qry);
    Upci_A2PC_DumpRegs(&a2qry);

    UPC_P_Dump_State(Kernel_ProcessorCoreID());
#endif

#if 0
    UPC_C_Dump_Counters(12,21);
#endif


    ShmCriticalUnlock();

    return(0);
}


