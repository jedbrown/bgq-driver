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
 * \file utility/include/cxxsockets/FileLocker.h
 */

#ifndef CXXSOCKET_FILE_LOCKER_H
#define CXXSOCKET_FILE_LOCKER_H

#include <bgq_util/include/pthreadmutex.h>

#include <boost/utility.hpp>

namespace CxxSockets {

struct FileLocker : boost::noncopyable
{
    PthreadMutexHolder _all;
    PthreadMutexHolder _send;
    PthreadMutexHolder _receive;
};

}

#endif
