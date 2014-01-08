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
#include "server/Connection.h"

#include "common/ConnectionContainer.h"
#include "common/logging.h"

#include "server/Options.h"
#include "server/Server.h"

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

Connection::Connection(
        const bgq::utility::portConfig::SocketPtr& socket,
        const Server::Ptr& server
        ) :
    runjob::Connection( socket ),
    _options( server->getOptions() ),
    _server( server)
{
    BOOST_ASSERT( socket );
    BOOST_ASSERT( server );
}

Connection::~Connection()
{
    // remove ourself from container
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getConnections()->remove( _socket );
}

void
Connection::resolveComplete() 
{
    // add ourself to container
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    server->getConnections()->add( this->getShared() );

    this->addComplete();
}

void
Connection::addComplete()
{
    this->readHeader();
}

} // server
} // runjob
