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
/// \file components/devices/bgq/mu2/model/Collective2DeviceBase.h
/// \brief ???
///
#ifndef __components_devices_bgq_mu2_model_Collective2DeviceBase_h__
#define __components_devices_bgq_mu2_model_Collective2DeviceBase_h__

#include "spi/include/mu/DescriptorBaseXX.h"
#include "components/devices/MulticastModel.h"
#include "components/devices/MulticombineModel.h"
#include "components/memory/MemoryAllocator.h"
#include "util/trace.h"
#include "components/devices/bgq/mu2/Context.h"
//#include "components/devices/bgq/mu2/msg/CollectiveMcomb2Device.h"
#include "components/devices/shmem/CNShmemDesc.h"
#include "sys/pami.h"
#include "math/a2qpx/Core_memcpy.h"


namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      // Has all the intialization data structures including the model descriptors cloned into the
      // injection fifos. Also contains the counters and BAT ids used in the operation
      // Post methods construct the message and post to the generic device
#define BYTES_ZERO 65536
      class Collective2DeviceBase
      {

        public:
          class CollState
          {
            public:
              bool                                _isInited;          /// Has this class been initialized?
              volatile uint64_t                   _colCounter;        /// Counter used in the collective op
              int32_t                             _pBatID;            /// payload bat id
              int32_t                             _cBatID;            /// counter bat id
              uint64_t                            _counter_phy;

              CollState ()
              {
                _isInited = false;
              }

              bool isInited () { return _isInited; }

              int payloadBatID() { return _pBatID; }
              int counterBatID() { return _cBatID; }

              void init (MU::Context &mucontext, pami_result_t &status)
              {
                if (_isInited) return;

                if (__global.mapping.t()==0)
                {

                  _pBatID = mucontext.getCNShmemCollectiveBufferBatId ();

                  if (_pBatID == -1)
                  {
                    status = PAMI_ERROR;
                    return;
                  }

                  _cBatID = mucontext.getCNShmemCollectiveCounterBatId ();

                  if (_cBatID == -1)
                  {
                    status = PAMI_ERROR;
                    return;
                  }

                  Kernel_MemoryRegion_t memRegion;
                  int rc = 0;

                  rc = Kernel_CreateMemoryRegion (&memRegion, (void *) & _collstate._colCounter, sizeof(uint64_t));
                  PAMI_assert ( rc == 0 );
                  uint64_t paddr = (uint64_t)memRegion.BasePa +
                    ((uint64_t)(void *) & _collstate._colCounter - (uint64_t)memRegion.BaseVa);

                  uint64_t atomic_address = MUSPI_GetAtomicAddress(paddr, MUHWI_ATOMIC_OPCODE_STORE_ADD);
                  mucontext.setCNShmemCollectiveCounterBatEntry (atomic_address);
                
                }

                _isInited = true;
                return;
              }
          };


          Collective2DeviceBase ():
              _mucontext(*(MU::Context*)NULL),
              _injChannel (*(InjChannel *)NULL),
              _gdev(*(Generic::Device*)NULL)
          {
          }

          Collective2DeviceBase (MU::Context    & context,
                                  pami_result_t  & status):
              _mucontext(context),
              _injChannel (context.injectionGroup.channel[0]),
              _index(0),
              _gdev(*context.getProgressDevice())
          {
            //Only local task 0 initializes all the resources
            if (__global.mapping.t() == 0)
            {
              _collstate.init(context, status);
              initDescBase();

              _zeroBytes = BYTES_ZERO;
              _zeroBuf =  malloc (_zeroBytes * sizeof(char));
              memset (_zeroBuf, 0, _zeroBytes);
            }

            pami_result_t rc;
            char key[PAMI::Memory::MMKEYSIZE];

            sprintf(key, "/Collective2DeviceBase-shared_desc_array-client-%2.2zu-context-%2.2zu", _gdev.clientId(), _gdev.contextId());

            size_t total_size = sizeof(PAMI::Device::Shmem::CNShmemRegion)* 3; 
            rc = __global.mm.memalign ((void **) & PAMI::Device::Shmem::_cn_shmem_region,
                128,
                total_size,
                key,
                Collective2DeviceBase::shmem_region_initialize,
                NULL);
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate shared memory resources for collective descriptors");

            for (size_t i = 0; i < 3; i++)
            {
              new (&_shmem_desc[i]) PAMI::Device::Shmem::CNShmemDesc(__global.mm, _gdev.clientId(), _gdev.contextId(), 0, i );
            }

          }

          static void shmem_region_initialize (void       * memory,
              size_t       bytes,
              const char * key,
              unsigned     attributes,
              void       * cookie)
          {
            PAMI::Device::Shmem::CNShmemRegion *shm_region = (PAMI::Device::Shmem::CNShmemRegion*) memory;

            size_t  i;
            for (i =0; i < 3;i++)
            {
              new (&shm_region[i]) PAMI::Device::Shmem::CNShmemRegion();
            }
          }
 

          void initDescBase()
          {
            //TRACE_FN_ENTER();
            // Zero-out the descriptor models before initialization
            memset((void *)&_modeldesc, 0, sizeof(_modeldesc));

            // --------------------------------------------------------------------
            // Set the common base descriptor fields
            //
            // For the remote get packet, send it using the high priority torus
            // fifo map.  Everything else uses non-priority torus fifos, pinned
            // later based on destination.  This is necessary to avoid deadlock
            // when the remote get fifo fills.  Note that this is in conjunction
            // with using the high priority virtual channel (set elsewhere).
            // --------------------------------------------------------------------
            MUSPI_BaseDescriptorInfoFields_t base;
            memset((void *)&base, 0, sizeof(base));

            base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
            base.Message_Length  = 0;
            base.Payload_Address = NULL;
            _modeldesc.setBaseFields (&base);

            // --------------------------------------------------------------------
            // Set the common point-to-point descriptor fields
            //
            // For the remote get packet, send it on the high priority virtual
            // channel.  Everything else is on the deterministic virtual channel.
            // This is necessary to avoid deadlock when the remote get fifo fills.
            // Note that this is in conjunction with setting the high priority
            // torus fifo map (set elsewhere).
            // --------------------------------------------------------------------
            MUSPI_CollectiveDescriptorInfoFields_t coll;
            memset((void *)&coll, 0, sizeof(coll));

            //Setup for broadcast
            coll.Op_Code = MUHWI_COLLECTIVE_OP_CODE_OR;
            coll.Word_Length = 4; //preset to doubles and uint8 operations
            coll.Misc =
              MUHWI_PACKET_VIRTUAL_CHANNEL_USER_SUB_COMM |
              MUHWI_COLLECTIVE_TYPE_ALLREDUCE;
            coll.Class_Route = 0;
            coll.Skip = 8;

            _modeldesc.setCollectiveFields (&coll);
            _modeldesc.setDataPacketType (MUHWI_COLLECTIVE_DATA_PACKET_TYPE);
            _modeldesc.PacketHeader.NetworkHeader.collective.Byte8.Size = 16;
            _modeldesc.setTorusInjectionFIFOMap(MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CUSER);

            // --------------------------------------------------------------------
            // Set the direct put descriptor fields
            // --------------------------------------------------------------------
            MUSPI_DirectPutDescriptorInfoFields dput;
            memset((void *)&dput, 0, sizeof(dput));

            dput.Rec_Payload_Base_Address_Id = _collstate.payloadBatID();
            dput.Rec_Payload_Offset          = 0;
            dput.Rec_Counter_Base_Address_Id = _collstate.counterBatID();
            dput.Rec_Counter_Offset          = 0;
            dput.Pacing                      = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;

            _modeldesc.setDirectPutFields (&dput);
            _modeldesc.setMessageUnitPacketType (MUHWI_PACKET_TYPE_PUT);
            //TRACE_FN_EXIT();
          }
#if 0
          ///contructing the message and posting to the generice device
          //Different advances are posted depending on the message size
          pami_result_t postCollective (uint32_t                   bytes,
                                        PAMI::PipeWorkQueue      * src,
                                        PAMI::PipeWorkQueue      * dst,
                                        pami_event_function        cb_done,
                                        void                     * cookie,
                                        uint32_t                   op,
                                        pami_op                   opcode,
                                        uint32_t                   sizeoftype,
                                        unsigned                   classroute)
          {

            CollectiveDputMcomb2Device *msg = new (&_mcomb_msg) CollectiveDputMcomb2Device (_mucontext,
                cb_done,
                cookie,
                src,
                dst,
                bytes,
                op,
                opcode,
                sizeoftype,
                &_collstate._colCounter,
                &_shmem_desc[_index]
                );

            _index = (++_index)%3;

            if (__global.mapping.t() == 0)
            {
              _shmem_desc[_index].reset();

              _collstate._colCounter = bytes;
              _modeldesc.clone (msg->_desc);
              msg->_desc.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _collstate.payloadBatID();
              msg->_desc.setClassRoute (classroute);
            }

            msg->init();
      
            bool flag;
            if (bytes <= SHORT_MSG_CUTOFF)
            { 
              flag = msg->advance_latency();

              if (!flag)
              {
                PAMI::Device::Generic::GenericThread *work = new (&_work) PAMI::Device::Generic::GenericThread(advance, msg);
                _mucontext.getProgressDevice()->postThread(work);
              }
              return PAMI_SUCCESS;
            }

            if (__global.mapping.tSize() != 64)
            {
              flag = msg->advance_large();

              if (!flag)
              {
                PAMI::Device::Generic::GenericThread *work = new (&_work) PAMI::Device::Generic::GenericThread(advance_large, msg);
                _mucontext.getProgressDevice()->postThread(work);
              }
            }
            else
            {
              flag = msg->advance_large_64procs();

              if (!flag)
              {
                PAMI::Device::Generic::GenericThread *work = 
                    new (&_work) PAMI::Device::Generic::GenericThread(advance_large_64procs, msg);
                _mucontext.getProgressDevice()->postThread(work);
              }
            }
            return PAMI_SUCCESS;
          }


          static pami_result_t advance (pami_context_t     context,
                                        void             * cookie)
          {
            CollectiveDputMcomb2Device *msg = (CollectiveDputMcomb2Device *) cookie;
            bool done = msg->advance();

            if (done)
              return PAMI_SUCCESS;

            return PAMI_EAGAIN;
          }

          static pami_result_t advance_large (pami_context_t     context,
                                        void             * cookie)
          {
            CollectiveDputMcomb2Device *msg = (CollectiveDputMcomb2Device *) cookie;
            bool done = msg->advance_large();

            if (done)
              return PAMI_SUCCESS;

            return PAMI_EAGAIN;
          }
#endif
          static CollState                            _collstate;

        protected:
          MU::Context                                & _mucontext;         /// Pointer to MU context
          InjChannel                                 & _injChannel;
          PAMI::Device::Shmem::CNShmemDesc          _shmem_desc[3];
          unsigned                                    _index;
          Generic::Device                            & _gdev;
          MUSPI_DescriptorBase                       _modeldesc;         /// Model descriptor
          pami_work_t                                _swork;
          pami_work_t                                _work;
          size_t                                      _myrank;
          void                                     * _zeroBuf;
          unsigned                                   _zeroBytes;
//          CollectiveDputMcomb2Device                 _mcomb_msg;
      };


    };
  };
};

#endif
