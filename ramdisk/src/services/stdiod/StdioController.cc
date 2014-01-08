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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  StdioController.cc
//! \brief Methods for bgcios::stdio::StdioController class.

// Includes
#include "StdioController.h"
#include <ramdisk/include/services/StdioMessages.h>
#include <ramdisk/include/services/common/logging.h>
#include <utility/include/version.h>

using namespace bgcios::stdio;

LOG_DECLARE_FILE("cios.stdiod");


StdioController::~StdioController()
{
   _dataChannel.reset();
}

int
#ifdef LOG_CIOS_INFO_DISABLE
StdioController::startup(in_port_t)
#else
StdioController::startup(in_port_t dataChannelPort)
#endif
{
   LOG_CIOS_INFO_MSG("data channel listener on port" << dataChannelPort << " was ignored");
   return 0;
}

