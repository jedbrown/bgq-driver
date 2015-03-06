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
 * \file components/devices/shmem/ShmemPacket.h
 * \brief ???
 */

#ifndef __components_devices_shmem_ShmemPacket_h__
#define __components_devices_shmem_ShmemPacket_h__

#include <sys/uio.h>

#include <pami.h>

#include "components/fifo/FifoInterface.h"

#include "util/common.h"

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

      template <class T_Packet>
      class Packet : public T_Packet
      {
        public:

          inline  Packet () {};

          inline ~Packet () {};

          static void setDispatch (T_Packet & packet, uint16_t dispatch)
          {
            // Packet header must be large enough to store the dispatch id.
            COMPILE_TIME_ASSERT(sizeof(uint16_t) <= T_Packet::header_size);

            uint16_t * hdr = (uint16_t *) packet.getHeader ();
            hdr[(T_Packet::header_size>>1)-1] = dispatch;
          };

          static uint16_t getDispatch (T_Packet & packet)
          {
            uint16_t * hdr = (uint16_t *) packet.getHeader();
            return hdr[(T_Packet::header_size>>1)-1];
          };

          static void * getMetadata (T_Packet & packet)
          {
            return (void *) packet.getHeader();
          };

          ///
          /// \brief Write data into the packet header
          ///
          /// \see PAMI::Fifo::Packet::getHeader()
          ///
          /// \param [in] addr  Source header data address
          /// \param [in] bytes Source header data length
          ///
          static void writeHeader (T_Packet & packet, uint8_t * addr, size_t bytes)
          {
            unsigned i;
            uint8_t * hdr = (uint8_t *) packet.getHeader ();

            for (i = 0; i < bytes; i++) hdr[i] = addr[i];
          };

          ///
          /// \brief Write data into the packet header
          ///
          /// This method allows for template specialization via the T_Bytes
          /// template parameter.
          ///
          /// \see PAMI::Fifo::Packet::getHeader()
          ///
          /// \tparam     T_Bytes Source header byte array length
          /// \param [in] header  Array of T_Niov source header bytes
          ///
          template <unsigned T_Bytes>
          static void writeHeader (uint8_t (&header)[T_Bytes])
          {
            writeHeader (header, T_Bytes);
          };

          ///
          /// \brief Write iovec payload data to the packet payload
          ///
          /// \param [in] iov  Array of source data iovec elements
          /// \param [in] niov Source data iovec array length
          ///
          static void writePayload (T_Packet & packet, struct iovec * iov, size_t niov)
          {
            // Packet payload size must be a multiple of 8.
            COMPILE_TIME_ASSERT((T_Packet::payload_size & 0x07) == 0);

            unsigned i, j, n, remainder;

            uint8_t  * payload = (uint8_t *) packet.getPayload ();
            uint64_t * dst;
            uint64_t * src;

            for (i = 0; i < niov; i++)
              {
                dst = (uint64_t *) payload;
                src = (uint64_t *) iov[i].iov_base;

                // Determine the number of 8 byte elements in the source and
                // save any remainder.
                //
                n = iov[i].iov_len >> 3; remainder = iov[i].iov_len & 0x07;

                for (j = 0; j < n; j++) dst[j] = src[j];

                if(remainder)
                {
                  uint8_t *src8 = (uint8_t *)&src[n];
                  uint8_t *dst8 = (uint8_t *)&dst[n];
                  while (remainder--)
                    *dst8 ++ = *src8 ++;
                }

                // Move destination pointer to the end of the previous iovec
                // data. This covers the remainder case.
                payload += iov[i].iov_len;
              }

            return;
          };

          ///
          /// \brief Write iovec payload data to the packet
          ///
          /// This method allows for template specialization via the T_Niov
          /// template parameter.
          ///
          /// \see PAMI::Fifo::Packet::getPayload()
          ///
          /// \tparam     T_Niov Source data iovec array length
          /// \param [in] iov    Array of T_Niov source data iovec elements
          ///
          template <unsigned T_Niov>
          static void writePayload (T_Packet & packet, struct iovec (&iov)[T_Niov])
          {
            writePayload (packet, (struct iovec *) iov, T_Niov);
          };

          ///
          /// \brief Write a single contiguous payload data buffer to the packet
          ///
          /// \param [in] payload Address of the data to write to the packet payload
          /// \param [in] bytes   Number of bytes to write
          ///
          static void writePayload (T_Packet & packet, void * payload, size_t bytes)
          {
            // Packet payload size must be a multiple of 8.
            COMPILE_TIME_ASSERT((T_Packet::payload_size & 0x07) == 0);

            //fprintf(stderr,"Shmem::Packet::writePayload<void>(.., %p, %zu) >>\n", payload, bytes);
            uint64_t * dst = (uint64_t *) packet.getPayload ();
            uint64_t * src = (uint64_t *) payload;

            // Determine the number of 8 byte elements in the source and
            // round up to cover any remainder.
            //
            // This forces the requirement that the packet payload size must
            // be a multiple of 8.
            size_t i, n = (bytes >> 3) + ((bytes & 0x07) != 0);

            //fprintf(stderr,"Shmem::Packet::writePayload<void>(.., %p, %zu) .. n = %zu\n", payload, bytes, n);
            for (i = 0; i < n; i++) dst[i] = src[i];

            //fprintf(stderr,"Shmem::Packet::writePayload<void>(.., %p, %zu) <<\n", payload, bytes);
            return;
          }
      };


      template <unsigned T_Niov>
      class PacketIovecWriter : public PAMI::Fifo::Interface::PacketProducer< PacketIovecWriter<T_Niov> >
      {
        protected:

          inline PacketIovecWriter () {};

        public:

          friend class PAMI::Fifo::Interface::PacketProducer < PacketIovecWriter<T_Niov> >;

          inline PacketIovecWriter (PacketIovecWriter<T_Niov> & writer) :
              PAMI::Fifo::Interface::PacketProducer< PacketIovecWriter<T_Niov> > (),
              _dispatch (writer._dispatch),
              _metadata (writer._metadata),
              _metasize (writer._metasize),
              _done (false)
          {
            unsigned i;
            for (i=0; i<T_Niov; i++)
              _iov[i] = writer._iov[i];
          };

          inline PacketIovecWriter (uint16_t dispatch) :
              PAMI::Fifo::Interface::PacketProducer< PacketIovecWriter<T_Niov> > (),
              _dispatch (dispatch)
          {};

          inline void setMetadata (void   * metadata,
                                   size_t   metasize)
          {
            _metadata = metadata;
            _metasize = metasize;
          };

          inline void setData (struct iovec * iov)
          {
            unsigned i;
            for (i=0; i<T_Niov; i++)
              _iov[i] = iov[i];
          };

          inline bool isDone ()
          {
            return _done;
          };


          template <class T_Packet>
          inline bool produce_impl (T_Packet & packet)
          {
            Shmem::Packet<T_Packet>::writePayload (packet, _iov, T_Niov);
            Shmem::Packet<T_Packet>::writeHeader (packet, (uint8_t *) _metadata, _metasize);
            Shmem::Packet<T_Packet>::setDispatch (packet, _dispatch);
            _done = true;
            return true;
          };

          template <class T_Packet>
          inline bool produce_impl (T_Packet & packet, bool & done)
          {
            produce_impl (packet);
            done = true;
            return true;
          };

          uint16_t       _dispatch;
          void         * _metadata;
          size_t         _metasize;
          struct iovec   _iov[T_Niov];
          bool           _done;
      };

      template <typename T_Payload>
      class PacketWriter : public PAMI::Fifo::Interface::PacketProducer < PacketWriter<T_Payload> >
      {
        protected:

          inline PacketWriter () {};

        public:

          friend class PAMI::Fifo::Interface::PacketProducer < PacketWriter<T_Payload> >;

          inline PacketWriter (PacketWriter<T_Payload> & writer) :
              PAMI::Fifo::Interface::PacketProducer< PacketWriter<T_Payload> > (),
              _dispatch (writer._dispatch),
              _metadata (writer._metadata),
              _metasize (writer._metasize),
              _payload (writer._payload),
              _length (writer._length)
          {};

          inline PacketWriter (uint16_t dispatch) :
              PAMI::Fifo::Interface::PacketProducer< PacketWriter<T_Payload> > (),
              _dispatch (dispatch)
          {};

          inline void setMetadata (void   * metadata,
                                   size_t   metasize)
          {
            _metadata = metadata;
            _metasize = metasize;
          };

          inline void setData (T_Payload * payload,
                               size_t      length)
          {
            _payload  = payload;
            _length   = length;
          };

          inline bool isDone ()
          {
            return (_length == 0);
          };


          template <class T_Packet>
          inline bool produce_impl (T_Packet & packet)
          {
            //fprintf(stderr,">> PacketWriter::produce_impl(T_Packet), _length = %zu, payload_size = %zu\n", _length, T_Packet::payload_size);
            Shmem::Packet<T_Packet>::writePayload (packet, _payload, _length);
            _length = 0;
            Shmem::Packet<T_Packet>::writeHeader (packet, (uint8_t *) _metadata, _metasize);
            Shmem::Packet<T_Packet>::setDispatch (packet, _dispatch);
            //fprintf(stderr,"<< PacketWriter::produce_impl(T_Packet), _length = %zu, payload_size = %zu\n", _length, T_Packet::payload_size);
            return true;
          };

          template <class T_Packet>
          inline bool produce_impl (T_Packet & packet, bool & done)
          {
            produce_impl (packet);
            done = true;
            return true;
          };

          uint16_t    _dispatch;
          void      * _metadata;
          size_t      _metasize;
          T_Payload * _payload;
          size_t      _length;
      };

      template <typename T_Payload>
      class MultiPacketWriter : public PAMI::Fifo::Interface::PacketProducer< MultiPacketWriter<T_Payload> >
      {
        protected:

          inline MultiPacketWriter () {};

        public:

          friend class PAMI::Fifo::Interface::PacketProducer < MultiPacketWriter<T_Payload> >;

          inline MultiPacketWriter (MultiPacketWriter<T_Payload> & writer) :
              PAMI::Fifo::Interface::PacketProducer< MultiPacketWriter<T_Payload> > (),
              _dispatch (writer._dispatch),
              _metadata (writer._metadata),
              _metasize (writer._metasize),
              _payload (writer._payload),
              _length (writer._length)
          {};

          inline MultiPacketWriter (uint16_t dispatch) :
              PAMI::Fifo::Interface::PacketProducer< MultiPacketWriter<T_Payload> > (),
              _dispatch (dispatch)
          {};

          inline void setMetadata (void   * metadata,
                                   size_t   metasize)
          {
            _metadata = metadata;
            _metasize = metasize;
          };

          inline void setData (T_Payload * payload,
                               size_t      length)
          {
            _payload        = payload;
            _length         = length;
            TRACE_ERR((stderr, "   MultiPacketWriter::init(%p,%zu,%p,%zu)\n", metadata, metasize, payload, length));
          };

          inline bool isDone ()
          {
            TRACE_ERR((stderr,"   MultiPacketWriter::isDone(), this = %p, _length (%p) = %zu, return = %d\n", this, &_length, _length, (_length == 0)));
            return (_length == 0);
          };

          template <class T_Packet>
          inline bool produce_impl (T_Packet & packet)
          {
            TRACE_ERR((stderr,">> MultiPacketWriter::produce_impl(T_Packet), this = %p, _length = %zu, payload_size = %zu\n", this, _length, T_Packet::payload_size));

            if (_length >= T_Packet::payload_size)
              {
                Shmem::Packet<T_Packet>::writePayload (packet, _payload, T_Packet::payload_size);
                _length -= T_Packet::payload_size;
                _payload = (uint8_t *)_payload + T_Packet::payload_size;
              }
            else
              {
                Shmem::Packet<T_Packet>::writePayload (packet, _payload, _length);
                _length = 0;
              }

            Shmem::Packet<T_Packet>::writeHeader (packet, (uint8_t *) _metadata, _metasize);
            Shmem::Packet<T_Packet>::setDispatch (packet, _dispatch);

            TRACE_ERR((stderr,"<< MultiPacketWriter::produce_impl(T_Packet), _length (%p) = %zu\n", &_length, _length));
            return true;
          };

          template <class T_Packet>
          inline bool produce_impl (T_Packet & packet, bool & done)
          {
            TRACE_ERR((stderr,"MultiPacketWriter::produce_impl(T_Packet,bool), _length = %zu\n", _length));
            bool result = produce_impl (packet);
            done = (_length == 0);
            TRACE_ERR((stderr,"MultiPacketWriter::produce_impl(T_Packet,bool), _length = %zu, result = %d, done = %d\n", _length, result, done));
            return result;
          };

          uint16_t    _dispatch;
          void      * _metadata;
          size_t      _metasize;
          T_Payload * _payload;
          size_t      _length;
      };

    };
  };
};

#undef TRACE_ERR

#endif // __components_devices_shmem_ShmemPacket_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
