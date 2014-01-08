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

#include "GenBlockParams.h"

#include "BGQDBlib.h"
#include "utility.h"

#include "tableapi/gensrc/bgqtableapi.h"

#include <boost/assign.hpp>
#include <boost/exception/all.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/spirit/include/classic_core.hpp>

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>

#include <stdint.h>

using namespace boost::assign; // bring 'operator+=()' into scope

using boost::lexical_cast;

using std::invalid_argument;
using std::logic_error;
using std::map;
using std::string;

namespace BGQDB {

//---------------------------------------------------------------------
// class DimensionSpec

const DimensionSpec DimensionSpec::Midplane( 1 );
const DimensionSpec DimensionSpec::Full( 0 );

DimensionSpec::IncludedMidplanes DimensionSpec::strToMidplanes( const std::string& s )
{
    IncludedMidplanes ret;

    for ( string::const_iterator i(s.begin()) ; i != s.end() ; ++i ) {
        ret.push_back( *i == '1' );
    }

    return ret;
}


std::string DimensionSpec::midplanesToStr( const IncludedMidplanes& mps )
{
    string ret;

    for ( IncludedMidplanes::const_iterator i(mps.begin()) ; i != mps.end() ; ++i ) {
        ret += *i ? '1' : '0';
    }

    return ret;
}


DimensionSpec::DimensionSpec(
        unsigned size,
        Connectivity::Value connectivity
    ) :
        _connectivity(connectivity),
        _included_midplanes(IncludedMidplanes( size, true ))
{
    _validate();
}


DimensionSpec::DimensionSpec(
        const IncludedMidplanes& included_midplanes,
        Connectivity::Value connectivity
    ) :
        _connectivity(connectivity),
        _included_midplanes(included_midplanes)
{
    _validate();
}


DimensionSpec::DimensionSpec(
        Connectivity::Value connectivity
    ) :
        _connectivity(connectivity)
{
    // Nothing to do.
}


DimensionSpec::DimensionSpec(
        const std::string& str
    )
{
    using namespace BOOST_SPIRIT_CLASSIC_NS;

    unsigned size(0);
    string connectivity_char("T");
    string specs_str;

    rule<> dim_spec_r =
            ( ! uint_p[assign_a(size)] >>
              ! (ch_p( 'T' ) | 'M')[assign_a(connectivity_char)] >>
              ! (':' >> (*anychar_p)[assign_a(specs_str)])
            );

    if ( ! parse( str.c_str(), dim_spec_r ).full
        ) {
        BOOST_THROW_EXCEPTION( invalid_argument( string() +
                "invalid dimension spec string '" + str + "'"
            ) );
    }

    _connectivity = connectivity_char == "M" ? Connectivity::Mesh : Connectivity::Torus;

    if ( ! specs_str.empty() ) {
        unsigned mps_count(std::count( specs_str.begin(), specs_str.end(), '1'));
        if ( size != 0 ) {
            // The number of 1s must match the size.
            if ( mps_count != size ) {
                BOOST_THROW_EXCEPTION( invalid_argument( string() +
                        "invalid dimension spec string '" + str + "'."
                        " The size must match the number of midplanes in the passthrough spec"
                    ) );
            }
        } else {
            size = mps_count;
        }

        _included_midplanes = strToMidplanes( specs_str );
    } else {
        if ( size != 0 ) {
            _included_midplanes = IncludedMidplanes( size, true );
        }
    }

    _validate();
}


DimensionSpec::DimensionSpec(
        unsigned size,
        const std::string& str
    )
{
    // str format: [T|M][specs_str]

    if ( str.empty() ) {
        _connectivity = Connectivity::Torus;
        _included_midplanes = IncludedMidplanes( size, true );
        return;
    }

    string::const_iterator i(str.begin());
    if ( *i == 'T' ) {
        _connectivity = Connectivity::Torus;
        ++i;
    } else if ( *i == 'M' ) {
        _connectivity = Connectivity::Mesh;
        ++i;
    }

    if ( i == str.end() ) {
        _included_midplanes = IncludedMidplanes( size, true );
    } else {

        if ( size != 0 &&
             size != unsigned(std::count( i, str.end(), '1' )) )
        {
            BOOST_THROW_EXCEPTION( invalid_argument( string() +
                    "the size and the passthrough spec are not consistent."
                    " The size is " + lexical_cast<string>( size ) + " and the passthrough spec is '" + str + "'"
                ) );
        }

        _included_midplanes = strToMidplanes( string( i, str.end() ) );
    }

    _validate();
}


uint8_t DimensionSpec::size() const
{
    return std::count( _included_midplanes.begin(), _included_midplanes.end(), true );
}


std::string DimensionSpec::toString() const
{
    string ret;
    if ( _connectivity == Connectivity::Torus ) {
        ret += "T";
    } else {
        ret += "M";
    }

    if ( _included_midplanes.empty() ) {
        return ret;
    }

    ret += ":";

    for ( IncludedMidplanes::const_iterator i(_included_midplanes.begin()) ;
          i != _included_midplanes.end() ;
          ++i )
    {
        ret += *i ? "1" : "0";
    }

    return ret;
}


void DimensionSpec::_validate()
{
    // May be empty, indicating use all mps in the dim.
    // If not empty, can't pass through all offsets.
    // If there's only 1 included midplane then can't be any passthrough and must be torus.

    if ( _included_midplanes.empty() ) {
        return;
    }

    unsigned mps(std::count( _included_midplanes.begin(), _included_midplanes.end(), true ));

    if ( mps == 0 ) {
        BOOST_THROW_EXCEPTION( invalid_argument( string() +
                "a dimension must have at least one included midplane"
            ) );
    }

    if ( mps == 1 &&
         (_included_midplanes.size() != 1 ||
          _connectivity != Connectivity::Torus) )
    {
        BOOST_THROW_EXCEPTION( invalid_argument( string() +
                "a dimension of size 1 must not have passthroughs and must be a torus"
            ) );
    }

    if ( _connectivity == Connectivity::Mesh ) {
        if ( ! _included_midplanes.front() || ! _included_midplanes.back() ) {
            BOOST_THROW_EXCEPTION( invalid_argument( string() +
                    "a mesh dimension cannot start or end with passthrough"
                ) );
        }
    }
}


const DimensionSpecs MidplaneDimensionSpecs = { {
        DimensionSpec::Midplane, DimensionSpec::Midplane,
        DimensionSpec::Midplane, DimensionSpec::Midplane
    } };


const DimensionSpecs FullDimensionSpecs = { {
        DimensionSpec::Full, DimensionSpec::Full,
        DimensionSpec::Full, DimensionSpec::Full
    } };


//---------------------------------------------------------------------
// class GenBlockParms


const GenBlockParams::MidplaneLocations GenBlockParams::EmptyPassthroughLocations;


GenBlockParams::GenBlockParams() :
    _description( "Generated by genBlock" ),
    _microloader_image(DEFAULT_MLOADERIMG),
    _node_configuration(DEFAULT_COMPUTENODECONFIG),
    _dimension_specs(MidplaneDimensionSpecs)
{
    // Nothing to do.
}

void GenBlockParams::setBlockId( const std::string& block_id )
{
    if ( ! isBlockIdValid( block_id, __FUNCTION__ ) ) {
        BOOST_THROW_EXCEPTION( invalid_argument( "the block ID is not valid" ) );
    }

    _block_id = block_id;
}


void GenBlockParams::setMidplane( const std::string& midplane_location )
{
    if ( midplane_location.empty() ) {
        BOOST_THROW_EXCEPTION( invalid_argument( "cannot set midplane to empty string" ) );
    }
    if ( midplane_location.size() >= sizeof ( DBTBpblockmap()._bpid ) ) {
        BOOST_THROW_EXCEPTION( invalid_argument( string() + "cannot set the midplane to '" + midplane_location + "' because it is too long" ) );
    }

    _midplane_location = midplane_location;

    _midplanes.clear(); // It's not Large with Midplanes.
}


void GenBlockParams::setDimensionSpecs(
        const DimensionSpecs& dim_specs
    )
{
    _node_board_positions.clear();

    _dimension_specs = dim_specs;
}


void GenBlockParams::setMidplanes(
        const MidplaneLocations& midplanes,
        const MidplaneLocations& passthrough_locations
    )
{
    if ( midplanes.empty() ) {
        BOOST_THROW_EXCEPTION( invalid_argument( "cannot set midplanes empty" ) );
    }


    _node_board_positions.clear();
    _midplane_location.clear();

    _midplanes = midplanes;
    _passthrough_locations = passthrough_locations;

    DimensionSpecs dim_specs = { {
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus ),
            DimensionSpec( Connectivity::Torus )
        } };

