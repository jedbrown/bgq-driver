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

#include "bgws.hpp"
#include "BgwsOptions.hpp"
#include "RequestData.hpp"
#include "Response.hpp"
#include "utility.hpp"

#include "capena-http/http/http.hpp"
#include "capena-http/http/uri/Path.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/GenBlockParams.h>

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/scope_exit.hpp>
#include <boost/throw_exception.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <curl/curl.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <string.h>


using namespace bgws_clients;

using BGQDB::Connectivity;
using BGQDB::Dimension;
using BGQDB::DimensionSpec;
using BGQDB::DimensionSpecs;
using BGQDB::GenBlockParams;

using boost::bind;
using boost::numeric_cast;
using boost::lexical_cast;

using std::cerr;
using std::cout;
using std::exception;
using std::runtime_error;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "bgws.clients" );


enum ArgsType {
    LARGE,
    SMALL,
    MIDPLANES,
    NODE_BOARDS
};


static bool isMidplane( const std::string& s )
{
    return (s.size() == 6 && s[0] == 'R');
}


static bool isNodeBoard( const std::string& s )
{
    return (s.size() == 10 && s[0] == 'R');
}


static json::ValuePtr genBlockParamsToJson(
        const GenBlockParams& gen_block_params,
        const std::string& description,
        ArgsType args_type
    )
{
    json::ObjectValuePtr object_value_ptr(json::Object::create());
    json::Object &obj(object_value_ptr->get());
    obj.set( "id", gen_block_params.getBlockId() );

    if ( ! description.empty() ) {
        obj.set( "description", description );
    }

    if ( gen_block_params.isLarge() ) {

        const DimensionSpecs &dim_specs(gen_block_params.getDimensionSpecs());

        if ( gen_block_params.isLargeWithMidplanes() ) {
            json::Array &mps_arr(obj.createArray( "midplanes" ) );

            const GenBlockParams::MidplaneLocations &mp_locs(gen_block_params.getMidplanes());

            BOOST_FOREACH ( const string& mp_loc , mp_locs ) {
                mps_arr.add( mp_loc );
            }

            const GenBlockParams::MidplaneLocations &pt_locs(gen_block_params.getPassthroughLocations());

            if ( ! pt_locs.empty() ) {
                json::Array &pts_arr(obj.createArray( "passthrough" ) );

                BOOST_FOREACH ( const string& pt_loc , pt_locs ) {
                    pts_arr.add( pt_loc );
                }
            }
        } else {
            obj.set( "midplane", gen_block_params.getMidplane() );

            json::Array &mps_arr(obj.createArray( "midplanes" ));
            mps_arr.add( DimensionSpec::midplanesToStr( dim_specs[Dimension::A].getIncludedMidplanes() ) );
            mps_arr.add( DimensionSpec::midplanesToStr( dim_specs[Dimension::B].getIncludedMidplanes() ) );
            mps_arr.add( DimensionSpec::midplanesToStr( dim_specs[Dimension::C].getIncludedMidplanes() ) );
            mps_arr.add( DimensionSpec::midplanesToStr( dim_specs[Dimension::D].getIncludedMidplanes() ) );
        }

        string torus_str;

        if ( dim_specs[Dimension::A].getConnectivity() == Connectivity::Torus )  torus_str += 'A';
        if ( dim_specs[Dimension::B].getConnectivity() == Connectivity::Torus )  torus_str += 'B';
        if ( dim_specs[Dimension::C].getConnectivity() == Connectivity::Torus )  torus_str += 'C';
        if ( dim_specs[Dimension::D].getConnectivity() == Connectivity::Torus )  torus_str += 'D';
        torus_str += 'E';

        obj.set( "torus", torus_str );

    } else { // it's a small block.
        obj.set( "midplane", gen_block_params.getMidplane() );

        if ( args_type == SMALL ) {
            obj.set( "nodeBoard", gen_block_params.getNodeBoard() );
            obj.set( "nodeBoardCount", gen_block_params.getNodeBoardCount() );
        } else if ( args_type == NODE_BOARDS ) {
            json::Array &nbs_arr(obj.createArray( "nodeBoards" ));

            BOOST_FOREACH ( const string& nb_pos , gen_block_params.getNodeBoardPositions() ) {
                nbs_arr.add( nb_pos );
            }
        }
    }

    return object_value_ptr;
}


