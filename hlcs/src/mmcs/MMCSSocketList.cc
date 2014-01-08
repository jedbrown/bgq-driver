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

#include <sys/socket.h>
#include "MMCSSocketList.h"

using namespace std;

// add a client socket - returns false if shutting down
bool
MMCSSocketList::add(CxxSockets::SocketPtr socket)
{
    bool success;
    PthreadMutexHolder mutex;
    std::list<CxxSockets::SocketPtr>::iterator it;
    int mutex_rc = mutex.Lock(&_socketListMutex);
    assert(mutex_rc == 0);
    if (_shutdown)
	success = false;
    else
    {
	for (it = _socketList.begin(); it != _socketList.end(); ++it)
	    if (*it == socket)
		break;
	if (it == _socketList.end())
	    _socketList.push_back(socket);
	success = true;
    }
    mutex.Unlock();
    return success;
}

void
MMCSSocketList::remove(CxxSockets::SocketPtr socket)
{
    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&_socketListMutex);
    assert(mutex_rc == 0);
    _socketList.remove(socket);
    mutex.Unlock();
}

void
MMCSSocketList::stopConnections()
{
    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&_socketListMutex);
    assert(mutex_rc == 0);

    for (list<CxxSockets::SocketPtr>::iterator it = _socketList.begin(); it != _socketList.end(); ) {
        (*it).reset();
        it = _socketList.erase(it);
    }
    _shutdown = true;
    mutex.Unlock();
}

