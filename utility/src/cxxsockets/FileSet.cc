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


bool CxxSockets::FileSet::LockSet(PthreadMutexHolder& mutex) {
    bool rc = false;
    rc = mutex.Lock(&_setLock);
    return rc;
}

// Do not lock.  Private method called by public methods that lock.
void CxxSockets::FileSet::pAddFile(FilePtr file) {
    std::vector<FilePtr>::iterator f = std::find(begin(), end(), file);
    if (f != end()) {
        std::ostringstream msg;
        msg << "Socket already added" << std::endl;
	throw CxxSocketUserError(-1, msg.str());
    }
    push_back(file);
}

void CxxSockets::FileSet::AddFile(FilePtr file) {
    PthreadMutexHolder mutex; LockSet(mutex);
    pAddFile(file);
}

void CxxSockets::FileSet::AddFiles(std::vector<FilePtr>& files) {
    PthreadMutexHolder mutex; LockSet(mutex);
    for(std::vector<FilePtr>::iterator it = files.begin();
        it != files.end(); ++it) {
        pAddFile((*it));
    }
}

void CxxSockets::FileSet::RemoveInternal(FilePtr file) {
    erase(remove(begin(),end(),file), end()); 
}

void CxxSockets::FileSet::RemoveFile(FilePtr file) { 
    PthreadMutexHolder mutex; 
    LockSet(mutex); 
    erase(remove(begin(),end(),file), end()); 
}
