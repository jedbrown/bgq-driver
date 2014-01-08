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

//#include <stdlib.h>
#include <string.h>
#include "TestArgs.h"
#include "TestBuffer.h"


bool verbose = false;
bool prtTimeStamp = false;

//-----------------------------------------------------
// TestArgs constructor
//-----------------------------------------------------
TestArgs::TestArgs(int iargc, const char *iargv[], const char *iValidArgs)
: 
maxBuf(4194304), 
minBuf(1), 
scaleFactor(2), 
increment(0), 
bufSize(1024), 
repSize(1), 
pairing(PCUT), 
takeTurns(false),
rootRank(0), 
outputType(ONORMAL), 
affinity(false), 
maxLoopCount(1000), 
loopCount(10000), 
warmCount(25), 
loopBarrier(0),
targSampleTime(500),
shiftBegin(1),
shiftEnd(1),
shiftStep(1),
shiftDir('A'),
pipelineDepth(1), 
xport(XBOTH),
chkMode(CHK_NONE),
globalRepSize(1),
curRepeatNum(0)
// testDimension(-1), limitHops(false), 
{
    // get command name stripped of path
    if (iargc > 0) {
        command=iargv[0];
        string::size_type idx = command.find_last_of("/");
        if (idx != string::npos) {
            command.erase(0,idx+1);
        }
    }
    
    //argv.reserve(iargc);  not avail on deque
    for (int i=1; i<iargc; i++) {
        argv.push_back(iargv[i]);
    }
    
    // parse out valid arguments
    char *dup = strdup(iValidArgs);
    const char *ws = " \t\n\r\f\v";
    char *tok = strtok(dup, ws);
    while (tok) {
    	validArgs.insert(tok);
    	tok = strtok(NULL, ws);
    }
    free(dup);
    if (validArgs.empty()) validArgs.insert("-h");
    	
    totalProcs = MPI::COMM_WORLD.Get_size();
    
    char name[100];
    *name = '\0';
    gethostname(name, 100);
    hostName = name;

    //cerr << "validArgs:" << endl;
    //PRINT_ELEMENTS(cerr, validArgs);
}

//-----------------------------------------------------
// virtual ParseArgs
//-----------------------------------------------------
void TestArgs::ParseArgs()
{
    string arg;
    while (! argv.empty()) {
        arg=SHIFTARGV(argv);
        ParseThisArg(arg);
    }   
    
    // mak sure max is greater than min.
    if (minBuf < 0) { minBuf *= -1; }
    if (maxBuf < 0) { maxBuf *= -1; }
    if (minBuf > maxBuf) {
        cout << "MaxBuf value (" << maxBuf << ") was less than MinBuf (" << minBuf << ") - swapping values" << endl;
        long temp = minBuf;
        minBuf = maxBuf;
        maxBuf = temp;
    }
    // make sure minBuf is at least 2 if scalefactor is negative (bufSize would never reach 1 when scaled)
    if ((scaleFactor < 0) && (minBuf < 2)) minBuf = 2;
    
}
    

