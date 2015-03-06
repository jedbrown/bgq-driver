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
/// \file common/bgp/Memregion.h
/// \brief PAMI BG/P specific memregion implementation.
///
#ifndef __common_bgp_Memregion_h__
#define __common_bgp_Memregion_h__

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <pthread.h>

#include <spi/kernel_interface.h>

#include "Global.h"
#include "common/MemregionInterface.h"
#include "util/common.h"

namespace PAMI
{
  class Memregion : public Interface::Memregion<Memregion>
  {
    public:
      inline Memregion () :
          Interface::Memregion<Memregion> ()
      {
        Interface::Mapping::nodeaddr_t address;
        __global.mapping.nodeAddr (address);
        __global.mapping.node2peer (address, _peer);
      }

      inline pami_result_t createMemregion_impl (size_t   * bytes_out,
                                                 size_t     bytes_in,
                                                 void     * base,
                                                 uint64_t   options)
      {
        PAMI_assert(base != NULL);

        _vaddr = base;

        uint32_t paddr_lower = 0;
        uint32_t paddr_upper = 0;
        int rc;
        rc = Kernel_Virtual2Physical(base, bytes_in, &paddr_upper, &paddr_lower );
        PAMI_assertf(rc == 0, "%s<%d> Bad return code from Kernel_Virtual2Physical: %d\n", __FILE__, __LINE__, rc);

        _paddr = ((uint64_t) paddr_upper) << 32;
        _paddr |= paddr_lower;

        *bytes_out = bytes_in;

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

      inline void * getBaseVirtualAddress ()
      {
        return _vaddr;
      }

      inline size_t getPeer ()
      {
        return _peer;
      }

    private:

      void     * _vaddr;
      uint64_t   _paddr;
      size_t     _peer;

  }; // end PAMI::Memregion::Memregion
}; // end namespace PAMI

#endif // __common_bgp_Memregion_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
