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

LOG_DECLARE_FILE( "utility.cxxsockets" );

bgq::utility::Properties::Ptr File::pProps;

void CxxSockets::File::setProperties(bgq::utility::Properties::Ptr p) { 
    pProps = p;
}

int CxxSockets::File::LockFile(CxxSockets::FileLocker& locker) {
    // Get the whole socket lock first to ensure that 
    // neither the sender nor the receiver gets invalidated
    // between the existence check and the lock() call.
    int rc = 0;
    rc = locker._all.Lock(&_fileLock); 

    // Note, we don't have to have either or both.  If they've
    // been invalidated, we don't care here.
    if(_sender) {
        locker._send.Lock(&_sender->getLock());
    }
    if(_receiver) {
        locker._receive.Lock(&_receiver->getLock());
    }
    return rc;
}

CxxSockets::File::File(int descriptor) : _fcntlFlags(0) {
    _fileDescriptor = descriptor;
    Initialize();
}

CxxSockets::File::File() : _fcntlFlags(0) {
    _fileDescriptor = -1;
    Initialize();
}

CxxSockets::File::File(const char* filename, bool tempfile, int flags, int mode) {
    _filename = (char*)filename;
    Open(filename, tempfile, flags, mode);
    Initialize();
}

CxxSockets::File::File(std::string& filename, bool tempfile, int flags, int mode) {
    _filename = (char*)filename.c_str();
    Open(filename, tempfile, flags, mode);
    Initialize();
}

void CxxSockets::File::Initialize() {
    // initialize the sender and the receiver
    CxxSockets::SocketSendSidePtr ssp(new CxxSockets::SocketSendSide);
    _sender = ssp;

    CxxSockets::SocketReceiveSidePtr rsp(new CxxSockets::SocketReceiveSide);
    _receiver = rsp;
}

int CxxSockets::File::Open(std::string& filename, bool tempfile, int flags, int mode) {
    File::Open(filename.c_str(), tempfile, flags, mode);
    return 1;
}

int CxxSockets::File::Open(const char* filename, bool tempfile, int flags, int mode) {
    CxxSockets::FileLocker locker;
    LockFile(locker);
    if(tempfile) {
        char fn[4096];
        strcpy(fn, filename);
        _fileDescriptor = mkstemp(fn);
        if(_fileDescriptor <= 0) {
            std::ostringstream msg;
            msg << "unable to generate temp file: " << strerror(errno);
            LOG_ERROR_MSG(msg.str());
            throw CxxSockets::SockHardError(errno, msg.str());
        }        
        _filename = fn;
    }

    if(!tempfile) {
        _filename = (char*)filename;
        
        _fileDescriptor = ::open(_filename, flags, mode);
        LOG_DEBUG_MSG("open fd=" << _fileDescriptor);
        if(_fileDescriptor <= 0) {
            std::ostringstream msg;
            msg << "unable to open file: " << strerror(errno);
            LOG_ERROR_MSG(msg.str());
            throw CxxSockets::SockHardError(errno, msg.str());
        }
    }

    File::CloseOnExec();
    return 1;
}

int CxxSockets::File::Write(CxxSockets::Message& msg) {
    PthreadMutexHolder mutex;
    int lockrc = LockSend(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "Write error.  File write side lock error: " << strerror(lockrc);
        else
            msg << "Write error.  File write side closed: " << lockrc;
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }

    uint32_t size = msg.str().length();
    LOG_DEBUG_MSG("write fd=" << _fileDescriptor);
    int count = ::write(_fileDescriptor, msg.str().c_str(), size);
    if((uint32_t)count != size) {
        std::ostringstream msg;
        msg << "File write error. " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
    }
    return count;
}

int CxxSockets::File::Lseek(off_t offset, int whence) {
    int rc = lseek(_fileDescriptor, offset, whence);
    if(rc < 0) {
        std::ostringstream msg;
        msg << "File seek error " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
    }
    return rc;
}

int CxxSockets::File::Read(CxxSockets::Message& msg, int bytes_to_read) {
    PthreadMutexHolder mutex;
    int lockrc = LockSend(mutex);
    if(lockrc != 0) {
        std::ostringstream msg;
        if(lockrc != -1)
            msg << "Read error.  File read side lock error: " << strerror(lockrc);
        else
            msg << "Read error.  File read side closed: " << lockrc;
        LOG_INFO_MSG(msg.str());
        throw CxxSockets::SockSoftError(lockrc, msg.str());
    }
    
    int bytes = bytes_to_read?bytes_to_read:SSIZE_MAX - 1;
    char buffer[bytes + 1];
    bzero(&buffer, bytes + 1);
    int bytes_read = read(_fileDescriptor, buffer, bytes); 

    if(bytes_read < 0) {
        std::ostringstream errmsg;
        errmsg << "File read error. " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SockSoftError(errno, errmsg.str());
    }
    //    msg << buffer;

    for(int u=0; u < bytes_read; ++u) {
        msg << buffer[u];
    }

    LOG_DEBUG_MSG("read " << bytes_read << " bytes");
    return bytes_read;
}

void CxxSockets::File::Delete() {
    if(unlink(_filename) != 0) {
        std::ostringstream msg;
        msg << "unable to delete file: " << strerror(errno);
        LOG_ERROR_MSG(msg.str());
        throw CxxSockets::SockHardError(errno, msg.str());
    }
}

int CxxSockets::File::LockSend(PthreadMutexHolder& smutex) { 
    int rc = 0;
    if(_sender) {
        rc = smutex.Lock(&_sender->getLock());
    }
    else rc = -1;
    return rc; 
}

int CxxSockets::File::LockReceive(PthreadMutexHolder& rmutex) { 
    int rc = 0;
    if(_receiver) {
        rc = rmutex.Lock(&_receiver->getLock());
    }
    else rc = -1;
    return rc;
}

int CxxSockets::File::UnlockSend() { 
    int rc = 0;
    if(_sender)
        rc = _sender->Unlock(); 
    else rc = -1;
    return rc;
}

int CxxSockets::File::UnlockReceive() { 
    int rc = 0;
    if(_receiver)
        rc = _receiver->Unlock(); 
    else rc = -1;
    return rc;
}
