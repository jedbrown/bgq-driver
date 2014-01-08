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

#ifndef BGWS_QUERY_ENV_COOLANT_OPTIONS_HPP_
#define BGWS_QUERY_ENV_COOLANT_OPTIONS_HPP_


#include "AbstractOptions.hpp"

#include "../../NumberCompareOption.hpp"

#include <boost/lexical_cast.hpp>

#include <string>

#include <sqltypes.h>


namespace bgws {
namespace query {
namespace env {


class CoolantValueParser
{
public:

    SQLINTEGER operator()( const std::string& num_str )
    {
        // num_str comes in as a double. Parse double multiply by 100, convert to integer (rounds to 0).
        double d_val(boost::lexical_cast<double>( num_str ));
        SQLINTEGER i_val(d_val * 100.0);
        return i_val;
    }

};


typedef NumberCompareOption<SQLINTEGER, CoolantValueParser> CoolantValueCompareOption;


class CoolantOptions : public AbstractOptions
{
public:

    CoolantOptions();

    // override
    const std::string& getTableName()  { static const std::string ret("bgqCoolantEnvironment"); return ret; }


protected:

    // override
    void _addExtraMappings(
            SortInfo::IdToCol& id_to_col_in_out
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

    CoolantValueCompareOption _inlet_flow_rate;
    CoolantValueCompareOption _outlet_flow_rate;
    CoolantValueCompareOption _coolant_pressure;
    CoolantValueCompareOption _diff_pressure;
    CoolantValueCompareOption _inlet_coolant_temp;
    CoolantValueCompareOption _outlet_coolant_temp;
    CoolantValueCompareOption _dewpoint_temp;
    CoolantValueCompareOption _ambient_temp;
    CoolantValueCompareOption _ambient_humidity;
    CoolantValueCompareOption _system_power;
    IntegerCompareOption _shutoff_cause;
};

} } } // namespace bgws::query::env

#endif
