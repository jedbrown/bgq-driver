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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include "BlockControllerBase.h"
#include "NeighborInfo.h"

#include <control/include/bgqconfig/BGQPersonality.h>
#include <control/include/bgqconfig/BGQTopology.h>

#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/DBTCable.h>
#include <db/include/api/tableapi/gensrc/DBVCniolink.h>
#include <db/include/api/BGQDBlib.h>

#include <utility/include/CableBadWires.h>
#include <utility/include/Log.h>

#include <boost/throw_exception.hpp>
#include <boost/assign/list_of.hpp>

LOG_DECLARE_FILE( "mmcs" );

#define NO_COORD (-1)

const std::set<int> NeighborInfo::MessageIds = boost::assign::list_of
    (0x00080018)
    (0x00080019)
    (0x00080037)
    (0x00080038)
    (0x00080039)
    (0x000900A0)
    (0x000900A1)
    (0x000900B0)
    (0x000900B1)
    (0x000900C0)
    (0x000900C1)
    (0x000900D0)
    (0x000900D1)
    (0x000900E0)
    (0x000900E1)
    (0x000900F0)
    (0x00090104)
    (0x00090105)
    (0x000901F1)
    (0x00090200)
    (0x00090201)
    (0x00090202)
    (0x0009020D)
    ;

NeighborInfo::NeighborInfo(
        const boost::shared_ptr<BlockControllerBase>& block,
        const char* location,
        char* rawdata
        ) :
    _location( location )
{
    switch ( _location.getType() ) {
        case bgq::util::Location::ComputeCardOnNodeBoard:
        case bgq::util::Location::ComputeCardOnIoBoard:
            this->compute( block, rawdata );
            break;
        case bgq::util::Location::LinkModuleOnNodeBoard:
        case bgq::util::Location::LinkModuleOnIoBoardInIoRack:
        case bgq::util::Location::LinkModuleOnIoBoardInComputeRack:
            this->linkChip( block, rawdata );
            break;
        default:
            BOOST_THROW_EXCEPTION(
                    std::invalid_argument(
                        std::string(location) + " is not a compute node or link chip location"
                        )
                    );
    }
}

