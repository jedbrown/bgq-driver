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

#include "spi/include/kernel/location.h"
#include "spi/include/upci/upc_atomic.h"
#include <firmware/include/personality.h>
#include <hwi/include/common/uci.h>
#include "spi/include/kernel/memory.h"

#include "../test_utils.h"


__thread char locStrg[1024];
__thread int  thdIdPos = 0;


extern "C" const char *WhoAmI()
{
    if (thdIdPos == 0) {
        Personality_t pers;
        BG_UniversalComponentIdentifier uci;
        Kernel_GetPersonality( &pers, sizeof(pers) );
        uci = pers.Kernel_Config.UCI;
        if (bg_uci_toString( uci, locStrg) < 0) {
            strcpy(locStrg, "unknown");
        }
        int pos = strlen(locStrg);

        Personality_Networks_t & nc = pers.Network_Config;
        pos += sprintf(locStrg+pos, " <%d,%d,%d,%d,%d,%d> r%05d",
                nc.Acoord, nc.Bcoord, nc.Ccoord, nc.Dcoord, nc.Ecoord,
                Kernel_MyTcoord(), Kernel_GetRank());
        thdIdPos = pos;
    }
    sprintf(locStrg+thdIdPos, " tid=%02d", Kernel_ProcessorID());
    return locStrg;
}





extern "C" void GetMemSizes(MemSizes_t * memSizes)
{
    Kernel_GetMemorySize( KERNEL_MEMSIZE_SHARED,          &(memSizes->shared));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_PERSIST,         &(memSizes->persist));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_HEAPAVAIL,       &(memSizes->heapavail));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_ESTHEAPAVAIL,    &(memSizes->estheapavail));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_STACKAVAIL,      &(memSizes->stackavail));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_ESTSTACKAVAIL,   &(memSizes->eststackavail));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_STACK,           &(memSizes->stack));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_ESTSTACK,        &(memSizes->eststack));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_HEAP,            &(memSizes->heap));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_GUARD,           &(memSizes->guard));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_HEAPMAX,         &(memSizes->heapmax));
    Kernel_GetMemorySize( KERNEL_MEMSIZE_MMAP,            &(memSizes->mmap));
}



extern "C" void PrintMemSizes(const char *location, MemSizes_t * memSizes)
{
    fprintf(stderr, "%s shared=0x%016lx  heapavail=0x%016lx  stack=0x%016lx  mmap=0x%016lx\n\n",
            location, memSizes->shared, memSizes->heapavail, memSizes->stack, memSizes->mmap );

#if 0
    fprintf(stderr, "%s Memory Sizes: \n", location);
    fprintf(stderr, "   shared          = 0x%016lx\n", memSizes->shared           );
    fprintf(stderr, "   persist         = 0x%016lx\n", memSizes->persist          );
    fprintf(stderr, "   heapavail       = 0x%016lx\n", memSizes->heapavail        );
    fprintf(stderr, "   estheapavail    = 0x%016lx\n", memSizes->estheapavail     );
    fprintf(stderr, "   stackavail      = 0x%016lx\n", memSizes->stackavail       );
    fprintf(stderr, "   eststackavail   = 0x%016lx\n", memSizes->eststackavail    );
    fprintf(stderr, "   stack           = 0x%016lx\n", memSizes->stack            );
    fprintf(stderr, "   eststack        = 0x%016lx\n", memSizes->eststack         );
    fprintf(stderr, "   heap            = 0x%016lx\n", memSizes->heap             );
    fprintf(stderr, "   guard           = 0x%016lx\n", memSizes->guard            );
    fprintf(stderr, "   heapmax         = 0x%016lx\n", memSizes->heapmax          );
    fprintf(stderr, "   mmap            = 0x%016lx\n", memSizes->mmap             );
#endif
}



extern "C" uint64_t * InitDecHistogram(unsigned buckets, uint64_t minVal, uint64_t maxVal)
{
    uint64_t *p = new uint64_t[buckets+2+2+2];  // 2 entries for # of buckets & valrange per bucket, 2 for min, max values, and 2 for entries less and greater.
    p[0] = buckets;
    p[1] = (maxVal - minVal) / buckets;
    p[2] = minVal;
    p[3] = maxVal;
    unsigned i;
    for (i=4; i<4+p[0]+2; i++) {
        p[i] = 0;
    }
    return p;
}



// Add Entry
extern "C" void Add2DecHistogram(uint64_t *pDecHistogram, uint64_t value)
{
    assert(pDecHistogram);
    uint64_t *p = pDecHistogram;
    uint64_t brange = p[1];
    uint64_t buckets = p[0];
    if      (value < p[2]) p[4]++;
    else if (value > p[2]+brange*buckets) p[4+buckets+1]++;  // cal max based on range incase of bits lost in range calculation value.
    else {
        uint64_t bkt;
        for (bkt=0; bkt<buckets; bkt++) {
            if (value <= (p[2]+brange*(bkt+1))) {
                p[5+bkt]++;
                break;
            }
        }
    }

}



// Print Results
extern "C" void PrintDecHistogram(const char *title, uint64_t *pDecHistogram)
{
    assert(pDecHistogram);
    uint64_t *p = pDecHistogram;
    uint64_t brange = p[1];
    uint64_t buckets = p[0];
    int thd = Kernel_ProcessorID();
    printf("thd=%02d; %s Histogram %ld target buckets:\n", thd, title, buckets);

    char strg[100];
    sprintf(strg, "Instances  < %ld", p[2]);
    printf("thd=%02d;    %-30s : %5ld\n", thd, strg, p[4]);

    uint64_t bkt;
    for (bkt=0; bkt<buckets; bkt++) {
        sprintf(strg, "Instances <= %ld", p[2]+brange*(bkt+1));
        printf("thd=%02d;    %-30s : %5ld\n", thd, strg, p[5+bkt]);
    }

    sprintf(strg, "Instances  > %ld", p[2]+brange*buckets);
    printf("thd=%02d;    %-30s : %5ld\n", thd, strg, p[4+buckets+1]);
    printf("thd=%02d;\n", thd);
}



extern "C" void DeleteDecHistogram(uint64_t *pDecHistogram)
{
    delete[] pDecHistogram;
}

