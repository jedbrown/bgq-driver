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

// The DefaultControlEventListener receives and processes RAS events
// when there is no specific EventListener registered for the location
// generating the RAS event.
//

#include "DefaultControlEventListener.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"

#include <control/include/mcServer/MCServerRef.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEvent.h>

#include <utility/include/Log.h>

#include <boost/foreach.hpp>

#include <pthread.h>


LOG_DECLARE_FILE( "mmcs" );


// singleton instance
// Note that this is not serialized by a mutex.
// Serialization must be managed by the caller.
DefaultControlEventListener* DefaultControlEventListener::_defaultControlEventListener = NULL;

DefaultControlEventListener*
DefaultControlEventListener::getDefaultControlEventListener()
{
    if (_defaultControlEventListener == NULL)
        _defaultControlEventListener = new DefaultControlEventListener;
    return _defaultControlEventListener;
}

DefaultControlEventListener::DefaultControlEventListener()
    : DBBlockController("mmcs", "DefaultControlEventListener"),
    _connection(),
    _query()
{
    // initialize machine XML
    MMCSCommandReply bogus;
    this->initMachineConfig( bogus );

    // iterate through I/O drawers and nodes in the machine to open a log file for each
    // one. This allows the default event listener to handle console events from I/O node
    // locations.
    const BlockPtr& base = this->getBase();
    const BGQMachineXML* machine = base->getMachineXML();
    BOOST_FOREACH( const BGQMachineIOBoard* i, machine->_ioBoards ) {
        BOOST_FOREACH( const BGQMachineNode* j, i->_nodes ) {
            const std::string location( i->_board + "-" + j->_location );
            LOG_DEBUG_MSG( location );

            // add BCNodeInfo so findNodeInfo( location ) will work
            BCNodeInfo* nodeInfo = new BCNodeInfo();
            nodeInfo->_location = std::string( i->_board + "-" + j->_location );

            BlockControllerBase::openLog( nodeInfo );

            base->_targetLocationMap.insert(
                    std::make_pair( nodeInfo->_location, nodeInfo )
                    );
            base->_nodes.push_back( nodeInfo );
        }
    }

    // assign locate ID to each I/O node
    unsigned locateid = 0;
    BOOST_FOREACH( BCNodeInfo* i, base->_nodes ) {
        i->_locateId = locateid++;
    }

    this->prepareQuery();
}

void
DefaultControlEventListener::prepareQuery()
{
    _connection = BGQDB::DBConnectionPool::Instance().getConnection();
    if ( !_connection ) {
        LOG_WARN_MSG( "could not get database connection" );
        return;
    }

    // this query is used to select the block ID for compute node and node board RAS events
    // that arrive on this default event listener
    _query = _connection->prepareQuery(
                "SELECT sb.blockid, b.status "
                "FROM BGQSmallBlock sb, BGQBlock b "
                "WHERE nodecardpos=? AND posinmachine=? AND sb.blockid = b.blockid AND b.status<>'F' "
                "UNION SELECT bm.blockid, b.status "
                "FROM BGQBpblockmap bm, BGQBlock b "
                "WHERE bpid=? AND bm.blockid = b.blockid and b.status<>'F'",
                // container of parameter names
                boost::assign::list_of
                (BGQDB::DBTSmallblock::NODECARDPOS_COL)
                (BGQDB::DBTSmallblock::POSINMACHINE_COL)
                (BGQDB::DBTBpblockmap::BPID_COL)
            );
    if ( !_query ) {
        LOG_WARN_MSG( "could not create query" );
        return;
    }
}

DefaultControlEventListener::~DefaultControlEventListener()
{
    disconnect();
}

void
DefaultControlEventListener::disconnect()
{
    PthreadMutexHolder mutex;
    mutex.Lock(&getMutex());   // serialize this code

    if (!getBase()->isConnected())
        return;

    LOG_INFO_MSG("disconnect() releasing block resources");

    // Wait for the mailbox I/O to quiesce, then stop the MailboxMonitor
    getBase()->quiesceMailbox(0);

    // Do this a bit more orderly
    getBase()->stopMailbox();
}
    
int
DefaultControlEventListener::processRASMessage(RasEvent& rasEvent)
{
    LOG_DEBUG_MSG( __FUNCTION__ );

    // short circuit if we failed to prepare the query in the ctor
    if ( !_query ) return DBBlockController::processRASMessage(rasEvent);

    // short circuit if we already have a block ID provided
    if ( !rasEvent.getDetail(RasEvent::BLOCKID).empty() ) return DBBlockController::processRASMessage(rasEvent);

    // no need to handle empty locations
    if ( rasEvent.getDetail(RasEvent::LOCATION).empty() ) return DBBlockController::processRASMessage(rasEvent);

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
                // for all other locations, proceed to processing
                return DBBlockController::processRASMessage(rasEvent);
        }
        _query->parameters()[ BGQDB::DBTSmallblock::NODECARDPOS_COL ].set( location.getLocation().substr(7,3) );
        _query->parameters()[ BGQDB::DBTSmallblock::POSINMACHINE_COL].set( location.getMidplaneLocation() );
        _query->parameters()[ BGQDB::DBTBpblockmap::BPID_COL].set(location.getMidplaneLocation() );

        const cxxdb::ResultSetPtr results( _query->execute() );
        if ( !results ) {
            LOG_WARN_MSG( "could not create result set" );
            return DBBlockController::processRASMessage(rasEvent);
        }
        if ( !results->fetch() ) {
            LOG_DEBUG_MSG( "no results found for " << location.getLocation() );
        } else {
            rasEvent.setDetail( RasEvent::BLOCKID, results->columns()["blockid"].getString() );
            LOG_DEBUG_MSG( "updated " << location.getLocation() << " block name to '" << results->columns()["blockid"].getString() << "'" );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return DBBlockController::processRASMessage(rasEvent);
}

