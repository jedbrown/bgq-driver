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

#include "spi/include/upci/upc_l2.h"
#include "spi/include/upci/testlib/upc_c_debug.h"


#define UPC_C_DEBUG_C
#include "spi/include/kernel/upci.h"

#ifdef __HWSTUBS__
uint64_t dummyDcrArray[32*1024*1024];
#endif



//! \brief: UPC_C_Dump_State
void UPC_C_Dump_State()
{

    // Broad to more specific configuration info
    // Gather regs into mem locations (make sure all can be read)

    // upc_c_sram_data_t data;
    // uint64_t mode1_count[128];



    PRINTF_X( "-----------------------------------------\n"
            "UPC_C Unit : Addr= 0x%011lx\n", (uint64_t)upc_c);
    PRINTF_X("c_config                  = 0x%016lx\n", upc_c->c_config);
    PRINTF_X("mode1_ec_config           = 0x%016lx\n", upc_c->mode1_ec_config);
    PRINTF_X("status                    = 0x%016lx\n", upc_c->status);
    PRINTF_X("mode1_counter_control0_rw = 0x%016lx\n", upc_c->mode1_counter_control0_rw);
    PRINTF_X("mode1_counter_control1_rw = 0x%016lx\n", upc_c->mode1_counter_control1_rw);
    PRINTF_X("mode1_counter_control2_rw = 0x%016lx\n", upc_c->mode1_counter_control2_rw);
    PRINTF_X("ccg_int_en                = 0x%016lx\n", upc_c->ccg_int_en);
    PRINTF_X("ccg_int_status_w1s        = 0x%016lx\n", upc_c->ccg_int_status_w1s);
    PRINTF_X("io_ccg_int_en             = 0x%016lx\n", upc_c->io_ccg_int_en);
    PRINTF_X("io_ccg_int_status_w1s     = 0x%016lx\n", upc_c->io_ccg_int_status_w1s);
    PRINTF_X("accum_control             = 0x%016lx\n", upc_c->accum_control);
    PRINTF_X("error_force               = 0x%016lx\n", upc_c->error_force);
    PRINTF_X("m2_trigger                = 0x%016lx\n", upc_c->m2_trigger);
    PRINTF_X("counter_start             = 0x%016lx\n", upc_c->counter_start);
    PRINTF_X("interrupt_state__state        = 0x%016lx\n", upc_c->interrupt_state__state);
    PRINTF_X("interrupt_state_control_low  = 0x%016lx\n", upc_c->interrupt_state_control_low);
    PRINTF_X("interrupt_state_control_high = 0x%016lx\n", upc_c->interrupt_state_control_high);
    // PRINTF_X("interrupt_state__force    = 0x%016lx\n", upc_c->interrupt_state__force);  (write only)
    PRINTF_X("interrupt_state__first        = 0x%016lx\n", upc_c->interrupt_state__first);
    PRINTF_X("interrupt_state__machinecheck = 0x%016lx\n", upc_c->interrupt_state__machinecheck);
    PRINTF_X("interrupt_state__critical     = 0x%016lx\n", upc_c->interrupt_state__critical);
    PRINTF_X("interrupt_state__noncritical  = 0x%016lx\n", upc_c->interrupt_state__noncritical);
    PRINTF_X("upc_sram_parity_info          = 0x%016lx\n", upc_c->upc_sram_parity_info);
    PRINTF_X("upc_io_sram_parity_info       = 0x%016lx\n", upc_c->upc_io_sram_parity_info);
    PRINTF_X("interrupt_internal_error__state        = 0x%016lx\n", upc_c->interrupt_internal_error__state);
    PRINTF_X("interrupt_internal_error_control_high = 0x%016lx\n", upc_c->interrupt_internal_error_control_high);
    // PRINTF_X("interrupt_internal_error__force        = 0x%016lx\n", upc_c->interrupt_internal_error__force);  (Write only)
    PRINTF_X("interrupt_internal_error__first        = 0x%016lx\n", upc_c->interrupt_internal_error__first);
    PRINTF_X("interrupt_internal_error_sw_info       = 0x%016lx\n", upc_c->interrupt_internal_error_sw_info);
    PRINTF_X("interrupt_internal_error_hw_info       = 0x%016lx\n", upc_c->interrupt_internal_error_hw_info);
    PRINTF_X("interrupt_internal_error_data_info     = 0x%016lx\n", upc_c->interrupt_internal_error_data_info);
    PRINTF_X("interrupt_internal_error__machinecheck = 0x%016lx\n", upc_c->interrupt_internal_error__machinecheck);
    PRINTF_X("interrupt_internal_error__critical     = 0x%016lx\n", upc_c->interrupt_internal_error__critical);
    PRINTF_X("interrupt_internal_error__noncritical  = 0x%016lx\n", upc_c->interrupt_internal_error__noncritical);
    PRINTF_X("overflow_state__state         = 0x%016lx\n", upc_c->overflow_state__state);
    PRINTF_X("overflow_state_control_low   = 0x%016lx\n", upc_c->overflow_state_control_low);
    PRINTF_X("overflow_state_control_high  = 0x%016lx\n", upc_c->overflow_state_control_high);
    // PRINTF_X("overflow_state__force         = 0x%016lx\n", upc_c->overflow_state__force);  (write only)
    PRINTF_X("overflow_state__first         = 0x%016lx\n", upc_c->overflow_state__first);
    PRINTF_X("overflow_state__machinecheck  = 0x%016lx\n", upc_c->overflow_state__machinecheck);
    PRINTF_X("overflow_state__critical      = 0x%016lx\n", upc_c->overflow_state__critical);
    PRINTF_X("overflow_state__noncritical   = 0x%016lx\n", upc_c->overflow_state__noncritical);


}






