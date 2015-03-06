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
 * \file components/devices/shmem/ShmemMessage.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemMessage_h__
#define __components_devices_shmem_ShmemMessage_h__

#include <sys/uio.h>

#include <pami.h>

#include "util/common.h"
#include "util/queue/Queue.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/generic/Message.h"

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      // GenericDeviceMessageQueue defined in GenericDevicePlatform.h
      class SendQueue : public GenericDeviceMessageQueue
      {
        public:

          class Message : public PAMI::Device::Generic::GenericMessage
          {
            protected:

              friend class SendQueue;

              inline Message (pami_work_function    work_func,
                              void               * work_cookie,
                              pami_event_function   done_fn,
                              void               * done_cookie,
                              size_t               contextid) :
                  PAMI::Device::Generic::GenericMessage(NULL, (pami_callback_t) {done_fn, done_cookie}, 0, contextid),
              _work (work_func, work_cookie),
              _genericdevice (NULL)
              {
                TRACE_ERR((stderr, "<> SendQueue::Message::Message()\n"));
                _work.setStatus (Ready);
              };

              virtual ~Message () {};

              void setup (PAMI::Device::Generic::Device * device, SendQueue * sendQ)
              {
                TRACE_ERR((stderr, ">> SendQueue::Message::setup(%p, %p)\n", device, sendQ));

                _genericdevice = device;
                this->_QS = sendQ;

                // Initialize the message and work state
                this->setStatus (Initialized);
                _work.setStatus (Ready);

                TRACE_ERR((stderr, "<< SendQueue::Message::setup(%p, %p)\n", device, sendQ));
              }

            public:

              /// \note This is required to make "C" programs link successfully with virtual destructors
              inline void operator delete (void * p)
              {
                (void)p;
                PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
              }

              ///
              /// \brief virtual function implementation
              /// \see PAMI::Device::Generic::GenericMessage::postNext()
              ///
              /// Post this message to the appropriate generic device, this is the
              /// completion message and the thread (work) message(s).
              ///
              /// \todo Figure out the input parameters and the return value
              ///
              inline pami_context_t postNext (bool something)
              {
                (void)something;
                TRACE_ERR((stderr, ">> SendQueue::Message::postNext(%d)\n", something));
                PAMI_assert_debug (_genericdevice != NULL);

                _genericdevice->postMsg ((PAMI::Device::Generic::GenericMessage *) this);
                _genericdevice->postThread ((PAMI::Device::Generic::GenericThread *) &_work);

                TRACE_ERR((stderr, "<< SendQueue::Message::postNext(%d), return NULL\n", something));
                return NULL; // what should this be?
              };

            protected:

              PAMI::Device::Generic::GenericThread _work;
              PAMI::Device::Generic::Device * _genericdevice;

          };  // PAMI::Device::Shmem::SendQueue::Message class

          inline SendQueue () :
              GenericDeviceMessageQueue (),
              _progress (NULL)
          {
          };

          inline void init (PAMI::Device::Generic::Device * progress)
          {
            TRACE_ERR((stderr, ">> SendQueue::init(%p) this=%p\n", progress, this));
            _progress = progress;
          };

          /// \brief post the message to be advanced later
          ///
          /// First, post to the "secondary" queue (owned by the shmem device),
          /// then, if the secondary queue was empty, post to the "primary"
          /// queue (owned by the generic device)
          inline void post (SendQueue::Message * msg)
          {
            TRACE_ERR((stderr, ">> SendQueue::post(%p)\n", msg));

            // Initialize the message for this queue
            TRACE_ERR((stderr, "   SendQueue::post() progress = %p, this = %p\n", _progress, this));
            msg->setup (_progress, this);

            // If the "local" queue is empty, then post this message, and its
            // associated work, to the generic device. Otherwise, simply add
            // the message to the local queue.
            if (this->isEmpty())
              {
                msg->postNext(true);
              }

            this->enqueue (msg);

            TRACE_ERR((stderr, "<< SendQueue::post(%p)\n", msg));
          };

        private:
          PAMI::Device::Generic::Device * _progress;

      };  // PAMI::Device::Shmem::SendQueue class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef TRACE_ERR
#endif // __components_devices_shmem_ShmemMessage_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
