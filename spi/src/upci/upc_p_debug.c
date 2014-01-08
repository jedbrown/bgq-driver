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
#include <string.h>
#include "us_debug.h"

#include "spi/include/upci/testlib/upc_p_debug.h"


//! \brief: UPC_P_Dump_State
void UPC_P_Dump_State_Indent( int unit, int indent )
{
    INIT_INDENT();

    upc_p_mmio_t *const upc_p = UPC_P_Addr(unit);

    // Broad to more specific configuration info
    // Gather counters info mem locations (make sure all can be read)

    // unit = core
    // upcp addr
    // print regs in order from the structure.
    uint64_t counter[UPC_P_NUM_COUNTERS];       // 14 bit counter array + extra bits.
    uint64_t counter_cfg[UPC_P_NUM_COUNTERS];   // counter configuration array
    uint64_t cfg_invert;                        // event signal inversion selection
    uint64_t cfg_edge;                          // event signal edge/level selection
    uint64_t int_status_t0;                     // thread 0 counter interrupt status (read only)
    uint64_t int_status_t1;                     // thread 1 counter interrupt status (read only)
    uint64_t int_status_t2;                     // thread 2 counter interrupt status (read only)
    uint64_t int_status_t3;                     // thread 3 counter interrupt status (read only)
    uint64_t opcode_cfg;                        // opcode counting config
    uint64_t p_config;                          // upc_p config reg
    uint64_t lp1_tc_config0;                    // upc_p lp1 thread combine config
    uint64_t lp1_tc_config1;                    // upc_p lp1 thread combine config
    uint64_t a2_tc_config;                      // upc_p a2 thread combine config
    uint64_t control_w1c;                       // counter control read/write reg
    uint64_t control_w1s;                       // counter control read/write reg
    uint64_t control;                           // counter control read/write reg

    int i;
    for (i=0; i<UPC_P_NUM_COUNTERS; i++) {
        counter[i]     = upc_p->counter[i];
        counter_cfg[i] = upc_p->counter_cfg[i];
    }
    cfg_invert      = upc_p->cfg_invert;
    cfg_edge        = upc_p->cfg_edge;
    int_status_t0   = upc_p->int_status[0];
    int_status_t1   = upc_p->int_status[1];
    int_status_t2   = upc_p->int_status[2];
    int_status_t3   = upc_p->int_status[3];
    opcode_cfg      = upc_p->opcode_cfg;
    p_config        = upc_p->p_config;
    lp1_tc_config0  = upc_p->lp1_tc_config0;
    lp1_tc_config1  = upc_p->lp1_tc_config1;
    a2_tc_config    = upc_p->a2_tc_config;
    control_w1c     = upc_p->control_w1c;
    control_w1s     = upc_p->control_w1s;
    control         = upc_p->control;


    PRINTF_XX( "%s-----------------------------------------\n"
            "%sUPC_P Core Unit %d : Addr= 0x%011lx\n"
            "%sCounters & Config:\n",  indentStrg, indentStrg, unit, (uint64_t)upc_p, indentStrg);
    for (i=0; i<UPC_P_NUM_COUNTERS; i++) {
        PRINTF_XXX("%s%02d  0x%016lx  0x%016lx\n", indentStrg, i, counter[i],  counter_cfg[i]);
    }
    PRINTF_XX(  "%sinvert/edge         = 0x%016lx 0x%016lx\n", indentStrg, cfg_invert,  cfg_edge);
    PRINTF_XXXX("%sint status t0,1,2,3 = 0x%016lx 0x%016lx 0x%016lx 0x%016lx\n", indentStrg, int_status_t0, int_status_t1, int_status_t2, int_status_t3);
    PRINTF_X(   "%sopcode_cfg          = 0x%016lx\n", indentStrg, opcode_cfg);
    PRINTF_X(   "%sp_config            = 0x%016lx\n", indentStrg, p_config);
    PRINTF_XXX( "%sthr comb l1p        = 0x%016lx 0x%016lx  a2=0x%016lx\n", indentStrg, lp1_tc_config0, lp1_tc_config1, a2_tc_config);
    PRINTF_X(   "%scontrol             = 0x%016lx\n", indentStrg, control);

    volatile uint64_t * l1p_p = UPC_L1p_Addr(unit);

    PRINTF_X(   "%sl1pUPCMode          = 0x%016lx\n\n", indentStrg, *l1p_p);
}



void UPC_P_Dump_M2_Parms(int unit)
{
    PRINTF("Mode 2 A2 Trace Parameters:\nNOT YET IMPLEMENTED\n");

    //! \todo add kernel SPI get of scom values.

#if 0
    upc_p_mmio_t *const upc_p = UPC_P_Addr(unit);
    uint64_t p_config = upc_p->p_config;

    if (unit<0) { unit = PhysicalProcessorID(); }
    PRINTF_X("   core  = %d\n", unit );
    PRINTF_X("   p_cfg = 0x%016lx\n", p_config );
    PRINTF_X("   pccr0 = 0x%016lx\n", pccr0);
    PRINTF_X("   abdsr = 0x%016lx\n", abdsr);
    PRINTF_X("   idsr  = 0x%016lx\n", idsr );
    PRINTF_X("   mpdsr = 0x%016lx\n", mpdsr);
    PRINTF_X("   xdsr1 = 0x%016lx\n", xdsr1);
    PRINTF_X("   xdsr2 = 0x%016lx\n", xdsr2);
#endif
}



