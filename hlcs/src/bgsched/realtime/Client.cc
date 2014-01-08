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
#include "ClientImpl.h"

#include <bgsched/realtime/Client.h>

#include <utility/include/Log.h>

#include <log4cxx/mdc.h>

using namespace std;

const string _API_MDC_PRE_STR("Client::");

#define LOG_DECLARE_API_MDC \
    log4cxx::MDC _mdc( "bg_api", _API_MDC_PRE_STR + __FUNCTION__ + "()" );

LOG_DECLARE_FILE( "bgsched" );

namespace bgsched {
namespace realtime {

Client::Client( const ClientConfiguration& client_configuration )
{
    LOG_DECLARE_API_MDC;
    _impl_ptr.reset( new Impl( client_configuration ) );
}


void Client::addListener( ClientEventListener& l )
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->addListener( l );
}


void Client::removeListener( ClientEventListener& l )
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->removeListener( l );
}


void Client::setBlocking( bool blocking )
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->setBlocking( blocking );
}


bool Client::isBlocking() const
{
    LOG_DECLARE_API_MDC;
    return _impl_ptr->isBlocking();
}


void Client::connect()
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->connect();
}


void Client::disconnect()
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->disconnect();
}


void Client::requestUpdates(
        bool *interrupted_out
    )
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->requestUpdates( interrupted_out );
}


void Client::setFilter(
        const Filter& filter,
        Filter::Id* filter_id_out,
        bool* interrupted_out
    )
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->setFilter( *filter._impl_ptr, filter_id_out, interrupted_out );
}


int Client::getPollDescriptor()
{
    LOG_DECLARE_API_MDC;
    return _impl_ptr->getPollDescriptor();
}


void Client::sendMessages(
        bool* interrupted_out,
        bool* again_out
    )
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->sendMessages( interrupted_out, again_out );
}


void Client::receiveMessages(
        bool* interrupted_out,
        bool* again_out,
        bool* end_out
    )
{
    LOG_DECLARE_API_MDC;
    _impl_ptr->receiveMessages( interrupted_out, again_out, end_out );
}


Client::~Client()
{
    LOG_DECLARE_API_MDC;
    LOG_DEBUG_MSG( "Destroyed a real-time client." );
}

} // namespace bgsched::realtime
} // namespace bgsched
