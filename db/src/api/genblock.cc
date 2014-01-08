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

#include "genblock.h"

#include "BlockDatabaseInfo.h"
#include "Exception.h"
#include "GenBlockParams.h"
#include "utility.h"

#include "cxxdb/cxxdb.h"

#include "tableapi/DBConnectionPool.h"
#include "tableapi/TxObject.h"

#include "tableapi/gensrc/bgqtableapi.h"

#include <utility/include/Log.h>
#include <utility/include/UserId.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>
#include <boost/unordered_map.hpp>

#include <boost/thread/once.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>

#include <stdio.h> // snprintf


using namespace boost::assign;

using boost::lexical_cast;

using std::exception;
using std::string;


LOG_DECLARE_FILE( "database" );


namespace BGQDB {


// see initializeMidplaneCoordsMaps()

typedef boost::unordered_map<string,MidplaneCoordinate> MidplaneLocationToCoordinates;
typedef boost::unordered_map<MidplaneCoordinate,string,MidplaneCoordinateHashFn> MidplaneCoordinatesToLocation;

static MidplaneLocationToCoordinates s_mp_to_coords;
static MidplaneCoordinatesToLocation s_coords_to_mp;
static STATUS s_mp_coords_maps_status;
static boost::once_flag s_mp_coords_maps_once_flag;


static void
initializeMidplaneCoordsMapsOnce()
{
    SQLRETURN sqlrc(SQL_SUCCESS);

    // Do a query over all the midplanes to get their location and coordinates,
    // add the info to the map.

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        s_mp_coords_maps_status = CONNECTION_ERROR;
        return;
    }

    ColumnsBitmap mp_cols;
    mp_cols.set(DBVMidplane::LOCATION);
    mp_cols.set(DBVMidplane::TORUSA);
    mp_cols.set(DBVMidplane::TORUSB);
    mp_cols.set(DBVMidplane::TORUSC);
    mp_cols.set(DBVMidplane::TORUSD);

    DBVMidplane dbMp( mp_cols );

    if ((sqlrc = tx.query(&dbMp, "" ))!= SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        s_mp_coords_maps_status = DB_ERROR;
        return;
    }

    while ( SQL_SUCCEEDED( sqlrc = tx.fetch( &dbMp ) ) ) {
        MidplaneCoordinate coord = { { dbMp._torusa, dbMp._torusb, dbMp._torusc, dbMp._torusd } };
        s_mp_to_coords[dbMp._location] = coord;
        s_coords_to_mp[coord] = dbMp._location;
    }

    if ( sqlrc == SQL_NO_DATA ) {
        return;
    }

    s_mp_coords_maps_status = DB_ERROR;
}


static void
initializeMidplaneCoordsMaps()
{
    boost::call_once( s_mp_coords_maps_once_flag, &initializeMidplaneCoordsMapsOnce );

    if ( s_mp_coords_maps_status != OK ) {
        BOOST_THROW_EXCEPTION( Exception( s_mp_coords_maps_status ) );
    }
}


void torusCoordinateForMidplane(
        const std::string& midplane_location,
        MidplaneCoordinate& midplane_coordinate_out
    )
{
    initializeMidplaneCoordsMaps();

    MidplaneLocationToCoordinates::const_iterator i(s_mp_to_coords.find( midplane_location ));
    if ( i == s_mp_to_coords.end() ) {
        BOOST_THROW_EXCEPTION( Exception( NOT_FOUND, string() + "didn't find midplane " + midplane_location ) );
    }

    midplane_coordinate_out = i->second;
}


static void midplaneAtTorusCoordinate(
        const MidplaneCoordinate& midplane_coordinate,
        std::string& midplane_location_out
    )
{
    initializeMidplaneCoordsMaps();

    MidplaneCoordinatesToLocation::const_iterator i(s_coords_to_mp.find( midplane_coordinate ));
    if ( i == s_coords_to_mp.end() ) {
        BOOST_THROW_EXCEPTION( Exception( NOT_FOUND, string() + "didn't find midplane at " + lexical_cast<string>( midplane_coordinate ) ) );
    }

    midplane_location_out = i->second;
}


static string calcSwitchId( Dimension::Value dim, const string& mp_loc )
{
    return (Dimension::toString( dim ) + "_" + mp_loc );
}


static void nextBP(
        string& bpid_in_out,
        Dimension::Value dimension
)
{
    // Advances to the next BP for the dimension. Used in genBlockParamsToBlockDatabaseInfoLargeMidplaneDimSpecs.

    STATUS db_status(OK);

    MidplaneCoordinate mp_coord;

    torusCoordinateForMidplane(
            bpid_in_out,
            mp_coord
        );

    DimensionSizes machine_size;

    db_status = getMachineBPSize( machine_size );

    if ( db_status != OK ) {
        BOOST_THROW_EXCEPTION( Exception( db_status, "getMachineBPSize failed" ) );
    }

    // advance the coord in the dimension.
    mp_coord[dimension] = (mp_coord[dimension] + 1) % machine_size[dimension];

    midplaneAtTorusCoordinate(
            mp_coord,
            bpid_in_out
        );
}


static bool blockExists( const string& id, cxxdb::Connection& conn )
{
    DBTBlock b;

    cxxdb::QueryStatementPtr stmt_ptr(b.prepareSelect( conn, "WHERE " + b.BLOCKID_COL + " = ?", list_of( b.BLOCKID_COL ) ));

    stmt_ptr->parameters()[b.BLOCKID_COL].set( id );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    if ( rs_ptr->fetch() ) {
        return true;
    }
    return false;
}


static std::string calcLinkToNextMidplaneId(
        const std::string& mp_location,
        Dimension::Value dimension,
        TxObject& tx
    )
{
    string source_switch_id(calcSwitchId( dimension, mp_location ));

    string next_mp_location(mp_location);

    nextBP( next_mp_location, dimension );

    string dest_switch_id(calcSwitchId( dimension, next_mp_location ));

    string link_id(source_switch_id + dest_switch_id); // Construct link id from source and dest switch IDs.

    return link_id;

#if 0
    SQLRETURN sqlrc(SQL_SUCCESS);

    string source_switch_id(calcSwitchId( dimension, mp_location ));

    ColumnsBitmap sl_cols;
    sl_cols.set(DBVSwitchlinks::DESTINATION);

    DBVSwitchlinks sl( sl_cols );

    string where_clause(string() + "where source = '" + source_switch_id + "'");

    sqlrc = tx.query( &sl, where_clause.c_str() );
    if ( sqlrc != SQL_SUCCESS ) {
        BOOST_THROW_EXCEPTION( Exception( DB_ERROR, string() +
                __FUNCTION__ + " failed while querying link with DB error: " + lexical_cast<string>( sqlrc )
            ) );
    }

    sqlrc = tx.fetch( &sl );
    if ( sqlrc != SQL_SUCCESS ) {
        BOOST_THROW_EXCEPTION( Exception( DB_ERROR, string() +
                __FUNCTION__ + " failed while fetching link with DB error: " + lexical_cast<string>( sqlrc )
            ) );
    }

    string link_id(source_switch_id + sl._destination); // Construct link id from source and dest switch IDs.

    return link_id;
#endif
}


