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
 * \file components/fifo/FifoPacket.h
 * \brief ???
 */

#ifndef __components_fifo_FifoPacket_h__
#define __components_fifo_FifoPacket_h__

#include <stdint.h>
#include <string.h>

#include "Arch.h"

#include "components/fifo/PacketInterface.h"

#ifndef TRACE
#define TRACE(x) // fprintf x
#endif

//#define DEBUG_MEMORY

namespace PAMI
{
  namespace Fifo
  {
    ///
    /// \brief Simple packet implementation
    ///
    /// \note The packet payload size is the entire packet size less the
    ///       packet header size.
    ///
    /// \tparam T_HeaderSize Specifies the number of bytes in the packet header
    /// \tparam T_PacketSize Specifies the number of bytes in the entire packet
    ///
    template <unsigned T_HeaderSize, unsigned T_PacketSize>
    class FifoPacket : public Interface::Packet<FifoPacket <T_HeaderSize, T_PacketSize> >
    {
      public:

        friend class Interface::Packet<FifoPacket <T_HeaderSize, T_PacketSize> >;

        static const size_t header_size  = T_HeaderSize;

        static const size_t payload_size = T_PacketSize - T_HeaderSize;

        inline FifoPacket () :
            Interface::Packet<FifoPacket <T_HeaderSize, T_PacketSize> > ()
        {};


      protected:

        inline void clear_impl ()
        {
#ifdef DEBUG_MEMORY
          unsigned i;
          unsigned * ptr = (unsigned *) _data;
          for (i=0; i<(T_PacketSize/sizeof(unsigned)); i++) ptr[i] = (unsigned) 0xdeadbeef;
#else
          memset ((void *)&_data[0], 0, T_PacketSize);
#endif
        }

        inline void * getHeader_impl ()
        {
          return (void *) &_data[0];
        };

        inline void copyHeader_impl (void * dst)
        {
          memcpy (dst, (void *)_data, T_HeaderSize);
        };

        inline void writeHeader_impl (void * src)
        {
          memcpy ((void *)_data, src, T_HeaderSize);
        };

        inline void * getPayload_impl ()
        {
          return (void *) &_data[T_HeaderSize];
        };

        inline void copyPayload_impl (void * addr)
        {
          memcpy ((void *) &_data[T_HeaderSize], addr, T_PacketSize - T_HeaderSize);
        };

      private:
        uint8_t _data[T_PacketSize];
    };
  };
};
#undef TRACE_ERR
#endif // __util_fifo_FifoPacket_h__


//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
