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
 * \file algorithms/interfaces/Executor.h
 * \brief ???
 */

#ifndef __algorithms_interfaces_Executor_h__
#define __algorithms_interfaces_Executor_h__

#include "algorithms/ccmi.h"
#include "util/ccmi_util.h"
#include "util/ccmi_debug.h"
#include "PipeWorkQueue.h"

namespace CCMI
{
  namespace Interfaces
  {
    /**
     * \brief Base Class for all Executors.
     */
    class Executor
    {

    public:

      /**
       * \brief  Constructor
       */
      Executor ()
      {
        _cb_done      =  NULL;
        _clientdata   =  NULL;
      }

      /**
       * \brief  Destructor
       */
      virtual ~Executor ()
      {
      }

      /// NOTE: This is required to make "C" programs link successfully with virtual destructors
      inline void operator delete(void * p)
      {
        (void)p;
        CCMI_abort();
      }

      /**
       * \brief Start method which is called when the collective
       *  operation cal be started
       */

      virtual void start () = 0;

      /**
       * \brief notify when a message has been recived
       * \param src : source of the message
       * \param buf : address of the pipeworkqueue to produce incoming message
       * \param cb_done: completion callback
       */
      virtual void   notifyRecv     (unsigned             src,
                                     const pami_quad_t   & info,
                                     PAMI::PipeWorkQueue ** pwq,
                                     pami_callback_t      * cb_done) = 0;

      void setDoneCallback (pami_event_function cb_done, void *cd)
      {
        _cb_done    =   cb_done;
        _clientdata =   cd;
      }

    protected:
      ///
      ///  \brief Callback to call when the barrier has finished
      ///
      pami_event_function    _cb_done;
      void                * _clientdata;

    };  //--  Executor class
  };  //-- Executor Name Space
};  //-- CCMI


#endif
