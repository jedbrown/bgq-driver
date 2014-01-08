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
/** \example spi/Omp-MatrixVec/test_main.c
 * This is a simple example of using the Upci Punit with OMP
 *
 * UPCI is an internal interface and direct use by user programs is deprecated.
 * Use the Bgpm API instead.
 *

 */

#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

#include "hwi/include/bqc/A2_inlines.h"
#include "spi/include/kernel/location.h"

#include "spi/include/upci/mode.h"
#include "spi/include/upci/punit.h"

#include "spi/include/upci/testlib/upc_p_debug.h"



#define GTB GetTimeBase()                //!< short name for convenience
typedef uint64_t cycle_t;                //!< cycle count types returned from GetTimeBase();



int numThreads;




#define WITH_UPCI

#define PRINTERR(...) printf("ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }



#define ADD_PUNIT_EVENT(evt, thd, handle)  \
{                                    \
    int rc = Upci_Punit_AddEvent(&(pEvts->punit), evt, thd, &(handle));    \
    if (rc < 0) {                                                 \
        DIE("Add of event %d Failed, rc=%d\n", evt, rc);          \
    } \
}


// Total Collection of event counters
uint64_t    eventSums[24];
const char *eventLabels[24];
short       numEvts = 0;
short       cycleIdx = 0;
short       instrIdx = 0;

void Print_EventSums()
{
    int i;
    for (i=0; i<numEvts; i++) {
        printf("%12ld <= %s\n", eventSums[i], eventLabels[i]);
    }
}





// Create stucture to hold UPCI objects needed for Punit
typedef struct sPevts {
    Upci_Mode_t         mode;
    Upci_Punit_t        punit;
    Upci_Punit_EventH_t hEvt[24];
    short               numEvts;
} Pevts_t;



// Configure and add desired events
void Pevts_Config(Pevts_t * pEvts)
{
    Upci_Mode_Init(&(pEvts->mode), 0, UPC_CM_INDEP, Kernel_ProcessorCoreID());
    Upci_Punit_Init(&(pEvts->punit), &(pEvts->mode), Kernel_ProcessorCoreID());

    // Add events in this order:
    //   1) l1p/Wakeup
    //   2) A2 XU
    //   other A2 3rd, Opcode events last (though none in this list)
    pEvts->numEvts = 0;
    short thd = Kernel_PhysicalHWThreadID();
    ADD_PUNIT_EVENT(PEVT_IU_TOT_ISSUE_COUNT, thd, pEvts->hEvt[pEvts->numEvts++]);
    ADD_PUNIT_EVENT(PEVT_IU_IS1_STALL_CYC, thd,  pEvts->hEvt[pEvts->numEvts++]);
    ADD_PUNIT_EVENT(PEVT_IU_IS2_STALL_CYC, thd,  pEvts->hEvt[pEvts->numEvts++]);
    ADD_PUNIT_EVENT(PEVT_IU_FXU_DEP_HIT_CYC, thd,  pEvts->hEvt[pEvts->numEvts++]);
    ADD_PUNIT_EVENT(PEVT_XU_PPC_COMMIT, thd,  pEvts->hEvt[pEvts->numEvts++]);
    cycleIdx = pEvts->numEvts;
    ADD_PUNIT_EVENT(PEVT_CYCLES, thd,  pEvts->hEvt[pEvts->numEvts++]);
    instrIdx = pEvts->numEvts;
    ADD_PUNIT_EVENT(PEVT_INST_ALL, thd,  pEvts->hEvt[pEvts->numEvts++]);
    ADD_PUNIT_EVENT(PEVT_INST_XU_ALL, thd,  pEvts->hEvt[pEvts->numEvts++]);
    //Upci_Punit_Dump(0, &(pEvts->punit));
    ADD_PUNIT_EVENT(PEVT_INST_QFPU_ALL, thd,  pEvts->hEvt[pEvts->numEvts++]);
    int i;
    for (i=0; i<pEvts->numEvts; i++) {
        eventLabels[i] = Upci_Punit_GetEventLabel(&(pEvts->punit),pEvts->hEvt[i]);
    }
    numEvts = pEvts->numEvts;


    int rc = Upci_Punit_Apply(&(pEvts->punit));
    if (rc) {
        DIE("Upci_Punit_Apply failed; rc = %d\n", rc);
    }
}



void mxv(int m, int n, double * a,
        double * b, double * c)
{
    int i, j;

    #pragma omp parallel default(none) num_threads(numThreads) \
            shared(m,n,a,b,c,eventSums,eventLabels,numEvts,cycleIdx,instrIdx) \
            private(i,j)
    {
        // Here is where the event initialization goes
        Pevts_t evts;
        Pevts_Config(&evts);


        Upci_Punit_Start(&(evts.punit), (UPCI_CTL_RESET | UPCI_CTL_DELAY));

        // ----------------
        // Target Test Loop
        #pragma omp for nowait
        for (i=0; i<m; i++)
        {
           a[i] = 0.0;
           for (j=0; j<n; j++) {
               a[i] += b[i*n+j]*c[j];
           }
        }
        // -----------------

        Upci_Punit_Stop(&(evts.punit), (UPCI_CTL_DELAY));


        // Here is where the data should be collected.
        #pragma omp critical (update_sum)
        {
            for (i=0; i<evts.numEvts; i++) {
                eventSums[i] += Upci_Punit_Event_Read(&(evts.punit),evts.hEvt[i]);
            }
        }

    }
}



int main(int argc, char *argv[])
{
    double *a,*b,*c;
    int i, j, m, n;

    printf("argc=%d\n",argc);
    if (argc < 3) {
      printf("ERROR: Please give m, n, numThreads\n");
      exit(-1);
    }

    m = atoi(argv[1]);
    n = atoi(argv[2]);
    numThreads = atoi(argv[3]);

    // init event Sums.
    for (i=0; i<24; i++) {
        eventSums[i] = 0;
    }

    printf("m x n = %d x %d, threads=%d, maxthread=%d\n",
            m, n, numThreads, omp_get_num_threads());

    if ((a=(double *)malloc(m*sizeof(double))) == NULL)
        perror("memory allocation for a");
    if ((b=(double *)malloc(m*n*sizeof(double))) == NULL)
        perror("memory allocation for b");
    if ((c=(double *)malloc(n*sizeof(double))) == NULL)
        perror("memory allocation for c");

    printf("Initializing matrix B and vector c\n");
    for (j=0; j<n; j++) {
        c[j] = 2.0;
    }
    for (i=0; i<m; i++) {
        for (j=0; j<n; j++) {
            b[i*n+j] = i;
        }
    }


    printf("Executing mxv function for m = %d n =  %d\n",m,n);
    cycle_t startc, deltac;
    startc = GTB;
    (void) mxv(m, n, a, b, c);
    deltac = GTB - startc;
    printf("total basetime cycles = %ld\n", deltac);

    Print_EventSums();
    printf("upci CPI = %lf\n",
            ((double)eventSums[cycleIdx])/eventSums[instrIdx]);

    free(a);free(b);free(c);
    return(0);
}
