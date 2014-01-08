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


//! \file
//! UPC_C Control Functions
//!
//! Common UPC_C Operations and Utilities (See upc_c.h) for descriptions
//#include <firmware/include/fwext/fwext.h>
//#include <firmware/include/fwext/fwext_lib.h>



#include <stdio.h>
#include <string.h>

#define UPC_C_C    // define global vars in H file.
#include "spi/include/upci/upc_c.h"
#include "spi/include/upci/upc_l2.h"
#include "spi/include/upci/upc_io.h"
#include "spi/include/kernel/location.h"
#include "cnk/include/Config.h"



//! \brief: UPC_C_Clear_SRAM
void UPC_C_Clear_SRAM( )
{
    int grpNum;
    int countNum;

    upc_c->counter_start = 0;
    upc_c->mode1_counter_control0_w1c = ~0ULL;
    upc_c->mode1_counter_control1_w1c = ~0ULL;
    upc_c->mode1_counter_control2_w1c = ~0ULL;

    // init mount 1 low resolution counts
    for (countNum = 0; countNum < UPC_C_DCR__MODE1_COUNT_range; countNum++ ) {
        upc_c->mode1_count[countNum] = 0ULL;
    }

    // init UPC_P counter SRAM.
    for (grpNum = 0; grpNum < UPC_C_SRAM_NUM_GROUPS; grpNum++) {
        for (countNum = 0; countNum < 16; countNum++) {
            upc_c->data16.grp[grpNum].counter[countNum] = 0;
        }
    }
    // init I/O counter SRAM
    for (countNum = 0; countNum < UPC_C_DCR__IOSRAM_DATA_range; countNum++) {
        upc_c->iosram_data[countNum] = 0;
    }


}


#if 0
//! \brief: UPC_C_Clear_SRAM
void UPC_C_Clear_SRAM1( )
{
    ALIGN_QUADWORD uint64_t val[4] = { 0, 0, 0, 0 };
    int i;
    // init UPC_P counter SRAM.

    // load quadword with zero
    register uint64_t incr = 32;
    register uint64_t baseAddr = (uint64_t)(&val[0]) - incr ;
    register uint64_t index = 0;
    register double init;
    asm volatile ("qvlfdx %0, %1, %2" : "=f" (init) : "b" (baseAddr), "r" (index) );

    baseAddr = (uint64_t)(&(upc_c->data16.grp[0].counter[0])) - incr;
    for (i = 0; i < 1024/32; i++) {
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
    }
    // init I/O counter SRAM
    baseAddr = (uint64_t)((&upc_c->iosram_data[0])) - incr;
    for (i = 0; i < 64/32; i++) {
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
        asm volatile ("qvstfdux %1,%0,%3" : "=b" (baseAddr) : "f" (init), "0" (baseAddr), "r" (incr) : "memory" );
    }
}



//! \brief: UPC_C_Clear_SRAM - use accum to speed things up.
void UPC_C_Clear_SRAM2( )
{
    int grpNum;
    int countNum;
    // init 1st group of UPC_P counter SRAM.
    for (countNum = 0; countNum < 16; countNum++) {
        upc_c->data16.grp[0].counter[countNum] = 0;
    }
    // Use accum function to init rest of groups.
    UPC_C_DCR__CONFIG__ACCUM_RESET_insert(upc_c->c_config,0);    // make sure accum reset is clear
    for (grpNum = 1; grpNum < UPC_C_SRAM_NUM_GROUPS; grpNum++) {
        while (UPC_C_DCR__ACCUM_CONTROL__BUSY_get(upc_c->accum_control)) { }
        upc_c->accum_control = UPC_C_DCR__ACCUM_CONTROL__GO_set(1)
                                | UPC_C_DCR__ACCUM_CONTROL__WAIT_TIME_set(0)
                                | UPC_C_DCR__ACCUM_CONTROL__MASK_set(0xFFFFUL)
                                | UPC_C_DCR__ACCUM_CONTROL__SOURCE_A_set(0)
                                | UPC_C_DCR__ACCUM_CONTROL__SOURCE_B_set(0)
                                | UPC_C_DCR__ACCUM_CONTROL__DEST_set(grpNum) ;
    }
    // init I/O counter SRAM
    for (countNum = 0; countNum < UPC_C_IOSRAM_NUM_COUNTERS; countNum++) {
        upc_c->iosram_data[countNum] = 0;
    }
    while (UPC_C_DCR__ACCUM_CONTROL__BUSY_get(upc_c->accum_control)) { }
    if (UPC_C_DCR__ACCUM_CONTROL__BUSY_FAIL_get(upc_c->accum_control)) {
        UPC_FATAL_ERR("ERROR: UPC_C Accumulate function busy fail: accumreg=%016lx\n", upc_c->accum_control);
        //Terminate(1);       // terminate test
    }
}




