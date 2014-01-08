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

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <cstdlib>
#include <stdio.h>


using bgws::command::BgwsClient;

using std::cerr;
using std::cout;
using std::exception;
using std::ostream;
using std::ostringstream;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws.command" );


static void listRas(
        const std::string& block,
        BGQDB::job::Id job_id,
        BgwsClient& bgws_client
    )
{
    json::ArrayValuePtr arr_val_ptr(bgws_client.getRas(
            block,
            job_id
        ));

    const json::Array &arr(arr_val_ptr->get());

    if ( arr.empty() ) {
        cout << "No RAS events.\n";
    }

    BOOST_FOREACH( const json::Array::value_type& ras_val_ptr, arr ) {
        const json::Object &ras_obj(ras_val_ptr->getObject());

        cout << ras_obj.as<int64_t>( "id" )
             << "  " << ras_obj.getString( "eventTime" )
             << (ras_obj.contains( "message" ) ? string() + "  " + ras_obj.getString( "message" ) : "")
             << "\n";
    }
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);

        BgwsClient bgws_client;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        string block;
        BGQDB::job::Id job_id(-1);

        po::options_description all_opts_desc;

        all_opts_desc.add_options()
            ;

        po::options_description filter_desc( "Filter options" );
        filter_desc.add_options()
                ( "block", po::value( &block ), "Block ID" )
                ( "job", po::value( &job_id ), "Job ID" )
            ;

        po::options_description desc( "Options" );

        desc.add_options()
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;
        logging_program_options.addTo( desc );

        desc.add( filter_desc );
        desc.add( bgws_client.getDesc() );

        all_opts_desc.add( desc );

        po::positional_options_description p;

        po::variables_map vm;

        try {
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {
                cout << "Usage: " << argv[0] << " [OPTIONS]\n"
                     << "\n"
                        "Prints a summary of RAS events.\n"
                        "\n"
                        "Requires hardware Read authority.\n"
                        "\n"
                     << desc << "\n";
                return EXIT_SUCCESS;
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

        listRas(
                block,
                job_id,
                bgws_client
            );

    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
