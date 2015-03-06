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
/// \file components/devices/bgq/mu2/model/CNShmemColl.h
/// \brief ???
///
#ifndef __components_devices_bgq_mu2_model_CNShmemColl_h__
#define __components_devices_bgq_mu2_model_CNShmemColl_h__

#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/model/Collective2DeviceBase.h"
#include "components/devices/bgq/mu2/model/MU_Collective_OP_DT_Table.h"
#include "sys/pami.h"
#include "components/devices/bgq/mu2/msg/CNShmemMcomb.h"
#include "components/devices/bgq/mu2/msg/CNShmemMcast.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      //Model classes to do the two device protocol..
      static const size_t cn_shmem_state_bytes = 1;
      class CNShmemColl: public Collective2DeviceBase,
          public Interface::MulticombineModel < CNShmemColl, MU::Context, cn_shmem_state_bytes >,
          public Interface::MulticastModel < CNShmemColl, MU::Context, cn_shmem_state_bytes >
      {
        public:
          static const size_t sizeof_msg = cn_shmem_state_bytes;

          CNShmemColl (pami_client_t    client,
                                          pami_context_t   context,
                                          MU::Context                 & device,
                                          pami_result_t               & status) :
              Collective2DeviceBase(device, status),
              Interface::MulticombineModel<CNShmemColl, MU::Context, cn_shmem_state_bytes>  (device, status),
              Interface::MulticastModel<CNShmemColl, MU::Context, cn_shmem_state_bytes>  (device, status)
          {

          }

          pami_result_t postMulticombineImmediate_impl(size_t                   client,
                                                       size_t                   context,
                                                       pami_multicombine_t    * mcombine,
                                                       void                   * devinfo = NULL)
          {
            //PAMI_assertf(PAMI_DOUBLE == mcombine->dtype, "Unsupported data type %x\n", mcombine->dtype);
            unsigned sizeoftype =  mu_collective_size_table[mcombine->dtype];
            unsigned bytes      =  mcombine->count * sizeoftype;
            unsigned op = mu_collective_op_table[mcombine->dtype][mcombine->optor];
            pami_op opcode = mcombine->optor;
            pami_dt dt  = mcombine->dtype;

            //op = mu_collective_op_table[PAMI_DOUBLE][PAMI_MAX];
            if (op == unsupported_operation)
              PAMI_abortf("Unsupported operation %X\n", mcombine->optor);
              //return PAMI_ERROR; //Unsupported operation

            unsigned classroute = 0;

            PAMI_assert(devinfo);
            classroute = ((uint32_t)(uint64_t)devinfo) - 1;

            PipeWorkQueue *spwq = (PipeWorkQueue *) mcombine->data;
            PipeWorkQueue *dpwq = (PipeWorkQueue *) mcombine->results;
            
            //printf("taking cnshmem path\n");
            /*return Collective2DeviceBase::postCollective (bytes,
                                                           spwq,
                                                           dpwq,
                                                           mcombine->cb_done.function,
                                                           mcombine->cb_done.clientdata,
                                                           op,
                                                           opcode,                                    
                                                           sizeoftype,
                                                          classroute);*/
            CNShmemMcomb *msg = new (&_mcomb_msg) CNShmemMcomb (_mucontext,
                mcombine->cb_done.function,
                mcombine->cb_done.clientdata,
                spwq,
                dpwq,
                bytes,
                op,
                opcode,
                dt,
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
                PAMI::Device::Generic::GenericThread *work = new (&_work) PAMI::Device::Generic::GenericThread(advanceMcomb, msg);
                _mucontext.getProgressDevice()->postThread(work);
              }
              return PAMI_SUCCESS;
            }

            if ((__global.mapping.tSize() != 64) && (__global.mapping.tSize() != 32))
            {
              flag = msg->advance_large_latency();

              if (!flag)
              {
                PAMI::Device::Generic::GenericThread *work = new (&_work) PAMI::Device::Generic::GenericThread(advance_large, msg);
                _mucontext.getProgressDevice()->postThread(work);
              }
            }
            else
            {
              if (__global.mapping.tSize() == 32)
              {
                flag = msg->advance_large_32procs_latency();

                if (!flag)
                {
                  PAMI::Device::Generic::GenericThread *work = 
                    new (&_work) PAMI::Device::Generic::GenericThread(advance_large_32procs, msg);
                  _mucontext.getProgressDevice()->postThread(work);
                }
              }
              else
              {
                flag = msg->advance_large_64procs_latency();

                if (!flag)
                {
                  PAMI::Device::Generic::GenericThread *work = 
                    new (&_work) PAMI::Device::Generic::GenericThread(advance_large_64procs, msg);
                  _mucontext.getProgressDevice()->postThread(work);
                }
              }
            }

            return PAMI_SUCCESS;
          }

          static pami_result_t advanceMcomb (pami_context_t     context,
                                        void             * cookie)
          {
            CNShmemMcomb *msg = (CNShmemMcomb *) cookie;
            bool done = msg->advance();

            if (done)
              return PAMI_SUCCESS;

            return PAMI_EAGAIN;
          }

          static pami_result_t advanceMcast (pami_context_t     context,
                                        void             * cookie)
          {
            CNShmemMcast *msg = (CNShmemMcast *) cookie;
            bool done = msg->advance();

            if (done)
              return PAMI_SUCCESS;

            return PAMI_EAGAIN;
          }

          static pami_result_t advance_large (pami_context_t     context,
                                        void             * cookie)
          {
            CNShmemMcomb *msg = (CNShmemMcomb *) cookie;
            bool done = msg->advance_large();

            if (done)
              return PAMI_SUCCESS;

            return PAMI_EAGAIN;
          }

          static pami_result_t advance_large_64procs (pami_context_t     context,
                                        void             * cookie)
          {
            CNShmemMcomb *msg = (CNShmemMcomb *) cookie;
            bool done = msg->advance_large_64procs();

            if (done)
              return PAMI_SUCCESS;

            return PAMI_EAGAIN;
          }

          static pami_result_t advance_large_32procs (pami_context_t     context,
                                        void             * cookie)
          {
            CNShmemMcomb *msg = (CNShmemMcomb *) cookie;
            bool done = msg->advance_large_32procs();

            if (done)
              return PAMI_SUCCESS;

            return PAMI_EAGAIN;
          }

          pami_result_t postMulticombine_impl(uint8_t (&state)[cn_shmem_state_bytes],
                                              size_t               client,
                                              size_t               context,
                                              pami_multicombine_t *mcombine,
                                              void                *devinfo = NULL)
          {
            return PAMI_ERROR;
          }

          pami_result_t postMulticastImmediate_impl(size_t                client,
              size_t                context,
              pami_multicast_t    * mcast,
              void                * devinfo = NULL)
          { 
            unsigned classroute = 0;

            PAMI_assert(devinfo);
            classroute = ((uint32_t)(uint64_t)devinfo) - 1;

            PipeWorkQueue *spwq = (PipeWorkQueue *) mcast->src;
            PipeWorkQueue *dpwq = (PipeWorkQueue *) mcast->dst;

            PAMI::Topology *root_topo = (PAMI::Topology*)mcast->src_participants;
            size_t  root  = root_topo->index2Rank(0);
            
            CNShmemMcast *msg = new (&_mcast_msg) CNShmemMcast (_mucontext,
                mcast->cb_done.function,
                mcast->cb_done.clientdata,
                spwq,
                dpwq,
                mcast->bytes,
                root,
                &_collstate._colCounter,
                &_shmem_desc[_index],
                _zeroBuf,
                _zeroBytes 
                );

            _index = (++_index)%3;
            
            if (__global.mapping.t() == 0)
            {
              _shmem_desc[_index].reset();

              _collstate._colCounter = mcast->bytes;
              _modeldesc.clone (msg->_desc);
              msg->_desc.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = _collstate.payloadBatID();
              msg->_desc.setClassRoute (classroute);
            }

            msg->init();
      
            bool flag;

            flag = msg->advance_latency();

            if (!flag)
            {
              PAMI::Device::Generic::GenericThread *work = new (&_work) PAMI::Device::Generic::GenericThread(advanceMcast, msg);
              _mucontext.getProgressDevice()->postThread(work);
            }

            return PAMI_SUCCESS;
          }

          pami_result_t postMulticast_impl(uint8_t (&state)[cn_shmem_state_bytes],
              size_t            client,
              size_t            context,
              pami_multicast_t *mcast,
              void             *devinfo = NULL)
          {
            //TRACE_FN_ENTER();
            return PAMI_ERROR;
            //TRACE_FN_EXIT();
          }
    
        protected:
       
        CNShmemMcomb  _mcomb_msg; 
        CNShmemMcast  _mcast_msg; 

      };
    };
  };
};

#endif
