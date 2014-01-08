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

#include "DBConsoleController.h"
#include "DefaultControlEventListener.h"
#include "MMCSCommandProcessor.h"
#include "MMCSConsoleMonitor.h"
#include "MMCSConsolePort.h"
#include "MMCSProperties.h"
#include "MMCSSocketList.h"
#include "MMCSThread.h"

#include <bgq_util/include/pthreadmutex.h>

#include <utility/include/cxxsockets/SocketTypes.h>

#include <utility/include/Log.h>

#include <string>

LOG_DECLARE_FILE( "mmcs" );

void*
MMCSConsoleListener::threadStart()
{
    bool portWorking = true;; // false == permanent socket error on accept

    // Get the parameters
    MMCSConsoleMonitorParms* parms
	= (MMCSConsoleMonitorParms*) getArg();
    MMCSConsolePortServer* consolePort
	= (MMCSConsolePortServer*) parms->consolePort;
    MMCSCommandMap* commandMap
	= (MMCSCommandMap*) parms->commandMap;
    MMCSSocketList* socketList
	= (MMCSSocketList*) parms->socketList;
    delete parms;

    LOG_INFO_MSG("MMCSConsoleMonitor started");

    while (!isThreadStopping() && portWorking)
    {
	MMCSConsolePortClient* connection = NULL;
	// Wait for a connection from a client
	try
	{
            connection = consolePort->accept();
	}
	catch (MMCSConsolePort::Error& e)
	{
	    connection = NULL; 	// we didn't get a connection
	    if (e.errcode == EINTR || e.errcode == ECONNABORTED || e.errcode == EMFILE ||
		e.errcode == ENFILE || e.errcode == ENOBUFS || e.errcode == ENOMEM || 
                e.errcode == -1 || e.errcode == 0); // recoverable error, retry accept
	    else
	    {
		if (e.errcode != EINVAL && e.errcode != 0) // we normally receive EINVAL on shutdown
		    LOG_ERROR_MSG("MMCSConsoleMonitor accept: " << e.what() << " " << e.errcode);
                portWorking = false;
	    }
	}
	if (connection && socketList->add(connection->getSock()))
	{
	    // Create a thread to service the connection
	    MMCSConsoleConnectionThread* serverThread =
		new MMCSConsoleConnectionThread;
	    serverThread->setArg(new MMCSConsoleMonitorParms(connection, commandMap, socketList));
	    serverThread->setJoinable(false);
	    serverThread->setDeleteOnExit(true); // delete this object when the thread exits
	    serverThread->start();
	}
    }

    // close the interactive console port
    delete consolePort;
    consolePort = NULL;

    // return when done
    LOG_INFO_MSG("MMCSConsoleMonitor stopped");
    return NULL;
}

void*
MMCSConsoleConnectionThread::threadStart()
{
    // Get the parameters
    MMCSConsoleMonitorParms* parms
	= (struct MMCSConsoleMonitorParms*) getArg();
    MMCSConsolePortClient* connection =
	(MMCSConsolePortClient*) parms->consolePort;
    MMCSCommandMap* commandMap
	= (MMCSCommandMap*) parms->commandMap;
    MMCSSocketList* socketList
	= (MMCSSocketList*) parms->socketList;

    // get user on the other end
    CxxSockets::SecureTCPSocketPtr sock;
    bgq::utility::UserId uid;
    CxxSockets::UserType utype( CxxSockets::Normal );
    sock = std::tr1::dynamic_pointer_cast<CxxSockets::SecureTCPSocket>( connection->getSock() );
    if ( sock) {
        uid = sock->getUserId();
        utype = sock->getUserType();
    } else {
        LOG_WARN_MSG( "could not get remote user" );
    }

    // Create a midplane controller object
    MMCSServerCommandProcessor commandProcessor(commandMap); // MMCS command processor
    DBConsoleController midplaneController(&commandProcessor, uid, utype, connection);
    setThreadName("console");
    midplaneController.setMMCSThread(this);
    midplaneController.setReplyFormat(parms->replyFormat);

    LOG_INFO_MSG("MMCSConsoleConnection started");

    CxxSockets::SockAddr local;
    CxxSockets::SockAddr remote;
    connection->getSock()->getSockName(local);
    connection->getSock()->getPeerName(remote);
    LOG_DEBUG_MSG("connected to " << remote.getHostAddr()
                  << ":" << remote.getServicePort() << " on port "
                  << local.getServicePort());

    delete parms;

    // Execute commands received on this connection
    midplaneController.run();

    // return when done
    socketList->remove(connection->getSock());

    LOG_INFO_MSG("MMCSConsoleConnection stopped");
    return NULL;
}
