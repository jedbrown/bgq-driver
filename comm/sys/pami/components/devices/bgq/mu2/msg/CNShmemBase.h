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
/// \file components/devices/bgq/mu2/msg/CNShmemBase.h
/// \brief ???
///
#ifndef __components_devices_bgq_mu2_msg_CNShmemBase_h__
#define __components_devices_bgq_mu2_msg_CNShmemBase_h__

#include "spi/include/mu/DescriptorBaseXX.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"
#include "common/bgq/Mapping.h"
#include "components/memory/MemoryAllocator.h"
#include "math/a2qpx/Core_memcpy.h"
#include "Memory.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      //This message class advances both the network and shared memory communication
      class CNShmemBase 
      {
        public:

          ///Default dummy constructor
          CNShmemBase (): _context(*(MU::Context*)NULL) {}

          ///
          /// \brief Inject descriptor(s) into a specific injection fifo
          ///
          /// \param[in] context the MU context for this message
          /// \param[in] fn  completion event fn
          /// \param[in] cookie callback cookie
          /// \param[in] spwq pipeworkqueue that has data to be consumed
          /// \param[in] dpwq pipeworkqueue where data has to be produced
          /// \param[in] length the totaly number of bytes to be transfered
          /// \param[in] op operation identifier
          /// \param[in] sizeoftype the size of the datatype
          /// \param[in] counterAddress address of the counter
          ///
          CNShmemBase (MU::Context         & context,
              pami_event_function   fn,
              void                * cookie,
              PipeWorkQueue       * spwq,
              PipeWorkQueue       * dpwq,
              uint32_t              length,
              volatile uint64_t   * counterAddress,
              PAMI::Device::Shmem::CNShmemDesc       *shmem_desc):
            _context (context),
            _injectedBytes (0),
            _length (length),
            _spwq (spwq),
            _dpwq (dpwq),
            _fn (fn),
            _cookie (cookie),
            _cc (length),
            _counterAddress (counterAddress),
            _shmem_desc(shmem_desc)
          { };

          inline ~CNShmemBase () {};

          MUSPI_DescriptorBase     _desc; 

        protected:

          MU::Context            & _context;
          uint32_t                 _injectedBytes;
          uint32_t                 _length;        
          PipeWorkQueue          * _spwq;
          PipeWorkQueue          * _dpwq;
          pami_event_function      _fn;
          void                   * _cookie;
          uint64_t                 _cc;
          volatile uint64_t      * _counterAddress;
          PAMI::Device::Shmem::CNShmemDesc          * _shmem_desc;

      }; // class     PAMI::Device::MU::CNShmemBase
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#endif // __components_devices_bgq_mu2_msg_CNShmemBase_h__                     
