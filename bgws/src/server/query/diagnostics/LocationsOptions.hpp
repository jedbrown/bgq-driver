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

#ifndef BGWS_QUERY_DIAGNOSTICS_LOCATIONS_OPTIONS_HPP_
#define BGWS_QUERY_DIAGNOSTICS_LOCATIONS_OPTIONS_HPP_


#include "../../RequestRange.hpp"
#include "../../SortSpec.hpp"
#include "../../TimeIntervalOption.hpp"
#include "../../WhereClause.hpp"
#include "../../WildcardOption.hpp"

#include "capena-http/http/uri/Query.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>


namespace bgws {
namespace query {
namespace diagnostics {


class LocationsOptions
{
public:

    LocationsOptions(
            const capena::http::uri::Query::Arguments& query_values,
            const RequestRange& range
        );

    void calcWhereClause(
            WhereClause* loc_out,
            WhereClause* main_out,
            cxxdb::ParameterNames* param_names_out
        );

    std::string calcSortClauseSql();

    void bindParameters(
            cxxdb::Parameters& params_in_out
        );

    const RequestRange& getRange() const  { return _range; }


private:

    std::vector<std::string> _hardware_statuses;
    TimeIntervalOption _interval;
    WildcardOption _location;
    boost::shared_ptr<bool> _replace_ptr;

    SortSpec _sort_spec;

    RequestRange _range;


    void _notifyHardwareStatus( const std::string& s );
    void _notifyReplace( const std::string& s );
};


} } } // namespace bgws::query::diagnostics


#endif
