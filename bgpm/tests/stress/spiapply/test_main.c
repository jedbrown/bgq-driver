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
#include "../../test_utils.h"

#include "bgpm/include/bgpm.h"
#include "spi/include/kernel/location.h"
#include "spi/include/upci/testlib/upci_debug.h"



#define ADD_PUNIT_EVENT(unit, evt, thd, handle)  \
{                                    \
    int rc = Upci_Punit_AddEvent(&unit, evt, thd, &(handle));      \
    if (rc < 0) {                                                 \
        PRINTERR("Add of event %d Failed, rc=%d\n", evt, rc);       \
    } \
}




unsigned evtList[6] = { PEVT_IU_AXU_FXU_DEP_HIT_CYC,    //  0
                        PEVT_LSU_COMMIT_LD_MISSES,      //  1
                        PEVT_LSU_COMMIT_CACHEABLE_LDS,  //  2
                        PEVT_L1P_BAS_MISS,              //  3
                        PEVT_INST_XU_ALL,               //  4
                        PEVT_INST_QFPU_ALL};            //  5

unsigned l2EvtList[6] = {
        PEVT_L2_HITS,
        PEVT_L2_MISSES,
        PEVT_L2_PREFETCH,
        PEVT_L2_FETCH_LINE,
        PEVT_L2_STORE_LINE,
        PEVT_L2_STORE_PARTIAL_LINE};

int numEvts = sizeof(evtList) / sizeof(unsigned);
int numL2Evts = sizeof(l2EvtList) / sizeof(unsigned);



int main(int argc, char *argv[])
{
    fprintf(stderr, "Stress of punit SPI Apply()\n");

    int numThreads = 64;
    int seconds = 60;
    if (argc > 1) {
       seconds = atoi(argv[1]);
    }
    fprintf(stderr, "numThreads=%d, seconds=%d\n", numThreads, seconds);



    //Bgpm_Init(BGPM_MODE_SWDISTRIB);
    uint64_t targCycles = (uint64_t)1600 * 1024*1024 * seconds;
    uint64_t cpuSpeed = 1600;
    //uint64_t targCycles = ((uint64_t)upciNodeParms.cpuSpeed) * 1024*1024 * seconds;
    fprintf(stderr, "cpuspeed=%ld, targCycles=%ld\n", cpuSpeed, targCycles);

    //UPC_C_Dump_State();

    //Bgpm_Disable();



    uint64_t startTB = GetTimeBase();
    uint64_t totalLoops = 0;

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(evtList,numEvts,stderr,targCycles,startTB,totalLoops,numL2Evts,l2EvtList)
    {
        while ((GetTimeBase()-startTB) < targCycles) {

            //if ((totalLoops % 16) == Kernel_ProcessorCoreID()) {
             if (Kernel_ProcessorID() == 0) {

                if ((totalLoops % 10000) == 0) {
                    fprintf(stderr, "core=%d, loops=%ld\n", Kernel_ProcessorCoreID(), totalLoops);
                }
             }

             //fprintf(stderr, _AT_ "\n");
                Upci_Mode_t upciMode;
                #if 1
                Upci_Punit_t punit;   // actual measurement punit
                Upci_Punit_EventH_t hEvt[24];   // array to hold handles
                int cNum = 0;
                unsigned thd = Kernel_PhysicalHWThreadID();
                #endif

                //Upci_DumpKDebug(0);
                Upci_Mode_Init(&upciMode, 0, UPC_CM_INDEP, Kernel_ProcessorCoreID());
                //fprintf(stderr, _AT_ "\n");


                #if 1
                Upci_Punit_Init(&punit, &upciMode, Kernel_ProcessorCoreID());

                for (cNum=0; cNum<numEvts; cNum++) {
                    ADD_PUNIT_EVENT(punit, evtList[cNum], thd, hEvt[cNum]);
                }

                int rc = Upci_Punit_Apply(&punit);
                if (rc) {
                    DIE("Upci_Punit_Apply failed; rc = %d\n", rc);
                }
                #endif

                //UPC_L2_EnableUPC(BGPM_TRUE, BGPM_TRUE);
                //UPC_L2_Start();
                //Upci_Punit_Start_Indep(&punit, UPCI_CTL_SHARED | UPCI_CTL_OVF);
                //Delay(5);
                //Upci_Punit_Stop_Indep(&punit, UPCI_CTL_SHARED | UPCI_CTL_OVF);
                //UPC_L2_Stop();

                //TEST_AT();

                Upci_Mode_Free(&upciMode);

                if (Kernel_ProcessorID() == 0) {
                //#pragma omp critical
                //{
                    totalLoops++;
                //}
                }

            #pragma omp barrier
        }
    }
#ifdef UPCI_INCL_DEBUG_DATA
    Upci_DumpDebugCounts(0);
#endif


    printf("After OMP parallel, totalLoops = %ld\n", totalLoops);
    return(0);
}


