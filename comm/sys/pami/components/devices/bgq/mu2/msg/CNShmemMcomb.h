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
/// \file components/devices/bgq/mu2/msg/CNShmemMcomb.h
/// \brief ???
///
#ifndef __components_devices_bgq_mu2_msg_CNShmemMcomb_h__
#define __components_devices_bgq_mu2_msg_CNShmemMcomb_h__

/*#include "spi/include/mu/DescriptorBaseXX.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/InjChannel.h"
#include "components/devices/bgq/mu2/msg/MessageQueue.h"
#include "common/bgq/Mapping.h"
#include "components/memory/MemoryAllocator.h"
#include "math/a2qpx/Core_memcpy.h"
#include "Memory.h"*/
#include "components/devices/bgq/mu2/msg/CNShmemBase.h"
#include "components/devices/shmem/msgs/CNShmemMessage.h"


namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      //This message class advances both the network and shared memory communication
      class CNShmemMcomb: public CNShmemBase 
      {
        public:

          ///Default dummy constructor
          CNShmemMcomb () {}

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
          CNShmemMcomb (MU::Context         & context,
              pami_event_function   fn,
              void                * cookie,
              PipeWorkQueue       * spwq,
              PipeWorkQueue       * dpwq,
              uint32_t              length,
              uint32_t              op,
              pami_op               opcode,
              pami_dt               dt,
              uint32_t              sizeoftype,
              volatile uint64_t   * counterAddress,
              PAMI::Device::Shmem::CNShmemDesc       *shmem_desc):
          CNShmemBase (context, fn, cookie, spwq, dpwq,length, counterAddress,shmem_desc),
              _op (op),
              _opcode(opcode),
              _dt(dt),
              _sizeoftype(sizeoftype),
              _doneMULarge (false),
              _doneMUShort (false),
              _doneShmemMcomb(0),
              _doneShmemMcombLarge(0),
              _doneShmemMcastLarge(0),
              _shmsg(shmem_desc,  length),
              _combineDone(false)
          { };

          inline ~CNShmemMcomb () {};

          // Create all the physical and global virtual addresses used
          // for the operation

          void init()
          {
            const uint64_t 	alignment = 64;
            uint64_t	mask	= 0;
            mask = (alignment - 1);

            char * rcvbuf = NULL, * srcbuf = NULL;
            uint64_t rcvbuf_phy=0, srcbuf_phy=0;
            rcvbuf = (char*)_dpwq->bufferToProduce(); 
            PAMI_assert (rcvbuf != NULL);
            //PAMI_assert(((uint64_t)rcvbuf & (uint64_t)mask) == 0);
            srcbuf = (char*)_spwq->bufferToConsume();
            PAMI_assert (srcbuf != NULL);
            //PAMI_assert(((uint64_t)srcbuf & (uint64_t)mask) == 0);

            Kernel_MemoryRegion_t memRegion;
            uint32_t rc;

            rc = Kernel_CreateMemoryRegion (&memRegion, _shmem_desc->get_buffer(), ShmBufSize);
            PAMI_assert ( rc == 0 );
            _shmbuf_phy = (uint64_t)memRegion.BasePa +
              ((uint64_t)_shmem_desc->get_buffer() - (uint64_t)memRegion.BaseVa);

            if (PAMI_DOUBLE == _dt)
            { 
              if (_length < VERY_SHORT_MSG_CUTOFF)
              {
                if (_length <= 32)
                {
                  char* buf = (char*) _shmem_desc->get_buffer();
                  memcpy((void*)&buf[_length*__global.mapping.t()], srcbuf, _length);
                }
                else
                {
                  void* buf = _shmem_desc->get_buffer(__global.mapping.t());
                  memcpy(buf, srcbuf, _length);
                }
              }
              else
              {
                rc = Kernel_CreateMemoryRegion (&memRegion, rcvbuf, _length);
                PAMI_assert ( rc == 0 );
                rcvbuf_phy = (uint64_t)memRegion.BasePa +
                  ((uint64_t)rcvbuf - (uint64_t)memRegion.BaseVa);
                void *rcvbuf_gva = NULL;
                rc = Kernel_Physical2GlobalVirtual ((void*)rcvbuf_phy, &rcvbuf_gva);


                rc = Kernel_CreateMemoryRegion (&memRegion, srcbuf, _length);
                PAMI_assert ( rc == 0 );
                srcbuf_phy = (uint64_t)memRegion.BasePa +
                  ((uint64_t)srcbuf - (uint64_t)memRegion.BaseVa);
                void *srcbuf_gva = NULL;
                rc = Kernel_Physical2GlobalVirtual ((void*)srcbuf_phy, &srcbuf_gva);

                _shmsg.init((void*)srcbuf, (void*)rcvbuf, (void*)srcbuf_gva, (void*)rcvbuf_gva, (void*)rcvbuf_phy, (void*)_shmbuf_phy, __global.mapping.t(), _opcode, _dt);

              }

            }
            else
            {
              if (_length <= 32)
              {
                char* buf = (char*) _shmem_desc->get_buffer();
                memcpy((void*)&buf[_length*__global.mapping.t()], srcbuf, _length);
              }
              else if (_length <= SHORT_MSG_CUTOFF/__global.mapping.tSize())
              {
                void* buf = _shmem_desc->get_buffer(__global.mapping.t());
                memcpy(buf, srcbuf, _length);
              }
              else
              {
                printf("unsupported data type\n");
                PAMI_abort();
              }
            }

            if (__global.mapping.t() == 0)
            {
              _desc.setOpCode (_op);
              _desc.setWordLength (_sizeoftype);
              if (_length <= SHORT_MSG_CUTOFF) 
                _context.setCNShmemCollectiveBufferBatEntry((uint64_t)_shmbuf_phy);
              else
                _context.setCNShmemCollectiveBufferBatEntry((uint64_t)rcvbuf_phy);
            }

            Memory::sync();
            _shmem_desc->signal_arrived(); //signal that I have copied all my addresses/data
          }

        //Short MU advance..gets done after a single injection
          bool advanceMUShort (void* next_inj_buf, uint64_t bytes_available)
          {
            if (_doneMUShort)
              return true;

            if (next_inj_buf == NULL)
              return false;

            _desc.Pa_Payload  = (uint64_t)next_inj_buf;
            _desc.Message_Length = bytes_available;

            size_t                fnum    = _context.getCombiningInjFifo();
            InjChannel & channel = _context.injectionGroup.channel[fnum];
            size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

            if (ndesc > 0)
            {
              MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();
              _desc.clone (*d);
              channel.injFifoAdvanceDesc ();
              _doneMUShort =  true;
            }
            else
              return false;

            _injectedBytes  += bytes_available;
            _desc.setRecPutOffset(_injectedBytes);

            return true;
          }

        //Large MU advance..updates state after each injection. 
          bool advanceMULarge (void* next_inj_buf, uint64_t bytes_available, uint64_t offset_b = 0)
          {

            if (_doneMULarge)
              return true;

            if (next_inj_buf == NULL)
              return false;

            _desc.Pa_Payload  = (uint64_t)next_inj_buf + offset_b;
            _desc.Message_Length = bytes_available;

            size_t                fnum    = _context.getCombiningInjFifo();
            InjChannel & channel = _context.injectionGroup.channel[fnum];
            size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

            if (ndesc > 0)
            {
              MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();
              _desc.clone (*d);
              
              channel.injFifoAdvanceDesc ();
              _shmsg.injection_complete();
            }
            else
              return false;

            _injectedBytes  += bytes_available;
            _desc.setRecPutOffset(_injectedBytes);

            _doneMULarge = (_injectedBytes == _length);

            return _doneMULarge;
          }

         bool advanceMULarge ()
          {
            void* next_inj_buf = NULL;
            uint64_t  bytes_available;

            if (_doneMULarge)
              return true;

            next_inj_buf = _shmsg.next_injection_buffer(&bytes_available, _length, __global.mapping.tSize());

            while (next_inj_buf != NULL)
            {
              _desc.Pa_Payload  = (uint64_t)next_inj_buf;
              _desc.Message_Length = bytes_available;

              size_t                fnum    = _context.getCombiningInjFifo();
              InjChannel & channel = _context.injectionGroup.channel[fnum];
              size_t ndesc = channel.getFreeDescriptorCountWithUpdate ();

              if (ndesc > 0)
              {
                MUSPI_DescriptorBase * d = (MUSPI_DescriptorBase *) channel.getNextDescriptor ();
                _desc.clone (*d);

                channel.injFifoAdvanceDesc ();
                _shmsg.injection_complete();
                _injectedBytes  += bytes_available;
                _desc.setRecPutOffset(_injectedBytes);
              }
              else
                break;

              next_inj_buf = _shmsg.next_injection_buffer(&bytes_available, _length, __global.mapping.tSize());
            }

            _doneMULarge = (_injectedBytes == _length);

            return _doneMULarge;
          }




        //Short message Mcombine..uses shmem and shaddr protocols to access the data buffers
          bool advanceShmemMcombShort(unsigned length, bool &combineDone)
          {
            if (_doneShmemMcomb) return true;

            pami_result_t res;

            if (PAMI_DOUBLE == _dt)
            {
              if (length < VERY_SHORT_MSG_CUTOFF)
                res = PAMI::Device::Shmem::CNShmemMessage::very_short_msg_combine(_shmem_desc, length, _opcode, _dt, __global.mapping.tSize(), __global.mapping.t(), combineDone);
              else
                res = _shmsg.short_msg_combine(length, __global.mapping.tSize(), __global.mapping.t(), combineDone);
            }
            else
              res = PAMI::Device::Shmem::CNShmemMessage::very_short_msg_combine(_shmem_desc, length, _opcode, _dt, __global.mapping.tSize(), __global.mapping.t(), combineDone);
            
            if (res == PAMI_SUCCESS)
            {
              _doneShmemMcomb = 1;
              return true;
            }

            return false;
          }

          bool advanceShmemMcastShort()
          {

            pami_result_t res;
            if (PAMI_DOUBLE == _dt)
            {
              if (_length < VERY_SHORT_MSG_CUTOFF)
                res = PAMI::Device::Shmem::CNShmemMessage::very_short_msg_multicast(_shmem_desc, _dpwq, _length, 
                    __global.mapping.tSize(), __global.mapping.t(), (uint64_t*)_counterAddress, _cc);
              else
                res = _shmsg.short_msg_multicast(_length, __global.mapping.tSize(), __global.mapping.t(),
                    (uint64_t*)_counterAddress, _cc);
            }
            else
              res = PAMI::Device::Shmem::CNShmemMessage::very_short_msg_multicast(_shmem_desc, _dpwq, _length, 
                  __global.mapping.tSize(), __global.mapping.t(), (uint64_t*)_counterAddress, _cc);
            

            if (res == PAMI_SUCCESS)
            {
              if (_fn)
                _fn (NULL, _cookie, PAMI_SUCCESS);

              _dpwq->produceBytes(_length);
              return true;
            }

            return false;
          }

        //advance posted for short messages..calls all the short message combines and multicasts
          bool advance()
          {
            bool flag;

            flag = false;
            void* next_inj_buf = NULL;

              if (__global.mapping.t() == 0)
              {
                flag = advanceShmemMcombShort(_length, _combineDone);
                if (_combineDone)
                  next_inj_buf = (void*)_shmbuf_phy;
                flag = flag && advanceMUShort(next_inj_buf, _length);
                flag = flag && advanceShmemMcastShort();
              }
              else
              {
                flag =  advanceShmemMcombShort(_length, _combineDone);
                flag = flag && advanceShmemMcastShort();
              }

            return flag;
          }

        //advance posted for short messages..calls all the short message combines and multicasts
          bool advance_latency()
          {
            bool flag;

            flag = false;
            void* next_inj_buf = NULL;

            register  uint64_t  count = 0;
            while ((flag == false) && (++count < 1024)){
              if (__global.mapping.t() == 0)
              {
                flag = advanceShmemMcombShort(_length, _combineDone);
                if (_combineDone)
                  next_inj_buf = (void*)_shmbuf_phy;
                flag = flag && advanceMUShort(next_inj_buf, _length);
                flag = flag && advanceShmemMcastShort();
              }
              else
              {
                flag =  advanceShmemMcombShort(_length, _combineDone);
                flag = flag && advanceShmemMcastShort();
              }

            }
            return flag;
          }
          //Large message combine..uses shaddr exclusively
          bool advanceShmemMcombLarge(unsigned length, unsigned offset_dbl=0)
          {
            if (_doneShmemMcombLarge) return true;

            pami_result_t res;

            res = _shmsg.large_msg_combine_peers(length, __global.mapping.tSize(), __global.mapping.t(), offset_dbl);
            if (res == PAMI_SUCCESS)
            {
              _doneShmemMcombLarge = 1;
              return true;
            }

            return false;
          }

          //Large message multicast..uses shaddr exclusively
          bool advanceShmemMcastLarge()
          {

            if (_doneShmemMcastLarge) return true;

            pami_result_t res;

            res = _shmsg.large_msg_multicast(_length, __global.mapping.tSize(), __global.mapping.t(), _counterAddress, _cc);
            if (res == PAMI_SUCCESS)
            {
              if (_fn)
                _fn (NULL, _cookie, PAMI_SUCCESS);

              _dpwq->produceBytes(_length);
              _doneShmemMcastLarge = 1;
              return true;
            }

            return false;
          }

          ///
          /// \brief Continuing (fair - no loop) large message advances..uses large message multicast/multicombine routines
          ///
          bool advance_large()
          {
            bool flag;

            flag = false;
            //uint64_t  bytes_available =0;
            //register  uint64_t  count = 0;

              if (__global.mapping.t() == 0)
              {
                //void* next_inj_buf = _shmsg.next_injection_buffer(&bytes_available, _length, __global.mapping.tSize());
                //flag = advanceMULarge(next_inj_buf, bytes_available);
                //flag = flag && advanceShmemMcastLarge();
                advanceMULarge();
                flag = advanceShmemMcastLarge() ;

              }
              else
              {
                flag =  advanceShmemMcombLarge(_length);
                flag = flag && advanceShmemMcastLarge();
              }

            return flag;
          }

          bool advance_large_64procs()
          {
            bool flag;

            flag = false;
            //uint64_t  bytes_available =0;
            //register  uint64_t  count = 0;

              if (__global.mapping.t() == 0)
              {
                advanceMULarge();
                flag = advanceShmemMcastLarge() ;
              }
              else if (__global.mapping.t()%4 == 0)
              {
                flag =  advanceShmemMcombLarge(_length);
                flag = flag && advanceShmemMcastLarge();
              }
              else
              {   
                flag = advanceShmemMcastLarge();
              }
            return flag;
          }

          bool advance_large_32procs()
          {
            bool flag;

            flag = false;
            //uint64_t  bytes_available =0;
            //register  uint64_t  count = 0;

              if (__global.mapping.t() == 0)
              {
                advanceMULarge();
                flag = advanceShmemMcastLarge() ;
              }
              else if (__global.mapping.t()%2 == 0)
              {
                flag =  advanceShmemMcombLarge(_length);
                flag = flag && advanceShmemMcastLarge();
              }
              else
              {   
                flag = advanceShmemMcastLarge();
              }
            return flag;
          }

          ///
          /// \brief Initial (latency optimized) large message advances..uses large message multicast/multicombine routines
          ///
          bool advance_large_latency()
          {
            bool flag;

            flag = false;
            //uint64_t  bytes_available =0;
            //register  uint64_t  count = 0;

            register  uint64_t  count = 0;
            while ((flag == false) && (++count < 1024)){
              if (__global.mapping.t() == 0)
              {
                //void* next_inj_buf = _shmsg.next_injection_buffer(&bytes_available, _length, __global.mapping.tSize());
                //flag = advanceMULarge(next_inj_buf, bytes_available);
                //flag = flag && advanceShmemMcastLarge();
                advanceMULarge();
                flag = advanceShmemMcastLarge() ;

              }
              else
              {
                flag =  advanceShmemMcombLarge(_length);
                flag = flag && advanceShmemMcastLarge();
              }

            }
            return flag;
          }

          bool advance_large_64procs_latency()
          {
            bool flag;

            flag = false;
            //uint64_t  bytes_available =0;
            //register  uint64_t  count = 0;

            register  uint64_t  count = 0;
            while ((flag == false) && (++count < 1024)){
              if (__global.mapping.t() == 0)
              {
                advanceMULarge();
                flag = advanceShmemMcastLarge() ;
              }
              else if (__global.mapping.t()%4 == 0)
              {
                flag =  advanceShmemMcombLarge(_length);
                flag = flag && advanceShmemMcastLarge();
              }
              else
              {   
                flag = advanceShmemMcastLarge();
              }
            }
            return flag;
          }

          bool advance_large_32procs_latency()
          {
            bool flag;

            flag = false;
            //uint64_t  bytes_available =0;
            //register  uint64_t  count = 0;

            register  uint64_t  count = 0;
            while ((flag == false) && (++count < 1024)){
              if (__global.mapping.t() == 0)
              {
                advanceMULarge();
                flag = advanceShmemMcastLarge() ;
              }
              else if (__global.mapping.t()%2 == 0)
              {
                flag =  advanceShmemMcombLarge(_length);
                flag = flag && advanceShmemMcastLarge();
              }
              else
              {   
                flag = advanceShmemMcastLarge();
              }
            }
            return flag;
          }


        protected:

          uint32_t                 _op;
          pami_op                 _opcode;
          pami_dt                 _dt;
          uint32_t                 _sizeoftype;
          bool                     _doneMULarge;
          bool                     _doneMUShort;
          bool                     _doneShmemMcomb;
          bool                     _doneShmemMcombLarge;
          bool                     _doneShmemMcastLarge;
          Shmem::CNShmemMessage   _shmsg;
          bool                      _combineDone;
          uint64_t                _shmbuf_phy;

      }; // class     PAMI::Device::MU::CNShmemMcomb
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI

#endif // __components_devices_bgq_mu2_msg_CNShmemMcomb_h__                     
