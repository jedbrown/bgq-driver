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



int hPUSet;
int hL2Set;



#define LOOPS 50
#define DSIZE ((1024*1024*1024/8))   // 1 GigBytes if 8 byte entries
uint64_t *pDBuffer;



int main(int argc, char *argv[])
{
    TEST_MODULE(Examine Cache Boundaries by looking at cache misses);

    pDBuffer = (uint64_t*)malloc(DSIZE*sizeof(uint64_t));

    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    // set to look at Punit Counters
    hPUSet = Bgpm_CreateEventSet();
    unsigned puEvtList[] = {
            PEVT_LSU_COMMIT_LD_MISSES,
            PEVT_LSU_COMMIT_CACHEABLE_LDS,
            PEVT_IU_RAW_DEP_HIT_CYC,
    };
    Bgpm_AddEventList(hPUSet, puEvtList, sizeof(puEvtList)/sizeof(unsigned));
    Bgpm_Apply(hPUSet);

    // L2 Counts
    int hL2Set = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hL2Set, PEVT_L2_MISSES);
    Bgpm_Apply(hL2Set);

    L1P_SetStreamPolicy(L1P_stream_disable);

    // for sizes below  128 = 1024/8 (thus 128 == 1k of buffer size)
    // Trials chosen to sit on each side of expected cache sizes.
    uint64_t trialSizes[] = { 128, 128*4, 128*8, 128*12, 128*16, 128*32, 128*30720, 128*32768, 128*34816 };
    int numTrials = sizeof(trialSizes)/sizeof(uint64_t);
    int trialNum = 0;

    unsigned loop;
    uint64_t idx;
    volatile register uint64_t data;
    uint64_t l1misses;
    uint64_t l2misses;
    uint64_t numLoads;
    uint64_t ldCycles;

    // Run 3 trials for each trial size - with the actual bumped +/- the target size.
    uint64_t bump = 64;

    int pass = 0;
    uint64_t nextMax = trialSizes[trialNum];
    while ((nextMax <= DSIZE) && (trialNum < numTrials)) {

        uint64_t curMax = nextMax;
        if (pass == 0) {
            curMax -= bump;
        }
        else if (pass == 1) {
            curMax = nextMax;
        }
        else if (pass == 2) {
            curMax += bump;
        }

        Bgpm_Reset(hPUSet);
        Bgpm_Reset(hL2Set);

        // Warmup
        for (idx=0; idx<curMax; ++idx) {
             data = pDBuffer[idx];
        }

        // measure
        Bgpm_Start(hL2Set);
        Bgpm_Start(hPUSet);

        for (loop=0; loop<LOOPS; ++loop) {
            for (idx=0; idx<curMax; ++idx) {
                data = pDBuffer[idx];
            }
        }

        Bgpm_Stop(hPUSet);
        Bgpm_Stop(hL2Set);


        Bgpm_ReadEvent(hPUSet, 0, &l1misses);
        Bgpm_ReadEvent(hPUSet, 1, &numLoads);
        Bgpm_ReadEvent(hPUSet, 2, &ldCycles);
        Bgpm_ReadEvent(hL2Set, 0, &l2misses);

        uint64_t calcLoads = LOOPS*curMax;
        uint64_t pct_l1miss10000 = l1misses*10000/numLoads;
        uint64_t pct_l2miss10000 = l2misses*10000/numLoads;
        uint64_t ldCycles100     = (numLoads+ldCycles)*100/numLoads;
        fprintf(stderr, _AT_ " PERFMETRICS: BUFSIZE=%-9ld  PASS=%d  CALCLOADS=%-9ld  NUMLOADS=%-9ld  L1MISSES=%-9ld  L2MISSES=%-9ld  "
                             " L1MISSRATIO=%02ld.%02ld%%  L2MISSRATIO=%02ld.%02ld%%  AVG_CYC_PER_LD=%02ld.%02ld\n",
                curMax*sizeof(uint64_t), pass, calcLoads, numLoads, l1misses, l2misses,
                pct_l1miss10000/100, pct_l1miss10000%100,
                pct_l2miss10000/100, pct_l2miss10000%100,
                ldCycles100/100, ldCycles100%100);

        if (pass == 0) {
            pass = 1;
        }
        else if (pass == 1) {
            pass = 2;
        }
        else if (pass == 2) {
            pass = 0;
            if (++trialNum < numTrials) {
                nextMax = trialSizes[trialNum];
            }
        }
    }


    TEST_RETURN();
}

