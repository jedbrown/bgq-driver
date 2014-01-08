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

#ifndef BGWS_QUERY_RAS_OPTIONS_HPP_
#define BGWS_QUERY_RAS_OPTIONS_HPP_


#include "RasIntervalOption.hpp"

#include "../JobIdOption.hpp"
#include "../MultiWildcardOption.hpp"
#include "../RequestRange.hpp"
#include "../SortInfo.hpp"
#include "../SortSpec.hpp"
#include "../StringDbColumnOption.hpp"
#include "../WildcardOption.hpp"

#include "capena-http/http/uri/Query.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEvent.h>

#include <boost/shared_ptr.hpp>

#include <set>
#include <string>


namespace bgws {
namespace query {


class RasOptions
{
public:

    typedef std::set<RasEvent::Severity> RasSeverities;

    static const RasSeverities ALL_RAS_SEVERITIES;


    RasOptions(
            const capena::http::uri::Query::Arguments& parameters,
            const RequestRange& req_range
        );


    void calcWhereClauseSql(
            std::string* sql_out,
            cxxdb::ParameterNames* parameter_names_out
        ) const;

    void bindParameters(
            cxxdb::QueryStatementPtr stmt_ptr
        ) const;

    std::string calcSortClauseSql() const;

    const RequestRange& getRange() const  { return _range; }


private:

    typedef std::set<std::string> _ControlActions;

    static const SortInfo _s_sort_info;

    static SortInfo _createSortInfo();


    MultiWildcardOption _block;
    MultiWildcardOption _category;
    MultiWildcardOption _component;
    _ControlActions _control_actions;
    RasIntervalOption _interval;
    JobIdOption _jobid;
    MultiWildcardOption _location;
    WildcardOption _message;
    MultiWildcardOption _msg_id;
    StringDbColumnOption _serialnumber;

    RasSeverities _severities;

    SortSpec _sort_spec;

    RequestRange _range;
};

} } // namespace bgws::query

#endif
