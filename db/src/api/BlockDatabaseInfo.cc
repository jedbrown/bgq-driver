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

#include "BlockDatabaseInfo.h"

#include <algorithm>
#include <iostream>

namespace BGQDB {

static bool operator==( const BlockDatabaseInfo::Midplanes& lhs, const BlockDatabaseInfo::Midplanes& rhs )
{
    return lhs.size() == rhs.size() && std::equal( lhs.begin(), lhs.end() , rhs.begin() );
}

static bool operator==( const BlockDatabaseInfo::Switches& lhs, const BlockDatabaseInfo::Switches& rhs )
{
    return lhs.size() == rhs.size() && std::equal( lhs.begin(), lhs.end() , rhs.begin() );
}

static std::ostream& operator<<( std::ostream& os, const BlockDatabaseInfo::ConnectivitySpecs& conn_specs )
{

    os << "[ ";
    for (int i = 0;  i < 3; ++i) {
        if (conn_specs[i] == BGQDB::Connectivity::Torus) {
            os << "Torus, ";
        } else {
            os << "Mesh, ";
        }
    }
    if (conn_specs[3] == BGQDB::Connectivity::Torus) {
        os << "Torus ]";
    } else {
        os << "Mesh ]";
    }
    return os;
}


static std::ostream& operator<<( std::ostream& os, const BlockDatabaseInfo::MidplaneInfo& mi )
{
    os << "{ loc=" << mi.location << " coords=" << mi.coord << " }";
    return os;
}


static std::ostream& operator<<( std::ostream& os, const BlockDatabaseInfo::SwitchInfo& si )
{
    os << "{ id=" << si.id;

    switch (si.config) {
    case BGQDB::SwitchConfig::Wrap:
        os << " config=Wrap";
        break;
    case BGQDB::SwitchConfig::Include:
        os << " config=Include";
        break;
    case BGQDB::SwitchConfig::Out:
        os << " config=Out";
        break;
    case BGQDB::SwitchConfig::In:
        os << " config=In";
        break;
    }

    if (si.used_for_passthrough) {
        os << " passthrough=true";
    } else {
        os << " passthrough=false";
    }

    os << " }";

    return os;
}


void BlockDatabaseInfo::addLink( const std::string& link_id )
{
    _links.push_back( link_id );
    //std::sort( _links.begin(), _links.end() );
}


bool BlockDatabaseInfo::operator==( const BlockDatabaseInfo& other ) const
{
    return (
         _id == other._id &&
         _description == other._description &&
         _options == other._options &&
         _boot_options == other._boot_options &&
         _mloader_img == other._mloader_img &&
         _node_config == other._node_config &&
         _sizes == other._sizes &&
         _connectivity == other._connectivity &&
         _mps == other._mps &&
         _passthru_mps == other._passthru_mps &&
         _switches == other._switches &&
         _links == other._links &&
         _midplane == other._midplane &&
         _node_boards == other._node_boards
     );
}


std::ostream& operator<<( std::ostream& os, const BlockDatabaseInfo& bdi )
{
    os << "{\n"
          "\tid=" << bdi.getId() << "\n"
          "\tdescription=" << bdi.getDescription() << "\n"
          "\toptions=" << bdi.getOptions() << "\n"
          "\tboot_options=" << bdi.getBootOptions() << "\n"
          "\tmloader_img=" << bdi.getMloaderImg() << "\n"
          "\tnode_config=" << bdi.getNodeConfig() << "\n"
          "\tsizes=" << bdi.getSizes() << "\n"
          "\tconnectivity=" << bdi.getConnectivitySpecs() << "\n";

    const BlockDatabaseInfo::Midplanes &mps(bdi.getMidplanes());

    if ( ! mps.empty() ) {
        os << "\tmidplanes=[\n";

        for ( BlockDatabaseInfo::Midplanes::const_iterator i(mps.begin()) ; i != mps.end() ; ++i ) {
            os << "\t\t" << *i << ",\n";
        }

        os << "\t]\n";
    }

    const BlockDatabaseInfo::Midplanes &passthru_mps(bdi.getPassthroughMidplanes());

    if ( ! passthru_mps.empty() ) {
        os << "\tPassthru midplanes=[\n";

        for ( BlockDatabaseInfo::Midplanes::const_iterator i(passthru_mps.begin()) ; i != passthru_mps.end() ; ++i ) {
            os << "\t\t" << *i << ",\n";
        }

        os << "\t]\n";
    }

    const BlockDatabaseInfo::Switches &switches(bdi.getSwitches());

    if ( ! switches.empty() ) {
        os << "\tswitches=[\n";

        for ( BlockDatabaseInfo::Switches::const_iterator i(switches.begin()) ; i != switches.end() ; ++i ) {
            os << "\t\t" << *i << ",\n";
        }

        os << "\t]\n";
    }

    const BlockDatabaseInfo::Links &links(bdi.getLinks());

    if ( ! links.empty() ) {
        os << "\tlinks=[\n";

        for ( BlockDatabaseInfo::Links::const_iterator i(links.begin()) ; i != links.end() ; ++i ) {
            os << "\t\t" << *i << ",\n";
        }

        os << "\t]\n";
    }

    if ( ! bdi.getMidplane().empty() ) {
        os << "\tmidplane=" << bdi.getMidplane() << "\n";
    }

    const NodeBoardPositions &nbs(bdi.getNodeBoards());

    if ( ! nbs.empty() ) {
        os << "\tnode boards=[\n";

        for ( NodeBoardPositions::const_iterator i(nbs.begin()) ; i != nbs.end() ; ++i ) {
            os << "\t\t" << *i << ",\n";
        }

        os << "\t]\n";
    }

    os << "}\n";

    return os;
}


} // namespace BGQDB
