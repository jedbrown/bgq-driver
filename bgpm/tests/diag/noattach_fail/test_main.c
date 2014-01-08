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
#include "spi/include/l1p/sprefetch.h"


//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


#define NUM_THREADS 68
#define BGPMDIE(_funcname_) { DIE(_funcname_ ": %d; %s\n", Bgpm_LastErr(), Bgpm_LastErrStrg()); }

#define ADD_EVENT(hEvtSet, evtId)  \
{                                    \
    int rc = Bgpm_AddEvent(hEvtSet, evtId); \
    if (rc < 0) BGPMDIE("Bgpm_AddEvent"); \
}


void PrintPuCounts(int hPuSet, int startThd)
{
    uint64_t cnt;
    int evtIdx, endThd;

    for (; startThd<NUM_THREADS; startThd+=8) {
        endThd = MIN(NUM_THREADS-1, startThd+7);

        fprintf(stderr, "\nPunit Counts (%d - %d):\n", startThd, endThd);
        int numEvts = Bgpm_NumEvents(hPuSet);
        for (evtIdx=0; evtIdx<numEvts; evtIdx++) {
            const char *label = Bgpm_GetEventLabel(hPuSet, evtIdx);
            char outstrg[1024];
            int pos = 0;
            pos += sprintf(&outstrg[pos], "%-30s ", label);
            int thd;
            for (thd=startThd; thd<=endThd; thd++) {
                uint64_t appThdMask = UPC_APP_HWTID_MASK(thd);
                uint64_t agentThdMask = UPC_AGENT_HWTID_MASK(thd);
                Bgpm_ReadThreadEvent(hPuSet, evtIdx, appThdMask, agentThdMask, &cnt);
                pos += sprintf(&outstrg[pos], "0x%016lx ", cnt);
            }
            pos += sprintf(&outstrg[pos], "\n");
            fprintf(stderr, outstrg);
        }
    }
}



int main(int argc, char *argv[])
{
    TEST_MODULE(Check BGPM Init Mode parameters);
    if (argc > 1) {
       //strcpy(mode,*argv[1]);
       //mode = *argv[1];
      printf("mode=%s\n",argv[1]);
    }

   Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error


   int i;
   for (i=0; i<20; i++) {
   	TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_HWDISTRIB),0);
   	// Bgpm_Init(*argv[1]);

    	// set to look at Punit Counters
    	int hPuSet = Bgpm_CreateEventSet();
    	ADD_EVENT(hPuSet, PEVT_L1P_BAS_LD);
    	ADD_EVENT(hPuSet, PEVT_L1P_BAS_ST_WC);
    	ADD_EVENT(hPuSet, PEVT_L1P_STRM_WRT_INVAL);
    	ADD_EVENT(hPuSet, PEVT_XU_BR_COMMIT);
    	ADD_EVENT(hPuSet, PEVT_CYCLES);

    	fprintf(stderr, "NumEvents = %d\n", Bgpm_NumEvents(hPuSet));

    	//Bgpm_Init(BGPM_MODE_HWDISTRIB); 
    	TEST_CHECK_EQUAL(Bgpm_Start(hPuSet), BGPM_ENOT_APPLIED);
    	TEST_CHECK_EQUAL(Bgpm_Disable(), 0);
    }

    TEST_RETURN();
}