    _dimension_specs = dim_specs;
}


void GenBlockParams::setNodeBoardAndCount(
        const std::string& start_node_board_position,
        uint8_t node_board_count
    )
{
    const SmallBlockSizeInfo &small_block_info(getSmallBlockInfoForSize( node_board_count ));

    const NodeBoardStartPositionToPositions &start_position_to_positions(small_block_info.start_position_to_positions);

    NodeBoardStartPositionToPositions::const_iterator spi(start_position_to_positions.find( start_node_board_position ));

    if ( spi == start_position_to_positions.end() ) {
        BOOST_THROW_EXCEPTION( invalid_argument( "invalid start node board position for size" ) );
    }

    _node_board_positions = spi->second;

    DimensionSpecs dim_specs = { {
            DimensionSpec( small_block_info.connectivity[Dimension::A] ),
            DimensionSpec( small_block_info.connectivity[Dimension::B] ),
            DimensionSpec( small_block_info.connectivity[Dimension::C] ),
            DimensionSpec( small_block_info.connectivity[Dimension::D] )
        } };

    _dimension_specs = dim_specs;

    _midplanes.clear(); // It's not Large with Midplanes.
}


void GenBlockParams::setNodeBoardLocations(
        const NodeBoardLocations& node_board_locations
    )
{
    static const string SampleMidplaneLocation( "R00-M0" );
    static const unsigned MidplaneLocationLength(SampleMidplaneLocation.size());
    static const string SampleNodeBoardLocation( SampleMidplaneLocation + "-N00" );
    static const unsigned NodeBoardPositionOffset((SampleMidplaneLocation + "-").size());
    static const unsigned NodeBoardLocationLength(SampleNodeBoardLocation.size());

    // Go through all the locations and make sure they are valid and start with the same midplane.

    if ( node_board_locations.empty() ) {
        BOOST_THROW_EXCEPTION( invalid_argument( "node board locations cannot be empty" ) );
    }

    string midplane_location;
    NodeBoardPositions node_board_positions;

    BOOST_FOREACH( const string& nb_loc, node_board_locations ) {
        if ( nb_loc.length() != NodeBoardLocationLength ) {
            BOOST_THROW_EXCEPTION( invalid_argument( string() +
                    "invalid node board location '" + nb_loc + "'"
                ) );
        }

        if ( midplane_location.empty() ) {
            midplane_location = nb_loc.substr( 0, MidplaneLocationLength );
        } else if ( nb_loc.substr( 0, MidplaneLocationLength ) != midplane_location ) {
            BOOST_THROW_EXCEPTION( invalid_argument( string() +
                    "node board locations are not on the same midplane,"
                    " found " + nb_loc.substr( 0, MidplaneLocationLength ) + " expected " + midplane_location
                ) );
        }

        node_board_positions.push_back( nb_loc.substr( NodeBoardPositionOffset ) );
    }

    const SmallBlockSizeInfo &small_block_info(getSmallBlockInfoForSize( node_board_positions.size() ));

    std::sort( node_board_positions.begin(), node_board_positions.end() );

    const NodeBoardStartPositionToPositions &start_position_to_positions(small_block_info.start_position_to_positions);

    NodeBoardStartPositionToPositions::const_iterator spi(start_position_to_positions.find( node_board_positions[0] ));

    if ( spi == start_position_to_positions.end() ) {
        BOOST_THROW_EXCEPTION( invalid_argument( string() +
                "invalid start node board position for size."
                " Start node board position=" + node_board_positions[0] + " size=" + lexical_cast<string>( node_board_locations.size() )
            ) );
    }

    if ( ! std::equal( node_board_positions.begin(), node_board_positions.end(), spi->second.begin() ) ) {
        BOOST_THROW_EXCEPTION( invalid_argument( "node board locations includes an invalid node board position." ) );
    }

    _midplane_location = midplane_location;
    _node_board_positions.swap( node_board_positions );

    DimensionSpecs dim_specs = { {
            DimensionSpec( small_block_info.connectivity[Dimension::A] ),
            DimensionSpec( small_block_info.connectivity[Dimension::B] ),
            DimensionSpec( small_block_info.connectivity[Dimension::C] ),
            DimensionSpec( small_block_info.connectivity[Dimension::D] )
        } };

    _dimension_specs = dim_specs;

    _midplanes.clear(); // It's not Large with Midplanes.
}


