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
///
/// \file common/bgq/Memregion.h
/// \brief PAMI BG/Q specific memregion implementation.
///
#ifndef __common_bgq_Memregion_h__
#define __common_bgq_Memregion_h__

#include <stdlib.h>
#include <string.h>

#include <spi/include/kernel/memory.h>

#include "common/MemregionInterface.h"
#include "util/common.h"

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace PAMI
{
  class Memregion : public Interface::Memregion<Memregion>
  {
    public:
      inline Memregion () :
          Interface::Memregion<Memregion> ()
      {
      }

      inline pami_result_t createMemregion_impl (size_t   * bytes_out,
                                                 size_t     bytes_in,
                                                 void     * base,
                                                 uint64_t   options)
      {
        PAMI_assert(base != NULL);

        Kernel_MemoryRegion_t memregion; // Memory region associated with the buffer.
        //Kernel_MemoryRegion_t globmemregion; // Memory region associated with the buffer.

        // Determine the physical address of the source buffer.
        uint32_t rc;
        rc = Kernel_CreateMemoryRegion (&memregion, base, bytes_in);
        PAMI_assert ( rc == 0 );

        uint64_t offset = (uint64_t)base - (uint64_t)memregion.BaseVa;
        TRACE_ERR((stderr, "Memregion::createDmaMemregion_impl() .. base = %p, memregion.BaseVa = %p, memregion.BasePa = %p, offset = %zu (%p)\n", base, memregion.BaseVa, memregion.BasePa, offset, (void *)offset));

        *bytes_out = bytes_in;

        //rc = Kernel_CreateGlobalMemoryRegion (&memregion, &globmemregion);
        //printf("in create, my global va:%p\n", globmemregion.BaseVa);

        _paddr = (uint64_t)memregion.BasePa + offset;
        TRACE_ERR((stderr, "Memregion::createDmaMemregion_impl() .. _paddr = %zu (%p)\n", (size_t)_paddr, (void *)_paddr));

        return PAMI_SUCCESS;

      }

      inline pami_result_t destroyMemregion_impl ()
      {
        return PAMI_SUCCESS;;
      }

      inline uint64_t getBasePhysicalAddress ()
      {
        return _paddr;
      }

    private:

      uint64_t _paddr;

  }; // end PAMI::Memregion::Memregion
}; // end namespace PAMI
#undef TRACE_ERR
#endif // __components_memregion_bgq_bgqmemregion_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