//! \brief: UPC_L2_Dump_State
void UPC_L2_Dump_State()
{
    // Dump the L2 Slice UPC State
    int i;
    PRINTF( "-----------------------------------------\n" );
    PRINTF_X("upc_c->c_config = 0x%016lx\n", upc_c->c_config);
    for (i=0; i<L2_DCR_num; i++) {
        PRINTF( "UPC L2 Unit %d: \n", i);

        PRINTF_X("   upc_l2_counter_control = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_COUNTER_CONTROL_RW)));
        PRINTF_X("   upc_l2_config          = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_CONFIG)));
#if 0
        PRINTF_X("   upc_l2_counter[0]      = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_COUNTER)+0));
        PRINTF_X("   upc_l2_counter[1]      = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_COUNTER)+1));
        PRINTF_X("   upc_l2_counter[2]      = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_COUNTER)+2));
        PRINTF_X("   upc_l2_counter[3]      = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_COUNTER)+3));
        PRINTF_X("   upc_l2_counter[4]      = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_COUNTER)+4));
        PRINTF_X("   upc_l2_counter[5]      = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_COUNTER)+5));
        PRINTF_X("   upc_l2_counter[6]      = 0x%016lx\n", DCRReadUser(L2_DCR(i, UPC_L2_COUNTER)+6));
#endif
    }
}




//! \brief: UPC_C_Dump_M1P_Counters
void UPC_C_Dump_M1P_Counters(int first_thread, int last_thread)
{
    PRINTF_X( "-----------------------------------------\n"
               "UPC_C Unit : Addr= 0x%011lx\n", (uint64_t)upc_c);

    int thdNum;
    if (first_thread < 0) last_thread = first_thread;

    for (thdNum = first_thread; thdNum <= last_thread; thdNum++) {
        PRINTF_XXXXXXX("Thd %2d  0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx\n",
                thdNum,
                upc_c->data16.grp[thdNum].counter[0],
                upc_c->data16.grp[thdNum].counter[1],
                upc_c->data16.grp[thdNum].counter[2],
                upc_c->data16.grp[thdNum].counter[3],
                upc_c->data16.grp[thdNum].counter[4],
                upc_c->data16.grp[thdNum].counter[5]);

    }
}


//! \brief: UPC_C_Dump_M1_Counters
void UPC_C_Dump_M1_Counters(int first_group_num, int last_group_num)
{
    PRINTF_X( "-----------------------------------------\n"
               "UPC_C Unit : Addr= 0x%011lx\n", (uint64_t)upc_c);

    int grpNum;
    if (last_group_num < 0) last_group_num = first_group_num;

    for (grpNum = first_group_num; grpNum <= last_group_num; grpNum++) {
        PRINTF_XXXXXXXXX("Grp %2d  0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx\n",
                grpNum,
                upc_c->datam1.grp[grpNum].counter[0],
                upc_c->datam1.grp[grpNum].counter[1],
                upc_c->datam1.grp[grpNum].counter[2],
                upc_c->datam1.grp[grpNum].counter[3],
                upc_c->datam1.grp[grpNum].counter[4],
                upc_c->datam1.grp[grpNum].counter[5],
                upc_c->datam1.grp[grpNum].counter[6],
                upc_c->datam1.grp[grpNum].counter[7]);

    }

}



//! \brief: UPC_C_Dump_Counters
void UPC_C_Dump_Counters(int first_group_num, int last_group_num)
{
    PRINTF_X( "-----------------------------------------\n"
               "UPC_C Unit : Addr= 0x%011lx\n", (uint64_t)upc_c);

    int grpNum;
    int countNum;
    if (last_group_num < 0) last_group_num = first_group_num;

    for (grpNum = first_group_num; grpNum <= last_group_num; grpNum++) {
        for (countNum = 0; countNum < 16; countNum++) {
            PRINTF_XXX("Counter %2d/%-2d 0x%016lx\n", grpNum, countNum, upc_c->data16.grp[grpNum].counter[countNum]);
        }
    }

}



//! \brief: UPC_C_Dump_IO_Counters
void UPC_C_Dump_IO_Counters(int first_count, int last_count)
{
    PRINTF_X( "-----------------------------------------\n"
               "UPC_C Unit : Addr= 0x%011lx\n", (uint64_t)upc_c);
    if (last_count < 0) { last_count = 63; }

    int countNum;
    for (countNum = first_count; countNum <= last_count; countNum++) {
        PRINTF_XX("ioCount %02d 0x%016lx\n", countNum, upc_c->iosram_data[countNum]);
    }

}



//! \brief: UPC_C_Dump_M2_Trace_Records
void UPC_C_Dump_M2_Trace_Records(upc_trace_buff_t *tbuff, short start, short len)
{
    PRINTF_XX("Mode 2 Raw Trace Records. Triggered=%d, TriggerRec=%d:\n",
               tbuff->triggered, tbuff->trigger_rec);
    PRINTF("Record#   Trig_DataBits(48)  TriggerRecord\n");
    int rnum;
    int trig_low = tbuff->trigger_rec;
    int trig_hi = tbuff->trigger_rec + 6;
    if (len == 0) len = UPC_MAX_TRACE_RECS;
    short maxrec = UPCI_MIN(start+len, UPC_MAX_TRACE_RECS);
    for (rnum=start; rnum<maxrec; rnum++) {
        if ((rnum >= trig_low) && (rnum < trig_hi)) {
            PRINTF_XXX( " %04d       0x%01lx_%011lx    <--- Trigger\n",
                    rnum, (tbuff->rec[rnum] >> 44), (tbuff->rec[rnum] & 0x0FFFFFFFFFFFLU));
        }
        else {
            PRINTF_XXX( " %04d       0x%01lx_%011lx\n",
                    rnum, (tbuff->rec[rnum] >> 44), (tbuff->rec[rnum] & 0x0FFFFFFFFFFFLU));
        }
    }
}






