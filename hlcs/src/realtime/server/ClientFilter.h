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


#ifndef REALTIME_SERVER_CLIENT_FILTER_H
#define REALTIME_SERVER_CLIENT_FILTER_H


#include "ClientFilterVisitor.h"

#include "bgsched/realtime/AbstractDatabaseChange.h"
#include "bgsched/realtime/FilterImpl.h"

#include <boost/utility.hpp>


namespace realtime {
namespace server {


class ClientFilter : boost::noncopyable
{
public:

    ClientFilter();

    void reset();
    void set( const bgsched::realtime::Filter::Impl& filter );

    bool check( bgsched::realtime::AbstractDatabaseChange &change );
        // returns true if should send.

private:

    ClientFilterVisitor _cfv;
};


} } // namespace realtime::server

#endif
