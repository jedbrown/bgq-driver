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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef MMCS_SERVER_CONSOLE_MONITOR_PARAMETERS_H_
#define MMCS_SERVER_CONSOLE_MONITOR_PARAMETERS_H_

#include "../MMCSCommandProcessor.h"

#include "libmmcs_client/ConsolePort.h"

namespace mmcs {
namespace server {

/*!
 * \brief Parameters passed to MMCSConsoleListener and MMCSConsoleConnectionThread.
 */
class ConsoleMonitorParameters
{
public:
    /*!
     * \brief ctor.
     */
    ConsoleMonitorParameters(
            mmcs_client::ConsolePort* port,      //!< [in]
            MMCSCommandMap* map                  //!< [in]
            ) :
        consolePort(port),
        commandMap(map)
    {

    }

    mmcs_client::ConsolePort* consolePort; // console port or console connection
    MMCSCommandMap* commandMap;  // commands that can be executed
};

} } // namespace mmcs::server

#endif
