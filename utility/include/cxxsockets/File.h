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
 * \file include/cxxsockets/File.h
 */

#ifndef CXXSOCKET_FILE_H
#define CXXSOCKET_FILE_H

#include <bgq_util/include/pthreadmutex.h>

#include <utility/include/cxxsockets/types.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace CxxSockets {

class File : boost::noncopyable
{
private:
    mutable PthreadMutex _fileLock;

protected:
    int _fileDescriptor;
    boost::scoped_ptr<PthreadMutex> _receiver;
    boost::scoped_ptr<PthreadMutex> _sender;

    //! \brief Mutual exclusion for all operations on this file.
    void LockFile(FileLocker& locker) const;
    //! \brief Lock send side only
    int __attribute__((warn_unused_result)) LockSend(PthreadMutexHolder& mutex); 
    //! \brief Lock receive side only
    int __attribute__((warn_unused_result)) LockReceive(PthreadMutexHolder& mutex);

    File();

    //! \brief Close the file
    int Close();

public:
    int getFileDescriptor() const { return _fileDescriptor; }

    virtual ~File() = 0;
};

}

#endif

