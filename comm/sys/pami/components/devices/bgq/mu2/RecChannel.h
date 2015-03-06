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
 * \file components/devices/bgq/mu2/RecChannel.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_RecChannel_h__
#define __components_devices_bgq_mu2_RecChannel_h__

#include <spi/include/mu/RecFifo.h>

#include "components/devices/bgq/mu2/MemoryFifoPacketHeader.h"
#include "components/devices/bgq/mu2/UnexpectedPacketQueue.h"

#include "util/trace.h"
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      ///
      /// \brief The reception channel component encapsulates all resources
      ///        associated with a reception fifo.
      ///
      class RecChannel
      {
        protected:

          /// Total number of dispatch sets
          static const size_t dispatch_set_count = 512;

          /// Number of dispatch functions in a dispatch set
          static const size_t dispatch_set_size  = 8;

          /// System notification dispatch identifier
          static const uint16_t dispatch_system_notify = dispatch_set_count * dispatch_set_size - 1;

          typedef struct
          {
            pami_event_function   fn;
            void                * cookie;
          } notify_t;

          /// \see PAMI::Device::Interface::RecvFunction_t
          static int notify (void   * metadata,
                             void   * payload,
                             size_t   bytes,
                             void   * recv_func_parm,
                             void   * cookie)
          {
            TRACE_FN_ENTER();
            notify_t * n = (notify_t *) metadata;

            TRACE_FORMAT("recv_func_parm = %p, n->fn = %p, n->cookie = %p .. &(n->fn) = %p, &(n->cookie) = %p", recv_func_parm, n->fn, n->cookie, &(n->fn), &(n->cookie));

            n->fn (recv_func_parm, // a.k.a. "pami_context_t"
                   n->cookie,
                   PAMI_SUCCESS);

            TRACE_FN_EXIT();
            return 0;
          };


          typedef struct
          {
            Interface::RecvFunction_t   f;
            void                      * p;
          } mu_dispatch_t;

        public:

          ///
          /// \brief Reception channel constructor
          ///
          inline RecChannel (size_t id = 0) :
              self (),
              _channel_id (id),
              _channel_cookie (NULL),
              _rfifo (NULL),
              _unexpected_packet_queue (dispatch_set_count * dispatch_set_size, MemoryFifoPacketHeader::packet_singlepacket_metadata_size)
          {
            TRACE_FN_ENTER();
            COMPILE_TIME_ASSERT(sizeof(notify_t) <= MemoryFifoPacketHeader::packet_singlepacket_metadata_size);

            // ----------------------------------------------------------------
            // Set the common base descriptor fields
            // ----------------------------------------------------------------
            MUSPI_BaseDescriptorInfoFields_t base;
            memset((void *)&base, 0, sizeof(base));

            base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
            base.Payload_Address = 0;
            base.Message_Length  = 0;
            base.Torus_FIFO_Map  = 0;
            base.Dest.Destination.Destination = 0;

            self.setBaseFields (&base);


            // ----------------------------------------------------------------
            // Set the common point-to-point descriptor fields
            // ----------------------------------------------------------------
            MUSPI_Pt2PtDescriptorInfoFields_t pt2pt;
            memset((void *)&pt2pt, 0, sizeof(pt2pt));

            pt2pt.Hints_ABCD = 0;
            pt2pt.Skip       = 136; // Don't link checksum entire notify packet
            pt2pt.Misc1 =
              MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
              MUHWI_PACKET_DO_NOT_DEPOSIT |
              MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE;
            pt2pt.Misc2 =
              MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;

            self.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
            self.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;   // huh?
            self.setPt2PtFields (&pt2pt);


            // ----------------------------------------------------------------
            // Set the common memory fifo descriptor fields
            // ----------------------------------------------------------------
            MUSPI_MemoryFIFODescriptorInfoFields_t memfifo;
            memset ((void *)&memfifo, 0, sizeof(memfifo));

            memfifo.Rec_FIFO_Id    = 0;
            memfifo.Rec_Put_Offset = 0;
            memfifo.Interrupt      = MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL;
            memfifo.SoftwareBit    = 0;

            self.setMemoryFIFOFields (&memfifo);
            self.setMessageUnitPacketType (MUHWI_PACKET_TYPE_FIFO);

            // Set the payload information.
            self.setPayload (0, 0);

            // ----------------------------------------------------------------
            // Set the "notify" system dispatch identifier
            // ----------------------------------------------------------------
            TRACE_HEXDATA(&self, 64);
            MemoryFifoPacketHeader * hdr =
              (MemoryFifoPacketHeader *) & self.PacketHeader;
            hdr->setSinglePacket (true);
            hdr->setDispatchId (dispatch_system_notify);
            TRACE_HEXDATA(&self, 64);

#ifdef ENABLE_MAMBO_WORKAROUNDS

            if (__global.personality._is_mambo) /// \todo mambo hack
              {
                // mambo does not process zero byte memfifo packets.
                TRACE_STRING("mambo workaround!");
                Kernel_MemoryRegion_t  mregion;
                Kernel_CreateMemoryRegion ( &mregion, this, 1 );
                self.setPayload ((uint64_t)mregion.BasePa, 1);
              }

#endif

            TRACE_STRING("construct 'notify self' descriptor");
            TRACE_HEXDATA(&self, 64);

            TRACE_FN_EXIT();
          };

          ///
          /// \brief Reception channel destructor
          ///
          inline ~RecChannel () {};

          ///
          /// \brief Initialize the reception channel
          ///
          /// The reception channel class does not allocate memory for the MU
          /// reception fifo and other resources. An external entity allocates
          /// these resources and then provides them to the reception channel
          /// object with this initialization method.
          ///
          /// \param[in] f                   An initialized reception fifo
          /// \param[in] channel_cookie      Cookie delivered to all completion
          ///                                events invoked by this channel,
          ///                                a.k.a., "pami_context_t"
          ///
          inline void initialize (uint16_t              rfifo_id,
                                  MUSPI_RecFifo_t     * rfifo,
                                  MUHWI_Destination_t * dest,
                                  void                * channel_cookie)
          {
            TRACE_FN_ENTER();

            TRACE_FORMAT("rfifo_id = %d, rfifo = %p, dest = 0x%08x, channel_cookie = %p", rfifo_id, rfifo, *((uint32_t *)dest), channel_cookie);
            TRACE_FORMAT("startva=%p, startpa=0x%lx, size-1 = %lx\n", rfifo->_fifo.va_start, rfifo->_fifo.pa_start, rfifo->_fifo.size_1);
            _rfifo_id = rfifo_id;
            _rfifo    = rfifo;

            // Initialize the dispatch table
            size_t i, n = dispatch_set_count * dispatch_set_size;

            for (i = 0; i < n; i++)
              {
                _dispatch[i].f = UnexpectedPacketQueue<uint16_t>::dispatch_fn;
                _dispatch[i].p = _unexpected_packet_queue.getCookie(i);
              }

            // Initialize any mu "system" dispatch functions
            _dispatch[dispatch_system_notify].f = notify;
            _dispatch[dispatch_system_notify].p = channel_cookie; // pami_context_t;

            // ----------------------------------------------------------------
            // Initialize the memory fifo descriptor to route to "self"
            // ----------------------------------------------------------------
            self.setRecFIFOId (rfifo_id);
            self.setDestination (*dest);

            TRACE_STRING("initialize 'notify self' descriptor");
            TRACE_HEXDATA(&self, 64);

            TRACE_FN_EXIT();
          };

          inline void initializeNotifySelfDescriptor (MUSPI_DescriptorBase & desc,
                                                      pami_event_function    fn,
                                                      void                 * cookie)
          {
            TRACE_FN_ENTER();

            TRACE_FORMAT("self = %p, desc = %p, fn = %p, cookie = %p", (void *)&self, (void *)&desc, (void *)fn, cookie);

            // Clone the "self" descriptor
            self.clone (desc);

            TRACE_HEXDATA(&desc, 64);

            // Copy the completion function+cookie into the packet header.
            MemoryFifoPacketHeader * hdr =
              (MemoryFifoPacketHeader *) & desc.PacketHeader;

            notify_t notify = {fn, cookie};
            hdr->setMetaData((void *)&notify, sizeof(notify_t));

            TRACE_HEXDATA(&desc, 64);

            TRACE_FN_EXIT();
          };


          ///
          /// \brief Register a packet handler function for a dispatch set
          ///
          /// The function and cookie are invoked ...
          ///
          /// The registration may fail if the requested dispatch set number is
          /// greater than the number of allocated dispatch sets, or if there
          /// are no free dispatch identifiers in the requested dispatch set
          /// due to previous registrations on the requested dispatch set.
          ///
          /// \see MU::RecChannel::dispatch_set_count
          /// \see MU::RecChannel::dispatch_set_size
          ///
          /// \param[in]  set    Dispatch set number
          /// \param[in]  fn     Dispatch function
          /// \param[in]  cookie Dispatch cookie
          /// \param[out] id     Assigned MU dispatch identifier
          ///
          /// \retval true  Successful packet handler registration
          /// \retval false Unsuccessful packet handler registration
          ///
          inline bool registerPacketHandler (size_t                      set,
                                             Interface::RecvFunction_t   fn,
                                             void                      * cookie,
                                             uint16_t                  & id)
          {
            TRACE_FN_ENTER();

            if (set >= dispatch_set_count) return false;

            uint16_t i;

            for (i = 0; i < dispatch_set_size; i++)
              {
                id = set * dispatch_set_size + i;

                if (_dispatch[id].f == UnexpectedPacketQueue<uint16_t>::dispatch_fn)
                  {
                    _dispatch[id].f = fn;
                    _dispatch[id].p = cookie;

                    // Deliver any unexpected packets for registered dispatch ids. Stop at
                    // the first unexpected packet for an un-registered dispatch id.
                    UnexpectedPacketQueue<uint16_t>::UnexpectedPacket * uepkt = NULL;

                    while ((uepkt = (UnexpectedPacketQueue<uint16_t>::UnexpectedPacket *) _unexpected_packet_queue.peek()) != NULL)
                      {
                        if (_dispatch[uepkt->id].f != UnexpectedPacketQueue<uint16_t>::dispatch_fn)
                          {
                            // Invoke the registered dispatch function
                            _dispatch[uepkt->id].f (uepkt->metadata,
                                                    uepkt->payload,
                                                    uepkt->bytes,
                                                    _dispatch[uepkt->id].p,
                                                    uepkt->payload);

                            // Remove the unexpected packet from the queue and free
                            _unexpected_packet_queue.dequeue();
                            free (uepkt);
                          }
                        else
                          {
                            // Stop unexpected queue processing.  This maintains packet order
                            // which is required for protocols such as eager.
                            break;
                          }
                      }



                    TRACE_FORMAT("registration success. set = %zu, fn = %p, cookie = %p --> id = %d", set, fn, cookie, id);
                    TRACE_FN_EXIT();
                    return true;
                  }
              }

            PAMI_abortf("Unable to find a free dispatch slot. Was this dispatch set (%zu) previously registered?", set);

            TRACE_FORMAT("registration failure. set = %zu, fn = %p, cookie = %p", set, fn, cookie);
            TRACE_FN_EXIT();
            return false;
          };

          size_t process_wrap() __attribute__((noinline, weak));

          inline size_t advance ()
          {
            TRACE_FN_ENTER();

            size_t packets = 0;
            MUSPI_RecFifo_t * rfifo = _rfifo;

            void        * va_tail;           /* Snapshot of the fifo's tail */
            void        * va_end;            /* Snapshot of the fifo's end */
            void        * va_head;           /* Snapshot of the fifo's head */
            volatile uint64_t  pa_tail;      /* Actual tail physical address */

            MUSPI_Fifo_t  *fifo_ptr = &rfifo->_fifo;
            va_head      = fifo_ptr->va_head;
            va_end       = fifo_ptr->va_end;

            pa_tail      = MUSPI_getHwTail (fifo_ptr);    /* Snapshot HW */
            va_tail      = (void*)(pa_tail - MUSPI_getStartPa (fifo_ptr) +
                                   (uint64_t)(MUSPI_getStartVa (fifo_ptr)));

            if (va_tail == va_head)
              return packets;

            uint32_t total_bytes = 0;
            uint32_t cumulative_bytes = 0;
            MemoryFifoPacketHeader *hdr = NULL;

            MUSPI_setTailVa (fifo_ptr, va_tail); /** extra store over older poll loop*/

            /** touch head for first packet */
            muspi_dcbt (va_head, 0);
            _bgq_msync();

            if (va_tail > va_head) /** one branch less over older poll loop */
              total_bytes = (uint64_t)va_tail - (uint64_t)va_head; /** No wrap and we can grab packets */
            else if ( va_head < (void*)((uint64_t)va_end - 544UL) )
              /** We wont process the wrap in this packet poll */
              total_bytes = (uint64_t)va_end - (uint64_t)va_head - 544UL;
            else
              {
                packets = process_wrap();
                va_head = fifo_ptr->va_head;
              }

            while (cumulative_bytes < total_bytes )
              {
                MUHWI_PacketHeader_t *phdr = (MUHWI_PacketHeader_t *) va_head;
                uint32_t pbytes   = (phdr->NetworkHeader.pt2pt.Byte8.Size + 1) << 5;
                va_head = (void *)((char *)va_head + pbytes);
		muspi_dcbt (va_head, 0);

                hdr = (MemoryFifoPacketHeader *) phdr;
                TRACE_HEXDATA(hdr, 64);

                cumulative_bytes += pbytes;
                uint16_t id = 0;
                void *metadata;
                hdr->getHeaderInfo (id, &metadata);
                TRACE_FORMAT("dispatch = %d, metadata = %p, hdr+1 = %p, cur_bytes-32 = %d", id, metadata, hdr + 1, pbytes - 32);
                _dispatch[id].f(metadata, hdr + 1, pbytes - 32, _dispatch[id].p, hdr + 1);
                packets++;
                //Touch head for next packet
                TRACE_FORMAT("Received packet of size %d, cumulative bytes = %d, total bytes %d", pbytes, cumulative_bytes, total_bytes);
              }

            fifo_ptr->va_head = va_head;
            //MUSPI_syncRecFifoHwHead (rfifo);
            MUSPI_setHwHead (fifo_ptr, (uint64_t)va_head - (uint64_t)(fifo_ptr->va_start));

            //TRACE_FORMAT("Number of packets received: %d", packets);
            TRACE_FN_EXIT();
            return packets;
          }

          // Memory fifo descriptor initialized to send to the reception
          // fifo in this reception channel
          MUSPI_Pt2PtMemoryFIFODescriptor self;

        private:

          size_t            _channel_id;     // a.k.a., MU::Context "fifo number"
          void            * _channel_cookie; // a.k.a., "pami_context_t"
          MUSPI_RecFifo_t * _rfifo;          // The actual reception fifo
          uint16_t          _rfifo_id;       // MUSPI reception fifo id

          mu_dispatch_t     _dispatch[dispatch_set_count * dispatch_set_size];

          UnexpectedPacketQueue<uint16_t> _unexpected_packet_queue;

      }; // class     PAMI::Device::MU::RecChannel

      size_t RecChannel::process_wrap ()
      {
        size_t packets = 0;
        MemoryFifoPacketHeader *hdr = NULL;
        uint32_t cur_bytes = 0;
        uint64_t cumulative_bytes = 0;
        uint64_t total_bytes = (uint64_t)_rfifo->_fifo.va_end - (uint64_t)_rfifo->_fifo.va_head;

        while (cumulative_bytes < total_bytes )
          {
            hdr = (MemoryFifoPacketHeader *) MUSPI_getNextPacketWrap (_rfifo, &cur_bytes);
            TRACE_HEXDATA(hdr, 64);

            uint16_t id = 0;
            void *metadata;
            cumulative_bytes += cur_bytes;
            hdr->getHeaderInfo (id, &metadata);

            _dispatch[id].f(metadata, hdr + 1, cur_bytes - 32, _dispatch[id].p, hdr + 1);
            packets++;
          }

        TRACE_FORMAT("Received packet wrap of size %d\n", cur_bytes);
        return packets;
      }

    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_RecChannel_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
