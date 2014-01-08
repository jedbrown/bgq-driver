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

using namespace std;

TestArgs *pArgs;

int hPUSet;
int hNWSet;
int hIOSet;

const Upci_Node_Parms_t* pNodeParms;


#define VERBOSE(A) if (verbose) { A; }
#define MAX(a,b) (a < b ? b : a)
#define MIN(a,b) (a < b ? a : b)
#define REPEATFUNC(_count_,_func_) for (int i=0; i<_count_; ++i) _func_

#define PRINTERR(...) printf("ERROR(" _AT_ ") " __VA_ARGS__)
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }

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


    Bgpm_Start(hIOSet);
    if ((rc = Bgpm_Start(hNWSet))) { DIE("Network start rc = %d\n", rc); }
    Bgpm_Start(hPUSet);

    timer.Reset();
    timer.Start();

    // Sends only - try and fill up MU as high as possible
    for (loop=0; loop<pArgs->loopCount; ++loop) {
        if (pinger) {
            MPI::COMM_WORLD.Send(buffer0, xferSize, MPI::BYTE, remote, 1);
        }
        else {
            MPI::COMM_WORLD.Recv(buffer0, xferSize, MPI::BYTE, remote, 1);
        }
    }

    timer.Stop();

    Bgpm_Stop(hPUSet);
    if ((rc = Bgpm_Stop(hNWSet))) { DIE("Network stop rc = %d\n", rc); }
    Bgpm_Stop(hIOSet);


    //double elapsedTime = timer.ElapsedTime() / 2.0;  // 1/2 round trip time.

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


    return testRc;
}



int main(int argc, const char *argv[])
{
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

    // Configure upc punit
    Bgpm_Init(BGPM_MODE_HWDISTRIB);
    hPUSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hPUSet, PEVT_INST_ALL);
    Bgpm_AddEvent(hPUSet, PEVT_CYCLES);
    Bgpm_Apply(hPUSet);

    hNWSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV_FIFO);
    Bgpm_AddEvent(hNWSet, PEVT_NW_CYCLES);
    Bgpm_Attach(hNWSet, UPC_NW_ALL_LINKS, 0);

    hIOSet = Bgpm_CreateEventSet();
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
    Bgpm_AddEvent(hIOSet, PEVT_MU_SLV_PORT_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_SLV_PORT_WRT);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT0_PEND_WRT);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT0_PEND_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT1_PEND_WRT);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT1_PEND_RD);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT2_PEND_WRT);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PORT2_PEND_RD);


    int rc = RunTest(argc, argv);  // call as sub function to allow destructors to clean up before
                //  finalize.
    MPI::Finalize();
    return(rc);
}