static void calcDatabaseInfo(
        const string& corner_midplane,
        DimensionSpecs dim_specs, // need a copy so can modify
        BlockDatabaseInfo& db_info_out
    )
{
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        BOOST_THROW_EXCEPTION( Exception( CONNECTION_ERROR, "unable to obtain database connection" ) );
    }

    STATUS db_status(OK);
    SQLRETURN sqlrc(SQL_SUCCESS);

    DimensionSizes machine_sizes;

    if ( (db_status = getMachineBPSize( machine_sizes )) != OK ) {
        BOOST_THROW_EXCEPTION( Exception( db_status, "failed to get machine BP size" ) );
    }

    for ( Dimension::Value dim(Dimension::A) ; unsigned(dim) < Dimension::Count ; dim = Dimension::Value(dim+1) ) {
        // - If dimension size is 0 then indicates use the full machine, so set the size here.
        // - If is torus and size > 1 then must fill the machine for the dimension.

        if ( dim_specs[dim].getIncludedMidplanes().empty() ) {
            dim_specs[dim] = DimensionSpec( machine_sizes[dim] );
        } else {
            if ( (dim_specs[dim].getIncludedMidplanes().size() > 1) &&
                 ((dim_specs[dim].getConnectivity() == Connectivity::Torus) &&
                  (dim_specs[dim].getIncludedMidplanes().size() != machine_sizes[dim])) ) {
                BOOST_THROW_EXCEPTION( Exception( INVALID_ARG,
                        string() + "midplanes in dimension " + Dimension::toString(dim) + " do not create a valid torus,"
                                " block size is " + lexical_cast<string>(dim_specs[dim].getIncludedMidplanes().size()) + " but machine size is " + lexical_cast<string>(machine_sizes[dim])
                    ) );
            }
        }
    }


    // Calculate info for the block table.

    DimensionSizes sizes = { {
            dim_specs[Dimension::A].size(),
            dim_specs[Dimension::B].size(),
            dim_specs[Dimension::C].size(),
            dim_specs[Dimension::D].size()
        } };

    DimensionSizes db_sizes = { {
            Midplane_A_Dimension * sizes[Dimension::A],
            Midplane_B_Dimension * sizes[Dimension::B],
            Midplane_C_Dimension * sizes[Dimension::C],
            Midplane_D_Dimension * sizes[Dimension::D]
        } };

    db_info_out.setSizes( db_sizes );

    // Get the corner midplane's machine coordinates
    // by querying BasePartition for the midplane location.

    MidplaneCoordinate corner_midplane_coord;
    torusCoordinateForMidplane( corner_midplane, corner_midplane_coord );

    uint32_t mp_a(0);

    DimensionSizes di;

    for ( di[Dimension::A] = 0 ; di[Dimension::A] < dim_specs[Dimension::A].getIncludedMidplanes().size() ; ++di[Dimension::A] ) {

        uint32_t mp_b(0);

        for ( di[Dimension::B] = 0 ; di[Dimension::B] < dim_specs[Dimension::B].getIncludedMidplanes().size() ; ++di[Dimension::B] ) {

            uint32_t mp_c(0);

            for ( di[Dimension::C] = 0 ; di[Dimension::C] < dim_specs[Dimension::C].getIncludedMidplanes().size() ; ++di[Dimension::C] ) {

                uint32_t mp_d(0);

                for ( di[Dimension::D] = 0 ; di[Dimension::D] < dim_specs[Dimension::D].getIncludedMidplanes().size() ; ++di[Dimension::D] ) {

                    const MidplaneCoordinate &mp_coord = di; // rename to the coordinate.
                    const MidplaneCoordinate torus_coord = {{
                            (corner_midplane_coord[Dimension::A] + mp_coord[Dimension::A]) % machine_sizes[Dimension::A],
                            (corner_midplane_coord[Dimension::B] + mp_coord[Dimension::B]) % machine_sizes[Dimension::B],
                            (corner_midplane_coord[Dimension::C] + mp_coord[Dimension::C]) % machine_sizes[Dimension::C],
                            (corner_midplane_coord[Dimension::D] + mp_coord[Dimension::D]) % machine_sizes[Dimension::D]
                        }};

                    string mp_location;
                    midplaneAtTorusCoordinate( torus_coord, mp_location );

                    unsigned dim_include_count(
                            (dim_specs[Dimension::A].getIncludedMidplanes()[di[Dimension::A]] ? 1 : 0) +
                            (dim_specs[Dimension::B].getIncludedMidplanes()[di[Dimension::B]] ? 1 : 0) +
                            (dim_specs[Dimension::C].getIncludedMidplanes()[di[Dimension::C]] ? 1 : 0) +
                            (dim_specs[Dimension::D].getIncludedMidplanes()[di[Dimension::D]] ? 1 : 0)
                        );

                    bool mp_included(dim_include_count == Dimension::Count);
                    bool used_for_passthrough(dim_include_count == (Dimension::Count-1));

                    LOG_DEBUG_MSG( "For midplane at " << di << " (location=" << mp_location << " torus coord=" << torus_coord << ") included_count=" << dim_include_count << " included=" << mp_included <<" pt=" << used_for_passthrough );

                    if ( ! (mp_included || used_for_passthrough) ) {
                        // not included or used for passthrough, so skip.
                        continue;
                    }

                    if ( mp_included ) {

                        MidplaneCoordinate block_coord = { { mp_a, mp_b, mp_c, mp_d } }; // this is the offset of the midplane in the block (only included, not passthrough)

                        db_info_out.addMidplane( BlockDatabaseInfo::MidplaneInfo( mp_location, block_coord ) );

                        // Insert links from midplane to its switch and back to linkblockmap
                        for ( Dimension::Value dim(Dimension::A) ; unsigned(dim) < Dimension::Count ; dim = Dimension::Value(dim+1) ) {
                            string switch_id(calcSwitchId(dim, mp_location));

                            db_info_out.addLink( mp_location + switch_id );
                            db_info_out.addLink( switch_id + mp_location );
                        }

                        // Add all the midplane switches with their configuration, and the links to the next midplane if necessary.

                        for ( Dimension::Value dim(Dimension::A) ; unsigned(dim) < Dimension::Count ; dim = Dimension::Value(dim+1) ) {

                            string switch_id(calcSwitchId(dim, mp_location));

                            SwitchConfig::Value switch_config;

                            if ( sizes[dim] == 1 ) {

                                // If the size is 1 or used for passthrough then the switch is wrapped.
                                switch_config = SwitchConfig::Wrap;

                            } else { // size > 1
                                // Included midplane typically uses both ports on the switch, but
                                // -- if dimension is mesh and is first in dimension then only using outbound port.
                                // -- if dimension is mesh and is last in dimension then only using inbound port.

                                if ( dim_specs[dim].getConnectivity() == Connectivity::Torus ) {

                                    switch_config = SwitchConfig::Include;

                                } else { // Connectivity is mesh.

                                    if ( di[dim] == 0 ) {
                                        // First midplane in the mesh.
                                        switch_config = SwitchConfig::Out;
                                    } else if ( di[dim] == (dim_specs[dim].getIncludedMidplanes().size()-1) ) {
                                        // Last midplane in the mesh.
                                        switch_config = SwitchConfig::In;
                                    } else {
                                        // Internal midplane to the mesh.
                                        switch_config = SwitchConfig::Include;
                                    }
                                }
                            }

                            db_info_out.addSwitch(
                                    BlockDatabaseInfo::SwitchInfo(
                                            switch_id,
                                            switch_config,
                                            false // not used for passthrough
                                        )
                                );

                            if ( switch_config == SwitchConfig::Out || switch_config == SwitchConfig::Include ) {
                                // add the link to the next midplane.

                                string link_id(calcLinkToNextMidplaneId( mp_location, dim, tx ));

                                db_info_out.addLink( link_id );

                            }
                        }

                    } else  { // used for passthrough

                        MidplaneCoordinate mp_coord = { { mp_a, mp_b, mp_c, mp_d } };

                        db_info_out.addPassthroughMidplane( BlockDatabaseInfo::MidplaneInfo( mp_location, mp_coord ) );

                        // The dimension passed through is the one that's not included.
                        Dimension::Value dim(Dimension::A);
                        for ( ; unsigned(dim) < Dimension::Count ; dim = Dimension::Value(dim+1) ) {
                            if ( dim_specs[dim].getIncludedMidplanes()[di[dim]] == false ) {
                                break;
                            }
                        }

                        string switch_id(calcSwitchId(dim, mp_location));

                        db_info_out.addSwitch(
                                BlockDatabaseInfo::SwitchInfo(
                                        switch_id,
                                        SwitchConfig::Wrap,
                                        true // used for passthrough
                                    )
                            );

                        // Add the link to the next midplane.

                        string link_id(calcLinkToNextMidplaneId( mp_location, dim, tx ));

                        db_info_out.addLink( link_id );

                    }

                    if ( dim_specs[Dimension::D].getIncludedMidplanes()[di[Dimension::D]] )  ++mp_d;
                }

                if ( dim_specs[Dimension::C].getIncludedMidplanes()[di[Dimension::C]] )  ++mp_c;
            }

            if ( dim_specs[Dimension::B].getIncludedMidplanes()[di[Dimension::B]] )  ++mp_b;
        }

        if ( dim_specs[Dimension::A].getIncludedMidplanes()[di[Dimension::A]] )  ++mp_a;
    } // end looping through midplanes.
}


