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
 * \file components/devices/shmem/shaddr/BgqShaddr.h
 * \brief ???
 */

#ifndef __components_devices_shmem_shaddr_BgqShaddr_h__
#define __components_devices_shmem_shaddr_BgqShaddr_h__

#include <pami.h>
#include "math/Memcpy.x.h"
#include "Arch.h"

#include "components/fifo/FifoInterface.h"

#define SHMEM_COPY_BLOCK_SIZE	2048*1024

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      class BgqShaddr
      {
        protected:

          inline  BgqShaddr () {};
          inline ~BgqShaddr () {};

        public:

          class PhysicalAddress
          {
            public:

              class PacketWriter : public PAMI::Fifo::Interface::PacketProducer<PhysicalAddress::PacketWriter>
              {
                protected:

                  inline PacketWriter () {};

                public:

                  friend class PAMI::Fifo::Interface::PacketProducer <PhysicalAddress::PacketWriter>;

                  ///
                  /// \brief Payload data for the system shared address packet
                  ///
                  typedef struct info
                  {
                    uint64_t paddr; /// Physical address of the "payload"
                    size_t   bytes; /// Number of bytes to transfer
                    uint16_t id;    /// Actual dispatch id to invoke
                  } info_t;

                  ///
                  /// \brief Provide a read-only shared-address global virtual address
                  ///        as the payload pointer to the packet dispatch function
                  ///
                  /// The actual data contained in the packet is the physical address
                  /// of the source data and the original dispatch id which identifies
                  /// the function and cookie to invoke with the global virtual address
                  /// converted from the physical address of the source data.
                  ///
                  /// \see PAMI::Device::Interface::RecvFunction_t
                  ///
                  template <class T_Device>
                  static int intercept_function (void   * metadata,
                                                 void   * payload,
                                                 size_t   bytes,
                                                 void   * recv_func_parm,
                                                 void   * cookie)
                  {
                    TRACE_ERR((stderr, "PhysicalAddress::PacketWriter::intercept_fn(), recv_func_parm (device*) = %p\n", recv_func_parm));
                    info_t * data = (info_t *) payload;
                    TRACE_ERR((stderr, "PhysicalAddress::PacketWriter::intercept_fn(), data->paddr = %ld, data->id = %d\n", data->paddr, data->id));

                    void * vaddr = NULL;
                    Kernel_Physical2GlobalVirtual ((void *)(data->paddr), &vaddr);

                    TRACE_ERR((stderr, "PhysicalAddress::PacketWriter::intercept_fn(), paddr (%ld) -> vaddr (%p)\n", data->paddr, vaddr));
                    T_Device * device = (T_Device *) recv_func_parm;
                    device->dispatch (data->id, metadata, vaddr, data->bytes);

                    return 0;
                  }

                  inline PacketWriter (uint16_t   dispatch_paddr,
                                       uint16_t   dispatch_user,
                                       void     * metadata,
                                       size_t     metasize,
                                       void     * payload,
                                       size_t     length) :
                      PAMI::Fifo::Interface::PacketProducer<PhysicalAddress::PacketWriter> (),
                      _dispatch_paddr (dispatch_paddr),
                      _dispatch_user (dispatch_user),
                      _metadata (metadata),
                      _metasize (metasize),
                      _length (length)
                  {
                    // ====================================================
                    // Determine the physical address of the source buffer.
                    //
                    uint32_t rc;
                    Kernel_MemoryRegion_t memregion;
                    rc = Kernel_CreateMemoryRegion (&memregion, payload, length);
                    PAMI_assert ( rc == 0 );

                    uint64_t offset = (uint64_t)payload - (uint64_t)memregion.BaseVa;
                    _paddr = (uint64_t)memregion.BasePa + offset;
                    //
                    // ====================================================
                  };


                  inline bool isDone ()
                  {
                    TRACE_ERR((stderr, "   PhysicalAddress::PacketWriter::isDone(), this = %p, _length (%p) = %zu, return = %d\n", this, &_length, _length, (_length == 0)));
                    return (_length == 0);
                  };

                  template <class T_Packet>
                  inline bool produce_impl (T_Packet & packet)
                  {
                    TRACE_ERR((stderr, ">> PhysicalAddress::PacketWriter::produce_impl(T_Packet), this = %p, _length = %zu, payload_size = %zu\n", this, _length, T_Packet::payload_size));

                    info_t payload;
                    payload.id = _dispatch_user;
                    payload.bytes = (_length < SHMEM_COPY_BLOCK_SIZE) ? _length : SHMEM_COPY_BLOCK_SIZE;
                    payload.paddr = _paddr;

                    Shmem::Packet<T_Packet>::writePayload (packet, (void *) &payload, sizeof(info_t));
                    Shmem::Packet<T_Packet>::writeHeader (packet, (uint8_t *) _metadata, _metasize);
                    Shmem::Packet<T_Packet>::setDispatch (packet, _dispatch_paddr);

                    _paddr  += payload.bytes;
                    _length -= payload.bytes;

                    TRACE_ERR((stderr, "<< PhysicalAddress::PacketWriter::produce_impl(T_Packet), _length (%p) = %zu\n", &_length, _length));
                    return true;
                  };

                  template <class T_Packet>
                  inline bool produce_impl (T_Packet & packet, bool & done)
                  {
                    TRACE_ERR((stderr, "PhysicalAddress::PacketWriter::produce_impl(T_Packet,bool), _length = %zu\n", _length));
                    bool result = produce_impl (packet);
                    done = (_length == 0);
                    TRACE_ERR((stderr, "PhysicalAddress::PacketWriter::produce_impl(T_Packet,bool), _length = %zu, result = %d, done = %d\n", _length, result, done));
                    return result;
                  };

                  uint16_t    _dispatch_paddr;
                  uint16_t    _dispatch_user;
                  void      * _metadata;
                  size_t      _metasize;
                  size_t      _length;
                  uint64_t    _paddr;
              };

              template <class T_Device>
              class PacketMessage : public SendQueue::Message
              {
                protected:

                  /// invoked by the thread object
                  /// \see SendQueue::Message::_work
                  static pami_result_t __advance (pami_context_t context, void * cookie)
                  {
                    PacketMessage * msg = (PacketMessage *) cookie;
                    return msg->advance();
                  };

                public:

                  inline pami_result_t advance ()
                  {
                    ssize_t last_packet_consumed = _device->_fifo[_fnum].lastPacketConsumed();
                    ssize_t last_packet_produced = _device->_fifo[_fnum].lastPacketProduced();

                    TRACE_ERR((stderr, ">> PhysicalAddress::PacketMessage::advance, this = %p, last_packet_consumed = %zu, last_packet_produced = %zu\n", this, last_packet_consumed, last_packet_produced));

                    if (_writer.isDone())
                      {
                        if (last_packet_produced <= last_packet_consumed)
                          {
                            // This removes the work from the generic device.
                            this->setStatus (PAMI::Device::Done);

                            // This causes the message completion callback to be invoked.
                            TRACE_ERR((stderr, "<< PhysicalAddress::PacketMessage::advance, this = %p, return PAMI_SUCCESS\n", this));
                            return PAMI_SUCCESS;
                          }

                        return PAMI_EAGAIN;
                      }


		    while(!_writer.isDone()) {
		      if (!_device->_fifo[_fnum].producePacket(_writer))
			{
			  // Unable to write a packet. Try again later.
			  TRACE_ERR((stderr, "<< PhysicalAddress::PacketMessage::advance, this = %p, return PAMI_EAGAIN\n", this));
			  return PAMI_EAGAIN;
			}
		    }		      

                    TRACE_ERR((stderr, "<< PhysicalAddress::PacketMessage::advance, this = %p, return PAMI_EAGAIN\n", this));
                    return PAMI_EAGAIN;
                  };

                  inline PacketMessage (pami_event_function   fn,
                                        void                * cookie,
                                        T_Device            * device,
                                        size_t                fnum,
                                        uint16_t   dispatch_paddr,
                                        uint16_t   dispatch_user,
                                        void     * metadata,
                                        size_t     metasize,
                                        void     * payload,
                                        size_t     length) :
                      SendQueue::Message (PacketMessage::__advance, this, fn, cookie, device->getContextOffset()),
                      _writer (dispatch_paddr, dispatch_user, metadata, metasize, payload, length),
                      _device (device),
                      _fnum (fnum)
                  {
		    // constructor
                  };

                protected:


                  PacketWriter          _writer;
                  T_Device            * _device;
                  size_t                _fnum;
              };

            private:

              inline PhysicalAddress () {};
              inline ~PhysicalAddress () {};
          };



          static const bool shaddr_va_supported    = false;
          static const bool shaddr_mr_supported    = true;

          static const bool shaddr_read_supported  = true;

          template <class T_Device>
          inline void initialize_impl (T_Device * device)
          {
            device->registerSystemRecvFunction (PhysicalAddress::PacketWriter::intercept_function<T_Device>,
                                                device, system_paddr_dispatch);
          };

          inline bool isEnabled_impl () { return true; };

          ///
          /// \brief Shared address read operation using virtual addresses
          ///
          /// \see ShaddrInterface::read
          ///
          inline size_t read_impl (void   * local,
                                   void   * remote,
                                   size_t   bytes,
                                   size_t   task);

          ///
          /// \brief Shared address read operation using memory regions
          ///
          /// \see ShaddrInterface::read
          ///
          inline size_t read_impl (Memregion * local,
                                   size_t      local_offset,
                                   Memregion * remote,
                                   size_t      remote_offset,
                                   size_t      bytes);

          uint16_t system_paddr_dispatch;

      };  // PAMI::Device::Shmem::BgqShaddr class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace

