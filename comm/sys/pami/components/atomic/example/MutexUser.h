/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file components/atomic/example/MutexUser.h
 * \brief ???
 */

#ifndef __components_atomic_example_MutexUser_h__
#define __components_atomic_example_MutexUser_h__

namespace PAMI
{
  template <T_Mutex>
  class MutexUser
  {
    public:

      inline MutexUser () :
        _mutex ()
      {
        _mutex.setNodeScope ();
        _mutex.init ();
      };

      inline void doSomething ()
      {
        _mutex.acquire ();

        // blah

        _mutex.release ();
      };

    private:

     T_Mutex _mutex;
  };
};

#endif
