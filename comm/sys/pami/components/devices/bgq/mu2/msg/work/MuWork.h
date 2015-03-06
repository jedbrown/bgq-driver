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
 * \file components/devices/bgq/mu2/msg/work/MuWork.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_msg_work_MuWork_h__
#define __components_devices_bgq_mu2_msg_work_MuWork_h__

#include "sys/pami.h"

#include "components/devices/generic/AdvanceThread.h"

#undef TRACE_ERR
#define TRACE_ERR(x) // fprintf x

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      template <class T>
      class MuWork : public Generic::GenericThread
      {
        protected:

          /// invoked by the base thread object
          /// \see PAMI::Device::Generic::GenericThread
          static pami_result_t __advance_cb (pami_context_t context, void * cookie)
          {
            T * work = (T *) cookie;

            pami_result_t result = T->advance (context);

            if (result == PAMI_SUCCESS)
              {
                // The work is complete.
                T->invokeCallback (context);

              }

            return result;
          };

          /// invoked by the base thread object
          /// \see PAMI::Device::Generic::GenericThread
          static pami_result_t __advance_msg (pami_context_t context, void * cookie)
          {
            T * work = (T *) cookie;

            pami_result_t result = T->advance (context);

            if (result == PAMI_SUCCESS)
              {
                // The work is complete.
                T->setMessageDone ();
              }

            return result;
          };

          inline MuWork (pami_event_function   done_fn,
                         void                * done_cookie,
                         size_t                contextid) :
              GenericThread (__advance_cb, this),
              _cb ((pami_callback_t) {done_fn, done_cookie}),
              _contextid (contextid)
          {
          };

          inline MuWork (Generic::GenericMessage * msg) :
              GenericThread (__advance_msg, this),
              _msg (msg)
          {
          };

          inline void invokeCallback (pami_context_t context)
          {
            _cb.function (_cb.clientdata, context, PAMI_SUCCESS);
          }

          inline void setMessageDone ()
          {
            _msg->setStatus (PAMI::Device::Done);
          }

          pami_callback_t           _cb;
          size_t                    _contextid;
          Generic::GenericMessage * _msg;

      };  // PAMI::Device::MU::MuWork class
    };    // PAMI::Device::MU namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __components_devices_bgq_mu2_msg_work_MuWork_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
