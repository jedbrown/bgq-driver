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
public:
    virtual ~FileSet() = 0;
    void AddFile(FilePtr file);
    void RemoveFile(FilePtr file);
    const FilePtr& front() const { return _filevec.front(); }

protected:
    FileSet();
    void LockSet(PthreadMutexHolder& mutex);

protected:
    typedef std::vector<FilePtr>::iterator iterator;
    typedef std::vector<FilePtr>::const_iterator const_iterator; 

private:
    void pAddFile(FilePtr file);

protected:
    std::vector<FilePtr> _filevec;

private:
    PthreadMutex _setLock;
};

}

#endif
