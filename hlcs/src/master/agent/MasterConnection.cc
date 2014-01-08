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

#include "MasterConnection.h"
#include "Agent.h"

LOG_DECLARE_FILE( "master" );

void
MasterConnection::create(
        boost::asio::io_service& io_service,
        const bgq::utility::PortConfiguration::Pairs& ports,
        Agent* const agent
        )
{
    const Ptr result(
            new MasterConnection(
                io_service,
                ports,
                agent
                )
            );

    io_service.post(
            boost::bind(
                &MasterConnection::impl,
                result,
                result->_ports.begin(),
                0 // number of attempts so far
                )
            );
}

MasterConnection::MasterConnection(
        boost::asio::io_service& io_service,
        const bgq::utility::PortConfiguration::Pairs& ports,
        Agent* const agent
        ) :
    _ports( ports ),
    _agent( agent ),
    _master( io_service ),
    _timer( io_service )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    BOOST_ASSERT( _agent );
    BOOST_ASSERT( !_ports.empty() );
}

MasterConnection::~MasterConnection()
{
    LOG_DEBUG_MSG( "Terminating" );
}

void
MasterConnection::impl(
        const bgq::utility::PortConfiguration::Pairs::const_iterator& port,
        const unsigned attempts
        )
{
    LOG_TRACE_MSG( __FUNCTION__ )
    try {
        if ( !attempts ) {
            LOG_INFO_MSG("Attempting to connect on " << port->first << ":" << port->second);
        }
        const int error = _agent->join( *port );
        if ( error ) {
            _timer.expires_from_now( boost::posix_time::seconds(1) );
            _timer.async_wait(
                    boost::bind(
                        &MasterConnection::handleWait,
                        shared_from_this(),
                        _1,
                        port,
                        attempts
                        )
                    );
            return;
        }

        _master.assign(
                dup( _agent->_prot->getResponder()->getFileDescriptor() ) // stream descriptor assumes ownership
                );
        this->read();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

void
MasterConnection::read()
{
    LOG_TRACE_MSG( __FUNCTION__ );
    _master.async_read_some(
            boost::asio::null_buffers(),
            boost::bind(
                &MasterConnection::handleRead,
                shared_from_this(),
                boost::asio::placeholders::error
                )
            );
}

void
MasterConnection::handleRead(
        const boost::system::error_code& error
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    if ( error ) {
        LOG_WARN_MSG( __FUNCTION__ << " " << boost::system::system_error(error).what() );
        this->impl( _ports.begin(), 0 );
        return;
    }

    _agent->processRequest();
    if ( !_agent->_prot->getResponder() ) {
        _master.close();
        this->impl( _ports.begin(), 0 );
    } else {
        this->read();
    }
}

void
MasterConnection::handleWait(
        const boost::system::error_code& error,
        bgq::utility::PortConfiguration::Pairs::const_iterator port,
        unsigned attempts
        )
{
    LOG_TRACE_MSG( __FUNCTION__ << " " << boost::system::system_error(error).what() );
    if ( error == boost::asio::error::operation_aborted ) {
        // asked to stop
        return;
    } else if ( error ) {
        LOG_WARN_MSG( "Could not wait: " << boost::system::system_error(error).what() );
        return;
    }

    static const unsigned MaxAttempts = 30;
    if ( ++attempts > MaxAttempts ) {
        // try next port
        LOG_WARN_MSG( "Connection to bgmaster_server failed on " << port->first << ":" << port->second );
        ++port;
        attempts = 0;
    }

    if ( port == _ports.end() ) {
        this->impl( _ports.begin(), attempts );
    } else {
        this->impl( port, attempts );
    }
}
