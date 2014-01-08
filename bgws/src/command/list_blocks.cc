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


string blockStatusCodeToText( const std::string & s )
{
    if ( s == "F" )  return "Free";
    if ( s == "A" )  return "Allocated";
    if ( s == "B" )  return "Booting";
    if ( s == "I" )  return "Initialized";
    if ( s == "T" )  return "Terminating";
    return string() + "Unknown(" + s + ")";
}


static void printBlockSummary(
        const json::Array& blocks,
        ostream& os
    )
{
    if ( blocks.empty() ) {
        os << "No blocks.\n";
        return;
    }

    unsigned block_field_width(6);
    unsigned user_field_width(5);
    static const unsigned STATUS_COL_WIDTH(12);

    BOOST_FOREACH( const json::Array::value_type& block_val_ptr, blocks ) {
        const json::Object &block_obj( block_val_ptr->getObject() );
        block_field_width = std::max( block_field_width, unsigned(block_obj.getString( "id" ).size()) + 1 );
        user_field_width = std::max( user_field_width, unsigned(block_obj.contains( "user" ) ? block_obj.getString( "user" ).size() + 1 : 0) );
    }

    os << std::setw(block_field_width) << std::left << "Block" << " " << std::setw(STATUS_COL_WIDTH) << std::left << "Status" << " " << "User" << "\n";
    os << std::setw(block_field_width) << std::setfill( '-' ) << "" << " " << std::setw(STATUS_COL_WIDTH) << std::setfill( '-' ) << "" << " " << std::setw(user_field_width) << std::setfill( '-' ) << "" << "\n";
    os << std::setfill( ' ' );

    BOOST_FOREACH( const json::Array::value_type& block_val_ptr, blocks ) {
        const json::Object &block_obj( block_val_ptr->getObject() );

        os << std::setw(block_field_width) << std::left << block_obj.getString( "id" ) << " "
                << std::setw(STATUS_COL_WIDTH) << blockStatusCodeToText( block_obj.getString( "status" ) )
           << (block_obj.contains( "user" ) ? (string() + " " + block_obj.getString( "user" )) : "")
           << "\n";
    }
}


static void printBlockDetails(
        const json::Object& block,
        ostream& os
    )
{
    boost::format NQ_LINE_FMT( "%1%: %2%\n" );
    boost::format Q_LINE_FMT( "%1%: '%2%'\n" );

    os <<
str( NQ_LINE_FMT % "Status" % blockStatusCodeToText( block.getString( "status" ) ) );

if ( block.contains( "user" ) ) {
    os <<
str( NQ_LINE_FMT % "User" % block.getString( "user" ) );
}

    if ( block.contains( "computeNodeCount" ) ) {
        os <<
str( NQ_LINE_FMT % "Compute nodes" % block.getDouble( "computeNodeCount" ) ) <<
str( NQ_LINE_FMT % "Shape" % block.getString( "shape" ) ) <<
str( NQ_LINE_FMT % "Job count" % block.getDouble( "jobCount" ) );
    }
    if ( block.contains( "ioNodeCount" ) ) {
        os <<
str( NQ_LINE_FMT % "I/O nodes" % block.getDouble( "ioNodeCount" ) );
    }

    os <<
str( Q_LINE_FMT % "Description" % block.getString( "description" ) );

    if ( block.contains( "bootOptions" ) )  os << str( Q_LINE_FMT % "Boot options" % block.getString( "bootOptions" ) );
    if ( block.contains( "microloaderImage" ) )  os << str( Q_LINE_FMT % "Microloader image" % block.getString( "microloaderImage" ) );

    os <<
str( Q_LINE_FMT % "Node configuration" % block.getString( "nodeConfiguration" ) );

    if ( block.contains( "options" ) ) os << str( Q_LINE_FMT % "Options" % block.getString( "options" ) );

    if ( block.contains( "torus" ) ) {
        string torus_dimensions_str(block.getString( "torus" ));
        if ( torus_dimensions_str.empty() )  torus_dimensions_str = "none";

        os << str( NQ_LINE_FMT % "Torus dimensions" % torus_dimensions_str );
    }

    if ( block.contains( "midplanes" ) ) {

        os << boost::format( "%1%:\n" ) % "Midplanes";

        BOOST_FOREACH( const json::Array::value_type& mp_val_ptr, block.getArray( "midplanes" ) ) {
            os << boost::format( "\t%1%\n" ) % mp_val_ptr->getString();
        }

        if ( block.contains( "passthrough" ) ) {

            os << boost::format( "%1%:\n" ) % "Passthrough midplanes";

            BOOST_FOREACH( const json::Array::value_type& mp_val_ptr, block.getArray( "passthrough" ) ) {
                os << boost::format( "\t%1%\n" ) % mp_val_ptr->getString();
            }

        }

    } else if ( block.contains( "nodeBoards" ) ) {

        os << boost::format( "%1%:\n" ) % "Node boards";

        BOOST_FOREACH( const json::Array::value_type& nb_val_ptr, block.getArray( "nodeBoards" ) ) {
            os << boost::format( "\t%1%-%2%\n" ) % block.getString( "midplane" ) % nb_val_ptr->getString();
        }

    } else if ( block.contains( "locations" ) ) {

        os << boost::format( "%1%:\n" ) % "Locations";

        BOOST_FOREACH( const json::Array::value_type& loc_val_ptr, block.getArray( "locations" ) ) {
            os << boost::format( "\t%1%\n" ) % loc_val_ptr->getString();
        }

    }

    os << "\n";
}


static void listBlocks(
        const string& block_id,
        bgws::common::blocks_query::Type block_type,
        const string& status,
        BgwsClient& bgws_client
    )
{
    if ( block_id.empty() ) {

        json::ArrayValuePtr arr_val_ptr(bgws_client.getBlocksSummary( status, block_type ));

        const json::Array &arr(arr_val_ptr->get());

        printBlockSummary( arr, cout );

    } else {

        json::ObjectValuePtr obj_val_ptr(bgws_client.getBlockDetails( block_id ));

        const json::Object &obj(obj_val_ptr->get());

        printBlockDetails( obj, cout );

    }
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);
        string block_id;
        bool io_blocks(false);
        string status;

        BgwsClient bgws_client;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );

        po::options_description all_opts_desc;

        all_opts_desc.add_options()
                ( "id", po::value<string>( &block_id ), "Block ID for block details" )
            ;

        po::options_description filter_desc( "Filter options" );
        filter_desc.add_options()
                ( "status", po::value( &status ), "Block status" )
                ( "io", po::bool_switch( &io_blocks ), "I/O blocks" )
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
        p.add( "id", 1 );

        po::variables_map vm;

        try {
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {
                cout << "Usage: " << argv[0] << " [OPTIONS] [BLOCKID]\n"
                     << "\n"
                        "Prints a summary of all blocks, or details for a single block.\n"
                        "\n"
                        "Requires Read authority to the block.\n"
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

        bgws::common::blocks_query::Type block_type(io_blocks ? bgws::common::blocks_query::Type::Io : bgws::common::blocks_query::Type::Compute);

        listBlocks(
                block_id,
                block_type,
                status,
                bgws_client
            );

    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
