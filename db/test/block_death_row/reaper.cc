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
#include <db/include/api/genblock.h>

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>

LOG_DECLARE_FILE( "database" );

bgq::utility::Properties::ConstPtr
openProperties(
        const bgq::utility::Properties::ProgramOptions& options,
        const bgq::utility::LoggingProgramOptions& lpo
        )
{
    using namespace bgq::utility;

    Properties::Ptr properties;
    try {
        if ( !options.getFilename().empty() ) {
            properties = Properties::create( options.getFilename() );
        } else {
            properties = Properties::create();
        }

        // setup logging
        initializeLogging( *properties, lpo );
    } catch ( const std::runtime_error& e ) {
        std::cerr << e.what();
        exit( EXIT_FAILURE );
    }

    return properties;
}

void
small(
        const unsigned count,
        const bgq::utility::UserId& uid
     )
{
    LOG_INFO_MSG( "creating and deleting " << count << " small blocks" );
    for ( unsigned int i = 0; i < count; ++i ) {
        const std::string midplane = "R00-M0";
        const std::string board( "N00" );
        const std::string name( midplane + "-" + board );
        const unsigned nodes = 32;

        BGQDB::genSmallBlock( name, midplane, nodes, board, uid.getUser() );
        BGQDB::deleteBlock( name );
        if ( ! (i%10) ) {
            LOG_INFO_MSG( "done with " << i << " small blocks" );
        }
    }
}

void
midplane(
        const unsigned count,
        const bgq::utility::UserId& uid
     )
{
    LOG_INFO_MSG( "creating and deleting " << count << " midplane blocks" );
    for ( unsigned int i = 0; i < count; ++i ) {
        const std::string midplane = "R00-M0";

        BGQDB::genBlock( midplane, midplane, uid.getUser() );
        BGQDB::deleteBlock( midplane );
        if ( ! (i%10) ) {
            LOG_INFO_MSG( "done with " << i << " midplane blocks" );
        }
    }
}

void
full(
        const unsigned count,
        const bgq::utility::UserId& uid
     )
{
    LOG_INFO_MSG( "creating and deleting " << count << " full system blocks" );
    for ( unsigned int i = 0; i < count; ++i ) {
        const std::string name = "full";

        BGQDB::genFullBlock( name, uid.getUser() );
        BGQDB::deleteBlock( name );
        if ( ! (i%10) ) {
            LOG_INFO_MSG( "done with " << i << " full system blocks" );
        }
    }
}

int
main(int argc, char** argv)
{
    unsigned numSmallBlocks = 0;
    unsigned numMidplaneBlocks = 0;
    unsigned numFullSystemBlocks = 0;
    namespace po = boost::program_options;
    po::options_description options;
    options.add_options()
        ("help,h", po::bool_switch(), "this help text")
        ("small", po::value(&numSmallBlocks)->default_value(0), "number of small blocks to create")
        ("midplane", po::value(&numMidplaneBlocks)->default_value(0), "number of midplane blocks to create")
        ("full", po::value(&numFullSystemBlocks)->default_value(0), "number of full system blocks to create")
        ;

    po::positional_options_description positionalArgs;
    positionalArgs.add( "small", 1 );
    positionalArgs.add( "midplane", 1 );
    positionalArgs.add( "full", 1 );

    // add properties and verbose options
    bgq::utility::Properties::ProgramOptions propertiesOptions;
    propertiesOptions.addTo( options );
    bgq::utility::LoggingProgramOptions lpo( "ibm.database" );
    lpo.addTo( options );

    po::options_description both;
    both.add( options );

    // parse
    po::variables_map vm;
    po::command_line_parser cmd_line( argc, argv );
    cmd_line.options( both );
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
        std::cout << argv[0] << " numblocks" << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << options << std::endl;
        exit(EXIT_SUCCESS);
    }
    const bgq::utility::Properties::ConstPtr properties( 
            openProperties(propertiesOptions, lpo)
            );
    if ( !numSmallBlocks && !numMidplaneBlocks && !numFullSystemBlocks ) {
        std::cerr << "specify one of --small --midplane or --full" << std::endl;
        exit( EXIT_FAILURE );
    } else if ( numSmallBlocks < 0 ) {
        std::cerr << "specify a positive number of small blocks to create" << std::endl;
        exit( EXIT_FAILURE );
    } else if ( numMidplaneBlocks < 0 ) {
        std::cerr << "specify a positive number of midplane blocks to create" << std::endl;
        exit( EXIT_FAILURE );
    } else if ( numFullSystemBlocks < 0 ) {
        std::cerr << "specify a positive number of full system blocks to create" << std::endl;
        exit( EXIT_FAILURE );
    }

    BGQDB::init( properties, 1 );
    const bgq::utility::UserId uid;

    small( numSmallBlocks, uid );
    midplane( numMidplaneBlocks, uid);
    full( numFullSystemBlocks, uid);

    LOG_INFO_MSG( "done" );
}