//! \brief: UPC_C_Clear_SRAM - use accum to speed things up and overlap I/O SRAM and accum
void UPC_C_Clear_SRAM3( )
{
    int grpNum;
    int countNum;
    // init 1st group of UPC_P counter SRAM.
    for (countNum = 0; countNum < 16; countNum++) {
        upc_c->data16.grp[0].counter[countNum] = 0;
    }
    // Use accum function to init rest of groups.
    UPC_C_DCR__CONFIG__ACCUM_RESET_insert(upc_c->c_config,0);    // make sure accum reset is clear
    int ioCountNum = 0;
    for (grpNum = 1; grpNum < UPC_C_SRAM_NUM_GROUPS; grpNum++) {
        while (UPC_C_DCR__ACCUM_CONTROL__BUSY_get(upc_c->accum_control)) {
            if (ioCountNum < UPC_C_IOSRAM_NUM_COUNTERS) upc_c->iosram_data[ioCountNum++] = 0;
        }
        upc_c->accum_control = UPC_C_DCR__ACCUM_CONTROL__GO_set(1)
                                | UPC_C_DCR__ACCUM_CONTROL__WAIT_TIME_set(0)
                                | UPC_C_DCR__ACCUM_CONTROL__MASK_set(0xFFFFUL)
                                | UPC_C_DCR__ACCUM_CONTROL__SOURCE_A_set(0)
                                | UPC_C_DCR__ACCUM_CONTROL__SOURCE_B_set(0)
                                | UPC_C_DCR__ACCUM_CONTROL__DEST_set(grpNum) ;
    }
    // init I/O counter SRAM
    while (ioCountNum < UPC_C_IOSRAM_NUM_COUNTERS) {
        upc_c->iosram_data[ioCountNum++] = 0;
    }
    while (UPC_C_DCR__ACCUM_CONTROL__BUSY_get(upc_c->accum_control)) { }
    if (UPC_C_DCR__ACCUM_CONTROL__BUSY_FAIL_get(upc_c->accum_control)) {
        UPC_FATAL_ERR("ERROR: UPC_C Accumulate function busy fail: accumreg=%016lx\n", upc_c->accum_control);
        //Terminate(1);       // terminate test
    }
}

#endif



//! \brief: UPC_C_Clear
void UPC_C_Clear( )
{
#ifdef __HWSTUBS__
    memset(dummyUPC_C_Array, 0, 0x5000);
#endif

    upc_c->c_config = 0ULL;
    upc_c->counter_start = 0ULL;

    // clear interrupts
    upc_c->ccg_int_en           = 0ULL; // UPC_C_DCR__CCG_INT_EN__IE_set(0);
    upc_c->ccg_int_status_w1c   = ~0ULL; // UPC_C_DCR__CCG_INT_STATUS_W1C__IP_set(ALLONES);
    upc_c->io_ccg_int_en           = 0ULL; // UPC_C_DCR__CCG_INT_EN__IE_set(0);
    upc_c->io_ccg_int_status_w1c   = ~0ULL; // UPC_C_DCR__CCG_INT_STATUS_W1C__IP_set(ALLONES);

    // clear mode1 settings
    upc_c->mode1_ec_config           = 0ULL; // UPC_C_DCR__MODE1_EC_CONFIG__MODE_set(0);
    upc_c->mode1_counter_control0_rw = 0ULL;
    upc_c->mode1_counter_control1_rw = 0ULL;
    upc_c->mode1_counter_control2_rw = 0ULL;

    // UPC_C indicates if an overflow interrupt has occurred.
    // don't clear control regs
    upc_c->overflow_state__state      = ~0ULL;

    // clear accum settings
    upc_c->accum_control        = 0ULL;

    // clear trace settings
    upc_c->m2_trigger           = 0ULL;

}



