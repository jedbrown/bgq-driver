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
 * \file components/devices/shmem/shaddr/BgqShaddrReadWrite.h
 * \brief ???
 */

#ifndef __components_devices_shmem_shaddr_BgqShaddrReadWrite_h__
#define __components_devices_shmem_shaddr_BgqShaddrReadWrite_h__

#include "components/devices/shmem/shaddr/BgqShaddr.h"
#include "components/devices/shmem/shaddr/ShaddrInterface.h"
#include "math/Memcpy.x.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      class BgqShaddrReadWrite : public BgqShaddr, public ShaddrInterface<BgqShaddrReadWrite>
      {
        public:

          static const bool shaddr_write_supported = true;

          inline BgqShaddrReadWrite () :
              BgqShaddr (),
              ShaddrInterface<BgqShaddrReadWrite> ()
          {
          };

          inline ~BgqShaddrReadWrite () {};

          ///
          /// \brief Shared address write operation using virtual addresses
          ///
          /// \see ShaddrInterface::write
          ///
          inline size_t write_impl (void   * remote,
                                    void   * local,
                                    size_t   bytes,
                                    size_t   task);

          ///
          /// \brief Shared address write operation using memory regions
          ///
          /// \see ShaddrInterface::write
          ///
          inline size_t write_impl (Memregion * remote,
                                    size_t      remote_offset,
                                    Memregion * local,
                                    size_t      local_offset,
                                    size_t      bytes);
      };  // PAMI::Device::Shmem::BgqShaddrReadWrite class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace


size_t PAMI::Device::Shmem::BgqShaddrReadWrite::write_impl (void   * remote,
                                                            void   * local,
                                                            size_t   bytes,
                                                            size_t   task)
{
  PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
  return 0;
};


size_t PAMI::Device::Shmem::BgqShaddrReadWrite::write_impl (Memregion * remote,
                                                            size_t      remote_offset,
                                                            Memregion * local,
                                                            size_t      local_offset,
                                                            size_t      bytes)
{
  TRACE_ERR((stderr, ">> Shmem::BgqShaddrReadWrite::write_impl()\n"));
  uint32_t rc = 0;

  void * local_paddr = (void *) (local->getBasePhysicalAddress() + local_offset);
  void * local_vaddr = NULL;
  rc = Kernel_Physical2Virtual (local_paddr, &local_vaddr);
  PAMI_assert_debugf(rc == 0, "%s<%d> .. Kernel_Physical2Virtual(), rc = %d\n", __FILE__, __LINE__, rc);

  void * remote_paddr = (void *) (remote->getBasePhysicalAddress() + remote_offset);
  void * remote_vaddr = NULL;
  rc = Kernel_Physical2GlobalVirtual (remote_paddr, &remote_vaddr);
  PAMI_assert_debugf(rc == 0, "%s<%d> .. Kernel_Physical2GlobalVirtual(), rc = %d\n", __FILE__, __LINE__, rc);


  TRACE_ERR((stderr, "   Shmem::BgqShaddrReadWrite::write_impl(), local_vaddr = %p, remote_vaddr = %p\n", local_vaddr, remote_vaddr));

  size_t bytes_to_copy = bytes;

  if (unlikely(bytes_to_copy > SHMEM_COPY_BLOCK_SIZE))
    bytes_to_copy = SHMEM_COPY_BLOCK_SIZE;

  Core_memcpy (remote_vaddr, local_vaddr, bytes_to_copy);

  TRACE_ERR((stderr, "<< Shmem::BgqShaddrReadWrite::write_impl(), bytes_to_copy = %zu\n", bytes_to_copy));
  return bytes_to_copy;
};

#undef TRACE_ERR
#endif /* __components_devices_shmem_bgq_BgqShaddrReadWrite_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
