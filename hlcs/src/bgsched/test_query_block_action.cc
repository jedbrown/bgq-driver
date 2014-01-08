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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#include <bgsched/core/core.h>
#include <bgsched/bgsched.h>

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <iostream>

LOG_DECLARE_FILE( "bgsched" );

typedef std::vector<std::string> Blocks;

class Impl
{
public:
    Impl(
            boost::asio::io_service& ios,
            const Blocks& blocks
        ) :
        _timer( ios ),
        _blocks( blocks )
    {
        this->wait();
    }

private:
    void wait()
    {
        _timer.expires_from_now(
                boost::posix_time::seconds(1)
                );
        _timer.async_wait(
                boost::bind(
                    &Impl::trigger,
                    this,
                    boost::asio::placeholders::error
                    )
                );
    }

    void trigger(
            const boost::system::error_code& error
            )
    {
        LOG_TRACE_MSG( __FUNCTION__ );
        if ( error ) {
            LOG_WARN_MSG( "could not wait: " << boost::system::system_error(error).what() );
            return;
        }

        this->query();

        this->wait();
    }

    void query()
    {
        BOOST_FOREACH( const std::string& block, _blocks ) {
            bgsched::BlockFilter filter;
            filter.setName( block );

            const bgsched::Block::Ptrs result(
                    bgsched::core::getBlocks( filter )
                    );
            BOOST_FOREACH( const bgsched::Block::Ptr& i, result ) {
                LOG_INFO_MSG( i->getName() << " action " << i->getAction().toValue() );
            }
        }
    }

private:
    boost::asio::deadline_timer _timer;
    const Blocks _blocks;
};

int
main(int argc, char** argv)
{
    Blocks blocks;
    namespace po = boost::program_options;
    po::options_description options;
    options.add_options()
        ("help,h", po::bool_switch(), "this help text")
        ("blocks", po::value(&blocks), "block IDs to query")
        ;

    // create positional arguments for block IDs
    po::positional_options_description positionalArgs;
    positionalArgs.add( "blocks", -1 );

    // add properties and verbose options
    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions lpo( "ibm.bgsched" );
    lpo.addTo( options );

    // parse
    po::variables_map vm;
    po::command_line_parser cmd_line( argc, argv );
    cmd_line.options( options );
    cmd_line.positional( positionalArgs );
    try {
        po::store( cmd_line.run(), vm );

        // notify variables_map that we are done processing options
        po::notify( vm );
    } catch ( const std::exception& e ) {
        std::cerr << e.what() << std::endl;
        exit( EXIT_FAILURE );
    }

    if ( vm["help"].as<bool>() ) {
        std::cout << argv[0] << " block1 [block2 ... blockn]" << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << options << std::endl;
        exit(EXIT_SUCCESS);
    }

    if ( blocks.empty() ) {
        std::cerr << "specify at least one block" << std::endl;
        exit( EXIT_FAILURE );
    }

    try {
        bgq::utility::Properties::Ptr properties;
        if ( !propertiesOptions.getFilename().empty() ) {
            properties = bgq::utility::Properties::create( propertiesOptions.getFilename() );
        } else {
            properties = bgq::utility::Properties::create();
        }

        // setup logging and bgsched
        initializeLogging( *properties, lpo );
        bgsched::init( properties->getFilename() );
    } catch ( const std::runtime_error& e ) {
        std::cerr << e.what();
        exit( EXIT_FAILURE );
    }

    boost::asio::io_service ios;
    Impl foo( ios, blocks );
    LOG_INFO_MSG( "starting" );
    ios.run();
}
