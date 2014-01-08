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
#include "mux/client/Server.h"

#include "mux/client/Id.h"

#include "mux/server/Connection.h"

#include "common/Message.h"

namespace runjob {
namespace mux {
namespace client {

Server::Server(
        const server::Connection::Ptr& server,
        const Id& id
        ) :
    _server( server ),
    _id( id )
{

}

void
Server::send(
        const runjob::Message::Ptr& msg,
        const uint64_t job,
        const SendHandler& handler
        )
{
    // set client and job ID
    msg->setClientId( _id );
    msg->setJobId( job );

    _server->write(
            msg,
            handler
            );
}

} // client
} // mux
} // runjob
