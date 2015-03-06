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
 * \file components/devices/bgq/mu2/MemoryFifoPacketHeader.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_MemoryFifoPacketHeader_h__
#define __components_devices_bgq_mu2_MemoryFifoPacketHeader_h__

#include <hwi/include/bqc/MU_PacketHeader.h>

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
      ///  ' ' = used by hardware
      ///  '.' = available for software
      ///  's' = available for single packet only
      ///  '?' = "is single packet"
      ///  'm' = single packet metadata offset
      ///  'd' = single packet dispatch
      ///  'D' = multi-packet dispatch (available for single packet)
      ///  'M' = multi-packet metadata offset (available for single packet)
      ///
      ///      +--------+ pt2pt network header, 12 bytes
      /// B 00 |        |
      /// B 01 |        |
      ///   .. |        |
      ///   .. |        |
      ///   .. |        |
      ///   .. |        |
      ///   .. |        |
      ///   .. |        |
      ///   .. |        |
      ///   .. |        |
      /// B 10 |        |
      /// B 11 |        |
      ///      +--------+
      ///
      ///      +--------+ message unit header, 20 bytes
      /// B 12 |        |
      /// B 13 |  ?sssss|
      /// B 14 |mmmmdddd|
      /// B 15 |dddddddd|
      /// B 16 |ssssssss|
      /// B 17 |ssssssss|
      /// B 18 |MMMMDDDD|
      ///   .. |DDDDDDDD|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      ///   .. |........|
      /// B 31 |........|
      ///      +--------+
      ///
      class MemoryFifoPacketHeader : public MUHWI_PacketHeader_t
      {
        public:

          /// Number of unused software bytes available in a single packet
          /// transfer. The dispatch identifier and other control data are
          /// are set outside of this range.
          static const size_t packet_singlepacket_metadata_size = 16;

          /// Number of unused software bytes available in a multi-packet
          /// transfer. The dispatch identifier and other control data are
          /// are set outside of this range.
          static const size_t packet_multipacket_metadata_size  = 12;

          ///
          /// \brief Set the 'single packet' attribute in a mu packet header
          ///
          /// The 'single packet' attribute is a single bit in the header which
          /// is unused/unmodified by hardware. This bit is a hint to the type
          /// of packet to be sent or received. A 'single packet' message has
          /// additional bytes in the packet header which are unused and
          /// unmodified by the mu hardware. This allows software to 'pack'
          /// additional information into a 'single packet' header.
          ///
          /// \warning The single packet attribute must be set before the
          ///          dispatch id is set.
          ///
          /// \see setDispatchId
          /// \see getDispatchId
          ///
          /// \param[in] value  The 'single packet' attribute
          ///
          inline void setSinglePacket (bool value)
          {
            messageUnitHeader.Packet_Types.Memory_FIFO.Unused1 = value;
          };

          ///
          /// \brief Retreive the 'single packet' attribute from a mu packet header
          ///
          /// \see setSinglePacket
          ///
          /// \return The 'single packet' attribute value
          ///
          inline uint32_t isSinglePacket ()
          {
            return messageUnitHeader.Packet_Types.Memory_FIFO.Unused1;
          };

          inline void setHeaderInfo (uint16_t id, void *metadata, uint16_t metasize)
          {
            uint16_t offset = metasize - 1;

            //Use bytes 14 and 15 for single packet and 18 and 19 for multipacket
            uint16_t *raw16 = (uint16_t*)((char *)this + (18 - isSinglePacket() * 4));
            *raw16 = (id | (offset << 12));

            uint8_t *raw8 = (uint8_t *) this + 31 - offset;
            uint8_t *src = (uint8_t *) metadata;
            //If metasize is a constant compiler will inline the memcpy
            memcpy(raw8, src, metasize);
          }

          inline void getHeaderInfo (uint16_t &id, void **metadata)
          {
            TRACE_FN_ENTER();

            uint16_t *raw16 = (uint16_t*)((char *)this + (18 - isSinglePacket() * 4));
            TRACE_FORMAT("this = %p, isSinglePacket() = %d, raw16 = %p", (void *) this, isSinglePacket(), (void *) raw16);

            id = (*raw16) & 0x0FFF; //Use 12bits for dispatch id
            *metadata = (char *)this + 31 - ((*raw16) >> 12);
            TRACE_FORMAT("*raw16 = %d, ((*raw16)>>12) = %d, id = %d, metadata = %p", *raw16, ((*raw16) >> 12), id, *metadata);

            TRACE_FN_EXIT();
          }


          ///
          /// \brief Set the dispatch id in a mu packet header
          ///
          /// The dispatch id is written in a different location depending on
          /// the value of the "single packet" attribute in the header.
          ///
          /// \warning The single packet attribute must be set before this
          ///          method is used.
          ///
          /// \see setSinglePacket
          /// \see isSinglePacket
          ///
          /// \param[in] id  The dispatch identifier to set
          ///
          inline void setDispatchId (uint16_t id)
          {
            //Use bytes 14 and 15 for single packet and 18 and 19 for multipacket
            uint16_t *raw16 = (uint16_t*)((char *)this + (18 - isSinglePacket() * 4));
            *raw16 = (id & 0x0FFF);
            return;
          };

          ///
          /// \brief Get the dispatch id from a mu packet header
          ///
          /// The dispatch id is in a different location depending on
          /// the value of the "single packet" attribute of the header.
          ///
          /// \warning The single packet attribute must be set before this
          ///          method is used.
          ///
          /// \see setSinglePacket
          /// \see isSinglePacket
          ///
          /// \return The dispatch identifier
          ///
          inline uint16_t getDispatchId ()
          {
            //Use bytes 14 and 15 for single packet and 18 and 19 for multipacket
            uint16_t *raw16 = (uint16_t*)((char *)this + (18 - isSinglePacket() * 4));
            return (*raw16)&0x0FFF; //Use 12 bits for dispatch id
          };

          ///
          /// \brief Set the Metadata correspoding to this
          /// packet. Metadata will be aligned to the input parameter
          /// bytes
          /// \param[in] metadata of the packet protocol
          /// \param[in] metadata size in bytes
          ///
          inline void setMetaData (void *metadata, int bytes)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("metadata = %p, bytes = %d", metadata, bytes);
            TRACE_HEXDATA(this, 32);

            //Use bytes 14 and 15 for single packet and 18 and 19 for multipacket
            uint16_t *raw16 = (uint16_t*)((char *)this + (18 - isSinglePacket() * 4));

            uint16_t offset = bytes - 1;

            *raw16 = ((*raw16) | (offset << 12));

            uint8_t *raw8 = (uint8_t *) this + 31 - offset;
            uint8_t *src = (uint8_t *) metadata;
            memcpy(raw8, src, bytes);

            TRACE_HEXDATA(this, 32);
            TRACE_FN_EXIT();
          }


          ///
          /// \brief Set the Metadata correspoding to this
          /// packet. Metadata will be aligned to the input parameter
          /// bytes
          /// \param[in] metadata of the packet protocol
          /// \param[in] metadata size in bytes
          ///
	  inline void setMetaData (void        * metadata, 
				   int           bytes, 
				   int16_t       disp_msize)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("metadata = %p, bytes %d, disp_msize = %d", 
			 metadata, bytes, disp_msize);
            TRACE_HEXDATA(this, 32);

            //Use bytes 14 and 15 for single packet and 18 and 19 for multipacket
            uint16_t *raw16 = (uint16_t*)((char *)this + (18 - isSinglePacket() * 4));
            uint16_t offset = bytes - 1;
            *raw16 = disp_msize;
	    
            uint8_t *raw8 = (uint8_t *) this + 31 - offset;
            uint8_t *src = (uint8_t *) metadata;
            memcpy(raw8, src, bytes);

            TRACE_HEXDATA(this, 32);
            TRACE_FN_EXIT();
          }

          ///
          /// \brief Get the Metadata in the packet
          /// \retval pointer to the metadata
          ///
          inline void* getMetaData ()
          {
            uint16_t *raw16 = (uint16_t*)((char *)this + (18 - isSinglePacket() * 4));
            uint16_t offset = (*raw16) >> 12;
            //return the bottom maxmetasize bytes
            return (char *)this + (31 - offset);
          }

      };
    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif   // __components_devices_bgq_mu2_MemoryFifoPacketHeader_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
