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

#ifndef BGWS_TIME_INTERVAL_OPTION_HPP_
#define BGWS_TIME_INTERVAL_OPTION_HPP_


#include "WhereClause.hpp"

#include <db/include/api/cxxdb/fwd.h>

#include <db/include/api/filtering/TimeInterval.h>

#include <boost/program_options.hpp>

#include <iosfwd>
#include <string>


namespace bgws {


class TimeIntervalOption
{
public:

    // Calculates SQL for a Duration. If plus_minus is + then get pluses between, else get minuses.
    // Reason we need this is because DB2 doesn't allow CURRENT_TIMESTAMP - (1 YEAR + 1 MONTH),
    // need to do CURRENT_TIMESTAMP - 1 YEAR - 1 MONTH
    static std::string calcSql( const BGQDB::filtering::Duration& d, bool plus_minus );

    enum class Nullable {
        Allow,
        Disallow
    };

    enum class Config {
        NoValue,
        HasInterval,
        IsNull,
        NotNull
    };


    explicit TimeIntervalOption(
            const std::string& name,
            Nullable nullable = Nullable::Disallow
        );

    TimeIntervalOption(
            const std::string& name,
            const BGQDB::filtering::TimeInterval& interval,
            Nullable nullable = Nullable::Disallow
        );

    void setTimeInterval(
            const BGQDB::filtering::TimeInterval& interval
        );

    void setIntervalStr( const std::string& s );

    Config getConfig() const  { return _config; }
    const BGQDB::filtering::TimeInterval& getInterval() const  { return _interval; }

    void addTo(
            boost::program_options::options_description& desc_in_out
        );

    // returns true if a clause was added.
    bool addTo(
            WhereClause& wc_in_out,
            cxxdb::ParameterNames& param_names_out,
            const std::string& column_name
        ) const;

    void bindParameters(
            cxxdb::Parameters &params_in_out
        ) const;


private:

    std::string _name;
    Nullable _nullable;

    Config _config;
    BGQDB::filtering::TimeInterval _interval;

};


std::istream& operator>>( std::istream& is, TimeIntervalOption& tia_out );


} // namespace bgws

#endif
