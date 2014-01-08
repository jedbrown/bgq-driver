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

#include <sys/time.h>
#include <ctime>
#include "TestArgs.h"
#include "TestResults.h"
#include "CommGroups.h"
#include "spi/include/kernel/location.h"





bool AscendingValues(const ResultValue & a, const ResultValue & b) { return(a.num < b.num); }

const int NUMBUCKETS = 8;

double latfactor = 1.0e6; 
double bwfactor = 1.0/(1024.0 * 1024.0);
string latLabel = "[usec]";
string bwLabel = "[MBs]";



void ProcessesCollection::RegisterThisProcess()
{
    myInfo = WhoAmI();
    comm.Gather(&myInfo,sizeof(ProcID),MPI::BYTE,pProcReg,sizeof(ProcID),MPI::BYTE,0);
}

ProcID ProcessesCollection::WhoAmI()
{
    ProcID info;
    info.pid = getpid();
    info.cpuNum = Kernel_ProcessorThreadID();

    //Personality_t personality;
    //Kernel_GetPersonality(&personality, sizeof(Personality_t));

    
    strcpy(info.hostname, "??");

    return info;
}


void ResultsCollection::CollectResultValues(ResultValue curValue)
{
    ResultValue *pValueList = procRank==0 ?  (ResultValue*)new char[collSize*sizeof(ResultValue)] : NULL;

    comm.Gather(&curValue,sizeof(ResultValue),MPI::BYTE,
                pValueList,sizeof(ResultValue),MPI::BYTE,0);

    if (procRank == 0) {
        for (int i=0; i<collSize; ++i) {
            // disregard negative values (not measured)
            if (pValueList[i].num >= 0) {
                values.push_back(pValueList[i]);
            }
        }
        delete pValueList;
        pValueList = NULL;
        
        // make sure there is a list one value stored.
        if (values.size() == 0) {
            ResultValue zeroVal(0,0);
            values.push_back(zeroVal);
        }   
        
        // VERBOSE2(cerr << "ResultsCollection: values.size()=" << values.size() << endl);
        // VERBOSE2(PRINT_ELEMENTS(cerr, values, "values:"));

        // do the long calculations here - once.
        sort (values.begin(), values.end(), AscendingValues);

        double sumValue = 0;
        for (unsigned i=0; i<values.size(); ++i) {
            sumValue += values[i].num;
        }
        avgVal = sumValue / values.size();
        totVal = sumValue;
    	if (totalConcurrent > 0) {
    		// cerr << "totalConcurrent=" << totalConcurrent << " values.size=" << values.size() << endl;
    		if (totalConcurrent < values.size()) {
    			totVal = totVal / values.size() * totalConcurrent;
    		}
    	}
    	else { 
    		totVal *= totalScale;
    		totalConcurrent = ulong(values.size() * totalScale);
    	}

    	// calc stddev
        sumValue = 0;
        for (unsigned i=0; i<values.size(); ++i) {
        	double diff = values[i].num - avgVal;
            sumValue += diff * diff; 
        }
        if (sumValue < 0) sumValue *= -1.0;
        sumValue /= values.size();
        stdDevVal = sqrt(sumValue);   

        sumValue = 0;
        for (unsigned i=0; i<values.size(); ++i) {
            if ((values[i].num >= (avgVal-stdDevVal)) &&
                (values[i].num <= (avgVal+stdDevVal))) {
                sumValue++;
            }
        }
        stdDevPercVal = sumValue / values.size() * 100;

        CalcBuckets();
        
    }
}


void ResultsCollection::CalcBuckets()
{
    bucketDelta = (values[values.size()-1].num - values[0].num) / NUMBUCKETS;
    for (int i=0; i<NUMBUCKETS; ++i) {
        bucketRanges.push_back(values[0].num + bucketDelta*i);
        buckets.push_back(0);
        //cout << values[0].num + bucketDelta*i << endl;
    }
    bucketRanges.push_back(values[values.size()-1].num);
    //PRINT_ELEMENTS(cout, bucketRanges, "bucketRanges:");
    // The top range has no value - buckets collected inclusive of top of range value.
    // Thus, the bottom range is [ bucketRange[0], bucketRange[1] ]
    //    The next range is      ( bucketRange[1], bucketRange[2] ]
    // so there are 1 more bucket range values than buckets.

    unsigned curBucketIndex = 0;
    unsigned i=0;
    while ((i<values.size()) && (curBucketIndex<buckets.size())) {
        if (values[i].num <= bucketRanges[curBucketIndex+1]) {
            buckets[curBucketIndex]++;
            ++i;
        }
        else curBucketIndex++;
    }
    // get max frequency
    maxFrequency = 0;
    for (curBucketIndex = 0; curBucketIndex < buckets.size(); curBucketIndex++) {
    	if (maxFrequency < buckets[curBucketIndex]) maxFrequency = buckets[curBucketIndex];
    }
    //PRINT_ELEMENTS(cout, buckets, "buckets:");
}

