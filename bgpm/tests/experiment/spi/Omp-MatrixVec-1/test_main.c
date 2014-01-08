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
/** \example spi/Omp-MatrixVec-1/test_main.c
 * This is a simple example of using the Upci Punit with compilable OMP, SE and TM sections.
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



#define GTB GetTimeBase()                //!< short name for convenience
typedef uint64_t cycle_t;                //!< cycle count types returned from GetTimeBase();



int numThreads;

uint64_t eventSums[24];
const char *eventLabels[24];



//#define WITH_UPCI
//#define WITH_TM



#define PRINTERR(...) printf("ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }

__thread void *pTmpMalloc;
#define MYMALLOC(_type_) \
    (_type_*)( pTmpMalloc = malloc(sizeof(_type_))) ? pTmpMalloc : DIE(ENOMEM, _AT_)



#define ADD_PUNIT_EVENT(evt, thd, handle)  \
{                                    \
    int rc = Upci_Punit_AddEvent(&punit, evt, thd, &(handle));    \
    if (rc < 0) {                                                 \
        DIE("Add of event %d Failed, rc=%d\n", evt, rc);          \
    } \
}





void mxv(int m, int n, double * a,
        double * b, double * c)
{
    int i, j;

#ifdef WITH_SPEC
        #pragma speculative sections default(none) num_threads(numThreads) \
                shared(m,n,a,b,c,eventSums,eventLabels) private(i,j)
#else
        #pragma omp parallel default(none) num_threads(numThreads) \
            shared(m,n,a,b,c,eventSums,eventLabels) private(i,j)
#endif
    {
        #ifdef WITH_UPCI
        // Here is where the event initialization goes
        // SPI Objects

        Upci_Mode_t upciMode;
        Upci_Punit_t punit;

        Upci_Mode_Init(&upciMode, 0, UPC_CM_INDEP, Kernel_ProcessorCoreID());
        Upci_Punit_Init(&punit, &upciMode, Kernel_ProcessorCoreID());

        // Arry to hold on to event handles.
        Upci_Punit_EventH_t hEvt[24];
        int cNum = 0;

        // Add events in this order:
        //   1) l1p/Wakeup
        //   2) A2 XU
        //   other A2 3rd, Opcode events last (though none in this list)
        short thd = Kernel_PhysicalHWThreadID();
        ADD_PUNIT_EVENT(PEVT_IU_TOT_ISSUE_COUNT, thd, hEvt[cNum++]);
        ADD_PUNIT_EVENT(PEVT_CYCLES, thd, hEvt[cNum++]);
        ADD_PUNIT_EVENT(PEVT_IU_IS1_STALL_CYC, thd, hEvt[cNum++]);

        int rc = Upci_Punit_Apply(&punit);
        if (rc) {
            DIE("Upci_Punit_Apply failed; rc = %d\n", rc);
        }

        Upci_Punit_Start(&punit, (UPCI_CTL_RESET | UPCI_CTL_DELAY));
#endif

        // ----------------
        // Target Test Loop

#if defined(WITH_TM)
        // ----------------
        // Target Test Loop
        #pragma omp for nowait
        for (i=0; i<m; i++) {
            #pragma tm_atomic
            {
            a[i] = 0.0;
            for (j=0; j<n; j++) {
               a[i] += b[i*n+j]*c[j];
            }
            }
        }
#elif defined(WITH_SPEC)
        #pragma speculative section for
        for (i=0; i<m; i++) {
            a[i] = 0.0;
            for (j=0; j<n; j++) {
               a[i] += b[i*n+j]*c[j];
            }
        }
#else
         #pragma omp for nowait
         for (i=0; i<m; i++) {
            a[i] = 0.0;
            for (j=0; j<n; j++) {
                a[i] += b[i*n+j]*c[j];
            }
         }
#endif
         // -----------------


#ifdef WITH_UPCI
        Upci_Punit_Stop(&punit, (UPCI_CTL_DELAY));



        // Here is where the data should be collected.
        #pragma omp critical (update_sum)
        {
            for (i=0; i<cNum; i++) {
                eventSums[i] += Upci_Punit_Event_Read(&punit,hEvt[i]);
                eventLabels[i] = Upci_Punit_GetEventLabel(&punit,hEvt[i]);
            }
        }
#endif
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

#ifdef WITH_UPCI
    printf("upci stallcyc,cycles,instr,CPI = %ld, %ld, %ld, %lf\n",
            eventSums[2], eventSums[1], eventSums[0], ((double)eventSums[1])/eventSums[0]);
#endif

    free(a);free(b);free(c);
    return(0);
}