static void genBlockParamsToBlockDatabaseInfoLargeMidplaneDimSpecs(
        const GenBlockParams& genblock_params,
        BlockDatabaseInfo& db_info_out
    )
{
    string corner_midplane;
    DimensionSpecs dim_specs;

    try {
        corner_midplane = genblock_params.getMidplane();
        dim_specs = genblock_params.getDimensionSpecs();
    } catch ( exception& e ) {
        BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() + "invalid arguments to " + __FUNCTION__ + ", " + e.what() ) );
    }

    calcDatabaseInfo(
            corner_midplane,
            dim_specs,
            db_info_out
        );
}


static void genBlockParamsToBlockDatabaseInfoLargeMidplanes(
        const GenBlockParams& genblock_params,
        BlockDatabaseInfo& db_info_out,
        bool do_check
    )
{
    STATUS db_status(OK);
    SQLRETURN sqlrc(SQL_SUCCESS);


    const GenBlockParams::MidplaneLocations &mps(genblock_params.getMidplanes());
    const GenBlockParams::MidplaneLocations &pts(genblock_params.getPassthroughLocations());
    const DimensionSpecs &dim_specs(genblock_params.getDimensionSpecs());

    BlockDatabaseInfo::ConnectivitySpecs connectivity = { {
            dim_specs[Dimension::A].getConnectivity(),
            dim_specs[Dimension::B].getConnectivity(),
            dim_specs[Dimension::C].getConnectivity(),
            dim_specs[Dimension::D].getConnectivity()
        } };


    // Need to know the size of the machine so know range of torus coordinates.

    DimensionSizes machine_sizes;

    if ( (db_status = getMachineBPSize( machine_sizes )) != OK ) {
        BOOST_THROW_EXCEPTION( Exception( db_status, "failed to get machine BP size" ) );
    }

    const string &origin_mp_location(mps.front());

    MidplaneCoordinate origin_torus_coord;

    torusCoordinateForMidplane( origin_mp_location, origin_torus_coord );


    // Calculate the block size, the number of
    // The first midplane is one corner of the block, and one of the other provided midplanes is the other corner.
    // The one that's the other corner is the one that's furthest away.
    // This also checks that the midplane exists on the system.

    DimensionSizes block_size = { { 1, 1, 1, 1 } }; // Number of midplanes interior to the block in each dimension, to be calculated.
        // Note that this includes any interior passthroughs.

    for ( GenBlockParams::MidplaneLocations::const_iterator i(mps.begin() + 1) ; i != mps.end() ; ++i ) { // for the other midplanes.
        const string &mp_location(*i);

        MidplaneCoordinate torus_coord;

        torusCoordinateForMidplane( mp_location, torus_coord );

        // Calculate the size, from origin torus coord to current torus coord

        for ( Dimension::Value dim(Dimension::A) ; dim <= Dimension::D ; dim = Dimension::Value(dim+1) ) {

            unsigned block_offset = (torus_coord[dim] + machine_sizes[dim] - origin_torus_coord[dim]) % machine_sizes[dim];

            block_size[dim] = std::max( block_size[dim], block_offset + 1 );

        }
    }

    // Test that all the passthrough midplanes exist on the system.

    BOOST_FOREACH( const string& pt_location, pts ) {
        MidplaneCoordinate torus_coord;

        torusCoordinateForMidplane( pt_location, torus_coord );
    }

    // Next, calculate the dimension specs from the block size, passthroughs, machine size...

    DimensionSpecs calc_dim_specs;

    for ( Dimension::Value dim(Dimension::A) ; dim <= Dimension::D ; dim = Dimension::Value(dim+1) ) {
        if ( block_size[dim] == 1 ) {
            // The size is one, so the dimension spec is for a midplane. The connectivity must be Torus of course!

            if ( connectivity[dim] != Connectivity::Torus ) {
                BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                        "dimension " + Dimension::toString( dim ) + " is size 1 but the connectivity is Mesh"
                    ));
            }

            calc_dim_specs[dim] = DimensionSpec::Midplane;
            continue;
        }

        // otherwise need to calculate the included midplanes

        DimensionSpec::IncludedMidplanes included_midplanes;
        included_midplanes.push_back( true ); // the origin mp is included.

        MidplaneCoordinate block_coord = { { 0 } };

        unsigned mp_count(1);

        while ( true ) {
            ++(block_coord[dim]); // advance to the next one

            if ( block_coord[dim] == machine_sizes[dim] )  break; // went through the entire machine.

            // calculate the torus coords...
            MidplaneCoordinate torus_coord = { {
                    (origin_torus_coord[Dimension::A] + block_coord[Dimension::A]) % machine_sizes[Dimension::A],
                    (origin_torus_coord[Dimension::B] + block_coord[Dimension::B]) % machine_sizes[Dimension::B],
                    (origin_torus_coord[Dimension::C] + block_coord[Dimension::C]) % machine_sizes[Dimension::C],
                    (origin_torus_coord[Dimension::D] + block_coord[Dimension::D]) % machine_sizes[Dimension::D]
                } };

            string mp_location;
            midplaneAtTorusCoordinate( torus_coord, mp_location );

            if ( std::find( pts.begin(), pts.end(), mp_location ) != pts.end() ) { // this one is used for passthrough.
                if ( connectivity[dim] == Connectivity::Mesh ) {
                    // passing through interior of the block so reduce the block size in the dim.
                    --block_size[dim];
                }

                LOG_DEBUG_MSG( "Found pt " << mp_location << " at " << block_coord << " (torus " << torus_coord << ")" );
                included_midplanes.push_back( false );
                continue;
            }

            // the midplane is included.
            included_midplanes.push_back( true );
            ++mp_count;

            // Check if this may be the last midplane in the block if is torus.
            if ( connectivity[dim] == Connectivity::Mesh && mp_count == block_size[dim] ) {
                break;
            }
        }

        calc_dim_specs[dim] = DimensionSpec( included_midplanes, connectivity[dim] );
    }


    // Now have all the dim specs...

    // Call fn to convert origin + dim specs to db

    calcDatabaseInfo( mps[0], calc_dim_specs, db_info_out );

    if ( do_check ) {
        // Compare the given mps and pts to what's going in the db...

        // Should find each database info midplane in mps, if not, then the caller didn't specify a midplane that they need to specify.

        const BlockDatabaseInfo::Midplanes &bdi_mps(db_info_out.getMidplanes());

        BOOST_FOREACH( const BlockDatabaseInfo::Midplanes::value_type& bdi_mp, bdi_mps ) {

            if ( std::find( mps.begin(), mps.end(), bdi_mp.location ) == mps.end() ) {
                // didn't find it.

                MidplaneCoordinate torus_coord = { {
                        (origin_torus_coord[Dimension::A] + bdi_mp.coord[Dimension::A]) % machine_sizes[Dimension::A],
                        (origin_torus_coord[Dimension::B] + bdi_mp.coord[Dimension::B]) % machine_sizes[Dimension::B],
                        (origin_torus_coord[Dimension::C] + bdi_mp.coord[Dimension::C]) % machine_sizes[Dimension::C],
                        (origin_torus_coord[Dimension::D] + bdi_mp.coord[Dimension::D]) % machine_sizes[Dimension::D]
                    } };

                BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                        "midplane " + bdi_mp.location + " wasn't assigned at " + lexical_cast<string>( bdi_mp.coord ) + " (torus coordinate " + lexical_cast<string>( torus_coord ) + ")"
                    ) );
            }
        }

        // Should find each mp in database info midplanes, if not, then the caller specified an extra midplane.

        BOOST_FOREACH( const string &mp_location, mps ) {

            BlockDatabaseInfo::Midplanes::const_iterator i(bdi_mps.begin());

            for (  ; i != bdi_mps.end() ; ++i ) {
                if ( i->location == mp_location ) break;
            }

            if ( i == bdi_mps.end() ) {
                BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                        "midplane " + mp_location + " wasn't expected"
                    ) );
            }
        }

        // Should find each database info pasthrough mp in the pts.

        const BlockDatabaseInfo::Midplanes &bdi_pts(db_info_out.getPassthroughMidplanes());

        BOOST_FOREACH( const BlockDatabaseInfo::Midplanes::value_type& bdi_pt, bdi_pts ) {

            if ( std::find( pts.begin(), pts.end(), bdi_pt.location ) == pts.end() ) {
                // didn't find it.

                MidplaneCoordinate torus_coord = { {
                        (origin_torus_coord[Dimension::A] + bdi_pt.coord[Dimension::A]) % machine_sizes[Dimension::A],
                        (origin_torus_coord[Dimension::B] + bdi_pt.coord[Dimension::B]) % machine_sizes[Dimension::B],
                        (origin_torus_coord[Dimension::C] + bdi_pt.coord[Dimension::C]) % machine_sizes[Dimension::C],
                        (origin_torus_coord[Dimension::D] + bdi_pt.coord[Dimension::D]) % machine_sizes[Dimension::D]
                    } };

                BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                        "passthrough midplane " + bdi_pt.location + " wasn't assigned at " + lexical_cast<string>( bdi_pt.coord ) + " (torus coordinate " + lexical_cast<string>( torus_coord ) + ")"
                    ) );
            }
        }

        // Should find each pt in database info passthrough midplanes, if not, then the caller specified an extra passthrough.

        BOOST_FOREACH( const string &pt_location, pts ) {

            BlockDatabaseInfo::Midplanes::const_iterator i(bdi_pts.begin());

            for (  ; i != bdi_pts.end() ; ++i ) {
                if ( i->location == pt_location ) break;
            }

            if ( i == bdi_pts.end() ) {
                BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                        "passthrough midplane " + pt_location + " wasn't expected"
                    ) );
            }
        }
    }
}


