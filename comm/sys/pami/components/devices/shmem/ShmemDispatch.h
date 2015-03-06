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
 * \file components/devices/shmem/ShmemDispatch.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemDispatch_h__
#define __components_devices_shmem_ShmemDispatch_h__

#include <sys/uio.h>

#include <pami.h>

#include "util/common.h"
#include "components/fifo/FifoInterface.h"
#include "util/queue/CircularQueue.h"

#include "util/trace.h"
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      template < class T_Packet, unsigned T_SetCount, unsigned T_SetSize >
      class Dispatch : public PAMI::Fifo::Interface::PacketConsumer< Dispatch<T_Packet, T_SetCount, T_SetSize> >
      {
        protected:

          class UnexpectedPacket : public CircularQueue::Element
          {
            public:

              inline UnexpectedPacket (T_Packet * packet, size_t s = 0) :
                  CircularQueue::Element (),
                  sequence (s)
              {
                TRACE_FN_ENTER();

                id = T_Packet::getDispatch (*packet);
                bytes = T_Packet::payload_size;
                memcpy ((void *) meta, T_Packet::getMetadata (*packet), T_Packet::header_size);
                memcpy ((void *) data, packet->getPayload (), T_Packet::payload_size);

                TRACE_FN_EXIT();
              };

              ///
              /// \see PAMI::Device::Interface::RecvFunction_t
              ///
              static int unexpected (void   * metadata,
                                     void   * payload,
                                     size_t   bytes,
                                     void   * recv_func_parm,
                                     void   * cookie)
              {
                TRACE_FN_ENTER();

                // The metadata is at the front of the packet.
                T_Packet * pkt = (T_Packet *) metadata;

                UnexpectedPacket * uepkt = NULL;
                pami_result_t rc;
                rc = __global.heap_mm->memalign ((void**)&uepkt, 16, sizeof(UnexpectedPacket));
                PAMI_assert_debugf(rc == PAMI_SUCCESS, "Failed to allocate Shmem::Dispatch::UnexpectedPacket from heap");
                new ((void *)uepkt) UnexpectedPacket (pkt);

                CircularQueue * q = (CircularQueue *) recv_func_parm;
                q->enqueue ((CircularQueue::Element *) uepkt);

                TRACE_FN_EXIT();
                return 0;
              }

              uint16_t id;
              size_t   sequence;
              uint8_t  meta[T_Packet::header_size];
              uint8_t  data[T_Packet::payload_size];
              size_t   bytes;
          };

        public:

          friend class PAMI::Fifo::Interface::PacketConsumer< Dispatch >;

          inline Dispatch ()
          {
            TRACE_FN_ENTER();

            COMPILE_TIME_ASSERT(T_SetCount*T_SetSize <= 65536); // 65536 == "UINT16_MAX"

            // Initialize the registered receive function array to unexpected().
            size_t i;

            for (i = 0; i < T_SetCount*T_SetSize; i++)
              {
                _function[i]   = UnexpectedPacket::unexpected;
                _clientdata[i] = (void *) & _ueQ[i/T_SetSize];
              }

            TRACE_FN_EXIT();
          };

          inline ~Dispatch () {};

          pami_result_t registerUserDispatch (size_t                      set,
                                              Interface::RecvFunction_t   function,
                                              void                      * clientdata,
                                              uint16_t                  & id)
          {
            TRACE_FN_ENTER();

            TRACE_FORMAT("set = %zu, function = %p, clientdata = %p", set, function, clientdata);
            if (set >= T_SetCount)
              {
                TRACE_FORMAT("T_SetCount = %d, return PAMI_ERROR\n", T_SetCount);
                TRACE_FN_EXIT();
                return PAMI_ERROR;
              }

            // Find the next available id for this dispatch set.
            bool found_free_slot = false;
            size_t n = set * T_SetSize + T_SetSize;

            for (id = set * T_SetSize; id < n; id++)
              {
                TRACE_FORMAT("unexpected fn = %p, n = %zu, _function[%d] = %p", UnexpectedPacket::unexpected, n, id, _function[id]);
                if (_function[id] == (Interface::RecvFunction_t) UnexpectedPacket::unexpected)
                  {
                    found_free_slot = true;
                    break;
                  }
              }

            PAMI_assert_debugf(found_free_slot == true, "Unable to find a free dispatch slot. Was this dispatch set (%zu) previously registered?", set);
            if (!found_free_slot)
              {
                TRACE_STRING("found_free_slot = false, return PAMI_ERROR");
                TRACE_FN_EXIT();
                return PAMI_ERROR;
              }

            _function[id]   = function;
            _clientdata[id] = clientdata;

            // Deliver any unexpected packets for registered dispatch ids. Stop at
            // the first unexpected packet for an un-registered dispatch id.
            UnexpectedPacket * uepkt = NULL;

            while ((uepkt = (UnexpectedPacket *) _ueQ[set].peek()) != NULL)
              {
                if (_function[uepkt->id] != UnexpectedPacket::unexpected)
                  {
                    // Invoke the registered dispatch function
                    TRACE_FORMAT("uepkt = %p, uepkt->id = %u", uepkt, uepkt->id);
                    _function[uepkt->id] (uepkt->meta,
                                          uepkt->data,
                                          uepkt->bytes,
                                          _clientdata[uepkt->id],
                                          uepkt->data);

                    // Remove the unexpected packet from the queue and free
                    _ueQ[set].dequeue();
                    free (uepkt);
                  }
                else
                  {
                    // Stop unexpected queue processing.  This maintains packet order
                    // which is required for protocols such as eager.
                    break;
                  }
              }

            TRACE_FORMAT("return %d", id);
            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

          pami_result_t registerSystemDispatch (Interface::RecvFunction_t   function,
                                                void                      * clientdata,
                                                uint16_t                  & id)
          {
            TRACE_FN_ENTER();

            // Search in reverse order for an unregistered dispatch id and
            // assign this system dispatch function to it.

            bool found_free_slot = false;

            for (id = T_SetSize * T_SetCount - 1; id > 0; id--)
              {
                if (_function[id] == (Interface::RecvFunction_t) UnexpectedPacket::unexpected)
                  {
                    found_free_slot = true;
                    break;
                  }
              }

            if (!found_free_slot)
              {
                TRACE_FN_EXIT();
                return PAMI_ERROR;
              }

            _function[id]   = function;
            _clientdata[id] = clientdata;

            // Deliver any unexpected packets for registered system dispatch ids.
            UnexpectedPacket * uepkt = NULL;

            while ((uepkt = (UnexpectedPacket *) _system_ueQ.peek()) != NULL)
              {
                if (_function[uepkt->id] != UnexpectedPacket::unexpected)
                  {
                    // Invoke the registered dispatch function
                    _function[uepkt->id] (uepkt->meta,
                                          uepkt->data,
                                          uepkt->bytes,
                                          _clientdata[uepkt->id],
                                          uepkt->data);

                    // Remove the unexpected packet from the queue and free
                    _system_ueQ.dequeue();
                    free (uepkt);
                  }
              }

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;
          };

          void dispatch (uint16_t id, void * metadata, void * payload, size_t bytes)
          {
            TRACE_FN_ENTER();
            _function[id] (metadata, payload, bytes, _clientdata[id], payload);
            TRACE_FN_EXIT();
          };

        protected:

          template <class T_FifoPacket>
          inline bool consume_impl (T_FifoPacket & packet)
          {
            TRACE_FN_ENTER();

            uint16_t id = T_Packet::getDispatch (packet);
            _function[id] (T_Packet::getMetadata (packet),
                           packet.getPayload (),
                           T_Packet::payload_size,
                           _clientdata[id],
                           packet.getPayload ());

            TRACE_FN_EXIT();
            return true;
          };

          Interface::RecvFunction_t   _function[T_SetCount*T_SetSize];
          void                      * _clientdata[T_SetCount*T_SetSize];

          CircularQueue               _ueQ[T_SetCount];
          CircularQueue               _system_ueQ;

      };
    };
  };
};
#endif // __components_devices_shmem_ShmemDispatch_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
