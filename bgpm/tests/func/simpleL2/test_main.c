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


// This test needs a lot of work
// - create library of bgq sizes and specs with known routines to force known events.
// - Use these routines to force predictable L2 events for verification.
// - Must be able to vary based on memsize, which is variable.


int hPUSet;
Bgpm_Punit_Handles_t hPCtrs;


uint64_t combinedCounts[6];   // use to sum actual slice counts for comparing with combined counts.


// Vars for dummy operations to measure
#define DSIZE 512
volatile long target[DSIZE];
volatile long source[DSIZE];


#define L2EXP_SLICE( _evtset_, _slice_, _ctr_, _exp1_, _exp2_ ) {      \
    uint64_t act; \
    Bgpm_L2_ReadEvent(_evtset_, _slice_, _ctr_, &act);     \
    if (( act < _exp1_) || (act > _exp2_)) {                   \
        fprintf(stderr, _AT_ " L2 Unexpect[%d:%d] = 0x%016lx Exp1=0x%016lx Exp2=0x%016lx\n", \
                _slice_, _ctr_, act, (uint64_t)_exp1_, (uint64_t)_exp2_ );       \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, _AT_ " L2 Count[%d:%d]    = 0x%016lx Exp1=0x%016lx Exp2=0x%016lx\n", \
                _slice_, _ctr_, act, (uint64_t)_exp1_, (uint64_t)_exp2_ );       \
    } \
}


#define L2EXP_COMB( _evtset_, _ctr_, _exp1_, _exp2_ ) {      \
    uint64_t act; \
    Bgpm_ReadEvent(_evtset_, _ctr_, &act);     \
    if (( act < _exp1_) || (act > _exp2_)) {                   \
        fprintf(stderr, _AT_ " L2 Unexpect[%d] = 0x%016lx Exp1=0x%016lx Exp2=0x%016lx\n", \
                _ctr_, act, (uint64_t)_exp1_, (uint64_t)_exp2_ );       \
        numFailures++; \
    } \
    else if (TEST_CASE_VERBOSE) { \
        fprintf(stderr, _AT_ " L2 Count[%d]    = 0x%016lx Exp1=0x%016lx Exp2=0x%016lx\n", \
                _ctr_, act, (uint64_t)_exp1_, (uint64_t)_exp2_ );       \
    } \
}



void Print_Punit_Counts(unsigned hEvtSet, Bgpm_Punit_Handles_t *phCtrs)
{
    printf("Fast Counter Results: %d\n", hEvtSet);
    int i;
    int numEvts = phCtrs->numCtrs;
    for (i=0; i<numEvts; i++) {
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx <= %s\n",
                Bgpm_Punit_Read(phCtrs->hCtr[i]),
                label);
    }
}


void PrintCounts(unsigned hEvtSet)
{
    printf("Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("   0x%016lx <= %s\n", cnt, label);
    }
}



void PrintSliceCounts(unsigned slice, int hEvtSet)
{
    printf("Slice %d Counter Results:\n", slice);
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_L2_ReadEvent(hEvtSet, slice, i, &cnt);
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("   0x%016lx <= %s\n", cnt, label);
    }
}



void MeasureOperations(unsigned hEvtSet)
{
    Kernel_Upci_Wait4MailboxEmpty();

    // init arrays for multiple operations test
    int i;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }

    // create a few L2 events - work backward to foil prefetch?
    // Perform loads to create miss events
    Bgpm_Reset(hPUSet);
    Bgpm_ResetStart(hEvtSet);
    Bgpm_Punit_Start(hPCtrs.allCtrMask);
    Upci_Delay(200000);
#if 0
    for (i=DSIZE-1; i>=0; i-=32) {
        source[i] *= target[i];
    }
#endif
    Bgpm_Punit_Stop(hPCtrs.allCtrMask);
    Bgpm_Stop(hEvtSet);
}








// Check counts when the L2 Slices count independently.
void Test_L2_Slices()
{
    TEST_CASE( Test L2 Slice Counts );


    // For testing - let's start L2 Values off at slice# * 256 + ctr (put slice # into 2nd byte)
    // Calculate the expected values for comparison purposes.
    short ctr, slice;
    uint64_t expected[16][6];
    for (slice=0; slice<16; slice++) {
        for (ctr=0; ctr<6; ctr++) {
            expected[slice][ctr] = (slice*256) + ctr;
        }
    }


    int hEvtSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hEvtSet, PEVT_L2_HITS_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_MISSES_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_PREFETCH_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_FETCH_LINE_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_LINE_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_PARTIAL_LINE_SLICE);

    // Attempt to add combined event - should fail
    Bgpm_ExitOnError(BGPM_FALSE);
    TEST_CHECK_EQUAL(Bgpm_AddEvent(hEvtSet, PEVT_L2_HITS), BGPM_ECONF_L2_COMBINE);
    Bgpm_ExitOnError(BGPM_TRUE);

    TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);
    Bgpm_Apply(hEvtSet);  // should have no effect for L2


    // Write starting values to L2 counters
    for (slice=0; slice<16; slice++) {
        for (ctr=0; ctr<6; ctr++) {
            Bgpm_L2_WriteEvent(hEvtSet, slice, ctr, (slice*256) + ctr);
        }
    }

    // Verify Written Values
    for (slice=0; slice<16; slice++) {
        for (ctr=0; ctr<6; ctr++) {
            L2EXP_SLICE(hEvtSet, slice, ctr, (slice*256) + ctr, (slice*256) + ctr);
        }
    }


    #if 0
    UPC_L2_Dump_State();
    #endif

    // init arrays for multiple operations test
    int i;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }

    // create a few L2 events - work backward to foil prefetch?
    // Perform loads to create miss events
    Bgpm_Start(hEvtSet);
    for (i=DSIZE-1; i>=0; i-=32) {
        source[i] *= target[i];
    }
    Bgpm_Stop(hEvtSet);



    #if 0
    UPC_L2_Dump_State();
    #endif

    // Check final values for each slice.
    for (slice=0; slice<16; slice++) {
        for (ctr=0; ctr<6; ctr++) {
            unsigned range = (ctr == 0 ? 0x100 : 10 ); // account for l2hit noise from network polling
            L2EXP_SLICE(hEvtSet, slice, ctr, expected[slice][ctr], expected[slice][ctr]+range);
        }
    }

    Bgpm_DeleteEventSet(hEvtSet);  // delete to free up l2 mode (allow combining)
}




