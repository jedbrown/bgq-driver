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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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
#ifndef RASSMARTLOCK_H_
#define RASSMARTLOCK_H_

#include <pthread.h>

// This class implements a "smart lock" idiom for a pthread mutex.
// The constructor locks the mutex and the destructor unlocks it.
class RasSmartLock
{
 public:
  pthread_mutex_t* _lock;
  RasSmartLock(pthread_mutex_t* lock)
  {
    _lock = lock;
    pthread_mutex_lock(_lock);
  }
  
  ~RasSmartLock()
  {
    pthread_mutex_unlock(_lock);
  }
};

#endif /*RASSMARTLOCK_H_*/