size_t PAMI::Device::Shmem::BgqShaddr::read_impl (void   * local,
                                                  void   * remote,
                                                  size_t   bytes,
                                                  size_t   task)
{
  PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
  return 0;
};


size_t PAMI::Device::Shmem::BgqShaddr::read_impl (Memregion * local,
                                                  size_t      local_offset,
                                                  Memregion * remote,
                                                  size_t      remote_offset,
                                                  size_t      bytes)
{
  TRACE_ERR((stderr, ">> Shmem::BgqShaddr::read_impl()\n"));
  uint32_t rc = 0;

  void * local_paddr = (void *) (local->getBasePhysicalAddress() + local_offset);
  void * local_vaddr = NULL;
  rc = Kernel_Physical2Virtual (local_paddr, &local_vaddr);
  PAMI_assert_debugf(rc == 0, "%s<%d> .. Kernel_Physical2Virtual(), rc = %d\n", __FILE__, __LINE__, rc);

  void * remote_paddr = (void *) (remote->getBasePhysicalAddress() + remote_offset);
  void * remote_vaddr = NULL;
  rc = Kernel_Physical2GlobalVirtual (remote_paddr, &remote_vaddr);
  PAMI_assert_debugf(rc == 0, "%s<%d> .. Kernel_Physical2GlobalVirtual(), rc = %d\n", __FILE__, __LINE__, rc);


  TRACE_ERR((stderr, "   Shmem::BgqShaddr::read_impl(), local_vaddr = %p, remote_vaddr = %p\n", local_vaddr, remote_vaddr));

  size_t bytes_to_copy = bytes;

  if (unlikely(bytes_to_copy > SHMEM_COPY_BLOCK_SIZE))
    bytes_to_copy = SHMEM_COPY_BLOCK_SIZE;

  Core_memcpy (local_vaddr, remote_vaddr, bytes_to_copy);

  TRACE_ERR((stderr, "<< Shmem::BgqShaddr::read_impl(), bytes_to_copy = %zu\n", bytes_to_copy));
  return bytes_to_copy;
};

#undef TRACE_ERR
#endif /* __components_devices_shmem_bgq_BgqShaddr_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