//-----------------------------------------------------
// virtual ParseThisArg
//-----------------------------------------------------
void TestArgs::ParseThisArg(string & arg)
{
    if      (eqv(arg,"-S")) { arg=SHIFTARGV(argv); maxBuf = atoi(arg.c_str()); }
    else if (eqv(arg,"-s")) { arg=SHIFTARGV(argv); minBuf = atoi(arg.c_str()); bufSize = minBuf; }
    else if (eqv(arg,"-x")) { 
        arg=SHIFTARGV(argv); 
        scaleFactor = atoi(arg.c_str());
        if (scaleFactor==0 || scaleFactor==1) { scaleFactor=2; }
    }
    else if (eqv(arg,"-i")) { 
        arg=SHIFTARGV(argv); 
        increment = atoi(arg.c_str());
        if (increment == 0) increment = 1;
    }
    else if (eqv(arg,"-p")) {
        arg=SHIFTARGV(argv); 
        transform (arg.begin(), arg.end(), arg.begin(), (int(*)(int))std::tolower); // see g++-3.0 Bug#144409
        if      (arg=="c") { pairing = PCUT; }
        else if (arg=="f") { pairing = PFOLD; }
        else if (arg=="s") { pairing = PSORT; }
        else    { die1("ERROR: don't recognize pairing type (-p %s).\n", arg.c_str()); }
    }
    else if (eqv(arg,"-tt")) { takeTurns = true; }
    else if (eqv(arg,"-r")) { arg=SHIFTARGV(argv); rootRank = atoi(arg.c_str()); }
    else if (eqv(arg,"-rp")) { 
        arg=SHIFTARGV(argv); repSize = atoi(arg.c_str());
        if (repSize < 1) repSize = 1;
    }
    else if (eqv(arg,"-RP")) { 
        arg=SHIFTARGV(argv); globalRepSize = atoi(arg.c_str());
        if (globalRepSize < 1) globalRepSize = 1;
    }
    else if (eqv(arg,"-o")) {
        arg=SHIFTARGV(argv); 
        outputType = 0;
        if (arg.find('N') != string::npos) outputType |= ONORMAL; 
        if (arg.find('S') != string::npos) outputType |= OSCRIPT; 
        if (arg.find('P') != string::npos) outputType |= OPROCS; 
        if (!outputType) outputType = ONORMAL;      
    }
    //else if (eqv(arg,"-a")) {
    //    arg=SHIFTARGV(argv); 
    //    int v = atoi(arg.c_str());
    //    affinity = (v==0) ? false : true;
    //}
    else if (eqv(arg,"-l")) { arg=SHIFTARGV(argv); maxLoopCount = loopCount = atoi(arg.c_str()); }
    else if (eqv(arg,"-w")) { arg=SHIFTARGV(argv); warmCount = atoi(arg.c_str()); }
    else if (eqv(arg,"-st")) { arg=SHIFTARGV(argv); targSampleTime = atoi(arg.c_str()); }
    else if (eqv(arg,"-g")) { groupFile=SHIFTARGV(argv); }
    else if (eqv(arg,"-lb")) { 
        arg=SHIFTARGV(argv); loopBarrier = atoi(arg.c_str());
        if (loopBarrier < 0) loopBarrier *= -1;
        if (loopBarrier == 0) loopBarrier = 1;
    }
    else if (eqv(arg,"-pd")) { 
        arg=SHIFTARGV(argv); pipelineDepth = atoi(arg.c_str());
        if (pipelineDepth < 1) pipelineDepth = 1;
    }
    else if (eqv(arg,"-xp")) {
        arg=SHIFTARGV(argv); 
        xport = 0;
        if (arg.find('D') != string::npos) xport |= XDACS; 
        if (arg.find('M') != string::npos) xport |= XMPI; 
        if (arg.find('B') != string::npos) xport |= XBOTH; 
        if (!xport) xport = XBOTH;      
    }

    else if (eqv(arg,"-v")) { verbose = true;  }
    else if (eqv(arg,"-pts")) { prtTimeStamp = true;  }
    else if (eqv(arg,"-h")) { PrintHelp(); }
    
    else if (eqv(arg,"-shft")) { 
        arg=SHIFTARGV(argv); shiftBegin = atoi(arg.c_str());
    }
    else if (eqv(arg,"-SHFT")) { 
        arg=SHIFTARGV(argv); shiftEnd = atoi(arg.c_str());
    }   
    else if (eqv(arg,"-sstep")) { 
        arg=SHIFTARGV(argv); shiftStep = atoi(arg.c_str());
    }  
    else if (eqv(arg,"-sdir")) { 
        arg=SHIFTARGV(argv); shiftDir = arg[0];
        if ((shiftDir != 'B') && (shiftDir != 'U') && (shiftDir != 'A')) {
            die1("ERROR: \"%s\" is not a valid value for -sdir option.\n", arg.c_str());
            PrintHelp();        	
        }
    }        
    else if (eqv(arg,"-chk")) {
        arg=SHIFTARGV(argv); 
        transform (arg.begin(), arg.end(), arg.begin(), (int(*)(int))std::tolower); // see g++-3.0 Bug#144409
        if      (arg=="n") { chkMode = CHK_NONE; }
        else if (arg=="a") { chkMode = CHK_ALL; }
        else    { die1("ERROR: don't recognize integrity test mode (-chk %s).\n", arg.c_str()); }
    }

    else {
        die1("ERROR: \"%s\" is not a valid input argument.\n", arg.c_str());
        PrintHelp();
    }  
}