static void genBlockParamsToBlockDatabaseInfoLarge(
        const GenBlockParams& genblock_params,
        BlockDatabaseInfo& db_info_out,
        bool do_check
    )
{
    if ( genblock_params.isLargeWithMidplanes() ) {
        genBlockParamsToBlockDatabaseInfoLargeMidplanes( genblock_params, db_info_out, do_check );
        return;
    }

    genBlockParamsToBlockDatabaseInfoLargeMidplaneDimSpecs( genblock_params, db_info_out );
}


static void genBlockParamsToBlockDatabaseInfoSmall(
        const GenBlockParams& genblock_params,
        BlockDatabaseInfo& db_info_out
    )
{
    // It's a small block.

    string midplane;
    NodeBoardPositions node_board_positions;

    try {
        midplane = genblock_params.getMidplane();
        node_board_positions = genblock_params.getNodeBoardPositions();
    } catch ( exception& e ) {
        BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                "invalid arguments to " + __FUNCTION__ + ", " + e.what()
            ));
    }

    try {
        const SmallBlockSizeInfo &small_block_info(getSmallBlockInfoForSize( node_board_positions.size() ));
        db_info_out.setSizes( small_block_info.sizes );
    } catch ( std::invalid_argument& e ) {
        BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                "invalid arguments to " + __FUNCTION__ + ", " + e.what()
            ));
    }

    // Check to see if the midplane exists.
    MidplaneCoordinate mp_coord;

    torusCoordinateForMidplane( midplane, mp_coord );

    // Set the midplane and node boards in the db info.
    db_info_out.setMidplane( midplane );
    db_info_out.setNodeBoards( node_board_positions );
}


