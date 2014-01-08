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

#include "src/api/utility.h"

#include <boost/foreach.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#define BOOST_TEST_MODULE insert_colons


BOOST_AUTO_TEST_SUITE( insert_colons )


BOOST_AUTO_TEST_CASE( empty_string )
{
    std::string empty;
    BOOST_CHECK_THROW(
            BGQDB::insert_colons(empty),
            std::invalid_argument
            );
    BOOST_CHECK_EQUAL(
            empty.size(),
            0u
            );
}

BOOST_AUTO_TEST_CASE( odd_size_string )
{
    std::string odd("abcde");
    BOOST_CHECK_EQUAL(
            odd.size() % 2,
            1u
            );
    BOOST_CHECK_THROW(
            BGQDB::insert_colons(odd),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( even_size_string )
{
    std::string even("abcdefghijklmnopqrstuvwxyz");
    std::string::size_type size = even.size();
    BOOST_CHECK_EQUAL(
            even.size() % 2,
            0u
            );

    // ensure inserting colons doesn't throw
    BOOST_CHECK_NO_THROW(
            BGQDB::insert_colons(even)
            );


    // ensure new size is what we expect
    BOOST_CHECK_EQUAL(
            even.size(),
            size + size / 2 - 1
            );

    // ensure every third token is a colon
    uint32_t count = 0;
    BOOST_FOREACH(char c, even) {
        if (++count == 3) {
            BOOST_CHECK_EQUAL(c, ':');
            count = 0;
        }
    }

}

BOOST_AUTO_TEST_SUITE_END()

