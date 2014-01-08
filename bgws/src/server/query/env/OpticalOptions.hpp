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

#ifndef BGWS_QUERY_ENV_OPTICAL_OPTIONS_HPP_
#define BGWS_QUERY_ENV_OPTICAL_OPTIONS_HPP_


#include "AbstractOptions.hpp"

#include "../../NumberCompareOption.hpp"

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>


namespace bgws {
namespace query {
namespace env {


class OpticalOptions : public AbstractOptions
{
public:

    OpticalOptions();

    // override
    const std::string& getTableName()  { static const std::string ret(BGQDB::DBTOpticalenvironment().getTableName()); return ret; }

    // override
    virtual const std::string& getExtraKeyColumnName()  { return BGQDB::DBTOpticalenvironment::CHANNEL_COL; }


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

    IntegerCompareOption
            _channel,
            _power
        ;

};

} } } // namespace bgws::query::env

#endif
