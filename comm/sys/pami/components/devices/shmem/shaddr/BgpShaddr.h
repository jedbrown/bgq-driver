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
 * \file components/devices/shmem/shaddr/BgpShaddr.h
 * \brief ???
 */

#ifndef __components_devices_shmem_shaddr_BgpShaddr_h__
#define __components_devices_shmem_shaddr_BgpShaddr_h__

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include <pthread.h>
#include <spi/kernel_interface.h>

#include <pami.h>
#include "Global.h"
#include "Memory.h"
#include "components/devices/shmem/shaddr/ShaddrInterface.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      class BgpShaddr : public ShaddrInterface<BgpShaddr>
      {
        protected:

          void print_data (void * addr, size_t bytes)
          {
            uint8_t * p = (uint8_t *) addr;
            size_t i;

            for (i = 0; i < bytes; i += 4)
              {
                fprintf (stderr, "  addr[%04zu] (%p): %02x %02x %02x %02x\n", i, &p[i], p[i], p[i+1], p[i+2], p[i+3]);
              }
          }

          typedef struct
          {
            uint64_t last_paddr;
            uint32_t last_vaddr;
            unsigned tlbslot;
          } process_window_t;

          static const bool process_window_size    = 1024 * 1024;

        public:

          static const bool shaddr_va_supported    = false;
          static const bool shaddr_mr_supported    = true;

          static const bool shaddr_read_supported  = true;
          static const bool shaddr_write_supported = false; // Test ro shared address (bgq style)
          //static const bool shaddr_write_supported = true;

          inline BgpShaddr () :
              ShaddrInterface<BgpShaddr> ()
          {
            int minslot;             // Lowest TLB in the range of ProcessWindow TLBs for the current core
            int maxslot;             // Highest TLB in the range of ProcessWindow TLBs for the current core

            // Determine the range of tlb slots allocated for process windows
            minslot = 0;
            maxslot = 0;
            int rc = 0;                   // Testcase return code
            rc = Kernel_GetProcessWindowSlotRange (&minslot, &maxslot);
            TRACE_ERR((stderr, "BgpShaddr(), after Kernel_GetProcessWindowSlotRange(): minslot = %d, maxslot = %d\n", minslot, maxslot));

            PAMI::Interface::Mapping::nodeaddr_t addr;
            __global.mapping.nodeAddr (addr);
            size_t thispeer;
            __global.mapping.node2peer (addr, thispeer);

            size_t npeers;
            __global.mapping.nodePeers (npeers);

            //
            //
            //
            if (minslot >= 0 && maxslot >= 0)
              {
                size_t nslots = maxslot - minslot + 1;
                _enabled = (nslots >= (npeers - 1));
                TRACE_ERR((stderr, "BgpShaddr(), _enabled = %d, nslots = %d\n", _enabled, nslots));
              }

            //
            //
            //

            TRACE_ERR((stderr, "BgpShaddr(), _enabled = %d, npeers = %zu, thispeer = %zu, this = %p\n", _enabled, npeers, thispeer, this));

            if (_enabled)
              {
                size_t i, j = 0;

                for (i = 0; i < npeers; i++)
                  {
                    if (thispeer != i)
                      {
                        _window[i].last_vaddr = (uint32_t) - 1;
                        _window[i].last_paddr = (uint32_t) - 1;
                        _window[i].tlbslot    = minslot + j++;
                        TRACE_ERR((stderr, "BgpShaddr(), _window[%zu].tlbslot = %d\n", i, _window[i].tlbslot));
                      }

                    TRACE_ERR((stderr, "BgpShaddr(), & _window[%zu] = %p\n", i, & _window[i]));
                  }
              }
          }

          inline ~BgpShaddr () {};

          template <class T_Device> 
          inline void initialize_impl (T_Device * device) 
          {}; 

          inline bool isEnabled_impl ()
          {
            return _enabled;
          }

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
          /// \brief Shared address write operation using virtual addresses
          ///
          /// \see ShaddrInterface::write
          ///
          inline size_t write_impl (void   * remote,
                                    void   * local,
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

        protected:

          ///
          /// \brief physical to virtual address translation and tlb mapper
          ///
          /// \param[in]  peer   Peer id
          /// \param[in]  paddr  Physical address
          /// \param[out] bytes  Number of bytes requested, available
          ///
          /// \return virtual address
          ///
          void * p2v (size_t peer, uint64_t paddr, size_t & bytes);
#if 1
          /// \brief Virtual to physical address translation and tlb mapper
          ///
          /// \param[in]  peer
          /// \param[in]  address   Virtual address
          /// \param[in]  bytes_in  Number of bytes requested
          /// \param[out] bytes_out Number of bytes available
          inline void * v2p (size_t   peer,
                             void   * address,
                             size_t   bytes_in,
                             size_t & bytes_out);
#endif
          process_window_t _window[4];
          bool             _enabled;

      };  // PAMI::Device::Shmem::BgpShaddr class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace

size_t PAMI::Device::Shmem::BgpShaddr::read_impl (void   * local,
                                                  void   * remote,
                                                  size_t   bytes,
                                                  size_t   task)
{
  TRACE_ERR((stderr, ">> BgpShaddr::read_impl(%p, %p, %zu, %zu)\n", local, remote, bytes, task));
  size_t n, bytes_available;
  void * paddr = v2p (task, remote, bytes, n);
  TRACE_ERR((stderr, "   BgpShaddr::read_impl(), paddr = %p, n = %zu\n", paddr, n));

  if (likely(bytes <= n))
    {
      Memory::sync();
      /* print_data (paddr, bytes); */
      TRACE_ERR((stderr, "   BgpShaddr::read_impl(), before memcpy(%p, %p, %zu)\n", local, paddr, bytes));
      memcpy (local, paddr, bytes);
      Memory::sync();
      TRACE_ERR((stderr, "<< BgpShaddr::read_impl()\n"));
      return bytes;
    }


  size_t bytes_to_copy = bytes - n;
  size_t bytes_copied  = n;

  Memory::sync();
  memcpy (local, paddr, n);
  Memory::sync();

  while (bytes_to_copy > 0)
    {
      void * source = (void *)(((size_t)paddr) + bytes_copied);
      TRACE_ERR((stderr, "   BgpShaddr::read_impl(), bytes_to_copy = %zu, bytes_copied = %zu, source = %p\n", bytes_to_copy, bytes_copied, source));
      paddr = v2p (task, source, bytes_to_copy, bytes_available);
      TRACE_ERR((stderr, "   BgpShaddr::read_impl(), bytes_available = %zu, paddr = %p\n", bytes_available, paddr));
      n = MIN(bytes_to_copy, bytes_available);
      Memory::sync();
      memcpy (local, paddr, n);
      Memory::sync();

      bytes_to_copy -= n;
      bytes_copied  += n;
    }

  TRACE_ERR((stderr, "<< BgpShaddr::read_impl()\n"));
  return bytes;
};

size_t PAMI::Device::Shmem::BgpShaddr::write_impl (void   * remote,
                                                   void   * local,
                                                   size_t   bytes,
                                                   size_t   task)
{
  TRACE_ERR((stderr, ">> BgpShaddr::write_impl(%p, %p, %zu, %zu)\n", remote, local, bytes, task));
  size_t n, bytes_available;
  void * paddr = v2p (task, remote, bytes, n);
  TRACE_ERR((stderr, "   BgpShaddr::write_impl(), paddr = %p, n = %zu\n", paddr, n));

  if (likely(bytes <= n))
    {
      /* print_data (local, bytes); */
      /* print_data (paddr, bytes); */
      TRACE_ERR((stderr, "   BgpShaddr::write_impl(), before memcpy(%p, %p, %zu)\n", paddr, local, bytes));
      Memory::sync();
      memcpy (paddr, local, bytes);
      Memory::sync();
      /* print_data (paddr, bytes); */
      TRACE_ERR((stderr, "<< BgpShaddr::write_impl()\n"));
      return bytes;
    }

  size_t bytes_to_copy = bytes - n;
  size_t bytes_copied  = n;

  Memory::sync();
  memcpy (paddr, local, n);
  Memory::sync();

  while (bytes_to_copy > 0)
    {
      paddr = v2p (task, (void *)(((size_t)paddr) + bytes_copied),
                   bytes_to_copy, bytes_available);
      n = MIN(bytes_to_copy, bytes_available);
      Memory::sync();
      memcpy (paddr, local, n);
      Memory::sync();

      bytes_to_copy -= n;
      bytes_copied  += n;
    }

  TRACE_ERR((stderr, "<< BgpShaddr::write_impl()\n"));
  return bytes;
};

size_t PAMI::Device::Shmem::BgpShaddr::read_impl (Memregion * local,
                                                  size_t      local_offset,
                                                  Memregion * remote,
                                                  size_t      remote_offset,
                                                  size_t      bytes)
{
  TRACE_ERR((stderr, ">> BgpShaddr::read_impl('memregion'), %p, %zu, %p, %zu, %zu)\n", local, local_offset, remote, remote_offset, bytes));

  size_t n = bytes;
  void * remote_va = p2v (remote->getPeer(), remote->getBasePhysicalAddress() + remote_offset, n);
  void * local_va = (void *) ((size_t) local->getBaseVirtualAddress() + local_offset);


  TRACE_ERR((stderr, "   BgpShaddr::read_impl('memregion'), remote_va = %p, n = %zu\n", remote_va, n));

  if (likely(bytes <= n))
    {
      Memory::sync();
      /* print_data (remote_va, bytes); */
      TRACE_ERR((stderr, "   BgpShaddr::read_impl('memregion'), before memcpy(%p, %p, %zu)\n", local_va, remote_va, bytes));
      memcpy (local_va, remote_va, bytes);
      Memory::sync();
      TRACE_ERR((stderr, "<< BgpShaddr::read_impl('memregion')\n"));
      return bytes;
    }


  Memory::sync();
  memcpy (local_va, remote_va, n);
  Memory::sync();

  size_t bytes_copied  = n;
  size_t bytes_to_copy = bytes - n;

  while (bytes_to_copy > 0)
    {
      void * dst = (void *) ((size_t) local_va + bytes_copied);
      void * src = p2v (remote->getPeer(), remote->getBasePhysicalAddress() + remote_offset + bytes_copied, n);

      TRACE_ERR((stderr, "   BgpShaddr::read_impl('memregion'), bytes_to_copy = %zu, bytes_copied = %zu, dst = %p, src = %p\n", bytes_to_copy, bytes_copied, dst, src));
      n = MIN(bytes_to_copy, n);
      Memory::sync();
      memcpy (dst, src, n);
      Memory::sync();

      bytes_to_copy -= n;
      bytes_copied  += n;
    }

  TRACE_ERR((stderr, "<< BgpShaddr::read_impl('memregion')\n"));
  return bytes;
};

size_t PAMI::Device::Shmem::BgpShaddr::write_impl (Memregion * remote,
                                                   size_t      remote_offset,
                                                   Memregion * local,
                                                   size_t      local_offset,
                                                   size_t      bytes)
{
  TRACE_ERR((stderr, ">> BgpShaddr::write_impl('memregion'):%d .. %p, %zu, %p, %zu, %zu)\n", __LINE__, local, local_offset, remote, remote_offset, bytes));
  size_t n = bytes;
  void * remote_va = p2v (remote->getPeer(), remote->getBasePhysicalAddress() + remote_offset, n);
  void * local_va = (void *) ((size_t) local->getBaseVirtualAddress() + local_offset);


  TRACE_ERR((stderr, "   BgpShaddr::write_impl('memregion'):%d .. remote_va = %p, n = %zu\n", __LINE__, remote_va, n));

  if (likely(bytes <= n))
    {
      Memory::sync();
      TRACE_ERR((stderr, "   BgpShaddr::write_impl('memregion'):%d .. before memcpy(%p, %p, %zu)\n", __LINE__, local_va, remote_va, bytes));
      memcpy (remote_va, local_va, bytes);
      Memory::sync();
      TRACE_ERR((stderr, "<< BgpShaddr::write_impl('memregion'):%d\n", __LINE__));
      return bytes;
    }


  Memory::sync();
  memcpy (remote_va, local_va, n);
  Memory::sync();

  size_t bytes_copied  = n;
  size_t bytes_to_copy = bytes - n;

  while (bytes_to_copy > 0)
    {
      void * src = (void *) ((size_t) local_va + bytes_copied);
      void * dst = p2v (remote->getPeer(), remote->getBasePhysicalAddress() + remote_offset + bytes_copied, n);

      TRACE_ERR((stderr, "   BgpShaddr::write_impl('memregion'):%d .. bytes_to_copy = %zu, bytes_copied = %zu, dst = %p, src = %p\n", __LINE__, bytes_to_copy, bytes_copied, dst, src));
      n = MIN(bytes_to_copy, n);
      Memory::sync();
      memcpy (dst, src, n);
      Memory::sync();

      bytes_to_copy -= n;
      bytes_copied  += n;
    }

  TRACE_ERR((stderr, "<< BgpShaddr::write_impl('memregion'):%d\n", __LINE__));
  return bytes;
};

void * PAMI::Device::Shmem::BgpShaddr::p2v (size_t     peer,
                                            uint64_t   paddr,
                                            size_t   & bytes)
{
  TRACE_ERR((stderr, ">> BgpShaddr::p2v(%zu, %lld, %zu):%d  this = %p\n", peer, paddr, bytes, __LINE__, this));
  uint32_t window_size = 1024 * 1024;

  uint32_t paddr_round_down = paddr & ~(window_size - 1);
  uint32_t paddr_offset     = paddr &  (window_size - 1);
  uint32_t actualsize       = window_size; // window size returned by setProcessWindow indexed by target core
  TRACE_ERR((stderr, "   BgpShaddr::p2v():%d .. window_size = %d, paddr_round_down = 0x%08x, paddr_offset = %d, actualsize = %u\n", __LINE__, window_size, paddr_round_down, paddr_offset, actualsize));

  TRACE_ERR((stderr, "   BgpShaddr::p2v():%d .. & _window[%zu] = %p .. _window[%zu].tlbslot = %d, _window[%zu].last_vaddr = %p, _window[%zu].last_paddr = %p\n", __LINE__, peer, &_window[peer], peer, _window[peer].tlbslot, peer, (void *) _window[peer].last_vaddr, peer, (void *) _window[peer].last_paddr));

  if (paddr_round_down != _window[peer].last_paddr)
    {
      // Map a new process window for this physical address
      TRACE_ERR((stderr, "   BgpShaddr::p2v():%d .. _window[%zu].tlbslot = %d, paddr_round_down = %d, window_size = %d\n", __LINE__, peer, _window[peer].tlbslot, paddr_round_down, window_size));
      int rc;
      rc = Kernel_SetProcessWindow (_window[peer].tlbslot, paddr_round_down,
                                    window_size,
                                    PROT_READ | PROT_WRITE,
                                    &_window[peer].last_vaddr, &_window[peer].last_paddr, &actualsize);

      PAMI_assertf(rc == 0, "%s<%d> Bad return code from Kernel_SetProcessWindow: %d\n", __FILE__, __LINE__, rc);
      PAMI_assert_debug (_window[peer].last_paddr == paddr_round_down);
      PAMI_assert_debug (actualsize == window_size);
    }

  bytes = actualsize - paddr_offset;
  void * va = (void *)(_window[peer].last_vaddr + paddr_offset);
  TRACE_ERR((stderr, "   BgpShaddr::p2v():%d .. paddr_offset = %d, actualsize = %u  Bytes=%zu\n", __LINE__, paddr_offset, actualsize, bytes));

  return va;
}


#if 1
/// \param[in]  peer
/// \param[in]  address   Virtual address
/// \param[in]  bytes_in  Number of bytes requested
/// \param[out] bytes_out Number of bytes available
void * PAMI::Device::Shmem::BgpShaddr::v2p (size_t   peer,
                                            void   * address,
                                            size_t   bytes_in,
                                            size_t & bytes_out)
{
#if 0
  uint32_t window_size = 1024 * 1024;

  // Get the physical address
  uint32_t paddr_lower = 0;
  uint32_t paddr_upper = 0;
  int rc = Kernel_Virtual2Physical(address, bytes_in, &paddr_upper, &paddr_lower );
  PAMI_assertf(rc == 0, "%s<%d> Bad return code from Kernel_Virtual2Physical: %d\n", __FILE__, __LINE__, rc);

  uint64_t paddr = ((uint64_t) paddr_upper) << 32;
  paddr |= paddr_lower;

  // =======================

  //uint32_t paddr_round_down = paddr & ~(process_window_size - 1);
  //uint32_t paddr_offset     = paddr &  (process_window_size - 1);
  uint32_t paddr_round_down = paddr & ~(window_size - 1);
  uint32_t paddr_offset     = paddr &  (window_size - 1);
  fprintf (stderr, "-- paddr_upper = 0x%08x, paddr_lower = 0x%08x, paddr = 0x%016llx, paddr_round_down = %d (0x%016x), window_size = %d\n", paddr_upper, paddr_lower, paddr, paddr_round_down, paddr_round_down, window_size);

  //uint32_t actualvaddr;      // virtual address returned by the setProcessWindow indexed by target core
  //uint64_t actualpaddr;      // physical address returned by setProcessWindow indexed by target core
  uint32_t actualsize;       // window size returned by setProcessWindow indexec by target core

  if (paddr_round_down != _window[peer].last_paddr)
    {
      // Map a new process window for this physical address
      fprintf (stderr, "-- _window[%zu].tlbslot = %d, paddr_round_down = %d (0x%016x), process_window_size = %d, window_size = %d, permissions: %d\n", peer, _window[peer].tlbslot, paddr_round_down, paddr_round_down,
               process_window_size, window_size, PROT_READ | PROT_WRITE);
      rc = Kernel_SetProcessWindow (_window[peer].tlbslot, paddr_round_down,
//                                    process_window_size,
                                    window_size,
                                    PROT_READ | PROT_WRITE,
                                    &_window[peer].last_vaddr, &_window[peer].last_paddr, &actualsize);

      PAMI_assertf(rc == 0, "%s<%d> Bad return code from Kernel_SetProcessWindow: %d\n", __FILE__, __LINE__, rc);
      fprintf (stderr, "-- actualpaddr = 0x%016llx, paddr_round_down = 0x%016x, actualsize = %d, window_size = %d\n", _window[peer].last_paddr, paddr_round_down, actualsize, window_size);
      PAMI_assert_debug (_window[peer].last_paddr == paddr_round_down);
//      PAMI_assert_debug (actualsize == process_window_size);
      PAMI_assert_debug (actualsize == window_size);

      //_window[peer].last_paddr = paddr_round_down;
      //_window[peer].last_vaddr = actualvaddr;
    }

  bytes_out = actualsize - paddr_offset;
  void * va = (void *)(_window[peer].last_vaddr + paddr_offset);
  fprintf (stderr, "-- actualsize = %d, paddr_offset = %d, _window[%zu].last_paddr = %lld, bytes_in = %zu, bytes_out = %zu, return: %p\n", actualsize, paddr_offset, peer, _window[peer].last_paddr, bytes_in, bytes_out, va);


  return va;
#else
  return NULL;
#endif
}
#endif
#endif /* __components_devices_shmem_bgp_BgpShaddr_h__ */
