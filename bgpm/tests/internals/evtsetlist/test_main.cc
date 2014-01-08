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

#define _TEST_INIT_C_
#include "spi/include/kernel/upci.h"
#include "../../test_utils.h"
#include "globals.h"  // replace the usual one for test

void bgpm::EvtSet::Dump(unsigned int) { }


TEST_INIT();
int numThreads;
int numProcs;

#define LOOPS 100


__thread unsigned randSeed;
inline int RandNum(int max) {
    double randVal = rand_r(&randSeed);
    //fprintf(stderr, _AT_ " max=%d, randVal=%f, RAND_MAX=%d\n", max, randVal, RAND_MAX);
    int randNum = 1 + (int)(((double)max) * (randVal/(RAND_MAX + 1.0)));
    return randNum;
}

#define MAXHANDLES 18000

UPC_Atomic_Int_t nullTypes = 0;
UPC_Atomic_Int_t baseTypes = 0;
UPC_Atomic_Int_t undefTypes = 0;
UPC_Atomic_Int_t deletedUndefTypes = 0;
unsigned maxReaderCount = 0;


int main(int argc, char *argv[])
{
    TEST_MODULE_PRINT0(Check for thread safe operations on EventSetList);

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }

    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();
    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

    fprintf(stderr, "sizeof(EvtSetList::ListRec)=%ld, maxEvtList=%d, memorySize=%ld\n",
            sizeof(EvtSetList::ListRec), procEvtSets.MaxSize(), procEvtSets.MaxSize()*sizeof(EvtSetList::ListRec));

    UPC_Atomic_Int_t done = 0;
    nullTypes = 0;
    baseTypes = 0;
    undefTypes = 0;

    #pragma omp parallel default(none) num_threads(numThreads) \
            shared(stderr,numFailures,numThreads,procEvtSets,done, \
                   nullTypes, baseTypes, undefTypes, \
                   deletedUndefTypes, maxReaderCount)
    {
        curHwThdId = Kernel_ProcessorID();
        thdInited = true;
        randSeed = GetTimeBase();

        int activeSets = 0;
        int handles[MAXHANDLES];
        int nextHandle = 0;
        int firstHandle = 0;
        int i,j,k;

        int procNum = omp_get_thread_num();

        if ((procNum % 4) == 0) {
            fprintf(stderr, _AT_ " I'm a Writer\n");
            for (i=0; i<LOOPS; i++) {
                int createNum = RandNum((procEvtSets.MaxSize()/numThreads) - activeSets);
                activeSets += createNum;
                //fprintf(stderr, _AT_ " createNum=%d, activeSets=%d\n", createNum, activeSets);

                for (j=0; j<createNum; j++) {
                    EvtSet *pEvtSet = new ES_Unassigned();
                    handles[nextHandle++] = procEvtSets.AddSet(pEvtSet);
                }
                //fprintf(stderr, _AT_ "\n");
                //procEvtSets.DumpCounts();



                int deleteNum = RandNum(activeSets);
                activeSets -= deleteNum;
                //fprintf(stderr, _AT_ " deleteNum=%d, activeSets=%d\n", deleteNum, activeSets);

                for (j=0; j<deleteNum; j++) {
                    procEvtSets.RmvSet(handles[firstHandle++]);
                }
                //fprintf(stderr, _AT_ "\n");
                //procEvtSets.DumpCounts();


                // shift remaining handles back to beginning
                for (j=firstHandle,k=0; j<nextHandle; j++,k++) {
                    handles[k] = handles[j];
                }
                firstHandle=0;
                nextHandle=activeSets;
            }

            procEvtSets.RmvAllSets(curHwThdId);

            UPC_Atomic_Incr(&done);
            mbar();
        }
        else {
            fprintf(stderr, _AT_ " I'm a Reader\n");
            while (!done) {
                volatile uint64_t data = 0;
                int idx = RandNum(procEvtSets.MaxSize());
                EvtSetHazardGuard guard(idx);
                EvtSet *pEvtSet = procEvtSets[idx];
                maxReaderCount = MAX(procEvtSets.ReaderCount(), maxReaderCount);
                if (pEvtSet == NULL) UPC_Atomic_Incr(&nullTypes);
                else if (pEvtSet->ESType() == EvtSet::ESType_Base) UPC_Atomic_Incr(&baseTypes);
                else if (pEvtSet->ESType() == EvtSet::ESType_Unassigned) {
                    Upci_Delay(5000);
                    data = pEvtSet->user1;
                    if (data) {
                        UPC_Atomic_Incr(&deletedUndefTypes);
                    }
                    UPC_Atomic_Incr(&undefTypes);
                }
                mbar();
            }
        }
    }

    fprintf(stderr, "nullTypes=%d, baseTypes=%d, undefTypes=%d, total=%d\n", nullTypes, baseTypes, undefTypes, nullTypes+baseTypes+undefTypes);
    fprintf(stderr, "deletedUndefTypes=%d, maxReaderCount=%d\n", deletedUndefTypes, maxReaderCount);
    TEST_CHECK_EQUAL(deletedUndefTypes, 0);

    procEvtSets.DumpCounts();


    TEST_RETURN();
}

