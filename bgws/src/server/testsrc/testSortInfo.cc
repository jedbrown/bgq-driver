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

#include "../SortInfo.hpp"
#include "../SortSpec.hpp"

#include "../utility/SortDirection.hpp"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <sstream>
#include <string>


using bgws::SortInfo;
using bgws::SortSpec;

using std::istringstream;
using std::string;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE SortInfo
#include <boost/test/unit_test.hpp>


static int initialize_()
{
    bgq::utility::initializeLogging( *(bgq::utility::Properties::create( "" )) );

    return 0;
}

static int s_initialized_(initialize_());


BOOST_AUTO_TEST_CASE( test_sort_spec_default )
{
    // Constructor with no args is default.

    SortSpec sort_spec;

    BOOST_CHECK( sort_spec.isDefault() );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_empty_string )
{
    // Constructed with empty string is default.

    SortSpec sort_spec( "" );

    BOOST_CHECK( sort_spec.isDefault() );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_col_id )
{
    // Constructed with column ID is not default, column ID is set, and sets direction to ascending.

    SortSpec sort_spec( "test1" );

    BOOST_REQUIRE( ! sort_spec.isDefault() );
    BOOST_CHECK_EQUAL( sort_spec.getColumnId(), "test1" );
    BOOST_CHECK_EQUAL( sort_spec.getDirection(), bgws::utility::SortDirection::Ascending );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_desc )
{
    // Constructed with - indicator is not default, column ID is set, and sets direction to descending.

    SortSpec sort_spec( "-test1" );

    BOOST_REQUIRE( ! sort_spec.isDefault() );
    BOOST_CHECK_EQUAL( sort_spec.getColumnId(), "test1" );
    BOOST_CHECK_EQUAL( sort_spec.getDirection(), bgws::utility::SortDirection::Descending );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_explicit_asc )
{
    // Constructed with + indicator is not default, column ID is set, and sets direction to ascending.

    SortSpec sort_spec( "+test1" );

    BOOST_REQUIRE( ! sort_spec.isDefault() );
    BOOST_CHECK_EQUAL( sort_spec.getColumnId(), "test1" );
    BOOST_CHECK_EQUAL( sort_spec.getDirection(), bgws::utility::SortDirection::Ascending );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_space_asc )
{
    // Constructed with " " as indicator not default, column ID is set, and sets direction to ascending.

    SortSpec sort_spec( " test1" );

    BOOST_REQUIRE( ! sort_spec.isDefault() );
    BOOST_CHECK_EQUAL( sort_spec.getColumnId(), "test1" );
    BOOST_CHECK_EQUAL( sort_spec.getDirection(), bgws::utility::SortDirection::Ascending );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_input )
{
    // Can extract a sort spec from an stream.

    SortSpec sort_spec;

    istringstream iss( "test1" );

    iss >> sort_spec;

    BOOST_REQUIRE( ! sort_spec.isDefault() );
    BOOST_CHECK_EQUAL( sort_spec.getColumnId(), "test1" );
    BOOST_CHECK_EQUAL( sort_spec.getDirection(), bgws::utility::SortDirection::Ascending );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_lexical_cast_asc_plus )
{
    // Can use lexical_cast.

    SortSpec sort_spec;

    sort_spec = boost::lexical_cast<SortSpec>( "+test1" );

    BOOST_REQUIRE( ! sort_spec.isDefault() );
    BOOST_CHECK_EQUAL( sort_spec.getColumnId(), "test1" );
    BOOST_CHECK_EQUAL( sort_spec.getDirection(), bgws::utility::SortDirection::Ascending );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_lexical_cast_desc )
{
    // Can use lexical_cast.

    SortSpec sort_spec;

    sort_spec = boost::lexical_cast<SortSpec>( "-test1" );

    BOOST_REQUIRE( ! sort_spec.isDefault() );
    BOOST_CHECK_EQUAL( sort_spec.getColumnId(), "test1" );
    BOOST_CHECK_EQUAL( sort_spec.getDirection(), bgws::utility::SortDirection::Descending );
}


