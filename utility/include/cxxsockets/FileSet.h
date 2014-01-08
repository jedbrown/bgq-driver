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
#ifndef _FILESET_H
#define _FILESET_H

#include <vector>
#include <list>
#ifndef _SOCKETTYPES_H
#include "SocketTypes.h"
#endif
#ifndef _SOCKETADDRS_H
#include "SocketAddrs.h"
#endif

#include <bgq_util/include/pthreadmutex.h>

namespace CxxSockets {
    
    class FileSet {
    protected:
        std::vector<FilePtr> _filevec;
        bool _blocking;
        std::list<FilePtr> _readySockets;
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
        virtual void AddFiles(std::vector<FilePtr>& files);
        virtual void RemoveFile(FilePtr file);
        FilePtr& front() { return _filevec.front(); }
        size_t size() { return _filevec.size(); }
        void push_back(FilePtr sp) { _filevec.push_back(sp); }
        void pop_back() { _filevec.pop_back(); }
        iterator insert ( iterator position, FilePtr& sp ) { return _filevec.insert(position, sp); }
        iterator begin() { return _filevec.begin(); }                                        
        iterator end() { return _filevec.end(); } 
        iterator erase ( iterator first, iterator last ) { return _filevec.erase(first, last); }
        const_iterator begin() const { return _filevec.begin(); }                            
        const_iterator end() const { return _filevec.end(); }        
    };
}

#endif