//-----------------------------------------------------
// StartSize
//-----------------------------------------------------
long TestArgs::StartSize() 
{
    long start = minBuf;
    if      ((increment != 0) && (increment < 0)) start = maxBuf;
    else if ((increment == 0) && (scaleFactor < 0)) start = maxBuf;
    curRepeatNum = repSize;
    return start;
}

//-----------------------------------------------------
// Done
//-----------------------------------------------------
bool TestArgs::Done()
{
    bool done = false;
    if (increment != 0) {
        if ((increment < 0) && (bufSize < minBuf)) done = true;
        else if ((increment > 0) && (bufSize > maxBuf)) done = true; 
    }
    else if ((scaleFactor > 0) && (bufSize > maxBuf)) done = true;
    else if ((scaleFactor < 0) && (bufSize < minBuf)) done = true; 
    return done;
}

//-----------------------------------------------------
// NextSize
//-----------------------------------------------------
long TestArgs::NextSize(bool decrReps)
{
    long next = bufSize;
    if (decrReps) --curRepeatNum; 
    if (curRepeatNum < 1) {
        curRepeatNum = repSize;
        if (increment != 0) next = bufSize + increment;
        else if (scaleFactor > 0) {
            next = bufSize * scaleFactor;
            if (next == 0) next = 1;  // in case bufSize started out 0
        }
        else {
            next = bufSize / (scaleFactor * -1);  // scaleFactor is negative
        }
    }
    return next;
}

//-----------------------------------------------------
// UpdateLoopCount
//-----------------------------------------------------
void TestArgs::UpdateLoopCount(MPI::Comm &comm, double priorElapsedTime)
{
    if (targSampleTime != 0) {
        if (comm.Get_rank() == 0) {
            double targTime = targSampleTime / 1000.0;
            //if (priorElapsedTime > targTime) {
            loopCount = (long) (targTime / 
            		( ( priorElapsedTime / bufSize ) * NextSize(false) )
            		* loopCount );
            if (loopCount > maxLoopCount) loopCount = maxLoopCount; 
            if (loopCount < warmCount) loopCount = warmCount;
            //}
        }
    }
    comm.Bcast(&loopCount, 1, MPI::LONG, 0);
}


//-----------------------------------------------------
// PrintHelp
//-----------------------------------------------------
void TestArgs::PrintHelp()
{
	if (MPI::COMM_WORLD.Get_rank() == 0) {
	    cout << "Usage: " << command << endl;
	    PrintArgHelp();
	}
    exit(-1);
}


