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

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include "utility/include/PluginHandle.h"

#include "SamplePlugin.h"

#include <boost/assign/list_of.hpp>

#include <boost/bind.hpp>

#include <vector>

using namespace bgq::utility;

typedef bgq::utility::PluginHandle<SamplePlugin> MyPlugin;

class InitializeLoggingFixture
{
public:
    InitializeLoggingFixture()
    {
        using namespace bgq::utility;
        bgq::utility::initializeLogging( *Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );

BOOST_AUTO_TEST_CASE( bad_path )
{
    const std::string path( "/hopefully/i/do/not/exist" );

    BOOST_CHECK_THROW(
            boost::cref( MyPlugin(path) ),
            MyPlugin::InvalidPath
            );
}

BOOST_AUTO_TEST_CASE( valid_path_bad_plugin )
{
    const std::string path( "/dev/null" );

    BOOST_CHECK_THROW(
            boost::cref( MyPlugin(path) ),
            MyPlugin::InvalidPath
            );
}


BOOST_AUTO_TEST_CASE( valid_plugin )
{
    const std::string path( "./libsample_plugin.so" );

    MyPlugin plugin( path );
    MyPlugin::PluginPtr handle = plugin.getPlugin();
    std::string something;
    handle->doSomething( something );
    BOOST_CHECK_EQUAL( something, "hello world" );
}
