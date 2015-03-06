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
 * \file components/devices/shmem/ShmemFenceMessage.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemFenceMessage_h__
#define __components_devices_shmem_ShmemFenceMessage_h__

#include <errno.h>
#include <pami.h>

#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemMessage.h"

#include "util/trace.h"

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      template < class T_Device>
      class FenceMessage : public SendQueue::Message
      {
        protected:

          /// invoked by the thread object
          /// \see SendQueue::Message::_work
          static pami_result_t __advance (pami_context_t context, void * cookie)
          {
            TRACE_FN_ENTER();

            FenceMessage * msg = (FenceMessage *) cookie;
            pami_result_t result = msg->advance();

            TRACE_FORMAT("return result=%d", result);
            TRACE_FN_EXIT();
            return result;
          };

          inline pami_result_t advance ()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("_fnum = %zu", _fnum);

            // Block until all previous packets from this origin context have
            // been received.
            if (_device->activePackets(_fnum))
              {
                TRACE_FORMAT("return result=%d", PAMI_EAGAIN);
                TRACE_FN_EXIT();
                return PAMI_EAGAIN;
              }

            // This removes the work from the generic device.
            this->setStatus (PAMI::Device::Done);

            // This causes the message completion callback to be invoked.
            TRACE_FORMAT("return result=%d", PAMI_SUCCESS);
            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

        public:

          inline FenceMessage (pami_event_function   fn,
                               void                * cookie,
                               T_Device            * device,
                               size_t                fnum) :
              SendQueue::Message (FenceMessage::__advance, this, fn, cookie, device->getContextOffset()),
              _device (device),
              _fnum (fnum)
          {
            TRACE_FN_ENTER();
            TRACE_FN_EXIT();
          };

        protected:

          T_Device            * _device;
          size_t                _fnum;
          pami_event_function   _user_fn;
          void                * _user_cookie;

      };  // PAMI::Device::Shmem::FenceMessage class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif  // __components_devices_shmem_ShmemFenceMessage_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
