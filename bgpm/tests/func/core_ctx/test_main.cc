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

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();

int numThreads;
int numProcs;

#define LOOPS 50001

int numPasses = 3;

//#define INCLEXTRA
#ifdef INCLEXTRA
#define EXTRA(_d)  _d
int maxThreads = 32;
#else
#define EXTRA(_d)
int maxThreads = 64;
#endif


unsigned evtList[] = {
    PEVT_XU_PPC_COMMIT,
    PEVT_IU_TOT_ISSUE_COUNT,
    PEVT_XU_ANY_FLUSH,
    EXTRA(PEVT_XU_BR_MISPRED_COMMIT)
};
int numEvts = sizeof(evtList)/sizeof(unsigned);

struct Counts {
    uint64_t cmtInstr;
    uint64_t issInstr;
    uint64_t flushes;
    EXTRA(uint64_t extra;)
};
const char *extraLabel;

struct Results {
    Counts both;
    Counts user;
    Counts kernel;
};


Results data[4][64];



const Counts & Exercise(int numThreads, Bgpm_Context ctx)
{
    static Counts totals;
    totals = Counts();
    extraLabel = Bgpm_GetEventIdLabel(evtList[numEvts-1]);

    #pragma omp parallel default(none) num_threads(numThreads)      \
        shared(stderr,numThreads,numProcs,numFailures,ctx,totals,   \
        evtList,numEvts)
    {
        int hEvtSet = -1;
        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        hEvtSet = Bgpm_CreateEventSet();
        Bgpm_SetContext(hEvtSet, ctx);
        Bgpm_AddEventList(hEvtSet, evtList, numEvts);

        Bgpm_Apply(hEvtSet);

        #pragma omp barrier

        Bgpm_Start(hEvtSet);
        int loop;
        for (loop=0; loop<LOOPS; loop++) {
            Delay(1000);
            // cause some syscalls
            Upci_KDebug_t kDebug;
            Kernel_Upci_GetKDebug(&kDebug);
            Delay(1000);
        }
        Bgpm_Stop(hEvtSet);

        // Collect Results each event.
        #pragma omp critical (GetEvts)
        {
            uint64_t count;
            Bgpm_ReadEvent(hEvtSet, Bgpm_GetEventIndex(hEvtSet,PEVT_XU_PPC_COMMIT,0), &count);
            totals.cmtInstr += count;
            Bgpm_ReadEvent(hEvtSet, Bgpm_GetEventIndex(hEvtSet,PEVT_IU_TOT_ISSUE_COUNT,0), &count);
            totals.issInstr += count;
            Bgpm_ReadEvent(hEvtSet, Bgpm_GetEventIndex(hEvtSet,PEVT_XU_ANY_FLUSH,0), &count);
            totals.flushes += count;
            #ifdef INCLEXTRA
            Bgpm_ReadEvent(hEvtSet, numEvts-1, &count);
            totals.extra += count;
            #endif

            //fprintf(stderr, _AT_ " count=%011ld\n", count);
            //UPC_P_Dump_State(-1);
        }

        #pragma omp barrier
        Bgpm_Disable();
        #pragma omp barrier
    }

    return totals;
}


#define ABSDIFF(l,r) ((l)-(r) < 0 ? (r)-(l) : (l)-(r))





int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Check Core Collection Context Setting);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();
    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

    int thds;
    int pass;

    for (pass=0; pass<numPasses; ++pass) {
        TEST_CASE(Count instructions in all contexts);
        for (thds=1; thds<=maxThreads; thds*=2) {
            data[pass][thds].both = Exercise(thds, BGPM_CTX_BOTH);
        }

        TEST_CASE(Count instructions in user context);
        for (thds=1; thds<=maxThreads; thds*=2) {
            data[pass][thds].user = Exercise(thds, BGPM_CTX_USER);
        }

        TEST_CASE(Count instructions in kernel context);
        for (thds=1; thds<=maxThreads; thds*=2) {
            data[pass][thds].kernel = Exercise(thds, BGPM_CTX_KERNEL);
        }
    }



    fprintf(stderr, "numThds  Pass    BothInstr  Kernel+User        Delta    UserInstr  KernelInstr    AvgBInstr       AvgK+U     AvgDelta      AvgUser    AvgKernel\n");
    for (thds=1; thds<=maxThreads; thds*=2) {
        for (pass=0; pass<numPasses; ++pass) {
            long t = data[pass][thds].both.cmtInstr;
            long u = data[pass][thds].user.cmtInstr;
            long k = data[pass][thds].kernel.cmtInstr;
            fprintf(stderr, "%7d  %4d  %011ld  %011ld  %011ld  %011ld  %011ld  %011ld  %011ld  %011ld  %011ld  %011ld\n",
                    thds, pass, t,u+k,ABSDIFF(t,u+k),u,k,  t/thds,(u+k)/thds,ABSDIFF(t,u+k)/thds,u/thds,k/thds);
            TEST_CHECK_RANGE(ABSDIFF(t,u+k), 0, 5);
        }
        fprintf(stderr, "\n");
    }

    #ifdef INCLEXTRA
    fprintf(stderr, "\nnumThds  Pass              Instr     IssInstr        Delta      Flushes   %s  Delta\n", extraLabel);
    for (thds=1; thds<=maxThreads; thds*=2) {
        for (pass=0; pass<numPasses; ++pass) {
            Counts & both = data[pass][thds].both;
            Counts & user = data[pass][thds].user;
            Counts & kernel = data[pass][thds].kernel;
            fprintf(stderr, "%7d  %4d  both  %011ld  %011ld  %011ld  %011ld  %011ld  %011ld\n"
                            "               user  %011ld  %011ld  %011ld  %011ld  %011ld  %011ld\n"
                            "               kern  %011ld  %011ld  %011ld  %011ld  %011ld  %011ld \n",
                    thds, pass,
                    both.cmtInstr,   both.issInstr,   both.issInstr-both.cmtInstr,     both.flushes,   both.extra,    both.flushes-both.extra,
                    user.cmtInstr,   user.issInstr,   user.issInstr-user.cmtInstr,     user.flushes,   user.extra,    user.flushes-user.extra,
                    kernel.cmtInstr, kernel.issInstr, kernel.issInstr-kernel.cmtInstr, kernel.flushes, kernel.extra,  kernel.flushes-kernel.extra
                    );
        }
        fprintf(stderr, "\n");
    }
    #endif

    TEST_RETURN();

}

