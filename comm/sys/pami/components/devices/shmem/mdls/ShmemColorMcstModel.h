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
 * \file components/devices/shmem/mdls/ShmemColorMcstModel.h
 * \brief ???
 */

#ifndef __components_devices_shmem_mdls_ShmemColorMcstModel_h__
#define __components_devices_shmem_mdls_ShmemColorMcstModel_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"
#include "Memory.h"

#include "sys/pami.h"
#include "common/bgq/TypeDefs.h"

#include "components/devices/MulticastModel.h"
#include "components/devices/ShmemCollInterface.h"
#include "components/devices/shmem/msgs/ShaddrMcstMessage.h"
#include "components/devices/shmem/msgs/BaseMessage.h"
#include "components/devices/shmem/ShmemCollDesc.h"

#include "util/trace.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

#define NUM_SHMEM_MCST_COLORS	16

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

      template <class T_Device>
        class ShmemColorMcstModel : public Interface::MulticastModel < ShmemColorMcstModel<T_Device>, T_Device, sizeof(Shmem::BaseMessage<T_Device>) >
      {

        public:
          //Shmem Multicast Model
	  ShmemColorMcstModel (pami_client_t                client,
			       pami_context_t               context,
			       T_Device                   & device, 
			       pami_result_t              & status) :
            Interface::MulticastModel < ShmemColorMcstModel<T_Device>, T_Device, sizeof(Shmem::BaseMessage<T_Device>) > (device, status),
            _device(device),
            _mytask(__global.mapping.task()),
            _local_rank(__global.topology_local.rank2Index(_mytask)),
            _npeers(__global.topology_local.size())

        {
          pami_result_t rc;
          char key[PAMI::Memory::MMKEYSIZE];

          //create shared descriptors
          sprintf(key, "/ShmemColorMcstModel-shared_desc_array-%s", _device.getUniqueString());

          size_t total_size = sizeof(ShmemRegion)*NUM_SHMEM_MCST_COLORS ; 
          rc = __global.mm.memalign ((void **) & _shmem_region,
              64,
              total_size,
              key,
              ShmemColorMcstModel::shmem_region_initialize,
              NULL);
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate shared memory resources for collective descriptors");

          for (size_t i = 0; i < NUM_SHMEM_MCST_COLORS; i++)
          {
            new (&_desc[i]) typename T_Device::CollectiveFifo::Descriptor(__global.mm, _device.getUniqueString(), 1, i );
          }

        };

          static const size_t packet_model_state_bytes          = sizeof(Shmem::BaseMessage<T_Device>);
          static const size_t sizeof_msg                        = sizeof(Shmem::BaseMessage<T_Device>);
          //static const size_t short_msg_cutoff                  = 512;

          inline pami_result_t postMulticastImmediate_impl(size_t           client,
							   size_t           context,
							   pami_multicast_t *mcast, void* devinfo) {
	    return PAMI_ERROR;
	  }

          inline pami_result_t postMulticast_impl(uint8_t (&state)[sizeof(Shmem::BaseMessage<T_Device>)],
						  size_t           client,
						  size_t           context,
						  pami_multicast_t *mcast, void* devinfo)
          {
            TRACE_FN_ENTER();

            PAMI::Topology *src_topo = (PAMI::Topology *)mcast->src_participants;
            PAMI::Topology *dst_topo = (PAMI::Topology *)mcast->dst_participants;
            size_t num_dst_ranks = 0;

            unsigned conn_id = mcast->connection_id;
            TRACE_FORMAT("conn_id:%u", conn_id);

            typename T_Device::CollectiveFifo::Descriptor *my_desc = NULL;

            my_desc = &_desc[conn_id];
            my_desc->set_mcast_params(mcast);

            if ((src_topo != NULL)  && (src_topo->index2Rank(0) == _mytask)) //am the master/root
            {
              if (dst_topo != NULL)
              {
                /// \todo We shouldn't have to pull out a subtopology.  This mcast should use
                /// the full dst topology (assuming the caller passes in the correct topology).

                PAMI_assert (dst_topo->type() == PAMI_AXIAL_TOPOLOGY);
                pami_coord_t *ll = NULL;
                pami_coord_t *ur = NULL;
                pami_coord_t *ref = NULL;
                unsigned char *isTorus = NULL;

                pami_result_t result = PAMI_SUCCESS;
                result = dst_topo->axial(&ll, &ur, &ref, &isTorus);
                PAMI_assert(result == PAMI_SUCCESS);
                num_dst_ranks =  ur->u.n_torus.coords[LOCAL_DIM] - ll->u.n_torus.coords[LOCAL_DIM];
                TRACE_FORMAT("num_dst_ranks:%zu", num_dst_ranks);
                my_desc->set_master(1);
              }
            }
            else
            {
              my_desc->set_master(0);
            }

            if ((src_topo != NULL)  && (src_topo->index2Rank(0) == _mytask)) //am the master/root
            { // I am the master
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

              my_desc->set_seq_id(my_desc->get_seq_id()+1);
            }

            Shmem::ShaddrMcstMessage<T_Device> * obj = (Shmem::ShaddrMcstMessage<T_Device> *) (&state[0]);
            new (obj) Shmem::ShaddrMcstMessage<T_Device> ( _device.getContext(), my_desc, Shmem::ShaddrMcstMessage<T_Device>::__advance_color, (void*)obj, _local_rank);
            _device.getLocalProgressDevice()->postThread(&(obj->_work));

            TRACE_FN_EXIT();
            return PAMI_SUCCESS;

          };


        protected:

          T_Device      & _device;
          pami_context_t   _context;
          pami_task_t _mytask;
          unsigned _local_rank;
          unsigned _npeers;

          typename T_Device::CollectiveFifo::Descriptor	_desc[NUM_SHMEM_MCST_COLORS];


          static void shmem_region_initialize (void       * memory,
              size_t       bytes,
              const char * key,
              unsigned     attributes,
              void       * cookie)
          {
            ShmemRegion *shm_region = (ShmemRegion*) memory;

            size_t  i;
            for (i =0; i < NUM_SHMEM_MCST_COLORS;i++)
            {
              new (&shm_region[i]) ShmemRegion((uint64_t)i);
            }

          }

      };  // PAMI::Device::Shmem::ShmemColorMcstModel class

    };    // PAMI::Device::Shmem namespace
  };      // PAMI::Device namespace
};        // PAMI namespace

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __components_devices_shmem_ShmemPacketModel_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
