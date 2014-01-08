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
#ifndef TestResults_h
#define TestResults_h

#include <vector>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include "CommGroups.h"

using namespace std;


extern double latfactor; 
extern double bwfactor;
extern string latLabel;
extern string bwLabel;

const int sendTag = 1;            // default MPI Send tag
const int recvTag = MPI::ANY_TAG;  // default MPI receive tag

#define HOSTNAME_LEN 50
struct ProcID {
    int    cpuNum;
    pid_t  pid;
    char   hostname[HOSTNAME_LEN];
};



struct ResultValue
{
    int rank;
    double num;

    ResultValue(int irank, double ivalue) {
        rank = irank;
        num = ivalue;
    }
};

inline ostream & operator<<(ostream & o, const ResultValue & s)
{
    o << s.rank << ":" << s.num;
    return o;
}

class ProcessesCollection
{
  public:
    ProcessesCollection(MPI::Comm &iComm): comm(iComm) {
        procRank = comm.Get_rank();
        collSize = comm.Get_size();
        pProcReg = procRank==0 ? new ProcID[collSize] : NULL;
        VERBOSE2(cout << "rank:" << procRank << " collSize:" << collSize << endl);
    }
    ~ProcessesCollection() { delete[] pProcReg; pProcReg=NULL; }
    void RegisterThisProcess();
    ProcID WhoAmI();
    ProcID & operator[](int index) { 
        return pProcReg[index];
    }

    MPI::Comm & comm;
    int procRank;
    int collSize;
    ProcID  myInfo;
    ProcID* pProcReg;   // index and recorded rank value will prob always be the same.
};

class ResultsCollection
{
  public:
    // in constructor: 
	// iTotalScale indicates a scaling factor to apply to totals calculation 
	//    It indicates the ratio of values reported which were running concurrently.  Thus, 
	//    after all values are SUMMed - if all the reported processes sent their data simultaniously, 
	//    Then the sum is the total.  However, in the case of pingpong - only one of the pair
	//    is sending at a time - so the ratio should be 0.5.  
	// iTotalConcurrent - if > 0 - indicates to use this value as the total number of concurrent
	//    operations instead of iTotalScale. This indicate the absolute number of concurrent
	//    operations instead of scale over the total collected.  Thus, the sum total should be 
	//    scaled by the iTotalConcurrent / #values.
    ResultsCollection(ProcessesCollection & prc, double iTotalScale=1.0, ulong iTotalConcurrent=0)  
    : comm(prc.comm), procRank(prc.procRank), collSize(prc.collSize), pc(prc), 
    totalScale(iTotalScale), totalConcurrent(iTotalConcurrent)
    {
        if (procRank == 0) values.reserve(collSize);
    }

    void RegisterThisProcess();
    void CollectResultValues(ResultValue curValue);
    void PrintProcessValues(ostream & o, const char *testName, const char* variation, 
                            long loops, long bufSize, const char *valLabel, double scaleFactor,
                            const char *tag="TPROC");

    ResultValue & MinValue() { return values[0]; }
    ResultValue & MaxValue() { return values[values.size()-1]; }
    ResultValue & MedianValue() { return values[values.size()/2]; }
    double TotalValue() { return totVal; }
    double DiffPercent() {
        long double percent;
        if (values[0].num == 0) { percent = 100; }
        else percent = (values[values.size()-1].num - values[0].num) / values[0].num * 100;
        return percent;
    }
    double AvgValue() { return avgVal; }
    double StdDev() { return stdDevVal; }
    double StdDevPercent() { return stdDevPercVal; }

    MPI::Comm & comm;
    int procRank;
    int collSize;
    ProcessesCollection & pc;

    vector<ResultValue> values;

    double avgVal;
    double totVal;
    double stdDevVal;
    double stdDevPercVal;
    double totalScale;
    ulong totalConcurrent;  // if 0, then all collected values ran concurrently.

    vector<double> bucketRanges;
    vector<ulong> buckets;
    double bucketDelta;
    ulong maxFrequency;

    void CalcBuckets();
};


void PrintScriptBWResults(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, ResultsCollection &coll, const char *tag="TBWDTA");
void PrintScriptLatResults(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, ResultsCollection &coll, const char *tag="TLATDTA");
void PrintStdResults(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, CommGroups &grps, 
                    ResultsCollection *pBwColl, ResultsCollection *pLatColl, 
                    ResultsCollection *pTotColl);
void PrintStdDeltaResults(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, long deltaVal, CommGroups &grps, 
                    ResultsCollection *pBwColl, ResultsCollection *pLatColl, 
                    ResultsCollection *pTotColl);
void PrintStdResultsWithStage(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, CommGroups &grps, const char *stageName, 
                    ResultsCollection *pBwColl, ResultsCollection *pLatColl, ResultsCollection *pTotColl);
void PrintStdResultsWithStageContinued(ostream & o, const char *stageName, 
                    ResultsCollection *pBwColl, ResultsCollection *pLatColl, ResultsCollection *pTotColl);


#endif
