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

#include "cxxsockets/FileSet.h"

#include "cxxsockets/exception.h"

#include "Log.h"

LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace CxxSockets {

FileSet::FileSet() :
    _filevec(),
    _setLock()
{

}

FileSet::~FileSet()
{

}

void
FileSet::LockSet(
        PthreadMutexHolder& mutex
        )
{
    const int rc = mutex.Lock(&_setLock);
    if ( rc ) {
        std::ostringstream msg;
        char buf[256];
        msg << "Could not lock set: " << strerror_r(rc, buf, sizeof(buf));
        throw SoftError(rc, msg.str());
    }
}

// Do not lock.  Private method called by public methods that lock.
void
FileSet::pAddFile(
        FilePtr file
        )
{
    const std::vector<FilePtr>::const_iterator f = std::find(_filevec.begin(), _filevec.end(), file);
    if (f != _filevec.end()) {
        std::ostringstream msg;
        msg << "Socket already added" << std::endl;
        LOG_DEBUG_MSG( msg.str() );
        throw UserError(-1, msg.str());
    }
    _filevec.push_back(file);
}

void
FileSet::AddFile(
        FilePtr file
        )
{
    PthreadMutexHolder mutex;
    LockSet(mutex);
    pAddFile(file);
}

void
FileSet::RemoveFile(
        FilePtr file
        )
{
    PthreadMutexHolder mutex;
    LockSet(mutex);
    _filevec.erase(remove(_filevec.begin(),_filevec.end(),file), _filevec.end());
}

}
