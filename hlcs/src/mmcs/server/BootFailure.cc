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

#include "BootFailure.h"

#include "BCNodeInfo.h"
#include "BlockControllerBase.h"
#include "CNBlockController.h"
#include "ras.h"

#include <db/include/api/BGQDBlib.h>
#include <ras/include/RasEventHandlerChain.h>
#include <utility/include/Log.h>

#include <boost/assert.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

namespace util = bgq::util;
const unsigned BootFailure::ComputeCardsOnMidplane = util::Location::NodeBoardsOnMidplane * util::Location::ComputeCardsOnNodeBoard;

BootFailure::BootFailure(
        const BlockPtr& block,
        const std::string& message
        ) :
    _event( MMCSOps_000A ),
    _block( block ),
    _io( ),
    _boards( ),
    _midplanes( )
{
    BOOST_ASSERT( _block );
    _event.setDetail( RasEvent::BLOCKID, _block->getBlockName() );
    _event.setDetail( RasEvent::MESSAGE, message );
}

BootFailure::~BootFailure()
{
    try {
        RasEventHandlerChain::handle( _event );
        BGQDB::putRAS(
                _event,
                _block->getBlockName(),
                0, /* job ID */
                _block->_bootCookie
                );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "Could not insert RAS event: " << e.what() );
    } catch ( ... ) {
        LOG_WARN_MSG( "Caught an unexpected exception." );
    }
}

void
BootFailure::timeout()
{
    std::vector<std::string> failedNodes;
    for (
            std::map<std::string, BCTargetInfo*>::const_iterator i = _block->_targetLocationMap.begin();
            i != _block->_targetLocationMap.end();
            ++i
        )
    {
        const BCNodeInfo* node = dynamic_cast<BCNodeInfo*>(i->second);
        if (!node) continue;
        if (node->_initialized) continue;

        failedNodes.push_back( i->first );

        this->gatherDetails( i->first );
    }

    LOG_WARN_MSG(
            failedNodes.size() << " of " <<
            (_block->isIoBlock() ? _block->_numNodesTotal : _block->_numComputesTotal) <<
            " nodes did not initialize."
            );

    // compute blocks get special treatment since they have more moving parts
    bool loggedDetails = false;
    if ( boost::dynamic_pointer_cast<CNBlockController>(_block) ) {
        loggedDetails = this->logComputeBlockDetails();
    }

    // if we logged details previously, omit the node specific information
    if ( loggedDetails ) return;

    const unsigned nodesPerLine = 10;
    while ( !failedNodes.empty() ) {
        std::vector<std::string>::iterator end = failedNodes.begin();
        if ( failedNodes.size() < nodesPerLine ) {
            end = failedNodes.end();
        } else {
            std::advance( end, nodesPerLine );
        }

        std::ostringstream os;
        std::copy( failedNodes.begin(), end - 1, std::ostream_iterator<std::string>(os, ", ") );
        os << *(end - 1);
        LOG_WARN_MSG( os.str() );
        failedNodes.erase( failedNodes.begin(), end );
    }
}

void
BootFailure::gatherDetails(
        const std::string& compute
        )
{
    // assume compute node location string Rxx-Mx-Nxx-Jxx
    if ( compute.size() != bgq::util::Location::ComputeCardOnNodeBoardLength ) return;

    const std::string midplaneLocation( compute.substr(0, bgq::util::Location::MidplaneLength) );
    const std::string nodeBoardLocation( compute.substr(0, bgq::util::Location::NodeBoardLength) );
    const std::string ioNodeLocation( _block->_block->connectedIONode(compute) );

    const NodeFailure::iterator midplane = _midplanes.find( midplaneLocation );
    if ( midplane == _midplanes.end() ) {
        _midplanes.insert( NodeFailure::value_type(midplaneLocation, 1) );
    } else {
        midplane->second += 1;
    }

    const NodeFailure::iterator board = _boards.find( nodeBoardLocation );
    if ( board == _boards.end() ) {
        _boards.insert( NodeFailure::value_type(nodeBoardLocation, 1) );
    } else {
        board->second += 1;
    }

    if ( !ioNodeLocation.empty() ) {
        const NodeFailure::iterator io = _io.find( ioNodeLocation );
        if ( io == _io.end() ) {
            _io.insert( NodeFailure::value_type(ioNodeLocation, 1) );
        } else {
            io->second += 1;
        }
    }
}

bool
BootFailure::logComputeBlockDetails() const
{
    bool addedMidplaneDetails = false;
    for ( NodeFailure::const_iterator i = _midplanes.begin(); i != _midplanes.end(); ++i ) {
        if ( i->second == ComputeCardsOnMidplane ) {
            LOG_WARN_MSG( "All compute nodes in " << i->first << " did not initialize." );
            addedMidplaneDetails = true;
        } else {
            LOG_DEBUG_MSG( i->second << " compute nodes in " << i->first << " did not initialize." );
        }
    }

    bool addedNodeBoardDetails = false;
    for ( NodeFailure::const_iterator i = _boards.begin(); i != _boards.end(); ++i ) {
        if ( i->second == bgq::util::Location::ComputeCardsOnNodeBoard ) {
            // do not log an entry for this node board if its encompassing midplane was logged previously
            const NodeFailure::const_iterator midplane = _midplanes.find( i->first.substr(0, bgq::util::Location::MidplaneLength) );
            if ( midplane != _midplanes.end() && midplane->second < ComputeCardsOnMidplane ) {
                LOG_WARN_MSG( "All compute nodes in " << i->first << " did not initialize." );
                addedNodeBoardDetails = true;
            }
        } else {
            LOG_DEBUG_MSG( i->second << " compute nodes in " << i->first << " did not initialize." );
        }
    }

    bool addedIoNodeDetails = false;
    for ( NodeFailure::const_iterator i = _io.begin(); i != _io.end(); ++i ) {
        // log any I/O node locations servicing 16 or more compute nodes that did not initialize.
        // There isn't a hard and fast rule here, we just really care about providing more information
        // that may help determine why the boot failed.
        if ( i->second >= 16 ) {
            LOG_WARN_MSG( i->second << " compute nodes serviced by " << i->first << " did not initialize." );
            addedIoNodeDetails = true;
        }
    }

    return addedMidplaneDetails || addedNodeBoardDetails || addedIoNodeDetails;
}

} } // namespace mmcs::server
