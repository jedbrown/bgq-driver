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
/*!
 * \file utility/include/cxxsockets/FileSet.h
 */
#ifndef CXXSOCKETS_FILESET_H
#define CXXSOCKETS_FILESET_H

#include <utility/include/cxxsockets/types.h>

#include <bgq_util/include/pthreadmutex.h>

#include <vector>
#include <list>

namespace CxxSockets {
    
class FileSet
{
protected:
    std::vector<FilePtr> _filevec;
    PthreadMutex _setLock;
    bool LockSet(PthreadMutexHolder& mutex);
    virtual void pAddFile(FilePtr file);
    void RemoveInternal(FilePtr file);
public:
    typedef std::vector<FilePtr>::iterator iterator;
    typedef std::vector<FilePtr>::const_iterator const_iterator; 
    FileSet() {};
    virtual ~FileSet() {}
    virtual void AddFile(FilePtr file);
    virtual void RemoveFile(FilePtr file);
    FilePtr& front() { return _filevec.front(); }
};

}

#endif
