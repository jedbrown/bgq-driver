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

// The LiteControlEventListener receives and processes RAS events
// when there is no specific EventListener registered for the location
// generating the RAS event and we're running under mmcs_lite.
//

#include "LiteControlEventListener.h"

#include "server/BlockControllerBase.h"

#include <control/include/mcServer/defaults.h>
#include <control/include/mcServer/MCServerRef.h>

#include <ras/include/RasEvent.h>

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


// singleton instance
// Note that this is not serialized by a mutex.
// Serialization must be managed by the caller.
LiteControlEventListener* LiteControlEventListener::_liteControlEventListener = NULL;

LiteControlEventListener*
LiteControlEventListener::getLiteControlEventListener()
{
    if (_liteControlEventListener == NULL)
        _liteControlEventListener = new LiteControlEventListener;
    return _liteControlEventListener;
}

// constructor
LiteControlEventListener::LiteControlEventListener()
    : lite::BlockController(NULL, "mmcs", mc_server::DefaultListener)
{
}

// destructor
LiteControlEventListener::~LiteControlEventListener()
{
    disconnect();
}



// Release connections to all nodes under our control.
//
void
LiteControlEventListener::disconnect()
{
    PthreadMutexHolder mutex;
    mutex.Lock(&getMutex());	// serialize this code

    if (!getBase()->isConnected())
	return;

    LOG_INFO_MSG("LiteControlEventListener::disconnect() releasing block resources");

    // close socket connection to mcServer
    MCServerRef* tempRef = getBase()->getMCServer();
    delete tempRef;
}

} } // namespace mmcs::lite
