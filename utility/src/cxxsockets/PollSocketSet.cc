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

LOG_DECLARE_FILE( "utility.cxxsockets" );

void PollingSocketSet::RemoveSock(SocketPtr sock) { 
    PollingFileSet::RemoveFile(sock);
}

void PollingSocketSet::pAddSock(SocketPtr sock, PollType p) {
    PollingFileSet::pAddFile(sock, p);
}

void PollingSocketSet::AddSock(SocketPtr sock, PollType p) {
    PthreadMutexHolder mutex; LockSet(mutex);
    pAddSock(sock, p);
}

void PollingSocketSet::AddSocks(std::vector<SocketPtr>& socks, PollType p) {
    PthreadMutexHolder mutex; LockSet(mutex);
    for(std::vector<SocketPtr>::iterator i = socks.begin(); 
        i != socks.end(); ++i) {
        FileSet::pAddFile((*i));
    }
}

void PollingTCPSocketSet::PolledSend(Message& msg, int flags) {

}

int PollingTCPSocketSet::PolledReceive(CxxSockets::MsgMap& msgmap, int flags) {
    PthreadMutexHolder mutex; LockSet(mutex);
    // Before we poll, we have to acquire the receive side locks on ALL sockets in the set.
    // Then, go through the sockets that return and do a receive on each, putting data
    // in a message buffer and pushing it in to the passed vector.

    std::vector<PthreadMutexHolder*> holders;
    // Loop the set and lock the locks
    for (PollingListenerSet::iterator it = begin(); it != end(); ++it) {
        PthreadMutexHolder mutex;
        int rc = (*it)->LockReceive(mutex);
        holders.push_back(&mutex);
        if(rc != 0) {
            throw SockHardError(rc, "PolledReceive cannot lock socket receive side");
        }
    }

    int pollrc = Poll(_timeout?_timeout:POLL_TIME);
    if(pollrc > 0) {
        // We have stuff to do.  Find all of the fds with work to do and return the 
        // first, marking the rest.
        boost::this_thread::interruption_point();
        // Loop through all of the poll-returned sockets
        for(unsigned int i = 0; i < _pollinfo.size() ; ++i) {
            if(_pollinfo[i].revents & (POLLIN|POLLERR|POLLHUP|POLLNVAL)) {
                LOG_DEBUG_MSG("Got a descriptor");
                // Got one!  Find it in our set and call receive.
                for (PollingFileSet::iterator it = begin(); it != end(); ++it) {
                    if((*it)->_fileDescriptor == _pollinfo[i].fd) {
                        LOG_DEBUG_MSG("poll set descriptor=" << _pollinfo[i].fd << " PollingListenerSet descriptor=" <<(*it)->_fileDescriptor);
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
    return pollrc;
}

void PollingSecureTCPSocketSet::PolledSend(Message& msg, int flags) {

}

int PollingSecureTCPSocketSet::PolledReceive(CxxSockets::MsgMap& msgmap, int flags) {
    PthreadMutexHolder mutex; LockSet(mutex);
    // Before we poll, we have to acquire the receive side locks on ALL sockets in the set.
    // Then, go through the sockets that return and do a receive on each, putting data
    // in a message buffer and pushing it in to the passed vector.

    std::vector<PthreadMutexHolder*> holders;
    // Loop the set and lock the locks
    for (PollingListenerSet::iterator it = begin(); it != end(); ++it) {
        PthreadMutexHolder mutex;
        int rc = (*it)->LockReceive(mutex);
        holders.push_back(&mutex);
        if(rc != 0) {
            throw SockHardError(rc, "PolledReceive cannot lock socket receive side");
        }
    }

    int pollrc = Poll(_timeout?_timeout:POLL_TIME);
    if(pollrc > 0) {
        // We have stuff to do.  Find all of the fds with work to do and return the 
        // first, marking the rest.

        // Loop through all of the poll-returned sockets
        for(unsigned int i = 0; i < _pollinfo.size() ; ++i) {
            if(_pollinfo[i].revents & (POLLIN|POLLERR|POLLHUP|POLLNVAL)) {
                LOG_DEBUG_MSG("Got a descriptor");
                // Got one!  Find it in our set and call receive.
                for (PollingFileSet::iterator it = begin(); it != end(); ++it) {
                    if((*it)->_fileDescriptor == _pollinfo[i].fd) {
                        LOG_DEBUG_MSG("poll set descriptor=" << _pollinfo[i].fd << " PollingListenerSet descriptor=" <<(*it)->_fileDescriptor);
                        // Found one.  Get the message and stick it in our vector
                        Message rcvmsg;
                        SecureTCPSocketPtr sock = std::tr1::static_pointer_cast<SecureTCPSocket>((*it));
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
    return pollrc;
}