const std::string& GenBlockParams::getBlockId() const
{
    if ( _block_id.empty() ) {
        BOOST_THROW_EXCEPTION( logic_error( "the block ID was not set" ) );
    }
    return _block_id;
}


const std::string& GenBlockParams::getMidplane() const
{
    if ( _midplane_location.empty() ) {
        BOOST_THROW_EXCEPTION( logic_error( "cannot get midplane before the midplane is set" ) );
    }
    return _midplane_location;
}


bool GenBlockParams::isLarge() const
{
    return _node_board_positions.empty();
}


bool GenBlockParams::isLargeWithMidplanes() const
{
    return (isLarge() && (! _midplanes.empty()));
}


const GenBlockParams::MidplaneLocations& GenBlockParams::getMidplanes() const
{
    if ( ! isLargeWithMidplanes() ) {
        BOOST_THROW_EXCEPTION( logic_error( "cannot get midplanes before the midplanes are set" ) );
    }

    return _midplanes;
}


const GenBlockParams::MidplaneLocations& GenBlockParams::getPassthroughLocations() const
{
    if ( ! isLargeWithMidplanes() ) {
        BOOST_THROW_EXCEPTION( logic_error( "cannot get passthrough midplanes before the midplanes are set" ) );
    }

    return _passthrough_locations;
}


