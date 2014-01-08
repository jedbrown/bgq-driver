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

#include "server/job/CopyMappingFile.h"
#include "server/job/ValidateMappingFile.h"

#include "test/make_argv.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <fstream>

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

        _mapping.open( "my_mapping_file", std::ios_base::trunc | std::ios_base::out );
    }

    BGQDB::job::Id _id;
    BG_JobCoords_t _size;
    runjob::JobInfo _info;
    std::ofstream _mapping;
};

struct InitializeLoggingFixture {
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );


BOOST_FIXTURE_TEST_CASE( a_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "1 0 0 0 0 0";
    _mapping.flush();
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    _info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( b_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 1 0 0 0 0";
    _mapping.flush();
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    _info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( c_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 0 1 0 0 0";
    _mapping.flush();
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    _info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( d_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 0 0 1 0 0";
    _mapping.flush();
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    _info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( e_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 0 0 0 1 0";
    _mapping.flush();
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    _info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _size),
            runjob::Exception
            );
}

BOOST_FIXTURE_TEST_CASE( t_coordinates_too_large_for_job, MyFixture )
{
    _mapping << "0 0 0 0 0 2";
    _mapping.flush();
    _info.setRanksPerNode( 1 );
    _size.shape.a = _size.shape.b = _size.shape.c = _size.shape.d = _size.shape.e = 1;

    _info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );

    BOOST_CHECK_THROW(
            job::ValidateMappingFile(_id, _info, _size),
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

        _mapping.close();
        _mapping.open( "my_mapping_file", std::ios_base::trunc | std::ios_base::out );

        // iterate through all possible T coordinates
        for ( unsigned j = 0; j < i; ++j ) {
            _mapping << "0 0 0 0 0 " << j << std::endl;
        }

        _mapping.flush();

        _info.setMapping(
                runjob::Mapping(
                    runjob::Mapping::Type::File,
                    "my_mapping_file"
                    )
                );

        BOOST_CHECK_NO_THROW(
                job::ValidateMappingFile(_id, _info, _size)
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

    _mapping.flush();

    _info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                "my_mapping_file"
                )
            );

    BOOST_CHECK_NO_THROW(
            job::ValidateMappingFile(_id, _info, _size)
            );
}

