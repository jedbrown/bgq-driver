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


/** \example spi/punit_pthread/test_main.c
 * UPCI is an internal interface and direct use by user programs is deprecated.
 * Use the Bgpm API instead.
 *
 * This is a temporary example of using the SPI with multiple cnk thread.
 *
 * Limitations
 *   - Modes on all threads must be equivalent (only mode 0 supported)
 *   - UPC Parm errors usually result in assert failures within code.
 *   - No punit shared events can be used (selected core wide events)
 *   - UPCI Error numbers are found in spi/include/upci/err.h
 *   - Locking required around
 *        Any counter write or reset operations
 *   - UPCI Objects must be thread local (__thread attribute)
 *   - # of simultaineous Punit counters:
 *        -- 6 per thread if 4 threads/core
 *        -- 12 per thread if 2 threads/core
 *        -- 12 per thread if 1 thread/core
 *   - Counting is asynchronous between threads.
 */

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

// Tell Upci_Mode_Init to skip the hardware initialization.  Only one
// thread can actually do the init, but all threads need a thread local
// equivalent thread object.
#define UPCI_MODE_SKIP_HWINIT
#include "spi/include/upci/mode.h"
#include "spi/include/kernel/upci.h"
#include "spi/include/upci/punit.h"
#include "spi/include/upci/upc_atomic.h"


UPC_Lock_t upcOpLock;  // any locking method ok - just using UPC_Lock fir convenience.


#define PRINTERR(...) printf("ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }
#define DIETHD(...) { PRINTERR(__VA_ARGS__); return(NULL); }


#define ADD_PUNIT_EVENT(evt, thd, handle)  \
{                                    \
    int rc = Upci_Punit_AddEvent(&punit, evt, thd, &(handle));      \
    if (rc < 0) {                                                 \
        DIETHD("Add of event %d Failed, rc=%d\n", evt, rc);       \
    } \
}



void PrintCounts(Upci_Punit_t *pPunit, Upci_Punit_EventH_t hEvt[], int startIdx, int endIdx)
{
    int i;
    for (i=startIdx; i<=endIdx; i++) {
        printf("0x%016lx <= %s\n",
                Upci_Punit_Event_Read(pPunit,hEvt[i]),
                Upci_Punit_GetEventLabel(pPunit,hEvt[i]));
    }
}




void CreateSomeEvents()
{
    // create some events.
#define DSIZE 1024
    volatile double target[DSIZE];
    volatile double source[DSIZE];
    short i,j;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }
    for (i=0; i<DSIZE; i++) {
        for (j=0; j<6; j++) {
            target[i] *= source[i];
            source[i] = j;
        }
    }
}



void* RunThread(void *ptr)
{
    //uint32_t tid_pthread = (long)ptr;

    // SPI Objects
    Upci_Mode_t upciMode;
    Upci_Punit_t punit;

    Upci_Mode_Init(&upciMode, 0, UPC_CM_INDEP, Kernel_ProcessorCoreID());

    Upci_Punit_Init(&punit, &upciMode, Kernel_ProcessorCoreID());

    // Array to hold on to event handles.
    Upci_Punit_EventH_t hEvt[24];
    int cNum = 0;


    // Add events in this order:
    //   1) l1p/Wakeup
    //   2) A2 XU
    //   other A2 3rd, Opcode events last (though none in this list)

    // Must use Cores threadID in Punit_AddEvent
    short thd = Kernel_ProcessorThreadID();
    ADD_PUNIT_EVENT(PEVT_L1P_BAS_LD, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_L1P_BAS_ST_WC, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_L1P_STRM_WRT_INVAL, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_XU_BR_COMMIT, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_CYCLES, thd, hEvt[cNum++]);

    int rc = Upci_Punit_Apply(&punit);
    if (rc) {
        DIETHD("Upci_Punit_Apply failed; rc = %d\n", rc);
    }

    else {
        Upci_Punit_Start(&punit, (UPCI_CTL_RESET | UPCI_CTL_DELAY));

        //-- The target of the test ---
        CreateSomeEvents();
        //------------------------------------------

        Upci_Punit_Stop(&punit, (UPCI_CTL_DELAY));

        // Locking below is just to serialize output for readability, not
        // because locking is needed to read the counters.
        UPC_Lock(&upcOpLock);
        printf("Counter Results:\n");
        PrintCounts(&punit, hEvt, 0, cNum-1);
        UPC_Unlock(&upcOpLock);

    }


    return(NULL);
}



#define NUMTHREADS 8

int main(int argc, char *argv[])
{
   printf("punit_multithread: SPI Example using multiple threads (temporary support)\n");

   long x;
   void *pRc;  // receive ptr to rc
   int rc;
   pthread_t tid[NUMTHREADS];
   pthread_attr_t attr;

   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   pthread_attr_setstacksize(&attr, 128*1024);

   for(x=0; x<NUMTHREADS; x++)
   {
      rc = pthread_create(&tid[x], &attr, RunThread, (void*)x);
      if (rc) { DIE("pthread_create error %d at %s\n", errno, _AT_ ); }
   }

   x = 0;
   RunThread((void*)x);

   for(x=0; x<NUMTHREADS; x++)
   {
      pthread_join(tid[x], &pRc);
      printf("tid[%d] rc = %d\n", (int)tid[x], *(int*)pRc);
   }
   printf("done\n");
   return 0;
}