//-----------------------------------------------------
// PrintArgHelp
//-----------------------------------------------------
void TestArgs::PrintArgHelp()
{
    TestArgs def(0, NULL);  // use to get argument default values
    if (valid("-s"))   cout << "    -s #        - (" << def.minBuf << ") minimum message size" << endl;   
    if (valid("-S"))   cout << "    -S #        - (" << def.maxBuf << ") maximum message size" << endl;   
    if (valid("-x"))   cout << "    -x #        - (" << def.scaleFactor << ") message scaling factor (overridden by increment)" << endl;   
    if (valid("-i"))   cout << "    -i #        - (" << def.increment << ") message increment value" << endl;   
    if (valid("-rp"))  cout << "    -rp #       - (" << def.repSize << ") # times to repeat a test message size" << endl;   
    if (valid("-RP"))  cout << "    -RP #       - (" << def.globalRepSize << ") # times to repeat whole (global) test " << endl;   
    if (valid("-p"))   cout << "    -p [s|c|f]  - (s) pairing type - sort, cut or fold" << endl;   
    if (valid("-tt"))  cout << "    -tt         - paired ranks take turns (i.e. run one pair at a time rather than simultaineous)" << endl;
    if (valid("-g"))   cout << "    -g file     - rank group specification file (use rrgroups_*.pl scripts to build)" << endl;   
    if (valid("-r"))   cout << "    -r #        - (" << def.rootRank << ") root rank for operation if applicable" << endl;   
    if (valid("-o"))   cout << "    -o [N|S|P]  - (N) optional outputs (N=normal,S=script, P=process detail)" << endl;   
    //cout << "    -a #        - (" << def.affinity << ") force affinity to # (not implemented)" << endl;   
    if (valid("-l"))   cout << "    -l #        - (" << def.maxLoopCount << ") maximum loop (repetition) count" << endl;   
    if (valid("-w"))   cout << "    -w #        - (" << def.warmCount << ") warmup loop count" << endl;   
    if (valid("-lb"))  cout << "    -lb         - (" << def.loopBarrier << ") add loop barriers (untimed barriers between loops)" << endl;   
    if (valid("-st")) {cout << "    -st #       - (" << def.targSampleTime << ") if nonzero, adjust # of loops to " << endl;
                       cout << "                  approximate the given run time (based on prior loops)" << endl;
    }
    if (valid("-v"))   cout << "    -v          - enable verbose messages" << endl;   
    if (valid("-pts")) cout << "    -pts        - include timestamps in output results" << endl;
    if (valid("-pd"))  cout << "    -pd #       - (1) message pipeline depth (number of segments)" << endl;  
    if (valid("-xp"))  cout << "    -xp [D|M|B] - (B) choose transports included in test - DaCS only, MPI only or Both" << endl;
    
    if (valid("-shft"))   cout << "    -shft #     - (1) beginning shift distance" << endl;
    if (valid("-SHFT"))   cout << "    -SHFT #     - (-shft val) ending shift distance" << endl;
    if (valid("-sstep"))  cout << "    -sstep #    - (1) step increment between shift distances attepted" << endl;
    if (valid("-sdir"))   cout << "    -sdir B|U|A - (A) shift direction bidi, uni or all (i.e. both)" << endl;
    
    if (valid("-chk"))    cout << "    -chk  N|A   - (N) integrity check (N=none, A=all buff mem" << endl;
}