void
NeighborInfo::compute( 
        const boost::shared_ptr<BlockControllerBase>& block,
        char* rawdata
        )
{
    BCNodeInfo* const nodeInfo =  block->findNodeInfo(_location);
    if ( !nodeInfo ) return;

    char direction[3] = {0};
    const char* match = strstr(rawdata, "DIM_DIR=");
    if ( match && sscanf(match, "DIM_DIR=%2s;", direction) != 1 ) {
        LOG_WARN_MSG( "missing DIM_DIR key in event from " << _location );
        LOG_WARN_MSG( rawdata );
        return;
    }

    char linkInfo[256] = {0};
    int coord = -1;
    BCNodeInfo *neighbor = NULL;
    if ( !strcmp(direction, "A+") ) {
        const std::string fromPos(nodeInfo->midplanePos());
        coord = next_coord(nodeInfo->personality().Network_Config.Acoord,
                nodeInfo->personality().Network_Config.Anodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_A);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(coord,
                    nodeInfo->personality().Network_Config.Bcoord,
                    nodeInfo->personality().Network_Config.Ccoord,
                    nodeInfo->personality().Network_Config.Dcoord,
                    nodeInfo->personality().Network_Config.Ecoord);

            if (neighbor) {
                BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), neighbor->_pos.nodeCard(),
                        fromPos,neighbor->midplanePos(),
                        (coord == 0) , direction, linkInfo);
            }
        }
    } else if ( !strcmp(direction, "A-") ) {
        const std::string fromPos(nodeInfo->midplanePos());
        coord = prev_coord(nodeInfo->personality().Network_Config.Acoord,
                nodeInfo->personality().Network_Config.Anodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_A);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(coord,
                    nodeInfo->personality().Network_Config.Bcoord,
                    nodeInfo->personality().Network_Config.Ccoord,
                    nodeInfo->personality().Network_Config.Dcoord,
                    nodeInfo->personality().Network_Config.Ecoord);

            if (neighbor) {
                BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), neighbor->_pos.nodeCard(),
                        fromPos,neighbor->midplanePos(),
                        (nodeInfo->personality().Network_Config.Acoord == 0), direction, linkInfo);
            }
        }
    } else if ( !strcmp(direction, "B+") ) {
        const std::string fromPos(nodeInfo->midplanePos());
        coord = next_coord(nodeInfo->personality().Network_Config.Bcoord,
                nodeInfo->personality().Network_Config.Bnodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_B);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                    coord,
                    nodeInfo->personality().Network_Config.Ccoord,
                    nodeInfo->personality().Network_Config.Dcoord,
                    nodeInfo->personality().Network_Config.Ecoord);

            if (neighbor) {
                BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), neighbor->_pos.nodeCard(),
                        fromPos,neighbor->midplanePos(),
                        (coord == 0) , direction, linkInfo);
            }
        }
    } else if ( !strcmp(direction, "B-") ) {
        const std::string fromPos(nodeInfo->midplanePos());
        coord = prev_coord(nodeInfo->personality().Network_Config.Bcoord,
                nodeInfo->personality().Network_Config.Bnodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_B);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                    coord,
                    nodeInfo->personality().Network_Config.Ccoord,
                    nodeInfo->personality().Network_Config.Dcoord,
                    nodeInfo->personality().Network_Config.Ecoord);

            if (neighbor) {
                BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), neighbor->_pos.nodeCard(),
                        fromPos,neighbor->midplanePos(),
                        (nodeInfo->personality().Network_Config.Bcoord == 0), direction, linkInfo);
            }
        }
    } else if ( !strcmp(direction, "C+") ) {
        const std::string fromPos(nodeInfo->midplanePos());
        coord = next_coord(nodeInfo->personality().Network_Config.Ccoord,
                nodeInfo->personality().Network_Config.Cnodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_C);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                    nodeInfo->personality().Network_Config.Bcoord,
                    coord,
                    nodeInfo->personality().Network_Config.Dcoord,
                    nodeInfo->personality().Network_Config.Ecoord);

            if (neighbor) {
                BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), neighbor->_pos.nodeCard(),
                        fromPos,neighbor->midplanePos(),
                        (coord == 0) , direction, linkInfo);
            }
        }
    } else if ( !strcmp(direction, "C-") ) {
        const std::string fromPos(nodeInfo->midplanePos());
        coord = prev_coord(nodeInfo->personality().Network_Config.Ccoord,
                nodeInfo->personality().Network_Config.Cnodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_C);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                    nodeInfo->personality().Network_Config.Bcoord,
                    coord,
                    nodeInfo->personality().Network_Config.Dcoord,
                    nodeInfo->personality().Network_Config.Ecoord);

            if (neighbor) {
                BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), neighbor->_pos.nodeCard(),
                        fromPos,neighbor->midplanePos(),
                        (nodeInfo->personality().Network_Config.Ccoord == 0), direction, linkInfo);
            }
        }
    } else if ( !strcmp(direction, "D+") && nodeInfo ) {
        const std::string fromPos(nodeInfo->midplanePos());
        coord = next_coord(nodeInfo->personality().Network_Config.Dcoord,
                nodeInfo->personality().Network_Config.Dnodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_D);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                    nodeInfo->personality().Network_Config.Bcoord,
                    nodeInfo->personality().Network_Config.Ccoord,
                    coord,
                    nodeInfo->personality().Network_Config.Ecoord);

            if (neighbor) {
                BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), neighbor->_pos.nodeCard(),
                        fromPos,neighbor->midplanePos(),
                        (coord == 0) , direction, linkInfo);
            }
        }
    } else if ( !strcmp(direction, "D-") ) {
        const std::string fromPos(nodeInfo->midplanePos());
        coord = prev_coord(nodeInfo->personality().Network_Config.Dcoord,
                nodeInfo->personality().Network_Config.Dnodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_D);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                    nodeInfo->personality().Network_Config.Bcoord,
                    nodeInfo->personality().Network_Config.Ccoord,
                    coord,
                    nodeInfo->personality().Network_Config.Ecoord);

            if (neighbor) {
                BGQTopology::neighborInfo(nodeInfo->_pos.jtagPort(), nodeInfo->_pos.nodeCard(), neighbor->_pos.nodeCard(),
                        fromPos,neighbor->midplanePos(),
                        (nodeInfo->personality().Network_Config.Dcoord == 0), direction, linkInfo);
            }
        }
    } else if ( !strcmp(direction, "E-") || !strcmp(direction, "E+") ) {
        coord = next_coord(nodeInfo->personality().Network_Config.Ecoord,
                nodeInfo->personality().Network_Config.Enodes,
                nodeInfo->personality().Network_Config.NetFlags & ND_ENABLE_TORUS_DIM_E);

        if (coord != NO_COORD) {
            neighbor = block->findNodeInfo(nodeInfo->personality().Network_Config.Acoord,
                    nodeInfo->personality().Network_Config.Bcoord,
                    nodeInfo->personality().Network_Config.Ccoord,
                    nodeInfo->personality().Network_Config.Dcoord,
                    coord);
            strcpy(linkInfo," Connects through node board");
        }
    } else if ( !strcmp(direction, "IO") ) {
        this->ioLink( rawdata );
        return;
    }

    strcat(rawdata,"NEIGHBOR=");
    if ( neighbor ) {
        strcat(rawdata,neighbor->location().c_str());
    } else {
        strcat(rawdata,"N/A");
    }
    strcat(rawdata, linkInfo);

}

