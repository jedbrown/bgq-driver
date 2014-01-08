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

#ifndef _MMCSSocketList_H
#define _MMCSSocketList_H

#include "bgq_util/include/pthreadmutex.h"
#include <list>
#include <utility/include/cxxsockets/SocketTypes.h>

//******************************************************************************
// SocketList: class to keep track of socket connections
//******************************************************************************
class MMCSSocketList
{
public:
    MMCSSocketList() : _socketListMutex(PTHREAD_MUTEX_ERRORCHECK_NP), _shutdown(false) {}
    bool add(CxxSockets::SocketPtr sock);		// add a socket to the list
    void remove(CxxSockets::SocketPtr sock);	        // remove a socket from the list
    void stopConnections();		// shutdown all sockets and prevent new ones
private:
    PthreadMutex   _socketListMutex;    // to serialize access to SocketList
    std::list<CxxSockets::SocketPtr> _socketList;		// list of sockets
    bool           _shutdown;		// true = disable new connections
};
#endif
