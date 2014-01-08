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
#include <sched.h>
#include <omp.h>

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"
#include "spi/include/l1p/sprefetch.h"

#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();

int numThreads;
int numProcs;


void Exercise(int useMuxSet)
{
    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,numFailures,useMuxSet)
    {
        //L1P_SetStreamPolicy(L1P_stream_disable);

        Bgpm_Init(BGPM_MODE_SWDISTRIB);

        unsigned evtId;
        for (evtId=1; evtId<=PEVT_PUNIT_LAST_EVENT; evtId++) {

            // Let's serialize the thread activity - just to allow use to examine
            // multiple threads per core, but make the cfg_edge still work easily, as they
            // will be examining the values which effect multiple.
            SHMC_SERIAL_Start(numProcs,numThreads);

            // make sure the we start from clean slate with next apply
            upc_p_local->cfg_edge = 0;

            // Verify the normal edge/cycle
            Bgpm_EventInfo_t eInfo;
            Bgpm_GetEventIdInfo(evtId, &eInfo);
            #ifdef TEST_CASE_VERBOSE
            fprintf(stderr, "\n");
            fprintf(stderr, "Eventid=%4d,  metric=%-4s,   label=%s\n", evtId, Upci_GetMetricText(eInfo.metric), eInfo.label);
            #endif


            int hEvtSet = Bgpm_CreateEventSet();
            if (useMuxSet) {
                Bgpm_SetMultiplex(hEvtSet, 0, 0);
            }
            Bgpm_AddEvent(hEvtSet, evtId);
            ushort eEdge;
            Bgpm_GetEventEdge(hEvtSet, 0, &eEdge);

            // Check the default edge/cycle value.
            // is this a edge default event.
            if ((eInfo.metric == UPC_METRIC_BEVENTS) || (eInfo.metric == UPC_METRIC_EVENTS)) {
                TEST_CHECK_EQUAL(eEdge, 1);
                Bgpm_Apply(hEvtSet);
                uint64_t upcpCfgEdge = upc_p_local->cfg_edge;
                TEST_CHECK_NONZERO(upcpCfgEdge);
            }
            // cycle default event
            else {
                TEST_CHECK_EQUAL(eEdge, 0);
                Bgpm_Apply(hEvtSet);
                uint64_t upcpCfgEdge = upc_p_local->cfg_edge;
                TEST_CHECK_EQUAL(upcpCfgEdge, 0);
            }

            Bgpm_DeleteEventSet(hEvtSet);


            // Now let's invert the edge/cycle for the event
            hEvtSet = Bgpm_CreateEventSet();
            if (useMuxSet) {
                Bgpm_SetMultiplex(hEvtSet, 0, 0);
            }

            Bgpm_AddEvent(hEvtSet, evtId);

            if ((eInfo.metric == UPC_METRIC_BEVENTS) || (eInfo.metric == UPC_METRIC_EVENTS)) {
                // ignore failures for punit events which cannot have their edge switched
                // (like l1p or opcode events)
                Bgpm_ExitOnError(BGPM_FALSE);
                int rc = Bgpm_SetEventEdge(hEvtSet, 0, 0);
                Bgpm_ExitOnError(BGPM_TRUE);
                if ( rc == 0 ) {
                    Bgpm_GetEventEdge(hEvtSet, 0, &eEdge);
                    TEST_CHECK_EQUAL(eEdge, 0);
                    Bgpm_Apply(hEvtSet);
                    uint64_t upcpCfgEdge = upc_p_local->cfg_edge;
                    TEST_CHECK_EQUAL(upcpCfgEdge, 0);
                }
            }
            else {
                Bgpm_ExitOnError(BGPM_FALSE);
                int rc = Bgpm_SetEventEdge(hEvtSet, 0, 1);
                Bgpm_ExitOnError(BGPM_TRUE);
                if ( rc == 0 ) {
                    Bgpm_GetEventEdge(hEvtSet, 0, &eEdge);
                    TEST_CHECK_EQUAL(eEdge, 1);
                    Bgpm_Apply(hEvtSet);
                    uint64_t upcpCfgEdge = upc_p_local->cfg_edge;
                    TEST_CHECK_NONZERO(upcpCfgEdge);
                }
            }

            Bgpm_DeleteEventSet(hEvtSet);

            SHMC_SERIAL_End();

        }
        Bgpm_Disable();
    }
}



int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Check if event edge/cycle values being set correctly);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

    SHMC_Init_Barriers(numProcs, numThreads);

    TEST_CASE(Check normal punit event set);
    Exercise(0);

    TEST_CASE(Check multiplexing event set);
    Exercise(1);

    TEST_RETURN();

}

