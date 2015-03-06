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
 * \file components/devices/bgq/mu2/model/CollectivePacketModelBase.h
 * \brief A templated base model for MU collective packets
 */
#ifndef __components_devices_bgq_mu2_model_CollectivePacketModelBase_h__
#define __components_devices_bgq_mu2_model_CollectivePacketModelBase_h__

#include "components/devices/bgq/mu2/CollectiveNetwork.h"

#include <spi/include/mu/DescriptorBaseXX.h>
#include <spi/include/mu/DescriptorWrapperXX.h>
#include <spi/include/mu/CollectiveMemoryFIFODescriptorXX.h>
#include <spi/include/kernel/memory.h>

#include "components/devices/CollectivePacketInterface.h"
#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/msg/InjectDescriptorMessage.h"


#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"
#include "util/trace.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#ifdef CCMI_TRACE_ALL
 #define DO_TRACE_ENTEREXIT 1
 #define DO_TRACE_DEBUG     1
#else
 #define DO_TRACE_ENTEREXIT 0
 #define DO_TRACE_DEBUG     0
#endif


namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      template <class T_Model, unsigned T_Collective, unsigned T_Channel>
      class CollectivePacketModelBase : public Interface::CollectivePacketModel < MU::CollectivePacketModelBase<T_Model, T_Collective, T_Channel>, MU::Context, 1024 + MU::InjChannel::completion_event_state_bytes, uint32_t, uint8_t, uint8_t >
      {
        protected :

          /// \see PAMI::Device::Interface::CollectivePacketModel::CollectivePacketModel
          CollectivePacketModelBase (MU::Context & context, void * cookie = NULL);

          /// \see PAMI::Device::Interface::CollectivePacketModel::~CollectivePacketModel
          virtual ~CollectivePacketModelBase () /*PAMI_abort();*/
          {
          }

        public:

          /// \see PAMI::Device::Interface::CollectivePacketModel::isPacketDeterministic
          static const bool   deterministic_packet_model         = true;

          /// \see PAMI::Device::Interface::CollectivePacketModel::isPacketReliable
          static const bool   reliable_packet_model              = true;

          /// \see PAMI::Device::Interface::CollectivePacketModel::getPacketMetadataBytes
          static const size_t packet_model_metadata_bytes        = MemoryFifoPacketHeader::packet_singlepacket_metadata_size;

          /// \see PAMI::Device::Interface::CollectivePacketModel::getPacketMultiMetadataBytes
          static const size_t packet_model_multi_metadata_bytes  = MemoryFifoPacketHeader::packet_multipacket_metadata_size;

          /// \see PAMI::Device::Interface::CollectivePacketModel::getPacketPayloadBytes
          static const size_t packet_model_payload_bytes         = MU::Context::packet_payload_size;

          /// \see PAMI::Device::Interface::CollectivePacketModel::getPacketImmediateMax
          static const size_t packet_model_immediate_bytes         = MU::Context::immediate_payload_size;

          /// \see PAMI::Device::Interface::CollectivePacketModel::getPacketTransferStateBytes
          static const size_t packet_model_state_bytes           = 1024 + MU::InjChannel::completion_event_state_bytes;

          /// \see PAMI::Device::Interface::CollectivePacketModel::init
          pami_result_t init_impl (size_t                      dispatch,
                                   Interface::RecvFunction_t   direct_recv_func,
                                   void                      * direct_recv_func_parm,
                                   Interface::RecvFunction_t   read_recv_func = NULL,
                                   void                      * read_recv_func_parm = NULL);

          /// \see PAMI::Device::Interface::CollectivePacketModel::postCollectivePacket
          template <unsigned T_Niov>
          inline bool postCollectivePacket_impl (uint8_t               (&state)[packet_model_state_bytes],
                                                 pami_event_function   fn,
                                                 void                * cookie,
                                                 uint32_t              route,
                                                 void                * metadata,
                                                 size_t                metasize,
                                                 struct iovec          (&iov)[T_Niov])
          {
            PAMI_abort();
            return false;
          };

          /// \see PAMI::Device::Interface::CollectivePacketModel::postCollectivePacket
          inline bool postCollectivePacket_impl (uint8_t              (&state)[packet_model_state_bytes],
                                                 pami_event_function   fn,
                                                 void                * cookie,
                                                 uint32_t              route,
                                                 void                * metadata,
                                                 size_t                metasize,
                                                 struct iovec        * iov,
                                                 size_t                niov)
          {
            PAMI_abort();
            return false;
          };


          /// \see PAMI::Device::Interface::CollectivePacketModel::postCollectivePacket
          inline bool postCollectivePacket_impl (uint8_t              (&state)[packet_model_state_bytes],
                                                 pami_event_function   fn,
                                                 void                * cookie,
                                                 uint32_t              route,
                                                 void                * metadata,
                                                 size_t                metasize,
                                                 void                * payload,
                                                 size_t                length)
          {
            PAMI_abort();
            return false;
          };


          /// \see PAMI::Device::Interface::CollectivePacketModel::postMultiCollectivePacket
          inline bool postMultiCollectivePacket_impl (uint8_t               (&state)[packet_model_state_bytes],
                                                      pami_event_function   fn,
                                                      void                * cookie,
                                                      uint32_t              route,
                                                      void                * metadata,
                                                      size_t                metasize,
                                                      void                * payload,
                                                      size_t                length)
          {
            PAMI_abort();
            return false;
          };


          /// \see PAMI::Device::Interface::CollectivePacketModel::postCollectivePacket
          template <unsigned T_Niov>
          inline bool postCollectivePacket_impl (uint8_t               (&state)[packet_model_state_bytes],
                                                 pami_event_function   fn,
                                                 void                * cookie,
                                                 uint32_t              route,
                                                 size_t                root,
                                                 uint8_t               op,
                                                 uint8_t               dt,
                                                 void                * metadata,
                                                 size_t                metasize,
                                                 struct iovec          (&iov)[T_Niov])
          {
            PAMI_abort();
            return false;
          };


          /// \see PAMI::Device::Interface::CollectivePacketModel::postCollectivePacket
          inline bool postCollectivePacket_impl (uint8_t              (&state)[packet_model_state_bytes],
                                                 pami_event_function   fn,
                                                 void                * cookie,
                                                 uint32_t              route,
                                                 size_t                root,
                                                 uint8_t               op,
                                                 uint8_t               dt,
                                                 void                * metadata,
                                                 size_t                metasize,
                                                 struct iovec        * iov,
                                                 size_t                niov)
          {
            PAMI_abort();
            return false;
          };


          /// \see PAMI::Device::Interface::CollectivePacketModel::postCollectivePacket
          inline bool postCollectivePacket_impl (uint8_t              (&state)[packet_model_state_bytes],
                                                 pami_event_function   fn,
                                                 void                * cookie,
                                                 uint32_t              route,
                                                 size_t                root,
                                                 uint8_t               op,
                                                 uint8_t               dt,
                                                 void                * metadata,
                                                 size_t                metasize,
                                                 void                * payload,
                                                 size_t                length);

          /// \see PAMI::Device::Interface::CollectivePacketModel::postMultiCollectivePacket
          inline bool postMultiCollectivePacket_impl (uint8_t               (&state)[packet_model_state_bytes],
                                                      pami_event_function   fn,
                                                      void                * cookie,
                                                      uint32_t              route,
                                                      size_t                root,
                                                      uint8_t               op,
                                                      uint8_t               dt,
                                                      void                * metadata,
                                                      size_t                metasize,
                                                      void                * payload,
                                                      size_t                length);

        protected:

          MUSPI_DescriptorBase            _singlepkt;
          MUSPI_DescriptorBase            _multipkt;
          MU::Context                   & _context;
          void                          * _cookie;
      };

      template <class T_Model, unsigned T_Collective, unsigned T_Channel>
      CollectivePacketModelBase<T_Model, T_Collective, T_Channel>::CollectivePacketModelBase (MU::Context & context, void * cookie) :
          Interface::CollectivePacketModel < MU::CollectivePacketModelBase<T_Model, T_Collective, T_Channel>, MU::Context, 1024 + MU::InjChannel::completion_event_state_bytes, uint32_t, uint8_t, uint8_t > (context),
          _context (context),
          _cookie (cookie)
      {
        TRACE_FN_ENTER();
        COMPILE_TIME_ASSERT(sizeof(InjectDescriptorMessage<1>) <= packet_model_state_bytes);

        // Zero-out the descriptor models before initialization
        memset((void *)&_singlepkt, 0, sizeof(_singlepkt));
        memset((void *)&_multipkt, 0, sizeof(_multipkt));

        // --------------------------------------------------------------------
        // Set the common base descriptor fields
        // --------------------------------------------------------------------
        MUSPI_BaseDescriptorInfoFields_t base;
        memset((void *)&base, 0, sizeof(base));

        base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
        base.Payload_Address = 0;
        base.Message_Length  = 0;
        base.Torus_FIFO_Map  = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CUSER;
        base.Dest.Destination.Destination = 0;

        _singlepkt.setBaseFields (&base);
        _multipkt.setBaseFields (&base);


        // --------------------------------------------------------------------
        // Set the common point-to-point descriptor fields
        // --------------------------------------------------------------------
        MUSPI_CollectiveDescriptorInfoFields_t collective;
        memset((void *)&collective, 0, sizeof(collective));

        collective.Op_Code = MUHWI_COLLECTIVE_OP_CODE_SIGNED_ADD;
        collective.Word_Length = 4;
        collective.Class_Route = 0;
        collective.Misc = T_Channel | T_Collective;
        collective.Skip = 8;

        _singlepkt.setDataPacketType (MUHWI_COLLECTIVE_DATA_PACKET_TYPE);
        _singlepkt.PacketHeader.NetworkHeader.collective.Byte8.Size = 16;
        _singlepkt.setCollectiveFields (&collective);

        _multipkt.setDataPacketType (MUHWI_COLLECTIVE_DATA_PACKET_TYPE);
        _multipkt.PacketHeader.NetworkHeader.collective.Byte8.Size = 16;
        _multipkt.setCollectiveFields (&collective);


        // --------------------------------------------------------------------
        // Set the common memory fifo descriptor fields
        // --------------------------------------------------------------------
        MUSPI_MemoryFIFODescriptorInfoFields_t memfifo;
        memset ((void *)&memfifo, 0, sizeof(memfifo));

        memfifo.Rec_FIFO_Id    = 0;
        memfifo.Rec_Put_Offset = 0;
        memfifo.Interrupt      = MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL;
        memfifo.SoftwareBit    = 0;

        _singlepkt.setMemoryFIFOFields (&memfifo);
        _singlepkt.setMessageUnitPacketType (MUHWI_PACKET_TYPE_FIFO);
        _multipkt.setMemoryFIFOFields (&memfifo);
        _multipkt.setMessageUnitPacketType (MUHWI_PACKET_TYPE_FIFO);


        // --------------------------------------------------------------------
        // Set the network header information in the descriptor models to
        // differentiate between a single-packet transfer and a multi-packet
        // transfer
        // --------------------------------------------------------------------
        MemoryFifoPacketHeader * hdr = NULL;

        hdr = (MemoryFifoPacketHeader *) & _singlepkt.PacketHeader;
        hdr->setSinglePacket (true);

        hdr = (MemoryFifoPacketHeader *) & _multipkt.PacketHeader;
        hdr->setSinglePacket (false);

        TRACE_FORMAT("<Collective %u/ Channel %u>", T_Collective, T_Channel);
        //MUSPI_DescriptorDumpHex((char *)"_singlepkt", &_singlepkt);
        //MUSPI_DescriptorDumpHex((char *)"_multipkt ", &_multipkt);

        TRACE_FN_EXIT();
      };

      template <class T_Model, unsigned T_Collective, unsigned T_Channel>
      pami_result_t CollectivePacketModelBase<T_Model, T_Collective, T_Channel>::init_impl (size_t                      dispatch,
          Interface::RecvFunction_t   direct_recv_func,
          void                      * direct_recv_func_parm,
          Interface::RecvFunction_t   read_recv_func,
          void                      * read_recv_func_parm)
      {
        TRACE_FN_ENTER();
        PAMI_assert_debug(read_recv_func == NULL && read_recv_func_parm == NULL); /// \todo remove these parms?
        // Register the direct dispatch function. The MU context will have access
        // to the packet payload in the memory fifo at the time the dispatch
        // function is invoked and can provide a direct pointer to the packet
        // payload.
        uint16_t id = 0;

        if (_context.registerPacketHandler (dispatch,
                                            direct_recv_func,
                                            direct_recv_func_parm,
                                            id))
          {
            MemoryFifoPacketHeader * hdr = NULL;

            hdr = (MemoryFifoPacketHeader *) & _singlepkt.PacketHeader;
            hdr->setDispatchId (id);

            hdr = (MemoryFifoPacketHeader *) & _multipkt.PacketHeader;
            hdr->setDispatchId (id);

            TRACE_FORMAT("<%u/%u> register packet handler success. dispatch id = %d", T_Collective, T_Channel, id);

            //MUSPI_DescriptorDumpHex((char *)"_singlepkt", &_singlepkt);
            //MUSPI_DescriptorDumpHex((char *)"_multipkt ", &_multipkt);
            TRACE_FN_EXIT();

            return PAMI_SUCCESS;
          }

        TRACE_STRING("register packet handler FAILED. return PAMI_ERROR");
        TRACE_FN_EXIT();

        return PAMI_ERROR;
      };

      template <class T_Model, unsigned T_Collective, unsigned T_Channel>
      bool CollectivePacketModelBase<T_Model, T_Collective, T_Channel>::postCollectivePacket_impl (uint8_t              (&state)[packet_model_state_bytes],
          pami_event_function   fn,
          void                * cookie,
          uint32_t              route,
          size_t                root,
          uint8_t               op,
          uint8_t               dt,
          void                * metadata,
          size_t                metasize,
          void                * payload,
          size_t                length)
      {
        TRACE_FN_ENTER();

        PAMI_assert(fn == NULL); // \todo remove fn - we always complete in the dispatch, not on post()
        PAMI_assert(T_Collective != MUHWI_COLLECTIVE_TYPE_BROADCAST);

        uint16_t              rfifo;

        size_t fnum = _context.pinCombiningFifo(root, rfifo);

        InjChannel & channel = _context.injectionGroup.channel[fnum];

        size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

        TRACE_FORMAT("<%u/%u> rfifo = %u, fnum = %zu, ndesc = %zd, empty=%u", T_Collective, T_Channel, rfifo, fnum, ndesc, channel.isSendQueueEmpty());

        MUHWI_Destination_t dest;
        _context.getMuDestinationTask (root, dest);
        _singlepkt.setDestination(dest);

        _singlepkt.setOpCode(op);
        _singlepkt.setWordLength(dt);

        TRACE_FORMAT("root %zd, op %u, dt %u, dest %u/%u/%u/%u/%u", root, op, dt,
                     dest.Destination.A_Destination,dest.Destination.B_Destination,dest.Destination.C_Destination,dest.Destination.D_Destination,dest.Destination.E_Destination);

        TRACE_FORMAT("metadata %p, metasize %zu, payload %p, length %zu, route %u",metadata, metasize, payload, length, route);
        // Determine the physical address of the payload
        Kernel_MemoryRegion_t memRegion;
        uint32_t rc;
        rc = Kernel_CreateMemoryRegion (&memRegion, payload, length);
        PAMI_assert ( rc == 0 );
        uint64_t paddr = (uint64_t) memRegion.BasePa +
                         ((uint64_t) payload - (uint64_t) memRegion.BaseVa);

        TRACE_FORMAT("paddr = %ld (%p)",paddr, (void *)paddr);

        if (likely(channel.isSendQueueEmpty() && ndesc > 0))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.

            MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();
            TRACE_FORMAT("<%u/%u> desc = %p", T_Collective, T_Channel, desc);

            // Clone the single-packet model descriptor into the injection fifo
            MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) desc;
            _singlepkt.clone (*memfifo);

            // Initialize the injection fifo descriptor in-place.
            memfifo->setRecFIFOId (rfifo);
            memfifo->setClassRoute (route);

