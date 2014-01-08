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

#include <iostream>
#include <mpi.h>
#include "TestBuffer.h"
#include "Timer.h"
#include "TestArgs.h"

#include "bgpm/include/bgpm.h"
#include "../../test_utils.h"

using namespace std;
int ovfs=0;

TestArgs *pArgs;

int hPUSet;
int hNWSet;
int hIOSet;
Bgpm_Punit_Handles_t hPUCtrs;

const Upci_Node_Parms_t* pNodeParms;


#define VERBOSE(A) if (verbose) { A; }
#define MAX(a,b) (a < b ? b : a)
#define MIN(a,b) (a < b ? a : b)
#define REPEATFUNC(_count_,_func_) for (int i=0; i<_count_; ++i) _func_

#define xMicroSec 1000000


void PrintCounts(unsigned hEvtSet)
{
    int i = Bgpm_NumEvents(hEvtSet);
    //fprintf(stderr, "set %d numevents = %d\n", hEvtSet, i);
    for (i=0; i<Bgpm_NumEvents(hEvtSet); i++) {
        uint64_t val;
        Bgpm_ReadEvent(hEvtSet, i, &val);
        fprintf(stderr, "0x%016lx <= %s\n", val, Bgpm_GetEventLabel(hEvtSet, i));
    }
}



int testRc = 0;
#define NWEXP( _evtset_, _link_, _idx_, _exp1_, _exp2_ ) {      \
    uint64_t act; \
    uint64_t lmask = UPC_NW_LINK_bit(_link_); \
    Bgpm_NW_ReadLinkEvent(_evtset_, _idx_, (UPC_NW_LinkMasks)lmask, &act);     \
    if (( act < _exp1_) || (act > _exp2_)) {                   \
        PRINTERR("NW Count[%d][%d]=0x%016lx Exp1=0x%016lx Exp2=0x%016lx\n", \
                _link_, _idx_, act, _exp1_, _exp2_ );       \
        testRc++; \
    } \
}

#define IOEXP( _evtset_, _evtid_, _exp1_, _exp2_ ) {      \
    uint64_t act; \
    unsigned idx = Bgpm_GetEventIndex(_evtset_, _evtid_, 0); \
    Bgpm_ReadEvent(_evtset_, idx, &act);     \
    if (( act < _exp1_) || (act > _exp2_)) {                   \
        PRINTERR("IO Count " #_evtid_ "= 0x%016lx Exp1=0x%016lx Exp2=0x%016lx\n", \
                act, _exp1_, _exp2_ );       \
        testRc++; \
    } \
}

void IOOvfHandler(int hIOSet, uint64_t address, uint64_t ovfVector, const ucontext_t *pContext)
{
    // This is the overflow handler for IO events.
    // Will get invoked whenever any IO event overflows.
    // Lets increment Number of overflows here.
    ovfs++;
}




/*
 *  PingPong (like bisection, but with unidirectional measurement
 *
 *  This test cuts the tree dimensions  in half (in a virtual sense)
 *  Having created the halves, it transmits the data to the other side
 *  in a process pair.
 *
 *  The nodes use MPI_Sendrecv() to transceive to its pair.  The operation
 *  is timed on each node and that time is used to calculate the local
 *  bandwidth: DATA_SENT/TIME.  This is then added globally and reported
 *  as the final result.
 *
 *  Latency  = 1/2 round trip time.
 *  BW       = sent / 1/2 roud trip time. (1/2 sent + received)
 *  Total BW = SUM(BWs) / 2. ( div 2 because - what was sent was counted as recieved by the other process)
 */



