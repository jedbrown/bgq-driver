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

#include "utility.h"

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>

using namespace boost::assign;

using boost::lexical_cast;

using std::invalid_argument;
using std::string;

namespace BGQDB {

typedef std::map<unsigned,SmallBlockSizeInfo> SmallBlockSizeToInfo;


static SmallBlockSizeToInfo initializeSmallBlockSizeToInfo()
{
    SmallBlockSizeToInfo ret;

    {
        SmallBlockSizeInfo i1;
        insert( i1.start_position_to_positions )
            ( "N00", list_of( "N00" ) )
            ( "N01", list_of( "N01" ) )
            ( "N02", list_of( "N02" ) )
            ( "N03", list_of( "N03" ) )
            ( "N04", list_of( "N04" ) )
            ( "N05", list_of( "N05" ) )
            ( "N06", list_of( "N06" ) )
            ( "N07", list_of( "N07" ) )
            ( "N08", list_of( "N08" ) )
            ( "N09", list_of( "N09" ) )
            ( "N10", list_of( "N10" ) )
            ( "N11", list_of( "N11" ) )
            ( "N12", list_of( "N12" ) )
            ( "N13", list_of( "N13" ) )
            ( "N14", list_of( "N14" ) )
            ( "N15", list_of( "N15" ) )
            ;

        DimensionConnectivity conn1 = { {
                Connectivity::Mesh,
                Connectivity::Mesh,
                Connectivity::Mesh,
                Connectivity::Mesh
            } };

        i1.connectivity = conn1;

        DimensionSizes sizes1 = { { 2, 2, 2, 2 } };
        i1.sizes = sizes1;

        ret[1] = i1;
    }

    {
        SmallBlockSizeInfo i2;
        insert( i2.start_position_to_positions )
            ( "N00", list_of( "N00" )( "N01" ) )
            ( "N02", list_of( "N02" )( "N03" ) )
            ( "N04", list_of( "N04" )( "N05" ) )
            ( "N06", list_of( "N06" )( "N07" ) )
            ( "N08", list_of( "N08" )( "N09" ) )
            ( "N10", list_of( "N10" )( "N11" ) )
            ( "N12", list_of( "N12" )( "N13" ) )
            ( "N14", list_of( "N14" )( "N15" ) )
            ;

        DimensionConnectivity conn2 = { {
                Connectivity::Mesh,
                Connectivity::Mesh,
                Connectivity::Torus,
                Connectivity::Mesh
            } };

        i2.connectivity = conn2;

        DimensionSizes sizes2 = { { 2, 2, 4, 2 } };
        i2.sizes = sizes2;

        ret[2] = i2;
    }

    {
        SmallBlockSizeInfo i4;
        insert( i4.start_position_to_positions )
            ( "N00", list_of( "N00" )( "N01" )( "N02" )( "N03" ) )
            ( "N04", list_of( "N04" )( "N05" )( "N06" )( "N07" ) )
            ( "N08", list_of( "N08" )( "N09" )( "N10" )( "N11" ) )
            ( "N12", list_of( "N12" )( "N13" )( "N14" )( "N15" ) )
            ;

        DimensionConnectivity conn4 = { {
                Connectivity::Mesh,
                Connectivity::Torus,
                Connectivity::Torus,
                Connectivity::Mesh
            } };

        i4.connectivity = conn4;

        DimensionSizes sizes4 = { { 2, 2, 4, 4 } };
        i4.sizes = sizes4;

        ret[4] = i4;
    }

    {
        SmallBlockSizeInfo i8;
        insert( i8.start_position_to_positions )
            ( "N00",
                list_of( "N00" )( "N01" )( "N02" )( "N03" )
                       ( "N04" )( "N05" )( "N06" )( "N07" ) )
            ( "N08",
                list_of( "N08" )( "N09" )( "N10" )( "N11" )
                       ( "N12" )( "N13" )( "N14" )( "N15" ) )
            ;

        DimensionConnectivity conn8 = { {
                Connectivity::Torus,
                Connectivity::Torus,
                Connectivity::Torus,
                Connectivity::Mesh
            } };

        i8.connectivity = conn8;

        DimensionSizes sizes8 = { { 4, 2, 4, 4 } };
        i8.sizes = sizes8;

        ret[8] = i8;
    }

    return ret;
}


const SmallBlockSizeInfo& getSmallBlockInfoForSize( unsigned size_in_node_boards )
{
    static SmallBlockSizeToInfo s_small_block_size_to_info(initializeSmallBlockSizeToInfo());

    SmallBlockSizeToInfo::const_iterator i(s_small_block_size_to_info.find( size_in_node_boards ));

    if ( i == s_small_block_size_to_info.end() ) {
        BOOST_THROW_EXCEPTION( invalid_argument( string() +
                "invalid size for small block."
                " The number of node boards must be 1, 2, 4, or 8."
                " The node board size supplied is " + lexical_cast<string>( size_in_node_boards ) + "." )
            );
    }

    return (i->second);
}


std::string dimensionConnectivityToDbTorusString( const DimensionConnectivity dim_conns )
{
    string ret;

    ret += (dim_conns[Dimension::A] == Connectivity::Torus ? '1' : '0');
    ret += (dim_conns[Dimension::B] == Connectivity::Torus ? '1' : '0');
    ret += (dim_conns[Dimension::C] == Connectivity::Torus ? '1' : '0');
    ret += (dim_conns[Dimension::D] == Connectivity::Torus ? '1' : '0');
    ret += '1';

    return ret;
}


DimensionConnectivity dbTorusStringToDimensionConnectivity( const std::string& s )
{
    if ( s.size() != 5 ) {
        BOOST_THROW_EXCEPTION( invalid_argument( string() + "invalid DB torus string, " + s ) );
    }

    DimensionConnectivity ret;

    ret[Dimension::A] = s[0] == '1' ? Connectivity::Torus : Connectivity::Mesh;
    ret[Dimension::B] = s[1] == '1' ? Connectivity::Torus : Connectivity::Mesh;
    ret[Dimension::C] = s[2] == '1' ? Connectivity::Torus : Connectivity::Mesh;
    ret[Dimension::D] = s[3] == '1' ? Connectivity::Torus : Connectivity::Mesh;

    return ret;
}


} // namespace BGQDB
