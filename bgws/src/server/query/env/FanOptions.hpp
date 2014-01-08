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

#ifndef BGWS_QUERY_ENV_FAN_OPTIONS_HPP_
#define BGWS_QUERY_ENV_FAN_OPTIONS_HPP_


#include "AbstractOptions.hpp"

#include "../../NumberCompareOption.hpp"


namespace bgws {
namespace query {
namespace env {


class FanOptions : public AbstractOptions
{
public:

    FanOptions();

    // override
    const std::string& getTableName()  { static const std::string ret("bgqFanEnvironment"); return ret; }

    // override
    virtual const std::string& getExtraKeyColumnName()  { static const std::string ret( "fanNumber" ); return ret; }


protected:

    // override
    void _addExtraMappings(
            SortInfo::IdToCol& /*id_to_col_in_out*/
        );


    // override
    void _addOptions(
            boost::program_options::options_description& desc
        );

    // override
    void _addOptions(
            WhereClause& where_clause_in_out,
            cxxdb::ParameterNames& parameter_names_in_out
        ) const;

    // override
    void _bindParameters(
            cxxdb::Parameters& parameters_in_out
        );


private:

    IntegerCompareOption _fan_number, _rpms;
};

} } } // namespace bgws::query::env

#endif
