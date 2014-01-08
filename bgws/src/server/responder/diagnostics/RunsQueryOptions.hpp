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

#ifndef BGWS_RESPONDER_DIAGNOSTICS_RUNS_QUERY_OPTIONS_HPP_
#define BGWS_RESPONDER_DIAGNOSTICS_RUNS_QUERY_OPTIONS_HPP_


#include "../../TimeIntervalOption.hpp"
#include "../../SortSpec.hpp"
#include "../../WhereClause.hpp"

#include "capena-http/http/uri/Query.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <set>
#include <string>


namespace bgws {
namespace responder {
namespace diagnostics {


class RunsQueryOptions
{
public:

    RunsQueryOptions(
            const capena::http::uri::Query::Arguments& args
        );


    void addToWhereClause(
            WhereClause* where_clause_out,
            cxxdb::ParameterNames* param_names_out,
            std::string* sort_clause_sql_out
        ) const;

    void bindParameters(
            cxxdb::QueryStatement& stmt
        ) const;


    bool includesRunning() const;

    const std::set<std::string>& getBlocks() const  { return _blocks; }
    const TimeIntervalOption& getEnd() const  { return _end; }
    const SortSpec& getSortSpec() const  { return _sort_spec; }
    const TimeIntervalOption& getStart() const  { return _start; }
    const std::set<std::string>& getStatuses() const  { return _statuses; }


private:

    std::set<std::string> _blocks;
    TimeIntervalOption _end;
    SortSpec _sort_spec;
    TimeIntervalOption _start;
    std::set<std::string> _statuses;

};


} } } // namespace bgws::responder::diagnostics


#endif