void ResultsCollection::PrintProcessValues(ostream & o, const char *testName, const char *variation, 
    long loops, long bufSize, const char *valLabel, double scaleFactor, const char *tag)
{
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        o << right << "TPHDR: " << setw(25) << "test" << " ";
        o << setw(5) << "var" << " ";
        o << setw(9)  << "#bytes" << " ";
        o << setw(9)  << "#loops" << " ";
        o << setw(8)  << "value" << " ";
        o << setw(6) << left << "metric" << right <<" ";
        o << setw(5)  << "rank" << " ";
        o << "host(pid.cpu)" << endl;
    }
    o.setf(ios::fixed);
    o.precision(2);
    o.setf(ios::showpoint);
    vector<ResultValue>::const_iterator pos;
    for (pos=values.begin(); pos!=values.end(); ++pos) {
        o << right << tag << ": " << setw(25) << testName << " ";
        o << setw(5) << variation << " ";
        o << setw(9) << bufSize << " ";
        o << setw(9) << loops << " ";        
        o << setw(8) << pos->num * scaleFactor << " ";
        o << setw(6) << left << valLabel << right << " ";  
        o << setw(5) << pos->rank << " ";
        ProcID & p = pc[pos->rank];
        o << p.hostname << "(" << p.pid << "." << p.cpuNum << ")";
        o << endl; 
    }
}



void PrintScriptLatResults(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, ResultsCollection &coll, const char *tag)
{
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        o << right << "TLATHDR:    " << setw(25) << "test" << " ";
        o << setw(5) << "var" << " ";
        o << setw(5) << "#proc" << " ";
        o << setw(9) << "#bytes" << " ";
        o << setw(9) << "#loops" << " ";
        o << setw(9) << "min[usec]"  << " ";
        o << setw(9) << "avg[usec]" << " ";
        o << setw(9) << "max[usec]"  << " ";
        o << setw(13) << " ";                 // add to align with BW total results - easier to read 
        o << setw(6) << "%diff"       << " ";
        o << setw(9) << "stddev"   << " ";
        o << setw(19)<< left << "max_process" << right << " ";
        o << setw(10) << "FBktWidth"   << " ";
        o << setw(5) << "FMax"     << " ";
        o << setfill('0');
        for (int i=0; i<NUMBUCKETS; ++i) {
            o << "f" << setw(4) << i << " ";
        }
        o << setfill(' ');
        o << endl;
    }
    
    string otag = tag;
    otag += ":";
    o.setf(ios::fixed);
    o.precision(2);
    o.setf(ios::showpoint);
    o << left << setw(11) << otag << " ";
    o << right << setw(25) << testName << " ";
    o << setw(5) << variation << " ";
    o << setw(5) << coll.values.size() << " ";
    o << setw(9) << bufSize << " ";
    o << setw(9) << loops << " ";            
    o << setw(9) << coll.MinValue().num * latfactor << " ";
    o << setw(9) << coll.avgVal * latfactor << " ";
    o << setw(9) << coll.MaxValue().num * latfactor << " ";
    o << setw(13) << " ";                                 // Align with BW results
    
    double diff = coll.MinValue().num > 0 ? (coll.MaxValue().num - coll.MinValue().num) / coll.MinValue().num * 100 : 100;
    o << setw(6) << diff << " ";
    o << setw(9) << coll.stdDevVal * latfactor << " ";
    
    char host[100];    
    ProcID & p = coll.pc[coll.values[coll.values.size()-1].rank];
    sprintf(host, "%s(%d.%d)", p.hostname, p.pid, p.cpuNum);
    o << setw(19) << left << host << right << " ";

    o.precision(4);
    o << setw(10) << coll.bucketDelta * latfactor  << " ";
    o.precision(2);
    o << setw(5) << coll.maxFrequency  << " ";
    for (int i=0; i<NUMBUCKETS; ++i) {
         cout << setw(5) << coll.buckets[i] << " ";
    }

    o << endl; 
}
    