ostream & operator <<(ostream & o, const TestArgs & s)
{
    int w = 25;
    o << left;
    o << setw(w) << "   Total Procs:" << s.totalProcs << endl;    
    if (s.valid("-s"))   o << setw(w) << "   MIN_BUFFER(-s):" << s.minBuf << endl;
    if (s.valid("-S"))   o << setw(w) << "   MAX_BUFFER(-S):" << s.maxBuf << endl;
    if (s.valid("-x"))   o << setw(w) << "   SCALE_FACTOR(-x):" << s.scaleFactor << endl;
    if (s.valid("-i"))   o << setw(w) << "   MSG_INCREMENT(-i):" << s.increment << endl;
    if (s.valid("-rp"))  o << setw(w) << "   REP_SIZE(-rp):" << s.repSize << endl;
    if (s.valid("-RP"))  o << setw(w) << "   GLOBAL_REP_SIZE(-RP):" << s.globalRepSize << endl;
    //o << setw(w) << "   PIPELINE_DEPTH(-pd):" << s.pipelineDepth << endl;

    if (s.valid("-p")) {  o << setw(w) << "   PAIRING(-p):";
	    switch (s.pairing) {
	        case PSORT : o<<"sort"; break;        
	        case PCUT  : o<<"cut"; break;
	        case PFOLD : o<<"fold"; break;
	        default:     o<<"unknown"; 
	    }
	    o << endl;
    }
    
    if (s.valid("-tt"))  o << setw(w) << "   TAKE_TURNS(-tt): " << s.takeTurns << endl;

    if (s.valid("-g"))   o << setw(w) << "   GROUP_FILE(-g): " << s.groupFile << endl;

    if (s.valid("-r"))   o << setw(w) << "   ROOT(-r):" << s.rootRank << endl;
    //o << setw(w) << "   LIMIT_HOPS(-lh):" << s.limitHops << endl;
    
    if (s.valid("-o")) { o << setw(w) << "   OUTPUT_TYPES(-o):";
	    if (s.outputType & ONORMAL)  o<<"normal "; 
	    if (s.outputType & OSCRIPT)  o<<"script "; 
	    if (s.outputType & OPROCS)   o<<"process-detail "; 
	    o << endl;
	}

    //o << setw(w) << "   AFFINITY(-a):" << s.affinity << endl;
    if (s.valid("-l"))   o << setw(w) << "   MAX_LOOP_COUNT(-l):" << s.maxLoopCount << endl;
    if (s.valid("-w"))   o << setw(w) << "   WARM_COUNT(-w):" << s.warmCount << endl;
    if (s.valid("-lb"))  o << setw(w) << "   LOOP_BARRIERS(-lb):" << s.loopBarrier << endl;    
    if (s.valid("-st"))  o << setw(w) << "   TARG_SAMPLE_TIME(-st):" << s.targSampleTime << " ms" << endl;
    if (s.valid("-pd"))  o << setw(w) << "   PIPELINE_DEPTH(-pd):" << s.pipelineDepth << endl;
    if (s.valid("-xp")) {o << setw(w) << "   XPORT(-xp):";
	    if (s.xport & XDACS )  o<<"DaCS "; 
	    if (s.xport & XMPI)    o<<"MPI "; 
	    o << endl;
    }
    
    if (s.valid("-shft"))   o << setw(w) << "   SHIFT_BEGIN(-shft):" << s.shiftBegin << endl;
    if (s.valid("-SHFT"))   o << setw(w) << "   SHIFT_END(-SHFT):" << s.shiftEnd << endl;
    if (s.valid("-sstep"))  o << setw(w) << "   SHIFT_STEP(-sstep):" << s.shiftStep << endl;
    if (s.valid("-sdir"))   o << setw(w) << "   SHIFT_DIR(-sdir):" << s.shiftDir << endl;
    
    if (s.valid("-chk"))   {o << setw(w) << "   INTG_CHECK(-chk):";
	    switch (s.chkMode) {
	        case CHK_NONE : o<<"none"; break;        
	        case CHK_ALL  : o<<"all of buffer"; break;
	        default:        o<<"unknown"; 
	    }
	    o << endl;
    }
    
    
    //o << setw(w);
    //PRINT_ELEMENTS(o, s.dimensions, "   DIMENSIONS(-d):", ':');
    //o << setw(w) << "   TEST_DIMENSION(-td):" << s.testDimension << endl;
    //o << setw(w) << "   StartDim:" << s.startDim << endl;
    //o << setw(w) << "   EndDim:" << s.endDim << endl;
    o << right;
    
    return o;
}






    
RankPair::RankPair(TestArgs & ta, MPI::Comm & testComm)
: localRank(testComm.Get_rank()), procs(testComm.Get_size()), 
  curPairCount(0), takeTurns(ta.takeTurns), pairType(ta.pairing)
{
	remoteRank = FindProcPair(ta);
}

bool RankPair::Myturn() 
{ 
	bool ret = true;
	if (takeTurns) {
		if      (pairType == PSORT) { ret = (curPairCount == (MIN(localRank,remoteRank)/2)); }
		else if (pairType == PCUT)  { ret = (curPairCount == MIN(localRank,remoteRank)); }
		else if (pairType == PFOLD) { ret = (curPairCount == MIN(localRank,remoteRank)); }
	}
	return ret;
}



int RankPair::FindProcPair(TestArgs & ta)
{
    int remote = 0;

    if (ta.pairing == PCUT) {
        int pairDist = procs/2;
        if (localRank < pairDist) {
            remote = localRank + pairDist;
        }
        else remote = localRank - pairDist;
    }
    else if (ta.pairing == PFOLD) {
        remote = (procs - 1) - localRank;   
    }
    else if (ta.pairing == PSORT) {
        remote = (localRank % 2) == 0 ? localRank + 1 : localRank - 1;
    }
    
    return remote;
}



