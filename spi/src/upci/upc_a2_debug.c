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

//! \addtogroup upci_spi_test
//@{
/**
 * \file
 *
 * \brief Debug implementation for dumping A2 Performance Counting state
 *
 */
//@}



#define UPC_A2_DEBUG_C


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "spi/include/upci/testlib/printf_repl.h"
#include "spi/include/upci/upc_a2_pc.h"
#include "spi/include/kernel/upci.h"
#include "us_debug.h"

#ifdef __KERNEL
#include "spi/include/upci/upc_scom.h"
#include "spi/include/upci/testlib/upc_a2_debug.h"
#endif



//! \brief: Upci_A2PC_DumpRegs
void Upci_A2PC_DumpRegs_Indent(Upci_A2PC_Val_t *pA2pcVal, int indent)
{
    INIT_INDENT();
    PRINTF( "%s-----------------------------------------\n"
            "%sA2 PC SPRs:\n", indentStrg, indentStrg);

    PRINTF_X(  "%scesr: 0x%08x\n", indentStrg,  pA2pcVal->cesr );
    PRINTF_X(  "%saesr: 0x%08x\n", indentStrg,  pA2pcVal->aesr );
    PRINTF_XX( "%siesr: 0x%08x  0x%08x\n", indentStrg,  pA2pcVal->iesr1, pA2pcVal->iesr2 );
    PRINTF_XX( "%sxesr: 0x%08x  0x%08x\n", indentStrg,  pA2pcVal->xesr1, pA2pcVal->xesr2 );
    PRINTF_XX( "%smesr: 0x%08x  0x%08x\n", indentStrg,  pA2pcVal->mesr1, pA2pcVal->mesr2 );
    PRINTF_XX( "%slusr: 0x%08x  0x%08x\n", indentStrg,  pA2pcVal->xesr3, pA2pcVal->xesr4 );
}



#ifdef __KERNEL__
//! \brief: UPC_A2_Dump_Event_State
//! printf to stdout a state summary of the A2 Event Config registers.
void UPC_A2_Dump_Event_State()
{
    PRINTF( "-----------------------------------------\n"
            "A2 Event State:\n");
    uint64_t val1;
    uint64_t val2;

    val1 = mfspr(SPRN_CESR);
    PRINTF_X( "cesr: 0x%016lx\n",  val1 );

    val1 = mfspr(SPRN_AESR);
    PRINTF_X( "aesr: 0x%016lx\n",  val1 );

    val1 = mfspr(SPRN_IESR1);
    val2 = mfspr(SPRN_IESR2);
    PRINTF_XX( "iesr: 0x%016lx  0x%016lx\n",  val1, val2 );

    val1 = mfspr(SPRN_XESR1);
    val2 = mfspr(SPRN_XESR2);
    PRINTF_XX( "xesr: 0x%016lx  0x%016lx\n",  val1, val2 );

    val1 = mfspr(SPRN_MESR1);
    val2 = mfspr(SPRN_MESR2);
    PRINTF_XX( "mesr: 0x%016lx  0x%016lx\n",  val1, val2 );

    val1 = mfspr(SPRN_XESR3);
    val2 = mfspr(SPRN_XESR4);
    PRINTF_XX( "lusr: 0x%016lx  0x%016lx\n",  val1, val2 );

}



//! \brief: UPC_A2_Dump_Debug_State
//! printf to stdout a state summary of the A2 Debug Select registers (uses slow SCOM interface)
void UPC_A2_Dump_Debug_State()
{
    PRINTF( "-----------------------------------------\n"
            "A2 Debug Select Reg State:\n");

    const Upci_Node_Parms_t *pNodeParms = Upci_GetNodeParms();
    if (pNodeParms->dd1Enabled) {
        PRINTF( "Disabled for DD1\n");
    }
    else {

        short core = PhysicalProcessorID();
        uint64_t val;

        uint64_t saveto = UPC_Init_Scom();

        val = UPC_Scom_Read(core, A2_SCOM_PCCR0);
        PRINTF_X( "pccr0: 0x%016lx\n",  val );

        val = UPC_Scom_Read(core, A2_SCOM_ABDSR);
        PRINTF_X( "abdsr: 0x%016lx\n",  val );

        val = UPC_Scom_Read(core, A2_SCOM_IDSR);
        PRINTF_X( "idsr:  0x%016lx\n",  val );

        val = UPC_Scom_Read(core, A2_SCOM_MPDSR);
        PRINTF_X( "mpdsr: 0x%016lx\n",  val );

        val = UPC_Scom_Read(core, A2_SCOM_XDSR1);
        PRINTF_X( "xdsr1: 0x%016lx\n",  val );

        val = UPC_Scom_Read(core, A2_SCOM_XDSR2);
        PRINTF_X( "xdsr2: 0x%016lx\n",  val );

        UPC_Restore_Timeout(saveto);
    }
}



//! \brief: UPC_A2_Dump_State
//! printf to stdout a state summary of the A2 Config registers.
void UPC_A2_Dump_State()
{
    UPC_A2_Dump_Event_State();
    if ( !(fwext_getPersonality()->Kernel_Config.NodeConfig & PERS_ENABLE_FPGA) ) { // we are not on FPGA
        PRINTF("\n");
        UPC_A2_Dump_Debug_State();
    }
}

#endif