void PrintScriptBWResults(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, ResultsCollection &coll, const char *tag)
{
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        o << right << "TBWHDR:     " << setw(25) << "test" << " ";
        o << setw(5) << "var" << " ";
        o << setw(5) << "#proc" << " ";
        o << setw(9) << "#bytes" << " ";
        o << setw(9) << "#loops" << " ";        
        o << setw(9) << "min[MBs]"  << " ";
        o << setw(9) << "avg[MBs]" << " ";
        o << setw(9) << "max[MBs]"  << " ";
        o << setw(12) << "total[MBs]"  << " ";
        o << setw(6) << "%diff"       << " ";
        o << setw(9) << "stddev"   << " ";
        o << setw(19) << left << "min_process" << right << " ";
        o << setw(8) << "FBktWidth"   << " ";
        o << setw(5) << "FMax"     << " ";
        o << setfill('0'); 
        for (int i=0; i<NUMBUCKETS; ++i) {
            o << "f" << setw(3) << i << " ";
        }
        o << setfill(' ');       
        o << endl;
    }
    
    string otag = tag;
    otag += ":";
    o.setf(ios::fixed);
    o.precision(2);
    o.setf(ios::showpoint);
    o << left << setw(11) << otag << " ";
    o << right << setw(25) << testName << " ";
    o << setw(5) << variation << " ";
    o << setw(5) << coll.values.size() << " ";
    o << setw(9) << bufSize << " ";
    o << setw(9) << loops << " ";                
    o << setw(9) << coll.MinValue().num * bwfactor << " ";
    o << setw(9) << coll.avgVal * bwfactor << " ";
    o << setw(9) << coll.MaxValue().num * bwfactor << " ";
    o << setw(12) << coll.TotalValue() * bwfactor << " ";
    
    double diff = coll.MinValue().num > 0 ? (coll.MaxValue().num - coll.MinValue().num)/coll.MinValue().num * 100 : 100;
    o << setw(6) << diff << " ";
    o << setw(9) << coll.stdDevVal * bwfactor << " ";

    char host[100];    
    ProcID & p = coll.pc[coll.values[0].rank];
    sprintf(host, "%s(%d.%d)", p.hostname, p.pid, p.cpuNum);
    o << setw(19) << left << host << right << " ";
    
    o.precision(4);
    o << setw(10) << coll.bucketDelta * bwfactor  << " ";
    o.precision(2);
    o << setw(5) << coll.maxFrequency  << " ";
    for (int i=0; i<NUMBUCKETS; ++i) {
         cout << setw(5) << coll.buckets[i] << " ";
    }
    
    o << endl; 
}


