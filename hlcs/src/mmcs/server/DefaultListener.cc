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

// The DefaultListener receives and processes RAS events when there is no specific
// EventListener registered for the location generating the RAS event.

#include "DefaultListener.h"

#include "BCNodeInfo.h"
#include "BlockControllerBase.h"

#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>
#include <control/include/mcServer/defaults.h>

#include <db/include/api/tableapi/gensrc/DBTBpblockmap.h>
#include <db/include/api/tableapi/gensrc/DBTSmallblock.h>
#include <db/include/api/tableapi/gensrc/DBVCniolink.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEvent.h>

#include <utility/include/Log.h>

#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <pthread.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

// Singleton instance.
// Note that this is not serialized by a mutex. Serialization must be managed by the caller.
DefaultListener* DefaultListener::_instance = NULL;

DefaultListener*
DefaultListener::get()
{
    if (_instance == NULL) {
        _instance = new DefaultListener;
    }
    return _instance;
}

DefaultListener::DefaultListener() :
    DBBlockController("mmcs", mc_server::DefaultListener),
    _connection(),
    _computeQuery(),
    _mutex(),
    _io()
{
    // Initialize machine XML
    mmcs_client::CommandReply reply;
    this->initMachineConfig( reply );
    if ( reply.getStatus() ) {
        LOG_ERROR_MSG( reply.str() );
        throw std::runtime_error( reply.str() );
    }

    // Iterate through I/O drawers and nodes in the machine to open a log file for each one.
    // This allows the default event listener to handle console events from I/O node locations.
    const BlockPtr& base = this->getBase();
    const BGQMachineXML* machine = base->_machineXML;
    BOOST_FOREACH( const BGQMachineIOBoard* i, machine->_ioBoards ) {
        BOOST_FOREACH( const BGQMachineNode* j, i->_nodes ) {
            // const std::string location( i->_board + "-" + j->_location );
            // LOG_TRACE_MSG( location );

            // Add BCNodeInfo so findNodeInfo( location ) will work
            BCNodeInfo* const nodeInfo = new BCNodeInfo();
            nodeInfo->_location = std::string( i->_board + "-" + j->_location );

            BlockControllerBase::openLog( nodeInfo );

            base->_targetLocationMap.insert(std::make_pair( nodeInfo->_location, nodeInfo ));
            base->_nodes.push_back( nodeInfo );
        }
    }

    // Assign locate ID to each I/O node
    unsigned locateid = 0;
    BOOST_FOREACH( BCNodeInfo* const i, base->_nodes ) {
        i->_locateId = locateid++;
    }

    this->prepareQuery();
}

void
DefaultListener::prepareQuery()
{
    _connection = BGQDB::DBConnectionPool::Instance().getConnection();
    if ( !_connection ) {
        LOG_WARN_MSG( "Could not get database connection." );
        return;
    }

    // This query is used to select the block ID for compute node and node board RAS events
    // that arrive on this default event listener
    _computeQuery = _connection->prepareQuery(
                "SELECT sb.blockid, b.status "
                "FROM BGQSmallBlock sb, BGQBlock b "
                "WHERE nodecardpos=? AND posinmachine=? AND sb.blockid = b.blockid AND b.status<>'F' "
                "UNION SELECT bm.blockid, b.status "
                "FROM BGQBpblockmap bm, BGQBlock b "
                "WHERE bpid=? AND bm.blockid = b.blockid and b.status<>'F'",
                // Container of parameter names
                boost::assign::list_of
                (BGQDB::DBTSmallblock::NODECARDPOS_COL)
                (BGQDB::DBTSmallblock::POSINMACHINE_COL)
                (BGQDB::DBTBpblockmap::BPID_COL)
            );
    if ( !_computeQuery ) {
        LOG_WARN_MSG( "Could not create compute node query." );
        return;
    }
}

DefaultListener::~DefaultListener()
{
    disconnect();
}

void
DefaultListener::disconnect()
{
    PthreadMutexHolder mutex;
    mutex.Lock(&getMutex());   // serialize this code

    if (!getBase()->isConnected()) {
        return;
    }

    LOG_INFO_MSG(__FUNCTION__ << "() releasing block resources.");

    // Do this a bit more orderly
    getBase()->stopMailbox();
}

