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


#include "common.hpp"


namespace bgws {
namespace common {


namespace resource_path {

    const capena::http::uri::Path BGWS_SERVER(capena::http::uri::Path() / "bgwsServer");
    const capena::http::uri::Path BLOCKS(capena::http::uri::Path() / "blocks");
    const capena::http::uri::Path JOBS(capena::http::uri::Path() / "jobs");
    const capena::http::uri::Path LOGGING(BGWS_SERVER / "logging");
    const capena::http::uri::Path RAS(capena::http::uri::Path() / "ras");

}


namespace blocks_query {

    const std::string STATUS_OPTION_NAME( "status" );
    const std::string TYPE_OPTION_NAME( "type" );

    namespace type {
        const std::string COMPUTE( "compute" );
        const std::string IO( "io" );
    }

} // namespace blocks_query


} } // namespace bgws::common
