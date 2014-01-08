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


/** \example spi/punit_1/test_main.c
 * This is a simple example of using the Upci Punit with sync counting.
 * UPCI is an internal interface and direct use by user programs is deprecated.
 * Use the Bgpm API instead.
 *
 * The output from running this example looks something like this:
 * \include spi/punit_1/output.txt
 */


#include "spi/include/upci/mode.h"
#include "spi/include/upci/punit.h"


#define PRINTERR(...) printf("ERROR(" _AT_ ") " __VA_ARGS__);
#define DIE(...) { PRINTERR(__VA_ARGS__); exit(-1); }


#define ADD_PUNIT_EVENT(evt, thd, handle)  \
{                                    \
    int rc = Upci_Punit_AddEvent(&punit, evt, thd, &(handle));      \
    if (rc < 0) {                                                 \
        PRINTERR("Add of event %d Failed, rc=%d\n", evt, rc);       \
    } \
}



void PrintCounts(Upci_Punit_t *pPunit, Upci_Punit_EventH_t hEvt[], int startIdx, int endIdx)
{
    int i;
    for (i=startIdx; i<=endIdx; i++) {
        printf("0x%016lx <= %s\n",
                Upci_Punit_Event_Read(pPunit,hEvt[i]),
                Upci_Punit_GetEventLabel(pPunit,hEvt[i]));
    }
}




void CreateSomeEvents()
{
    // create some events.
#define DSIZE 256
    volatile double target[DSIZE];
    volatile double source[DSIZE];
    short i,j;
    for (i=0; i<DSIZE; i++) {
        source[i] = i;
        target[i] = i+1;
    }
    for (i=0; i<DSIZE; i++) {
        for (j=0; j<6; j++) {
            target[i] *= source[i];
            source[i] = j;
        }
    }
}





int main(int argc, char *argv[])
{
    printf("punit_1: Example tracking small sample of Punit Events\n");

    // SPI Objects
    Upci_Mode_t upciMode;
    Upci_Punit_t punit;

    Upci_Mode_Init(&upciMode, 0, UPC_CM_INDEP, Kernel_ProcessorCoreID());
    Upci_Punit_Init(&punit, &upciMode, Kernel_ProcessorCoreID());

    // Arry to hold on to event handles.
    Upci_Punit_EventH_t hEvt[24];
    int cNum = 0;


    // Add events in this order:
    //   1) l1p/Wakeup
    //   2) A2 XU
    //   other A2 3rd, Opcode events last (though none in this list)
    short thd = Kernel_PhysicalHWThreadID();
    ADD_PUNIT_EVENT(PEVT_L1P_BAS_LD, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_L1P_BAS_ST_WC, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_L1P_STRM_WRT_INVAL, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_XU_BR_COMMIT, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_CYCLES, thd, hEvt[cNum++]);



    int rc = Upci_Punit_Apply(&punit);
    if (rc) {
        DIE("Upci_Punit_Apply failed; rc = %d\n", rc);
    }

    else {
        Upci_Punit_Start(&punit, (UPCI_CTL_RESET | UPCI_CTL_DELAY));

        CreateSomeEvents();

        Upci_Punit_Stop(&punit, (UPCI_CTL_DELAY));

        printf("Counter Results:\n");
        PrintCounts(&punit, hEvt, 0, cNum-1);
    }


    exit(0);
}
