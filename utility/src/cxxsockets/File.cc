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

#include "cxxsockets/File.h"
#include "cxxsockets/FileLocker.h"
#include "cxxsockets/exception.h"
#include "cxxsockets/SocketReceiveSide.h"
#include "cxxsockets/SocketSendSide.h"

#include "Log.h"

#include <fcntl.h>


LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace CxxSockets {

int
File::LockFile(
        FileLocker& locker
        )
{
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

File::File(
        const int descriptor
        ) :
    _fileDescriptor(descriptor),
    _fileLock(),
    _receiver( new SocketReceiveSide ),
    _sender( new SocketSendSide )
{

}

File::File() :
    _fileDescriptor(-1),
    _fileLock(),
    _receiver( new SocketReceiveSide ),
    _sender( new SocketSendSide )
{

}

File::~File()
{

}

int
File::LockSend(
        PthreadMutexHolder& smutex
        )
{ 
    int rc = 0;
    if(_sender) {
        rc = smutex.Lock(&_sender->getLock());
    }
    else rc = -1;
    return rc; 
}

int
File::LockReceive(
        PthreadMutexHolder& rmutex
        )
{ 
    int rc = 0;
    if(_receiver) {
        rc = rmutex.Lock(&_receiver->getLock());
    }
    else rc = -1;
    return rc;
}

int
File::Close()
{
    return ::close(_fileDescriptor);
}

}