// Check counts when the L2 Slices count independently.
void Test_L2_Slices2()
{
    TEST_CASE( Get Sum of L2 Slice Counts );

    int hEvtSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hEvtSet, PEVT_L2_HITS_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_MISSES_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_PREFETCH_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_FETCH_LINE_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_LINE_SLICE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_PARTIAL_LINE_SLICE);

    Bgpm_Apply(hEvtSet);  // should have no effect for L2

    MeasureOperations(hEvtSet);

    // init combined values.
    int ctr, slice;
    for (ctr = 0; ctr<6; ctr++) combinedCounts[ctr] = 0;

    // Check final values for each slice.
    UpciBool_t foundNonZeroValues = BGPM_FALSE;
    for (slice=0; slice<16; slice++) {
        for (ctr=0; ctr<6; ctr++) {
            // sum into combined value for later comparison
            uint64_t act;
            Bgpm_L2_ReadEvent(hEvtSet, slice, ctr, &act);
            combinedCounts[ctr] += act;

            if (act != 0) { foundNonZeroValues = BGPM_TRUE; }

            unsigned range = (ctr == 0 ? 0x4000 : 10 );
            L2EXP_SLICE(hEvtSet, slice, ctr, 0, range);
        }
    }
    TEST_CHECK(foundNonZeroValues);  // make sure at least some values were nonzero

    if (TEST_CASE_VERBOSE) {
        for (ctr = 0; ctr<6; ctr++) {
            fprintf(stderr, "L2 Ctr[%d] sum = 0x%016lx\n", ctr, combinedCounts[ctr]);
        }
    }

    Print_Punit_Counts(hPUSet, &hPCtrs);

    Bgpm_DeleteEventSet(hEvtSet);  // delete to free up l2 mode (allow combining)
}



// Check counts when the L2 Slices are combined
void Test_L2_Combined()
{
    TEST_CASE( Test L2 Combined Counts );

    int hEvtSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hEvtSet, PEVT_L2_HITS);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_MISSES);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_PREFETCH);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_FETCH_LINE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_LINE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_PARTIAL_LINE);

    // Attempt to add slice event - should fail
    Bgpm_ExitOnError(BGPM_FALSE);
    TEST_CHECK_EQUAL(Bgpm_AddEvent(hEvtSet, PEVT_L2_HITS_SLICE), BGPM_ECONF_L2_COMBINE);
    Bgpm_ExitOnError(BGPM_TRUE);

    TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);
    Bgpm_Apply(hEvtSet);  // should have no effect for L2

    MeasureOperations(hEvtSet);


    // Check final combined values against sum from slice test.
    int ctr;
    for (ctr=0; ctr<6; ctr++) {
        uint64_t compareLow = combinedCounts[ctr] > 100 ? combinedCounts[ctr] - 100 : 0;
        L2EXP_COMB(hEvtSet, ctr, compareLow, combinedCounts[ctr]+256);

        uint64_t act;
        Bgpm_ReadEvent(hEvtSet, ctr, &act);
        combinedCounts[ctr] = act;
    }

    // make sure BGPM_TRUEly stopped - check values again.
    for (ctr = 0; ctr<6; ctr++) {
        L2EXP_COMB(hEvtSet, ctr, combinedCounts[ctr], combinedCounts[ctr]);
    }
    Print_Punit_Counts(hPUSet, &hPCtrs);


    Bgpm_DeleteEventSet(hEvtSet);
}




int main(int argc, char *argv[])
{
    TEST_MODULE(L2 operation sanity check);

    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    // set to look at Punit Counters
    hPUSet = Bgpm_CreateEventSet();
    unsigned puEvtList[] = {
            PEVT_IU_INSTR_FETCHED,
            PEVT_IU_IL1_MISS,
            PEVT_INST_XU_LD,
            PEVT_LSU_COMMIT_LD_MISSES,
            PEVT_L1P_BAS_LD,
            PEVT_L1P_BAS_HIT,
            PEVT_INST_ALL,
    };
    Bgpm_AddEventList(hPUSet, puEvtList, sizeof(puEvtList)/sizeof(unsigned));
    Bgpm_Apply(hPUSet);
    Bgpm_Punit_GetHandles(hPUSet, &hPCtrs);

    L1P_SetStreamPolicy(L1P_stream_disable);

    Test_L2_Slices();

    Test_L2_Slices2();  // do again to get sums
    Test_L2_Slices2();  // do again to get sums with hot cache

    Test_L2_Combined();
    //Test_L2_Combined();
    //Test_L2_Combined();
    //Test_L2_Combined();
    //Test_L2_Combined();
    //Test_L2_Compare_Slice_Combined();
    //Test_L2_Compare_Slice_Combined();

    TEST_RETURN();
}

