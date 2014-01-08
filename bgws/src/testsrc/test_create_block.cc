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

#include "utility.hpp"

#include <string>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE bgws
#include <boost/test/unit_test.hpp>


using std::string;


static string calc_invalid_block_id_error_json( const string& block_id )
{
    return (string() +

"{\n"
"  \"data\" : {\n"
"      \"blockId\" : \"" + block_id + "\",\n"
"      \"errMsg\" : \"the block ID is not valid\"\n"
"    },\n"
"  \"id\" : \"invalidArgument\",\n"
"  \"operation\" : \"createBlock\",\n"
"  \"text\" : \"Could not create '" + block_id + "' because an argument was not valid. The error is 'the block ID is not valid'.\"\n"
"}\n"

        );

}


BOOST_GLOBAL_FIXTURE( CurlGlobalSetup );


//---------------------------------------------------------------------
// Test cases


BOOST_AUTO_TEST_CASE( test_single_midplane )
{
    string block_id( "testSingle" );

    Response response;

    http_post(
            BLOCKS_URL,
            calc_create_single_midplane_block_json( block_id ),
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::Created );
    BOOST_CHECK_EQUAL( response.getHeaders().at("Location"), string() + "/bg/blocks/" + block_id );

    simpleDeleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_single_midplane_torus )
{
    string block_id( "testSingleTorus");

    string json( "{ \"id\": \"" + block_id + "\", \"midplane\": \"R00-M0\", \"midplanes\": [ \"1\", \"1\", \"1\", \"1\" ], \"torus\": \"ABCD\" }" );
    Response response;

    http_post( BLOCKS_URL, json, response );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::Created );
    BOOST_CHECK_EQUAL( response.getHeaders().at("Location"), string() + "/bg/blocks/" + block_id );

    simpleDeleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_id_max_length )
{
    // block ID can only be 32 characters, otherwise is invalid.

    string block_id( "testMaxLength4567820123456783012" );

    Response response;

    http_post(
            BLOCKS_URL,
            calc_create_single_midplane_block_json( block_id ),
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::Created );

    // Delete just in case it worked.
    simpleDeleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_id_invalid_char )
{
    string block_id( "b''" );

    Response response;

    http_post(
            BLOCKS_URL,
            calc_create_single_midplane_block_json( block_id ),
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );
    BOOST_CHECK_EQUAL( response.getContentType(), CONTENT_TYPE_JSON );
    BOOST_CHECK_EQUAL( response.getContent(), calc_invalid_block_id_error_json( block_id ) );

    // Delete just in case it worked.
    try {
        simpleDeleteBlock( block_id );
    } catch ( ... ) {
        // Ignore any error.
    }
}


BOOST_AUTO_TEST_CASE( test_empty_id )
{
    string block_id;

    Response response;

    http_post(
            BLOCKS_URL,
            calc_create_single_midplane_block_json( block_id ),
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );
    BOOST_CHECK_EQUAL( response.getContentType(), CONTENT_TYPE_JSON );
    BOOST_CHECK_EQUAL( response.getContent(), calc_invalid_block_id_error_json( block_id ) );

    // Delete just in case it worked.
    try {
        simpleDeleteBlock( block_id );
    } catch ( ... ) {
        // Ignore any error.
    }
}


BOOST_AUTO_TEST_CASE( test_id_too_long )
{
    // block ID can only be 32 characters, otherwise is invalid.

    string block_id( "b23456781012345678201234567830123" );

    Response response;

    http_post(
            BLOCKS_URL,
            calc_create_single_midplane_block_json( block_id ),
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );
    BOOST_CHECK_EQUAL( response.getContentType(), CONTENT_TYPE_JSON );
    BOOST_CHECK_EQUAL( response.getContent(), calc_invalid_block_id_error_json( block_id ) );

    // Delete just in case it worked.
    try {
        simpleDeleteBlock( block_id );
    } catch ( ... ) {
        // Ignore any error.
    }
}


