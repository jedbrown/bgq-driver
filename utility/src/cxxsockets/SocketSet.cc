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
#include "SocketTypes.h"

using namespace CxxSockets;

void SocketSet::AddSock(SocketPtr sock) {
    PthreadMutexHolder mutex; LockSet(mutex);
    pAddFile(sock);
}

void SocketSet::AddSocks(std::vector<SocketPtr> socks) {
    PthreadMutexHolder mutex; LockSet(mutex);
    for(std::vector<SocketPtr>::iterator it = socks.begin();
        it != socks.end(); ++it) {
        pAddFile((*it));
    }
}

void SocketSet::RemoveSock(SocketPtr& sock) { 
    PthreadMutexHolder mutex; 
    LockSet(mutex); 
    erase(remove(begin(),end(),sock), end()); 
}

TCPSocketSet::TCPSocketSet(SockAddrList& sal, Policy p) {
    PthreadMutexHolder mutex; LockSet(mutex);
    for (SockAddrList::iterator it = sal.begin(); it != sal.end(); ++it) {
        TCPSocketPtr ts(new TCPSocket((*it).family(), 0, p));
        if(!_blocking) {
            // Set non-blocking.  
        }

        SocketPtr p = std::tr1::dynamic_pointer_cast<Socket>(ts);
        pAddFile(p);
    }    
}
