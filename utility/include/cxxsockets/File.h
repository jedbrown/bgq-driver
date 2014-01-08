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

#include <boost/shared_ptr.hpp>

namespace CxxSockets {

class SocketReceiveSide;
class SocketSendSide;

class File
{
protected:
    int _fileDescriptor;
    PthreadMutex _fileLock;
    boost::shared_ptr<SocketReceiveSide> _receiver;
    boost::shared_ptr<SocketSendSide> _sender;

    //! \brief Mutual exclusion for all operations on this file.
    int LockFile(FileLocker& locker);
    //! \brief Lock send side only
    int LockSend(PthreadMutexHolder& mutex); 
    //! \brief Lock receive side only
    int LockReceive(PthreadMutexHolder& mutex);

    //! construct object with pre-existing descriptor
    File(int descriptor);

public:
    File();

    int getFileDescriptor() const { return _fileDescriptor; }

    virtual ~File() = 0;

    //! \brief Close the file
    int Close();
};

}

#endif

