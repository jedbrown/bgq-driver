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
#define BOOST_TEST_MODULE mapping_file
#include <boost/test/unit_test.hpp>

#include "common/Exception.h"
#include "common/JobInfo.h"
#include "common/Mapping.h"
#include "common/properties.h"

#include "server/job/ValidateMappingFile.h"

#include "test/make_argv.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <sstream>

using namespace runjob::server;

struct MyFixture
{
    MyFixture() :
        _id( 0 ),
        _size( ),
        _info( )
    {
        _size.corner.a = 0;
        _size.corner.b = 0;
        _size.corner.c = 0;
        _size.corner.d = 0;
        _size.corner.e = 0;
        _size.shape.a = 0;
        _size.shape.b = 0;
        _size.shape.c = 0;
        _size.shape.d = 0;
        _size.shape.e = 0;

        _info.setNp( 1 );
    }

    BGQDB::job::Id _id;
    BG_JobCoords_t _size;
    runjob::JobInfo _info;
    std::stringstream _mapping;
};

struct InitializeLoggingFixture {
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_FIXTURE_TEST_CASE( comment, MyFixture )
{
    _mapping <<
        "# line one is a comment\n" <<
        "0 0 0 0 0 0"
        ;
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    BOOST_CHECK_NO_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size)
            );
}

BOOST_FIXTURE_TEST_CASE( comment_end_of_line, MyFixture )
{
    _mapping <<
        "0 0 0 0 0 0 # this is a comment"
        ;
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    BOOST_CHECK_NO_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size)
            );
}

BOOST_FIXTURE_TEST_CASE( a_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "1 0 0 0 0 0";
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( b_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 1 0 0 0 0";
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( c_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 0 1 0 0 0";
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( d_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 0 0 1 0 0";
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( e_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 0 0 0 1 0";
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( t_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 0 0 0 0 2";
    _info.setRanksPerNode( 1 );
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( negative_a_coordinates, MyFixture )
{
    _mapping << "-1 0 0 0 0 0";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( negative_b_coordinates, MyFixture )
{
    _mapping << "0 -1 0 0 0 0";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( negative_c_coordinates, MyFixture )
{
    _mapping << "0 0 -1 0 0 0";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( negative_d_coordinates, MyFixture )
{
    _mapping << "0 0 0 -1 0 0";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( negative_e_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 -1 0";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( negative_t_coordinates, MyFixture )
{
    _mapping << "0 0 0 0 0 -1";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( ranks_per_node, MyFixture )
{
    // iterate through all possible ranks-per-node values
    for( unsigned i = 1; i <= 64; i *= 2 ) {
        _info.setNp( i );
        _info.setRanksPerNode( i );
        _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

        // iterate through all possible T coordinates
        _mapping.clear();
        for ( unsigned j = 0; j < i; ++j ) {
            _mapping << "0 0 0 0 0 " << j << std::endl;
        }

        BOOST_CHECK_NO_THROW(
                job::ValidateMappingFile(_id, _info, _mapping, _size)
                );
    }
}

BOOST_FIXTURE_TEST_CASE( small_block_32_nodes, MyFixture )
{
    _info.setRanksPerNode( 2 );
    _info.setNp( 64 );
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 2;

    for( unsigned a = 0; a < 2; ++a ) {
        for( unsigned b = 0; b < 2; ++b ) {
            for( unsigned c = 0; c < 2; ++c ) {
                for( unsigned d = 0; d < 2; ++d ) {
                    for( unsigned e = 0; e < 2; ++e ) {
                        for( unsigned t = 0; t < 2; ++t ) {
                            _mapping << a << " " << b << " " << c << " " << d << " " << e << " " << t << "\n";
                        }
                    }
                }
            }
        }
    }

    BOOST_CHECK_NO_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size)
            );
}

BOOST_FIXTURE_TEST_CASE( missing_a_coordinates, MyFixture )
{
    _size.shape.a = 1;
    _size.shape.b = 2;
    _size.shape.c = 2;
    _size.shape.d = 1;
    _size.shape.e = 1;
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "\n";
    _mapping << "0 0 1 0 0 0\n";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( missing_b_coordinates, MyFixture )
{
    _size.shape.a = 1;
    _size.shape.b = 2;
    _size.shape.c = 2;
    _size.shape.d = 1;
    _size.shape.e = 1;
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 \n";
    _mapping << "0 0 1 0 0 0\n";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( missing_c_coordinates, MyFixture )
{
    _size.shape.a = 1;
    _size.shape.b = 2;
    _size.shape.c = 2;
    _size.shape.d = 1;
    _size.shape.e = 1;
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 \n";
    _mapping << "0 0 1 0 0 0\n";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( missing_d_coordinates, MyFixture )
{
    _size.shape.a = 1;
    _size.shape.b = 2;
    _size.shape.c = 2;
    _size.shape.d = 1;
    _size.shape.e = 1;
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 0 \n";
    _mapping << "0 0 1 0 0 0\n";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( missing_e_coordinates, MyFixture )
{
    _size.shape.a = 1;
    _size.shape.b = 2;
    _size.shape.c = 2;
    _size.shape.d = 1;
    _size.shape.e = 1;
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 0 0 \n";
    _mapping << "0 0 1 0 0 0\n";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( missing_t_coordinates, MyFixture )
{
    _size.shape.a = 1;
    _size.shape.b = 2;
    _size.shape.c = 2;
    _size.shape.d = 1;
    _size.shape.e = 1;
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 0 0 0 \n";
    _mapping << "0 0 1 0 0 0\n";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( duplicate_coordinates, MyFixture )
{
    _size.shape.a = 2;
    _size.shape.b = 2;
    _size.shape.c = 1;
    _size.shape.d = 1;
    _size.shape.e = 1;
    _mapping << "0 0 0 0 0 0\n";
    _mapping << "0 1 0 0 0 0\n";
    _mapping << "1 0 0 0 0 0\n";
    _mapping << "1 1 0 0 0 0\n";
    _mapping << "0 0 0 0 0 0\n"; // rank 4 duplicates rank 0

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( np_greater_than_mapping, MyFixture )
{
    _size.shape.a = 1;
    _size.shape.b = 1;
    _size.shape.c = 1;
    _size.shape.d = 1;
    _size.shape.e = 1;
    _info.setNp( 2 );
    _mapping << "0 0 0 0 0 0\n";

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _mapping, _size),
            runjob::Exception
            );
}