void insertBlock(
        const BlockDatabaseInfo& info
    )
{
    // Validate that the block owner exists on the system
    try {
        bgq::utility::UserId owner(info.getOwner(),false);
    } catch ( const std::runtime_error& rte ) {
        BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() + "block owner " + info.getOwner() + " does not exist on system."));
    }

    cxxdb::ConnectionPtr conn_ptr(DBConnectionPool::Instance().getConnection());

    cxxdb::Transaction tx( *conn_ptr ); // undo a block insert if, e.g., fails to insert bpblockmap.

    { // Insert to the block table.

        ColumnsBitmap block_cols;
        block_cols.set(DBTBlock::BLOCKID);
        block_cols.set(DBTBlock::BOOTOPTIONS);
        block_cols.set(DBTBlock::DESCRIPTION);
        block_cols.set(DBTBlock::OWNER);
        block_cols.set(DBTBlock::ISTORUS);
        block_cols.set(DBTBlock::MLOADERIMG);
        block_cols.set(DBTBlock::NODECONFIG);
        block_cols.set(DBTBlock::NUMCNODES);
        block_cols.set(DBTBlock::OPTIONS);
        block_cols.set(DBTBlock::SIZEA);
        block_cols.set(DBTBlock::SIZEB);
        block_cols.set(DBTBlock::SIZEC);
        block_cols.set(DBTBlock::SIZED);
        block_cols.set(DBTBlock::SIZEE);

        DBTBlock db_block( block_cols );

        cxxdb::UpdateStatementPtr block_ins_stmt_ptr(db_block.prepareInsert( *conn_ptr ));

        block_ins_stmt_ptr->parameters()[db_block.BLOCKID_COL].set( info.getId() );

        bool desc_truncated;
        block_ins_stmt_ptr->parameters()[db_block.DESCRIPTION_COL].set( info.getDescription(), &desc_truncated ); // allow truncating the description.

        if ( desc_truncated ) {
            LOG_DEBUG_MSG( "Description for '" << info.getId() << "' truncated, length is " << info.getDescription().size() );
        }

        block_ins_stmt_ptr->parameters()[db_block.MLOADERIMG_COL].set( info.getMloaderImg() );
        block_ins_stmt_ptr->parameters()[db_block.NODECONFIG_COL].set( info.getNodeConfig() );
        block_ins_stmt_ptr->parameters()[db_block.BOOTOPTIONS_COL].set( info.getBootOptions() );
        block_ins_stmt_ptr->parameters()[db_block.OPTIONS_COL].set( info.getOptions() );
        block_ins_stmt_ptr->parameters()[db_block.OWNER_COL].set( info.getOwner() );

        block_ins_stmt_ptr->parameters()[db_block.NUMCNODES_COL].cast(
                info.getSizes()[Dimension::A] *
                info.getSizes()[Dimension::B] *
                info.getSizes()[Dimension::C] *
                info.getSizes()[Dimension::D] *
                2 // E dimension is always 2 for blocks
            );

        block_ins_stmt_ptr->parameters()[db_block.SIZEA_COL].cast( info.getSizes()[Dimension::A] );
        block_ins_stmt_ptr->parameters()[db_block.SIZEB_COL].cast( info.getSizes()[Dimension::B] );
        block_ins_stmt_ptr->parameters()[db_block.SIZEC_COL].cast( info.getSizes()[Dimension::C] );
        block_ins_stmt_ptr->parameters()[db_block.SIZED_COL].cast( info.getSizes()[Dimension::D] );
        block_ins_stmt_ptr->parameters()[db_block.SIZEE_COL].cast( 2 );

        string torus_value;
        torus_value += (info.getConnectivitySpecs()[Dimension::A] == Connectivity::Torus ? '1' : '0');
        torus_value += (info.getConnectivitySpecs()[Dimension::B] == Connectivity::Torus ? '1' : '0');
        torus_value += (info.getConnectivitySpecs()[Dimension::C] == Connectivity::Torus ? '1' : '0');
        torus_value += (info.getConnectivitySpecs()[Dimension::D] == Connectivity::Torus ? '1' : '0');
        torus_value += '1';

        block_ins_stmt_ptr->parameters()[db_block.ISTORUS_COL].set( torus_value );

        try {
            block_ins_stmt_ptr->execute();
        } catch ( cxxdb::DatabaseException& dbe ) {
            bool block_exists( false );
            try {
                block_exists = blockExists( info.getId(), *conn_ptr );
            } catch ( std::exception& e ) {
                // just log any error that occurs checking for block exists.
                LOG_WARN_MSG( "error when trying to check if block exists, " << e.what() );
            }

            if ( block_exists ) {
                BOOST_THROW_EXCEPTION( Exception( DUPLICATE, "block with matching ID already exists. Try a different name." ) );
            }
            throw; // throw the original exception if the block didn't exist.
        }
    }

    if ( info.getMidplanes().empty() ) {
        // Inserting small block.

        // Insert row(s) to the smallblock table.

        ColumnsBitmap sb_cols;
        sb_cols.set(DBTSmallblock::BLOCKID);
        sb_cols.set(DBTSmallblock::POSINMACHINE);
        sb_cols.set(DBTSmallblock::NODECARDPOS);

        DBTSmallblock db_sb( sb_cols );

        cxxdb::UpdateStatementPtr sb_ins_stmt_ptr(db_sb.prepareInsert( *conn_ptr ));

        sb_ins_stmt_ptr->parameters()[db_sb.BLOCKID_COL].set( info.getId() );
        sb_ins_stmt_ptr->parameters()[db_sb.POSINMACHINE_COL].set( info.getMidplane() );

        for ( NodeBoardPositions::const_iterator i(info.getNodeBoards().begin()) ; i != info.getNodeBoards().end() ; ++i ) {
            sb_ins_stmt_ptr->parameters()[db_sb.NODECARDPOS_COL].set( *i );
            sb_ins_stmt_ptr->execute();
        }
    } else {
        // Inserting large block.

        { // Insert the midplanes.

            ColumnsBitmap bpbm_cols;
            bpbm_cols.set( DBTBpblockmap::BPID );
            bpbm_cols.set( DBTBpblockmap::BLOCKID );
            bpbm_cols.set( DBTBpblockmap::ACOORD );
            bpbm_cols.set( DBTBpblockmap::BCOORD );
            bpbm_cols.set( DBTBpblockmap::CCOORD );
            bpbm_cols.set( DBTBpblockmap::DCOORD );

            DBTBpblockmap db_bpbm( bpbm_cols );

            cxxdb::UpdateStatementPtr bpb_ins_stmt_ptr(db_bpbm.prepareInsert( *conn_ptr ));

            bpb_ins_stmt_ptr->parameters()[db_bpbm.BLOCKID_COL].set( info.getId() );

            for ( BlockDatabaseInfo::Midplanes::const_iterator i(info.getMidplanes().begin()) ; i != info.getMidplanes().end() ; ++i ) {
                bpb_ins_stmt_ptr->parameters()[db_bpbm.BPID_COL].set( i->location );
                bpb_ins_stmt_ptr->parameters()[db_bpbm.ACOORD_COL].cast( i->coord[Dimension::A] );
                bpb_ins_stmt_ptr->parameters()[db_bpbm.BCOORD_COL].cast( i->coord[Dimension::B] );
                bpb_ins_stmt_ptr->parameters()[db_bpbm.CCOORD_COL].cast( i->coord[Dimension::C] );
                bpb_ins_stmt_ptr->parameters()[db_bpbm.DCOORD_COL].cast( i->coord[Dimension::D] );

                bpb_ins_stmt_ptr->execute();
            }
        }

        { // Insert the switches.

            ColumnsBitmap sbm_cols;
            sbm_cols.set( DBTSwitchblockmap::SWITCHID );
            sbm_cols.set( DBTSwitchblockmap::INCLUDE );
            sbm_cols.set( DBTSwitchblockmap::ENABLEPORTS );
            sbm_cols.set( DBTSwitchblockmap::BLOCKID );

            DBTSwitchblockmap db_sbm( sbm_cols );

            cxxdb::UpdateStatementPtr sbm_ins_stmt_ptr(db_sbm.prepareInsert( *conn_ptr ));

            sbm_ins_stmt_ptr->parameters()[db_sbm.BLOCKID_COL].set( info.getId() );

            for ( BlockDatabaseInfo::Switches::const_iterator i(info.getSwitches().begin()) ; i != info.getSwitches().end() ; ++i ) {
                sbm_ins_stmt_ptr->parameters()[db_sbm.SWITCHID_COL].set( i->id );
                sbm_ins_stmt_ptr->parameters()[db_sbm.INCLUDE_COL].set( i->config == SwitchConfig::Wrap ? 'F' : 'T' );
                sbm_ins_stmt_ptr->parameters()[db_sbm.ENABLEPORTS_COL].set(
                        i->config == SwitchConfig::Out ? 'O' : i->config == SwitchConfig::In ? 'I' : 'B'
                    );

                sbm_ins_stmt_ptr->execute();
            }
        }

        { // Insert the links.

            ColumnsBitmap lbm_cols;
            lbm_cols.set( DBTLinkblockmap::LINKID );
            lbm_cols.set( DBTLinkblockmap::BLOCKID );

            DBTLinkblockmap db_lbm( lbm_cols );

            cxxdb::UpdateStatementPtr lbm_ins_stmt_ptr(db_lbm.prepareInsert( *conn_ptr ));

            lbm_ins_stmt_ptr->parameters()[db_lbm.BLOCKID_COL].set( info.getId() );

            for ( BlockDatabaseInfo::Links::const_iterator i(info.getLinks().begin()) ; i != info.getLinks().end() ; ++i ) {
                lbm_ins_stmt_ptr->parameters()[db_lbm.LINKID_COL].set( *i );
                lbm_ins_stmt_ptr->execute();
            }
        }
    }

    conn_ptr->commit();
}


