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

#include <db/include/api/GenBlockParams.h>

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/throw_exception.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <string.h>


using BGQDB::Connectivity;
using BGQDB::Dimension;
using BGQDB::DimensionSpec;
using BGQDB::DimensionSpecs;
using BGQDB::GenBlockParams;

using bgws::command::BgwsClient;

using boost::bind;
using boost::numeric_cast;
using boost::lexical_cast;

using std::cerr;
using std::cout;
using std::exception;
using std::runtime_error;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws.command" );


static bool isMidplane( const std::string& s )
{
    return (s.size() == 6 && s[0] == 'R');
}


static bool isNodeBoard( const std::string& s )
{
    return (s.size() == 10 && s[0] == 'R');
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);

        bool args_type_set(false);
        BgwsClient::GenBlockParamsType::Value args_type(BgwsClient::GenBlockParamsType::LARGE);

        string midplane;
        DimensionSpecs dim_specs;

        string node_board_pos;
        uint8_t node_board_count(1);

        GenBlockParams::MidplaneLocations midplane_locations;
        string midplanes_connectivity( "ABCDE" );

        GenBlockParams::NodeBoardLocations node_board_locations;

        string block_id;
        string block_description;
        bool is_large(false), is_small(false), is_midplanes(false), is_node_boards(false);
        vector<string> args;
        GenBlockParams::MidplaneLocations passthrough_locations;

        BgwsClient bgws_client;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );


        po::options_description visible_desc( "Options" );
        visible_desc.add( bgws_client.getDesc() );
        visible_desc.add_options()
                ( "description", po::value( &block_description ), "Description" )
                ( "large,l", po::bool_switch( &is_large ), "Corner midplane and dimension specs" )
                ( "small", po::bool_switch( &is_small ), "Midplane, start node board, node board count" )
                ( "mps,m", po::bool_switch( &is_midplanes ), "Midplane locations" )
                ( "pts", po::value( &passthrough_locations )->multitoken(), "Passthrough locations" )
                ( "torus", po::value( &midplanes_connectivity )->default_value( "ABCDE" ), "Dimensions that are torus, the others are mesh" )
                ( "nbs,n", po::bool_switch( &is_node_boards ), "Node board locations" )
                ( "help,h", po::bool_switch( &help ), "Print help text" )
            ;
        logging_program_options.addTo( visible_desc );

        po::options_description all_opts_desc;
        all_opts_desc.add( visible_desc );
        all_opts_desc.add_options()
                ( "id", po::value( &block_id ), "Block ID" )
                ( "args", po::value( &args ), "Arguments for type" )
            ;

        po::positional_options_description p;
        p.add( "id", 1 );
        p.add( "args", -1 );

        po::variables_map vm;


        try {
            po::store( po::command_line_parser( argc, argv ).options( all_opts_desc ).positional( p ).run(), vm );
            po::notify( vm );

            if ( help ) {

                cout <<
"Usage: " << argv[0] << " [OPTIONS] BLOCKID (--large LARGE_PARAMS) | (--small SMALL_PARAMS) | (--mps MIDPLANE_PARAMS) | (--nbs NODE_BOARD_PARAMS)\n"
"\n"
"Creates a block.\n"
"\n"
"Requires block create authority.\n"
"\n"
"This command attempts to guess the block type from the arguments. You can typically provide\n"
"a) a list of midplanes for a large block,\n"
"b) a corner midplane and dimension specifications for a large block \n"
"c) a list of node boards for a small block,\n"
"d) a node board and count for a small block.\n"
"\n"
"Examples:\n"
"\n"
"Create a block called block1 on midplane R00-M0:\n"
"  " << argv[0] << " block1 R00-M0\n"
"\n"
"Create a block on midplanes R00-M0 and R00-M1:\n"
"  " << argv[0] << " block1 R00-M0 R00-M1\n"
"\n"
"Create a block on midplanes R00-M0 and R00-M1, passing through R01-M0 and R01-M1:\n"
"  " << argv[0] << " block1 R00-M0 R00-M1 --pts R01-M0 R01-M1\n"
"\n"
"Create a block on midplanes R00-M0 and R00-M1 that is a mesh in the D dimension:\n"
"  " << argv[0] << " block1 R00-M0 R00-M1 --torus ABC\n"
"\n"
"Create a block starting at midplane R00-M0 whose size is 2 in the D dimension:\n"
"  " << argv[0] << " block1 R00-M0 1 1 1 2\n"
"  " << argv[0] << " block1 R00-M0 1 1 1 :11\n"
"\n"
"Create a block starting at midplane R00-M0 whose size is 2 in the D dimension and is a mesh:\n"
"  " << argv[0] << " block1 R00-M0 1 1 1 2M\n"
"  " << argv[0] << " block1 R00-M0 1 1 1 M:11\n"
"\n"
"Create a block starting at R00-M0 that includes the next midplane in the D dimension and passes through the next two midplanes:\n"
"  " << argv[0] << " block1 R00-M0 1 1 1 :1100\n"
"\n"
"Create a block starting at midplane R00-M0 that uses the entire machine:\n"
"  " << argv[0] << " block1 R00-M0 0 0 0 0\n"
"\n"
"Create a small block on R00-M0-N00:\n"
"  " << argv[0] << " block1 R00-M0-N00\n"
"\n"
"Create a small block on R00-M0-N00 and R00-M0-N00:\n"
"  " << argv[0] << " block1 R00-M0-N00 R00-M0-N01\n"
"  " << argv[0] << " block1 R00-M0-N00 2\n"
"\n"
"Argument details:\n"
"\n"
"For --large, the arguments are CORNER_MIDPLANE A_DIMENSION_SPEC B_DIMENSION_SPEC C_DIMENSION_SPEC D_DIMENSION_SPEC.\n"
"The format for dimension specifications is : [<size>][<connectivity>][:<midplane_indicators>]\n"
" where\n"
" - <size> is optional and is the size in midplanes,\n"
"       the default is to use the midplane_indicators to specify the size.\n"
" - <connectivity> is optional and specifies whether the dimension\n"
"       is connected in a torus (T) or a mesh (M), the default is torus.\n"
" - :<midplane_indicators> is optional and specifies the midplanes\n"
"       to include in the dimension, where 1 indicates include\n"
"       and 0 indicates pass-through.\n"
"       The default is to include all midplanes\n"
"\n"
"For --small, the arguments are NODE_BOARD NODE_BOARD_COUNT.\n"
"\n"
"For --mps, the arguments are midplane locations.\n"
"    --pts specifies the passthrough locations.\n"
"    --torus specifies the torus dimensions.\n"
"\n"
"For --nbs, the arguments are node board locations.\n"
"\n"
<< visible_desc << "\n";

                return 0;
            }

            if ( vm.count( "id" ) == 0 ) {
                BOOST_THROW_EXCEPTION( runtime_error( "must specify a block ID" ) );
            }

            if ( is_large ) {
                if ( args_type_set )  BOOST_THROW_EXCEPTION( runtime_error( "can only have one size, either --large, --small, --mps, or --nbs" ) );
                args_type = BgwsClient::GenBlockParamsType::LARGE;
                args_type_set = true;
            }
            if ( is_small ) {
                if ( args_type_set )  BOOST_THROW_EXCEPTION( runtime_error( "can only have one size, either --large, --small, --mps, or --nbs" ) );
                args_type = BgwsClient::GenBlockParamsType::SMALL;
                args_type_set = true;
            }
            if ( is_midplanes ) {
                if ( args_type_set )  BOOST_THROW_EXCEPTION( runtime_error( "can only have one size, either --large, --small, --mps, or --nbs" ) );
                args_type = BgwsClient::GenBlockParamsType::MIDPLANES;
                args_type_set = true;
            }
            if ( is_node_boards ) {
                if ( args_type_set )  BOOST_THROW_EXCEPTION( runtime_error( "can only have one size, either --large, --small, --mps, or --nbs" ) );
                args_type = BgwsClient::GenBlockParamsType::NODE_BOARDS;
                args_type_set = true;
            }

            if ( ! args_type_set ) {
                // Try to figure out the type based on the arguments.
                if (
                        args.size() == 5 &&
                        isMidplane( args[0] ) &&
                        (! isMidplane( args[1] ))
                   )
                {
                    args_type = BgwsClient::GenBlockParamsType::LARGE;
                } else if (
                        args.size() == 2 &&
                        isNodeBoard( args[0] ) &&
                        (! isNodeBoard( args[1] ))
                    )
                {
                    args_type = BgwsClient::GenBlockParamsType::SMALL;
                } else if (
                        std::find_if( args.begin(), args.end(), ! bind( isMidplane, _1 ) ) == args.end()
                    )
                {
                    args_type = BgwsClient::GenBlockParamsType::MIDPLANES;
                } else if (
                        std::find_if( args.begin(), args.end(), ! bind( isNodeBoard, _1 ) ) == args.end()
                    )
                {
                    args_type = BgwsClient::GenBlockParamsType::NODE_BOARDS;
                }
            }

            // check midplanes_connectivity only set if is_midplanes and only contains "ABCDE":

            if ( midplanes_connectivity.find_first_not_of( "ABCDE" ) != string::npos ) {
                BOOST_THROW_EXCEPTION( runtime_error( "midplanes connectivity contains invalid dimension" ) );
            }

            if ( args_type == BgwsClient::GenBlockParamsType::MIDPLANES ) {
                dim_specs[Dimension::A] =
                        DimensionSpec( midplanes_connectivity.find( 'A' ) == string::npos ? Connectivity::Mesh : Connectivity::Torus );
                dim_specs[Dimension::B] =
                        DimensionSpec( midplanes_connectivity.find( 'B' ) == string::npos ? Connectivity::Mesh : Connectivity::Torus );
                dim_specs[Dimension::C] =
                        DimensionSpec( midplanes_connectivity.find( 'C' ) == string::npos ? Connectivity::Mesh : Connectivity::Torus );
                dim_specs[Dimension::D] =
                        DimensionSpec( midplanes_connectivity.find( 'D' ) == string::npos ? Connectivity::Mesh : Connectivity::Torus );
            }

            // check passthrough_midplanes only set if is_midplanes.

            switch ( args_type ) {
            case BgwsClient::GenBlockParamsType::LARGE:
                // go through the args and make sure contains corner midplane and dim specs.
                if ( args.size() != 5 ) {
                    BOOST_THROW_EXCEPTION( runtime_error( "wrong number of arguments for large block" ) );
                }

                midplane = args[0];
                dim_specs[Dimension::A] = DimensionSpec( args[1] );
                dim_specs[Dimension::B] = DimensionSpec( args[2] );
                dim_specs[Dimension::C] = DimensionSpec( args[3] );
                dim_specs[Dimension::D] = DimensionSpec( args[4] );
                break;
            case BgwsClient::GenBlockParamsType::SMALL:
                if ( args.size() != 2 ) {
                    BOOST_THROW_EXCEPTION( runtime_error( "wrong number of arguments for small block" ) );
                }

                midplane = args[0].substr( 0, string( "R00-M0" ).length() );
                node_board_pos = args[0].substr( string( "R00-M0" ).length() + 1 );
                node_board_count = numeric_cast<uint8_t>( lexical_cast<unsigned>( args[1] ) );
                break;
            case BgwsClient::GenBlockParamsType::MIDPLANES:
                midplane_locations = args;
                break;
            case BgwsClient::GenBlockParamsType::NODE_BOARDS:
                node_board_locations = args;
                break;
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


        if ( ! args_type_set ) {
            LOG_DEBUG_MSG( "Guessed type is " << args_type );
        }

        GenBlockParams gen_block_params;

        gen_block_params.setBlockId( block_id );

        switch ( args_type ) {
        case BgwsClient::GenBlockParamsType::LARGE:
            LOG_DEBUG_MSG( "midplane=" << midplane <<
                   " specs=" << dim_specs[Dimension::A].toString() <<
                         " " << dim_specs[Dimension::B].toString() <<
                         " " << dim_specs[Dimension::C].toString() <<
                         " " << dim_specs[Dimension::D].toString()
               );

            gen_block_params.setMidplane( midplane );
            gen_block_params.setDimensionSpecs( dim_specs );
            break;
        case BgwsClient::GenBlockParamsType::SMALL:
            LOG_DEBUG_MSG( "midplane=" << midplane << " nodeBoard=" << node_board_pos << " count=" << unsigned(node_board_count) );

            gen_block_params.setMidplane( midplane );
            gen_block_params.setNodeBoardAndCount( node_board_pos, node_board_count );
            break;
        case BgwsClient::GenBlockParamsType::MIDPLANES:
            LOG_DEBUG_MSG(
                    "midplanes.size=" << midplane_locations.size() << " pt.size=" << passthrough_locations.size() <<
                    " specs=" << dim_specs[Dimension::A].toString() <<
                          " " << dim_specs[Dimension::B].toString() <<
                          " " << dim_specs[Dimension::C].toString() <<
                          " " << dim_specs[Dimension::D].toString()
                );
            gen_block_params.setMidplanes(
                    midplane_locations,
                    passthrough_locations
                );
            gen_block_params.setDimensionSpecs( dim_specs );
            break;
        case BgwsClient::GenBlockParamsType::NODE_BOARDS:
            LOG_DEBUG_MSG( "node_boards.size=" << node_board_locations.size() );
            gen_block_params.setNodeBoardLocations( node_board_locations );
            break;
        }


        bgws_client.createBlock(
                gen_block_params,
                block_description,
                args_type
            );

        return 0;
    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return 1;
    }
}