BOOST_AUTO_TEST_CASE( test_no_id )
{
    // If no id field in the request then should send back BadRequest

    string json( "{ \"midplane\": \"R00-M0\", \"midplanes\": [ \"1\", \"1\", \"1\", \"1\" ] }" );

    Response response;

    http_post(
            BLOCKS_URL,
            json,
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );
}


BOOST_AUTO_TEST_CASE( test_id_not_string )
{
    // If id isn't a string then should send back BadRequest

    string json( "{ \"id\": 1, \"midplane\": \"R00-M0\", \"midplanes\": [ \"1\", \"1\", \"1\", \"1\" ] }" );

    Response response;

    http_post(
            BLOCKS_URL,
            json,
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );
}


BOOST_AUTO_TEST_CASE( test_midplane_empty )
{
    // If midplane is empty then should send back BadRequest.

    string block_id( "testMidplaneEmpty" );

    string json( "{ \"id\": \"" + block_id + "\", \"midplane\": \"\", \"midplanes\": [ \"1\", \"1\", \"1\", \"1\" ] }" );

    Response response;

    http_post(
            BLOCKS_URL,
            json,
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );

    // Delete just in case it worked.
    try {
        simpleDeleteBlock( block_id );
    } catch ( ... ) {
        // Ignore any error.
    }
}


BOOST_AUTO_TEST_CASE( test_midplane_missing )
{
    string block_id( "testNoMidplane" );

    string json( "{ \"id\": \"" + block_id + "\", \"midplanes\": [ \"1\", \"1\", \"1\", \"1\" ] }" );

    Response response;

    http_post(
            BLOCKS_URL,
            json,
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );

    // Delete just in case it worked.
    try {
        simpleDeleteBlock( block_id );
    } catch ( ... ) {
        // Ignore any error.
    }
}


BOOST_AUTO_TEST_CASE( test_midplane_wrong_type )
{
    string block_id( "testMidplaneType" );

    string json( "{ \"id\": \"" + block_id + "\", \"midplane\": 1, \"midplanes\": [ \"1\", \"1\", \"1\", \"1\" ] }" );

    Response response;

    http_post(
            BLOCKS_URL,
            json,
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );

    // Delete just in case it worked.
    try {
        simpleDeleteBlock( block_id );
    } catch ( ... ) {
        // Ignore any error.
    }
}


BOOST_AUTO_TEST_CASE( test_midplanes_empty )
{
    string block_id( "testMidplanesEmpty" );

    string json( "{ \"id\": \"" + block_id + "\", \"midplane\": \"R00-M0\", \"midplanes\": [] }" );

    Response response;

    http_post(
            BLOCKS_URL,
            json,
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );

    // Delete just in case it worked.
    try {
        simpleDeleteBlock( block_id );
    } catch ( ... ) {
        // Ignore any error.
    }
}


BOOST_AUTO_TEST_CASE( test_post_blocks_content_type_not_json )
{
    // If post to /bg/blocks and Content-Type header isn't set to application/json, then should fail with 415 Unsupported Media Type

    string block_id( "testPostInvalid" );

    Response response;

    string content_type( "application/x-www-form-urlencoded" );

    http_post(
            BLOCKS_URL,
            calc_create_single_midplane_block_json( block_id ),
            response,
            content_type
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::UnsupportedMediaType );

    // Delete just in case it worked.
    try {
        simpleDeleteBlock( block_id );
    } catch ( ... ) {
        // Ignore any error.
    }
}


BOOST_AUTO_TEST_CASE( test_post_blocks_content_type_with_charset )
{
    // If post to /bg/blocks and Content-Type has charset then shouldn't fail.

    string block_id( "testCreateContentTypeWithCharset" );

    Response response;

    string content_type( "application/json; charset=UTF-8" );

    http_post(
            BLOCKS_URL,
            calc_create_single_midplane_block_json( block_id ),
            response,
            content_type
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::Created );

    // Delete the block.
    simpleDeleteBlock( block_id );
}
