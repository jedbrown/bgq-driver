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
 * \file components/devices/shmem/shaddr/BgqShaddrReadOnly.h
 * \brief ???
 */

#ifndef __components_devices_shmem_shaddr_BgqShaddrReadOnly_h__
#define __components_devices_shmem_shaddr_BgqShaddrReadOnly_h__

#include "components/devices/shmem/shaddr/BgqShaddr.h"
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
      class BgqShaddrReadOnly : public BgqShaddr, public ShaddrInterface<BgqShaddrReadOnly>
      {
        public:

          static const bool shaddr_write_supported = false;

          inline BgqShaddrReadOnly () :
              BgqShaddr (),
              ShaddrInterface<BgqShaddrReadOnly> ()
          {
          };

          inline ~BgqShaddrReadOnly () {};

          ///
          /// \brief Shared address write operation using virtual addresses
          ///
          /// \see ShaddrInterface::write
          ///
          inline size_t write_impl (void   * remote,
                                    void   * local,
                                    size_t   bytes,
                                    size_t   task)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
            return 0;
          }

          ///
          /// \brief Shared address write operation using memory regions
          ///
          /// \see ShaddrInterface::write
          ///
          inline size_t write_impl (Memregion * remote,
                                    size_t      remote_offset,
                                    Memregion * local,
                                    size_t      local_offset,
                                    size_t      bytes)
          {
            PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
            return 0;
          }
      };  // PAMI::Device::Shmem::BgqShaddrReadOnly class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace

#undef TRACE_ERR
#endif /* __components_devices_shmem_bgq_BgqShaddrReadOnly_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
