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
#define BOOST_TEST_MODULE mapping

#include "common/Mapping.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/test/unit_test.hpp>
#include <boost/scoped_array.hpp>

#include <iostream>
#include <fstream>

struct MyFixture
{
    MyFixture()
    {
        _mapping.open( "my_mapping_file", std::ios_base::trunc | std::ios_base::out );
    }

    std::ofstream _mapping;
};

struct InitializeLoggingFixture {
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_FIXTURE_TEST_CASE( blank_line, MyFixture )
{
    _mapping <<
        "# line one is a comment\n" <<
        "\n" // line two is blank
        "0 0 0 0 0 0\n"
        "\n" // line four is blank
        ;
    _mapping.flush();

    BOOST_CHECK_NO_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );
}

BOOST_FIXTURE_TEST_CASE( comment, MyFixture )
{
    _mapping <<
        "# line one is a comment\n" <<
        "0 0 0 0 0 0"
        ;
    _mapping.flush();

    BOOST_CHECK_NO_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );
}

BOOST_FIXTURE_TEST_CASE( comment_end_of_line, MyFixture )
{
    _mapping <<
        "0 0 0 0 0 0 # this is a comment"
        ;
    _mapping.flush();

    BOOST_CHECK_NO_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );
}

BOOST_FIXTURE_TEST_CASE( negative_a_coordinates, MyFixture )
{
    _mapping << "-1 0 0 0 0 0";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( negative_b_coordinates, MyFixture )
{
    _mapping << "0 -1 0 0 0 0";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( negative_c_coordinates, MyFixture )
{
    _mapping << "0 0 -1 0 0 0";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( negative_d_coordinates, MyFixture )
{
    _mapping << "0 0 0 -1 0 0";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( negative_e_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 -1 0";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( negative_t_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 -1";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( missing_a_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 0\n";
    _mapping << " \n";
    _mapping << "0 0 1 0 0 0\n";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( missing_b_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 \n";
    _mapping << "0 0 1 0 0 0\n";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( missing_c_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 \n";
    _mapping << "0 0 1 0 0 0\n";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( missing_d_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 0 \n";
    _mapping << "0 0 1 0 0 0\n";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( missing_e_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 0 0 \n";
    _mapping << "0 0 1 0 0 0\n";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( missing_t_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 0 0 0 \n";
    _mapping << "0 0 1 0 0 0\n";
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}

BOOST_FIXTURE_TEST_CASE( duplicate_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 0 0 0 0\n";
    _mapping << "1 0 0 0 0 0\n";
    _mapping << "1 1 0 0 0 0\n";
    _mapping << "0 0 0 0 0 0\n"; // rank 4 duplicates rank 0
    _mapping.flush();

    BOOST_CHECK_THROW(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                ),
            std::logic_error
            );
}


