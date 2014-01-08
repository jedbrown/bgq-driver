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
#include <omp.h>

#include "cnk/include/Config.h"
#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"
extern Upci_Punit_t * DebugGetPunit(unsigned);

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"




TEST_INIT();



int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Try HwMode with Multiple Processeses and thread);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    int numThreads = atoi(argv[1]);
    int numProcs = Kernel_ProcessCount();

    if (Kernel_ProcessorID() == 0) {
        fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);
    }

    Bgpm_Init(BGPM_MODE_HWDISTRIB);

    unsigned evtList1[] = {
        PEVT_L1P_BAS_LD,
        PEVT_L1P_BAS_ST_WC,
        PEVT_L1P_STRM_WRT_INVAL,
        PEVT_XU_BR_COMMIT,
        PEVT_CYCLES
    };
    int hPuSet;
    int i;
    hPuSet = Bgpm_CreateEventSet();
    Bgpm_AddEventList(hPuSet, evtList1, sizeof(evtList1)/sizeof(unsigned));

    uint64_t thdMask = Kernel_ThreadMask(Kernel_MyTcoord());
    fprintf(stderr, _AT_ " thdMask=0x%016lx\n", thdMask);

    Bgpm_Attach(hPuSet, thdMask, 0);
    Bgpm_Start(hPuSet);

    Bgpm_Disable();



    TEST_CASE_PRINT0(Stress Creation of  slightly different event sets in each sw thread);
    for (i=0; i<10000; i++) {
        if (Kernel_ProcessorID() == 0) {
            //fprintf(stderr, "loop = %d\n", i);
        }
        #pragma omp parallel default(none) num_threads(numThreads)      \
            shared(stderr,numFailures)
        {
            unsigned thdId = Kernel_ProcessorID();
            unsigned firstEvt = 50 + thdId;   // pick an arbitrary event, but insure you avaoid any core-shared events.

            Bgpm_Init(BGPM_MODE_HWDISTRIB);  // gives thread it's own environment.

            int hThdEvtSet = Bgpm_CreateEventSet();
            int hThdEvtSet2 = Bgpm_CreateEventSet();  // just to duplicate Bob's failing scenario
            TEST_CHECK(hThdEvtSet2 != hThdEvtSet);

            unsigned evtList[4];
            evtList[0] = firstEvt;
            evtList[1] = firstEvt+1;
            evtList[2] = firstEvt+2;
            evtList[3] = firstEvt+3;
            Bgpm_AddEventList(hThdEvtSet, evtList, 2);

            Bgpm_Disable();
        }
    }

    TEST_RETURN();
}

