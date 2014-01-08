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


#include "stdlib.h"
#include "stdio.h"


#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"


#define PRINTERR(...) fprintf(stderr, "ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }
#define BGPMDIE(_funcname_) { DIE(_funcname_ ": %d; %s\n", Bgpm_LastErr(), Bgpm_LastErrStrg()); }



#define ADD_EVENT(hEvtSet, evtId)  \
{                                    \
    int rc = Bgpm_AddEvent(hEvtSet, evtId); \
    if (rc < 0) BGPMDIE("Bgpm_AddEvent"); \
}



void PrintCounts(unsigned hEvtSet)
{
    fprintf(stderr, "Counter Results:\n");
    int i;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    uint64_t cnt;
    for (i=0; i<numEvts; i++) {
        if (Bgpm_ReadEvent(hEvtSet, i, &cnt)) BGPMDIE("Bgpm_ReadEvent");
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        if (!label) BGPMDIE("Bgpm_GetEventLabel");
        fprintf(stderr, "    0x%016lx <= %s\n", cnt, label);
    }
}




void CreateEvents()
{
    // create some events.
#define DSIZE 256
    volatile double target[DSIZE];
    volatile double source[DSIZE];
    //fprintf(stderr, _AT_ "\n");
    unsigned i,j;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }
    //fprintf(stderr, _AT_ "\n");

    for (i=0; i<DSIZE; i++) {
        for (j=0; j<6; j++) {
            target[i] *= source[i];
            source[i] = j;
        }
    }
    //fprintf(stderr, _AT_ "\n");

}





int main(int argc, char *argv[])
{

    fprintf(stderr, "Produce a big overflow with subsequent RAS message - need to manually check\n");

    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    int hEvtSet = Bgpm_CreateEventSet();

    ADD_EVENT(hEvtSet, PEVT_L1P_BAS_LD);
    ADD_EVENT(hEvtSet, PEVT_L1P_BAS_ST_WC);
    ADD_EVENT(hEvtSet, PEVT_L1P_STRM_WRT_INVAL);
    ADD_EVENT(hEvtSet, PEVT_XU_BR_COMMIT);
    ADD_EVENT(hEvtSet, PEVT_CYCLES);


    Bgpm_Apply(hEvtSet);

    Bgpm_Start(hEvtSet);
    CreateEvents();
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);


    Bgpm_Start(hEvtSet);


    // muck up the ring for core 0 to cause big overflow and a ras message
    UPC_P__CONFIG__JOIN_RING_insert(upc_p_local->p_config, 0);


    CreateEvents();
    Bgpm_Stop(hEvtSet);
    PrintCounts(hEvtSet);



    return 0;
}

