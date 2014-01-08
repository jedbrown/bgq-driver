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

#include "utility.h"

#include <string>

#include <errno.h>

using std::string;

namespace mmcs_client {

static void sendCommand(
        ConsolePortClient& client,
        const string& cmdString
    )
{
    string cmdStringCopy(cmdString);

    while ( true ) {
        try {
            client.sendMessage( cmdStringCopy );
        } catch ( ConsolePort::Error &e ) {
            switch ( e.errcode ) {
                case EINTR:
                case EAGAIN:
                    continue;
                default:
                    throw;
            }
        }
        break;
    }
}


static void receiveReply(
        ConsolePortClient& client,
        CommandReply& reply
    )
{
    // loop receiving command output until end of reply is received
    string replyString;
    bool   nullTerm = false;
    bool   eom = false;
    reply.reset();

    do {
        try {
            nullTerm = client.receiveMessage( replyString ); // nullTerm == true if '\0' received
        } catch( ConsolePort::Error &e ) {
            switch (e.errcode) {
                case EINTR:
                case EAGAIN:
                    continue;
                default:
                    throw;
            }

        }

        if (replyString.length() == 0) {
            reply << ABORT << "Internal error: null reply from server" << DONE;
            return;
        }

        // reconstruct the MMCSCommandReply from the server response
        if ( reply.getStatus() == CommandReply::STATUS_NOT_SET ) {
            reply.assign( replyString );
        } else {
            reply.append( replyString );
        }

        replyString.clear();
        // exit loop if a null terminator was received
        eom = nullTerm;

    } while (!eom);

    if ( ! reply.isDone() ) {
        reply << DONE;
    }
}


void sendCommandReceiveReply(
        ConsolePortClient& client,
        const std::string& cmdString,
        CommandReply& reply
    )
{
    sendCommand( client, cmdString );
    receiveReply( client, reply );
}

} // namespace mmcs_client