#if __KERNEL__ || __HWSTUBS__

//! \brief: UPC_C_Full_Clear
void UPC_C_Full_Clear( )
{
    upc_c->c_config = 0ULL;
    upc_c->counter_start = 0ULL;

    // clear any error and error interrupt settings
    // I think these force bits are one shot, but doesn't hurt to
    // write 0's anyway.
    DCRWritePriv( UPC_C_DCR(ERROR_FORCE), 0ULL);
    DCRWritePriv( UPC_C_DCR(UPC_C_INTERRUPT_STATE__FORCE), 0ULL);
    DCRWritePriv( UPC_C_DCR(INTERRUPT_INTERNAL_ERROR__FORCE), 0ULL);
    DCRWritePriv( UPC_C_DCR(UPC_C_OVERFLOW_STATE__FORCE), 0ULL);

    // make sure SRAM, I/O SRAM, and M1 Regs are clear
    UPC_C_Clear_SRAM();

    UPC_C_Clear();

    // UPC_C Status for hardware and software type internal errors
    DCRWritePriv( UPC_C_DCR(INTERRUPT_INTERNAL_ERROR__STATE), ~0ULL);
    //DCRWritePriv( UPC_C_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 0ULL);

    // clear interrupt sources.
    // Don't clear the control registers - set by firmware
    DCRWritePriv( UPC_C_DCR(UPC_C_INTERRUPT_STATE__STATE), ~0ULL);        // clear any pending interrupts.
    //DCRWritePriv( UPC_C_DCR(INTERRUPT_STATE_CONTROL_LOW), 0ULL);        // clear interrupt delivery settings.
    //DCRWritePriv( UPC_C_DCR(INTERRUPT_STATE_CONTROL_HIGH), 0ULL);

    // DCRWritePriv( UPC_C_DCR(UPC_SRAM_PARITY_INFO), 0ULL);    can't write "interrupt info" registers any more

    mbar();
}




//! \brief: UPC_C_Init_Mode
void UPC_C_Init_Mode( int upcMode, UPC_Ctr_Mode_t ctrMode, short unit)
{
    //UPC_C_Clear();

    // Don't init the UPC Interrupt control registers??  - should be owned by the firmware check initialization code instead
#ifdef __KERNEL__
    DCRWritePriv( UPC_C_DCR(UPC_C_INTERRUPT_STATE_CONTROL_LOW), UPC_C__INTERRUPT_STATE_CONTROL_LOW__DEFAULT);
    DCRWritePriv( UPC_C_DCR(UPC_C_INTERRUPT_STATE_CONTROL_HIGH), UPC_C__INTERRUPT_STATE_CONTROL_HIGH__DEFAULT);
    DCRWritePriv( UPC_C_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), UPC_C__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__DEFAULT);
    DCRWritePriv( UPC_C_DCR(UPC_C_OVERFLOW_STATE_CONTROL_LOW), UPC_C__OVERFLOW_STATE_CONTROL_LOW__DEFAULT);
    DCRWritePriv( UPC_C_DCR(UPC_C_OVERFLOW_STATE_CONTROL_HIGH), UPC_C__OVERFLOW_STATE_CONTROL_HIGH__DEFAULT);