//---------------------------------------------------------------------
// External functions


// This version throws an exception.
void genBlockParamsToBlockDatabaseInfoEx(
        const GenBlockParams& genblock_params,
        BlockDatabaseInfo& db_info_out,
        bool do_check
    )
{
    bool is_large;
    DimensionSpecs dim_specs;

    try {
        is_large = genblock_params.isLarge();
        dim_specs = genblock_params.getDimensionSpecs();
    } catch ( exception& e ) {
        BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                "invalid arguments to " + __FUNCTION__ + ", " + e.what()
            ) );
    }

    LOG_DEBUG_MSG( string() + "Dim specs: [" + dim_specs[0].toString() + "," + dim_specs[1].toString() + "," + dim_specs[2].toString() + "," + dim_specs[3].toString() + "]" );

    try {
        db_info_out.setDescription( genblock_params.getDescription() );
        db_info_out.setOptions( genblock_params.getOptions() );
        db_info_out.setBootOptions( genblock_params.getBootOptions() );
        db_info_out.setMloaderImg( genblock_params.getMicroloaderImage() );
        db_info_out.setNodeConfig( genblock_params.getNodeConfiguration() );
        db_info_out.setOwner( genblock_params.getOwner() );

        BlockDatabaseInfo::ConnectivitySpecs connectivity = { {
                dim_specs[Dimension::A].getConnectivity(),
                dim_specs[Dimension::B].getConnectivity(),
                dim_specs[Dimension::C].getConnectivity(),
                dim_specs[Dimension::D].getConnectivity()
            } };
        db_info_out.setConnectivity( connectivity );

        if ( is_large ) {
            genBlockParamsToBlockDatabaseInfoLarge(
                    genblock_params,
                    db_info_out,
                    do_check
               );
        } else {
            genBlockParamsToBlockDatabaseInfoSmall(
                    genblock_params,
                    db_info_out
                );
        }
    } catch ( Exception& dbe ) {
        BOOST_THROW_EXCEPTION( Exception( dbe.getStatus(), string() +
                "failed to calculate block information, " + dbe.what()
            ) );
    } catch ( cxxdb::DatabaseException& cxxdbe ) {
        BOOST_THROW_EXCEPTION( Exception( DB_ERROR, string() +
                "failed to calculate block information, " + cxxdbe.what()
            ) );
    } catch ( std::invalid_argument& e ) {
        BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                "failed to calculate block information, " + e.what()
            ) );
    } catch ( std::exception& e ) {
        BOOST_THROW_EXCEPTION( Exception( FAILED, string() +
                "failed to calculate block information, " + e.what()
            ) );
    }
}


