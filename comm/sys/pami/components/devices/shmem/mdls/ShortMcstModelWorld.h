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
 * \file components/devices/shmem/mdls/ShortMcstModelWorld.h
 * \brief ???
 */

#ifndef __components_devices_shmem_mdls_ShortMcstModelWorld_h__
#define __components_devices_shmem_mdls_ShortMcstModelWorld_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"

#include "sys/pami.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/ShmemCollInterface.h"
#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/msgs/ShortMcstMessage.h"
//#include "components/devices/shmemcoll/msgs/ShaddrMcstMessage.h"

#undef TRACE_ERR

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); fprintf x
#endif

//#define SHORT_MCST_OPT (CPS 97XFS2 risky - advances until done but doesn't make progress on other pami work)

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

      template <class T_Device, class T_Desc>
        class ShmemMcstModelWorld : public Interface::MulticastModel < ShmemMcstModelWorld<T_Device, T_Desc>, T_Device, sizeof(ShortMcstMessage<T_Device, T_Desc>) >
      {

        public:
          //Shmem Multicast Model
          ShmemMcstModelWorld (T_Device &device, pami_result_t &status) :
            Interface::MulticastModel < ShmemMcstModelWorld<T_Device, T_Desc>, T_Device, sizeof(ShortMcstMessage<T_Device, T_Desc>) > (device, status),
            _device(device),
            _local_rank(__global.topology_local.rank2Index(__global.mapping.task())),
            _npeers(__global.topology_local.size())

        {
          TRACE_ERR((stderr, "registering match dispatch function\n"));
        };

          static const size_t packet_model_state_bytes          = sizeof(ShortMcstMessage<T_Device, T_Desc>);
          static const size_t sizeof_msg                        = sizeof(ShortMcstMessage<T_Device, T_Desc>);
          static const size_t short_msg_cutoff                  = 256;
          //static const size_t short_msg_cutoff                  = 512;

          inline pami_result_t postMulticastImmediate_impl(size_t           client,
							   size_t           context,
							   pami_multicast_t *mcast, void* devinfo) {
	    return PAMI_ERROR;
	  }

          inline pami_result_t postMulticast_impl(uint8_t (&state)[sizeof(ShortMcstMessage<T_Device, T_Desc>)],
						  size_t           client,
						  size_t           context,
						  pami_multicast_t *mcast, void* devinfo)
          {

            PAMI::Topology *src_topo = (PAMI::Topology *)mcast->src_participants;
            //PAMI::Topology *dst_topo = (PAMI::Topology *)mcast->dst_participants;
            //size_t num_dst_ranks = dst_topo->size();
            //TRACE_ERR((stderr, "size of destination topology:%zu\n", num_dst_ranks));
            unsigned local_root = __global.topology_local.rank2Index(src_topo->index2Rank(0));

            T_Desc *my_desc = NULL; 
            pami_result_t res =	 _device.getShmemWorldDesc(&my_desc);
            while (res != PAMI_SUCCESS)
            {
              res = _device.getShmemWorldDesc(&my_desc);
              _device.advance();
            }

            if (mcast->bytes <= short_msg_cutoff)
            {
              if (local_root == _local_rank)
              {
                void* buf = (void*) my_desc->get_buffer(local_root);
                while (((PAMI::PipeWorkQueue *)mcast->src)->bytesAvailableToConsume() > 0) {} //spin wait for data to arrive
                void* mybuf = ((PAMI::PipeWorkQueue *)mcast->src)->bufferToConsume();
                memcpy(buf, mybuf, mcast->bytes);
                TRACE_ERR((stderr, "copied bytes:%zu from %p to %p data[0]:%u\n", mcast->bytes, mybuf, buf, ((unsigned*)buf)[0]));
                //my_desc->set_consumers(num_dst_ranks);
                my_desc->signal_flag();
              }
              my_desc->set_my_state(Shmem::INIT);

#ifdef SHORT_MCST_OPT
              res = ShortMcstMessage<T_Device, T_Desc>::short_msg_advance(my_desc, mcast, local_root, _local_rank);
              if (res == PAMI_SUCCESS)    
              {
                mcast->cb_done.function(_context, mcast->cb_done.clientdata, PAMI_SUCCESS);
                my_desc->set_my_state(Shmem::DONE);
              }
              my_desc->signal_done();
              return PAMI_SUCCESS;
#else
              my_desc->set_mcast_params(mcast);
              my_desc->set_master(local_root);
              ShortMcstMessage<T_Device, T_Desc> * obj = (ShortMcstMessage<T_Device, T_Desc> *) (&state[0]);
              new (obj) ShortMcstMessage<T_Device, T_Desc> (_device.getContext(), my_desc);
              PAMI::Device::Generic::Device &generic = _device.getProgressDevice();
              generic.postThread(&(obj->_work));
#endif
            }
            else
            {
#if 0
              my_desc->set_mcast_params(mcast);
              my_desc->set_master(local_root);

              if (local_root == _local_rank)
              {
                Memregion memregion;
                void* buf = (void*) my_desc->get_buffer(local_root);
                void* mybuf = ((PAMI::PipeWorkQueue *)mcast->src)->bufferToConsume();
                size_t bytes_out;
                memregion.createMemregion(&bytes_out, mcast->bytes, mybuf, 0);
                void* phy_addr = (void*)memregion.getBasePhysicalAddress();
                void * global_vaddr = NULL;
                uint32_t rc = 0;
                rc = Kernel_Physical2GlobalVirtual (phy_addr, &global_vaddr);
                assert(rc == 0);

                memcpy(buf, &global_vaddr, sizeof(global_vaddr));
                TRACE_ERR((stderr, "copied global_vaddr:%p to %p \n", global_vaddr, buf));
                my_desc->set_consumers(num_dst_ranks);
                my_desc->set_state(Shmem::INIT);
              }

              ShortMcstMessageShaddr<T_Device, T_Desc> * obj = (ShortMcstMessageShaddr<T_Device, T_Desc> *) (&state[0]);
              new (obj) ShortMcstMessageShaddr<T_Device, T_Desc> (&_device, my_desc, my_desc);
              _device.post(obj);
#endif
            }

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
#undef TRACE_ERR
#endif // __components_devices_shmem_ShmemPacketModel_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