#endif

    upc_c->c_config = ( UPC_C_DCR__CONFIG__MODE_set(upcMode) |
                        UPC_C_DCR__CONFIG__UPC_C_ENABLE_set(1) |
                        UPC_C_DCR__CONFIG__L2_COMBINE_set(1)
                       );
    // leave accum_reset bit alone - should remain 0.

    UpciBool_t indepStart = ((ctrMode==UPC_CM_INDEP) || (ctrMode==UPC_CM_DISCON));

    // if mode not in distributed mode - all other units on central ring must be disabled.
    if (ctrMode != UPC_CM_NO_INIT) {
        unsigned i;

        unsigned numCores = CONFIG_MAX_CORES;

        unsigned targCore = (unit >= 0) ? unit : Kernel_PhysicalProcessorID();

        if (upcMode == UPC_DISTRIB_MODE) {
            unsigned i;
            for (i=0; i<numCores; i++) {
                UPC_P_Init_Unit_Mode(i, upcMode, ctrMode);
            }
            UPC_L2_EnableUPC(indepStart, UpciTrue);
        }

        else {
            for (i=0; i<numCores; i++) {
                if (i != targCore) {
                    UPC_P_Disable_Unit(i);
                }
                else {
                    UPC_P_Init_Unit_Mode(i, upcMode, UPC_CM_SYNC);
                }
            }
            UPC_L2_DisableUPC();
        }

        Personality_t personality;
        Kernel_GetPersonality(&personality, sizeof(Personality_t));
        uint64_t nodeConfig = personality.Kernel_Config.NodeConfig;

        UPC_IO_EnableUPC(indepStart, nodeConfig);
    }

}



//! \brief: UPC_C_Init_Mode
void UPC_C_Disable( UPC_Ctr_Mode_t ctrMode)
{

    upc_c->c_config = UPC_C_DCR__CONFIG__SRAM_DEEPSLEEP_set(1);

    // Disable other units
    if (ctrMode != UPC_CM_NO_INIT) {
        unsigned i;        // BGQ_GetNumThreads()
        unsigned numCores = CONFIG_MAX_CORES;
        for (i=0; i<numCores; i++) {
            UPC_P_Disable_Unit(i);
        }
        UPC_L2_DisableUPC();

        Personality_t personality;
        Kernel_GetPersonality(&personality, sizeof(Personality_t));
        uint64_t nodeConfig = personality.Kernel_Config.NodeConfig;

        UPC_IO_DisableUPC(nodeConfig);
    }
}



#endif


//! \brief Parse_SRAM_Trace_Cycle
//! Parse given SRAM trace cycle record into 6 uint64_t records starting at address
//! @param[in,out]  buff         pointer within trace buffer
//! @param[in]      sram_cycle   SRAM offset to parse
void Parse_SRAM_Trace_Cycle(uint64_t *buff, short sram_cycle)
{
    uint64_t tmp1,tmp2,tmp3,tmp4,tmp5;
    tmp5 = upc_c->psram_mode2_data[sram_cycle];
    short dIndex = sram_cycle * 4;      // 4 long word per cycle
    tmp1 = upc_c->datam2.data[dIndex];
    tmp2 = upc_c->datam2.data[dIndex+1];
    tmp3 = upc_c->datam2.data[dIndex+2];
    tmp4 = upc_c->datam2.data[dIndex+3];

    //            Trigger 0                     Data 0
    buff[0] = (_BGQ_GET(4,19,tmp2) << 44) | _BGQ_GET(44,43,tmp1);
    //            Trigger 1                     Data 1 A                     Data 1 B                    Data 1 C
    buff[1] = (_BGQ_GET(4,23,tmp2) << 44) | (_BGQ_GET(20,63,tmp1)<<24) | (_BGQ_GET(8,39,tmp5)<<16) | _BGQ_GET(16,15,tmp2);
    //            Trigger 2                     Data 2 A                     Data 2 B                    Data 2 C
    buff[2] = (_BGQ_GET(4,43,tmp3) << 44) | (_BGQ_GET(40,63,tmp2)<<4) |  _BGQ_GET(4,43,tmp5);
    //            Trigger 3                     Data 3 A                     Data 3 B
    buff[3] = (_BGQ_GET(4,47,tmp3) << 44) | (_BGQ_GET(4,47,tmp5)<<40) | _BGQ_GET(40,39,tmp3);
    //            Trigger 4                     Data 4 A                     Data 4 B                  Data 4 C
    buff[4] = (_BGQ_GET(4,59,tmp5) << 44) | (_BGQ_GET(16,63,tmp3)<<28) | (_BGQ_GET(8,55,tmp5)<<20) | _BGQ_GET(20,19,tmp4);
    //            Trigger 5                     Data 5
    buff[5] = (_BGQ_GET(4,63,tmp5) << 44) | _BGQ_GET(44,63,tmp4);

    //PRINTF_XXXXXX("trc cycle=%04d; 0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx\n", sram_cycle, tmp1, tmp2, tmp3, tmp4, tmp5);
    //PRINTF_XXXXXX("buf             0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx  0x%016lx\n", buff[0], buff[1], buff[2], buff[3], buff[4], buff[5]);


}



