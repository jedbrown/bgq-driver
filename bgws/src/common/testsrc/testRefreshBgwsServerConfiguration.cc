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


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE RefreshBgwsServerConfiguration
#include <boost/test/unit_test.hpp>


#include "../RefreshBgwsServerConfiguration.hpp"

#include "chiron-json/json.hpp"


using bgws::common::RefreshBgwsServerConfiguration;


//---------------------------
// get type and filename


BOOST_AUTO_TEST_CASE( testRereadCurrentType )
{
    // There's a static const RereadCurrent. The "type" of RereadCurrent is RereadCurrent.
    BOOST_CHECK_EQUAL( RefreshBgwsServerConfiguration::RereadCurrent.getType(), RefreshBgwsServerConfiguration::Type::RereadCurrent );
}


BOOST_AUTO_TEST_CASE( testReadDefaultType )
{
    // There's a static const ReadDefault. The "type" of ReadDefault is ReadDefault.
    BOOST_CHECK_EQUAL( RefreshBgwsServerConfiguration::ReadDefault.getType(), RefreshBgwsServerConfiguration::Type::ReadDefault );
}


BOOST_AUTO_TEST_CASE( testReadNewType )
{
    // There's a static constructor function ReadNew. The "type" of object created with ReadNew is ReadNew.
    BOOST_CHECK_EQUAL( RefreshBgwsServerConfiguration::ReadNew( "/myConfig.properties" ).getType(), RefreshBgwsServerConfiguration::Type::ReadNew );
}


BOOST_AUTO_TEST_CASE( testReadNewFilename )
{
    // There's a static constructor function ReadNew. The "filename" is whatever was passed in.
    BOOST_CHECK_EQUAL( RefreshBgwsServerConfiguration::ReadNew( "/myConfig.properties" ).getFilename(), "/myConfig.properties" );
}


//---------------------------
// getFilename when no filename

BOOST_AUTO_TEST_CASE( testRereadCurrentFilename_ex )
{
    // If try to get filename when is RereadCurrent, fails with logic_error.
    BOOST_CHECK_THROW( RefreshBgwsServerConfiguration::RereadCurrent.getFilename(), std::logic_error );
}


BOOST_AUTO_TEST_CASE( testReadDefaultFilename_ex )
{
    // If try to get filename when is ReadDefault, fails with logic_error.
    BOOST_CHECK_THROW( RefreshBgwsServerConfiguration::ReadDefault.getFilename(), std::logic_error );
}


//---------------------------
// ReadNew error

BOOST_AUTO_TEST_CASE( testReadNewNotFullPath_ex )
{
    // If try to create ReadNew with relative path then fails.
    BOOST_CHECK_THROW( RefreshBgwsServerConfiguration::ReadNew( "thisFile.properties" ), RefreshBgwsServerConfiguration::PathNotComplete );
}


//---------------------------
// toJson

BOOST_AUTO_TEST_CASE( testRereadCurrentToJson )
{
    // Calling toJson on RereadCurrent returns a JSON object that causes server to refresh config with current properties file.

    json::ValuePtr val_ptr(RefreshBgwsServerConfiguration::RereadCurrent.toJson());

    BOOST_REQUIRE( val_ptr->isObject() );

    const json::Object &obj(val_ptr->getObject());

    BOOST_CHECK_EQUAL( obj.getString( "operation" ), "refreshConfig" );
    BOOST_CHECK( ! obj.contains( "propertiesFile" ) );
}


BOOST_AUTO_TEST_CASE( testReadDefaultToJson )
{
    // Calling toJson on ReadDefault returns a JSON object that causes the server to refresh config with the default properties file.

    json::ValuePtr val_ptr(RefreshBgwsServerConfiguration::ReadDefault.toJson());

    BOOST_REQUIRE( val_ptr->isObject() );

    const json::Object &obj(val_ptr->getObject());

    BOOST_CHECK_EQUAL( obj.getString( "operation" ), "refreshConfig" );
    BOOST_CHECK_EQUAL( obj.getString( "propertiesFile" ), "" );
}


BOOST_AUTO_TEST_CASE( testReadNewToJson )
{
    // Calling toJson on ReadNew returns a JSON object that causes the server to refresh config with the new properties file.

    json::ValuePtr val_ptr(RefreshBgwsServerConfiguration::ReadNew( "/newConfigFile.properties" ).toJson());

    BOOST_REQUIRE( val_ptr->isObject() );

    const json::Object &obj(val_ptr->getObject());

    BOOST_CHECK_EQUAL( obj.getString( "operation" ), "refreshConfig" );
    BOOST_CHECK_EQUAL( obj.getString( "propertiesFile" ), "/newConfigFile.properties" );
}


