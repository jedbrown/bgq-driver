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

#ifndef UPC_C_DEBUG_H
#define UPC_C_DEBUG_H

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief UPC_C debug functions
 *
 */
//@}


#include <hwi/include/common/compiler_support.h>

#include "spi/include/kernel/upci.h"
#include "spi/include/upci/upc_c.h"
#include "printf_repl.h"

__BEGIN_DECLS



//! \brief: UPC_C_Dump_State
//! printf to stdout a state summary of the UPC_C - all registers (does not include SRAM counters)
void UPC_C_Dump_State();


//! \brief: UPC_L2_Dump_State;
//! printf to stdout a state summary of the UPC portion of the L2 units
void UPC_L2_Dump_State();


//! \brief: UPC_C_Dump_Counters
//! printf to stdout a state summary UPC_C counters by counter group (16 counters)
//! @param[in]  first_group_num  0 - 63
//! @param[in]  last_group_num   0 - 63  (or -1 for only one group)
void UPC_C_Dump_Counters(int first_group_num, int last_group_num);



//! \brief: UPC_C_Dump_M1P_Counters
//! printf the UPC_P counters in upc_c mode 1
//! @param[in]  first_thread  0 - 3
//! @param[in]  last_thread   0 - 3  (or -1 for only one thread)
void UPC_C_Dump_M1P_Counters(int first_thread, int last_thread);



//! \brief: UPC_C_Dump_M1_Counters
//! printf to stdout a state summary UPC_C counters by M1 counter group (8 counters)
//! @param[in]  first_group_num  0 - 11
//! @param[in]  last_group_num   0 - 11  (or -1 for only one group)
void UPC_C_Dump_M1_Counters(int first_group_num, int last_group_num);



//! \brief: UPC_C_Dump_IO_Counters
//! printf to stdout a state summary UPC_C IO Counters by counter number (64 counters)
//! @param[in]  first_count  0 - 63
//! @param[in]  last_count  0 - 63  (or -1 for all)
void UPC_C_Dump_IO_Counters(int first_count, int last_count);



//! \brief: UPC_C_Dump_M2_Trace_Records
//! Print the raw trace records found in the given trace buffer.
//!
//! @param[in]  tbuff  pointer to trace buffer filled by UPC_C_Extract_M2_Trace_Records
//! @param[in]  start  start recrod to print (0-1535)
//! @param[in]  len    length to print (0-1536).  0 means print whole buffer.
//!
//! Requirements:  Assumes all 1536 records are valid.
void UPC_C_Dump_M2_Trace_Records(upc_trace_buff_t *tbuff, short start, short len);




__END_DECLS


#endif
