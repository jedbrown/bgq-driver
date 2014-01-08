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
#include <boost/thread.hpp>

using namespace CxxSockets;

static const int POLL_TIME = 50;

void EpollingSocketSet::RemoveSock(SocketPtr sock) { 
    EpollingFileSet::RemoveFile(sock);
}

// Private internal method does not lock and is called by
// external methods that do.
void EpollingSocketSet::pAddSock(SocketPtr sock, PollType p){
    EpollingFileSet::pAddFile(sock);
}

void EpollingSocketSet::AddSock(SocketPtr sock, PollType p) {
    PthreadMutexHolder mutex; LockSet(mutex);
    pAddSock(sock, p);
}

void EpollingSocketSet::AddSocks(std::vector<SocketPtr>& socks, PollType p) {
    PthreadMutexHolder mutex; LockSet(mutex);
        for(std::vector<SocketPtr>::iterator i = socks.begin(); 
        i != socks.end(); ++i) {
        pAddSock((*i));
    }
}

void EpollingTCPSocketSet::PolledSend(Message& msg) {

}

void EpollingTCPSocketSet::PolledReceive(CxxSockets::MsgMap& msgmap, int flags) {
    PthreadMutexHolder mutex; LockSet(mutex);
    // Loop the set and lock the locks
    std::vector<PthreadMutexHolder*> holders;
    for (PollingListenerSet::iterator it = begin(); it != end(); ++it) {
        PthreadMutexHolder mutex;
        int rc = (*it)->LockReceive(mutex);
        holders.push_back(&mutex);
        if(rc != 0) {
            throw SockHardError(rc, "EpolledReceive cannot lock socket receive side");
        }
    }

    if(pEpoll(_timeout?_timeout:POLL_TIME) > 0) {
        boost::this_thread::interruption_point();
        // Loop through all of the poll-returned sockets
        for(unsigned int i = 0; i < _events.size() ; ++i) {
            if(_events[i].events & (EPOLLIN|EPOLLERR|EPOLLHUP)) {
                // Got one!  Find it in our set and call receive.
                for (PollingListenerSet::iterator it = begin(); it != end(); ++it) {
                    if((*it)->_fileDescriptor == _events[i].data.fd) {
                        // Found one.  Get the message and stick it in our vector
                        Message rcvmsg;
                        TCPSocketPtr sock = std::tr1::static_pointer_cast<TCPSocket>((*it));
                        // This blocks on each receive serially so it doesn't scale great, 
                        // but this model is inherently poorly scaling because we end up
                        // looping twice.  Once to get the messages, once to consume them.
                        // We =could= improve it a bit by generating threads for each receive
                        // and returning when all are done, but part of the reason for
                        // polling is avoiding threading.
                        sock->Receive(rcvmsg);
                        std::pair<CxxSockets::TCPSocketPtr, std::string> element(sock,rcvmsg.str());
                        msgmap.insert(element);
                    } // Other side of poll event check
                }
            } 
        }
    }
}
