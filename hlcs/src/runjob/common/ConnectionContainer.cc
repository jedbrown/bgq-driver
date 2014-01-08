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
#include "common/ConnectionContainer.h"

#include "common/logging.h"

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

ConnectionContainer::ConnectionContainer(
        boost::asio::io_service& io_service
        ) :
    _strand( io_service ),
    _connections()
{

}

void
ConnectionContainer::add(
        const Connection::Ptr& connection
        )
{
    _strand.post(
            boost::bind(
                &ConnectionContainer::addImpl,
                shared_from_this(),
                connection
                )
            );
}

void
ConnectionContainer::addImpl(
        const Connection::Ptr& connection
        )
{
    // add
    bool result = 
        _connections.insert(
            Container::value_type(
                connection->getFileDescriptor(),
                connection
                )
            ).second;

    // ensure connection was not already there, if it is we have a bug
    if ( !result ) {
        LOG_FATAL_MSG( "connection " << connection->getFileDescriptor() << " is already in container" );

        // log contents of container for triage purposes
        BOOST_FOREACH( const Container::value_type& i, _connections ) {
            LOG_FATAL_MSG( i.first );
            Connection::Ptr connection = i.second.lock();
            if ( connection ) {
                LOG_FATAL_MSG( connection->getEndpoint() );
            } else {
                LOG_FATAL_MSG( "no connection" );
            }
        }

        // punt
        BOOST_ASSERT( !result );
    }

    LOG_TRACE_MSG( "added connection " << connection->getFileDescriptor() << " " << _connections.size() << " connections" );
}

void
ConnectionContainer::remove(
        const bgq::utility::portConfig::SocketPtr& socket
        )
{
    _strand.post(
            boost::bind(
                &ConnectionContainer::removeImpl,
                shared_from_this(),
                socket
                )
            );
}

void
ConnectionContainer::removeImpl(
        const bgq::utility::portConfig::SocketPtr& socket
        )
{
    Container::iterator result = _connections.find( socket->lowest_layer().native() );
    if ( result != _connections.end() ) {
        _connections.erase( result );
        LOG_TRACE_MSG( "removed connection " << socket->lowest_layer().native() );
    }
}

void
ConnectionContainer::get(
        const GetHandler& handler
        )
{
    _strand.post(
            boost::bind(
                &ConnectionContainer::getImpl,
                shared_from_this(),
                handler
                )
            );
}

void
ConnectionContainer::getImpl(
        const GetHandler& handler
        )
{
    handler( _connections );
}

} // runjob
