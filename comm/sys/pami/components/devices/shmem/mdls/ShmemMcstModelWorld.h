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
 * \file components/devices/shmem/mdls/ShmemMcstModelWorld.h
 * \brief ???
 */

#ifndef __components_devices_shmem_mdls_ShmemMcstModelWorld_h__
#define __components_devices_shmem_mdls_ShmemMcstModelWorld_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"
#include "Memory.h"

#include "sys/pami.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/ShmemCollInterface.h"
#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/msgs/BaseMessage.h"
#include "components/devices/shmem/msgs/ShortMcstMessage.h"
#include "components/devices/shmem/msgs/ShaddrMcstMessage.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

//#define SHORT_MCST_OPT (CPS 97XFS2 risky - advances until done but doesn't make progress on other pami work)

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

      template <class T_Device>
        class ShmemMcstModelWorld : public Interface::MulticastModel < ShmemMcstModelWorld<T_Device>, T_Device, sizeof(BaseMessage<T_Device>) >
      {

        public:
          //Shmem Multicast Model
          ShmemMcstModelWorld (T_Device &device, pami_result_t &status) :
            Interface::MulticastModel < ShmemMcstModelWorld<T_Device>, T_Device, sizeof(BaseMessage<T_Device>) > (device, status),
            _device(device),
            _local_rank(__global.topology_local.rank2Index(__global.mapping.task())),
            _npeers(__global.topology_local.size())
        {
          TRACE_FN_ENTER();
          TRACE_FN_EXIT();
        };

          static const size_t packet_model_state_bytes          = sizeof(BaseMessage<T_Device>);
          static const size_t sizeof_msg                        = sizeof(BaseMessage<T_Device>);
          static const size_t short_msg_cutoff                  = 256;

          inline pami_result_t postMulticastImmediate_impl(size_t           client,
							   size_t           context,
							   pami_multicast_t *mcast, void* devinfo)
        {
          TRACE_FN_ENTER();
          TRACE_FN_EXIT();
	    return PAMI_ERROR;
	  }

          inline pami_result_t postMulticast_impl(uint8_t (&state)[sizeof(BaseMessage<T_Device>)],
						  size_t           client,
						  size_t           context,
						  pami_multicast_t *mcast, void* devinfo)
          {

          TRACE_FN_ENTER();
            PAMI::Topology *src_topo = (PAMI::Topology *)mcast->src_participants;
            //PAMI::Topology *dst_topo = (PAMI::Topology *)mcast->dst_participants;
            //size_t num_dst_ranks = dst_topo->size();
           
            unsigned local_root = __global.topology_local.rank2Index(src_topo->index2Rank(0));

            typename T_Device::CollectiveFifo::Descriptor *my_desc = NULL; 
            pami_result_t res =	 _device.getShmemWorldDesc(&my_desc);
            while (res != PAMI_SUCCESS)
            {
              res = _device.getShmemWorldDesc(&my_desc);
              _device.advance();
            }

            if (mcast->bytes <= short_msg_cutoff)
            {

#ifdef SHORT_MCST_OPT
              res = ShortMcstMessage<T_Device>::short_msg_advance(my_desc, mcast, local_root, _local_rank);
              if (res == PAMI_SUCCESS)    
              {
                mcast->cb_done.function(_context, mcast->cb_done.clientdata, PAMI_SUCCESS);
                my_desc->set_my_state(Shmem::DONE);
              }
              my_desc->signal_done();
            TRACE_FN_EXIT();
              return PAMI_SUCCESS;
#else
              my_desc->set_mcast_params(mcast);
              my_desc->set_master(local_root);
              my_desc->set_my_state(Shmem::DESCSTATE_INIT);
              ShortMcstMessage<T_Device> * obj = (ShortMcstMessage<T_Device> *) (&state[0]);
              new (obj) ShortMcstMessage<T_Device> (_device.getContext(), my_desc,_local_rank);

              _device.post_obj((BaseMessage<T_Device> *)obj);
              /*PAMI::Device::Generic::Device &generic = _device.getProgressDevice();
              generic.postThread(&(obj->_work));*/
#endif
            }
            else
            {
              my_desc->set_mcast_params(mcast);
              my_desc->set_master(local_root);

              if (local_root == _local_rank)
              {
                my_desc->signal_done(); //master signals done earlier in this case
                
                Memregion memregion;
                Shmem::McstControl* mcst_control = (Shmem::McstControl*) my_desc->get_buffer();
                void* mybuf = ((PAMI::PipeWorkQueue *)mcast->src)->bufferToConsume();
                size_t bytes_out;

                //? Have to create the entire memregion at once..does mcast->bytes include all the data bytes
                memregion.createMemregion(&bytes_out, mcast->bytes, mybuf, 0);
                PAMI_assert(bytes_out == mcast->bytes);
                void* phy_addr = (void*)memregion.getBasePhysicalAddress();
                void * global_vaddr = NULL;
                uint32_t rc = 0;
                rc = Kernel_Physical2GlobalVirtual (phy_addr, &global_vaddr);
                PAMI_assert(rc == 0);
                TRACE_FORMAT("buffer to consume:va:%p ga:%p", mybuf, global_vaddr);

                mcst_control->glob_src_buffer = global_vaddr;
                mcst_control->incoming_bytes = 0;
                //my_desc->set_consumers(num_dst_ranks);
                my_desc->reset_master_done();
                Memory::sync();

                my_desc->signal_flag();
              }

              ShaddrMcstMessage<T_Device> * obj = (ShaddrMcstMessage<T_Device> *) (&state[0]);
              new (obj) ShaddrMcstMessage<T_Device> ( _device.getContext(), my_desc, _local_rank);
              _device.post_obj((BaseMessage<T_Device> *)obj);
              /*PAMI::Device::Generic::Device &generic = _device.getProgressDevice();
              generic.postThread(&(obj->_work));*/

            }

          TRACE_FN_EXIT();
            return PAMI_SUCCESS;

          };

        protected:
          T_Device      & _device;
          pami_context_t   _context;
          unsigned _local_rank;
          unsigned _npeers;
      };  // PAMI::Device::Shmem::ShmemMcstModelWorld class


    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace
#undef DO_TRACE_ENTEREXIT
#endif // __components_devices_shmem_ShmemPacketModel_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
