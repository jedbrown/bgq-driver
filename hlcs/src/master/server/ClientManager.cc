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

#include "ClientManager.h"

#include "ClientController.h"



LOG_DECLARE_FILE( "master" );

ClientManager::ClientManager() : _ending(false)
{
    // Nothing to do
}

void
ClientManager::cancel()
{
    LOG_TRACE_MSG(__FUNCTION__);
    _ending = true;

    std::vector<ClientControllerPtr> clients;
    {
        boost::mutex::scoped_lock lock( _clientMutex );
        clients = _clients;
    }

    for (unsigned i = 0; i < clients.size(); ++i)
        clients[i]->cancel();
}

void
ClientManager::removeClient(
        ClientControllerPtr c
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock lock( _clientMutex );
    _clients.erase(remove(_clients.begin(), _clients.end(), c), _clients.end());
}

std::vector<ClientControllerPtr>
ClientManager::getClients()
{
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock lock( _clientMutex );
    return _clients;
}

void
ClientManager::addClient(
        ClientControllerPtr c
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    if (_ending)
        return;  // Don't accept anybody new if we're going away.
    boost::mutex::scoped_lock lock( _clientMutex );
    _clients.push_back(c);
    LOG_DEBUG_MSG( _clients.size() << " clients managed");
    c->startPoller();
    return;
}
