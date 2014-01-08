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

#include "common/Thread.h"

namespace mmcs {
namespace common {

class ConsoleController;

} // common

namespace console {

/*!
 * \brief polling the mmcs connection
 */
class ConnectionMonitor : public common::Thread
{
public:
    ConnectionMonitor(
            common::ConsoleController* p
            );

private:
    void* threadStart();

private:
    common::ConsoleController* const _controller;
};

} } // namespace mmcs::console

#endif
