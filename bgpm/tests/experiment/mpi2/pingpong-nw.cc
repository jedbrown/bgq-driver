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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
#include "spi/include/kernel/process.h"


using namespace std;

#define MAXBUF (1024*32)

double ClockSpeed()
{
    Personality_t personality;
    Kernel_GetPersonality(&personality, sizeof(Personality_t));
    return (personality.Kernel_Config.FreqMHz * 1.0e6);
}




#define xMicroSec 1000000





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
    

    return 0;
}



int main(int argc, char *argv[]) 
{
    ios::sync_with_stdio(true); 
    MPI::Init();

    int rc = RunTest(argc, argv);

    MPI::Finalize();
    return(rc);
}
