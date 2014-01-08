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

#include "../TimeIntervalOption.hpp"
#include "../WhereClause.hpp"

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <sstream>
#include <string>
#include <vector>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TimeIntervalOption
#include <boost/test/unit_test.hpp>


using namespace boost::assign;

using BGQDB::filtering::Duration;
using BGQDB::filtering::TimeInterval;

using bgws::TimeIntervalOption;
using bgws::WhereClause;

using boost::lexical_cast;

using std::istringstream;
using std::string;
using std::vector;


BOOST_AUTO_TEST_CASE( duration_sql )
{
    // calcSql converts a duration to SQL labeled duration

    BOOST_CHECK_EQUAL( TimeIntervalOption::calcSql( Duration( Duration::Date( 1 ) ), true ), "1 DAY" );
    BOOST_CHECK_EQUAL( TimeIntervalOption::calcSql( Duration( Duration::Date( 0, 1, 0 ) ), true ), "1 MONTH" );
    BOOST_CHECK_EQUAL( TimeIntervalOption::calcSql( Duration( Duration::Date( 0, 1, 1 ) ), true ), "1 MONTH + 1 DAY" );
    BOOST_CHECK_EQUAL( TimeIntervalOption::calcSql( Duration( Duration::Date( 1, 2, 3 ) ), true ), "1 YEAR + 2 MONTH + 3 DAY" );

    BOOST_CHECK_EQUAL( TimeIntervalOption::calcSql( Duration( boost::posix_time::hours( 1 ) ), true ), "1 HOUR" );
    BOOST_CHECK_EQUAL( TimeIntervalOption::calcSql( Duration( boost::posix_time::minutes( 1 ) ), true ), "1 MINUTE" );
    BOOST_CHECK_EQUAL( TimeIntervalOption::calcSql( Duration( boost::posix_time::seconds( 1 ) ), true ), "1 SECOND" );

    BOOST_CHECK_EQUAL( TimeIntervalOption::calcSql( Duration( boost::posix_time::hours( 1 ) + boost::posix_time::minutes( 2 ) + boost::posix_time::seconds( 3 ) ), true ), "1 HOUR + 2 MINUTE + 3 SECOND" );

    BOOST_CHECK_EQUAL(
            TimeIntervalOption::calcSql( Duration( Duration::Date( 1, 2, 3 ), boost::posix_time::hours( 4 ) + boost::posix_time::minutes( 5 ) + boost::posix_time::seconds( 6 ) ), true ),
            "1 YEAR + 2 MONTH + 3 DAY + 4 HOUR + 5 MINUTE + 6 SECOND"
        );

    BOOST_CHECK_EQUAL(
            TimeIntervalOption::calcSql( Duration( Duration::Date( 1, 2, 3 ), boost::posix_time::hours( 4 ) + boost::posix_time::minutes( 5 ) + boost::posix_time::seconds( 6 ) ), false ),
            "1 YEAR - 2 MONTH - 3 DAY - 4 HOUR - 5 MINUTE - 6 SECOND"
        );
}

BOOST_AUTO_TEST_CASE( default_not_added_to_where_clause )
{
    // If have default interval in TIA and add to WhereClause, nothing is added.

    TimeIntervalOption tia( "interval" );

    WhereClause wc;
    cxxdb::ParameterNames param_names;

    BOOST_CHECK( ! tia.addTo( wc, param_names, "colname" ) );

    BOOST_CHECK_EQUAL( wc.getString(), "" );
    BOOST_CHECK( param_names.empty() );
}


BOOST_AUTO_TEST_CASE( all_not_added_to_where_clause )
{
    // If have default interval in TIA and add to WhereClause, nothing is added.

    TimeIntervalOption tia(
            "interval",
            TimeInterval::ALL
        );

    WhereClause wc;
    cxxdb::ParameterNames param_names;

    BOOST_CHECK( ! tia.addTo( wc, param_names, "colname" ) );

    BOOST_CHECK_EQUAL( wc.getString(), "" );
    BOOST_CHECK( param_names.empty() );
}


BOOST_AUTO_TEST_CASE( start_end_time )
{
    // If TIA has interval with start and end timestamps, get where clause like "col BETWEEN ? AND ?"

    TimeIntervalOption tia( "interval",
            TimeInterval( boost::posix_time::time_from_string( "2011-01-31 10:00" ), boost::posix_time::time_from_string( "2011-01-31 11:00" ) )
        );

    WhereClause wc;
    cxxdb::ParameterNames param_names;

    BOOST_CHECK( tia.addTo( wc, param_names, "col1" ) );

    BOOST_CHECK_EQUAL( wc.getString(), " WHERE (col1 BETWEEN ? AND ?)" );

    cxxdb::ParameterNames exp_param_names = boost::assign::list_of( "_START_interval" )( "_END_interval" );
    BOOST_CHECK_EQUAL_COLLECTIONS( param_names.begin(), param_names.end(), exp_param_names.begin(), exp_param_names.end() );
}


