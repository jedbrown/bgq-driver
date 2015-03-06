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
 * \file components/devices/bgq/mu2/model/PacketModelBase.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_PacketModelBase_h__
#define __components_devices_bgq_mu2_model_PacketModelBase_h__

#include <hwi/include/bqc/MU_PacketHeader.h>

#include <spi/include/mu/DescriptorBaseXX.h>
#include <spi/include/mu/DescriptorWrapperXX.h>
#include <spi/include/mu/Pt2PtMemoryFIFODescriptorXX.h>
#include <spi/include/kernel/memory.h>

#include "components/devices/PacketInterface.h"
#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/msg/InjectDescriptorMessage.h"

#include "math/Memcpy.x.h"

#include "util/trace.h"
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0


namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      template <class T_Model>
      class PacketModelBase : public Interface::PacketModel < MU::PacketModelBase<T_Model> >
      {
        protected :

          /// \see PAMI::Device::Interface::PacketModel::PacketModel
          PacketModelBase (MU::Context & context, void * cookie = NULL);

          /// \see PAMI::Device::Interface::PacketModel::~PacketModel
          virtual ~PacketModelBase () { /*PAMI_abort();*/ }

        public:

          typedef MU::Context Device;

          /// \see PAMI::Device::Interface::PacketModel::isPacketDeterministic
          static const bool   deterministic_packet_model         = true;

          /// \see PAMI::Device::Interface::PacketModel::isPacketReliable
          static const bool   reliable_packet_model              = true;

          /// \see PAMI::Device::Interface::PacketModel::isReadRequired
          static const bool   read_is_required_packet_model      = false;

          /// \see PAMI::Device::Interface::PacketModel::getPacketMetadataBytes
          static const size_t packet_model_metadata_bytes        =
            MemoryFifoPacketHeader::packet_singlepacket_metadata_size;

          /// \see PAMI::Device::Interface::PacketModel::getPacketMultiMetadataBytes
          static const size_t packet_model_multi_metadata_bytes  =
            MemoryFifoPacketHeader::packet_multipacket_metadata_size;

          /// \see PAMI::Device::Interface::PacketModel::getPacketPayloadBytes
          static const size_t packet_model_payload_bytes         = MU::Context::packet_payload_size;

          /// \see PAMI::Device::Interface::PacketModel::getPacketImmediateBytes
          static const size_t packet_model_immediate_bytes       = MU::Context::immediate_payload_size;

          /// \see PAMI::Device::Interface::PacketModel::getPacketTransferStateBytes
          static const size_t packet_model_state_bytes           = 1024 + MU::InjChannel::completion_event_state_bytes;

          /// \see PAMI::Device::Interface::PacketModel::init
          pami_result_t init_impl (size_t                      dispatch,
                                   Interface::RecvFunction_t   recv_func,
                                   void                      * recv_func_parm);

          /// \see PAMI::Device::Interface::PacketModel::postPacket
          template <unsigned T_Niov>
          inline bool postPacket_impl (size_t         target_task,
                                       size_t         target_offset,
                                       void         * metadata,
                                       size_t         metasize,
                                       struct iovec   (&iov)[T_Niov]);

          /// \see PAMI::Device::Interface::PacketModel::postPacket
          template <unsigned T_StateBytes, unsigned T_Niov>
          inline bool postPacket_impl (uint8_t               (&state)[T_StateBytes],
                                       pami_event_function   fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       void                * metadata,
                                       size_t                metasize,
                                       struct iovec          (&iov)[T_Niov]);

          /// \see PAMI::Device::Interface::PacketModel::postPacket
          template <unsigned T_StateBytes>
          inline bool postPacket_impl (uint8_t              (&state)[T_StateBytes],
                                       pami_event_function   fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       void                * metadata,
                                       size_t                metasize,
                                       struct iovec        * iov,
                                       size_t                niov);

          /// \see PAMI::Device::Interface::PacketModel::postPacket
          template <unsigned T_StateBytes>
          inline bool postPacket_impl (uint8_t              (&state)[T_StateBytes],
                                       pami_event_function   fn,
                                       void                * cookie,
                                       size_t                target_task,
                                       size_t                target_offset,
                                       void                * metadata,
                                       size_t                metasize,
                                       void                * payload,
                                       size_t                length);

          /// \see PAMI::Device::Interface::PacketModel::postMultiPacket
          template <unsigned T_StateBytes>
          inline bool postMultiPacket_impl (uint8_t               (&state)[T_StateBytes],
                                            pami_event_function   fn,
                                            void                * cookie,
                                            size_t                target_task,
                                            size_t                target_offset,
                                            void                * metadata,
                                            size_t                metasize,
                                            void                * payload,
                                            size_t                length);


        protected:
          void short_copy (void *dst, void *src, int n);

          MUSPI_DescriptorBase            _singlepkt;
          MUSPI_DescriptorBase            _multipkt;
          void                          * _cookie;

        public:
        
          MU::Context                   & device;
      };

      template <class T_Model>
      PacketModelBase<T_Model>::PacketModelBase (MU::Context & context, void * cookie) :
          Interface::PacketModel < MU::PacketModelBase<T_Model> > (context),
          _cookie (cookie),
          device (context)
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
        base.Torus_FIFO_Map  = 0;
        base.Dest.Destination.Destination = 0;

        _singlepkt.setBaseFields (&base);
        _multipkt.setBaseFields (&base);


        // --------------------------------------------------------------------
        // Set the common point-to-point descriptor fields
        // --------------------------------------------------------------------
        MUSPI_Pt2PtDescriptorInfoFields_t pt2pt;
        memset((void *)&pt2pt, 0, sizeof(pt2pt));

        pt2pt.Hints_ABCD = 0;
        pt2pt.Misc1 =
          MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
          MUHWI_PACKET_DO_NOT_DEPOSIT |
          MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE;
        pt2pt.Misc2 =
          MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;

        pt2pt.Skip       = 136; /* Skip entire single packet for checksum since it
				 * may contain metadata */
        _singlepkt.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
        _singlepkt.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;
        _singlepkt.setPt2PtFields (&pt2pt);
        pt2pt.Skip       = 8; /* Skip only the data packet header for checksum */
        _multipkt.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
        _multipkt.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;
        _multipkt.setPt2PtFields (&pt2pt);


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

        TRACE_FN_EXIT();
      };

      template <class T_Model>
      pami_result_t PacketModelBase<T_Model>::init_impl (size_t                      dispatch,
                                                         Interface::RecvFunction_t   recv_func,
                                                         void                      * recv_func_parm)
      {
        TRACE_FN_ENTER();

        // Register the direct dispatch function. The MU context will have access
        // to the packet payload in the memory fifo at the time the dispatch
        // function is invoked and can provide a direct pointer to the packet
        // payload.
        uint16_t id = 0;

        if (device.registerPacketHandler (dispatch,
                                          recv_func,
                                          recv_func_parm,
                                          id))
          {
            MemoryFifoPacketHeader * hdr = NULL;

            hdr = (MemoryFifoPacketHeader *) & _singlepkt.PacketHeader;
            hdr->setDispatchId (id);

            hdr = (MemoryFifoPacketHeader *) & _multipkt.PacketHeader;
            hdr->setDispatchId (id);

            TRACE_FORMAT("register packet handler success. dispatch id = %d", id);
            TRACE_FN_EXIT();

            return PAMI_SUCCESS;
          }

        TRACE_STRING("register packet handler FAILED. return PAMI_ERROR");
        TRACE_FN_EXIT();

        return PAMI_ERROR;
      };

      template <class T_Model>
      template <unsigned T_Niov>
      bool PacketModelBase<T_Model>::postPacket_impl (size_t         target_task,
                                                      size_t         target_offset,
                                                      void         * metadata,
                                                      size_t         metasize,
                                                      struct iovec   (&iov)[T_Niov])
      {
        TRACE_FN_ENTER();

        uint32_t              dest;
        uint16_t              rfifo;
        uint64_t              map;

        size_t fnum = device.pinFifo (target_task,
                                        target_offset,
                                        dest,
                                        rfifo,
                                        map);

        InjChannel & channel = device.injectionGroup.channel[fnum];
        bool isfree = channel.hasFreeSpaceWithUpdate ();

	uint16_t rfifo_base =  *(uint16_t *)(&_singlepkt.PacketHeader.messageUnitHeader.Packet_Types.Memory_FIFO);
	uint16_t disp_msize = (((MemoryFifoPacketHeader*)&_singlepkt.PacketHeader)->getDispatchId()) | ((metasize-1) << 12);

        register double f0  FP_REGISTER(0);
        register double f1  FP_REGISTER(1);
        VECTOR_LOAD_NU (&_singlepkt,  0, f0);  /* Load first 32 bytes to reg 0*/
        VECTOR_LOAD_NU (&_singlepkt, 32, f1);  /* Load second 32 bytes to reg 1*/

        if (likely(channel.isSendQueueEmpty() && isfree))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.

            MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();

            void * vaddr;
            uint64_t paddr;

            channel.getDescriptorPayload (desc, vaddr, paddr);

            // Clone the single-packet model descriptor into the injection fifo
            MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) desc;
            //_singlepkt.clone (*memfifo);
            VECTOR_STORE_NU(memfifo,  0, f0);
            VECTOR_STORE_NU(memfifo, 32, f1);

            // Initialize the injection fifo descriptor in-place.
            memfifo->setDestination (dest);
            memfifo->setTorusInjectionFIFOMap (map);
	    //eliminate load from mem fifo as that is cache miss
            //memfifo->setRecFIFOId (rfifo);	    
	    *(uint16_t *)(&memfifo->PacketHeader.messageUnitHeader.Packet_Types.Memory_FIFO) = rfifo_base | (rfifo << 6); 

            MemoryFifoPacketHeader *hdr = (MemoryFifoPacketHeader*)
                                          & memfifo->PacketHeader;
            //Eliminated memcpy and an if branch
            hdr->setMetaData(metadata, metasize, disp_msize);

            // Copy the payload into the immediate payload buffer.
            size_t i, tbytes = 0;
            uint8_t * dst = (uint8_t *)vaddr;

            for (i = 0; i < T_Niov; i++)
              {
                //Optimize 0byte message rate
                short_copy ((dst + tbytes), iov[i].iov_base, iov[i].iov_len);
                tbytes += iov[i].iov_len;
              }

