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

static const int POLL_TIME = 50;

void EpollingFileSet::RemoveFile(FilePtr file) { 
    PthreadMutexHolder mutex; LockSet(mutex);
    int rc = epoll_ctl(_epfd, EPOLL_CTL_DEL, file->_fileDescriptor, 0);
    if(rc < 0) 
       throw CxxSockets::SockSoftError(errno, "epoll_ctl failure, could not delete");
    FileSet::RemoveInternal(file);
}

// Private internal method does not lock.
int EpollingFileSet::pEpoll(unsigned int timeout) {
    // Check to see if we actually have something to poll
    int rc = 0;
    if(_events.size() != 0) {
        // Do the poll
        rc = epoll_wait(_epfd, &_events[0], _events.size(), timeout);
        if(rc < 0) 
            throw CxxSockets::SockHardError(errno, "epoll_wait failure");
    }
    return rc;
}

int EpollingFileSet::Epoll(unsigned int timeout) {
    PthreadMutexHolder mutex; LockSet(mutex);
    return pEpoll(timeout);
}

int EpollingFileSet::Epoll(unsigned int timeout, FileSetPtr fs) {
    PthreadMutexHolder mutex; LockSet(mutex);
    int rc = pEpoll(timeout);
    if(rc > 0) {
        for(int i = 0; i < rc; ++i) {
            FilePtr fp(new File(_events[i].data.fd));
            fs->AddFile(fp);
        }
    } // Don't need to handle the < 0 case because pEpoll throws an exception
    return rc;
}

// Private internal method does not lock and is called by
// external methods that do.
void EpollingFileSet::pAddFile(FilePtr file, PollType p){
    FileSet::pAddFile(file);
    static struct epoll_event ev;
    if(p == RECV)
        ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
    else
        ev.events = EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP;

    ev.data.fd = file->_fileDescriptor;
    int res = epoll_ctl(_epfd, EPOLL_CTL_ADD, ev.data.fd, &ev);
    if(res < 0) {
        std::ostringstream msg;
        msg << "Epoll AddFile error: " << strerror(errno);
        switch(res) {

            // Internal
        case EINVAL:
            throw CxxSockets::CxxSocketInternalError(errno, msg.str());
            break;

            // Hard
        case ENOMEM:
            throw CxxSockets::SockHardError(errno, msg.str());
            break;

            // User
        case EPERM:
        case ENOENT:
        case EEXIST:
        case EBADF:
            throw CxxSocketUserError(errno, msg.str());
            break;
        }
    }
    _events.push_back(ev);
}

void EpollingFileSet::AddFile(FilePtr file, PollType p) {
    PthreadMutexHolder mutex; LockSet(mutex);
    pAddFile(file, p);
}

void EpollingFileSet::AddFiles(std::vector<FilePtr>& files, PollType p) {
    PthreadMutexHolder mutex; LockSet(mutex);
        for(std::vector<FilePtr>::iterator i = files.begin(); 
        i != files.end(); ++i) {
        pAddFile((*i));
    }
}

int EpollingFileSet::ProtectedPoll() {
    PthreadMutexHolder mutex; LockSet(mutex);

    std::vector<FileLocker*> holders;
    // Acquire all of the locks
    for (FileSet::iterator it = begin(); it != end(); ++it) {
        FileLocker locker;
        (*it)->LockFile(locker);
        holders.push_back(&locker);
    }
    return pEpoll(POLL_TIME);
}

int EpollingFileSet::UnprotectedPoll() {
    PthreadMutexHolder mutex; LockSet(mutex);
    return pEpoll(POLL_TIME);
}


