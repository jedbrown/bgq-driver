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
/// \file components/devices/bgq/mu2/msg/CNShmemMcast.h
/// \brief ???
///
#ifndef __components_devices_bgq_mu2_msg_CNShmemMcast_h__
#define __components_devices_bgq_mu2_msg_CNShmemMcast_h__

/*#include "spi/include/mu/DescriptorBaseXX.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"
#include "common/bgq/Mapping.h"
#include "components/memory/MemoryAllocator.h"
#include "math/a2qpx/Core_memcpy.h"
#include "Memory.h"*/
#include "components/devices/bgq/mu2/msg/CNShmemBase.h"
#include "components/devices/shmem/msgs/CNShmemMcastMessage.h"


namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      //This message class advances both the network and shared memory communication
      class CNShmemMcast: public CNShmemBase 
      {
        public:

          ///Default dummy constructor
          CNShmemMcast () {}

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
          CNShmemMcast (MU::Context         & context,
              pami_event_function   fn,
              void                * cookie,
              PipeWorkQueue       * spwq,
              PipeWorkQueue       * dpwq,
              uint32_t              length,
              size_t                root,
              volatile uint64_t   * counterAddress,
              PAMI::Device::Shmem::CNShmemDesc       *shmem_desc,
              void                * zerobuf,
              unsigned              zbytes
              ):
          CNShmemBase (context, fn, cookie, spwq, dpwq,length, counterAddress,shmem_desc),
              _root(root),
              _doneMU (false),
              _doneShmemMcast(0),
              _shmsg(shmem_desc,  length),
              _zerobuf(zerobuf),
              _zbytes(zbytes)
          { };

          inline ~CNShmemMcast () {};

          // Create all the physical and global virtual addresses used
          // for the operation

          void init()
          {
            char * rcvbuf = NULL, * srcbuf = NULL;
            uint64_t rcvbuf_phy=0, srcbuf_phy=0;
            rcvbuf = (char*)_dpwq->bufferToProduce(); 
            PAMI_assert (rcvbuf != NULL);
            srcbuf = (char*)_spwq->bufferToConsume();
            PAMI_assert (srcbuf != NULL);
              

            if (__global.mapping.task() == _root)
              rcvbuf  = srcbuf;
            else
              srcbuf  = rcvbuf;

            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;

            _shmsg.init(srcbuf, rcvbuf, __global.mapping.t());

            if (__global.mapping.t()==0)
            {
              rc = Kernel_CreateMemoryRegion (&memRegion, rcvbuf, _length);
              PAMI_assert ( rc == 0 );
              rcvbuf_phy = (uint64_t)memRegion.BasePa +
                ((uint64_t)rcvbuf - (uint64_t)memRegion.BaseVa);
              void *rcvbuf_gva = NULL;
              rc = Kernel_Physical2GlobalVirtual ((void*)rcvbuf_phy, &rcvbuf_gva);
              _shmsg.set_rcvbuf_master((void*)rcvbuf_gva); 

              _context.setCNShmemCollectiveBufferBatEntry((uint64_t)rcvbuf_phy);

              if (!(__global.mapping.isPeer(__global.mapping.task(), _root))) //masters on non-root nodes
              {
                rc = Kernel_CreateMemoryRegion (&memRegion, _zerobuf, _zbytes);
                PAMI_assert ( rc == 0 );
                srcbuf_phy = (uint64_t)memRegion.BasePa +
                  ((uint64_t)_zerobuf - (uint64_t)memRegion.BaseVa);
                //_shmsg.set_srcbuf_mcast((void*)srcbuf_phy); 
                _desc.Pa_Payload  = srcbuf_phy;
              }
            }

            if (__global.mapping.task() == _root)
            {
              rc = Kernel_CreateMemoryRegion (&memRegion, srcbuf, _length);
              PAMI_assert ( rc == 0 );
              srcbuf_phy = (uint64_t)memRegion.BasePa +
                ((uint64_t)srcbuf - (uint64_t)memRegion.BaseVa);
              _shmsg.set_srcbuf_mcast((void*)srcbuf_phy); 
            }

            Memory::sync();
            _shmem_desc->signal_arrived(); 
          }



         bool advanceMU ()
          {
            if (_doneMU)
              return true;

            void* next_inj_buf = NULL;

            if (__global.mapping.isPeer(__global.mapping.task(), _root)) //masters on non-root nodes
            {
              next_inj_buf  = _shmsg.get_srcbuf_mcast();
              if (next_inj_buf != NULL)
                _desc.Pa_Payload  = (uint64_t)next_inj_buf + _injectedBytes;
              else
                return false;
            }

            uint64_t bytes_available = _length - _injectedBytes;

            if (bytes_available > _zbytes)
              bytes_available = _zbytes;

            _desc.Message_Length = bytes_available;

            //The is computed when the first descriptor for this round is injected
            size_t                fnum    = _context.getCombiningInjFifo();
            InjChannel & channel = _context.injectionGroup.channel[fnum];
            size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

            if (ndesc > 0)
              {
                // Clone the message descriptors directly into the injection fifo.
                MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();

                _desc.clone (*d);
                channel.injFifoAdvanceDesc ();
              }
            //No descriptor slots available, so come back and try later
            else
              return false;

            _injectedBytes  += bytes_available;
            _desc.setRecPutOffset(_injectedBytes);

            _doneMU = (_injectedBytes == _length);

            return _doneMU;
          }

          //Large message multicast..uses shaddr exclusively
          bool advanceShmemMcast()
          {

            if (_doneShmemMcast) return true;

            pami_result_t res;

            res = _shmsg.large_msg_multicast(_length, __global.mapping.tSize(), __global.mapping.t(), _counterAddress, _cc);
            if (res == PAMI_SUCCESS)
            {
              if (_fn)
                _fn (NULL, _cookie, PAMI_SUCCESS);

              _dpwq->produceBytes(_length);
              _doneShmemMcast = 1;
              return true;
            }

            return false;
          }

        //Large message advance..uses large message multicast/multicombine routines
          bool advance()
          {
            bool flag;

            flag = false;

            //while ((flag == false)){
              if (__global.mapping.t() == 0)
              {
                advanceMU();
                flag = advanceShmemMcast() ;
              }
              else
              {
                flag = advanceShmemMcast();
              }

            return flag;
          }

        //Large message advance..uses large message multicast/multicombine routines
          bool advance_latency()
          {
            bool flag;

            flag = false;
            register  uint64_t  count = 0;

            while ((flag == false) && (++count < 1024)){
            //while ((flag == false)){
              if (__global.mapping.t() == 0)
              {
                advanceMU();
                flag = advanceShmemMcast() ;
              }
              else
              {
                flag = advanceShmemMcast();
              }
            }

            return flag;
          }
        protected:

          size_t                    _root;
          bool                     _doneMU;
          bool                     _doneShmemMcast;
          Shmem::CNShmemMcastMessage   _shmsg;
          void*                       _zerobuf;
          unsigned                    _zbytes;
      }; // class     PAMI::Device::MU::CNShmemMcast
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#endif // __components_devices_bgq_mu2_msg_CNShmemMcast_h__                     