#ifdef ENABLE_MAMBO_WORKAROUNDS

            // Mambo does not support zero-byte packets
            if (__global.personality._is_mambo) /// \todo mambo hack
              tbytes = MAX(1, tbytes);

#endif
            // Set the payload information.
            memfifo->setPayload (paddr, tbytes);
            //MUSPI_DescriptorDumpHex((char*)"PAMI SendImmediate:  Descriptor",desc);
            // Finally, advance the injection fifo tail pointer. This action
            // completes the injection operation.
            //uint64_t sequence =
            channel.injFifoAdvanceDesc ();

            //bool done = channel.checkDescComplete (sequence);
            //TRACE_FORMAT("done = %d", done);

            TRACE_FN_EXIT();
            return true;
          }

        TRACE_FN_EXIT();
        return false;
      };

      template <class T_Model>
      template <unsigned T_StateBytes, unsigned T_Niov>
      bool PacketModelBase<T_Model>::postPacket_impl (uint8_t               (&state)[T_StateBytes],
                                                      pami_event_function   fn,
                                                      void                * cookie,
                                                      size_t           target_task,
                                                      size_t                target_offset,
                                                      void                * metadata,
                                                      size_t                metasize,
                                                      struct iovec          (&iov)[T_Niov])
      {
        TRACE_FN_ENTER();

        MUHWI_Destination_t   dest;
        uint16_t              rfifo;
        uint64_t              map;

        size_t fnum = device.pinFifo (target_task, target_offset, dest,
                                        rfifo, map);

        InjChannel & channel = device.injectionGroup.channel[fnum];
        size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

        size_t i, tbytes = 0;

        for (i = 0; i < T_Niov; i++)
          {
            tbytes += iov[i].iov_len;
          }

#ifdef ENABLE_MAMBO_WORKAROUNDS

        // Mambo does not support zero-byte packets
        if (__global.personality._is_mambo) /// \todo mambo hack
          tbytes = MAX(1, tbytes);

#endif

        if (likely(channel.isSendQueueEmpty() && ndesc > 0))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.

            MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();

            void * vaddr   = NULL;
            uint64_t paddr = 0;

            // Clone the single-packet model descriptor into the injection fifo
            MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) desc;
            _singlepkt.clone (*memfifo);

            // Initialize the injection fifo descriptor in-place.
            memfifo->setDestination (dest);
            memfifo->setTorusInjectionFIFOMap (map);
            memfifo->setRecFIFOId (rfifo);

            MemoryFifoPacketHeader *hdr = (MemoryFifoPacketHeader*)
                                          & memfifo->PacketHeader;
            //Eliminated memcpy and an if branch
            hdr->setMetaData(metadata, metasize);

            TRACE_HEXDATA(hdr, 32);

            if (unlikely(tbytes > packet_model_immediate_bytes))
              {
                // Determine the physical address of the (temporary) payload
                // buffer from the model state memory.
                vaddr = ((uint8_t *) state) + InjChannel::completion_event_state_bytes;
                Kernel_MemoryRegion_t memRegion;
                uint32_t rc;
                rc = Kernel_CreateMemoryRegion (&memRegion, vaddr, tbytes);
                PAMI_assert_debug ( rc == 0 );
                paddr = (uint64_t)memRegion.BasePa +
                        ((uint64_t)vaddr - (uint64_t)memRegion.BaseVa);

                TRACE_FORMAT("desc = %p, vaddr = %p, paddr = %ld (%p)", desc, vaddr, paddr, (void *)paddr);

                // Set the payload information.
                memfifo->setPayload (paddr, tbytes);

                // Copy the payload into the model state memory
                uint8_t * dst = (uint8_t *) vaddr;

                for (i = 0; i < T_Niov; i++)
                  {
                    memcpy (dst, iov[i].iov_base, iov[i].iov_len);
                    dst += iov[i].iov_len;
                  }

                TRACE_HEXDATA(vaddr, 32);

                // Advance the injection fifo tail pointer. This action
                // completes the injection operation.
                uint64_t sequence = channel.injFifoAdvanceDesc ();

                // Add a completion event for the sequence number associated with
                // the descriptor that was injected.
                if (likely(fn != NULL))
                  channel.addCompletionEvent (state, fn, cookie, sequence);
              }
            else
              {
                // Determine the physical address of the (immediate) payload
                // buffer associated with this descriptor slot.
                channel.getDescriptorPayload (desc, vaddr, paddr);

                TRACE_FORMAT("desc = %p, vaddr = %p, paddr = %ld (%p)", desc, vaddr, paddr, (void *)paddr);

                // Set the payload information.
                memfifo->setPayload (paddr, tbytes);

                // Copy the payload into the immediate payload buffer.
                uint8_t * dst = (uint8_t *) vaddr;

                for (i = 0; i < T_Niov; i++)
                  {
                    memcpy (dst, iov[i].iov_base, iov[i].iov_len);
                    dst += iov[i].iov_len;
                  }

                TRACE_HEXDATA(vaddr, 32);

                // Advance the injection fifo tail pointer. This action
                // completes the injection operation.
                channel.injFifoAdvanceDesc ();

                // Invoke the completion callback function
                if (likely(fn != NULL))
                  fn (_cookie, cookie, PAMI_SUCCESS);
              }
          }
        else
          {
            // Create a simple single-descriptor message
            InjectDescriptorMessage<1> * msg =
              (InjectDescriptorMessage<1> *) state;
            new (msg) InjectDescriptorMessage<1> (channel, fn, cookie);

            uint8_t * payload = (uint8_t *) (msg + 1);

            // Clone the single-packet descriptor model into the message
            _singlepkt.clone (msg->desc[0]);

            // Copy the payload into the model state memory
            uint8_t * dst = payload;

            for (i = 0; i < T_Niov; i++)
              {
                memcpy (dst, iov[i].iov_base, iov[i].iov_len);
                dst += iov[i].iov_len;
              }

            // Determine the physical address of the (temporary) payload
            // buffer from the model state memory.
            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;
            rc = Kernel_CreateMemoryRegion (&memRegion, payload, tbytes);
            PAMI_assert ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)payload - (uint64_t)memRegion.BaseVa);

            // Initialize the injection fifo descriptor in-place.
            msg->desc[0].setDestination (dest);
            msg->desc[0].setTorusInjectionFIFOMap (map);
            msg->desc[0].setRecFIFOId (rfifo);
            msg->desc[0].setPayload (paddr, tbytes);

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

      template <class T_Model>
      template <unsigned T_StateBytes>
      bool PacketModelBase<T_Model>::postPacket_impl (uint8_t               (&state)[T_StateBytes],
                                                      pami_event_function   fn,
                                                      void                * cookie,
                                                      size_t                target_task,
                                                      size_t                target_offset,
                                                      void                * metadata,
                                                      size_t                metasize,
                                                      struct iovec        * iov,
                                                      size_t                niov)
      {
        TRACE_FN_ENTER();

        MUHWI_Destination_t   dest;
        uint16_t              rfifo;
        uint64_t              map;

        size_t fnum = device.pinFifo (target_task, target_offset, dest,
                                        rfifo, map);

        InjChannel & channel = device.injectionGroup.channel[fnum];
        size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

        // Determine the amount of data to write into the packet payload.
        size_t i, tbytes = 0;

        for (i = 0; i < niov; i++)
          {
            tbytes += iov[i].iov_len;
          }

#ifdef ENABLE_MAMBO_WORKAROUNDS
        // Mambo does not support zero-byte packets

        if (__global.personality._is_mambo) /// \todo mambo hack
          tbytes = MAX(1, tbytes);

#endif

        if (likely(channel.isSendQueueEmpty() && ndesc > 0))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.

            MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();

            void * vaddr   = NULL;
            uint64_t paddr = 0;

            // Clone the single-packet model descriptor into the injection fifo
            MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) desc;
            _singlepkt.clone (*memfifo);

            // Initialize the injection fifo descriptor in-place.
            memfifo->setDestination (dest);
            memfifo->setTorusInjectionFIFOMap (map);
            memfifo->setRecFIFOId (rfifo);

            MemoryFifoPacketHeader *hdr = (MemoryFifoPacketHeader*)
                                          & memfifo->PacketHeader;
            //Eliminated memcpy and an if branch
            hdr->setMetaData(metadata, metasize);

            TRACE_HEXDATA(hdr, 32);

            if (unlikely(tbytes > packet_model_immediate_bytes))
              {
                // Determine the physical address of the (temporary) payload
                // buffer from the model state memory.
                vaddr = ((uint8_t *) state) + InjChannel::completion_event_state_bytes;
                Kernel_MemoryRegion_t memRegion;
                uint32_t rc;
                rc = Kernel_CreateMemoryRegion (&memRegion, vaddr, tbytes);
                PAMI_assert_debug ( rc == 0 );
                paddr = (uint64_t)memRegion.BasePa +
                        ((uint64_t)vaddr - (uint64_t)memRegion.BaseVa);

                TRACE_FORMAT("desc = %p, vaddr = %p, paddr = %ld (%p)", desc, vaddr, paddr, (void *)paddr);

                // Set the payload information.
                memfifo->setPayload (paddr, tbytes);

                // Copy the payload into the model state memory
                uint8_t * dst = (uint8_t *) vaddr;

                for (i = 0; i < niov; i++)
                  {
                    memcpy (dst, iov[i].iov_base, iov[i].iov_len);
                    dst += iov[i].iov_len;
                  }

                TRACE_HEXDATA(vaddr, 32);

                // Advance the injection fifo tail pointer. This action
                // completes the injection operation.
                uint64_t sequence = channel.injFifoAdvanceDesc ();

                // Add a completion event for the sequence number associated with
                // the descriptor that was injected.
                if (likely(fn != NULL))
                  channel.addCompletionEvent (state, fn, cookie, sequence);
              }
            else
              {
                // Determine the physical address of the (immediate) payload
                // buffer associated with this descriptor slot.
                channel.getDescriptorPayload (desc, vaddr, paddr);

                TRACE_FORMAT("desc = %p, vaddr = %p, paddr = %ld (%p)", desc, vaddr, paddr, (void *)paddr);

                // Set the payload information.
                memfifo->setPayload (paddr, tbytes);

                // Copy the payload into the immediate payload buffer.
                uint8_t * dst = (uint8_t *) vaddr;

                for (i = 0; i < niov; i++)
                  {
                    memcpy (dst, iov[i].iov_base, iov[i].iov_len);
                    dst += iov[i].iov_len;
                  }

                TRACE_HEXDATA(vaddr, 64);

                // Advance the injection fifo tail pointer. This action
                // completes the injection operation.
                channel.injFifoAdvanceDesc ();

                // Invoke the completion callback function
                if (likely(fn != NULL))
                  fn (_cookie, cookie, PAMI_SUCCESS);
              }
          }
        else
          {
            // Create a simple single-descriptor message
            InjectDescriptorMessage<1> * msg =
              (InjectDescriptorMessage<1> *) state;
            new (msg) InjectDescriptorMessage<1> (channel, fn, cookie);

            uint8_t * payload = (uint8_t *) (msg + 1);

            // Clone the single-packet descriptor model into the message
            _singlepkt.clone (msg->desc[0]);

            // Copy the payload into the model state memory
            uint8_t * dst = payload;

            for (i = 0; i < niov; i++)
              {
                memcpy (dst, iov[i].iov_base, iov[i].iov_len);
                dst += iov[i].iov_len;
              }

            // Determine the physical address of the (temporary) payload
            // buffer from the model state memory.
            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;
            rc = Kernel_CreateMemoryRegion (&memRegion, payload, tbytes);
            PAMI_assert ( rc == 0 );
            uint64_t paddr = (uint64_t)memRegion.BasePa +
                             ((uint64_t)payload - (uint64_t)memRegion.BaseVa);

            // Initialize the injection fifo descriptor in-place.
            msg->desc[0].setDestination (dest);
            msg->desc[0].setTorusInjectionFIFOMap (map);
            msg->desc[0].setRecFIFOId (rfifo);
            msg->desc[0].setPayload (paddr, tbytes);

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

      template <class T_Model>
      template <unsigned T_StateBytes>
      bool PacketModelBase<T_Model>::postPacket_impl (uint8_t               (&state)[T_StateBytes],
                                                      pami_event_function   fn,
                                                      void                * cookie,
                                                      size_t                target_task,
                                                      size_t                target_offset,
                                                      void                * metadata,
                                                      size_t                metasize,
                                                      void                * payload,
                                                      size_t                length)
      {
        TRACE_FN_ENTER();

        register iovec iov[1] = {{payload, length}};

        postPacket_impl (state, fn, cookie, target_task, target_offset,
                         metadata, metasize, iov);

        TRACE_FN_EXIT();
        return true;
      };

      template <class T_Model>
      template <unsigned T_StateBytes>
      bool PacketModelBase<T_Model>::postMultiPacket_impl (uint8_t               (&state)[T_StateBytes],
                                                           pami_event_function   fn,
                                                           void                * cookie,
                                                           size_t                target_task,
                                                           size_t                target_offset,
                                                           void                * metadata,
                                                           size_t                metasize,
                                                           void                * payload,
                                                           size_t                length)
      {
        TRACE_FN_ENTER();

        MUHWI_Destination_t   dest;
        uint16_t              rfifo;
        uint64_t              map;

        // Determine the physical address of the source data.
        Kernel_MemoryRegion_t memregion;
        Kernel_CreateMemoryRegion (&memregion, payload, length);
        uint64_t paddr = (uint64_t) memregion.BasePa +
                         ((uint64_t) payload - (uint64_t) memregion.BaseVa);

        size_t fnum = device.pinFifo (target_task, target_offset, dest,
                                        rfifo, map);

        InjChannel & channel = device.injectionGroup.channel[fnum];
        size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

        if (likely(channel.isSendQueueEmpty() && ndesc > 0))
          {
            // There is at least one descriptor slot available in the injection
            // fifo before a fifo-wrap event.

            MUHWI_Descriptor_t * desc = channel.getNextDescriptor ();

            // Clone the multi-packet model descriptor into the injection fifo
            MUSPI_DescriptorBase * memfifo = (MUSPI_DescriptorBase *) desc;
            _multipkt.clone (*memfifo);

            // Initialize the injection fifo descriptor in-place.
            memfifo->setDestination (dest);
            memfifo->setTorusInjectionFIFOMap (map);
            memfifo->setRecFIFOId (rfifo);
#ifdef ENABLE_MAMBO_WORKAROUNDS

            // Mambo does not support zero-byte packets
            if (__global.personality._is_mambo) /// \todo mambo hack
              memfifo->setPayload (paddr, MAX(1, length));
            else
#endif
              memfifo->setPayload (paddr, length);

            MemoryFifoPacketHeader *hdr = (MemoryFifoPacketHeader*)
                                          & memfifo->PacketHeader;
            //Eliminated memcpy and an if branch
            hdr->setMetaData(metadata, metasize);

            // Advance the injection fifo tail pointer. This action
            // completes the injection operation.
            uint64_t sequence = channel.injFifoAdvanceDesc ();

            // Add a completion event for the sequence number associated with
            // the descriptor that was injected.
            if (likely(fn != NULL))
              channel.addCompletionEvent (state, fn, cookie, sequence);
          }
        else
          {
            // Create a simple single-descriptor message
            InjectDescriptorMessage<1> * msg =
              (InjectDescriptorMessage<1> *) state;
            new (msg) InjectDescriptorMessage<1> (channel, fn, cookie);

            // Clone the multi-packet descriptor model into the message
            _multipkt.clone (msg->desc[0]);

            // Copy the metadata into the packet header.
            MemoryFifoPacketHeader * hdr =
              (MemoryFifoPacketHeader*) & msg->desc[0].PacketHeader;

            // Eliminated memcpy and an if branch
            hdr->setMetaData (metadata, metasize);

            // Initialize the injection fifo descriptor in-place.
            msg->desc[0].setDestination (dest);
            msg->desc[0].setTorusInjectionFIFOMap (map);
            msg->desc[0].setRecFIFOId (rfifo);
#ifdef ENABLE_MAMBO_WORKAROUNDS

            // Mambo does not support zero-byte packets
            if (__global.personality._is_mambo) /// \todo mambo hack
              msg->desc[0].setPayload (paddr, MAX(1, length));
            else
#endif
              msg->desc[0].setPayload (paddr, length);

            // Post the message to the injection channel
            channel.post (msg);
          }

        TRACE_FN_EXIT();
        return true;
      }

      ///
      /// \brief A copy routing for short messages that assumes destination is
      ///        aligned to 8 bytes.
      ///
      /// PPC64 hw can handle unaligned integer loads/stores with few cycles of
      /// overhead.
      ///
      template <class T_Model>
      void PacketModelBase<T_Model>::short_copy (void *dst, void *src, int n)
      {
        if (n == 0)
          return;

        uint64_t *sp = (uint64_t *)src;
        uint64_t *dp = (uint64_t  *)dst;

        ///For MPICH meta data
        if (n == 16)
          {
            *dp ++ = *sp ++;
            *dp    = *sp;
          }
        else
          {
            size_t dwords   = n >> 3;
            n  = n & 0x7;

            while ( dwords-- )
              *dp ++ = *sp ++;

            uint8_t *sp8 = (uint8_t *)sp;
            uint8_t *dp8 = (uint8_t *)dp;

            while (n--)
              *dp8 ++ = *sp8 ++;
          }
      }

    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_PacketModelBase_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