const NodeBoardPositions& GenBlockParams::getNodeBoardPositions() const
{
    if ( isLarge() ) {
        BOOST_THROW_EXCEPTION( logic_error( "cannot get node board positions for large block" ) );
    }
    return _node_board_positions;
}


const std::string& GenBlockParams::getNodeBoard() const
{
    if ( isLarge() ) {
        BOOST_THROW_EXCEPTION( logic_error( "cannot get node board for large block" ) );
    }
    return _node_board_positions[0];
}


uint8_t GenBlockParams::getNodeBoardCount() const
{
    if ( isLarge() ) {
        BOOST_THROW_EXCEPTION( logic_error( "cannot get node board count for large block" ) );
    }
    return _node_board_positions.size();
}


GenBlockParams::DimensionStrings GenBlockParams::calcDimensionSpecStrings() const
{
    if ( ! isLarge() ) {
        BOOST_THROW_EXCEPTION( logic_error( "cannot get dimension strings for small block" ) );
    }

    DimensionStrings ret = { {
            _dimension_specs[Dimension::A].toString(),
            _dimension_specs[Dimension::B].toString(),
            _dimension_specs[Dimension::C].toString(),
            _dimension_specs[Dimension::D].toString()
        } };

    return ret;
}


std::ostream& operator<<( std::ostream& os, const GenBlockParams& gen_block_params )
{
    os << "GenBlockParameters:\n"
          "\tid=" << gen_block_params._block_id << "\n"
          "\tdesc=" << gen_block_params._description << "\n"
          "\toptions=" << gen_block_params._options << "\n"
          "\tboot_options=" << gen_block_params._boot_options << "\n"
          "\tmicroloader_image=" << gen_block_params._microloader_image << "\n"
          "\tnode_configuration=" << gen_block_params._node_configuration << "\n";

    if ( ! gen_block_params._midplane_location.empty() ) {
        os << "\tmidplane=" << gen_block_params._midplane_location << "\n";
    }
    if ( ! gen_block_params._midplanes.empty() ) {
        os << "\tmidplanes=";
        BOOST_FOREACH( const std::string& mp, gen_block_params._midplanes ) {
            if ( &mp != &gen_block_params._midplanes.front() ) {
                os << ",";
            }
            os << mp;
        }
        os << "\n";
    }
    if ( ! gen_block_params._passthrough_locations.empty() ) {
        os << "\tpassthrough=";
        BOOST_FOREACH( const std::string& pt, gen_block_params._passthrough_locations ) {
            if ( &pt != &gen_block_params._passthrough_locations.front() ) {
                os << ",";
            }
            os << pt;
        }
        os << "\n";
    }
    if ( ! gen_block_params._node_board_positions.empty() ) {
        os << "\nnode boards=";
        BOOST_FOREACH( const std::string& nb, gen_block_params._node_board_positions ) {
            if ( &nb != &gen_block_params._node_board_positions.front() ) {
                os << ",";
            }
            os << nb;
        }
        os << "\n";
    }
    os << "\tdim_specs=";
    BOOST_FOREACH( const DimensionSpec& dim_spec, gen_block_params._dimension_specs ) {
        if ( &dim_spec != &gen_block_params._dimension_specs.front() ) {
            os << ",";
        }
        os << dim_spec.toString();
    }
    os << "\n";

    return os;
}


} // namespace BGQDB

