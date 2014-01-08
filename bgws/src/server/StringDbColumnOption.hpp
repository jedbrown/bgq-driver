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


#ifndef BGWS_STRING_DB_COLUMN_OPTION_HPP_
#define BGWS_STRING_DB_COLUMN_OPTION_HPP_


#include "WhereClause.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/program_options.hpp>

#include <string>


namespace bgws {


class StringDbColumnOption
{
public:

    StringDbColumnOption(
            const std::string& name,
            unsigned max_length
        );

    void addTo(
            boost::program_options::options_description& desc_in_out
        );

    void addTo(
            WhereClause& wc_in_out,
            cxxdb::ParameterNames& parameter_names_in_out,
            const std::string& col_name
        ) const;

    void bindParameters(
            cxxdb::Parameters& parameters_in_out
        ) const;

    bool hasValue() const  { return (! _no_value); }
    const std::string& getValue() const  { return _value; }


private:

    std::string _name;
    unsigned _max_length;

    bool _no_value;
    std::string _value;


    void _notifyValue( const std::string& s );

};


} // namespace bgws

#endif
