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

#ifndef BGWS_DB_CONNECTION_POOL_HPP_
#define BGWS_DB_CONNECTION_POOL_HPP_


#include <db/include/api/cxxdb/fwd.h>
#include <utility/include/Properties.h>


namespace bgws {
namespace dbConnectionPool {


void initialize(
        bgq::utility::Properties::Ptr bg_properties_ptr
    );


/*! \throws runtime_error if couldn't get a connection. */
cxxdb::ConnectionPtr getConnection();


} } // namespace bgws::dbConnectionPool


#endif
