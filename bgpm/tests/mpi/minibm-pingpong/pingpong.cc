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

#include <mpi.h>
#include "TestArgs.h"
#include "TestResults.h"
#include "TestBuffer.h"
#include "Timer.h"
#include "CommGroups.h"
using namespace std;





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


void RunTest(int argc, const char *argv[])
{
    string testName = "PingPong";
    
    TestArgs ta(argc, argv, BASEARGS PINGARGS);
    ta.ParseArgs();
    if (MPI::COMM_WORLD.Get_rank() == 0) {
        cout << ta.command << " arguments:" << endl;
        cout << ta << endl;
    }
    
    CommGroups grps(ta.groupFile);

    if (grps.iamExcluded) {
        return;
    }
      
    ProcessesCollection pc(grps.bmComm);
    pc.RegisterThisProcess();
    
    TestBuffer buffer0( MAX( MIN_BUFF_SIZE, ta.maxBuf));  // send/recv buffers
    TestBuffer buffer1( MAX( MIN_BUFF_SIZE, ta.maxBuf));
    //TestBuffer dummyBuffer(OPT_CACHE_SIZE);  // create extra to flush opt cache    
    
    //MPI::Request requests[2]; // requests for non-blocking comm
    //MPI::Status  status[2];   // status flags for wait_all
    
    MPI::Comm & tcomm = grps.testComm;
    PerfTimer timer;
   
    RankPair pair(ta, tcomm); // contains info this node and remote pair

    VERBOSE(cout << pc.myInfo.hostname << "(" << pc.myInfo.pid << "." << pc.myInfo.cpuNum 
                  << ") local/remote ranks: local(" 
                  << pair.localRank << "/" << pair.remoteRank
                  << ")  World(" 
                  << grps.GetWorldRank(pair.localRank) << "/" 
                  << grps.GetWorldRank(pair.remoteRank) << ")"
                  << endl
            );

    bool Pinger = pair.localRank < pair.remoteRank;  //  this process "pinger" or "ponger"
    int initBarrierCount = MAX(2, ta.loopBarrier);
    for (ta.bufSize = ta.StartSize(); !ta.Done(); ta.bufSize = ta.NextSize()) { 
        //buffer0.reset();
        //buffer1.reset();
        //cout << "buff0=" << hex << (void*)buffer0 << " buff0end=" << hex << (void*)((char*)buffer0+buffer0.size) << endl;
            
        // repeat while taking turns between pairs of ranks
        for (pair.Reset(); !pair.Done(); pair.Next()) {
        	
            REPEATFUNC(initBarrierCount, grps.bmComm.Barrier());  // all nodes barrier
        	
        	int loop;
        	if (pair.Myturn()) {
        
		        for (loop=0; loop<ta.warmCount; ++loop) {
		            if (Pinger) {               
		                //requests[0] = tcomm.Isend(buffer1, ta.bufSize, MPI::BYTE, pair.remoteRank, 1);
		                //requests[1] = tcomm.Irecv(buffer0, ta.bufSize, MPI::BYTE, pair.remoteRank, 0);
		                //MPI::Request::Waitall(2, requests, status);
		                tcomm.Send(buffer0, ta.bufSize, MPI::BYTE, pair.remoteRank, 1);
		                tcomm.Recv(buffer1, ta.bufSize, MPI::BYTE, pair.remoteRank, 0);
		            }
		            else {
		                //requests[0] = tcomm.Irecv(buffer0, ta.bufSize, MPI::BYTE, pair.remoteRank, 1);
		                //MPI::Request::Waitall(1, requests, status);               
		                //requests[0] = tcomm.Isend(buffer1, ta.bufSize, MPI::BYTE, pair.remoteRank, 0);
		                //MPI::Request::Waitall(1, requests, status);                
		                tcomm.Recv(buffer0, ta.bufSize, MPI::BYTE, pair.remoteRank, 1);
		                tcomm.Send(buffer1, ta.bufSize, MPI::BYTE, pair.remoteRank, 0);
		            }             
		             if (ta.loopBarrier) REPEATFUNC(ta.loopBarrier, grps.bmComm.Barrier());
		            //buffer0.advance(ta.bufSize);
		            //buffer1.advance(ta.bufSize);
		        }
		
		        timer.Reset();
		        if (!ta.loopBarrier) timer.Start();
		        
		        for (loop=0; loop<ta.loopCount; ++loop) {
		            if (ta.loopBarrier) timer.Start();
		            if (Pinger) {               
		                //requests[0] = tcomm.Isend(buffer1, ta.bufSize, MPI::BYTE, pair.remoteRank, 1);
		                //requests[1] = tcomm.Irecv(buffer0, ta.bufSize, MPI::BYTE, pair.remoteRank, 0);
		                //MPI::Request::Waitall(2, requests, status);
		                tcomm.Send(buffer0, ta.bufSize, MPI::BYTE, pair.remoteRank, 1);
		                tcomm.Recv(buffer1, ta.bufSize, MPI::BYTE, pair.remoteRank, 0);
		            }
		            else {
		                //requests[0] = tcomm.Irecv(buffer0, ta.bufSize, MPI::BYTE, pair.remoteRank, 1);
		                //MPI::Request::Waitall(1, requests, status);                
		                //requests[0] = tcomm.Isend(buffer1, ta.bufSize, MPI::BYTE, pair.remoteRank, 0);
		                //MPI::Request::Waitall(1, requests, status);               
		                tcomm.Recv(buffer0, ta.bufSize, MPI::BYTE, pair.remoteRank, 1);
		                tcomm.Send(buffer1, ta.bufSize, MPI::BYTE, pair.remoteRank, 0);
		            }             
		            if (ta.loopBarrier) {
		                timer.Stop();
		                REPEATFUNC(ta.loopBarrier, grps.bmComm.Barrier());
		            }
		            //buffer0.advance(ta.bufSize);
		            //buffer1.advance(ta.bufSize);
		        }
		        if (!ta.loopBarrier) timer.Stop();
        	}
        	
        	// not my turn - still make sure it does the barrier even if not timing.
        	else {
		        for (loop=0; loop<(ta.warmCount+ta.loopCount); ++loop) {
		             if (ta.loopBarrier) REPEATFUNC(ta.loopBarrier, grps.bmComm.Barrier());
		        }
        	}
        }
 	
        double elapsedTime = timer.ElapsedTime() / 2.0;  // 1/2 round trip time.   
        
        // Calculate BW and Latency values
        double bw = 0;
        double lat = 0;
        if (elapsedTime > 0) {  // just in case of other bug - prevent exception
            bw = (1.0 * ta.bufSize * ta.loopCount) / elapsedTime;  
            lat = elapsedTime / ta.loopCount;
        }
        
        // collect results
        // counting the amount sent by both processes in pair - but they don't send
        // simultaineously - so cut bandwidth in half.
        // if "taking turns" then there is only one pair out of each group sending data
        // simultaineously.  Tell collection to adjust total according to # of groups
        // (only the collection knows how many valid values it has totaled).
        int concur = 0;
        //cerr << "here1 (" << pc.procRank << ") " << grps.headComm.Get_size() << endl;
        if (ta.takeTurns) concur = grps.headGrp.Get_size();
        //cerr << "here2" << endl;

        ResultsCollection bwColl(pc, 0.5, concur);
        ResultValue bwVal(pc.procRank, bw);
        bwColl.CollectResultValues(bwVal);

        ResultsCollection latColl(pc);
        ResultValue latVal(pc.procRank, lat);
        latColl.CollectResultValues(latVal);

        // print results
        if (bwColl.procRank==0) { 
            if (ta.outputType & OPROCS) {
                bwColl.PrintProcessValues(cout, testName.c_str(), "-", ta.loopCount, ta.bufSize, "[MBs]", bwfactor);
                latColl.PrintProcessValues(cout, testName.c_str(), "-", ta.loopCount, ta.bufSize, "[usec]", latfactor);
            }
    
            if (ta.outputType & OSCRIPT) {
                PrintScriptBWResults(cout, testName.c_str(), "-", ta.loopCount, ta.bufSize, bwColl);
                PrintScriptLatResults(cout, testName.c_str(), "-", ta.loopCount, ta.bufSize, latColl);
            }
    
            if (ta.outputType & ONORMAL) {
                PrintStdResults(cout, testName.c_str(), "", ta.loopCount, 
                                ta.bufSize, grps, &bwColl, &latColl, &bwColl);
            }
        }

        // Let process 0 print output before starting next pass and calc new loopCount
        ta.UpdateLoopCount(grps.bmComm, elapsedTime); 
        //sleep(1);
        //grps.bmComm.Barrier();
    }
}


int main(int argc, const char *argv[])
{
    ios::sync_with_stdio(true); 
    MPI::Init();

    RunTest(argc, argv);  // call as sub function to allow destructors to clean up before
                //  finalize.   

    MPI::COMM_WORLD.Barrier(); // let excluded ranks finish
    MPI::Finalize();
    return(0);
}
