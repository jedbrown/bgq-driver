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

//! \file  JobController.cc
//! \brief Methods for bgcios::jobctl::JobController class.

// Includes
#include "JobController.h"
#include <ramdisk/include/services/JobctlMessages.h>
#include <ramdisk/include/services/common/logging.h>

using namespace bgcios::jobctl;

LOG_DECLARE_FILE("cios.jobctld");


JobController::~JobController()
{
   _dataChannel.reset();
}

int
#ifdef LOG_CIOS_INFO_DISABLE
JobController::startup(in_port_t)
#else
JobController::startup(in_port_t dataChannelPort)
#endif
{
   LOG_CIOS_INFO_MSG("data channel listener on port " << dataChannelPort << " was ignored");
   return 0;
}