//---------------------------
// Create from json value.

BOOST_AUTO_TEST_CASE( testRereadCurrentFromJson )
{
    // Construct with JSON for RereadCurrent get type is RereadCurrent.
    BOOST_CHECK_EQUAL( RefreshBgwsServerConfiguration( *(json::Parser()( "{ \"operation\": \"refreshConfig\" }" )) ).getType(), RefreshBgwsServerConfiguration::Type::RereadCurrent );
}


BOOST_AUTO_TEST_CASE( testReadDefaultFromJson )
{
    // Construct with JSON for RereadCurrent get type is RereadCurrent.
    BOOST_CHECK_EQUAL( RefreshBgwsServerConfiguration( *(json::Parser()( "{ \"operation\": \"refreshConfig\", \"propertiesFile\": \"\" }" )) ).getType(), RefreshBgwsServerConfiguration::Type::ReadDefault );
}


BOOST_AUTO_TEST_CASE( testReadNewFromJson )
{
    // Construct with JSON for ReadNew get type is ReadNew and can get filename.
    RefreshBgwsServerConfiguration refresh_bgws_server_configuration( *(json::Parser()( "{ \"operation\": \"refreshConfig\", \"propertiesFile\": \"/newConfigFile2.properties\" }" )) );
    BOOST_REQUIRE_EQUAL( refresh_bgws_server_configuration.getType(), RefreshBgwsServerConfiguration::Type::ReadNew );
    BOOST_CHECK_EQUAL( refresh_bgws_server_configuration.getFilename(), "/newConfigFile2.properties" );
}


//---------------------------
// Problems creating from JSON.


BOOST_AUTO_TEST_CASE( testFromJsonNotObject_ex )
{
    // Construct with JSON with not an object throws.
    BOOST_CHECK_THROW( RefreshBgwsServerConfiguration( *(json::Parser()( "[]" )) ), RefreshBgwsServerConfiguration::NotObjectError );
}


BOOST_AUTO_TEST_CASE( testFromJsonNoOperation_ex )
{
    // Construct with JSON when no operation fails with NoOperation.
    BOOST_CHECK_THROW( RefreshBgwsServerConfiguration( *(json::Parser()( "{}" )) ), RefreshBgwsServerConfiguration::NoOperationError );
}


BOOST_AUTO_TEST_CASE( testFromJsonOperationNotString_ex )
{
    // Construct with JSON when no operation fails with NoOperation.
    BOOST_CHECK_THROW( RefreshBgwsServerConfiguration( *(json::Parser()( "{ \"operation\": 5 }" )) ), RefreshBgwsServerConfiguration::OperationNotStringError );
}


BOOST_AUTO_TEST_CASE( testFromJsonOperationNotRefreshConfig_ex )
{
    // Construct with JSON when no operation fails with NoOperation.
    BOOST_CHECK_THROW( RefreshBgwsServerConfiguration( *(json::Parser()( "{ \"operation\": \"shutdown\" }" )) ), RefreshBgwsServerConfiguration::UnexpectedOperationError );
}


BOOST_AUTO_TEST_CASE( testFromJsonOperationPropertiesFileNotComplete_ex )
{
    // Construct with JSON when no operation fails with NoOperation.
    BOOST_CHECK_THROW( RefreshBgwsServerConfiguration( *(json::Parser()( "{ \"operation\": \"refreshConfig\", \"propertiesFile\": \"incompletePath.properties\" }" )) ), RefreshBgwsServerConfiguration::PathNotComplete );
}


//---------------------------
// If propertiesFile is not a string then it's ignored and the default is used (RereadCurrent)

BOOST_AUTO_TEST_CASE( testRereadCurrentWhenPropertiesFileNotString )
{
    // Construct with JSON for RereadCurrent get type is RereadCurrent.
    BOOST_CHECK_EQUAL( RefreshBgwsServerConfiguration( *(json::Parser()( "{ \"operation\": \"refreshConfig\", \"propertiesFile\": 100 }" )) ).getType(), RefreshBgwsServerConfiguration::Type::RereadCurrent );
}
