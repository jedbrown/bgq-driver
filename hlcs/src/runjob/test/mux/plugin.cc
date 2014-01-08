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
#define BOOST_TEST_MODULE plugin
#include <boost/test/unit_test.hpp>

#include "common/JobInfo.h"

#include "mux/client/PluginUpdateResources.h"

#include "test/mux/Fixture.h"

#include "hlcs/src/bgsched/runjob/VerifyImpl.h"

#include <bgq_util/include/Location.h>

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_FIXTURE_TEST_SUITE( plugin, Fixture )

BOOST_AUTO_TEST_CASE( override_corner )
{
    // user gives corner and shape
    runjob::JobInfo job;
    job.setCorner( runjob::Corner("R00-M0-N00-J00") );
    job.setShape( runjob::Shape("1x1x1x1x1") );

    // plugin overrides corner but not shape
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_corner = bgsched::runjob::Corner().coordinates( bgsched::runjob::Coordinates().a(0).b(0).c(0).d(0).e(0) );
    impl->_shape = bgsched::runjob::Shape().value( job.getSubBlock().shape());

    // corner should have not block coordinates yet since they come
    // from the plugin
    BOOST_CHECK_EQUAL(
            job.getSubBlock().corner().getBlockCoordinates().valid(),
            false
            );

    // update info with plugin contents
    bgsched::runjob::Verify plugin( impl );
    runjob::mux::client::PluginUpdateResources( job, plugin );

    BOOST_CHECK_EQUAL(
            job.getSubBlock().corner().getBlockCoordinates().valid(),
            true
            );
    BOOST_CHECK_EQUAL(
            std::string( job.getSubBlock().shape() ),
            "1x1x1x1x1"
            );
}

BOOST_AUTO_TEST_CASE( override_corner_array )
{
    // user gives corner and shape
    runjob::JobInfo job;
    job.setCorner( runjob::Corner("R00-M0-N00-J00") );
    job.setShape( runjob::Shape("1x1x1x1x1") );

    // plugin overrides corner but not shape
    const unsigned coordinates[5] = {0,0,0,0,0};
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_corner = bgsched::runjob::Corner( coordinates );
    impl->_shape = bgsched::runjob::Shape().value( job.getSubBlock().shape() );

    // corner should have not block coordinates yet since they come
    // from the plugin
    BOOST_CHECK_EQUAL(
            job.getSubBlock().corner().getBlockCoordinates().valid(),
            false
            );

    // update info with plugin contents
    bgsched::runjob::Verify plugin( impl );
    runjob::mux::client::PluginUpdateResources( job, plugin );

    BOOST_CHECK_EQUAL(
            job.getSubBlock().corner().getBlockCoordinates().valid(),
            true
            );
    BOOST_CHECK_EQUAL(
            std::string( job.getSubBlock().shape() ),
            "1x1x1x1x1"
            );
}

BOOST_AUTO_TEST_CASE( override_shape )
{
    // user gives corner and shape
    runjob::JobInfo job;
    job.setCorner( runjob::Corner("R00-M0-N00-J00") );
    job.setShape( runjob::Shape("1x1x1x1x1") );

    // plugin overrides shape but not corner
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_corner = bgsched::runjob::Corner().location( 
            boost::lexical_cast<std::string>( job.getSubBlock().corner())
            );
    impl->_shape = bgsched::runjob::Shape().a(2).b(2).c(2).d(2).e(2);

    // update info with plugin contents
    bgsched::runjob::Verify plugin( impl );
    runjob::mux::client::PluginUpdateResources( job, plugin );

    BOOST_CHECK_EQUAL(
            std::string( job.getSubBlock().shape() ),
            "2x2x2x2x2"
            );

    BOOST_CHECK_EQUAL(
            boost::lexical_cast<std::string>( job.getSubBlock().corner() ),
            "R00-M0-N00-J00"
            );
}

BOOST_AUTO_TEST_CASE( override_shape_array )
{
    // user gives corner and shape
    runjob::JobInfo job;
    job.setCorner( runjob::Corner("R00-M0-N00-J00") );
    job.setShape( runjob::Shape("1x1x1x1x1") );

    // plugin overrides shape but not corner
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_corner = bgsched::runjob::Corner().location( 
            boost::lexical_cast<std::string>( job.getSubBlock().corner() )
            );
    const unsigned shape[5] = {2,2,2,2,2};
    impl->_shape = bgsched::runjob::Shape(shape);

    // update info with plugin contents
    bgsched::runjob::Verify plugin( impl );
    runjob::mux::client::PluginUpdateResources( job, plugin );

    BOOST_CHECK_EQUAL(
            std::string( job.getSubBlock().shape() ),
            "2x2x2x2x2"
            );

    BOOST_CHECK_EQUAL(
            boost::lexical_cast<std::string>( job.getSubBlock().corner() ),
            "R00-M0-N00-J00"
            );
}

