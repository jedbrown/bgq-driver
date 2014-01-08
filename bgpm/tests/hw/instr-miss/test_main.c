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
#include <omp.h>
#include "hwi/include/bqc/A2_inlines.h"
#include "spi/include/kernel/location.h"

#include "spi/include/upci/testlib/upc_p_debug.h"

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


#define STARTPASS \
    Upci_Punit_Start(&punit, (UPCI_CTL_RESET | UPCI_CTL_DELAY)); \
    for (i=0; i<LOOPS; i++)

#define FINISHPASS(_num_) \
    Upci_Punit_Stop(&punit, (UPCI_CTL_DELAY));   \
    misses = Upci_Punit_Event_Read(&punit,hEvt[0]);   \
    miss_cyc = Upci_Punit_Event_Read(&punit,hEvt[1]);   \
    instrs = Upci_Punit_Event_Read(&punit,hEvt[2]);   \
    printf("len=%04d; il1_misses=%ld;  il1_miss_cyc=%ld, instrs=%ld;  misses/instr=%lf\n",   \
            _num_, misses, miss_cyc, instrs, (double)misses/instrs);



#define GTB GetTimeBase()                //!< short name for convenience
typedef uint64_t cycle_t;                //!< cycle count types returned from GetTimeBase();


#define NOP asm volatile("nop;")
#define LOOPS 100


int main(int argc, char *argv[])
{
    printf("Vary num of instrs to verify instr misses and instr cache len\n");
    //upc_p_mmio_t *const upc_p = UPC_P_Addr(UPC_LOCAL_UNIT);

    // SPI Objects
    Upci_Mode_t upciMode;
    Upci_Punit_t punit;

    Upci_Mode_Init(&upciMode, 0, UPC_CM_INDEP, Kernel_ProcessorCoreID());
    Upci_Punit_Init(&punit, &upciMode, Kernel_ProcessorCoreID());

    // Arrays to hold on to event handles.
    Upci_Punit_EventH_t hEvt[24];
    int cNum = 0;

    // Add events in this order:
    //   1) l1p/Wakeup
    //   2) A2 XU
    //   other A2 3rd, Opcode events last (though none in this list)
    short thd = Kernel_PhysicalHWThreadID();
    ADD_PUNIT_EVENT(PEVT_IU_IL1_MISS, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_IU_IL1_MISS_CYC, thd, hEvt[cNum++]);
    ADD_PUNIT_EVENT(PEVT_INST_ALL, thd, hEvt[cNum++]);

    int rc = Upci_Punit_Apply(&punit);
    if (rc) {
        DIE("Upci_Punit_Apply failed; rc = %d\n", rc);
    }

    Kernel_Upci_Wait4MailboxEmpty();

    //Upci_Punit_Dump(0,&punit);
    //UPC_P_Dump_State(UPC_LOCAL_UNIT);

    int i;
    uint64_t misses;
    uint64_t miss_cyc;
    uint64_t instrs;



    STARTPASS
    { NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;

      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;

      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;

      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;

      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
    }
    FINISHPASS(500);

    STARTPASS
    { NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;

      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;

      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;

      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;

      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
    }
    FINISHPASS(500);

    STARTPASS
    { NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
    }
    FINISHPASS(100);

    STARTPASS
    { NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
      NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP;
    }
    FINISHPASS(50);

    STARTPASS
    { NOP; NOP; NOP; NOP; NOP;   NOP; NOP; NOP; NOP; NOP; }
    FINISHPASS(10);

    STARTPASS
    { NOP; NOP; NOP; NOP; NOP; }
    FINISHPASS(5);

    STARTPASS
    { NOP; }
    FINISHPASS(1);


    return(0);
}
