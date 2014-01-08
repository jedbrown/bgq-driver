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

#ifndef UPC_A2_DEBUG_H
#define UPC_A2_DEBUG_H

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief A2 Perf Counting debug functions
 */
//@}


#include <hwi/include/common/compiler_support.h>

#include "spi/include/kernel/upci.h"
#include "spi/include/upci/upc_a2.h"
#include "printf_repl.h"

__BEGIN_DECLS



//! \brief: UPC_A2_Dump_State
//! printf to stdout a state summary of the A2 Event and Debug Config registers.
void UPC_A2_Dump_State();



//! \brief: UPC_A2_Dump_Event_State
//! printf to stdout a state summary of the A2 Event Config registers.
void UPC_A2_Dump_Event_State();



//! \brief: UPC_A2_Dump_Debug_State
//! printf to stdout a state summary of the A2 Debug Select registers (uses slow SCOM interface)
void UPC_A2_Dump_Debug_State();



//! \brief: UPC_A2_Dump_SPRs
//! printf to stdout the contents of the current threads UPC A2 SPRs.
//! Also available from user state.
__INLINE__ void UPC_A2_Dump_SPRs()
{
    Upci_A2PC_Val_t a2qry;
    Kernel_Upci_A2PC_GetRegs(&a2qry);
    Upci_A2PC_DumpRegs(&a2qry);
}



__END_DECLS


#endif
