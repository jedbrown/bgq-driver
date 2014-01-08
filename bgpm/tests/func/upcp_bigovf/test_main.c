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

extern Upci_Punit_t * DebugGetPunit(unsigned);


unsigned evtList[] = {      // List of performance events to collect
    PEVT_INST_ALL,
};
int numEvts = sizeof(evtList) / sizeof(int);


void Print_Punit_LLCounts(unsigned hEvtSet, Bgpm_Punit_LLHandles_t *phCtrs)
{
    //printf("Low Latency Counter Results:\n");
    int i;
    int numEvts = phCtrs->numCtrs;
    for (i=0; i<numEvts; i++) {
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("0x%016lx <= %s\n",
                Bgpm_Punit_LLRead(phCtrs->hCtr[i]),
                label);
    }
}



int main(int argc, char *argv[])
{
    printf("Test UPC_P Big Overflow capture thru signal handler\n");

    if (argc < 2) {
        printf("ERROR: Pass in number of threads\n");
        exit(-1);
    }
    int numThreads = atoi(argv[1]);
    printf("numThreads=%d, numEvts=%d\n", numThreads, numEvts);

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(evtList,numEvts,stderr)
    {
        //Upci_DumpKDebug(0);
#if 0
        upc_p_local->p_config       = 0x80000003fffc0000ULL;
        upc_p_local->counter_cfg[0] = 0x0000000000000002ULL;
        upc_p_local->counter[0]     = 0x0000000000002000ULL;
        upc_p_local->int_status_w1s = 0x0000000080000000ULL;
#else
        Bgpm_Init(BGPM_MODE_LLDISTRIB);

        int hEvtSet = Bgpm_CreateEventSet();   // get container for events.
        Bgpm_AddEventList(hEvtSet, evtList, numEvts);

        // LLDISTRIB mode doesn't enable PM interrupts, so let's trick it.
        Upci_Punit_t *pPunit = DebugGetPunit(hEvtSet);
        Upci_Punit_EnablePMInt(pPunit);

        if (Kernel_ProcessorID() == 0) {
            //Upci_Punit_Dump(0, pPunit);
        }

        Bgpm_Apply(hEvtSet);        // apply to hardware

        Bgpm_Punit_LLHandles_t hCtrs;
        Bgpm_Punit_GetLLHandles(hEvtSet, &hCtrs);
        Bgpm_Punit_LLResetStart(hCtrs.allCtrMask);

        // Target code to measure - produce a big overflow
         int pclks = 4000;
         while( pclks-- )
         {
             asm volatile("nop;");
             asm volatile("nop;");
             asm volatile("nop;");
             asm volatile("nop;");
         }

         Bgpm_Punit_LLStop(hCtrs.allCtrMask);

         #pragma omp barrier
         #pragma omp critical
         {
             Print_Punit_LLCounts(hEvtSet, &hCtrs);
         }
#endif
    }

    //Upci_A2PC_Val_t a2qry;
    //Kernel_Upci_A2PC_GetRegs(&a2qry);
    //Upci_A2PC_DumpRegs(&a2qry);

    //UPC_P_Dump_State(Kernel_ProcessorCoreID());

    //UPC_C_Dump_Counters(12,21);

    return(0);
}


