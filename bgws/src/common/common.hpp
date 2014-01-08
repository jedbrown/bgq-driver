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


#ifndef BGWS_COMMON_COMMON_HPP_
#define BGWS_COMMON_COMMON_HPP_


#include "capena-http/http/uri/Path.hpp"

#include <string>


namespace bgws {
namespace common {


namespace resource_path {

    extern const capena::http::uri::Path BGWS_SERVER;
    extern const capena::http::uri::Path BLOCKS;
    extern const capena::http::uri::Path JOBS;
    extern const capena::http::uri::Path LOGGING;
    extern const capena::http::uri::Path RAS;

} // namespace resource_path


namespace blocks_query {

    extern const std::string STATUS_OPTION_NAME;

    extern const std::string TYPE_OPTION_NAME;

    enum class Type {
        Compute,
        Io
    };

    namespace type {
        extern const std::string COMPUTE;
        extern const std::string IO;
    }

} // namespace blocks_query


} } // namespace bgws::common

#endif
