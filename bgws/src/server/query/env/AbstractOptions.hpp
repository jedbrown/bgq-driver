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

#ifndef BGWS_QUERY_ENV_ABSTRACT_OPTIONS_HPP_
#define BGWS_QUERY_ENV_ABSTRACT_OPTIONS_HPP_


#include "../../RequestRange.hpp"
#include "../../SortInfo.hpp"
#include "../../TimeIntervalOption.hpp"
#include "../../WhereClause.hpp"
#include "../../WildcardOption.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>


namespace bgws {
namespace query {
namespace env {


class AbstractOptions
{
public:

    explicit AbstractOptions(
            unsigned location_column_size
        );

    void setArgs(
            const std::vector<std::string>& args,
            const RequestRange& range
        );


    virtual const std::string& getTableName() =0;

    // Override to provide an extra key column.
    virtual const std::string& getExtraKeyColumnName()  { static const std::string ret; return ret; }


    bool isMostRecent() const  { return _interval.getInterval() == BGQDB::filtering::TimeInterval(); }

    void calcWhereClauseSql(
            bool is_count,
            std::string* where_clause_sql_out,
            cxxdb::ParameterNames* parameter_names_out
        ) const;

    std::string calcSortClauseSql() const;

    void bindParameters(
            bool is_count,
            cxxdb::QueryStatementPtr stmt_ptr
        );


    virtual ~AbstractOptions()  { /* Nothing to do */ }


protected:

    static const std::string _QUAL;


    virtual void _addExtraMappings(
            SortInfo::IdToCol& /*id_to_col_in_out*/
        )
    { /* Do nothing by default */ }

    virtual void _addOptions(
            boost::program_options::options_description& /*desc*/
        )
    { /* Do nothing by default. */ }

    virtual void _addOptions(
            WhereClause& /*where_clause_in_out*/,
            cxxdb::ParameterNames& /*parameter_names_in_out*/
        ) const
    { /* Do nothing by default */ }

    virtual void _bindParameters(
            cxxdb::Parameters& /*parameters_in_out*/
        )
    { /* Do nothing by default */ }


private:

    boost::shared_ptr<SortInfo> _sort_info_ptr;

    TimeIntervalOption _interval;
    WildcardOption _location;
    SortSpec _sort_spec;

    boost::scoped_ptr<RequestRange> _range_ptr;


    boost::shared_ptr<SortInfo> _createSortInfo();
};

} } } // namespace bgws::query::env


#endif
