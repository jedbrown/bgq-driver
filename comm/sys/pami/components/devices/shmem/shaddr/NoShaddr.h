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
 * \file components/devices/shmem/shaddr/NoShaddr.h
 * \brief ???
 */

#ifndef __components_devices_shmem_shaddr_NoShaddr_h__
#define __components_devices_shmem_shaddr_NoShaddr_h__

#include <pami.h>
#include "components/devices/shmem/shaddr/ShaddrInterface.h"

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
      class NoShaddr : public ShaddrInterface<NoShaddr>
      {
        public:

          static const bool shaddr_va_supported    = false;
          static const bool shaddr_mr_supported    = false;

          static const bool shaddr_read_supported  = false;
          static const bool shaddr_write_supported = false;

          template <class T_Device> 
          inline void initialize_impl (T_Device * device) 
          {};

          inline bool isEnabled_impl () { return true; };

          ///
          /// \brief Shared address read operation using virtual addresses
          ///
          /// \see ShaddrInterface::read
          ///
          inline size_t read_impl (void   * local,
                                   void   * remote,
                                   size_t   bytes,
                                   size_t   task)
          {
            (void)local;(void)remote;
            (void)bytes;(void)task;
            return 0;
          };

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
            (void)local;(void)remote;
            (void)bytes;(void)task;
            return 0;
          };

          ///
          /// \brief Shared address read operation using memory regions
          ///
          /// \see ShaddrInterface::read
          ///
          inline size_t read_impl (Memregion * local,
                                   size_t      local_offset,
                                   Memregion * remote,
                                   size_t      remote_offset,
                                   size_t      bytes)
          {
            (void)local;(void)local_offset;(void)remote;
            (void)remote_offset;(void)bytes;
            return 0;
          };

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
            (void)local;(void)local_offset;(void)remote;
            (void)remote_offset;(void)bytes;
            return 0;
          };

      };  // PAMI::Device::Shmem::NoShaddr class
    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace

#endif /* __components_devices_shmem_shaddr_NoShaddr_h__ */