int
DefaultListener::processRASMessage(
        RasEvent& rasEvent
        )
{
    LOG_DEBUG_MSG( rasEvent );

    // Short circuit if we failed to prepare the query in the ctor
    if ( !_computeQuery ) {
        return DBBlockController::processRASMessage(rasEvent);
    }

    // Short circuit if we already have a block ID provided
    if ( !rasEvent.getDetail(RasEvent::BLOCKID).empty() ) {
        return DBBlockController::processRASMessage(rasEvent);
    }

    // No need to handle empty locations
    if ( rasEvent.getDetail(RasEvent::LOCATION).empty() ) {
        return DBBlockController::processRASMessage(rasEvent);
    }

    try {
        const bgq::util::Location location( rasEvent.getDetail(RasEvent::LOCATION) );
        switch ( location.getType() ) {
            case bgq::util::Location::ComputeCardOnNodeBoard:
            case bgq::util::Location::NodeBoard:
                // fall through
                // for compute node and board locations, look up the block name since we
                // can get these events on the default event listener (instead of a specific block listener)
                // when mmcs_server starts up after a failover
                break;
            default:
                // For all other locations, proceed to processing
                return DBBlockController::processRASMessage(rasEvent);
        }
        _computeQuery->parameters()[ BGQDB::DBTSmallblock::NODECARDPOS_COL ].set( location.getLocation().substr(7,3) );
        _computeQuery->parameters()[ BGQDB::DBTSmallblock::POSINMACHINE_COL].set( location.getMidplaneLocation() );
        _computeQuery->parameters()[ BGQDB::DBTBpblockmap::BPID_COL].set(location.getMidplaneLocation() );

        const cxxdb::ResultSetPtr results( _computeQuery->execute() );
        if ( !results ) {
            LOG_WARN_MSG( "Could not create result set." );
            return DBBlockController::processRASMessage(rasEvent);
        }
        if ( !results->fetch() ) {
            LOG_DEBUG_MSG( "No results found for " << location.getLocation() );
        } else {
            rasEvent.setDetail( RasEvent::BLOCKID, results->columns()["blockid"].getString() );
            LOG_DEBUG_MSG( "Updated " << location.getLocation() << " block name to '" << results->columns()["blockid"].getString() << "'" );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return DBBlockController::processRASMessage(rasEvent);
}

void
DefaultListener::processConsoleMessage(
        MCServerMessageSpec::ConsoleMessage& message
        )
{
    try {
        const bgq::util::Location location(
                message._cardLocation + "-" +
                BGQTopology::processorCardNameFromJtagPort(message._jtagPort)
                );
        switch ( location.getType() ) {
            case bgq::util::Location::ComputeCardOnIoBoard:
                // fall through
                break;
            default:
                return DBBlockController::processConsoleMessage(message);
        }

        // Use equal_range here instead of find since there can be more than one
        // compute block associated with an I/O node
        std::pair<IoMap::iterator, IoMap::iterator> blocks;
        {
            boost::mutex::scoped_lock lock( _mutex );
            blocks = _io.equal_range( location.getLocation() );
        }

        std::string blockName;
        for ( IoMap::iterator i = blocks.first; i != blocks.second; ++i ) {
            if ( !blockName.empty() ) {
                blockName.append( "," );
            }
            blockName.append( i->second );
        }

        if ( blockName.empty() ) {
            return DBBlockController::processConsoleMessage(message);
        }

        this->getBase()->_blockName.swap( blockName );
        DBBlockController::processConsoleMessage(message);
        this->getBase()->_blockName.swap( blockName );
        return;
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return DBBlockController::processConsoleMessage(message);
}

void
DefaultListener::add(
        const BGQBlockNodeConfig* block
        )
{
    BOOST_ASSERT( block );

    if ( block->ioboardBegin() != block->ioboardEnd() ) {
        // Skip I/O blocks
        return;
    }

    // Mapping of I/O node location to compute block ID
    typedef std::set<std::string> Io;
    Io io;

    for (
            BGQBlockNodeConfig::midplane_iterator i = block->midplaneBegin();
            i != block->midplaneEnd();
            ++i
        )
    {
        BOOST_FOREACH( const BGQNodePos& node, *i->computes() ) {
            const std::string location( i->posInMachine() + "-" + boost::lexical_cast<std::string>(node) );
            const std::pair<Io::iterator, bool> result(io.insert(block->connectedIONode(location)));
            if ( result.second ) {
                LOG_TRACE_MSG( __FUNCTION__ << " " << *result.first );
            }
        }
    }

    boost::mutex::scoped_lock lock( _mutex );
    for ( Io::const_iterator i = io.begin(); i != io.end(); ++i ) {
        _io.insert(IoMap::value_type( *i, block->blockName()));
    }
}

void
DefaultListener::remove(
        const std::string& block
        )
{
    boost::mutex::scoped_lock lock( _mutex );

    // Remove all entries for this block
    for ( IoMap::iterator i = _io.begin(); i != _io.end(); ) {
        const IoMap::iterator erase_iter( i++ );
        if ( erase_iter->second == block ) {
            //LOG_TRACE_MSG( __FUNCTION__ << " " << erase_iter->first );
            _io.erase( erase_iter );
        }
    }
}

} } // namespace mmcs::server
