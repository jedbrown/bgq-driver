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
 * \file algorithms/composite/Composite.h
 * \brief An executor composite class which supports optional barriers
 */

#ifndef __algorithms_composite_Composite_h__
#define __algorithms_composite_Composite_h__

#include "algorithms/executor/Executor.h"

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

#include "util/trace.h"

#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif


namespace CCMI
{
  namespace Executor
  {

    class Composite
    {
      public:
        //Base Composite class
        Composite() 
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>",this);
          TRACE_FN_EXIT();
        }

        ///
        /// \brief Destructor
        ///
        virtual ~Composite() 
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>",this);
          TRACE_FN_EXIT();
        }

        void setDoneCallback (pami_event_function cb_done, void *cd)
        {
          TRACE_FN_ENTER();
          _cb_done    =   cb_done;
          _clientdata =   cd;
          TRACE_FORMAT("<%p> %p(%p)",this,_cb_done,_clientdata);
          TRACE_FN_EXIT();
        }

        ///
        /// \brief start a freshly constructed algorithm composite
        ///
        virtual void start()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p> do nothing",this);
          TRACE_FN_EXIT();
        }

        ///
        /// \breif start a previously constructed algorithm composite
        ///
        virtual unsigned restart (void *cmd)
        {
          (void)cmd;
          //Currently not all composites implement this method
          CCMI_abort();
          return PAMI_SUCCESS;
        }

        void   setAlgorithmFactory (void *f) 
        { 
          TRACE_FN_ENTER();
          _afactory = f; 
          TRACE_FORMAT("<%p> %p",this,f);
          TRACE_FN_EXIT();
        }

        void  * getAlgorithmFactory() 
        { 
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p> %p",this,_afactory);
          TRACE_FN_EXIT();
          return _afactory; 
        }


        /**
         * \brief notify when a message has been recived
         * \param src : source of the message
         * \param buf : address of the pipeworkqueue to produce incoming message
         * \param cb_done: completion callback
         * \param cookie: hint to which executor this recv is for in a multi executor composite
         */
        virtual void   notifyRecv     (unsigned              src,
                                       const pami_quad_t   & metadata,
                                       PAMI::PipeWorkQueue ** pwq,
                                       pami_callback_t      * cb_done,
                                       void                 * cookie)
        {
          // -pedantic warning
          (void)src;(void)metadata;(void)pwq;(void)cb_done;(void)cookie;
          //Currently not all composites implement this method
          CCMI_abort();
        }
        inline void setContext(pami_context_t ctxt) 
        {
          TRACE_FN_ENTER();
          _context=ctxt;
          TRACE_FORMAT("<%p> %p",this,ctxt);
          TRACE_FN_EXIT();
        }
        inline pami_context_t getContext() 
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p> %p",this,_context);
          TRACE_FN_EXIT();
          return _context;
        }

	void cleanup () {
	  this->~Composite();  //Call the destructor and avoid namespace issues
	}

      protected:
        ///
        ///  \brief Callback to call when the barrier has finished
        ///
        pami_event_function    _cb_done;
        void                * _clientdata;
        pami_context_t        _context;
        //Store a pointer to the algorithm factory
        void                * _afactory;

    };


    template <int NUM_EXECUTORS, class T_Bar, class T_Exec>
    class CompositeT : public Composite
    {
      protected:
        ///
        /// \brief for synchronous protocols which need a barrier
        /// before the collective. Asynchronous protocols can leave
        /// this variable as NULL.
        ///
        T_Bar        * _barrier;

        ///
        /// \brief Executors for data movement
        ///
        T_Exec       * _executors [NUM_EXECUTORS];
        unsigned       _numExecutors;

      public:

        CompositeT () : Composite()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>",this);
          reset();
          TRACE_FN_EXIT();
        }

        /// Default Destructor
        virtual ~CompositeT()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>",this);
          TRACE_FN_EXIT();
          for (unsigned count = 0; count < _numExecutors; count ++)
            {
              _executors[count]->~T_Exec();
              _executors[count] = NULL;
            }

          _numExecutors = 0;
        }
        ///NOTE: This is required to make "C" programs link successfully with virtual destructors
        inline void operator delete(void * p)
        {
          CCMI_abort();
        }

        void addExecutor (T_Exec *exec)
        {
          CCMI_assert (_numExecutors < NUM_EXECUTORS);
          _executors [_numExecutors] = exec;
          _numExecutors ++;
        }

        void addBarrier (T_Bar *exec)
        {
          _barrier = exec;
        }

        T_Exec * getExecutor (int idx)
        {
          return _executors [idx];
        }

        unsigned getNumExecutors ()
        {
          return _numExecutors;
        }

        inline void reset () 
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>",this);
          _barrier = NULL;
          for (unsigned count = 0; count < NUM_EXECUTORS; count ++) 
            _executors[count] = NULL;	  
          _numExecutors = 0;
          TRACE_FN_EXIT();
        }

        //virtual void start () = 0;
    };  //-- end class Composite

  };  //-- end namespace Executor
};  //-- end namespace CCMI


#endif
