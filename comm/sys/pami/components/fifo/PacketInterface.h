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
 * \file components/fifo/PacketInterface.h
 * \brief ???
 */

#ifndef __components_fifo_PacketInterface_h__
#define __components_fifo_PacketInterface_h__

namespace PAMI
{
  namespace Fifo
  {
    namespace Interface
    {
      ///
      /// \brief Packet interface class
      ///
      /// \tparam T_Packet Packet implementation class
      ///
      template <class T_Packet>
      class Packet
      {
        public:

          ///
          /// \brief Number of available bytes in each packet header for application data
          ///
          /// \attention All packet interface implementation classes \b must
          ///            contain a public static const data member named
          ///            'size_t header_size'.
          ///
          /// C++ code using templates to specify the packet may safely access the
          /// 'header_size' static constant.
          ///
          static size_t getHeaderSize ();

          ///
          /// \brief Number of available bytes in each packet payload for application data
          ///
          /// \attention All packet interface implementation classes \b must
          ///            contain a public static const data member named
          ///            'size_t payload_size'.
          ///
          /// C++ code using templates to specify the packet may safely access the
          /// 'payload_size' static constant.
          ///
          static size_t getPayloadSize ();

          Packet () {};
          ~Packet () {};

          ///
          /// \brief Clear all header and payload data in a packet
          ///
          inline void clear ();

          ///
          /// \brief Retrieve a pointer to the packet header location.
          ///
          inline void * getHeader ();

          ///
          /// \brief Copy a packet header into a destination buffer.
          ///
          /// Packet::header_size bytes will be copied into the destination
          /// buffer.
          ///
          /// \param [in] dst Destination buffer for the header data
          ///
          inline void copyHeader (void * dst);

          ///
          /// \brief Write a packet header from a source buffer.
          ///
          /// Packet::header_size bytes will be copied from the source buffer.
          ///
          /// \param [in] src Source buffer for the header data
          ///
          inline void writeHeader (void * src);

          ///
          /// \brief Retrieve a pointer to the packet payload location.
          ///
          inline void * getPayload ();

          ///
          /// \brief Copy a packet payload into a destination buffer.
          ///
          /// Packet::payload_size bytes will be copied into the destination
          /// buffer.
          ///
          /// \param [in] dst Destination buffer for the payload data
          ///
          inline void copyPayload (void * dst);
      };

      template <class T_Packet>
      size_t Packet<T_Packet>::getHeaderSize ()
      {
        return T_Packet::header_size;
      }

      template <class T_Packet>
      size_t Packet<T_Packet>::getPayloadSize ()
      {
        return T_Packet::payload_size;
      }

      template <class T_Packet>
      void Packet<T_Packet>::clear ()
      {
        return static_cast<T_Packet*>(this)->clear_impl ();
      }

      template <class T_Packet>
      void * Packet<T_Packet>::getHeader ()
      {
        return static_cast<T_Packet*>(this)->getHeader_impl ();
      }

      template <class T_Packet>
      void Packet<T_Packet>::copyHeader (void * dst)
      {
        static_cast<T_Packet*>(this)->copyHeader_impl (dst);
      }

      template <class T_Packet>
      void Packet<T_Packet>::writeHeader (void * src)
      {
        static_cast<T_Packet*>(this)->writeHeader_impl (src);
      }

      template <class T_Packet>
      void * Packet<T_Packet>::getPayload ()
      {
        return static_cast<T_Packet*>(this)->getPayload_impl ();
      }

      template <class T_Packet>
      void Packet<T_Packet>::copyPayload (void * dst)
      {
        static_cast<T_Packet*>(this)->copyPayload_impl (dst);
      }
    };
  };
};
#endif // __components_fifo_PacketInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
