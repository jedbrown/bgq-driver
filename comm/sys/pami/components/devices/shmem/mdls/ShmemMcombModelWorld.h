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
 * \file components/devices/shmem/mdls/ShmemMcombModelWorld.h
 * \brief ???
 */

#ifndef __components_devices_shmem_mdls_ShmemMcombModelWorld_h__
#define __components_devices_shmem_mdls_ShmemMcombModelWorld_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"
#include "Memory.h"

#include "sys/pami.h"

#include "components/devices/MulticombineModel.h"
#include "components/devices/ShmemCollInterface.h"
#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/msgs/ShortMcombMessage.h"
#include "components/devices/shmem/msgs/ShaddrMcombMessagePipe.h"
#include "components/devices/shmem/msgs/BaseMessage.h"

#ifndef TRACE_ERR
//#define TRACE_ERR(x) //fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); fprintf x
#define TRACE_ERR(x) //fprintf x
#endif
namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

      template <class T_Device>
        class ShmemMcombModelWorld : public Interface::MulticombineModel < ShmemMcombModelWorld<T_Device>, T_Device, sizeof(BaseMessage<T_Device>) >
      {
        public:

          ShmemMcombModelWorld (T_Device &device, pami_result_t &status) :
            Interface::MulticombineModel < ShmemMcombModelWorld<T_Device>, T_Device, sizeof(Shmem::BaseMessage<T_Device>) > (device, status),
            _device(device),
            _local_rank(__global.topology_local.rank2Index(__global.mapping.task())),
            _npeers(__global.topology_local.size())

        { };


          static const size_t packet_model_state_bytes          = sizeof(Shmem::BaseMessage<T_Device>);
          static const size_t sizeof_msg                        = sizeof(Shmem::BaseMessage<T_Device>);
          static const size_t short_msg_cutoff                  = 64;

          inline pami_result_t postMulticombineImmediate_impl(size_t           client,
							      size_t           context,
							      pami_multicombine_t *mcomb, void* devinfo) {
	    return PAMI_ERROR;
	  }

          inline pami_result_t postMulticombine_impl(uint8_t (&state)[sizeof(Shmem::BaseMessage<T_Device>)],
						     size_t           client,
						     size_t           context,
						     pami_multicombine_t *mcomb, void* devinfo)
          {

            TRACE_ERR((stderr, "entering postMulticomb\n"));

            typename T_Device::CollectiveFifo::Descriptor *my_desc = NULL;

            pami_result_t res =	 _device.getShmemWorldDesc(&my_desc);
            while (res != PAMI_SUCCESS)
            {
              res =	 _device.getShmemWorldDesc(&my_desc);
              _device.advance();
            }

            TRACE_ERR((stderr, "mcomb->count:%zd\n", mcomb->count));
            size_t bytes = mcomb->count << pami_dt_shift[mcomb->dtype];

            if (bytes <= short_msg_cutoff)
            {

              //TRACE_ERR((stderr, "local_root%u my_local_rank:%u\n", local_root, _local_rank));
              void* buf = (void*) my_desc->get_buffer(_local_rank);
              void* mybuf = ((PAMI::PipeWorkQueue *)mcomb->data)->bufferToConsume();
              memcpy(buf, mybuf, bytes);
              TRACE_ERR((stderr, "copied bytes:%zu from %p to %p data[0]:%u\n", bytes, mybuf, buf, ((unsigned*)buf)[0]));
              ((PAMI::PipeWorkQueue *)mcomb->data)->consumeBytes(bytes);
              my_desc->set_my_state(Shmem::DESCSTATE_INIT);

              //if (local_root == _local_rank) //default is set to local ranks in desc
              //  my_desc->set_consumers(num_src_ranks);

              my_desc->set_mcomb_params(mcomb);
              my_desc->signal_arrived(); //signal that I have copied all my addresses/data

              Shmem::ShortMcombMessage<T_Device> * obj = (Shmem::ShortMcombMessage<T_Device> *) (&state[0]);
              new (obj) Shmem::ShortMcombMessage<T_Device> (_device.getContext(), my_desc, _local_rank);

              _device.post_obj(obj);

            }
            else
            {
              /*if (local_root == _local_rank)
                my_desc->set_consumers(num_src_ranks);*/

              my_desc->set_mcomb_params(mcomb);
              void* src_buf = ((PAMI::PipeWorkQueue *)mcomb->data)->bufferToConsume();
              void* dst_buf = ((PAMI::PipeWorkQueue *)mcomb->results)->bufferToProduce();
              TRACE_ERR((stderr, "Taking shaddr path local_root%u my_local_rank:%u my_va_src_buf:%p my_va_dst_buf:%p\n",
                    local_root, _local_rank, src_buf, dst_buf));

              if (src_buf == dst_buf)
                my_desc->_in_place = 1;

              Memregion memreg_src;
              Memregion memreg_dst;
              Shmem::McombControl* mcomb_control = (Shmem::McombControl*) my_desc->get_buffer();

              size_t bytes_out;
              memreg_src.createMemregion(&bytes_out, bytes, src_buf, 0);
              memreg_dst.createMemregion(&bytes_out, bytes, dst_buf, 0);

              void* phy_addr = (void*)memreg_src.getBasePhysicalAddress();
              void * global_vaddr = NULL;
              uint32_t rc = 0;
              rc = Kernel_Physical2GlobalVirtual (phy_addr, &global_vaddr);
              assert(rc == 0);
              mcomb_control->GlobalAddressTable.src_bufs[_local_rank] = global_vaddr;
              TRACE_ERR((stderr, "src buffer info..[%d]phy_addr:%p set my global src address:%p \n", _local_rank, phy_addr, global_vaddr));
              TRACE_ERR((stderr, "src buffer[0] via global VA:%f \n", ((double*)global_vaddr)[0]));

              phy_addr = (void*)memreg_dst.getBasePhysicalAddress();
              global_vaddr = NULL;
              rc = 0;
              rc = Kernel_Physical2GlobalVirtual (phy_addr, &global_vaddr);
              assert(rc == 0);
              mcomb_control->GlobalAddressTable.dst_bufs[_local_rank] = global_vaddr;
              TRACE_ERR((stderr, "dst buffer info..[%d]phy_addr:%p set my global dst address:%p \n", _local_rank, phy_addr, global_vaddr));
              TRACE_ERR((stderr, "dst buffer[0] via global VA:%f \n", ((double*)global_vaddr)[0]));

              mcomb_control->chunks_done[_local_rank] = 0;
              mcomb_control->chunks_copied[_local_rank] = 0;
              mcomb_control->current_iter=0;
              TRACE_ERR((stderr, "[%d]setting my chunks_done:%p to 0\n", _local_rank, &mcomb_control->chunks_done[_peer]));
              my_desc->set_my_state(Shmem::DESCSTATE_INIT);
              //src->consumeBytes(bytes);

              Memory::sync();
              /*Shmem::ShaddrMcombMessage<T_Device, T_Desc> * obj = (Shmem::ShaddrMcombMessage<T_Device, T_Desc> *) (&state[0]);
              new (obj) Shmem::ShaddrMcombMessage<T_Device, T_Desc> (_device.getContext(), my_desc, _local_rank);*/
              Shmem::ShaddrMcombMessagePipe<T_Device> * obj = (Shmem::ShaddrMcombMessagePipe<T_Device> *) (&state[0]);
              new (obj) Shmem::ShaddrMcombMessagePipe<T_Device> (_device.getContext(), my_desc, _local_rank);

              _device.post_obj(obj);
              my_desc->signal_arrived(); //signal that I have copied all my addresses/data

            }

            return PAMI_SUCCESS;

          };


        protected:

          T_Device      & _device;
          pami_context_t   _context;
          unsigned _local_rank;
          unsigned _npeers;

      };  // PAMI::Device::Shmem::ShmemMcombModelWorld class


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
