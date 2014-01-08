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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "add.h"

#include "../BlockHelper.h"
#include "../DBConsoleController.h"
#include "../IOBlockController.h"
#include "Info.h"

#include <bgq_util/include/Location.h>
#include <bgq_util/include/pthreadmutex.h>

#include <control/include/mcServer/defaults.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>

#include <set>


LOG_DECLARE_FILE( "mmcs.server" );

namespace {

const std::set<uint32_t> MessageIds = boost::assign::list_of
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
    (0x000901A0)
    (0x000901A1)
    (0x000901B0)
    (0x000901B1)
    (0x000901C0)
    (0x000901C1)
    (0x000901D0)
    (0x000901D1)
    (0x000901E0)
    (0x000901E1)
    (0x000901F0)
    (0x00090104)
    (0x00090105)
    (0x000901F1)
    (0x00090200)
    (0x00090201)
    (0x00090202)
    (0x0009020D)
    (0x00090210)
    (0x00090211)
    ;

} // anonymous namespace

namespace mmcs {
namespace server {
namespace neighbor {

void
addDetails(
        const BlockPtr& b,
        const RasEvent& event,
        BGQDB::DBTEventlog& dbe
        ) throw()
{
    if ( MessageIds.find(event.msgId()) == MessageIds.end() ) {
        LOG_TRACE_MSG( "skipping neighbor details for " << std::hex << event.msgId() );
        return;
    }

    BlockPtr block( b ); // not const since we may modify below when finding I/O block

    try {
        const bgq::util::Location location( dbe._location );

        if (
                block->getBlockName() == mc_server::DefaultListener &&
                location.getType() == bgq::util::Location::ComputeCardOnIoBoard
           )
        {
            block = findIoBlock( location );
            if ( !block ) return;
        }

        BOOST_ASSERT( block );

        const std::auto_ptr<Info> neighbor(
                Info::create(
                    block,
                    event.msgId(),
                    location,
                    dbe._rawdata
                    )
                );
        const size_t remaining = sizeof(dbe._rawdata) - strlen(dbe._rawdata);
        LOG_DEBUG_MSG( remaining << " of " << sizeof(dbe._rawdata) << " bytes remaining" );
        const std::string result( neighbor->getDetails() );
        LOG_DEBUG_MSG( "Adding " << result.size() << " bytes for neighbor details" );
        strncat( dbe._rawdata, result.c_str(), remaining - 1 );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "event 0x" << std::hex << std::setw(8) << std::setfill('0') << event.msgId() << ": " << e.what() );
    } catch ( ... ) {
        LOG_ERROR_MSG( "Caught an unexpected exception" );
    }
}

BlockPtr
findIoBlock(
        const bgq::util::Location& location
        )
{
    LOG_DEBUG_MSG( "Finding I/O block for " << location );

    PthreadMutexHolder holder;
    holder.Lock(&DBConsoleController::getBlockMapMutex());

    for (BlockMap::const_iterator it = DBConsoleController::getBlockMap().begin(); it != DBConsoleController::getBlockMap().end(); ++it) {
        const BlockPtr block( it->second->getBase() );
        if ( !boost::dynamic_pointer_cast<IOBlockController>(block) ) {
            LOG_DEBUG_MSG( "Skipping compute block " << it->first );
            continue;
        }
        if ( block->findNodeInfo(location.getLocation()) ) {
            LOG_DEBUG_MSG( "Matched " << it->first );
            return block;
        }
        LOG_DEBUG_MSG( "Did not match " << it->first );
    }

    LOG_WARN_MSG( "Did not find I/O block for " << location );
    return BlockPtr();
}

} } } // namespace mmcs::server::neighbor
