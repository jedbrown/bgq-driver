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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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

//! \file  Counter.h
//! \brief Declaration and inline methods for bgcios::Counter class.

#ifndef COMMON_COUNTER_H
#define COMMON_COUNTER_H

// Includes
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <tr1/memory>

namespace bgcios
{

//! \brief  Counter for synchronization between threads in a process.
//!
//! This class implements a thread-safe synchronization mechanism using a pthread
//! mutex and condition variable.  It can be used for threads to wait for events or 
//! for multiple threads to keep a shared counter of events or status.

class Counter
{
public:

   //! \brief  Default constructor.

   Counter()
   {
      // Initialize private data.
      _value = 0;
      _waitValue = 0;

      // Create the mutex and condition variable.
      pthread_mutex_init(&_mutex, NULL);
      pthread_cond_init(&_condition, NULL);
   }

   //! \brief  Default destructor.

   ~Counter()
   {
      // Destroy the mutex and condition variable.
      pthread_mutex_destroy(&_mutex);
      pthread_cond_destroy(&_condition);
   }

   //! \brief  Wait for the counter to become non-zero.
   //! \return 0 when successful, errno when unsuccessful.

   int wait(void)
   {
      // Lock the mutex.
      int rc = pthread_mutex_lock(&_mutex);

      // Wait for the value of the counter to become non-zero.
      while (_value == 0) {
         rc = pthread_cond_wait(&_condition, &_mutex);
      }

      // Unlock the mutex;
      pthread_mutex_unlock(&_mutex);

      return rc;
   }

   //! \brief  Wait for the counter to reach a specified value.
   //! \param  val Value to wait for.
   //! \return 0 when successful, errno when unsuccessful.

   int waitForValue(int val)
   {
      // Lock the mutex.
      int rc = pthread_mutex_lock(&_mutex);

      // Wait for the value of the counter to reach the specified value.
      _waitValue = val;
      while (_value < _waitValue) {
         rc = pthread_cond_wait(&_condition, &_mutex);
      }

      // Unlock the mutex;
      pthread_mutex_unlock(&_mutex);

      return rc;
   }

   //! \brief  Wait for the value of the counter to become zero.
   //! \return 0 when successful, errno when unsuccessful.

   int waitForZero(void)
   {  
      // Lock the mutex.
      int rc = pthread_mutex_lock(&_mutex);

      // Wait for the value of the counter to become zero.
      while (_value != 0) {
         rc = pthread_cond_wait(&_condition, &_mutex);
      }

      // Unlock the mutex;
      pthread_mutex_unlock(&_mutex);

      return rc;
   }

   //! \brief  Increment the value of the counter by one.
   //! \return 0 when successful, errno when unsuccessful.

   int increment(void)
   {
      // Lock the mutex.
      int rc = pthread_mutex_lock(&_mutex);

      // Increment the value and signal a waiter if needed.
      _value += 1;
      if (_value >= _waitValue) {
         rc = pthread_cond_signal(&_condition);
      }

      // Unlock the mutex.
      pthread_mutex_unlock(&_mutex);

      return rc;
   }

   //! \brief  Increment the value of the counter by one only if the current value is zero.
   //! \return 0 when successful, errno when unsuccessful.

   int incrementFromZero(void)
   {
      // Lock the mutex.
      int rc = pthread_mutex_lock(&_mutex);

      // Increment the value and signal a waiter if needed.
      if (_value == 0) {
         _value += 1;
         if (_value >= _waitValue) {
            rc = pthread_cond_signal(&_condition);
         }
      }

      // Unlock the mutex.
      pthread_mutex_unlock(&_mutex);

      return rc;
   }

   //! \brief  Decrement the value of the counter by one.
   //! \return 0 when successful, errno when unsuccessful.

   int decrement(void)
   {
      // Lock the mutex.
      int rc = pthread_mutex_lock(&_mutex);

      // Decrement the value and signal a waiter if needed.
      _value -= 1;
      if (_value == 0) {
         rc = pthread_cond_signal(&_condition);
      }

      // Unlock the mutex.
      pthread_mutex_unlock(&_mutex);

      return rc;
   }

   //! \brief  Decrement the value of the counter by one only if the current value is not zero.
   //! \return 0 when successful, errno when unsuccessful.

   int decrementToZero(void)
   {
      // Lock the mutex.
      int rc = pthread_mutex_lock(&_mutex);

      // Decrement the value and signal a waiter if needed.
      if (_value > 0) {
         _value -= 1;
         if (_value == 0) {
            rc = pthread_cond_signal(&_condition);
         }
      }

      // Unlock the mutex.
      pthread_mutex_unlock(&_mutex);

      return rc;
   }

   //! \brief  Get the current value of the counter.
   //! \return Current value.

   inline int getValue(void)
   {
      // Lock the mutex.
      pthread_mutex_lock(&_mutex);

      // Get the value.
      volatile int val = _value;

      // Unlock the mutex.
      pthread_mutex_unlock(&_mutex);

      return (int)val;
   }

   //! \brief  Set the value of the counter to a specific value.
   //! \param  val New value of counter.
   //! \return 0 when successful, errno when unsuccessful.

   int setValue(int val)
   {
      // Lock the mutex.
      int rc = pthread_mutex_lock(&_mutex);
      if (rc != 0) {
         return rc;
      }

      // Set the value.
      _value = val;

      // Unlock the mutex.
      pthread_mutex_unlock(&_mutex);

      return 0;
   }

private:

   //! Mutex for lock.
   pthread_mutex_t _mutex;

   //! Condition for blocking.
   pthread_cond_t _condition;

   //! Current value of counter.
   sig_atomic_t _value;

   //! Value to wait for.
   sig_atomic_t _waitValue;
};

//! Smart pointer for Counter object.
typedef std::tr1::shared_ptr<Counter> CounterPtr;

}

#endif // COMMON_COUNTER_H

