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

bool
FileSet::LockSet(
        PthreadMutexHolder& mutex
        )
{
    return mutex.Lock(&_setLock);
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
	throw UserError(-1, msg.str());
    }
    _filevec.push_back(file);
}

void
FileSet::AddFile(
        FilePtr file
        )
{
    PthreadMutexHolder mutex; LockSet(mutex);
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