STATUS
genBlockParamsToBlockDatabaseInfo(
        const GenBlockParams& genblock_params,
        BlockDatabaseInfo& db_info_out
    )
{
    try {
        genBlockParamsToBlockDatabaseInfoEx(
                genblock_params,
                db_info_out
            );
    } catch ( Exception& dbe ) {
        LOG_WARN_MSG( dbe.what() << "\n" << genblock_params );
        return dbe.getStatus();
    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Failed to calculate block information, " << e.what() << "\n" << genblock_params );
        return FAILED;
    }

    return OK;
}


void
genBlockEx(
        const GenBlockParams& params,
        bool do_check
    )
{
    string id;
    try {
        id = params.getBlockId();
    } catch ( exception& e ) {
        BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                "invalid arguments to " + __FUNCTION__ + ", " + e.what()
            ) );
    }

    try {
        BlockDatabaseInfo db_info;

        db_info.setId( id );

        if ( params.getOwner().empty() ) {
            BOOST_THROW_EXCEPTION( Exception( INVALID_ARG, string() +
                                              "block owner was not set"
                                              ));
        }

        genBlockParamsToBlockDatabaseInfoEx( params, db_info, do_check );
        insertBlock( db_info );
    } catch ( Exception& dbe ) {
        BOOST_THROW_EXCEPTION( Exception( dbe.getStatus(), string() +
                "failed to create block '" + id + "', " + dbe.what()
            ) );
    } catch ( cxxdb::DatabaseException& cxxdbe ) {
        BOOST_THROW_EXCEPTION( Exception( DB_ERROR, string() +
                "failed to create block '" + id + "', " + cxxdbe.what()
            ) );
    } catch ( std::exception& e ) {
        BOOST_THROW_EXCEPTION( Exception( FAILED, string() +
                "failed to create block '" + id + "', " + e.what()
            ) );
    }
}


STATUS genBlock(
        const GenBlockParams& params
    )
{
    try {
        genBlockEx( params );
        LOG_DEBUG_MSG( "Created block '" << params.getBlockId() << "'\n" << params );
        return OK;
    } catch ( Exception& dbe ) {
        LOG_WARN_MSG( dbe.what() << "\n" << params );
        return dbe.getStatus();
    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Failed to create block, " << e.what() << "\n" << params );
        return FAILED;
    }
}


STATUS
genBlock(
        const std::string& id,
        const std::string& midplane,
        const std::string& owner
)
{
    GenBlockParams params;

    try {
        params.setBlockId( id );
        params.setDescription( string("Generated via ") + __FUNCTION__ );
        params.setMidplane( midplane );
        params.setOwner( owner );
        params.setDimensionSpecs( MidplaneDimensionSpecs );
    } catch ( exception& e ) {
        LOG_WARN_MSG( "Invalid arguments to " << __FUNCTION__ << ", " << e.what() );
        return INVALID_ARG;
    }

    STATUS status = genBlock( params );

    return status;
}


STATUS
genBlocks(
          const std::string& owner,
          const std::string& prefix
)
{
    DBVBasepartition bp;
    ColumnsBitmap colBitmap;
    int status;
    SQLRETURN sqlrc;

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    // Fields for checking midplane
    colBitmap.reset();
    colBitmap.set(bp.BPID);
    bp._columns = colBitmap.to_ulong();

    // Check all midplanes
    if ((status = tx.query(&bp))!= SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << status << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    status = tx.fetch(&bp);
    if (status != SQL_SUCCESS){
        LOG_ERROR_MSG(__FUNCTION__ << " Midplane not found");
        return NOT_FOUND;
    }

    const DBTBlock block;
    if (prefix.size() > (sizeof(block._blockid) - sizeof(bp._bpid))
            ||  (! checkIdentifierValidCharacters( prefix )))
    {
        LOG_ERROR_MSG(__FUNCTION__ << " block name prefix exceeds allowed size or has invalid characters");
        return INVALID_ID;
    }

    for (;status == SQL_SUCCESS;) {
        std::string blockid;
        if (prefix.empty()) {
            blockid.append(bp._bpid);
        } else {
            blockid.append(prefix);
            blockid.append(bp._bpid);
        }

        GenBlockParams params;

        try {
            params.setBlockId( blockid );
            params.setOwner( owner );
            params.setDescription( string("Generated via ") + __FUNCTION__ );
            params.setMidplane( bp._bpid );
            params.setDimensionSpecs( MidplaneDimensionSpecs );
        } catch ( exception& e ) {
            LOG_ERROR_MSG( "Cannot create params in " << __FUNCTION__ << ", " << e.what() );
            return DB_ERROR;
        }

        STATUS db_status = genBlock( params );

        if (db_status != OK){
            LOG_ERROR_MSG(__FUNCTION__ << " Database error in genblock: " << db_status);
            return DB_ERROR;
        }


        status = tx.fetch(&bp);
    }

    return OK;
}


STATUS
genFullBlock(
             const std::string& id,
             const std::string& owner
        )
{
    STATUS status;
    SQLRETURN sqlreturn;

    // The size of the block in each dimension is the size of the machine.
    uint32_t asize,bsize,csize,dsize;
    status = getMachineBPSize(asize, bsize, csize,dsize);
    if (status != OK) {
        LOG_ERROR_MSG( "Failed to get machine size for genFullBlock." );
        return DB_ERROR;
    }

    // Calculate the corner, query the database for the midplane at row=0, column=0, midplane=0
    TxObject tx(DBConnectionPool::Instance());

    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    DBVBasepartition bp;

    // fields for checking midplane
    ColumnsBitmap colBitmap;
    colBitmap.set(bp.BPID);
    bp._columns = colBitmap.to_ulong();

    string condition = " where row=0 and column=0 and midplane=0";

    // first check if BP exists, for the right machine
    if ((sqlreturn = tx.query(&bp,condition.c_str()))!= SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlreturn << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlreturn = tx.fetch(&bp))!= SQL_SUCCESS){
        LOG_ERROR_MSG(__FUNCTION__ << " Midplane not found");
        return NOT_FOUND;
    }

    GenBlockParams params;

    try {
        params.setBlockId( id );
        params.setOwner( owner );
        params.setDescription( string("Generated via ") + __FUNCTION__ );
        params.setMidplane( bp._bpid );
        params.setDimensionSpecs( FullDimensionSpecs );
    } catch ( exception& e ) {
        LOG_WARN_MSG( "Invalid arguments to " << __FUNCTION__ << ", " << e.what() );
        return INVALID_ARG;
    }

    status = genBlock( params );

    return status;
}


