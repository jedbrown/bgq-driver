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


#include "stdlib.h"
#include "stdio.h"

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upc_p_debug.h"


#define PRINTERR(...) fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }
#define BGPMDIE(_funcname_) { DIE(_funcname_ ": %d; %s\n", Bgpm_LastErr(), Bgpm_LastErrStrg()); }



#define ADD_EVENT(hEvtSet, evtId)  \
{                                    \
    int rc = Bgpm_AddEvent(hEvtSet, evtId); \
    if (rc < 0) BGPMDIE("Bgpm_AddEvent"); \
}



void PrintCounts(unsigned hEvtSet)
{
    // event has total cycles and total instructions
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cycles = 0;
    uint64_t instrs = 0;

    for (i=0; i<numEvts; i++) {
        int evtId = Bgpm_GetEventId(hEvtSet, i);
        if (evtId == PEVT_CYCLES) {
            if (Bgpm_ReadEvent(hEvtSet, i, &cycles)) BGPMDIE("Bgpm_ReadEvent");
        }
        else if (evtId == PEVT_INST_ALL) {
            if (Bgpm_ReadEvent(hEvtSet, i, &instrs)) BGPMDIE("Bgpm_ReadEvent");
        }
    }

    printf("Counter Results:\n");
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        if (Bgpm_ReadEvent(hEvtSet, i, &cnt)) BGPMDIE("Bgpm_ReadEvent");
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        if (!label) BGPMDIE("Bgpm_GetEventLabel");
        if (cycles && instrs) {
            printf("    %-40s = %-10ld,  %%Instr=%4.1f%%, %%Cycles=%4.1f%%\n",
                    label, cnt, ((double)cnt)/instrs*100.0, ((double)cnt)/cycles*100.0);
        }
        else if (instrs) {
            printf("    %-40s = %-10ld,  %%Instr=%4.1f%%\n",
                    label, cnt, ((double)cnt)/instrs*100.0);
        }
        else if (cycles) {
            printf("    %-40s = %-10ld,  %%Cycles=%4.1f%%\n",
                    label, cnt, ((double)cnt)/cycles);
        }
        else {
            printf("    %-40s = %-10ld\n", label, cnt);
        }
    }
}






int numEvtSets = 0;
void InitEvtSets()
{
    int rc = Bgpm_Init(BGPM_MODE_SWDISTRIB);
    if (rc < 0) BGPMDIE("Bgpm_Init");

    int hEvtSet = Bgpm_CreateEventSet();
    if (hEvtSet < 0) BGPMDIE("Bgpm_CreateEventSet");
    ADD_EVENT(hEvtSet, PEVT_IU_AXU_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_FXU_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_RAW_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_WAW_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES);
    ADD_EVENT(hEvtSet, PEVT_LSU_COMMIT_AXU_LDS);
    ADD_EVENT(hEvtSet, PEVT_INST_XU_LD);
    ADD_EVENT(hEvtSet, PEVT_INST_ALL);
    ADD_EVENT(hEvtSet, PEVT_CYCLES);
    numEvtSets++;

    hEvtSet = Bgpm_CreateEventSet();
    if (hEvtSet < 0) BGPMDIE("Bgpm_CreateEventSet");
    ADD_EVENT(hEvtSet, PEVT_IU_AXU_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_FXU_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_RAW_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_WAW_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES);
    ADD_EVENT(hEvtSet, PEVT_LSU_COMMIT_AXU_LDS);
    ADD_EVENT(hEvtSet, PEVT_INST_XU_LD);
    ADD_EVENT(hEvtSet, PEVT_INST_ALL);
    ADD_EVENT(hEvtSet, PEVT_CYCLES);
    numEvtSets++;

    hEvtSet = Bgpm_CreateEventSet();
    if (hEvtSet < 0) BGPMDIE("Bgpm_CreateEventSet");
    ADD_EVENT(hEvtSet, PEVT_IU_AXU_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_FXU_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_RAW_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_IU_WAW_DEP_HIT_CYC);
    ADD_EVENT(hEvtSet, PEVT_LSU_COMMIT_LD_MISSES);
    ADD_EVENT(hEvtSet, PEVT_LSU_COMMIT_AXU_LDS);
    ADD_EVENT(hEvtSet, PEVT_INST_XU_LD);
    ADD_EVENT(hEvtSet, PEVT_INST_ALL);
    ADD_EVENT(hEvtSet, PEVT_CYCLES);
    numEvtSets++;


}



const long arraySize = 1024L*1024L*1024L * 2L / sizeof(uint64_t);
uint64_t *buffer;

#define NUM_SAMPLES  4000
void IntegerSamples()
{
    buffer = (uint64_t *) malloc(arraySize*sizeof(uint64_t));  
    long i;
    for (i=0; i<arraySize; i++) {
        buffer[i] = i;
    }

    long index[NUM_SAMPLES];
    for (i=0; i<NUM_SAMPLES; i++) {
        index[i] = rand() % arraySize;
    }

    //printf("RAND_MAX = %d\n", RAND_MAX);

    int setNum, sample;
    long sum;
    for (setNum=0; setNum<numEvtSets; setNum++) {
        if (Bgpm_Apply(setNum)) BGPMDIE("Bgpm_Apply");
        if (Bgpm_ResetStart(setNum) < 0) BGPMDIE("Bgpm_ResetStart");
        for (sample=0; sample<NUM_SAMPLES; sample++) {
            sum = sum + buffer[index[sample]];
        }
        Bgpm_Stop(setNum);
        PrintCounts(setNum);
    }
}





int main(int argc, char *argv[])
{
    printf("Cycle thru stall counters\n");

    InitEvtSets();
    IntegerSamples();

    return 0;
}

