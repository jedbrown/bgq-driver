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

#include "BgwsClient.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


using bgws::command::BgwsClient;

using std::cerr;
using std::cout;
using std::exception;
using std::runtime_error;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws.command" );


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);
        vector<string> block_ids;

        BgwsClient bgws_client;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        po::options_description all_opts_desc;

        all_opts_desc.add_options()
                ( "id", po::value( &block_ids ), "Block ID" )
            ;

        po::options_description desc( "Options" );
        desc.add( bgws_client.getDesc() );
        desc.add_options()
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;
        logging_program_options.addTo( desc );

        all_opts_desc.add( desc );

        try {
            po::positional_options_description p;
            p.add( "id", -1 );

            po::variables_map vm;
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {
                cout << "Usage: " << argv[0] << " [OPTIONS] BLOCKID\n"
                     << "\n"
                        "Delete the block.\n"
                        "\n"
                        "Requires Delete authority to the block.\n"
                        "\n"
                     << desc << "\n";
                return 0;
            }

            if ( vm.count( "id" ) == 0 ) {
                BOOST_THROW_EXCEPTION( runtime_error( "must specify a block" ) );
            }
        } catch ( exception& e ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid arguments, " + e.what() + ". Use -h for help." ) );
        }


        bgws_client.notifyOptionsSet();

        bgq::utility::initializeLogging(
                *bgws_client.getBgProperties(),
                logging_program_options
            );

        bgws_client.notifyLoggingIsInitialized();


        BOOST_FOREACH( const string& block_id, block_ids ) {
            bgws_client.deleteBlock( block_id );
        }


    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return 1;
    }
}