BOOST_AUTO_TEST_CASE( test_sort_spec_lexical_cast_asc_space )
{
    // Can use lexical_cast.

    SortSpec sort_spec;

    sort_spec = boost::lexical_cast<SortSpec>( " test1" );

    BOOST_REQUIRE( ! sort_spec.isDefault() );
    BOOST_CHECK_EQUAL( sort_spec.getColumnId(), "test1" );
    BOOST_CHECK_EQUAL( sort_spec.getDirection(), bgws::utility::SortDirection::Ascending );
}


BOOST_AUTO_TEST_CASE( test_default_1_key_col_asc )
{
    // Given 1 key column, default ascending, can pass default SortSpec to calcSortClause to get the default string.

    const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of( "location", "location" );
    const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( "location" );
    const string DEFAULT_COLUMN_NAME( "location" );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Ascending);

    SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    string s(sort_info.calcSortClause( SortSpec() ));

    BOOST_CHECK_EQUAL( s, "location ASC" );
}


BOOST_AUTO_TEST_CASE( test_default_2_key_col_asc )
{
    // Given 2 key columns, default ascending, can pass default SortSpec to calcSortClause to get the default string.

    const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of( "location", "location" )( "time", "time" );
    const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( "location" )( "time" );
    const string DEFAULT_COLUMN_NAME( "location" );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Ascending);

    SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    string s(sort_info.calcSortClause( SortSpec() ));

    BOOST_CHECK_EQUAL( s, "location ASC, time ASC" );
}


BOOST_AUTO_TEST_CASE( test_default_desc )
{
    // If the default direction is Descending, then passing default SortSpec gives DESC.

    const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of( "location", "location" );
    const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( "location" );
    const string DEFAULT_COLUMN_NAME( "location" );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Descending);

    SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    string s(sort_info.calcSortClause( SortSpec() ));

    BOOST_CHECK_EQUAL( s, "location DESC" );
}


BOOST_AUTO_TEST_CASE( test_default_desc_2_keys )
{
    // If the default direction is Descending, then passing default SortSpec gives DESC, both keys are DESC.

    const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of( "location", "location" )( "time", "time" );
    const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( "location" )( "time" );
    const string DEFAULT_COLUMN_NAME( "location" );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Descending);

    SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    string s(sort_info.calcSortClause( SortSpec() ));

    BOOST_CHECK_EQUAL( s, "location DESC, time DESC" );
}


BOOST_AUTO_TEST_CASE( test_col_id )
{
    // If not using default sort spec, maps column ID to column name, picks up sort direction from sort spec, appends key columns after

    const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of( "location", "location" )( "time", "Time" );
    const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( "location" );
    const string DEFAULT_COLUMN_NAME( "location" );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Ascending);

    SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    string s(sort_info.calcSortClause( SortSpec( "time" ) ));

    BOOST_CHECK_EQUAL( s, "Time ASC, location ASC" );
}


BOOST_AUTO_TEST_CASE( test_desc )
{
    // If not using default sort spec, picks up sort direction from sort spec

    const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of( "location", "location" )( "time", "Time" );
    const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( "location" );
    const string DEFAULT_COLUMN_NAME( "location" );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Ascending);

    SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    string s(sort_info.calcSortClause( SortSpec( "-time" ) ));

    BOOST_CHECK_EQUAL( s, "Time DESC, location DESC" );
}


BOOST_AUTO_TEST_CASE( test_unexpected_column )
{
    // If SortSpec has unexpected column name, uses default column name.

    const SortInfo::IdToCol ID_TO_COL = boost::assign::map_list_of( "location", "location" );
    const SortInfo::KeyCols KEY_COLS = boost::assign::list_of( "location" );
    const string DEFAULT_COLUMN_NAME( "location" );
    const bgws::utility::SortDirection::Value DEFAULT_DIRECTION(bgws::utility::SortDirection::Ascending);

    SortInfo sort_info( ID_TO_COL, KEY_COLS, DEFAULT_COLUMN_NAME, DEFAULT_DIRECTION );

    string s(sort_info.calcSortClause( SortSpec( "notacolumn" ) ));

    BOOST_CHECK_EQUAL( s, "location ASC" );
}
