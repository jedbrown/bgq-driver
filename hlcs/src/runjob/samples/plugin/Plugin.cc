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
#include "Plugin.h"
#include "ProcessTree.h"

#include <bgsched/bgsched.h>

#include <algorithm>
#include <iostream>
#include <iterator>

#include <boost/foreach.hpp>

Plugin::Plugin() :
    bgsched::runjob::Plugin(),
    _mutex()
{
    std::cout << "Hello from sample runjob plugin ctor" << std::endl;
    std::cout << "major version: " << bgsched::version::major << std::endl;
    std::cout << "minor version: " << bgsched::version::minor << std::endl;
    std::cout << "mod version:   " << bgsched::version::mod << std::endl;
    std::cout << "driver:        " << bgsched::version::driver << std::endl;
}

Plugin::~Plugin()
{
    std::cout << "Goodbye from sample runjob plugin dtor" << std::endl;
}

void
Plugin::execute(
        bgsched::runjob::Verify& verify
        )
{
    boost::lock_guard<boost::mutex> lock( _mutex );

    std::cout << "starting job from pid " << verify.pid() << std::endl;
    std::cout << "for user " << verify.user().uid() << std::endl;
    std::cout << "executable: " << verify.exe() << std::endl;
    std::cout << "args      : ";
    std::copy( verify.args().begin(), verify.args().end(), std::ostream_iterator<std::string>(std::cout, " ") );
    std::cout << std::endl;
    std::cout << "envs      : ";
    std::copy( verify.envs().begin(), verify.envs().end(), std::ostream_iterator<std::string>(std::cout, " ") );
    std::cout << std::endl;
    std::cout << "block     : " << verify.block() << std::endl;
    if ( !verify.corner().location().empty() ) {
        std::cout << "corner:     " << verify.corner().location() << std::endl;
    }

    const std::string& shape = verify.shape().value();
    if ( !shape.empty() ) {
        std::cout << "shape:      " << shape << std::endl;
    }

    const ProcessTree tree( verify.pid() );
    std::cout << tree << std::endl;

    return;
}

void
Plugin::execute(
        const bgsched::runjob::Started& data
        )
{
    boost::lock_guard<boost::mutex> lock( _mutex );
    std::cout << "runjob " << data.pid() << " started with ID " << data.job() << std::endl;
}

void
Plugin::execute(
        const bgsched::runjob::Terminated& data
        )
{
    boost::lock_guard<boost::mutex> lock( _mutex );
    std::cout << "runjob " << data.pid() << " shadowing job " << data.job() << " finished with status " << data.status() << std::endl;

    std::cout << "kill timeout: " << std::boolalpha << data.kill_timeout() << std::endl;

    // output failed nodes
    const bgsched::runjob::Terminated::Nodes& nodes = data.software_error_nodes();
    if ( !nodes.empty() ) {
        std::cout << nodes.size() << " failed nodes" << std::endl;
        BOOST_FOREACH( const auto& i, data.software_error_nodes() ) {
            std::cout << i.location() << ": " << i.coordinates() << std::endl;
        }
    } else {
        std::cout << "no failed nodes" << std::endl;
    }
}

extern "C"
{

bgsched::runjob::Plugin*
create()
{
    return new Plugin();
}

void
destroy(bgsched::runjob::Plugin* p)
{
    delete p;
}

} // extern "C"
