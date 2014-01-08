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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef MMCS_CONSOLE_CONNECTION_MONITOR_H_
#define MMCS_CONSOLE_CONNECTION_MONITOR_H_


#include "common/ConsoleController.h"
#include "common/Thread.h"


namespace mmcs {
namespace console {


//******************************************************************************
// Class for polling the mmcs connection
//******************************************************************************
class ConnectionMonitor : public common::Thread
{
    common::ConsoleController* _controller;
public:
    ConnectionMonitor() {};
    void setConsole(common::ConsoleController* p) { _controller = p; }
    void* threadStart();
};

} } // namespace mmcs::console

#endif
