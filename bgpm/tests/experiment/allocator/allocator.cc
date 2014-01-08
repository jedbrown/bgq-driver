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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits>
#include <map>
#include <vector>
#include <algorithm>
#include <memory>

#include "hwi/include/common/compiler_support.h"
#include "bgpm/include/bgpm.h"
#include "../../test_utils/map_allocator.h"

using namespace std;

int numThreads;
int numProcs;




typedef pair<uint64_t,int> HitPair;
typedef pair<const uint64_t,int> ConstHitPair;

typedef map<uint64_t, int, std::less<uint64_t>,
       MapAddrAllocator<ConstHitPair, 3> > AddrMap;

typedef vector<HitPair> AddrList;


int main(int argc, char *argv[])
{

    if (argc < 2) {
        fprintf(stderr, "ERROR: Pass in number of threads\n");
        exit(-1);
    }
    numThreads = atoi(argv[1]);
    numProcs = Kernel_ProcessCount();

    fprintf(stderr, "numProcesses=%d, numThreads=%d\n", numProcs, numThreads);

#if 0
    vector<int, MyAllocator<int> > stuff;

    fprintf(stderr, _AT_ "\n");
    stuff.push_back(1);
    fprintf(stderr, _AT_ "\n");
    stuff.push_back(2);
    fprintf(stderr, _AT_ "\n");
    stuff.push_back(3);
    fprintf(stderr, _AT_ "\n");
#endif




    AddrMap am;

    try
    {
        uint64_t addr = 0x0000100000ULL;
        am[addr] += 1;

        addr = 0x0000200000ULL;
        am[addr] += 1;

        addr = 0x0000300000ULL;
        am[addr] += 1;

        addr = 0x0000400000ULL;
        am[addr] += 1;

        addr = 0x0000100000ULL;
        am[addr] += 1;
    }
    catch (...) {
        fprintf(stderr, _AT_ " out of map allocation space\n");
    }


    AddrMap & addrMap = am;
    //AddrMap::iterator pos;
    AddrList::iterator pos;

    int num = addrMap.size();

    auto_ptr<vector<HitPair> > pHitList(new vector<HitPair>);

    pHitList->assign(addrMap.begin(), addrMap.end());


    fprintf(stderr, "Number of Unique Addresses profiled = %d\n", (int)addrMap.size());
    fprintf(stderr, "     addr               hits\n");
    //for (pos = addrMap.begin(); pos != addrMap.end(); ++pos) {
    //    fprintf(stderr, "0x%016lx : %6d\n", pos->first, pos->second);
    //}
    int total = 0;
    int other = 0;
    for (pos = pHitList->begin(); pos != pHitList->end(); ++pos) {
        if (num-- > 0) {
            fprintf(stderr, "0x%016lx : %7d\n", pos->first, pos->second);
        }
        else {
            other += pos->second;
        }
        total += pos->second;
    }
    if (other) {
        fprintf(stderr, "       other addrs : %7d\n", other);
    }
    fprintf(stderr, "        total ovfs : %7d\n", total);

    return (0);
}
