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
 * \file algorithms/executor/Executor.h
 * \brief ???
 */
#ifndef __algorithms_executor_Executor_h__
#define __algorithms_executor_Executor_h__

#include "algorithms/ccmi.h"
#include "util/ccmi_util.h"
#include "util/ccmi_debug.h"

#include "common/MultisendInterface.h"

namespace CCMI
{
  namespace Executor
  {

    /**
     * \brief Base Class for all Executors
     **/

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
          _consistency  =  PAMI_UNDEFINED_CONSISTENCY;
        }

        /**
         * \brief  Destructor
         */
        virtual ~Executor ()
        {
//           _cb_done      =  NULL;
//           _clientdata   =  NULL;
//           _consistency  =  CCMI_UNDEFINED_CONSISTENCY;
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
         * \brief notification called when the send/multisend
         * operation has finished
         */

        virtual void notifySendDone ( const pami_quad_t &info ) = 0;


        /**
         * \brief notify when a message has been recived
         * \param src : source of the message
         * \param buf : address of the incoming message
         * \param bytes : number of bytes received
         */

        virtual void notifyRecv (unsigned src, const pami_quad_t &info, char * buf, unsigned bytes) = 0;

        void setDoneCallback (pami_event_function cb_done, void *cd)
        {
          _cb_done    =   cb_done;
          _clientdata =   cd;
        }

        void setConsistency  (pami_consistency_t consistency)
        {
          _consistency = consistency;
        }

        pami_consistency_t getConsistency  ()
        {
          return _consistency;
        }

      protected:
        ///
        ///  \brief Callback to call when the barrier has finished
        ///
        pami_event_function    _cb_done;
        void                * _clientdata;

        ///
        ///  \brief Consistency required to perform the collective
        ///
        pami_consistency_t      _consistency;
    };  //--  Executor class
  };  //-- Executor Name Space
};  //-- CCMI


#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
