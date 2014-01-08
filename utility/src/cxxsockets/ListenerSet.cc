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

#include "cxxsockets/ListenerSet.h"

#include "cxxsockets/exception.h"
#include "cxxsockets/ListeningSocket.h"
#include "cxxsockets/SockAddrList.h"

#include "Log.h"

#include <boost/foreach.hpp>

#include <vector>

#include <errno.h>
#include <poll.h>

using std::vector;

LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace CxxSockets {

ListenerSet::ListenerSet(
        const SockAddrList& sal,
        const int backlog
        )
{
    // Fill this in by binding to all of them in the list.
    // We only fail if we are unable to bind to ANY of the addresses.
    unsigned int fail = 0;

    if (sal.size() <= 0) {
        std::ostringstream msg;
        msg << "Empty list of sockaddrs.";
        LOG_DEBUG_MSG( msg.str() );
        throw UserError(-1, msg.str());
    }

    std::string last_error;
    LOG_TRACE_MSG("Building listener set from sock addr list of size " << sal.size());
    for (SockAddrList::const_iterator it = sal.begin(); it != sal.end(); ++it) {
        try {
            const ListeningSocketPtr ls(new ListeningSocket(*it, backlog));
            AddFile(ls);
            LOG_TRACE_MSG("Added socket fd=" << ls->getFileDescriptor() << " to listener set." );
        } catch (const HardError& e) {
            ++fail;
            last_error = e.what();
        }
    }

    if (fail >= sal.size()) {
        std::ostringstream msg;
        msg << "Unable to start any listeners: " << last_error;
        LOG_DEBUG_MSG( msg.str() );
        throw HardError(0, msg.str());
    } else if (0 < fail && fail < sal.size()) {
        // In this case we got a successful listen but not to ALL addresses available to us.
        LOG_WARN_MSG("Failed " << fail << " out of " << sal.size() << " sockets.");
    }
}

bool
ListenerSet::AcceptNew(
        const TCPSocketPtr& sock
        )
{
    // This function uses poll() to find a socket that's READ ready.
    // It will call accept() one of the READ ready sockets.

    PthreadMutexHolder mutex;
    LockSet(mutex);

    typedef vector<struct pollfd> Fds;
    Fds fds;

    BOOST_FOREACH( const FilePtr& f_ptr, _filevec ) {
        struct pollfd pfd = { f_ptr->getFileDescriptor(), POLLIN, 0 };
        fds.push_back( pfd );
    }

    LOG_TRACE_MSG( "Polling on listening sockets for connect request." );
    int pollrc(poll( fds.data(), fds.size(), -1 /* timeout, forever */ ));

    if ( pollrc < 0 ) {
        int my_errno(errno);

        if ( my_errno == EINTR ) {
            std::ostringstream msg;
            msg << "Poll interrupted.";
            LOG_DEBUG_MSG( msg.str() );
            throw SoftError(my_errno, msg.str());
        } else {
            std::ostringstream msg;
            msg << "Error on poll.";
            LOG_DEBUG_MSG( msg.str() );
            throw HardError( my_errno, msg.str() );
        }
    }

    if ( pollrc == 0 ) {
        return false;
    }

    vector<FilePtr> remove_ptrs;
    ListeningSocketPtr accept_ptr;

    BOOST_FOREACH( const struct pollfd &pfd, fds ) {

        bool remove_listener(false);

        if ( pfd.revents & POLLERR ) {
            LOG_WARN_MSG( "Error on listening socket " << pfd.fd << ", removing.");
            remove_listener = true;
        }

        if ( pfd.revents & POLLHUP ) {
            LOG_WARN_MSG( "HUP on listening socket " << pfd.fd << ", removing.");
            remove_listener = true;
        }

        if ( pfd.revents & POLLNVAL ) {
            LOG_WARN_MSG( "NVAL on listening socket " << pfd.fd << ", removing.");
            remove_listener = true;
        }

        if ( remove_listener ) {
            for (ListenerSet::const_iterator it = _filevec.begin(); it != _filevec.end(); ++it) {
                if ( (*it)->getFileDescriptor() != pfd.fd )  {
                    continue;
                }
                remove_ptrs.push_back( *it );
                break;
            }
            continue;
        }

        // Already know which one to accept on.
        if ( accept_ptr )  {
            continue;
        }

        if ( ! (pfd.revents & POLLIN) ) {
            continue;
        }

        BOOST_FOREACH( FilePtr f_ptr, _filevec ) {
            const ListeningSocketPtr p = boost::static_pointer_cast<ListeningSocket>(f_ptr);

            if ( p->getFileDescriptor() != pfd.fd )  {
                continue;
            }

            accept_ptr = p;
            break;
        }
    }

    // Remove the listening sockets with errors.
    BOOST_FOREACH( FilePtr f_ptr, remove_ptrs ) {
        RemoveFile( f_ptr );
    }

    if ( ! accept_ptr )  {
        return false;
    }

    LOG_TRACE_MSG( "Calling AcceptNew() for listening socket fd=" << accept_ptr->getFileDescriptor() );

    accept_ptr->AcceptNew(sock);
    return true;
}

}
