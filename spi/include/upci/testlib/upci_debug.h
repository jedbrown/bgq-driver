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

#ifndef UPCI_DEBUG_H
#define UPCI_DEBUG_H

//! \addtogroup upci_spi
//@{
/**
 * \file
 *
 * \brief UPCI debug functions
 *
 */
//@}


#include "../upci.h"
#include "printf_repl.h"
#include "upc_a2_debug.h"
#include "upc_c_debug.h"
#include "upc_p_debug.h"


void Upci_DumpKDebug(unsigned indent);

#endif
