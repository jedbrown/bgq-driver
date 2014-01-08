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

#ifndef BGWS_CLIENTS_UTILITY_HPP_
#define BGWS_CLIENTS_UTILITY_HPP_


#include "BgwsOptions.hpp"

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <curl/curl.h>

#include <string>


namespace bgws_clients {
namespace utility {


void calcSessionId(
        const BgwsOptions& bgws_options,
        std::string& session_id_out
    );


boost::filesystem::path calcDefaultSessionPath();


void getSessionIdFromSessionFile(
        const boost::filesystem::path session_path,
        std::string& session_id_out
    );


typedef boost::shared_ptr<void> HeaderGuard;

HeaderGuard setHeaders(
        CURL* chandle,
        char error_buffer[],
        const std::string& session_id,
        bool posting_set_content_type_json = false
    );


void setSslOptions(
        const BgwsOptions& bgws_options,
        CURL *chandle,
        char error_buffer[]
    );


} } // namespace bgws_clients::utility


#endif
