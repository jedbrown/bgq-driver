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

using namespace CxxSockets;

LOG_DECLARE_FILE( "utility.cxxsockets" );

void PollingFileSet::RemoveFile(FilePtr file) { 
    for(std::vector<pollfd>::iterator it = _pollinfo.begin();
        it != _pollinfo.end(); ++it) {
        if((*it).fd == file->_fileDescriptor) {
            // found it, remove it.
            _pollinfo.erase(it);
            // Now break because we're done and the iterators may not
            // be valid any more.
            break;
        }
    }
    FileSet::RemoveFile(file);
}

int PollingFileSet::Poll(unsigned int timeout, FileSetPtr fs) {
    int retval = Poll(timeout);
    // Now get the successful fds and stick them in the fs
    for(unsigned int i = 0; i < _pollinfo.size() ; ++i) {
        // looping through polled descriptors
        if(_pollinfo[i].revents & (POLLIN|POLLERR|POLLHUP|POLLNVAL)) {
            // got one
            for (PollingFileSet::iterator it = begin(); it != end(); ++it) {
                // loop through our set
                if((*it)->_fileDescriptor == _pollinfo[i].fd) {
                    // got a match! stick it in our file set
                    fs->push_back((*it));
                }
            }
        }
    }
    return retval;
}

int PollingFileSet::Poll(unsigned int timeout) {
    // This just does the basic poll so it doesn't lock.
    // Fancy stuff calls this.
    if(_pollinfo.size() == 0)
        return  0;  // Nobody to poll
    
    int ready = ::poll(&_pollinfo[0], _pollinfo.size(), timeout);
    if(ready < 0) { // Error on the poll
        if(errno == EINTR || errno == EWOULDBLOCK 
           || errno == EAGAIN || errno == ERESTART)
            throw CxxSockets::SockSoftError(errno, "poll interrupted");
        else
            throw CxxSockets::SockHardError(errno, "poll failure");
    }
    if(ready > 0)
        LOG_DEBUG_MSG("Poll() returning " << ready << " descriptors");
    return ready;
}

void PollingFileSet::pAddFile(FilePtr file, PollType p) {
    // Add it to our list of files that we have to maintain to obtain locks
    FileSet::pAddFile(file);
    // We can store the file descriptor in our pollfd struct because the file 
    // cannot be closed as long as the smart pointer is held.
    struct pollfd pfd;
    pfd.fd = file->_fileDescriptor;
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

void PollingFileSet::AddFile(FilePtr file, PollType p) {
    PthreadMutexHolder mutex; LockSet(mutex);
    pAddFile(file, p);
}

void PollingFileSet::AddFiles(std::vector<FilePtr>& files) {
    PthreadMutexHolder mutex; LockSet(mutex);
    for(std::vector<FilePtr>::iterator i = files.begin(); 
        i != files.end(); ++i) {
        FileSet::pAddFile((*i));
    }
}
