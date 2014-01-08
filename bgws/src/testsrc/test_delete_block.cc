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

#include "capena-http/http/http.hpp"

#include <curl/curl.h>

#include <iostream>
#include <string>


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE bgws
#include <boost/test/unit_test.hpp>


using std::cout;
using std::string;


BOOST_GLOBAL_FIXTURE( CurlGlobalSetup );


//---------------------------------------------------------------------
// Test cases


BOOST_AUTO_TEST_CASE( test_delete_block_exists )
{
    string block_id( "testDeleteBlock" );

    {
        // First create a block to delete.
        Response response;

        http_post(
                BLOCKS_URL,
                calc_create_single_midplane_block_json( block_id ),
                response
            );

        BOOST_REQUIRE_EQUAL( response.getHttpStatus(), capena::http::Status::Created );
    }

    {
        // clean up, delete the block.

        Response response;

        http_delete(
                calc_block_url( block_id ),
                response
            );

        BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::NoContent );
        BOOST_CHECK_EQUAL( response.getContent(), "" );
    }
}


BOOST_AUTO_TEST_CASE( test_delete_block_invalid_id )
{
    string block_id( "b23456781012345678201234567830123" );

    Response response;

    http_delete(
            calc_block_url( block_id ),
            response
        );

    BOOST_CHECK_EQUAL( response.getHttpStatus(), capena::http::Status::BadRequest );
    BOOST_CHECK_EQUAL( response.getContentType(), CONTENT_TYPE_JSON );
    BOOST_CHECK_EQUAL( response.getContent(),

"{\n"
"  \"data\" : {\n"
"      \"blockId\" : \"" + block_id + "\"\n"
"    },\n"
"  \"id\" : \"invalidId\",\n"
"  \"operation\" : \"deleteBlock\",\n"
"  \"text\" : \"Could not delete '" + block_id + "' because the block ID is not valid.\"\n"
"}\n"

        );
}
