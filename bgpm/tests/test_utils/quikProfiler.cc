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
#include <map>
#include <vector>
#include <algorithm>
#include <memory>

#include "hwi/include/common/compiler_support.h"
#include "quikProfiler.h"
#include "bgpm/include/bgpm.h"

using namespace std;

#include "map_allocator.h"

typedef pair<uint64_t,uint64_t> HitPair;
typedef pair<const uint64_t,uint64_t> ConstHitPair;
typedef vector<HitPair>    AddrList;

typedef std::map<uint64_t, uint64_t, std::less<uint64_t>,
        MapAddrAllocator<ConstHitPair, MAX_MAP_ENTRIES> > AddrMap;




extern "C" uint64_t GetNewProfileMapHandle()
{
    AddrMap *newmap = new AddrMap();
    (*newmap)[~0ULL] = 0;   // reserve space for when out of space for new entries
    return (uint64_t)newmap;
}



extern "C" void FreeProfileMapHandle(uint64_t handle)
{
    delete (AddrMap *)handle;
}



extern "C" void RecordAddressHit(uint64_t hProf, uint64_t addr)
{
    assert(hProf);
    //fprintf(stderr, _AT_ " hProf=0x%016lx, addr=0x%016lx\n", hProf, addr);
    AddrMap * addrMap = (AddrMap*)hProf;
    try {
        (*addrMap)[addr] += 1;
    }
    catch (...) {
        (*addrMap)[~0ULL] += 1;  // no room for more new entries
    }
}


bool SortByHits(const HitPair & lp, const HitPair & rp)
{
    //fprintf(stderr, "lp.second=%ld, rp.second=%ld\n", lp.second, rp.second);
    return (lp.second > rp.second);
}


extern "C" int PrintTopHits(uint64_t hProf, int num)
{
    QuikHit_t hits;
    GetTopHits(hProf, num, &hits);

    fprintf(stderr, "Number of Unique Addresses profiled = %ld\n", hits.numAddrs);
    fprintf(stderr, "     addr               hits\n");
    unsigned i;
    for (i=0; i<hits.numAddrs; i++) {
        fprintf(stderr, "0x%016lx : %7ld\n", hits.addr[i], hits.count[i]);
    }
    if (hits.other) {
        fprintf(stderr, "  other addr hits  : %7ld\n", hits.other);
    }
    if (hits.lost) {
        fprintf(stderr, "  lost addr hits   : %7ld\n", hits.lost);
    }
    fprintf(stderr, "        total ovfs : %7ld\n", hits.total);
    return hits.total;
}



extern "C" void GetTopHits(uint64_t hProf, int num, QuikHit_t *pHits)
{
    AddrMap & addrMap = *(AddrMap*)hProf;
    //AddrMap::iterator pos;
    AddrList::iterator pos;
    assert(num>0);
    assert(pHits);
    num = UPCI_MIN((unsigned)num, QUIK_MAXTOPHITS);
    num = UPCI_MIN((unsigned)num, addrMap.size());
    memset(pHits, 0, sizeof(QuikHit_t));

    auto_ptr<vector<HitPair> > pHitList(new vector<HitPair>);

    pHitList->assign(addrMap.begin(), addrMap.end());
    sort(pHitList->begin(), pHitList->end(), SortByHits);


    int total = 0;
    int other = 0;
    int lost = 0;
    int i = 0;
    for (pos = pHitList->begin(); pos != pHitList->end(); ++pos) {
        //fprintf(stderr, "first=0x%016lx, second=%ld\n", pos->first, pos->second);
        if (i < num) {
            if (pos->first == ~0ULL) {
                lost = pos->second;
            }
            else {
                pHits->addr[i] = pos->first;
                pHits->count[i] = pos->second;
                i++;
            }
        }
        else {
            other += pos->second;
        }
        total += pos->second;
    }
    if (other) {
        pHits->other = other;
    }
    if (lost) {
        pHits->lost = lost;
    }
    pHits->total = total;
    pHits->numAddrs = i;

}



extern "C" void PrintCountsInclOvf(unsigned hEvtSet)
{
    int numEvts = Bgpm_NumEvents(hEvtSet);
    int i;
    // calc label width
    unsigned lblWidth = 0;
    for (i=0; i<numEvts; ++i) {
        lblWidth = UPCI_MAX(lblWidth, strlen(Bgpm_GetEventLabel(hEvtSet, i)));
    }
    char format1[100];
    char format2[100];
    sprintf(format1, "   %%12ld <- %%-%ds\n", lblWidth);
    sprintf(format2, "   %%12ld <- %%-%ds ovfPeriod=%%d\n", lblWidth);


    fprintf(stderr, "NumEvents=%d:\n", numEvts);
    for (i=0; i<numEvts; ++i) {
        uint64_t val;
        Bgpm_ReadEvent(hEvtSet, i, &val);

        uint64_t period;
        Bgpm_OverflowHandler_t handler;
        Bgpm_GetOverflow(hEvtSet, i, &period, &handler);

        if (period) {
            fprintf(stderr, format2, val, Bgpm_GetEventLabel(hEvtSet, i), period);
        }
        else {
            fprintf(stderr, format1, val, Bgpm_GetEventLabel(hEvtSet, i));
        }
    }
}




