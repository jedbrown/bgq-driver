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
#define BOOST_TEST_MODULE corner

#include <boost/test/unit_test.hpp>

#include "common/Exception.h"
#include "common/SubBlock.h"

#include "test/common.h"

#include <bgq_util/include/Location.h>

#include <boost/bind.hpp>

#include <iomanip>

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

using namespace runjob;

BOOST_AUTO_TEST_CASE( garbage_corner )
{
    BOOST_CHECK_THROW(
            Corner( "asdf" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( not_a_core_or_node_corner )
{
    BOOST_CHECK_THROW(
            Corner( "R00-M0" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( node_number_too_large )
{
    Shape shape("1x1x1x1x1");
    BOOST_CHECK_THROW(
            Corner( "R00-M0-N00-J32" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( single_node_corners )
{
    for ( unsigned i = 0; i < 32; ++i ) {
        std::ostringstream corner;
        corner << "R00-M0-N00-J";
        corner << std::setfill('0') << std::setw(2) << i;
        BOOST_CHECK_NO_THROW(
                Corner( corner.str() );
            );
    }
}

BOOST_AUTO_TEST_CASE( single_core_corners )
{
    for ( unsigned node = 0; node < 32; ++node ) {
        for ( unsigned core = 0; core < 16; ++core ) {
            std::ostringstream corner;
            corner << "R00-M0-N00-J";
            corner << std::setfill('0') << std::setw(2) << node;
            corner << "-C";
            corner << std::setfill('0') << std::setw(2) << core;
            BOOST_CHECK_NO_THROW(
                    Corner( corner.str() )
                    );
        }
    }
}

BOOST_AUTO_TEST_CASE( invalid_core_corner )
{
    BOOST_CHECK_THROW(
            Corner( "R00-M0-N00-J00-C16" ),
            std::invalid_argument
            );
}
