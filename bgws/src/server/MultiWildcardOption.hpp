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

#ifndef BGWS_MULTI_WILDCARD_OPTION_HPP_
#define BGWS_MULTI_WILDCARD_OPTION_HPP_


#include "WhereClause.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <db/include/api/tableapi/DBObj.h>

#include <boost/program_options.hpp>

#include <string>


namespace bgws {


class MultiWildcardOption
{
public:

    MultiWildcardOption(
            const std::string& name,
            unsigned max_length = -1,
            BGQDB::DBObj::ColumnType::Value col_type = BGQDB::DBObj::ColumnType::Other
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


private:

    struct _Pattern
    {
        bool use_like;
        std::string pattern;

        _Pattern() {};
        _Pattern( const std::string &pattern_str );
    };

    typedef std::vector<_Pattern> _Patterns;


    std::string _name;
    unsigned _max_length;
    BGQDB::DBObj::ColumnType::Value _col_type;

    _Patterns _neg_patterns;
    _Patterns _inc_patterns;


    void _notifyValue( const std::string& s );
};

} // namespace bgws

#endif