static void createBlock(
        const GenBlockParams& gen_block_params,
        const std::string& description,
        const BgwsOptions& bgws_options,
        ArgsType args_type
    )
{
    CURLcode crc;

    json::ValuePtr json_ptr(genBlockParamsToJson(
            gen_block_params,
            description,
            args_type
        ));

    string url(bgws_options.getBase() + bgws::BLOCKS_URL_PATH.toString());

    LOG_DEBUG_MSG( "Posting to '" << url << "'" );

    CURL *chandle(curl_easy_init());

    if ( chandle == NULL ) {
        BOOST_THROW_EXCEPTION( runtime_error( "failed to initialize the curl handle" ) );
    }

    BOOST_SCOPE_EXIT( (&chandle) ) { curl_easy_cleanup( chandle ); } BOOST_SCOPE_EXIT_END

    char error_buffer[CURL_ERROR_SIZE] = { 0 };

    if ( (crc = curl_easy_setopt( chandle, CURLOPT_ERRORBUFFER, error_buffer )) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the ERRORBUFFER option on the curl handle, " + curl_easy_strerror( crc ) ) );
    }

    if ( curl_easy_setopt( chandle, CURLOPT_URL, url.c_str() ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to set the URL option on the curl handle, " + error_buffer ) );
    }

    bgws_clients::utility::setSslOptions(
            bgws_options,
            chandle,
            error_buffer
        );

    string session_id;
    utility::calcSessionId(
            bgws_options,
            session_id
        );

    utility::HeaderGuard hg(utility::setHeaders( chandle, error_buffer, session_id, true ));

    RequestData request_data(
            json::Formatter()( *json_ptr ),
            chandle, error_buffer
        );

    Response response( chandle, error_buffer );

    if ( curl_easy_perform( chandle ) != CURLE_OK ) {
        BOOST_THROW_EXCEPTION( runtime_error( string() + "curl operation failed, " + error_buffer ) );
    }

    // response should now hold the server's response

    // Check to make sure that the response looks OK, throws if it's not.
    bgws::checkResponse( response, capena::http::Status::Created );
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bool help(false);

        bool args_type_set(false);
        ArgsType args_type(LARGE);

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
        BgwsOptions bgws_options;
        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgws" );


        po::options_description visible_desc( "Options" );
        visible_desc.add( bgws_options.getDesc() );
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
"    --connectivity specifies the torus dimensions.\n"
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
                args_type = LARGE;
                args_type_set = true;
            }
            if ( is_small ) {
                if ( args_type_set )  BOOST_THROW_EXCEPTION( runtime_error( "can only have one size, either --large, --small, --mps, or --nbs" ) );
                args_type = SMALL;
                args_type_set = true;
            }
            if ( is_midplanes ) {
                if ( args_type_set )  BOOST_THROW_EXCEPTION( runtime_error( "can only have one size, either --large, --small, --mps, or --nbs" ) );
                args_type = MIDPLANES;
                args_type_set = true;
            }
            if ( is_node_boards ) {
                if ( args_type_set )  BOOST_THROW_EXCEPTION( runtime_error( "can only have one size, either --large, --small, --mps, or --nbs" ) );
                args_type = NODE_BOARDS;
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
                    args_type = LARGE;
                } else if (
                        args.size() == 2 &&
                        isNodeBoard( args[0] ) &&
                        (! isNodeBoard( args[1] ))
                    )
                {
                    args_type = SMALL;
                } else if (
                        std::find_if( args.begin(), args.end(), ! bind( isMidplane, _1 ) ) == args.end()
                    )
                {
                    args_type = MIDPLANES;
                } else if (
                        std::find_if( args.begin(), args.end(), ! bind( isNodeBoard, _1 ) ) == args.end()
                    )
                {
                    args_type = NODE_BOARDS;
                }
            }

            // check midplanes_connectivity only set if is_midplanes and only contains "ABCDE":

            if ( midplanes_connectivity.find_first_not_of( "ABCDE" ) != string::npos ) {
                BOOST_THROW_EXCEPTION( runtime_error( "midplanes connectivity contains invalid dimension" ) );
            }

            if ( args_type == MIDPLANES ) {
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
            case LARGE:
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
            case SMALL:
                if ( args.size() != 2 ) {
                    BOOST_THROW_EXCEPTION( runtime_error( "wrong number of arguments for small block" ) );
                }

                midplane = args[0].substr( 0, string( "R00-M0" ).length() );
                node_board_pos = args[0].substr( string( "R00-M0" ).length() + 1 );
                node_board_count = numeric_cast<uint8_t>( lexical_cast<unsigned>( args[1] ) );
                break;
            case MIDPLANES:
                midplane_locations = args;
                break;
            case NODE_BOARDS:
                node_board_locations = args;
                break;
            }
        } catch ( exception& e ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "invalid arguments, " + e.what() + ". Use -h for help." ) );
        }


        bgq::utility::Properties bg_properties( bgws_options.getBgPropertiesFileName() );

        bgq::utility::initializeLogging(
                bg_properties,
                logging_program_options
            );


        bgws_options.setBgProperties( bg_properties );


        if ( ! args_type_set ) {
            LOG_DEBUG_MSG( "Guessed type is " << args_type );
        }

        GenBlockParams gen_block_params;

        gen_block_params.setBlockId( block_id );

        switch ( args_type ) {
        case LARGE:
            LOG_DEBUG_MSG( "midplane=" << midplane <<
                   " specs=" << dim_specs[Dimension::A].toString() <<
                         " " << dim_specs[Dimension::B].toString() <<
                         " " << dim_specs[Dimension::C].toString() <<
                         " " << dim_specs[Dimension::D].toString()
               );

            gen_block_params.setMidplane( midplane );
            gen_block_params.setDimensionSpecs( dim_specs );
            break;
        case SMALL:
            LOG_DEBUG_MSG( "midplane=" << midplane << " nodeBoard=" << node_board_pos << " count=" << unsigned(node_board_count) );

            gen_block_params.setMidplane( midplane );
            gen_block_params.setNodeBoardAndCount( node_board_pos, node_board_count );
            break;
        case MIDPLANES:
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
        case NODE_BOARDS:
            LOG_DEBUG_MSG( "node_boards.size=" << node_board_locations.size() );
            gen_block_params.setNodeBoardLocations( node_board_locations );
            break;
        }


        CURLcode crc;

        if ( (crc = curl_global_init( CURL_GLOBAL_ALL )) != CURLE_OK ) {
            BOOST_THROW_EXCEPTION( runtime_error( string() + "failed to initialize the curl library, " + curl_easy_strerror( crc ) ) );
        }

        BOOST_SCOPE_EXIT() { curl_global_cleanup(); } BOOST_SCOPE_EXIT_END

        createBlock(
                gen_block_params,
                block_description,
                bgws_options,
                args_type
            );

        return 0;
    } catch ( exception& e ) {
        cout << argv[0] << ": error, " << e.what() << "\n";
        return 1;
    }
}
