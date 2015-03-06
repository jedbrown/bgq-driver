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
 * \file components/fifo/wrap/WrapFifo.h
 * \brief ???
 */

#ifndef __components_fifo_wrap_WrapFifo_h__
#define __components_fifo_wrap_WrapFifo_h__

#include <string.h>

#include "Arch.h"
#include "Memory.h"

#include "components/memory/MemoryManager.h"
#include "components/atomic/CounterInterface.h"

#include "components/fifo/FifoInterface.h"
#include "components/fifo/PacketInterface.h"

#include "components/devices/shmem/wakeup/WakeupNoop.h"

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

//#define DEBUG_MEMORY


namespace PAMI
{
  namespace Fifo
  {
    template < class T_Packet, class T_Atomic, unsigned T_Size = 128, class T_Wakeup = Wakeup::Noop >
    class WrapFifo : public PAMI::Fifo::Interface::Fifo <PAMI::Fifo::WrapFifo <T_Packet, T_Atomic, T_Size, T_Wakeup> >
    {
      public:

        typedef T_Packet Packet;

        friend class Interface::Fifo <WrapFifo <T_Packet, T_Atomic, T_Size, T_Wakeup> >;

        static const size_t mask = T_Size - 1;
        static const size_t packet_header_size = T_Packet::header_size;
        static const size_t packet_payload_size = T_Packet::payload_size;
        static const size_t fifo_memory_size =
          sizeof(T_Packet) * T_Size + sizeof(size_t) * 2;

        inline WrapFifo () :
            Interface::Fifo <WrapFifo <T_Packet, T_Atomic, T_Size, T_Wakeup> > (),
            _packet (NULL),
            _head (NULL),
            _active (),
            _bounded_counter (),
            _last_packet_produced (-1)
        {
          // Do a compile-time-assert that the fifo length is a power of two
          COMPILE_TIME_ASSERT(!(T_Size & (T_Size - 1)));
        };

        inline ~WrapFifo () {};

      protected:

        // ---------------------------------------------------------------------
        // PAMI::Fifo::Fifo interface implementation
        // ---------------------------------------------------------------------

        template <class T_MemoryManager>
        inline bool initialize_impl (T_MemoryManager * mm,
                                     char            * key,
                                     size_t            npeers,
                                     size_t            pid)
        {
          TRACE_ERR((stderr, ">> WrapFifo::initialize_impl(%p, \"%s\")\n", mm, key));

          if (mm->available() < WrapFifo::fifo_memory_size)
            {
              fprintf (stderr, "Not enough memory. required = %zu available = %zu\n", WrapFifo::fifo_memory_size, mm->available());
              return false;
            }

          // Initialize the tail atomic counter using the base key.
          char atomic_key[PAMI::Memory::MMKEYSIZE];
          snprintf (atomic_key, PAMI::Memory::MMKEYSIZE - 1, "%s-counter", key);
          _bounded_counter.init (mm, atomic_key);

          char wakeup_key[PAMI::Memory::MMKEYSIZE];
          snprintf (wakeup_key, PAMI::Memory::MMKEYSIZE - 1, "%s-wakeup", key);
          _wakeup = T_Wakeup::generate(npeers, wakeup_key);

          char active_key[PAMI::Memory::MMKEYSIZE];
          snprintf (active_key, PAMI::Memory::MMKEYSIZE - 1, "%s-active", key);
          _active.init (mm, active_key, T_Size, &_wakeup[pid]);

          // Allocate an array of packets with the provided memory manager
          // and key. The WrapFifo::packet_initialize() function is only
          // invoked by the first process to allocate from the memory manager
          // with this key. All other processes do not return from the
          // memalign until the first process has completed the memory
          // initialization.
          //
          // Allocation is for N packets, 1 head counter,
          // and 1 wrap counter.

          mm->memalign ((void **)&_packet,
                        sizeof(T_Packet),
                        WrapFifo::fifo_memory_size,
                        key,
                        WrapFifo::packet_initialize,
                        (void *)this);

          TRACE_ERR((stderr, "   WrapFifo::initialize_impl() after sync memalign\n"));

          _head = (ssize_t *) & _packet[T_Size];
          *(_head) = 0;

          TRACE_ERR((stderr, "<< WrapFifo::initialize_impl(%p, \"%s\"), _head = %p, *_head = %zd\n", mm, key, _head, *_head));
          return true;
        };

