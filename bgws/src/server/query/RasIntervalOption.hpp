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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef BGWS_QUERY_RAS_INTERVAL_OPTION_HPP_
#define BGWS_QUERY_RAS_INTERVAL_OPTION_HPP_


#include "../TimeIntervalOption.hpp"
#include "../WhereClause.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <cstdint>
#include <iosfwd>
#include <string>


namespace bgws {
namespace query {


class RasIntervalOption
{
public:

    enum class Config {
        IdRange,
        TimeInterval,
        NotUsed
    };


    explicit RasIntervalOption(
            const std::string& name
        );


    void setRange( std::uint64_t start, std::uint64_t end );
    void setTimeIntervalOption( const TimeIntervalOption& tio );

    const std::string& getName() const  { return _name; }
    Config getConfig() const  { return _config; }

    void addTo(
            boost::program_options::options_description& desc_in_out
        );

    // returns true if a clause was added.
    bool addTo(
            WhereClause& wc_in_out,
            cxxdb::ParameterNames& param_names_out
        ) const;

    void bindParameters(
            cxxdb::Parameters& params_in_out
        ) const;


private:

    std::string _name;

    Config _config;
    TimeIntervalOption _time_interval_option;
    std::uint64_t _range_start, _range_end;


    void _set( const std::string& s );
};


} } // namespace bgws::query

#endif
