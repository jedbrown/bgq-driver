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
#ifndef QUIK_PROFILER_H
#define QUIK_PROFILER_H


#include <stdlib.h>
#include <stdio.h>
#include "hwi/include/common/compiler_support.h"


#define MAX_MAP_ENTRIES 1024

__BEGIN_DECLS;

// Return a new profile map handle
uint64_t GetNewProfileMapHandle();

// Return a new profile map handle
void FreeProfileMapHandle(uint64_t handle);

// Return a hit to address in ProfileMap
void RecordAddressHit(uint64_t hProf, uint64_t addr);

// Print out the top num hit Entries in profile map and return the total ovfs recorded
int PrintTopHits(uint64_t hProf, int num);


#define QUIK_MAXTOPHITS  50
// Get array of top hits.
// *pNum value replaced with number of addresses actually returned.
typedef struct {
    uint64_t numAddrs;
    uint64_t addr[QUIK_MAXTOPHITS];
    uint64_t count[QUIK_MAXTOPHITS];
    uint64_t other;  // other hits not in the top hits.
    uint64_t lost;   // hit address lost (could not be recorded) because map was full
    uint64_t total;  // total hits.
} QuikHit_t;

void GetTopHits(uint64_t hProf, int num, QuikHit_t *pHits);


// Print out counts including those with overflow.
void PrintCountsInclOvf(unsigned hEvtSet);


__END_DECLS

#endif