        inline void initialize_impl (WrapFifo & fifo)
        {
          TRACE_ERR((stderr, ">> WrapFifo::initialize_impl(WrapFifo &)\n"));
          _bounded_counter.clone (fifo._bounded_counter);


          _packet = fifo._packet;
          _active = fifo._active;
          _head = fifo._head;
          _last_packet_produced = fifo._last_packet_produced;

          _wakeup = fifo._wakeup;
          _active.init (fifo._active);


          TRACE_ERR((stderr, "<< WrapFifo::initialize_impl(WrapFifo &), _packet = %p, _head = %p, *_head = %zd, _last_packet_produced = %zu\n", _packet, _head, *_head, _last_packet_produced));
        };

        inline void dumpPacket (size_t index)
        {
          char tmp[10240];
          char * str = tmp;
          size_t i = 0;

          uint32_t * hdr = (uint32_t *) _packet[index].getHeader();
          str += sprintf(str, "WrapFifo::dumpPacket.header  [%p,%4zu] ", hdr, packet_header_size_impl);
          size_t bytes = 0;

          while (bytes < packet_header_size_impl)
            {
              str += sprintf(str, "%08x ", hdr[i++]);
              bytes += sizeof(uint32_t);

              if (i % 4 == 0)
                str += sprintf(str, "\nWrapFifo::dumpPacket.header  [%p]      ", &hdr[i]);
            }

          fprintf(stderr, "%s\n", tmp);

          str = tmp;
          uint32_t * payload = (uint32_t *) _packet[index].getPayload();
          str += sprintf(str, "WrapFifo::dumpPacket.payload [%p,%4zu] ", payload, packet_payload_size_impl);
          bytes = 0;
          i = 0;

          while (bytes < packet_payload_size_impl)
            {
              str += sprintf(str, "%08x ", payload[i++]);
              bytes += sizeof(uint32_t);

              if (i % 4 == 0)
                str += sprintf(str, "\nWrapFifo::dumpPacket.payload [%p]      ", &payload[i]);
            }

          fprintf(stderr, "%s\n", tmp);
        };

        static const size_t packet_header_size_impl = T_Packet::header_size;

        static const size_t packet_payload_size_impl = T_Packet::payload_size;

        inline ssize_t lastPacketProduced_impl ()
        {
          return _last_packet_produced;
        };

        inline ssize_t lastPacketConsumed_impl ()
        {
          return *_head - 1;
        };

        ///
        /// \param [in] packet Functor object that implements the PacketProducer interface
        ///
        /// \see PAMI::Fifo::Interface::PacketProducer
        ///
        template <class T_Producer>
        inline bool producePacket_impl (T_Producer & packet)
        {
          TRACE_ERR((stderr, ">> WrapFifo::producePacket_impl(T_Producer &)\n"));

          size_t tail = _bounded_counter.fetch_and_inc_bounded ();

          if (likely (tail != T_Atomic::bound_error))
            {
              TRACE_ERR((stderr, "   WrapFifo::producePacket_impl(T_Producer &), tail = %zu\n", tail));
              const size_t index = tail & WrapFifo::mask;

              //dumpPacket(index);
#ifdef DEBUG_MEMORY
              _packet[index].clear();
#endif
              packet.produce (_packet[index]);
#ifdef DEBUG_MEMORY
              fprintf (stderr, "%s:%d\n", __func__, __LINE__);
              dumpPacket(index);
#endif

              if (Memory::supports<Memory::remote_msync>() &&
                  Memory::supports<Memory::l1p_flush>())
                {
                  // This "L1P flush" forces all previous memory operations
                  // (header writes, payload writes, etc) to flush to the L2
                  // before the packet is marked 'active'.  As soon as the
                  // consumer process sees that the 'active' attribute is set it
                  // will start to read the packet header and payload data.
                  Memory::sync<Memory::l1p_flush>();
                }
              else
                {
                  Memory::sync();
                }

              _active[index] = 1;

              _last_packet_produced = tail;

              TRACE_ERR((stderr, "<< WrapFifo::producePacket_impl(T_Producer &), return true..tail:%lld\n", (unsigned long long)tail));
              return true;
            }

          TRACE_ERR((stderr, "<< WrapFifo::producePacket_impl(T_Producer &), return false\n"));
          return false;
        };

