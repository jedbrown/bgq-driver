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
#include "spi/include/upci/testlib/upci_debug.h"
#include "spi/include/l1p/sprefetch.h"


#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


#define NUM_THREADS 68


void PrintPuCounts(int hPuSet, int startThd)
{
    uint64_t cnt;
    int evtIdx, endThd;

    for (; startThd<NUM_THREADS; startThd+=8) {
        endThd = MIN(NUM_THREADS-1, startThd+7);

        fprintf(stderr, "\nPunit Counts (%d - %d):\n", startThd, endThd);
        int numEvts = Bgpm_NumEvents(hPuSet);
        for (evtIdx=0; evtIdx<numEvts; evtIdx++) {
            const char *label = Bgpm_GetEventLabel(hPuSet, evtIdx);
            char outstrg[1024];
            int pos = 0;
            pos += sprintf(&outstrg[pos], "%-30s ", label);
            int thd;
            for (thd=startThd; thd<=endThd; thd++) {
                uint64_t appThdMask = UPC_APP_HWTID_MASK(thd);
                uint64_t agentThdMask = UPC_AGENT_HWTID_MASK(thd);
                Bgpm_ReadThreadEvent(hPuSet, evtIdx, appThdMask, agentThdMask, &cnt);
                pos += sprintf(&outstrg[pos], "0x%016lx ", cnt);
            }
            pos += sprintf(&outstrg[pos], "\n");
            fprintf(stderr, outstrg);
        }
    }
}



int main(int argc, char *argv[])
{
    TEST_MODULE(Check core activity when should be idle);

    Bgpm_Init(BGPM_MODE_HWDISTRIB);

    // set to look at Punit Counters
    int hPuSet = Bgpm_CreateEventSet();
    unsigned puEvtList[] = {
            PEVT_IU_INSTR_FETCHED,
            PEVT_IU_IL1_MISS,
            //PEVT_INST_XU_LD,
            PEVT_LSU_COMMIT_LD_MISSES,
            PEVT_INST_XU_ST,
            //PEVT_INST_XU_LSCX,
            //PEVT_L1P_BAS_LD,
            //PEVT_L1P_BAS_HIT,
            PEVT_XU_THRD_RUNNING,
            PEVT_CYCLES

    };
    Bgpm_AddEventList(hPuSet, puEvtList, sizeof(puEvtList)/sizeof(unsigned));

    int startThd = 0;
    Bgpm_Attach(hPuSet, ~0ULL, ~0ULL);

    //L1P_SetStreamPolicy(L1P_stream_disable);

    int hL2Set = Bgpm_CreateEventSet();
    unsigned l2EvtList[] = {
            PEVT_L2_HITS,
            PEVT_L2_MISSES,
            PEVT_L2_PREFETCH,
            PEVT_L2_FETCH_LINE,
            PEVT_L2_STORE_LINE,
            PEVT_L2_STORE_PARTIAL_LINE
    };
    Bgpm_AddEventList(hL2Set, l2EvtList, sizeof(l2EvtList)/sizeof(unsigned));

    Bgpm_Start(hPuSet);

    Upci_Delay(10000);

    Bgpm_Stop(hPuSet);


    fprintf(stderr, "L2 Counts:\n");
    int numEvts = Bgpm_NumEvents(hL2Set);
    uint64_t cnt;
    int i;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hL2Set, i, &cnt);
        const char *label = Bgpm_GetEventLabel(hL2Set, i);
        printf("   0x%016lx <= %s\n", cnt, label);
    }


    PrintPuCounts(hPuSet, startThd);

    TEST_RETURN();
}

