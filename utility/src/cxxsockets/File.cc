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

void CxxSockets::File::Initialize() {
    // initialize the sender and the receiver
    CxxSockets::SocketSendSidePtr ssp(new CxxSockets::SocketSendSide);
    _sender = ssp;

    CxxSockets::SocketReceiveSidePtr rsp(new CxxSockets::SocketReceiveSide);
    _receiver = rsp;
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
