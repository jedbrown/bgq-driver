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
#define BOOST_TEST_MODULE shape

#include <boost/test/unit_test.hpp>

#include "common/Exception.h"
#include "common/Shape.h"

#include "test/common.h"

#include <boost/bind.hpp>

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

using namespace runjob;

BOOST_AUTO_TEST_CASE( too_many_tokens )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x1x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( missing_B_C_D_E )
{
    BOOST_CHECK_THROW(
            Shape( "1x" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( missing_C_D_E )
{
    BOOST_CHECK_THROW(
            Shape( "1x2" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( missing_D_E )
{
    BOOST_CHECK_THROW(
            Shape( "1x2x2" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( missing_E )
{
    BOOST_CHECK_THROW(
            Shape( "1x2x2x3" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( all_zeros )
{
    BOOST_CHECK_THROW(
            Shape( "0x0x0x0x0" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( zero_A )
{
    BOOST_CHECK_THROW(
            Shape( "0x1x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( zero_B )
{
    BOOST_CHECK_THROW(
            Shape( "1x0x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( zero_C )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x0x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( zero_D )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x1x0x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( zero_E )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x1x1x0" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( max_A )
{
    BOOST_CHECK_THROW(
            Shape( "5x1x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( max_B )
{
    BOOST_CHECK_THROW(
            Shape( "1x5x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( max_C )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x5x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( max_D )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x1x5x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( max_E )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x1x1x3" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( negative_A )
{
    BOOST_CHECK_THROW(
            Shape( "-5x1x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( negative_B )
{
    BOOST_CHECK_THROW(
            Shape( "1x-4x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( negative_C )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x-3x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( negative_D )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x1x-2x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( negative_E )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x1x1x-11" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( three_A )
{
    BOOST_CHECK_THROW(
            Shape( "3x1x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( three_B )
{
    BOOST_CHECK_THROW(
            Shape( "1x3x1x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( three_C )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x3x1x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( three_D )
{
    BOOST_CHECK_THROW(
            Shape( "1x1x1x3x1" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( all_valid_shapes )
{
    for ( size_t a = 1; a <= 4; a *= 2 ) {
        for ( size_t b = 1; b <= 4; b *= 2) {
            for ( size_t c = 1; c <= 4; c *= 2) {
                for ( size_t d = 1; d <= 4; d *= 2) {
                    for ( size_t e = 1; e <= 2; ++e ) {
                        std::ostringstream shape;
                        shape << a << "x" << b << "x" << c << "x" << d << "x" << e;
                        BOOST_CHECK_NO_THROW(
                                Shape( shape.str() )
                                );
                    }
                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE( empty_string_conversion )
{
    Shape foo;
    std::string bar(foo);
    BOOST_CHECK_EQUAL(
            bar.empty(),
            true
            );
}

BOOST_AUTO_TEST_CASE( non_empty_string_conversion )
{
    const std::string shape( "2x2x2x2x2" );
    Shape foo( shape );
    BOOST_CHECK_EQUAL(
            std::string(foo),
            shape
            );
}