        ///
        /// \param [in] packet Functor object that implements the PacketConsumer interface
        ///
        /// \see PAMI::Fifo::Interface::PacketConsumer
        ///
        template <class T_Consumer>
        inline bool consumePacket_impl (T_Consumer & packet)
        {
          TRACE_ERR((stderr, ">> WrapFifo::consumePacket_impl(T_Consumer &)\n"));

          const size_t head = (size_t)(*(this->_head));
          size_t index = head & WrapFifo::mask;

          if (_active[index])
            {
              // Immediately clear the 'active' status to flush this write with
              // the packet data memory synchronization.
              _active[index] = 0;

              if (Memory::supports<Memory::remote_msync>())
                {
                  // This memory sync forces all previous memory operations on
                  // the node to complete, including:
                  // - the pending local update to the 'active' status, and
                  // - the writes to the packet data by the producer
                  Memory::sync<Memory::remote_msync>();
                }
              else
                {
                  // This memory sync forces all previous local memory writes
                  // to complete, including the pending update to the 'active'
                  // status.
                  //
                  // Without this write synchronization a race condition is
                  // present where it is possible that the atomic counter
                  // increment could be visible to other cores before the clear
                  // of the 'active' status.
                  Memory::sync();
                }

              TRACE_ERR((stderr, "   WrapFifo::consumePacket_impl(T_Consumer &), head = %zu, index = %zu (WrapFifo::mask = %p)\n", head, index, (void *)WrapFifo::mask));
#ifdef DEBUG_MEMORY
              fprintf (stderr, "%s:%d\n", __func__, __LINE__);
              dumpPacket(head);
#endif
              packet.consume (_packet[index]);
              //dumpPacket(head);

              *(this->_head) = (ssize_t)(head + 1);

              // Increment the upper bound everytime a packet is consumed..ok
              // to be incremented in chunks
              _bounded_counter.bound_upper_store_and_add();

              TRACE_ERR((stderr, "<< WrapFifo::consumePacket_impl(T_Consumer &), return true\n"));
              return true;
            }

          TRACE_ERR((stderr, "<< WrapFifo::consumePacket_impl(T_Consumer &), return false\n"));
          return false;
        };

      private:

        ///
        /// \brief Initialize the packet resources
        ///
        /// \see PAMI::Memory::MM_INIT_FN
        ///
        static void packet_initialize (void       * memory,
                                       size_t       bytes,
                                       const char * key,
                                       unsigned     attributes,
                                       void       * cookie)
        {
          TRACE_ERR((stderr, ">> WrapFifo::packet_initialize(%p, %zu, \"%s\", %d, %p)\n", memory, bytes, key, attributes, cookie));
          WrapFifo * fifo = (WrapFifo *) cookie;
          T_Packet * packet = (T_Packet *) memory;
          size_t * active = (size_t *) & packet[T_Size];

          fifo->_bounded_counter.bound_lower_clear();
          fifo->_bounded_counter.bound_upper_clear();
          size_t i;

          for (i = 0; i < T_Size; i++)
            {
              new (&packet[i]) T_Packet();
              active[i] = 0;
              fifo->_bounded_counter.bound_upper_fetch_and_inc();
            }

          TRACE_ERR((stderr, "<< WrapFifo::packet_initialize(%p, %zu, \"%s\", %d, %p)\n", memory, bytes, key, attributes, cookie));
        }

        // -----------------------------------------------------------------
        // Located in shared memory
        // -----------------------------------------------------------------
        T_Packet         * _packet;
        volatile ssize_t * _head;

        // -----------------------------------------------------------------
        // Located in-place
        // -----------------------------------------------------------------
        T_Wakeup                                           * _wakeup;
        typename T_Wakeup::template Region<volatile uint8_t> _active;
        T_Atomic                                             _bounded_counter;
        ssize_t                                              _last_packet_produced;
    };
  };
};
#undef TRACE_ERR
#endif // __util_fifo_wrapfifo_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