STATUS genMidplaneBlock(
        const std::string& id,
        const std::string& corner,
        unsigned int asize,
        unsigned int bsize,
        unsigned int csize,
        unsigned int dsize,
        const std::string& owner,
        const std::string& apt,
        const std::string& bpt,
        const std::string& cpt,
        const std::string& dpt
        )
{
    STATUS db_status;

    GenBlockParams params;

    try {
        params.setBlockId( id );
        params.setDescription( string("Generated via ") + __FUNCTION__ );
        params.setMidplane( corner );
        params.setOwner( owner );

        DimensionSpecs specs = { {
                DimensionSpec( asize, apt ),
                DimensionSpec( bsize, bpt ),
                DimensionSpec( csize, cpt ),
                DimensionSpec( dsize, dpt )
            } };

        params.setDimensionSpecs( specs );
    } catch ( exception& e ) {
        LOG_WARN_MSG( "Invalid input to " << __FUNCTION__ << ", " << e.what() );
        return INVALID_ARG;
    }

    db_status = genBlock( params );

    return db_status;
}


STATUS
genSmallBlock(
        const std::string& id,
        const std::string& midplane,
        uint32_t cnodes,
        const std::string& nodecard,
        const std::string& owner
)
{
    STATUS db_status;

    GenBlockParams params;

    try {
        params.setBlockId( id );
        params.setOwner( owner );
        params.setDescription( string("Generated via ") + __FUNCTION__ );

        params.setMidplane( midplane );
        params.setNodeBoardAndCount( nodecard, cnodes / Nodes_Per_Node_Board );
    } catch ( exception& e ) {
        LOG_WARN_MSG( "Invalid arguments to " << __FUNCTION__ << ", " << e.what() );
        return INVALID_ARG;
    }

    db_status = genBlock( params );
    return db_status;
}


STATUS
genIOBlock(
        const std::string& id,
        const std::string& location,
        unsigned int ionodes,
        const std::string& owner
)
{
    DBTBlock dbo;
    DBTIoblockmap iob;
    ColumnsBitmap colBitmap;
    int position;
    SQLRETURN sqlrc;

    if (!isBlockIdValid(id, __FUNCTION__)) {
        return INVALID_ID;
    }

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    sqlrc = tx.setAutoCommit(false);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database auto commit failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if (ionodes == 0) {
        LOG_ERROR_MSG(__FUNCTION__ << " Number of I/O nodes must be a positive integer");
        return FAILED;
    }

    if (location.size() != 10 && location.size()!= 6) {
        LOG_ERROR_MSG(__FUNCTION__ << " Location must be an I/O node or an I/O drawer");
        return FAILED;
    }

    if (location.size() == 10 && ionodes > 7) {
        LOG_ERROR_MSG(__FUNCTION__ << " Location must be an I/O drawer for 8 or more I/O nodes");
        return FAILED;
    }

    if (ionodes == 3 || ionodes == 5 || ionodes == 6 || ionodes == 7) {
        LOG_ERROR_MSG(__FUNCTION__ << " Invalid number of I/O nodes");
        return FAILED;
    }

    if ((ionodes == 2 &&
         location.substr(7,3) != "J00" && location.substr(7,3) != "J02" &&
         location.substr(7,3) != "J04" && location.substr(7,3) != "J06") ||
        (ionodes == 4 &&
         location.substr(7,3) != "J00" && location.substr(7,3) != "J04")) {
        LOG_ERROR_MSG(__FUNCTION__ << " Invalid location for number of I/O nodes");
        return FAILED;
    }


    if (location.size() == 6 && ionodes%8 != 0) {
        LOG_ERROR_MSG(__FUNCTION__ << " Number of I/O nodes must be a multiple of 8 when specifying an I/O drawer");
        return FAILED;
    }

    // check for valid locations, the checks above only checked the length
    string whereClause(" where location='");
    whereClause += location +string("'");
    int nrows;
    if (location.size() == 6) {
        nrows = tx.count("BGQIODrawer",whereClause.c_str());
    } else {
        nrows = tx.count("BGQIONode",whereClause.c_str());
    }
    if (nrows !=1 ) {
        LOG_ERROR_MSG(__FUNCTION__ << " Location must be a valid I/O drawer or I/O node");
        return FAILED;
    }

    colBitmap.reset();
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMIONODES);
    colBitmap.set(dbo.OWNER);
    colBitmap.set(dbo.NUMCNODES);
    colBitmap.set(dbo.NUMIONODES);
    colBitmap.set(dbo.DESCRIPTION);
    //    colBitmap.set(dbo.ISTORUS);    At this point, software doesn't support IO torus
    colBitmap.set(dbo.MLOADERIMG);
    colBitmap.set(dbo.NODECONFIG);

    dbo._columns = colBitmap.to_ulong();
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
    snprintf(dbo._owner, sizeof(dbo._owner), "%s", owner.c_str());
    snprintf(dbo._description, sizeof dbo._description, "%s", "Generated via genIOBlock");
    snprintf(dbo._mloaderimg, sizeof dbo._mloaderimg, "%s", DEFAULT_MLOADERIMG.c_str());
    snprintf(dbo._nodeconfig, sizeof dbo._nodeconfig, "%s", DEFAULT_IONODECONFIG.c_str());
    dbo._numcnodes = 0;
    dbo._numionodes = ionodes;

    // insert the block record - could fail if its a duplicate, or for other database errors
    //dbo.dump();
    sqlrc = tx.insert(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    colBitmap.reset();
    colBitmap.set(iob.BLOCKID);
    colBitmap.set(iob.LOCATION);
    iob._columns = colBitmap.to_ulong();

    snprintf(iob._blockid, sizeof(iob._blockid), "%s", id.c_str());
    snprintf(iob._location, sizeof(iob._location), "%s", location.c_str());


    // Handle the spanning of I/O racks
    DBVIodrawer iod;
    colBitmap.reset();
    colBitmap.set(iod.LOCATION);
    iod._columns = colBitmap.to_ulong();

    for (unsigned int nodes = 0 ; nodes < ionodes ; ) {
        sqlrc = tx.insert(&iob);
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
        if (location.size() == 6) {
            nodes += 8;

            // move to the next drawer
            if (nodes < ionodes) {
                sqlrc =  tx.query(&iod,(string(" where location >'") +  string(iob._location)  + "' order by location").c_str() );
                if ( (sqlrc = tx.fetch( &iod )) != SQL_SUCCESS ) {
                    LOG_ERROR_MSG( __FUNCTION__ << " Invalid I/O drawer specification. ");
                    return FAILED;
                } else {
                    snprintf(iob._location, sizeof(iob._location), "%s", iod._location);
                    tx.close(&iod);
                }
            }

        } else {
            nodes += 1;
            position = atoi(&iob._location[8]);
            sprintf(&iob._location[8],"%02d",++position);
        }
    }

    if ( (sqlrc = tx.commit()) != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database commit failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__  );
        return DB_ERROR;
    }

    return OK;
}


} // namespace BGQDB
