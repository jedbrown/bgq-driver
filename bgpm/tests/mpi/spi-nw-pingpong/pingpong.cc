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

#include "spi/include/upci/upci.h"

const Upci_Node_Parms_t* pNodeParms;



#if 0
typedef struct
{
unsigned Size[MPID_TORUS_MAX_DIMS]; /**< Max coordinates on the torus */
unsigned Coords[MPID_TORUS_MAX_DIMS]; /**< This node's coordinates */
unsigned isTorus[MPID_TORUS_MAX_DIMS]; /**< Do we have wraparound links? */
unsigned torus_dimension;/**< Actual dimension for the torus */
unsigned coreID; /**< Core+Thread info. Value ranges from 0..63 */
unsigned prank; /**< Physical rank of the node (irrespective of mapping) */
unsigned psize; /**< Size of the partition (irrespective of mapping) */
unsigned ppn; /**< Processes per node ("T+P" size) */
unsigned rankInPset;
unsigned sizeOfPset;
unsigned idOfPset;

unsigned clockMHz; /**< Frequency in MegaHertz */
unsigned memSize; /**< Size of the core memory in MB */

} MPIX_Hardware_t;


#include "mpix.h"
...
MPIX_Hardware_t hw;
MPIX_Get_hardware(&hw);

#endif





using namespace std;

TestArgs *pArgs;


#define VERBOSE(A) if (verbose) { A; }
#define MAX(a,b) (a < b ? b : a)
#define MIN(a,b) (a < b ? a : b)
#define REPEATFUNC(_count_,_func_) for (int i=0; i<_count_; ++i) _func_

#define PRINTERR(...) printf("ERROR(" _AT_ ") " __VA_ARGS__)
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }

#define xMicroSec 1000000

// SPI Objects
Upci_Mode_t upciMode;
Upci_Punit_t punit;

// Array to hold on to event handles.
Upci_Punit_EventH_t hEvt[24];
int cNum = 0;
int cycleEvtIdx = 0;  // keep index to event containing cycle count

// NW Counters
uint64_t nwCtlMask = 0;