void PrintStdResults(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, CommGroups & grps, 
                    ResultsCollection *pBwColl, ResultsCollection *pLatColl, ResultsCollection *pTotColl)
{
    static const char * firstVar = NULL; 
    if ((firstVar == NULL) || (strcmp(firstVar, variation) != 0)) {
        firstVar = variation;
        o << endl << right << "#" << string(50,'=') << endl;
        o << "# MiniBenchmark: " << testName << ", " << variation << endl;
        time_t curTime = time(NULL);
        o << "# Date: " << ctime(&curTime);
        o << "# Groups: " << grps.headComm.Get_size() << endl;
        o << "# #procs = " << grps.bmComm.Get_size() << endl;
        
        o << "# #concur = ";
        if (pBwColl)       o << pBwColl->totalConcurrent << endl;
        else if (pLatColl) o << pLatColl->totalConcurrent << endl;
        	
        o << "#" << string(50,'=') << endl << endl;

        if (prtTimeStamp) {
            o << setw(12) << "time" << " ";
        }

        o << setw(9) << "#bytes" << " ";
        o << setw(9) << "#loops" << " ";        
        if (pLatColl) {
            string label; 
            label = "min"; label += latLabel; o << setw(9) << label << " ";
            label = "avg"; label += latLabel; o << setw(9) << label << " ";
            label = "max"; label += latLabel; o << setw(9) << label << " ";
        }
        if (pBwColl) {
            string label; 
            label = "min"; label += bwLabel; o << setw(9) << label << " ";
            label = "avg"; label += bwLabel; o << setw(9) << label << " ";
            label = "max"; label += bwLabel; o << setw(9) << label << " ";
        }
        // If a total coll is passed in - is a BW value. Add up all the values.
        if (pTotColl) {
            string label; 
            label = "Total"; label += bwLabel; o << setw(12) << label << " ";
        }
        o << endl;
    }

    if (prtTimeStamp) {
        // time_t ct = time(NULL);
        // tm *ptm = localtime(&ct);
        timeval ct;
        //timezone tz; 
        //memset(&tz, 0, sizeof(tz));
        gettimeofday(&ct, NULL);
        tm *ptm = localtime(&ct.tv_sec);
        char tsstrg[30];
        strftime(tsstrg, 29, "%T", ptm);
        o << setw(8) << tsstrg << ":" << setw(3) << setfill('0') << ct.tv_usec/1000 << setfill(' ') << " ";
    }
        
    o.setf(ios::fixed);
    o.precision(2);
    o.setf(ios::showpoint);
    o << setw(9) << bufSize << " ";
    o << setw(9) << loops << " ";
                   
    if (pLatColl) {
        o << setw(9) << pLatColl->MinValue().num * latfactor << " ";
        o << setw(9) << pLatColl->avgVal * latfactor << " ";
        o << setw(9) << pLatColl->MaxValue().num * latfactor << " ";
    }
    
    if (pBwColl) {
        o << setw(9) << pBwColl->MinValue().num * bwfactor << " ";
        o << setw(9) << pBwColl->avgVal * bwfactor << " ";
        o << setw(9) << pBwColl->MaxValue().num * bwfactor << " ";
    }
    
    if (pTotColl) {
        o << setw(12) << pTotColl->TotalValue() * bwfactor << " ";       
    }
        
    o << endl; 
}

 
void PrintStdDeltaResults(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, long deltaVal, CommGroups & grps, 
                    ResultsCollection *pBwColl, ResultsCollection *pLatColl, ResultsCollection *pTotColl)
{
    static const char * firstVar = NULL; 
    if ((firstVar == NULL) || (strcmp(firstVar, variation) != 0)) {
        firstVar = variation;
        o << endl << right << "#" << string(50,'=') << endl;
        o << "# MiniBenchmark: " << testName << ", " << variation << endl;
        time_t curTime = time(NULL);
        o << "# Date: " << ctime(&curTime);
        o << "# Groups: " << grps.headComm.Get_size() << endl;
        o << "# #procs = " << grps.bmComm.Get_size() << endl;
        
        o << "# #concur = ";
        if (pBwColl)       o << pBwColl->totalConcurrent << endl;
        else if (pLatColl) o << pLatColl->totalConcurrent << endl;
        	
        o << "#" << string(50,'=') << endl << endl;

        if (prtTimeStamp) {
            o << setw(12) << "time" << " ";
        }

        o << setw(9) << "#bytes" << " ";
        o << setw(9) << "delta" << " ";            
        o << setw(9) << "#loops" << " ";        
        if (pLatColl) {
            string label; 
            label = "min"; label += latLabel; o << setw(9) << label << " ";
            label = "avg"; label += latLabel; o << setw(9) << label << " ";
            label = "max"; label += latLabel; o << setw(9) << label << " ";
        }
        if (pBwColl) {
            string label; 
            label = "min"; label += bwLabel; o << setw(9) << label << " ";
            label = "avg"; label += bwLabel; o << setw(9) << label << " ";
            label = "max"; label += bwLabel; o << setw(9) << label << " ";
        }
        // If a total coll is passed in - is a BW value. Add up all the values.
        if (pTotColl) {
            string label; 
            label = "Total"; label += bwLabel; o << setw(12) << label << " ";
        }
        o << endl;
    }

    if (prtTimeStamp) {
        // time_t ct = time(NULL);
        // tm *ptm = localtime(&ct);
        timeval ct;
        //timezone tz; 
        //memset(&tz, 0, sizeof(tz));
        gettimeofday(&ct, NULL);
        tm *ptm = localtime(&ct.tv_sec);
        char tsstrg[30];
        strftime(tsstrg, 29, "%T", ptm);
        o << setw(8) << tsstrg << ":" << setw(3) << setfill('0') << ct.tv_usec/1000 << setfill(' ') << " ";
    }
        
    o.setf(ios::fixed);
    o.precision(2);
    o.setf(ios::showpoint);
    o << setw(9) << bufSize << " ";
    o << setw(9) << deltaVal << " ";
    o << setw(9) << loops << " ";
                   
    if (pLatColl) {
        o << setw(9) << pLatColl->MinValue().num * latfactor << " ";
        o << setw(9) << pLatColl->avgVal * latfactor << " ";
        o << setw(9) << pLatColl->MaxValue().num * latfactor << " ";
    }
    
    if (pBwColl) {
        o << setw(9) << pBwColl->MinValue().num * bwfactor << " ";
        o << setw(9) << pBwColl->avgVal * bwfactor << " ";
        o << setw(9) << pBwColl->MaxValue().num * bwfactor << " ";
    }
    
    if (pTotColl) {
        o << setw(12) << pTotColl->TotalValue() * bwfactor << " ";       
    }
        
    o << endl; 
}

 
void PrintStdResultsWithStage(ostream & o, const char *testName, const char* variation, 
                    long loops, long bufSize, CommGroups & grps, const char* stageName,
                    ResultsCollection *pBwColl, ResultsCollection *pLatColl, ResultsCollection *pTotColl)
{
    static const char * firstVar = NULL; 
    if ((firstVar == NULL) || (strcmp(firstVar, variation) != 0)) {
        firstVar = variation;
        o << endl << right << "#" << string(50,'=') << endl;
        o << "# MiniBenchmark: " << testName << ", " << variation << endl;
        time_t curTime = time(NULL);
        o << "# Date: " << ctime(&curTime);
        o << "# Groups: " << grps.headComm.Get_size() << endl;
        o << "# #procs = " << grps.bmComm.Get_size() << endl;
        
        o << "# #concur = ";
        if (pBwColl)       o << pBwColl->totalConcurrent << endl;
        else if (pLatColl) o << pLatColl->totalConcurrent << endl;
       
        o << "#" << string(50,'=') << endl << endl;
        
        if (prtTimeStamp) {
            o << setw(12) << "time" << " ";
        }

        o << setw(9) << "#bytes" << " ";
        o << setw(9) << "#loops" << " ";  
        o << setw(3) << "stg" << " ";      
        if (pLatColl) {
            o << setw(9) << "min[usec]"  << " ";
            o << setw(9) << "avg[usec]" << " ";
            o << setw(9) << "max[usec]"  << " ";
        }
        if (pBwColl) {
            o << setw(9) << "min[MBs]"  << " ";
            o << setw(9) << "avg[MBs]" << " ";
            o << setw(9) << "max[MBs]"  << " ";
        }
        // If a total coll is passed in - is a BW value. Add up all the values.
        if (pTotColl) {
            o << setw(12) << "Total[MBs]";
        }
        o << endl;
    }
    
    if (prtTimeStamp) {
        // time_t ct = time(NULL);
        // tm *ptm = localtime(&ct);
        timeval ct;
        //timezone tz; 
        //memset(&tz, 0, sizeof(tz));
        gettimeofday(&ct, NULL);
        tm *ptm = localtime(&ct.tv_sec);
        char tsstrg[30];
        strftime(tsstrg, 29, "%T", ptm);
        o << setw(9) << tsstrg << ":" << setw(3) << setfill('0') << ct.tv_usec/1000 << setfill(' ') << " ";
    }
        
    o.setf(ios::fixed);
    o.precision(2);
    o.setf(ios::showpoint);
    o << setw(9) << bufSize << " ";
    o << setw(9) << loops << " ";
    o << setw(3) << stageName << " ";
                   
    if (pLatColl) {
        o << setw(9) << pLatColl->MinValue().num * latfactor << " ";
        o << setw(9) << pLatColl->avgVal * latfactor << " ";
        o << setw(9) << pLatColl->MaxValue().num * latfactor << " ";
    }
    
    if (pBwColl) {
        o << setw(9) << pBwColl->MinValue().num * bwfactor << " ";
        o << setw(9) << pBwColl->avgVal * bwfactor << " ";
        o << setw(9) << pBwColl->MaxValue().num * bwfactor << " ";
    }
    
    if (pTotColl) {
        o << setw(12) << pTotColl->TotalValue() * bwfactor << " ";       
    }
        
    o << endl; 
}

void PrintStdResultsWithStageContinued(ostream & o, const char* stageName,
                    ResultsCollection *pBwColl, ResultsCollection *pLatColl, ResultsCollection *pTotColl)
{   
    if (prtTimeStamp) {
         o << setw(10) << " ";
    }
        
     
    o.setf(ios::fixed);
    o.precision(2);
    o.setf(ios::showpoint);
    
    o << setw(10) << " ";
    o << setw(10) << " ";
    o << setw(3) << stageName << " ";
                   
    if (pLatColl) {
        o << setw(9) << pLatColl->MinValue().num * latfactor << " ";
        o << setw(9) << pLatColl->avgVal * latfactor << " ";
        o << setw(9) << pLatColl->MaxValue().num * latfactor << " ";
    }
    
    if (pBwColl) {
        o << setw(9) << pBwColl->MinValue().num * bwfactor << " ";
        o << setw(9) << pBwColl->avgVal * bwfactor << " ";
        o << setw(9) << pBwColl->MaxValue().num * bwfactor << " ";
    }
    
    if (pTotColl) {
        o << setw(12) << pTotColl->TotalValue() * bwfactor << " ";       
    }
        
    o << endl; 
}