#ifdef ENABLE_MAMBO_WORKAROUNDS
            // Mambo does not support zero-byte packets
            if (__global.personality._is_mambo)
              memfifo->setPayload (paddr, MAX(1, length));
            else
#endif
              memfifo->setPayload (paddr, length);

            MemoryFifoPacketHeader *hdr = (MemoryFifoPacketHeader*)
                                          & memfifo->PacketHeader;
            //Eliminated memcpy and an if branch
            hdr->setMetaData(metadata, metasize);

            // Finally, advance the injection fifo tail pointer. This action
            // completes the injection operation.
            channel.injFifoAdvanceDesc ();
          }
        else
          {
            // Create a simple single-descriptor message
            InjectDescriptorMessage<1> * msg =
              (InjectDescriptorMessage<1> *) state;
            new (msg) InjectDescriptorMessage<1> (channel, fn, cookie);

            // Clone the single-packet descriptor model into the message
            _singlepkt.clone (msg->desc[0]);

            // Initialize the injection fifo descriptor in-place.
            msg->desc[0].setRecFIFOId (rfifo);/// \todo necessary? or always the same?
            msg->desc[0].setClassRoute (route);

#ifdef ENABLE_MAMBO_WORKAROUNDS
            // Mambo does not support zero-byte packets
            if (__global.personality._is_mambo)
              msg->desc[0].setPayload (paddr, MAX(1, length));
            else
#endif
              msg->desc[0].setPayload (paddr, length);

            TRACE_FORMAT("<%u/%u> desc = %p, paddr = %ld, payload = %p, length = %zu", T_Collective, T_Channel, & msg->desc[0], paddr, payload, length);
            // Copy the metadata into the packet header.
            MemoryFifoPacketHeader * hdr =
              (MemoryFifoPacketHeader*) & msg->desc[0].PacketHeader;

            // Eliminated memcpy and an if branch
            hdr->setMetaData (metadata, metasize);

            // Post the message to the injection channel
            channel.post (msg);
          }

        TRACE_FN_EXIT();
        return true;
      };

      template <class T_Model, unsigned T_Collective, unsigned T_Channel>
      bool CollectivePacketModelBase<T_Model, T_Collective, T_Channel>::postMultiCollectivePacket_impl (uint8_t               (&state)[packet_model_state_bytes],
          pami_event_function   fn,
          void                * cookie,
          uint32_t              route,
          size_t                root,
          uint8_t               op,
          uint8_t               dt,
          void                * metadata,
          size_t                metasize,
          void                * payload,
          size_t                length)
      {
        TRACE_FN_ENTER();

        PAMI_assert(fn == NULL); // \todo remove fn - we always complete in the dispatch, not on post()
        PAMI_assert(T_Collective != MUHWI_COLLECTIVE_TYPE_BROADCAST);

        uint16_t              rfifo;

        size_t fnum = _context.pinCombiningFifo(root, rfifo);

        InjChannel & channel = _context.injectionGroup.channel[fnum];

        size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

        TRACE_FORMAT("<%u/%u> rfifo = %u, fnum = %zu, ndesc = %zd, empty=%u", T_Collective, T_Channel, rfifo, fnum, ndesc, channel.isSendQueueEmpty());

        MUHWI_Destination_t dest;
        _context.getMuDestinationTask (root, dest);
        _multipkt.setDestination(dest);

        _multipkt.setOpCode(op);
        _multipkt.setWordLength(dt);

        TRACE_FORMAT("root %zd, op %u, dt %u, dest %u/%u/%u/%u/%u", root, op, dt,
                     dest.Destination.A_Destination,dest.Destination.B_Destination,dest.Destination.C_Destination,dest.Destination.D_Destination,dest.Destination.E_Destination);

        // Determine the physical address of the payload
        Kernel_MemoryRegion_t memRegion;
        uint32_t rc;
        rc = Kernel_CreateMemoryRegion (&memRegion, payload, length);
        PAMI_assert ( rc == 0 );
        uint64_t paddr = (uint64_t) memRegion.BasePa +
                         ((uint64_t) payload - (uint64_t) memRegion.BaseVa);

        TRACE_FORMAT("metadata %p, metasize %zu, payload %p, paddr = %ld (%p), length %zu, route %u",metadata, metasize, payload, paddr, (void *)paddr, length, route);

        if (likely(channel.isSendQueueEmpty() && ndesc > 0))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.

            MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();
            TRACE_FORMAT("<%u/%u> desc = %p", T_Collective, T_Channel, desc);

            // Clone the multi-packet model descriptor into the injection fifo
            MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) desc;
            _multipkt.clone (*memfifo);

            // Initialize the injection fifo descriptor in-place.
            memfifo->setRecFIFOId (rfifo);/// \todo necessary? or always the same?
            memfifo->setClassRoute (route);

