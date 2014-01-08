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

#ifndef MMCS_CLIENT_UTILITY_H_
#define MMCS_CLIENT_UTILITY_H_

#include "CommandReply.h"
#include "ConsolePort.h"

#include <string>

namespace mmcs_client {

/*!
 *  \throw ConsolePort::Error Failed to send the message or receive the reply because lost connection to server.
 */
void sendCommandReceiveReply(
        ConsolePortClient& client,
        const std::string& cmdString,
        CommandReply& reply
    );

} // namespace mmcs_client

#endif