int RunTest(int argc, const char *argv[])
{

    TestBuffer buffer0( pArgs->maxBuf );  // send/recv buffers
    TestBuffer buffer1( pArgs->maxBuf );

    PerfTimer timer;


    int local = MPI::COMM_WORLD.Get_rank();
    int numRanks = MPI::COMM_WORLD.Get_size();
    int remote = (local < numRanks/2) ? local + numRanks/2 : local - numRanks/2;
    bool pinger = local < remote;  //  this process "pinger" or "ponger"
    printf("ranks: local(%d) remote(%d)\n", local, remote);

    //int initBarrierCount = 2;
    //REPEATFUNC(initBarrierCount, MPI::COMM_WORLD.Barrier());  // all nodes barrier

    int rc = 0;
    Bgpm_Reset(hPUSet);
    Bgpm_Reset(hNWSet);
    Bgpm_Reset(hIOSet);

    int loop;
    pArgs->bufSize = pArgs->StartSize();
    long xferSize = pArgs->bufSize;

    for (loop=0; loop<pArgs->warmCount; ++loop) {
        if (pinger) {
            MPI::COMM_WORLD.Send(buffer0, xferSize, MPI::BYTE, remote, 1);
            MPI::COMM_WORLD.Recv(buffer1, xferSize, MPI::BYTE, remote, 0);
        }
        else {
            MPI::COMM_WORLD.Recv(buffer0, xferSize, MPI::BYTE, remote, 1);
            MPI::COMM_WORLD.Send(buffer1, xferSize, MPI::BYTE, remote, 0);
        }
    }


    //Bgpm_DebugDumpShrMem();

    Bgpm_Start(hIOSet);
    if ((rc = Bgpm_Start(hNWSet))) { DIE("Network start rc = %d\n", rc); }
    Bgpm_Start(hPUSet);

    //UPC_NW_Dump();


    timer.Reset();
    timer.Start();

    for (loop=0; loop<pArgs->loopCount; ++loop) {
        if (pinger) {
            MPI::COMM_WORLD.Send(buffer0, xferSize, MPI::BYTE, remote, 1);
            MPI::COMM_WORLD.Recv(buffer1, xferSize, MPI::BYTE, remote, 0);
        }
        else {
            MPI::COMM_WORLD.Recv(buffer0, xferSize, MPI::BYTE, remote, 1);
            MPI::COMM_WORLD.Send(buffer1, xferSize, MPI::BYTE, remote, 0);
        }
    }

    timer.Stop();

    Bgpm_Stop(hPUSet);
    if ((rc = Bgpm_Stop(hNWSet))) { DIE("Network stop rc = %d\n", rc); }
    Bgpm_Stop(hIOSet);


    double elapsedTime = timer.ElapsedTime() / 2.0;  // 1/2 round trip time.

    // Calculate BW and Latency values
    double bw = 0;
    double lat = 0;
    if (elapsedTime > 0) {  // just in case of other bug - prevent exception
        bw = (1.0 * xferSize * pArgs->loopCount) / elapsedTime;
        lat = elapsedTime / pArgs->loopCount;
    }

    printf("Total Xfer (bytes) = %ld\n"
           "Elapsed Time (1/2 round trip) = %0.2f us (%ld loops)\n"
           "Bandwidth = %0.3f kB/sec\n"
           "Latency = %0.1f us/xfer\n",
           xferSize * pArgs->loopCount, elapsedTime * xMicroSec, pArgs->loopCount, bw / 1000, lat * xMicroSec);

    uint64_t val;
    int cycleIdx = Bgpm_GetEventIndex(hPUSet, PEVT_CYCLES, 0);
    if (cycleIdx >= Bgpm_NumEvents(hPUSet)) {
        DIE("PEVT_CYCLES not found\n");
    }
    Bgpm_ReadEvent(hPUSet, cycleIdx, &val);
    double upciElapsedTime = val;
    upciElapsedTime = upciElapsedTime / 2.0 / pNodeParms->cpuSpeed;
    printf("Elapsed Time from cycle counts = %0.2f us\n", upciElapsedTime);

#ifdef __VERBOSE__
    UPC_NW_CtrArray_t nwCtrs;
    UPC_NW_GetCtrArray(&nwCtrs);
    UPC_NW_DumpCtrArray(0, &nwCtrs);

    UPC_IO_SnapShot_t ioCtrs;
    UPC_IO_GetSnapShot(&ioCtrs);
    UPC_IO_DumpSnapShot(0, &ioCtrs);
#endif

    PrintCounts(hPUSet);
    PrintCounts(hNWSet);
    PrintCounts(hIOSet);

    //fprintf(stderr, _AT_ "\n");

    // find active link
    uint16_t link = 0;
    while (link < 10) {
        uint64_t act = 0;
        uint64_t lmask = UPC_NW_LINK_bit(link);
        if ((rc = Bgpm_NW_ReadLinkEvent(hNWSet, 0, (UPC_NW_LinkMasks)lmask, &act)) != 0) { DIE("rc=%d\n", rc); }
        //fprintf(stderr, _AT_ " lmask=0x%016lx, act=%ld\n", lmask, act);
        if (act > 0) break;
        link++;
    }
    if (link >= 10) {
        DIE("Didn't find an active link\n");
    }

    //fprintf(stderr, _AT_ "\n");

    // Validate counts
    uint64_t expsends = 0x1a9c8; // old eager value @ mid Feb 2011 = 0x1aa2c;
    uint64_t exprcvs  = 0x1964;
    NWEXP(hNWSet,link,0,expsends,expsends);
    NWEXP(hNWSet,link,1,expsends,expsends);
    NWEXP(hNWSet,link,2,expsends,expsends);
    NWEXP(hNWSet,link,3,expsends,expsends);
    NWEXP(hNWSet,link,4,exprcvs,exprcvs);

    //IOEXP(hIOSet, PEVT_MU_PKT_INJ, 0x189cUL, 0x189cUL);
    //IOEXP(hIOSet, PEVT_MU_FIFO_PKT_RCV, 0x1964UL, 0x1964UL);
    IOEXP(hIOSet, PEVT_MU_PKT_INJ, 0x188aUL, 0x189dUL);
    IOEXP(hIOSet, PEVT_MU_FIFO_PKT_RCV, 0x189aUL, 0x195fUL);

    //fprintf(stderr, _AT_ "\n");

    return testRc;
}



