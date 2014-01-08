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

#ifndef BGWS_RESPONDER_PERFORMANCE_MONITORING_HPP_
#define BGWS_RESPONDER_PERFORMANCE_MONITORING_HPP_


#include "../AbstractResponder.hpp"

#include "../RequestRange.hpp"
#include "../TimeIntervalOption.hpp"

#include "capena-http/http/uri/Path.hpp"

#include <string>


namespace bgws {
namespace responder {


class PerformanceMonitoring : public AbstractResponder
{
public:

    static bool matchesUrl(
            const capena::http::uri::Path& requested_resource
        );


    PerformanceMonitoring(
            CtorArgs& args
        ) :
            AbstractResponder( args )
    { /* Nothing to do */ }


    capena::http::Methods _getAllowedMethods() const  { return { capena::http::Method::GET }; }

    void _doGet();


private:

    enum class _Grouping {
        None,
        Daily,
        Weekly,
        Monthly
    };


    static const std::string _FUNCTION_BOOT_MODE_BASIC_SQL;

    static _Grouping _parseGroupingCode( const std::string& grouping_code );


    void _checkAuthority();

    void _getMultiDetails(
            const RequestRange& req_range,
            const TimeIntervalOption& interval
        );

    void _getSingle(
            const TimeIntervalOption& interval,
            const std::string& detail,
            const std::string& block
        );

    void _getGrouped(
            const RequestRange& req_range,
            const TimeIntervalOption& interval,
            const std::string& detail,
            const std::string& block,
            _Grouping grouping
        );

};


}} // namespace bgws::responder


#endif
