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


#include "ClientFilter.h"

#include "utility/include/Log.h"


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {


ClientFilter::ClientFilter()
    : _cfv( bgsched::realtime::Filter::Impl::DEFAULT )
{
    // Nothing to do.
}


void ClientFilter::reset()
{
    _cfv = ClientFilterVisitor( bgsched::realtime::Filter::Impl::DEFAULT );
}


void ClientFilter::set( const bgsched::realtime::Filter::Impl& filter )
{
    _cfv = ClientFilterVisitor( filter );
}


bool ClientFilter::check( bgsched::realtime::AbstractDatabaseChange &change )
{
    change.accept( _cfv );

    return _cfv.getRes();
}


} } // namespace realtime::server
