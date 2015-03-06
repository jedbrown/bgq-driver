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
 * \file components/devices/shmem/ShmemPacketMessage.h
 * \brief ???
 *
 * Need several types of messages:
 * 1. "immediate" fifo operation with local completion
 * 2. "immediate" fifo operation with remote completion
 * 3. "fence" fifo operation with local completion
 * 4. "fence" fifo operation with remote completion
 * 3. "fence" shaddr operation with local completion
 *
 * "immediate" means that the packet is written as soon as space is
 * available in the fifo.
 *
 * "fence" means that the packet is only written after all previous
 * packets from the origin context have been received.
 *
 * "fifo operation" means a packet is produced into the fifo.
 *
 * "shaddr operation" means a shared address read/write is performed.
 */

#ifndef __components_devices_shmem_ShmemPacketMessage_h__
#define __components_devices_shmem_ShmemPacketMessage_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"

#include <pami.h>

#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemMessage.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      template < class T_Device, class T_Functor, bool T_Remote = false, bool T_Fence = false >
      class PacketMessage : public SendQueue::Message
      {
        protected:

          static void remote_completion_intercept (pami_context_t   context,
                                                   void           * cookie,
                                                   pami_result_t    status)
          {
            PacketMessage * msg = (PacketMessage *) cookie;

            // No completion notification is needed if the function pointer is NULL
            if (unlikely(msg->_user_fn == NULL)) return;

            size_t sequence = msg->_device->_fifo[msg->_fnum].lastPacketProduced();
            msg->_device->postCompletion (msg->_state,
                                          msg->_user_fn,
                                          msg->_user_cookie,
                                          msg->_fnum,
                                          sequence);

            return ;
          };

          /// invoked by the thread object
          /// \see SendQueue::Message::_work
          static pami_result_t __advance (pami_context_t context, void * cookie)
          {
            PacketMessage * msg = (PacketMessage *) cookie;
            return msg->advance();
          };

          inline pami_result_t write ()
          {
            // Attempt to write the packet into the fifo as soon as space
            // becomes available.

            while (_device->_fifo[_fnum].producePacket(_writer) == true)
              {
                if (_writer.isDone())
                  {
                    // This removes the work from the generic device.
                    this->setStatus (PAMI::Device::Done);

                    // This causes the message completion callback to be invoked.
                    return PAMI_SUCCESS;
                  }
              }

            return PAMI_EAGAIN;
          };

          inline pami_result_t advance ()
          {
            TRACE_ERR((stderr, ">> PacketMessage::advance(), device->isSendQueueEmpty(%zu) = %d\n", _fnum, _device->isSendQueueEmpty (_fnum)));

            if (T_Fence == false)
              return this->write ();

            // Do not attempt to write the packet into the fifo until all
            // previous packets from this origin context have been received.
            if (! _device->activePackets(_fnum))
              return this->write ();

            TRACE_ERR((stderr, "<< PacketMessage::advance(), return PAMI_EAGAIN\n"));
            return PAMI_EAGAIN;
          };

        public:
          inline PacketMessage (pami_event_function   fn,
                                void                * cookie,
                                T_Device            * device,
                                size_t                fnum,
                                T_Functor           & writer) :
              SendQueue::Message (PacketMessage::__advance, this, fn, cookie, device->getContextOffset()),
              _writer (writer),
              _device (device),
              _fnum (fnum)
          {
            TRACE_ERR((stderr, ">> PacketMessage::PacketMessage()\n"));

            if (T_Remote == true)
              {
                // Intercept the message completion callback and invoke the
                // "post remote completion work" callback instead of the user
                // callback when the message completes.
                _user_fn       = fn;
                _user_cookie   = cookie;
                _cb.function   = remote_completion_intercept;
                _cb.clientdata = (void *) this;
              }

            TRACE_ERR((stderr, "<< PacketMessage::PacketMessage()\n"));
          };

          inline PacketMessage (pami_event_function   fn,
                                void                * cookie,
                                T_Device            * device,
                                size_t                fnum,
                                uint16_t              dispatch_id,
                                void                * metadata,
                                size_t                metasize,
                                void                * payload,
                                size_t                bytes) :
              SendQueue::Message (PacketMessage::__advance, this, fn, cookie, device->getContextOffset()),
              _writer (dispatch_id),
              _device (device),
              _fnum (fnum)
          {
            TRACE_ERR((stderr, ">> PacketMessage::PacketMessage()\n"));

            _writer.setMetadata (metadata, metasize);
            _writer.setData (payload, bytes);

            if (T_Remote == true)
              {
                // Intercept the message completion callback and invoke the
                // "post remote completion work" callback instead of the user
                // callback when the message completes.
                _user_fn       = fn;
                _user_cookie   = cookie;
                _cb.function   = remote_completion_intercept;
                _cb.clientdata = (void *) this;
              }

            TRACE_ERR((stderr, "<< PacketMessage::PacketMessage()\n"));
          };

          T_Functor             _writer;

        protected:

          T_Device            * _device;
          size_t                _fnum;
          uint8_t               _state[T_Device::completion_work_size];
          pami_event_function   _user_fn;
          void                * _user_cookie;

      };  // PAMI::Device::PacketMessage class
    };
  };    // PAMI::Device namespace
};      // PAMI namespace
#undef TRACE_ERR
#endif  // __components_devices_shmem_ShmemPacketMessage_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
