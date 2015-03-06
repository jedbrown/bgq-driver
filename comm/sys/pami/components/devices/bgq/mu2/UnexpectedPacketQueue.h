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
 * \file components/devices/bgq/mu2/UnexpectedPacketQueue.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_UnexpectedPacketQueue_h__
#define __components_devices_bgq_mu2_UnexpectedPacketQueue_h__

#include "util/queue/Queue.h"

#include "util/trace.h"
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      template <typename T>
      class UnexpectedPacketQueue : public Queue
      {
        public:

          class UnexpectedPacket : public Queue::Element
          {
            public:

              UnexpectedPacket (uint8_t  * m,
                                uint8_t  * p,
                                size_t     b,
                                uint16_t   i) :
                  Queue::Element (),
                  metadata (m),
                  payload (p),
                  bytes (b),
                  id (i)
              {
              };

              ~UnexpectedPacket () {};

              uint8_t  * metadata;
              uint8_t  * payload;
              size_t     bytes;
              uint16_t   id;
          };

          UnexpectedPacketQueue (T dispatch_count, size_t maximum_metadata_bytes) :
              Queue (),
              _maximum_metadata_bytes (maximum_metadata_bytes)
          {
            UnexpectedPacketQueue ** queue = (UnexpectedPacketQueue **) malloc (sizeof(T) * dispatch_count + sizeof (UnexpectedPacketQueue *));
            *queue = this;

            uintptr_t ptr = (uintptr_t) queue;
            _id = (T *) (ptr + sizeof(uintptr_t));

            unsigned i;

            for (i = 0; i < dispatch_count; i++)
              {
                _id[i] = i;
              }
          };

          ~UnexpectedPacketQueue () {};

          void * getCookie (size_t id)
          {
            return (void *) & _id[id];
          };

          /// \see PAMI::Device::Interface::RecvFunction_t
          static int dispatch_fn (void   * metadata,
                                  void   * payload,
                                  size_t   bytes,
                                  void   * recv_func_parm,
                                  void   * cookie)
          {
            uint16_t * id = (uint16_t *) recv_func_parm;

            TRACE_FORMAT("Warning. Dispatch to unregistered id (%d).\n", *id);

            uint16_t * array = id - *id;

            UnexpectedPacketQueue ** queue = ((UnexpectedPacketQueue **) array) - 1;

            UnexpectedPacket * packet = (UnexpectedPacket *) malloc (sizeof(UnexpectedPacket) + bytes + (*queue)->_maximum_metadata_bytes);

            uint8_t * tmp_payload  = (uint8_t *) (packet + 1);
            memcpy (tmp_payload, payload, bytes);

            uint8_t * tmp_metadata = (uint8_t *) (tmp_payload + bytes);
            memcpy (tmp_metadata, metadata, (*queue)->_maximum_metadata_bytes);

            new (packet) UnexpectedPacket (tmp_metadata, tmp_payload, bytes, *id);

            (*queue)->enqueue (packet);

            return 0;
          };

          uint16_t              * _id;
          unsigned                _maximum_metadata_bytes;

      };   // class PAMI::Device::MU::UnexpectedPacketQueue
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_UnexpectedPacketQueue_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
