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
#ifndef TestArgs_h
#define TestArgs_h

#include <deque>
#include <set>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstdlib>
//#include <string.h>
//#include <stdlib.h>
#include <algorithm>
#include <cctype>
#include <mpi.h>

using namespace std;

#define UNUSED __attribute__((unused))

#ifndef die
#define die(...) fprintf(stderr, __VA_ARGS__); exit(-1)
#endif
#define die0(a1) fprintf(stderr, a1); exit(-1)
#define die1(a1,a2) fprintf(stderr, a1, a2); exit(-1)
#define die2(a1,a2,a3) fprintf(stderr, a1, a2, a3); exit(-1)
#define VERBOSE(A) if (verbose) { A; }
#define VERBOSE2(A) // A    // more detailed debug messages
#define LongProduct(T) accumulate(T.begin(), T.end(), 1, multiplies<long>())

#define MIN_BUFF_SIZE 131072    // switch the opteron data cache to prevent xfer cache perf issues
#define OPT_CACHE_SIZE 1048576  // flush at least this memory size.
#define MAX(a,b) (a < b ? b : a)
#define MIN(a,b) (a < b ? a : b)
#define REPEATFUNC(_count_,_func_) for (int i=0; i<_count_; ++i) _func_

// unsigned long long int var = ULL0 - Unsigned long long integer type
// long double var = 64bit floating point?

enum PairingMethodTypes {
    PSORT = 0,
    PCUT  = 1,
    PFOLD = 2,
};
enum OutputTypes {
    ONORMAL = 0x01,
    OSCRIPT = 0x02,
    OPROCS  = 0x04,
};

// Transport types - to select which communications to include in the test.
enum XportTypes {
   XDACS = 0x01,
   XMPI  = 0x02,
   XBOTH = 0x03
};

// Integrity Check types
enum ICheckMode {
   CHK_NONE = 0x00,
   CHK_ALL  = 0x01   // check 
};

extern bool verbose;
extern bool prtTimeStamp;  // include current timestamp in result output

typedef deque<string> ArgvList; 
//#define SHIFTARGV(a) a.empty() ? "" : a.front(); if (!a.empty()) a.erase(a.begin())
#define SHIFTARGV(a) a.empty() ? "" : a.front(); if (!a.empty()) a.pop_front()

inline bool nocase_compare (char c1, char c2) { return tolower(c1)==tolower(c2); }

// Container for the test arguments.  Can't say I'm happy using a common argument 
// handler (sounded good at the start - but entropy took over), but don't wish to change it now.
//
// Synopsis: 
//    string validArgs = "-h -v -s -S -x -i -rp -p -tt -r -o -l -w -lb -st -g -pts";
//    TestArgs args(argc, argv, validArgs);
//    args.ParseArgs();
//    if (MPI::COMM_WORLD.Get_rank() == 0) {
//        cout << args.command << " arguments:" << endl;
//        cout << args << endl;
//    }
// 

#define BASEARGS   "-h -v -s -S -x -i -rp -tt -o -l -w -g -pts"
#define PINGARGS   "-p -tt -lb -st "
#define HYBRIDARGS "-pd -xp -chk -RP"
#define COLLECTIVEARGS = "-r "
#define RINGARGS   "-lb -shft -SHFT -sstep -sdir "

class TestArgs
{
  public:
    // construct - build argv strings for parsing.
    TestArgs(int iargc, const char *iargv[], const char * iValidArgs=BASEARGS);
    virtual ~TestArgs() { }
        
    // parse list of arguments in argv
    virtual void ParseArgs();
    void ParseThisArg(string & arg);
       
    // override to print test Help Text and exit (should print usage info, call PrintArgHelp and exit
    virtual void PrintHelp();
    void PrintArgHelp();
    
    // return if src is equal to dest and is valid.
    bool eqv(string & src, const char *dest) const { return ((src == dest) && valid(dest)); }
    // indicate if argument is valid;
    bool valid(const char *dest) const { return (validArgs.find(dest) != validArgs.end()); }

       
    // Loop control methods.  Uses max,min,scale and increment values and signs to allow 
    //   increasing or decreasing message sample values. 
    // Synopsis: 
    //   TestArgs ta;
    //   ta.ParseArgs();
    //   for (ta.bufSize = ta.StartSize(); ta.!Done(); ta.bufSize = ta.NextSize()) {
    //      test current ta.bufSize;
    //   }
    long StartSize();
    bool Done();
    long NextSize(bool decrReps = true);
     