BOOST_AUTO_TEST_CASE( duration )
{
    // If TIA has interval with duration, get where clause like "col >= (CURRENT_TIMESTAMP - (duration_sql))"

    {
        TimeIntervalOption tia( "interval",
                TimeInterval( Duration( Duration::Date( 1 ) ) )
            );

        WhereClause wc;
        cxxdb::ParameterNames param_names;

        BOOST_CHECK( tia.addTo( wc, param_names, "col1" ) );

        BOOST_CHECK_EQUAL( wc.getString(), " WHERE (col1 >= (CURRENT_TIMESTAMP - 1 DAY))" );
        BOOST_CHECK( param_names.empty() );
    }

    // Also check a more complicated duration.
    {
        TimeIntervalOption tia( "interval",
                TimeInterval( Duration( Duration::Date( 1, 2, 3 ), boost::posix_time::hours( 4 ) + boost::posix_time::minutes( 5 ) + boost::posix_time::seconds( 6 ) ) )
            );

        WhereClause wc;
        cxxdb::ParameterNames param_names;

        BOOST_CHECK( tia.addTo( wc, param_names, "col1" ) );

        BOOST_CHECK_EQUAL( wc.getString(), " WHERE (col1 >= (CURRENT_TIMESTAMP - 1 YEAR - 2 MONTH - 3 DAY - 4 HOUR - 5 MINUTE - 6 SECOND))" );
        BOOST_CHECK( param_names.empty() );
    }
}


BOOST_AUTO_TEST_CASE( start_duration )
{
    // If TIA has start time & duration, get where clause like (col >= ? AND col <= (? + (duration_sql))

    TimeIntervalOption tia( "interval",
            TimeInterval( boost::posix_time::time_from_string( "2011-01-31 10:00" ), Duration( Duration::Date( 1 ) ) )
        );

    WhereClause wc;
    cxxdb::ParameterNames param_names;
    BOOST_CHECK( tia.addTo( wc, param_names, "col1" ) );

    BOOST_CHECK_EQUAL( wc.getString(), " WHERE (col1 BETWEEN ? AND ?)" );

    cxxdb::ParameterNames exp_param_names = boost::assign::list_of( "_START_interval" )( "_END_interval" );
    BOOST_CHECK_EQUAL_COLLECTIONS( param_names.begin(), param_names.end(), exp_param_names.begin(), exp_param_names.end() );
}


BOOST_AUTO_TEST_CASE( duration_end )
{
    // If TIA has start time & duration, get where clause like (col >= ? AND col <= (? + (duration_sql))

    TimeIntervalOption tia( "interval",
            TimeInterval( Duration( Duration::Date( 1 ) ), boost::posix_time::time_from_string( "2011-01-31 10:00" ) )
        );

    WhereClause wc;
    cxxdb::ParameterNames param_names;
    BOOST_CHECK( tia.addTo( wc, param_names, "col1" ) );

    BOOST_CHECK_EQUAL( wc.getString(), " WHERE (col1 BETWEEN ? AND ?)" );

    cxxdb::ParameterNames exp_param_names = boost::assign::list_of( "_START_interval" )( "_END_interval" );
    BOOST_CHECK_EQUAL_COLLECTIONS( param_names.begin(), param_names.end(), exp_param_names.begin(), exp_param_names.end() );
}


BOOST_AUTO_TEST_CASE( input )
{
    TimeIntervalOption tia( "interval" );

    istringstream iss( "P1D" );

    iss >> tia;

    BOOST_CHECK_EQUAL( lexical_cast<string>( tia.getInterval() ), "P1D" );
}


static TimeInterval tryArgValue( const std::string &arg_value )
{
    namespace po = boost::program_options;

    TimeIntervalOption end_time_interval( "endTime" );

    po::options_description desc;

    end_time_interval.addTo( desc );

    vector<string> args = list_of( string() + "--endTime" )( arg_value );

    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( desc ).allow_unregistered().run(), vm );
    po::notify( vm );

    return end_time_interval.getInterval();
}


BOOST_AUTO_TEST_CASE( program_options )
{
    BOOST_CHECK_EQUAL( tryArgValue( "P1D" ), lexical_cast<TimeInterval>( "P1D" ) );
    BOOST_CHECK_EQUAL( tryArgValue( "P1Y" ), lexical_cast<TimeInterval>( "P1Y" ) );
    BOOST_CHECK_EQUAL( tryArgValue( "20100617T010101/20100617T020202" ), lexical_cast<TimeInterval>( "20100617T010101/20100617T020202" ) );
    BOOST_CHECK_EQUAL( tryArgValue( "20100617T000000/P1D" ), lexical_cast<TimeInterval>( "20100617T000000/P1D" ) );
}


BOOST_AUTO_TEST_CASE( invalid_value )
{
    // If you pass in an invalid value for the program option, it should be ignored, just get invalid interval (no exception)

    BOOST_CHECK_EQUAL( tryArgValue( "faslkdjflksajfsdaf" ), TimeInterval() );
}
