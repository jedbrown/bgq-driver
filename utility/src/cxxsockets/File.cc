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

#include "Log.h"

#include <fcntl.h>

LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace CxxSockets {

void
File::LockFile(
        FileLocker& locker
        ) const
{
    // Get the whole socket lock first to ensure that
    // neither the sender nor the receiver gets invalidated
    // between the existence check and the lock() call.
    int rc = locker._all.Lock(&_fileLock);
    if ( rc ) {
        std::ostringstream msg;
        char buf[256];
        msg << "Could not lock file: " << strerror_r(rc, buf, sizeof(buf));
        throw SoftError(rc, msg.str());
    }

    // Note, we don't have to have either or both.  If they've
    // been invalidated, we don't care here.
    if (_sender) {
        rc = locker._send.Lock(_sender.get());
        if ( rc ) {
            std::ostringstream msg;
            char buf[256];
            msg << "Could not lock sender: " << strerror_r(rc, buf, sizeof(buf));
            throw SoftError(rc, msg.str());
        }
    }
    if (_receiver) {
        rc = locker._receive.Lock(_receiver.get());
        if ( rc ) {
            std::ostringstream msg;
            char buf[256];
            msg << "Could not lock receiver: " << strerror_r(rc, buf, sizeof(buf));
            throw SoftError(rc, msg.str());
        }
    }
}

File::File() :
    _fileLock(),
    _fileDescriptor(-1),
    _receiver( new PthreadMutex ),
    _sender( new PthreadMutex )
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
    if (_sender) {
        rc = smutex.Lock(_sender.get());
    } else {
        rc = -1;
    }
    return rc;
}

int
File::LockReceive(
        PthreadMutexHolder& rmutex
        )
{
    int rc = 0;
    if (_receiver) {
        rc = rmutex.Lock(_receiver.get());
    } else {
        rc = -1;
    }
    return rc;
}

int
File::Close()
{
    if ( _fileDescriptor == -1 ) return 0;

    return ::close(_fileDescriptor);
}

}
