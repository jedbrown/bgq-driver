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
/**
 * \example mpi/pingpong-nw.cc
 *
 * simple MPI test to illustrate getting BGPM punit, network and I/O counters
 */

#include <iostream>
#include <mpi.h>
#include "bgpm/include/bgpm.h"
#include "spi/include/kernel/process.h"


using namespace std;

#define MAXBUF (1024*32)

double ClockSpeed()
{
    Personality_t personality;
    Kernel_GetPersonality(&personality, sizeof(Personality_t));
    return (personality.Kernel_Config.FreqMHz * 1.0e6);
}



int hPSet;  // Punit Event Set handle
int hL2Set;  // Punit Event Set handle
int hNWSet; // Network Event Set handle
int hIOSet; // I/O Event Set handle


#define xMicroSec 1000000


// Print counter results from evt set
void PrintCounts(char *name, int hEvtSet)
{
    printf("%s Counter Results:\n", name);
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        printf("    0x%016lx <= %s\n", cnt, Bgpm_GetEventLabel(hEvtSet, i));
    }
}



// Print  avg packets received and queue len.
void PrintNWCounts(char *name, int hEvtSet)
{
    printf("%s NW Counter Results:\n", name);
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);

    // Get the elapsed cycles if event exists.
    uint64_t elapsedNwCycles = 0;
    unsigned cycIdx = Bgpm_GetEventIndex(hEvtSet, PEVT_NW_CYCLES, 0);
    if (cycIdx < numEvts) {
        Bgpm_ReadEvent(hEvtSet, cycIdx, &elapsedNwCycles);
    }

    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        Bgpm_ReadEvent(hEvtSet, i, &cnt);
        printf("    0x%016lx <= %s\n", cnt, Bgpm_GetEventLabel(hEvtSet, i));

        // derive some nw metrics
        if (elapsedNwCycles > 0) {
            unsigned evtId = Bgpm_GetEventId(hEvtSet, i);
            // packet print arrival rate
            if ((evtId >= PEVT_NW_USER_PP_RECV) && (evtId <= PEVT_NW_COMMWORLD_COL_RECV)) {
                double arate = (double)cnt / (double)elapsedNwCycles;
                printf("    %16f <= %s avg arrival rate (chunks/nwcycle)\n", arate, Bgpm_GetEventLabel(hEvtSet, i));
            }
            else if ((evtId >= PEVT_NW_USER_PP_RECV_FIFO) && (evtId <= PEVT_NW_COMMWORLD_COL_RECV_FIFO)) {
                double quelen = (double)cnt / (double)elapsedNwCycles;
                printf("    %16f <= %s avg queue len (packets/nwcycle)\n", quelen, Bgpm_GetEventLabel(hEvtSet, i));
            }
        }
    }
}


#define LOOPS 10000

int RunTest(int argc, char *argv[])
{
    char buffer0[MAXBUF];  // send/recv buffer
    char buffer1[MAXBUF];  // send/recv buffer
    
    int local = MPI::COMM_WORLD.Get_rank();
    int numRanks = MPI::COMM_WORLD.Get_size();
    int remote = (local < numRanks/2) ? local + numRanks/2 : local - numRanks/2;
    bool pinger = local < remote;  //  this process "pinger" or "ponger"
    printf("ranks: local(%d) remote(%d)\n", local, remote);

    int loop;
    int xferSize = MAXBUF;
    Bgpm_ResetStart(hNWSet);
    Bgpm_ResetStart(hIOSet);
    Bgpm_ResetStart(hL2Set);
    Bgpm_ResetStart(hPSet);  // best to start last so resets of other starts not counted.
        	
    uint64_t startCycle = GetTimeBase();

    for (loop=0; loop<LOOPS; ++loop) {
        if (pinger) {
            MPI::COMM_WORLD.Send(buffer0, xferSize, MPI::BYTE, remote, 1);
            MPI::COMM_WORLD.Recv(buffer1, xferSize, MPI::BYTE, remote, 0);
        }
        else {
            MPI::COMM_WORLD.Recv(buffer0, xferSize, MPI::BYTE, remote, 1);
            MPI::COMM_WORLD.Send(buffer1, xferSize, MPI::BYTE, remote, 0);
        }
    }

	uint64_t stopCycle = GetTimeBase();

    Bgpm_Stop(hPSet);
    Bgpm_Stop(hL2Set);
    Bgpm_Stop(hIOSet);
    Bgpm_Stop(hNWSet);

    double elapsedTime = (stopCycle - startCycle) / 2.0 * ClockSpeed();  // 1/2 round trip time.
        
    // Calculate BW and Latency values
    double bw = 0;
    double lat = 0;
    if (elapsedTime > 0) {  // just in case of other bug - prevent exception
        bw = (1.0 * xferSize * LOOPS) / elapsedTime;
        lat = elapsedTime / LOOPS;
    }
    
    printf("Total Xfer (bytes) = %d\n"
           "Elapsed Time (1/2 round trip) = %0.2f us (%d loops)\n"
           "Bandwidth = %0.3f kB/sec\n"
           "Latency = %0.1f us/xfer\n",
           xferSize * LOOPS, elapsedTime * xMicroSec,
           LOOPS, bw / 1000, lat * xMicroSec);
    
    PrintCounts("Punit", hPSet);
    PrintCounts("L2", hL2Set);
    PrintNWCounts("Network", hNWSet);
    PrintCounts("I/O", hIOSet);

    return 0;
}



int main(int argc, char *argv[]) 
{
    ios::sync_with_stdio(true); 
    MPI::Init();

    // Configure Punit Events
    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    // Create event set and add events needed to calculate CPI
    hPSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hPSet, PEVT_CYCLES);
    Bgpm_AddEvent(hPSet, PEVT_INST_ALL);
    Bgpm_Apply(hPSet);

    hL2Set = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hL2Set, PEVT_L2_HITS);
    Bgpm_AddEvent(hL2Set, PEVT_L2_MISSES);
    Bgpm_AddEvent(hL2Set, PEVT_L2_PREFETCH);
    Bgpm_Apply(hL2Set);

    // Configure Network Events
    hNWSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_COMMWORLD_COL_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV_FIFO);
    Bgpm_AddEvent(hNWSet, PEVT_NW_CYCLES);

    // Must Attach to links specifid by mask
    // 1st thread wins link counting ownership
    if (Bgpm_Attach(hNWSet, UPC_NW_ALL_TORUS_LINKS, 0) == BGPM_WALREADY_ATTACHED) {
        printf("Another sw thread on node owns network link counters\n");
    }

    // Configure I/O Events
    hIOSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hIOSet, PEVT_MU_FIFO_PKT_RCV);
    Bgpm_AddEvent(hIOSet, PEVT_MU_PUT_PKT_RCV);
    // no apply or attach needed

    int rc = RunTest(argc, argv);

    MPI::Finalize();
    return(rc);
}
