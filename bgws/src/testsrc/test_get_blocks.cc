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


using std::string;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE bgws
#include <boost/test/unit_test.hpp>


BOOST_GLOBAL_FIXTURE( CurlGlobalSetup );


BOOST_AUTO_TEST_CASE( test_get_block_summary_no_blocks )
{
    // Try get block summary when no blocks. Should get an empty object back.

    Response response;

    http_get(
            BLOCKS_URL,
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::OK );
    BOOST_CHECK_EQUAL( response.getContentType(), CONTENT_TYPE_JSON );

    BOOST_CHECK_EQUAL( response.getContent(), "{}\n" );
}


BOOST_AUTO_TEST_CASE( test_get_block_summary_one_block )
{
    string block_id("testGetBlocksOneBlock");

    // Set up: create block.
    {
        string data_to_send( string() + "{ \"id\": \"" + block_id + "\", \"midplane\": \"R00-M0\", \"midplanes\": [ \"1\", \"1\", \"1\", \"1\" ] }" );

        Response response;

        http_post(
                BLOCKS_URL,
                data_to_send,
                response
            );

        BOOST_REQUIRE_EQUAL( response.getHttpStatus(), capena::http::Status::Created );
    }

    {
        // Get block summary.

        Response response;

        http_get(
                BLOCKS_URL,
                response
            );

        BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::OK );
        BOOST_CHECK_EQUAL( response.getContentType(), CONTENT_TYPE_JSON );

        BOOST_CHECK_EQUAL( response.getContent(), string() +

"{\n"
"  \"" + block_id + "\" : {\n"
"      \"URI\" : \"\\/bg\\/blocks\\/" + block_id + "\",\n"
"      \"computeNodeCount\" : 512,\n"
"      \"description\" : \"Generated via web services\",\n"
"      \"ioNodeCount\" : 0,\n"
"      \"owner\" : \"\",\n"
"      \"status\" : \"Free\",\n"
"      \"statusSequenceId\" : 0,\n"
"      \"torus\" : \"ABCDE\"\n"
"    }\n"
"}\n"

             );
    }

    // clean up:
    simpleDeleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_get_block_details )
{
    string block_id("testGetBlockDetails");

    // Set up: create block.
    {
        Response response;

        http_post(
                BLOCKS_URL,
                calc_create_single_midplane_block_json( block_id ),
                response
            );

        BOOST_REQUIRE_EQUAL( response.getHttpStatus(), capena::http::Status::Created );
    }

    {
        // Get block details.

        Response response;

        http_get(
                calc_block_url( block_id ),
                response
            );

        BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::OK );
        BOOST_CHECK_EQUAL( response.getContentType(), CONTENT_TYPE_JSON );

        BOOST_CHECK_EQUAL( response.getContent(), string() +

"{\n"
"  \"bootOption\" : \"\",\n"
"  \"computeNodeCount\" : 512,\n"
"  \"description\" : \"Generated via web services\",\n"
"  \"id\" : \"testGetBlockDetails\",\n"
"  \"ioNodeCount\" : 0,\n"
"  \"jobCount\" : 0,\n"
"  \"microloaderImage\" : \"\\/bgsys\\/drivers\\/ppcfloor\\/boot\\/uloader\",\n"
"  \"midplanes\" : [ \"R00-M0\" ],\n"
"  \"nodeConfiguration\" : \"CNKDefault\",\n"
"  \"options\" : \"\",\n"
"  \"owner\" : \"\",\n"
"  \"status\" : \"Free\",\n"
"  \"statusSequenceId\" : 0,\n"
"  \"torus\" : \"ABCDE\"\n"
"}\n"

             );
    }

    // clean up:
    simpleDeleteBlock( block_id );
}


BOOST_AUTO_TEST_CASE( test_get_block_details_no_block )
{
    Response response;

    http_get(
            calc_block_url( "notablock" ),
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::NotFound );
    BOOST_CHECK_EQUAL( response.getContentType(), CONTENT_TYPE_JSON );

    BOOST_CHECK_EQUAL( response.getContent(), string() +

"{\n"
"  \"data\" : {\n"
"      \"blockId\" : \"notablock\"\n"
"    },\n"
"  \"id\" : \"notFound\",\n"
"  \"operation\" : \"getBlockDetails\",\n"
"  \"text\" : \"Could not get details for block 'notablock' because the block does not exist.\"\n"
"}\n"


         );

}