void
NeighborInfo::ioLink(
        char* rawdata
        )
{
    // using the location that generated this event, get the port locations and I/O node
    // neighbor from the cniolink view and cable table
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    const cxxdb::ResultSetPtr result(
            connection->query(
                "SELECT " +
                BGQDB::DBVCniolink::DESTINATION_COL + ", " +
                BGQDB::DBVCniolink::ION_COL + ", " +
                BGQDB::DBTCable::FROMLOCATION_COL + " FROM " +
                BGQDB::DBVCniolink().getTableName() + ", " +
                BGQDB::DBTCable().getTableName() + " WHERE " +
                BGQDB::DBVCniolink::SOURCE_COL + "=" +
                "'" + _location.getLocation() + "' AND " + 
                BGQDB::DBVCniolink::DESTINATION_COL + "=" +
                BGQDB::DBTCable::TOLOCATION_COL
                )
            );
    if ( !result->fetch() ) {
        LOG_WARN_MSG( "could not find neighbor for " << _location );
        return;
    }

    const std::string neighbor( result->columns()[BGQDB::DBVCniolink::ION_COL].getString() );
    const std::string ioport( result->columns()[BGQDB::DBVCniolink::DESTINATION_COL].getString());
    const std::string cnport( result->columns()[BGQDB::DBTCable::FROMLOCATION_COL].getString() );
    const std::map<std::string,std::string> ioLinkChip = boost::assign::map_list_of
        ("T00","U05")
        ("T01","U00")
        ("T02","U05")
        ("T03","U00")
        ("T04","U05")
        ("T05","U00")
        ("T06","U05")
        ("T07","U00")
        ("T08","U04")
        ("T09","U01")
        ("T10","U04")
        ("T11","U01")
        ("T12","U04")
        ("T13","U01")
        ("T14","U04")
        ("T15","U01")
        ("T16","U03")
        ("T17","U02")
        ("T18","U03")
        ("T19","U02")
        ("T20","U03")
        ("T21","U02")
        ("T22","U03")
        ("T23","U02")
        ;
    const std::map<std::string,std::string>::const_iterator linkChip = ioLinkChip.find( ioport.substr(7,3) );

    strcat(rawdata,"NEIGHBOR=");
    strcat(rawdata,neighbor.c_str());
    strcat(rawdata," Connects through link chips " );
    strcat(rawdata,_location.getLocation().substr(0,10).c_str());
    strcat(rawdata,"-U04 and ");
    strcat(rawdata,neighbor.substr(0,7).c_str());
    if ( linkChip != ioLinkChip.end() ) {
        strcat(rawdata,linkChip->second.c_str());
    } else {
        strcat(rawdata,"N/A");
    }
    strcat(rawdata," using cable from port ");
    strcat(rawdata,cnport.c_str());
    strcat(rawdata," to " );
    strcat(rawdata,ioport.c_str());
}