#ifdef ENABLE_MAMBO_WORKAROUNDS
            // Mambo does not support zero-byte packets
            if (__global.personality._is_mambo)
              memfifo->setPayload (paddr, MAX(1, length));
            else
#endif
              memfifo->setPayload (paddr, length);

            MemoryFifoPacketHeader *hdr = (MemoryFifoPacketHeader*)
                                          & memfifo->PacketHeader;
            //Eliminated memcpy and an if branch
            hdr->setMetaData(metadata, metasize);

            // Finally, advance the injection fifo tail pointer. This action
            // completes the injection operation.
            channel.injFifoAdvanceDesc ();
          }
        else
          {
            // Create a simple single-descriptor message
            InjectDescriptorMessage<1> * msg =
              (InjectDescriptorMessage<1> *) state;
            new (msg) InjectDescriptorMessage<1> (channel, fn, cookie);

            // Clone the multi-packet descriptor model into the message
            _multipkt.clone (msg->desc[0]);

            // Initialize the injection fifo descriptor in-place.
            msg->desc[0].setRecFIFOId (rfifo);/// \todo necessary? or always the same?
            msg->desc[0].setClassRoute (route);

            // Copy the metadata into the packet header.
            MemoryFifoPacketHeader * hdr =
              (MemoryFifoPacketHeader*) & msg->desc[0].PacketHeader;

            // Eliminated memcpy and an if branch
            hdr->setMetaData (metadata, metasize);

#ifdef ENABLE_MAMBO_WORKAROUNDS
            // Mambo does not support zero-byte packets
            if (__global.personality._is_mambo)
              msg->desc[0].setPayload (paddr, MAX(1, length));
            else
#endif
              msg->desc[0].setPayload (paddr, length);

            // Post the message to the injection channel
            channel.post (msg);
          }

        TRACE_FN_EXIT();
        return true;
      };



    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_CollectivePacketModelBase_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
