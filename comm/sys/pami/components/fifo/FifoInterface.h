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
 * \file components/fifo/FifoInterface.h
 * \brief ???
 */

#ifndef __components_fifo_FifoInterface_h__
#define __components_fifo_FifoInterface_h__

namespace PAMI
{
  namespace Fifo
  {
    namespace Interface
    {
      template <class T_Producer>
      class PacketProducer
      {
        protected:

          inline PacketProducer () {};
          inline ~PacketProducer () {};

        public:

          ///
          /// \brief Intialize a packet with header and payload data
          ///
          template <class T_Packet>
          inline bool produce (T_Packet & packet)
          {
            return static_cast<T_Producer*>(this)->produce_impl (packet);
          };
      };

      template <class T_Consumer>
      class PacketConsumer
      {
        protected:

          inline PacketConsumer () {};
          inline ~PacketConsumer () {};

        public:

          ///
          /// \brief Read packet header and payload data
          ///
          template <class T_Packet>
          inline bool consume (T_Packet & packet)
          {
            return static_cast<T_Consumer*>(this)->consume_impl (packet);
          };
      };

      ///
      /// \brief Fifo interface class
      ///
      /// \tparam T_Fifo Fifo implementation class
      ///
      template <class T_Fifo>
      class Fifo
      {
        protected:

          Fifo () {};

          ~Fifo () {};

        public:

          ///
          /// \brief Number of bytes required from the memory manager
          ///
          /// The initialize() method will fail if not enough memory is provided.
          ///
          /// \attention All fifo interface implementation classes \b must
          ///            contain a public static const data member named
          ///            'size_t fifo_memory_size'.
          ///
          /// C++ code using templates to specify the fifo may safely access the
          /// 'fifo_memory_size' static constant.
          ///
          static size_t getFifoMemorySize ();

          ///
          /// \brief Number of available bytes in each packet header for application data
          ///
          /// \attention All fifo interface implementation classes \b must
          ///            contain a public static const data member named
          ///            'size_t packet_header_size'.
          ///
          /// C++ code using templates to specify the fifo may safely access the
          /// 'packet_header_size' static constant.
          ///
          static size_t getPacketHeaderSize ();

          ///
          /// \brief Number of available bytes in each packet payload for application data
          ///
          /// \attention All fifo interface implementation classes \b must
          ///            contain a public static const data member named
          ///            'size_t packet_payload_size'.
          ///
          /// C++ code using templates to specify the fifo may safely access the
          /// 'packet_payload_size' static constant.
          ///
          static size_t getPacketPayloadSize ();

          ///
          /// \brief Initialize a fifo using a memory manager and unique key.
          ///
          /// The memory manager may be used by a fifo implementation to
          /// construct the actual fifo packet array in a known location and
          /// must provide at least \c fifo_memory_size bytes.
          ///
          /// After successful initialization the fifo is available for use.
          ///
          /// \see getFifoMemorySize()
          ///
          /// \param [in] mm     Memory manager to allocate internal fifo objects
          /// \param [in] key    Unique key that identifies the fifo
          /// \param [in] npeers Number of fifo being constructed
          /// \param [in] pid    Peer id of this fifo in the fifo set
          ///
          /// \retval true Successful initialization
          /// \retval false Initialization failed; perhaps due to memory constraints
          ///
          template <class T_MemoryManager>
          inline bool initialize (T_MemoryManager * mm,
                                  char            * key,
                                  size_t            npeers,
                                  size_t            pid);

          ///
          /// \brief Initialize a fifo by cloning a previously initialized fifo.
          ///
          /// \param [in] A previously initialized fifo object
          ///
          inline void initialize (T_Fifo & fifo);

          ///
          /// \brief Retrieve the identifier of the last packet that was produced by this fifo object.
          ///
          /// The identifier is a monotonically increasing integer.
          ///
          /// \note The packet indentifier does not represent the last packet
          ///       produced globally, such as when there are multiple fifo
          ///       objects used by multiple producers.
          ///
          /// \retval -1  No packets have been produced by this fifo object
          /// \retval >=0 Identifier of the last packet produced by this fifo object
          ///
          inline ssize_t lastPacketProduced ();