void
NeighborInfo::linkChip(
        const boost::shared_ptr<BlockControllerBase>& block,
        char* rawdata
        )
{
    char* matchMask = strstr(rawdata, "Mask=");
    if ( !matchMask ) {
        LOG_ERROR_MSG( "missing Mask= in raw data for " << _location );
        return;
    }
    char* matchReg = strstr(rawdata, "Register=");
    if ( !matchReg ) {
        LOG_ERROR_MSG( "missing Register= in raw data for " << _location );
        return;
    }

    char regFromEvent[33] = {0};
    int maskInt = 0;
    sscanf(matchMask, "Mask=%x", &maskInt);
    sscanf(matchReg, "Register=%s", regFromEvent);

    try  {
        bgq::utility::CableBadWires badWires(_location, std::string(regFromEvent).substr(0,3), maskInt);
        const std::string portLoc = badWires.getPortLocation();

        // using the location that generated this event, get the port locations and I/O node
        // neighbor from the cniolink view and cable table
        const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
        const cxxdb::ResultSetPtr result(
                connection->query(
                    "SELECT " +
                    BGQDB::DBTCable::TOLOCATION_COL + ", " +
                    BGQDB::DBTCable::FROMLOCATION_COL + ", " +
                    BGQDB::DBTCable::BADWIREMASK_COL + " FROM " +
                    BGQDB::DBTCable().getTableName() + " WHERE " +
                    BGQDB::DBTCable::TOLOCATION_COL +  "='" + portLoc + "' OR " +
                    BGQDB::DBTCable::FROMLOCATION_COL +  "='" + portLoc + "'"
                    )
                );
        if ( !result->fetch() ) {
            LOG_WARN_MSG( "could not find neighbor for " << _location );
            return;
        }
        const std::string from( result->columns()[BGQDB::DBTCable::FROMLOCATION_COL].getString() );
        const std::string to( result->columns()[BGQDB::DBTCable::TOLOCATION_COL].getString() );
        const uint64_t mask ( result->columns()[BGQDB::DBTCable::BADWIREMASK_COL].as<int64_t>() );
        if ( from.substr(0,6) == _location.getLocation().substr(0,6) ) {
            badWires.setTxPortAndBadWireMask(to, mask);
        } else {
            badWires.setTxPortAndBadWireMask(from, mask);
        }
        strcat(rawdata,"NEIGHBOR=");
        strcat(rawdata,badWires.getTxLinkChipLocation().c_str());
        strcat(rawdata, " using cable from " );
        strcat(rawdata, from.c_str() );
        strcat(rawdata, " to " );
        strcat(rawdata, to.c_str() );
    } catch ( std::exception& e ) {
        LOG_ERROR_MSG( e.what() );
    }
}

int
NeighborInfo::next_coord(
        unsigned start,
        const unsigned size,
        const bool torus
        )
{
    if (++start == size) {
        if (torus)
            return 0;
        else
            return NO_COORD;
    }

    return start;
}

int
NeighborInfo::prev_coord(
        unsigned start, 
        const unsigned size,
        const bool torus
        )
{
    if (start == 0) {
	if (torus)
	    return size-1;
	else
            return NO_COORD;
    }

    return --start;
}


