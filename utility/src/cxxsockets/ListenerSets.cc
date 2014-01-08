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
#include "cxxsockets/SocketTypes.h"

// We need a way to modify the timeout from the client side... ctor parm?
const unsigned int DEFAULT_TIMEOUT = 50;

LOG_DECLARE_FILE( "utility.cxxsockets" );

using namespace CxxSockets;

ListenerSet::ListenerSet(SockAddrList& sal, int backlog) {
    LOG_DEBUG_MSG(__FUNCTION__);
    // Fill this in by binding to all of them in the list.
    // We only fail if we are unable to bind to ANY of the addresses.
    unsigned int fail = 0;

    if(sal.size() <= 0) {
        throw CxxSockets::CxxSocketUserError(-1, "Empty list of sockaddrs");
    }
    
    std::string last_error;
    LOG_TRACE_MSG("Building listener set from sock addr list of size " << sal.size());
    for (SockAddrList::iterator it = sal.begin(); it != sal.end(); ++it) {
        try {
            ListeningSocketPtr ls(new ListeningSocket(*it, backlog));
            AddFile(ls);
            LOG_TRACE_MSG("Adding a socket to listener set.");
        } catch (CxxSockets::SockHardError& e) {
            ++fail;
            last_error = e.what();
        }
    }
    
    if(fail >= sal.size()) {
        std::ostringstream msg;
        msg << "Unable to start any listeners. " << last_error;
        throw CxxSockets::SockHardError(0, msg.str());
    } else if(0 < fail && fail  < sal.size()) {
        // In this case we got a successful listen but not to ALL addresses
        // available to us.
        LOG_WARN_MSG("Failed " << fail << " out of " << sal.size() << " sockets.");
    }
}

bool ListenerSet::AcceptNew(TCPSocketPtr& sock) {
    LOG_DEBUG_MSG(__FUNCTION__ << ":insecure");
    // This is the serially blocked non-polled version.
    PthreadMutexHolder mutex; LockSet(mutex);
    for (ListenerSet::iterator it = begin(); it != end(); ++it) {
        ListeningSocketPtr p = std::tr1::static_pointer_cast<ListeningSocket>(*it);
        if(p->AcceptNew(sock) == true) return true; // If we have one, return.
    }
    return true;
}

bool ListenerSet::AcceptNew(SecureTCPSocketPtr& sock, const bgq::utility::ServerPortConfiguration& port_config) {
    LOG_DEBUG_MSG(__FUNCTION__ << ":secure");
    // This is the serially blocked non-polled version.
    PthreadMutexHolder mutex; LockSet(mutex);
    for (ListenerSet::iterator it = begin(); it != end(); ++it) {
        ListeningSocketPtr p = std::tr1::static_pointer_cast<ListeningSocket>(*it);
        if(p->AcceptNew(sock, port_config) == true) return true;
    }
    return true;
}

PollingListenerSet::PollingListenerSet(SockAddrList& sal, int backlog) {
    // Fill this in by binding to all of them in the list.
    // We only fail if we are unable to bind to ANY of the addresses.

    unsigned int inuse = 0;
    unsigned int fail = 0;
    std::string last_error;

    for (SockAddrList::iterator it = sal.begin(); it != sal.end(); ++it) {
        try {
            ListeningSocketPtr ls(new ListeningSocket(*it, backlog));
            PollingSocketSet::AddSock(ls, RECV);
            LOG_TRACE_MSG("Added socket to listener set.");
        } catch (CxxSockets::SockHardError& e) {
            LOG_INFO_MSG(e.what());
            if(e.errcode == EADDRINUSE) {
                ++inuse; //++fail;
            } else {
                ++fail;
                last_error = e.what();
            }
        }
        if(inuse >= sal.size()) {
            std::ostringstream msg;
            msg << "Unable to start any listeners. All addresses in use.";
            throw CxxSockets::SockHardError(errno, msg.str());
        }
    }

    if(fail >= sal.size()) {
        std::ostringstream msg;
        msg << "Unable to start any listeners. " << last_error;
        throw CxxSockets::SockHardError(0, msg.str());
    } else if(0 < fail && fail < sal.size()) {
        // In this case we got a successful listen but not to ALL addresses
        // available to us.
        LOG_WARN_MSG("Failed " << fail << " out of " << sal.size() << " sockets.");
    }
}

bool PollingListenerSet::AcceptNew(TCPSocketPtr& sock) {
    LOG_TRACE_MSG(__FUNCTION__ << ":polling,insecure");
    // Empty parms
    SecureTCPSocketPtr null_ptr;
    bgq::utility::ServerPortConfiguration port_config(0);
    return AcceptNewCommon(null_ptr, sock, port_config, false);
}

bool PollingListenerSet::AcceptNew(SecureTCPSocketPtr& sock, const bgq::utility::ServerPortConfiguration& port_config) {
    LOG_TRACE_MSG(__FUNCTION__ << ":polling,secure");
    // Check to see if a socket is "ready" from a previous poll, return
    // that instead of polling again.
    TCPSocketPtr null_ptr;
    return AcceptNewCommon(sock, null_ptr, port_config, true);
}

bool PollingListenerSet::AcceptNewCommon(SecureTCPSocketPtr& secure_sock, TCPSocketPtr& sock, const bgq::utility::ServerPortConfiguration& port_config, bool secure) {
    LOG_TRACE_MSG(__FUNCTION__);
    // Check to see if a socket is "ready" from a previous poll, return
    // that instead of polling again.
    if(_readySockets.size() != 0) {
        ListeningSocketPtr p = std::tr1::static_pointer_cast<ListeningSocket>(_readySockets.front());
        _readySockets.pop_front();
        if(secure)
            p->AcceptNew(secure_sock, port_config);
        else
            p->AcceptNew(sock);
    } else {
        if(Poll(DEFAULT_TIMEOUT) > 0) {
            // We have stuff to do.  Find all of the fds with work to do and return the 
            // first, marking the rest.
            bool first = true;
            // Loop through all of the poll-returned sockets
            for(unsigned int i = 0; i < _pollinfo.size() ; ++i) {
                if(_pollinfo[i].revents & (POLLIN|POLLERR|POLLHUP|POLLNVAL)) {
                    // Got one!
                    // This will be the one to return.  Need to find it in
                    // our socket object list and call accept().
                    for (PollingListenerSet::iterator it = begin(); it != end(); ++it) {
                        if((*it)->_fileDescriptor == _pollinfo[i].fd) {
                            // Found one.  If it's the first, we accept() it.
                            if(first) {
                                ListeningSocketPtr p = std::tr1::static_pointer_cast<ListeningSocket>(*it);
                                first = false;
                                if(secure)
                                    return p->AcceptNew(secure_sock, port_config);
                                else return p->AcceptNew(sock);
                            } else {
                                // Not first.  Stick it in the list of sockets
                                // ready for action.  Next time, we'll get it 
                                // instead of polling again.
                                _readySockets.push_back(*it);
                            }
                        } // Other side of poll event check
                    } // end search loop
                } 
            } // end loop through poll descriptors 
        } // other side of poll check
    }
    return false;
}