//! \brief: UPC_C_Extract_M2_Trace_Records
void UPC_C_Extract_M2_Trace_Records(upc_trace_buff_t *tbuff)
{
    short trigger_sram_cycle = UPC_C_DCR__STATUS__MODE2_TRIGGER_ADDR_get(upc_c->status);
    short capture_sram_cycles = UPC_C_DCR__CONFIG__MODE2_CAPTURE_CYCLES_get(upc_c->c_config); // excluding trigger cycle

    short start_sram_cycle = (trigger_sram_cycle + capture_sram_cycles + 1) % (UPC_MAX_TRACE_RECS/6);
    tbuff->trigger_rec = (UPC_MAX_TRACE_RECS/6 - capture_sram_cycles - 1) * 6;

    // foreach SRAM trace record
    short cur_sram_cycle = start_sram_cycle;
    short cur_rec = 0;
    do {
        Parse_SRAM_Trace_Cycle(&(tbuff->rec[cur_rec]), cur_sram_cycle);
        if (++cur_sram_cycle >= (UPC_MAX_TRACE_RECS/6)) cur_sram_cycle = 0;
        cur_rec += 6;
    } while (cur_sram_cycle != start_sram_cycle);
}



//! \brief: UPC_C_Start_M2_Capture
void UPC_C_Start_M2_Capture(int final_x1_cycles, UpciBool_t int_when_done)
{
    unsigned m2_cycles = (final_x1_cycles + 5) / 6;
    if (m2_cycles > 254) {
        UPC_FATAL_ERR("Argument final_x1_cycles (%d) must have value of 1524 or less\n", final_x1_cycles);
    }
    upc_c->c_config = ( UPC_C_DCR__CONFIG__MODE_set(2) |
                            UPC_C_DCR__CONFIG__UPC_C_ENABLE_set(1) |
                            UPC_C_DCR__CONFIG__MODE2_RUN_set(1) |
                            UPC_C_DCR__CONFIG__MODE2_CAPTURE_CYCLES_set(m2_cycles) |
                            UPC_C_DCR__CONFIG__MODE2_UPC_P_TRIG_EN_set(1)
                         );

    if (int_when_done) {
        UPC_C_DCR__UPC_C_OVERFLOW_STATE_CONTROL_LOW__MODE2_DONE_insert(upc_c->overflow_state_control_low, UPC_C_INT_CONTROL_NONCRIT);
    }
    else {
        UPC_C_DCR__UPC_C_OVERFLOW_STATE_CONTROL_LOW__MODE2_DONE_insert(upc_c->overflow_state_control_low, UPC_C_INT_CONTROL_MASKED);
    }
}


