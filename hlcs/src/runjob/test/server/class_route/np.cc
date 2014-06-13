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
#define BOOST_TEST_MODULE np
#include <boost/test/unit_test.hpp>

#include "common/JobInfo.h"

#include "server/job/class_route/Mapping.h"
#include "server/job/class_route/Np.h"
#include "server/job/class_route/Rectangle.h"

#include "test/server/args/Fixture.h"
#include "test/make_argv.h"

#include <spi/include/mu/Classroute.h>

#include <fstream>
#include <string>

namespace runjob {
namespace server {

BOOST_FIXTURE_TEST_SUITE( np, Fixture )

BOOST_AUTO_TEST_CASE( max_np )
{
    std::string emptyMap = std::string();
    bool isPermuationMap = true;
    // --np for the entire job shape should result in no nodes excluded
    JobInfo info;
    info.setNp( 2*2*2*2*2 ); 
    info.setMapping( Mapping(Mapping::Type::Permutation, "ABCDET") );

    CR_RECT_T world;
    *CR_RECT_LL(&world) = (CR_COORD_T) {{0,0,0,0,0}};
    *CR_RECT_UR(&world) = (CR_COORD_T) {{1,1,1,1,1}};
    job::class_route::Rectangle rect( &world );
    job::class_route::Mapping mapping;

    const job::class_route::Np np( info, &rect, &mapping, emptyMap, isPermuationMap, false );

    // no nodes should be excluded
    BOOST_CHECK_EQUAL(
            np.includeCount(),
            rect.size()
            );
}

BOOST_AUTO_TEST_CASE( max_np_and_ranks_per_node )
{
    std::string emptyMap = std::string();
    bool isPermuationMap = true;
    bgq::utility::BoolAlpha ignore;
    // --ranks-per-node should not alter the include count when --np is not provided
    for ( unsigned int i=1; i<=64; i*=2 ) {
        // valid ranks per node 1, 2, 4, 8, 16, 32, 64
        JobInfo info;
        info.setRanksPerNode( i );
        info.setNp( 2*2*2*2*2*i ); 
        info.setMapping( Mapping(Mapping::Type::Permutation, "ABCDET") );
        job::class_route::Mapping mapping;

        CR_RECT_T world;
        *CR_RECT_LL(&world) = (CR_COORD_T) {{0,0,0,0,0}};
        *CR_RECT_UR(&world) = (CR_COORD_T) {{1,1,1,1,1}};
        job::class_route::Rectangle rect( &world );

        const job::class_route::Np np( info, &rect, &mapping, emptyMap, isPermuationMap, false );

        // no nodes should be excluded
        BOOST_CHECK_EQUAL(
                np.includeCount(),
                rect.size()
                );
    }
}

BOOST_AUTO_TEST_CASE( np_provided )
{
    std::string emptyMap = std::string();
    bool isPermuationMap = true;
    bgq::utility::BoolAlpha ignore;
    
    CR_RECT_T world;
    *CR_RECT_LL(&world) = (CR_COORD_T) {{0,0,0,0,0}};
    *CR_RECT_UR(&world) = (CR_COORD_T) {{1,1,1,1,1}};
    job::class_route::Rectangle rect( &world );

    // --np can range between 1 and the job size 
    for ( unsigned int i=1; i <= rect.size(); ++i ) {
        JobInfo info;
        info.setNp( i );
        info.setMapping( Mapping(Mapping::Type::Permutation, "ABCDET") );
        job::class_route::Mapping mapping;

        const job::class_route::Np np( info, &rect, &mapping, emptyMap, isPermuationMap, false );

        BOOST_CHECK_EQUAL(
                np.includeCount(),
                i
                );
    }
}

BOOST_AUTO_TEST_CASE( np_and_ranks_per_node_provided )
{
    std::string emptyMap = std::string();
    bool isPermuationMap = true;
    bgq::utility::BoolAlpha ignore;

    CR_RECT_T world;
    *CR_RECT_LL(&world) = (CR_COORD_T) {{0,0,0,0,0}};
    *CR_RECT_UR(&world) = (CR_COORD_T) {{1,1,1,1,1}};
    job::class_route::Rectangle rect( &world );

    // --ranks-per-node 1, 2, 4, 8, 16, 32, 64
    for ( unsigned int i=1; i <= 64; i*=2 ) {
        for ( unsigned j=1; j <= rect.size() * i; ++j ) {
            JobInfo info;
            info.setNp( j );
            info.setMapping( Mapping(Mapping::Type::Permutation, "ABCDET") );
            info.setRanksPerNode( i );
            job::class_route::Mapping mapping;

            const job::class_route::Np np( info, &rect, &mapping, emptyMap, isPermuationMap, false );

            // exclude count will be the number of nodes outside of --np
            // rounded down because fractional nodes will still be included
            const unsigned exclude_count = (rect.size() * i - j ) / i;
            BOOST_CHECK_EQUAL(
                    np.includeCount(),
                    rect.size() - exclude_count
                    );
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

} // server
} // runjob

