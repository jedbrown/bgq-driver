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
 * \file components/devices/shmem/msgs/ShortReduceMessage.h
 * \brief ???
 */

#ifndef __components_devices_shmem_msgs_ShortReduceMessage_h__
#define __components_devices_shmem_msgs_ShortReduceMessage_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"
#include "sys/pami.h"
//#include "quad_sum.h"
//#include "16way_sum.h"
#include "assert.h"

#undef TRACE_ERR

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); fprintf x
#endif

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

#define SHMEMBUF(x)	((double*) _master_desc->get_buffer(x))
#define SHMEMBUF_SHORT(x)	((double*) master_desc->get_buffer(x))

      template <class T_Device, class T_Desc>
        class ShortReduceMessage
        {
          public:
            //currently optimized to a many-to-one combine
            static inline pami_result_t short_msg_advance(T_Desc* master_desc, pami_multicombine_t* mcomb_params,
                unsigned npeers, unsigned local_rank, unsigned task)
            {

              if (local_rank == 0)
              {

                size_t num_src_ranks = ((PAMI::Topology*)mcomb_params->data_participants)->size();
                while (master_desc->arrived_peers() != (unsigned)num_src_ranks) {};
                TRACE_ERR((stderr, "all peers:%zu arrived, starting the blocking Shmem Mcomb protocol\n", num_src_ranks));
                PAMI::PipeWorkQueue *rcv = (PAMI::PipeWorkQueue *)mcomb_params->results;
                size_t bytes = mcomb_params->count << pami_dt_shift[mcomb_params->dtype];
                double* dst = (double*)(rcv->bufferToConsume());

                if (npeers == 4)
                {

                  for (unsigned i = 0; i < mcomb_params->count; i++)
                    SHMEMBUF_SHORT(0)[i] = SHMEMBUF_SHORT(0)[i] + SHMEMBUF_SHORT(1)[i] + SHMEMBUF_SHORT(2)[i] + SHMEMBUF_SHORT(3)[i];

                }
                else if (npeers == 8)
                {
                  for (unsigned i = 0; i < mcomb_params->count; i++)
                    SHMEMBUF_SHORT(0)[i] = SHMEMBUF_SHORT(0)[i] + SHMEMBUF_SHORT(1)[i] + SHMEMBUF_SHORT(2)[i] + SHMEMBUF_SHORT(3)[i] +
                      SHMEMBUF_SHORT(4)[i] + SHMEMBUF_SHORT(5)[i] + SHMEMBUF_SHORT(6)[i] + SHMEMBUF_SHORT(7)[i] ;

                }
                else if (npeers == 16)
                {
                  for (unsigned i = 0; i < mcomb_params->count; i++)
                    SHMEMBUF_SHORT(0)[i] = SHMEMBUF_SHORT(0)[i] + SHMEMBUF_SHORT(1)[i] + SHMEMBUF_SHORT(2)[i] + SHMEMBUF_SHORT(3)[i] +
                      SHMEMBUF_SHORT(4)[i] + SHMEMBUF_SHORT(5)[i] + SHMEMBUF_SHORT(6)[i] + SHMEMBUF_SHORT(7)[i] +
                      SHMEMBUF_SHORT(8)[i] + SHMEMBUF_SHORT(9)[i] + SHMEMBUF_SHORT(10)[i] + SHMEMBUF_SHORT(11)[i] +
                      SHMEMBUF_SHORT(12)[i] + SHMEMBUF_SHORT(13)[i] + SHMEMBUF_SHORT(14)[i] + SHMEMBUF_SHORT(15)[i];
                }
                else
                {
                  fprintf(stderr,"%s:%u npeers %u sum not yet supported\n",__FILE__,__LINE__,npeers);
                }

                char* src = (char*) master_desc->get_buffer(0);
                char* my_dst = (char*)(dst);
                memcpy((void*)my_dst, (void*)src, bytes);
                rcv->produceBytes(bytes);
                TRACE_ERR((stderr, "Finished gathering results, signalling done\n"));
                //master_desc->signal_flag();
              }

              return PAMI_SUCCESS;
            }

          protected:
            // invoked by the thread object
            /// \see SendQueue::Message::_work
            static pami_result_t __advance (pami_context_t context, void * cookie)
            {
              ShortReduceMessage * msg = (ShortReduceMessage *) cookie;
              return msg->advance();
            };

            inline pami_result_t advance ()
            {

              T_Desc* _my_desc = this->_my_desc;
              pami_multicombine_t & mcomb_params = _my_desc->get_mcomb_params();
              T_Desc* _master_desc = this->_master_desc;
              size_t num_src_ranks = ((PAMI::Topology*)mcomb_params.data_participants)->size();

              if (_master_desc->arrived_peers() != (unsigned) num_src_ranks)
                return PAMI_EAGAIN;
              //while (_master_desc->arrived_peers() != (unsigned)num_src_ranks) {};
              TRACE_ERR((stderr, "all peers:%zu arrived, starting the blocking Shmem Mcomb protocol\n", num_src_ranks));

              unsigned _npeers = __global.topology_local.size();
              unsigned _task = __global.mapping.task();
              unsigned _local_rank = __global.topology_local.rank2Index(_task);

              /* Start the protocol here..blocking version since everyone arrived */
              PAMI::PipeWorkQueue *rcv = (PAMI::PipeWorkQueue *)mcomb_params.results;
              size_t bytes = mcomb_params.count << pami_dt_shift[mcomb_params.dtype];
              double* dst = (double*)(rcv->bufferToConsume());

              if (_local_rank == 0)
              {
                if (_npeers == 4)
                {

                  for (unsigned i = 0; i < mcomb_params.count; i++)
                    SHMEMBUF(0)[i] = SHMEMBUF(0)[i] + SHMEMBUF(1)[i] + SHMEMBUF(2)[i] + SHMEMBUF(3)[i];

                }
                else if (_npeers == 8)
                {
                  for (unsigned i = 0; i < mcomb_params.count; i++)
                    SHMEMBUF(0)[i] = SHMEMBUF(0)[i] + SHMEMBUF(1)[i] + SHMEMBUF(2)[i] + SHMEMBUF(3)[i] +
                      SHMEMBUF(4)[i] + SHMEMBUF(5)[i] + SHMEMBUF(6)[i] + SHMEMBUF(7)[i] ;


                }
                else if (_npeers == 16)
                {
                  for (unsigned i = 0; i < mcomb_params.count; i++)
                    SHMEMBUF(0)[i] = SHMEMBUF(0)[i] + SHMEMBUF(1)[i] + SHMEMBUF(2)[i] + SHMEMBUF(3)[i] +
                      SHMEMBUF(4)[i] + SHMEMBUF(5)[i] + SHMEMBUF(6)[i] + SHMEMBUF(7)[i] +
                      SHMEMBUF(8)[i] + SHMEMBUF(9)[i] + SHMEMBUF(10)[i] + SHMEMBUF(11)[i] +
                      SHMEMBUF(12)[i] + SHMEMBUF(13)[i] + SHMEMBUF(14)[i] + SHMEMBUF(15)[i];

                }
                else
                {
                  fprintf(stderr,"%s:%u npeers %u sum not yet supported\n",__FILE__,__LINE__,_npeers);
                }

                char* src = (char*) master_desc->get_buffer(0);
                char* my_dst = (char*)(dst);
                memcpy((void*)my_dst, (void*)src, bytes);
                rcv->produceBytes(bytes);
                TRACE_ERR((stderr, "Finished gathering results, signalling done\n"));
                //master_desc->signal_flag();

              }

              //this->setStatus (PAMI::Device::Done);
              _my_desc->set_state(Shmem::DONE);

              mcomb_params.cb_done.function(_context, mcomb_params.cb_done.clientdata, PAMI_SUCCESS);
              return PAMI_SUCCESS;

            }


          public:
            inline ShortReduceMessage (pami_context_t context, T_Desc* desc, T_Desc* master_desc):
              _context(context), _my_desc(desc), _master_desc(master_desc), _work(ShortReduceMessage::__advance, this)

          {
            TRACE_ERR((stderr, "<> Shmem::ShortReduceMessage\n"));
          };

            pami_context_t                      _context;
            T_Desc                *_my_desc, *_master_desc;
            PAMI::Device::Generic::GenericThread _work;

        };  // PAMI::Device::McombMessageShmem class

    };
  };    // PAMI::Device namespace
};      // PAMI namespace
#undef TRACE_ERR
#endif  // __components_devices_shmem_ShortReduceMessageShmem_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