          ///
          /// \brief Retrieve the identifier of the last packet that was consumed in this fifo.
          ///
          /// The identifier is a monotonically increasing integer.
          ///
          /// \note The packet indentifier represents the last packet that was
          ///       consumed globally, typically as when there are multiple fifo
          ///       objects but only one consumer.
          ///
          /// \retval -1  No packets have been consumed in this fifo
          /// \return >=0 Identifier of the last packet consumed in this fifo
          ///
          inline ssize_t lastPacketConsumed ();

          ///
          /// \brief Produce a packet into the fifo.
          ///
          /// If the fifo is not full or otherwise unavailable the functor object
          /// produce method is invoked to write the packet into the fifo.
          ///
          /// \see PAMI::Fifo::Interface::PacketProducer::produce()
          /// \see PAMI::Fifo::Interface::Packet
          ///
          /// \tparam T_Functor Packet producer functor implementation class
          ///
          /// \param [in] produce Packet producer functor implementation object
          ///
          /// \retval true  A packet was produced
          /// \retval false A packet was not produced
          ///
          template <class T_Functor>
          inline bool producePacket (T_Functor & produce);

          ///
          /// \brief Consume a packet from the fifo.
          ///
          /// If the fifo is not empty the functor object consume method is
          /// invoked to read the next packet from the fifo.
          ///
          /// \see PAMI::Fifo::Interface::PacketConsume::consume()
          /// \see PAMI::Fifo::Interface::Packet
          ///
          /// \tparam T_Functor Packet consumer functor implementation class
          ///
          /// \param [in] consume Packet consumer functor implementation object
          ///
          /// \retval true  A packet was produced
          /// \retval false A packet was not produced
          ///
          template <class T_Functor>
          inline bool consumePacket (T_Functor & consume);
      };

      template <class T_Fifo>
      size_t Fifo<T_Fifo>::getFifoMemorySize ()
      {
        return T_Fifo::fifo_memory_size;
      }

      template <class T_Fifo>
      size_t Fifo<T_Fifo>::getPacketHeaderSize ()
      {
        return T_Fifo::packet_header_size;
      }

      template <class T_Fifo>
      size_t Fifo<T_Fifo>::getPacketPayloadSize ()
      {
        return T_Fifo::packet_payload_size;
      }

      template <class T_Fifo>
      template <class T_MemoryManager>
      bool Fifo<T_Fifo>::initialize (T_MemoryManager * mm,
                                     char            * key,
                                     size_t            npeers,
                                     size_t            pid)
      {
        return static_cast<T_Fifo*>(this)->initialize_impl (mm, key, npeers, pid);
      }

      template <class T_Fifo>
      void Fifo<T_Fifo>::initialize (T_Fifo & fifo)
      {
        static_cast<T_Fifo*>(this)->initialize_impl (fifo);
      }

      template <class T_Fifo>
      ssize_t Fifo<T_Fifo>::lastPacketProduced ()
      {
        return static_cast<T_Fifo*>(this)->lastPacketProduced_impl ();
      }

      template <class T_Fifo>
      ssize_t Fifo<T_Fifo>::lastPacketConsumed ()
      {
        return static_cast<T_Fifo*>(this)->lastPacketConsumed_impl ();
      }

      template <class T_Fifo>
      template <class T_Functor>
      bool Fifo<T_Fifo>::producePacket (T_Functor & produce)
      {
        return static_cast<T_Fifo*>(this)->producePacket_impl (produce);
      }

      template <class T_Fifo>
      template <class T_Functor>
      bool Fifo<T_Fifo>::consumePacket (T_Functor & consume)
      {
        return static_cast<T_Fifo*>(this)->consumePacket_impl (consume);
      }
    };
  };
};
#endif // __components_fifo_FifoInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