int main(int argc, const char *argv[])
{

    uint16_t linkMask = UPC_NW_GetEnabledLinks();
    fprintf(stderr, "%s: enabledLinks=0x%04x\n", WhoAmI(), linkMask);

#if 0
    // debug what can be written (via printf in UPC routine below)
    unsigned l, c;
    for (c=0; c<5; c++) {
        for (l=0; l<11; l++) {
             UPC_NW_SetCtrVChannel(l, c, 0xFF);
         }
     }
     exit(-1);
#endif


    ios::sync_with_stdio(true);
    MPI::Init();
    pNodeParms = Upci_GetNodeParms();

    // Let's work around boot issue when multiple args passed.
    const char *useArgs[] = {
            "",
            "-l", "100",
            "-s", "32768",
            "-w", "4"
    };
    useArgs[0] = argv[0];
    int useArgc = sizeof(useArgs)/sizeof(const char *);

    string validArgs = "-h -v -s -w -l";
    pArgs = new TestArgs(useArgc, useArgs, validArgs.c_str());
    pArgs->ParseArgs();
    if (MPI::COMM_WORLD.Get_rank() == 0) {
        cout << pArgs->command << " arguments:" << endl;
        cout << *pArgs << endl;
    }

    fprintf(stderr, _AT_ "\n");

    // Configure upc punit
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    hPUSet = Bgpm_CreateEventSet();
    //fprintf(stderr, _AT_ " hPUSet=%d\n", hPUSet);
    fprintf(stderr, _AT_ " PEVT_INST_ALL=%d\n", PEVT_INST_ALL );
    Bgpm_AddEvent(hPUSet, PEVT_INST_ALL);
    Bgpm_AddEvent(hPUSet, PEVT_CYCLES);
    Bgpm_Apply(hPUSet);
    Bgpm_Punit_GetHandles(hPUSet, &hPUCtrs);

    //fprintf(stderr, _AT_ "\n");

    hNWSet = Bgpm_CreateEventSet();
    //fprintf(stderr, _AT_ " hNWSet=%d\n", hNWSet);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV_FIFO);
    Bgpm_AddEvent(hNWSet, PEVT_NW_CYCLES);

    //UPC_NW_Dump();

    Bgpm_Attach(hNWSet, UPC_NW_ALL_TORUS_LINKS, 0);

    //fprintf(stderr, _AT_ "\n");
   //Bgpm_DumpEvtSet(hNWSet, 0);
    //UPC_NW_Dump();


    hIOSet = Bgpm_CreateEventSet();
    //fprintf(stderr, _AT_ " hIOSet=%d\n", hIOSet);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PKT_INJ);
    Bgpm_AddEvent(hIOSet, PEVT_MU_MSG_INJ);
    Bgpm_AddEvent(hIOSet, PEVT_MU_FIFO_PKT_RCV);
    Bgpm_AddEvent(hIOSet, PEVT_MU_RGET_PKT_RCV);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PUT_PKT_RCV);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT0_16B_WRT);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT0_32B_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT0_64B_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT0_128B_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT1_16B_WRT);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT1_32B_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT1_64B_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT1_128B_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT2_16B_WRT);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT2_32B_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT2_64B_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT2_128B_RD);

    //fprintf(stderr, _AT_ "\n");
    Bgpm_SetOverflow(hIOSet, Bgpm_GetEventIndex(hIOSet, PEVT_MU_PKT_INJ, 0), 1000);
    Bgpm_SetOverflow(hIOSet, Bgpm_GetEventIndex(hIOSet, PEVT_MU_PORT0_16B_WRT, 0), 1000);
    Bgpm_SetOverflow(hIOSet, Bgpm_GetEventIndex(hIOSet, PEVT_MU_PORT2_128B_RD, 0), 1000);
    Bgpm_SetOverflowHandler(hIOSet, IOOvfHandler);
    Bgpm_Apply(hIOSet);

    int rc = RunTest(argc, argv);  // call as sub function to allow destructors to clean up before
                //  finalize.
//    printf("Number of IO Overflows: %d\n",ovfs); 
    MPI::Finalize();
    return(rc);
}