BOOST_AUTO_TEST_CASE( override_corner_and_shape )
{
    // user gives corner and shape
    runjob::JobInfo job;
    job.setCorner( runjob::Corner("R00-M0-N00-J00") );
    job.setShape( runjob::Shape("1x1x1x1x1") );

    // plugin overrides shape AND corner
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_corner = bgsched::runjob::Corner().coordinates( bgsched::runjob::Coordinates().a(0).b(0).c(0).d(0).e(0) );
    impl->_shape = bgsched::runjob::Shape().a(2).b(2).c(2).d(2).e(2);

    // update info with plugin content
    bgsched::runjob::Verify plugin( impl );
    runjob::mux::client::PluginUpdateResources( job, plugin );

    BOOST_CHECK_EQUAL(
            std::string( job.getSubBlock().shape() ),
            "2x2x2x2x2"
            );

    // location should have block coordinates
    BOOST_CHECK_EQUAL(
            job.getSubBlock().corner().getBlockCoordinates().valid(),
            true
            );

    // but coordinates should be valid
    BOOST_CHECK_EQUAL(
            job.getSubBlock().corner().isValid(),
            true
            );
}

BOOST_AUTO_TEST_CASE( override_corner_and_shape_array )
{
    // user gives corner and shape
    runjob::JobInfo job;
    job.setCorner( runjob::Corner("R00-M0-N00-J00") );
    job.setShape( runjob::Shape("1x1x1x1x1") );

    // plugin overrides shape AND corner
    const unsigned corner[5] = {0,0,0,0,0};
    bgsched::runjob::Verify::Pimpl impl(
            new bgsched::runjob::Verify::Impl
            );
    impl->_corner = bgsched::runjob::Corner( corner );
    const unsigned shape[5] = {2,2,2,2,2};
    impl->_shape = bgsched::runjob::Shape(shape);

    // update info with plugin contents
    bgsched::runjob::Verify plugin( impl );
    runjob::mux::client::PluginUpdateResources( job, plugin );

    BOOST_CHECK_EQUAL(
            std::string( job.getSubBlock().shape() ),
            "2x2x2x2x2"
            );

    // location should have block coordinates
    BOOST_CHECK_EQUAL(
            job.getSubBlock().corner().getBlockCoordinates().valid(),
            true
            );

    // but coordinates should be valid
    BOOST_CHECK_EQUAL(
            job.getSubBlock().corner().isValid(),
            true
            );
}

BOOST_AUTO_TEST_CASE( invalid_shape_size_zero )
{
    // 0 is not a valid shape size in any dimension
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().a(0),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().b(0),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().c(0),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().d(0),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().e(0),
            std::logic_error
            );
}

BOOST_AUTO_TEST_CASE( invalid_shape_size_three )
{
    // 0 is not a valid shape size in any dimension
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().a(3),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().b(3),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().c(3),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().d(3),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().e(3),
            std::logic_error
            );
}

BOOST_AUTO_TEST_CASE( invalid_shape_size_too_large )
{
    // 0 is not a valid shape size in any dimension
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().a(5),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().b(5),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().c(5),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().d(5),
            std::logic_error
            );
    BOOST_CHECK_THROW(
            bgsched::runjob::Shape().e(3),
            std::logic_error
            );
}

BOOST_AUTO_TEST_CASE( set_core )
{
    // all valid cores range from 0 to 15
    for ( unsigned i = 0; i < bgq::util::Location::ComputeCardCoresOnBoard - 1; ++i ) {
        runjob::JobInfo job;
        bgsched::runjob::Verify::Pimpl impl(
                new bgsched::runjob::Verify::Impl
                );

        impl->_shape = bgsched::runjob::Shape().core(i);
        BOOST_CHECK_EQUAL(
                impl->_shape.core(), i
                );
        BOOST_CHECK_EQUAL( impl->_shape.a(), 1 );
        BOOST_CHECK_EQUAL( impl->_shape.b(), 1 );
        BOOST_CHECK_EQUAL( impl->_shape.c(), 1 );
        BOOST_CHECK_EQUAL( impl->_shape.d(), 1 );
        BOOST_CHECK_EQUAL( impl->_shape.e(), 1 );


        // update info with plugin contents
        bgsched::runjob::Verify plugin( impl );
        runjob::mux::client::PluginUpdateResources( job, plugin );

        BOOST_CHECK_EQUAL(
                job.getSubBlock().corner().getCore(), i
                );
        BOOST_CHECK_EQUAL(
                job.getSubBlock().corner().isCoreSet(), 
                true
                );
    }
}

BOOST_AUTO_TEST_SUITE_END()