#define ADD_PUNIT_EVENT(evt, thd, handle)  \
{                                    \
    int rc = Upci_Punit_AddEvent(&punit, evt, thd, &(handle));      \
    if (rc < 0) {                                                 \
        DIE("Add of event %d Failed, rc=%d\n", evt, rc);       \
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



int testRc = 0;
#define NWEXP( _link_, _ctr_, _exp1_, _exp2_ ) {      \
    uint64_t act = UPC_NW_ReadCtr(_link_, _ctr_);     \
    if (( act < _exp1_) || (act > _exp2_)) {                   \
        PRINTERR("NW Count[%d][%d]=0x%016lx Exp1=0x%016lx Exp2=0x%016lx\n", \
                _link_, _ctr_, act, _exp1_, _exp2_ );       \
        testRc++; \
    } \
}

#define IOEXP( _evt_, _exp1_, _exp2_ ) {      \
    uint64_t act = UPC_IO_ReadEvtCtr(_evt_);     \
    if (( act < _exp1_) || (act > _exp2_)) {                   \
        PRINTERR("IO Count " #_evt_ "=0x%016lx Exp1=0x%016lx Exp2=0x%016lx\n", \
                act, _exp1_, _exp2_ );       \
        testRc++; \
    } \
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

    Upci_Punit_Reset_Counts(&punit, UPCI_CTL_RESET | UPCI_CTL_DELAY);
        	
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

    //uint64_t val = DCRReadUser(MU_DCR(UPC_MU_COUNTER_CONTROL_RW));
    //printf("MU Control 1 = 0x%016lx\n", val);
    //val = DCRReadUser(MU_DCR(UPC_MU_CONFIG));
    //printf("MU Config 1  = 0x%016lx\n", val);

    //printf("nwCtlMask = 0x%016lx   nwCtl 1 = 0x%016lx\n", nwCtlMask, DCRReadUser(ND_500_DCR(CTRL_UPC_STOP)));

    UPC_IO_Start(Upci_GetNodeParms()->nodeConfig);
    UPC_NW_Start(nwCtlMask);
    Upci_Punit_Start(&punit, 0);

    //val = DCRReadUser(MU_DCR(UPC_MU_COUNTER_CONTROL_RW));
    //printf("MU Control 2 = 0x%016lx\n", val);
    //val = DCRReadUser(MU_DCR(UPC_MU_CONFIG));
    //printf("MU Config 2  = 0x%016lx\n", val);

    //printf("nwCtlMask = 0x%016lx   nwCtl 2 = 0x%016lx\n", nwCtlMask, DCRReadUser(ND_500_DCR(CTRL_UPC_STOP)));

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

    Upci_Punit_Stop(&punit, 0);
    UPC_NW_Stop(nwCtlMask);
    UPC_IO_Stop(Upci_GetNodeParms()->nodeConfig);

    //printf("nwCtlMask = 0x%016lx   nwCtl 3 = 0x%016lx\n", nwCtlMask, DCRReadUser(ND_500_DCR(CTRL_UPC_STOP)));

    //val = DCRReadUser(MU_DCR(UPC_MU_COUNTER_CONTROL_RW));
    //printf("MU Control 3 = 0x%016lx\n", val);
    //val = DCRReadUser(MU_DCR(UPC_MU_CONFIG));
    //printf("MU Config 3  = 0x%016lx\n", val);


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
    
    PrintCounts(&punit, hEvt, 0, cNum-1);
    double upciElapsedTime = (double)Upci_Punit_Event_Read(&punit,hEvt[cycleEvtIdx]);
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

    // find active link
    uint16_t link = 0;
    while (link < 10) {
        uint64_t act = UPC_NW_ReadCtr(link, 0);
        if (act > 0) break;
        link++;
    }
    if (link >= 10) {
        DIE("Didn't find an active link\n");
    }

    // Validate counts
    uint64_t expsends = 0x1a9c8; // old eager value @ mid Feb 2011 = 0x1aa2c;
    uint64_t exprcvs  = 0x1964;
    NWEXP(link,0,expsends,expsends);
    NWEXP(link,1,expsends,expsends);
    NWEXP(link,2,expsends,expsends);
    NWEXP(link,3,expsends,expsends);
    NWEXP(link,4,exprcvs,exprcvs);

    IOEXP(PEVT_MU_PKT_INJ, 0x189cUL, 0x189cUL);
    IOEXP(PEVT_MU_FIFO_PKT_RCV, 0x1964UL, 0x1964UL);

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
    Upci_Mode_Init(&upciMode, 0, UPC_CM_INDEP, Kernel_ProcessorCoreID());
    Upci_Punit_Init(&punit, &upciMode, Kernel_ProcessorCoreID());

    // Must use Cores threadID in Punit_AddEvent
    short thd = Kernel_ProcessorThreadID();
    ADD_PUNIT_EVENT(PEVT_INST_ALL, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_CYCLES, thd, hEvt[cNum++]);
    cycleEvtIdx = cNum-1;

    int rc = Upci_Punit_Apply(&punit);
    if (rc) {
        DIE("Upci_Punit_Apply failed; rc = %d\n", rc);
    }

    nwCtlMask = UPC_NW_CtrCtlMask(UPC_NW_ALL_LINKS, UPC_NW_ALL_CTRS);
    UPC_NW_EnableUPC();
    // Set all send counters to same user channels.
    uint8_t vcMask = (UPC_NW_VC_USER_DYNAMIC | UPC_NW_VC_USER_ESCAPE | UPC_NW_VC_USER_PRIORITY);
    UPC_NW_SetLinkCtrVChannel(UPC_NW_ALL_LINKS, 0, vcMask);
    UPC_NW_SetLinkCtrVChannel(UPC_NW_ALL_LINKS, 1, vcMask);
    UPC_NW_SetLinkCtrVChannel(UPC_NW_ALL_LINKS, 2, vcMask);
    UPC_NW_SetLinkCtrVChannel(UPC_NW_ALL_LINKS, 3, vcMask);
    UPC_NW_Clear(nwCtlMask);


    rc = RunTest(argc, argv);  // call as sub function to allow destructors to clean up before
                //  finalize.   
    MPI::Finalize();
    return(rc);
}