    // based on the prior sample elapsed time and current loopCount, 
    // Have rank 0 calculate a new loopCount which will fit between
    // warmCount and maxLoopCount with an estimated Sample time around
    // tgtSampleTime (assumes time per loop is equal - which is close enough).
    // The new Loop Count is broadcast to all procs in the test for update. 
    // The purpose is to reduce the amount of time to needed to run test
    // by reducing the loops as the message size get's larger.
    void UpdateLoopCount(MPI::Comm &comm, double priorElapsedTime);

    string command;  // called command name           
    ArgvList argv;   // string collection of arguments 

    long maxBuf;                // -S #
    long minBuf;                // -s #
    long scaleFactor;           // -x #
    long increment;             // -i # (non zero should override scaleFactor)
    long bufSize;               // current buffer size
    long repSize;               // -rp # repeat size (number of times to repeat test with each message size)
        
    PairingMethodTypes pairing; // -p (= c,f,s)
    bool takeTurns;             // -tt -  Take turns option
    long rootRank;              // -r #

    //bool limitHops;
    short outputType;           // -o (= N,S,P)
    bool affinity;

    long maxLoopCount;          // -l #
    long loopCount;             // current loop count (starts at max, but adjustable)
    long warmCount;             // -w #
    long loopBarrier;           // -lb #
    long targSampleTime;        // -st #  - target elapsed time for a message sized sample (milli-seconds) ( 0 = disabled) 

    string groupFile;           // -g file
    //vector<long> dimensions;
    //int testDimension;
    //int startDim;
    //int endDim;
    
    long shiftBegin;			// -shft # -  Ring test shift beginning size (default=1)
    long shiftEnd;              // -SHFT #
    long shiftStep;             // -sstep #    shift step size (default=1)
    char shiftDir;              // -sdir  B|U|A  - bidi, uni or all (both)
    

    // hybrid vars
    long pipelineDepth;         // -pd #
    short xport;                // -xp D(acs), M(pi) or B(oth)

    int chkMode;                // -chk N(one) A(ll)  
    
    long globalRepSize;          // -RP # 
    
    long totalProcs;
    
    string hostName; 
    
    
    
  private:
	set<string> validArgs;      // list of valid arguments.  
	  
    // private loop variables - used by startSize(), NotDone(), NextSize() methods
    long startBufSize;          // Based on min,max,scale and increment sign values - these
    long endBufSize;            // values are set to the starting, ending buffer sizes, and 
    bool incrSizes;             // incrSizes indicates whether buffer (message) sizes are to 
                                // increase or decrease during the test.
  public:
    long curRepeatNum;          // Current repeat number for repSize
    
};


// Provide ability to stream out current arguments.
ostream & operator <<(ostream & o, const TestArgs & s);


// General template to stream out collection.
template <class T>
inline void PRINT_ELEMENTS (ostream & o, const T& coll, const char *optcstr="", const char sep=' ')
{
    typename T::const_iterator pos;
    o << optcstr;
    for (pos=coll.begin(); pos!=coll.end(); ++pos) {
        o << *pos << sep;
    }
    o << std::endl;
}


// RankPair Class 
// Role: create rank pairs according to argument "PairingMethod". 
//       Also allow ability to indicate which pair should be active in a loop through methods
//       This is needed for the -tt "taketurns" option.
//       See how this is used in pingpong.cpp for a better understanding.
// Parms: 
//       testComm    : is the input test communicator - the comm group this process is a member for the test.
//       pBarrComm   : is a ptr to a created communicator for which internal loop barriers should be performed.
//                     If doing take turns - this is a communicator for only the rank pair.  Every other pair
//                     has to wait on a tcomm.barrier.
//                     If not takingturns - then it's just a pointer to the passed tcomm.
//       ta          : The test arguments.
class RankPair 
{
public:
	int localRank;
	int remoteRank;
	int procs;
    
	RankPair(TestArgs & ita, MPI::Comm & testComm);
	~RankPair() {}
	
	void Reset() { curPairCount = 0; }
	// reset back to first pair to test.
	
	bool Done() { return (curPairCount >= procs/2); }
	// indicate if all pairs are done (from -tt and .next method)
	
	RankPair & Next() { curPairCount = takeTurns ? curPairCount+1 : procs; return(*this); } 
	// move to next pair in the communication.
	
	bool Myturn(); 
	// indicates it's this ranks turn to communicate. This rank's turn is when the current
	//   pair count = the smaller of the paired ranks from testComm - or if not taking turns.
	//   all other ranks (one's where Myturn is false) should enter a barrier on testComm.
	
    
private:
	int curPairCount;
	bool takeTurns;
	PairingMethodTypes pairType;
	
	int FindProcPair(TestArgs & ta);  
	
	RankPair(const RankPair &);
	RankPair &operator=(const RankPair &);
		
};




#endif
