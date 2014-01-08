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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#ifndef BGWS_QUERY_ALERTS_OPTIONS_HPP_
#define BGWS_QUERY_ALERTS_OPTIONS_HPP_


#include "../MultiWildcardOption.hpp"
#include "../RequestRange.hpp"
#include "../SortSpec.hpp"
#include "../TimeIntervalOption.hpp"

#include "capena-http/http/http.hpp"

#include <set>
#include <string>


namespace bgws {
namespace query {


struct AlertsOptions
{
    bool dups;
    std::set<std::string> location_types;
    std::set<std::string> states;
    std::set<std::string> severities;
    std::set<std::string> urgencies;
    TimeIntervalOption interval;
    MultiWildcardOption location;
    SortSpec sort_spec;
    RequestRange req_range;


    AlertsOptions(
            const RequestRange& req_range,
            const capena::http::uri::Query& query
        );

};


} } // namespace bgws::query

#endif
