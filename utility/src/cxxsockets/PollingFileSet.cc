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
#include "cxxsockets/PollingFileSet.h"

#include "cxxsockets/exception.h"
#include "cxxsockets/File.h"

#include "Log.h"

namespace CxxSockets {

LOG_DECLARE_FILE( "utility.cxxsockets" );

PollingFileSet::PollingFileSet() :
    FileSet()
{

}

int
PollingFileSet::Poll(
        const unsigned int timeout
        )
{
    // This just does the basic poll so it doesn't lock.
    // Fancy stuff calls this.
    if(_pollinfo.size() == 0)
        return  0;  // Nobody to poll
    
    int ready = ::poll(&_pollinfo[0], _pollinfo.size(), timeout);
    if(ready < 0) { // Error on the poll
        if(errno == EINTR || errno == EWOULDBLOCK 
           || errno == EAGAIN || errno == ERESTART)
            throw SoftError(errno, "poll interrupted");
        else
            throw HardError(errno, "poll failure");
    }
    if(ready > 0)
        LOG_DEBUG_MSG("Poll() returning " << ready << " descriptors");
    return ready;
}

void
PollingFileSet::pAddFile(
        const FilePtr file,
        const PollType p
        )
{
    // Add it to our list of files that we have to maintain to obtain locks
    FileSet::pAddFile(file);
    // We can store the file descriptor in our pollfd struct because the file 
    // cannot be closed as long as the smart pointer is held.
    struct pollfd pfd;
    pfd.fd = file->getFileDescriptor();
    pfd.events = 0;
    if(p == RECV)
        pfd.events |= POLLIN;
    else if(p == SEND)
        pfd.events |= POLLOUT;
    else { // error polling
        pfd.events |= POLLERR;
        pfd.events |= POLLRDHUP;
        pfd.events |= POLLHUP;
        pfd.events |= POLLNVAL;
    }

    pfd.events |= POLLPRI;
    pfd.events |= POLLHUP;
    _pollinfo.push_back(pfd);
}

void 
PollingFileSet::AddFile(
        const FilePtr file,
        const PollType p
        )
{
    PthreadMutexHolder mutex; LockSet(mutex);
    pAddFile(file, p);
}

}
