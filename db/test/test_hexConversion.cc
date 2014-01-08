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

#include "api/dataconv.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <iostream>

#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE hexConversion
#include <boost/test/unit_test.hpp>

class MyFixture
{
public:

    MyFixture() {
        bgq::utility::Properties::Ptr properties_ptr(bgq::utility::Properties::create());

        bgq::utility::initializeLogging( *properties_ptr );
    }

};

BOOST_GLOBAL_FIXTURE( MyFixture );

BOOST_AUTO_TEST_CASE( easy )
{
    unsigned char dest[1];
    const std::string input( "AB" );
    BOOST_CHECK_EQUAL( BGQDB::hexchar2bitdata( dest, sizeof(dest), input ), true );
    BOOST_CHECK_EQUAL( dest[0], 0xAB );
}

BOOST_AUTO_TEST_CASE( harder )
{
    const std::string input( "81850001041C570E0308C57054F93817" );
    std::vector<unsigned char> dest( input.size() / 2, ' ');
    BOOST_CHECK_EQUAL( BGQDB::hexchar2bitdata( &dest[0], dest.size(), input ), true );
    BOOST_CHECK_EQUAL( dest[0], 0x81 );  // 81
    BOOST_CHECK_EQUAL( dest[1], 0x85 );  // 85
    BOOST_CHECK_EQUAL( dest[2], 0x0 );   // 00
    BOOST_CHECK_EQUAL( dest[3], 0x1 );   // 01
    BOOST_CHECK_EQUAL( dest[4], 0x4 );   // 04
    BOOST_CHECK_EQUAL( dest[5], 0x1C );  // 1C
    BOOST_CHECK_EQUAL( dest[6], 0x57 );  // 57
    BOOST_CHECK_EQUAL( dest[7], 0xE );   // 0E
    BOOST_CHECK_EQUAL( dest[8], 0x3 );   // 03
    BOOST_CHECK_EQUAL( dest[9], 0x8 );   // 08
    BOOST_CHECK_EQUAL( dest[10], 0xc5 ); // C5
    BOOST_CHECK_EQUAL( dest[11], 0x70 ); // 70
    BOOST_CHECK_EQUAL( dest[12], 0x54 ); // 54
    BOOST_CHECK_EQUAL( dest[13], 0xF9 ); // F9
    BOOST_CHECK_EQUAL( dest[14], 0x38 ); // 38
    BOOST_CHECK_EQUAL( dest[15], 0x17 ); // 17
}


BOOST_AUTO_TEST_CASE( source_not_a_power_of_two  )
{
    unsigned char dest[1];
    const std::string input( "ABC" );
    BOOST_CHECK_EQUAL( BGQDB::hexchar2bitdata( dest, sizeof(dest), input ), false );
}

BOOST_AUTO_TEST_CASE( source_bigger_than_destination )
{
    unsigned char dest[1];
    const std::string input( "ABCD" );
    BOOST_CHECK_EQUAL( BGQDB::hexchar2bitdata( dest, sizeof(dest), input ), false );
}

BOOST_AUTO_TEST_CASE( non_hex_character )
{
    unsigned char dest[1];

    // need to check both position 0 and position 1
    {
        const std::string input( "AZ" );
        BOOST_CHECK_EQUAL( BGQDB::hexchar2bitdata( dest, sizeof(dest), input ), false );
    }
   
    {
        const std::string input( "ZA" );
        BOOST_CHECK_EQUAL( BGQDB::hexchar2bitdata( dest, sizeof(dest), input ), false );
    }
}


