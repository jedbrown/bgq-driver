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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  Thread.cc
//! \brief Methods for bgcios::Thread class.

// Includes
#include <ramdisk/include/services/common/Thread.h>
#include <ramdisk/include/services/common/logging.h>
#include <errno.h>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

int
Thread::setDetached(void)
{
   // Note that attributes can only be set before the thread is started.
   int rc = EBUSY;
   if (_threadId == 0) {
      rc = pthread_attr_setdetachstate(&_attributes, PTHREAD_CREATE_DETACHED);
   }
   return rc;
}

bool
Thread::isDetached(void)
{
   int value = 0;
   pthread_attr_getdetachstate(&_attributes, &value);
   return (value == PTHREAD_CREATE_DETACHED);
}

int
Thread::setStackSize(size_t stackSize)
{
   // Note that attributes can only be set before the thread is started.
   int rc = EBUSY;
   if (_threadId == 0) {
      rc = pthread_attr_setstacksize(&_attributes, stackSize);
   }
   return rc;
}

size_t
Thread::getStackSize(void)
{
   size_t stackSize = 0;
   pthread_attr_getstacksize(&_attributes, &stackSize);
   return stackSize;
}

int
Thread::start(void)
{
   int rc = EBUSY;
   if (_threadId == 0) {
      rc = pthread_create(&_threadId, &_attributes, startWrapper, this);
   }
   return rc;
}

void *
Thread::startWrapper(void *arg)
{
   Thread *thisPtr = (Thread *)arg;
   void *returnVal = thisPtr->run();
   return returnVal;
}
