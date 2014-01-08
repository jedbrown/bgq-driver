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

#ifndef UPC_P_DEBUG_H
#define UPC_P_DEBUG_H

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief UPC_P debug functions
 *
 */
//@}


#include <hwi/include/common/compiler_support.h>

#include "spi/include/kernel/upci.h"
#include "spi/include/upci/upc_p.h"
#include "printf_repl.h"

__BEGIN_DECLS



//! \brief: UPC_P_Dump_State_Indent
//! printf to stdout a state summary of the UPC_P - all registers.
//! @param[in]  unit    The core index of the UPC_P unit to disable. A value < 0 means use local unit.
//! @param[in]  indent  left indent level (add spaces to each line of output)
void UPC_P_Dump_State_Indent( int unit, int indent );

//! \brief: UPC_P_Dump_State
//! printf to stdout a state summary of the UPC_P - all registers.
//! @param[in]  unit  The core index of the UPC_P unit to disable. A value < 0 means use local unit.
__INLINE__ void UPC_P_Dump_State( int unit ) { UPC_P_Dump_State_Indent( unit, 0 ); }




//! \brief: UPC_P_Dump_M2_Parms
//! Extract and print the UPC_P and A2 Debug registers and UPC_P context used in Mode2
//!
//! @param[in]  unit  unit number must be same as unit number from UPC_P_Init_Mode2_Parms
//!
void UPC_P_Dump_M2_Parms(int unit);



__END_DECLS


#endif
