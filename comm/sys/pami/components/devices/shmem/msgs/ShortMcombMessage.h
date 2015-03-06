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
 * \file components/devices/shmem/msgs/ShortMcombMessage.h
 * \brief ???
 */

#ifndef __components_devices_shmem_msgs_ShortMcombMessage_h__
#define __components_devices_shmem_msgs_ShortMcombMessage_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"
#include "sys/pami.h"
#include "assert.h"
//#include "../a2qpx_nway_sum.h"
#include "../a2qpx_nway_math.h"
#include "common/MultisendInterface.h"

#undef TRACE_ERR

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); fprintf x
#endif

#include "../bgq_math.h"
namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

//#define SHMEMBUF(x)	((double*) _my_desc->get_buffer(x))
#define SHMEMBUF(x)	((char*) _my_desc->get_buffer(x))
#define SHMEMBUF_SHORT(x)	((double*) my_desc->get_buffer(x))

      template <class T_Device>
        class ShortMcombMessage: public BaseMessage<T_Device>
        {
          public:
            // invoked by the thread object
            /// \see SendQueue::Message::_work
            static pami_result_t __advance (pami_context_t context, void * cookie)
            {
              ShortMcombMessage * msg = (ShortMcombMessage *) cookie;
              return msg->advance();
            }

            //This advance is for reduction to root locally in a node
            inline virtual pami_result_t advance ()
              //inline pami_result_t advance ()
            {

              typename T_Device::CollectiveFifo::Descriptor* _my_desc = this->_my_desc;
              pami_multicombine_t & mcomb_params = _my_desc->get_mcomb_params();
              size_t num_src_ranks = ((PAMI::Topology*)mcomb_params.data_participants)->size();

              //if (_my_desc->arrived_peers() != (unsigned) num_src_ranks)
              //  return PAMI_EAGAIN;
              //while (_my_desc->arrived_peers() != (unsigned)num_src_ranks) {};

              unsigned _npeers = __global.topology_local.size();
              unsigned _task = __global.mapping.task();
              unsigned _local_rank = __global.topology_local.rank2Index(_task);

              /* Start the protocol here..blocking version since everyone arrived */
              PAMI::PipeWorkQueue *rcv = (PAMI::PipeWorkQueue *)mcomb_params.results;
              size_t bytes = mcomb_params.count << pami_dt_shift[mcomb_params.dtype];
              double* dst = rcv?(double*)(rcv->bufferToConsume()):(double*)NULL;

              pami_op opcode = mcomb_params.optor;
              pami_dt dt  = mcomb_params.dtype;

              if (_local_rank == 0)
              {

                if (_my_desc->arrived_peers() != (unsigned) num_src_ranks)
                  return PAMI_EAGAIN;

                if (_npeers == 4)
                {

                  /*for (unsigned i = 0; i < mcomb_params.count; i++)
                    SHMEMBUF(0)[i] = SHMEMBUF(0)[i] + SHMEMBUF(1)[i] + SHMEMBUF(2)[i] + SHMEMBUF(3)[i];*/
                  bgq_math_4way(SHMEMBUF(0),  SHMEMBUF(0), SHMEMBUF(1), SHMEMBUF(2), SHMEMBUF(3), bytes, opcode, dt);

                }
                else if (_npeers == 8)
                {
                  /*for (unsigned i = 0; i < mcomb_params.count; i++)
                    SHMEMBUF(0)[i] = SHMEMBUF(0)[i] + SHMEMBUF(1)[i] + SHMEMBUF(2)[i] + SHMEMBUF(3)[i] +
                    SHMEMBUF(4)[i] + SHMEMBUF(5)[i] + SHMEMBUF(6)[i] + SHMEMBUF(7)[i] ;*/
                  bgq_math_8way(SHMEMBUF(0),  SHMEMBUF(0), SHMEMBUF(1), SHMEMBUF(2), SHMEMBUF(3),
                      SHMEMBUF(4), SHMEMBUF(5), SHMEMBUF(6), SHMEMBUF(7), bytes, opcode, dt);

                }
                else if (_npeers == 16)
                {
                  /*for (unsigned i = 0; i < mcomb_params.count; i++)
                    SHMEMBUF(0)[i] = SHMEMBUF(0)[i] + SHMEMBUF(1)[i] + SHMEMBUF(2)[i] + SHMEMBUF(3)[i] +
                    SHMEMBUF(4)[i] + SHMEMBUF(5)[i] + SHMEMBUF(6)[i] + SHMEMBUF(7)[i] +
                    SHMEMBUF(8)[i] + SHMEMBUF(9)[i] + SHMEMBUF(10)[i] + SHMEMBUF(11)[i] +
                    SHMEMBUF(12)[i] + SHMEMBUF(13)[i] + SHMEMBUF(14)[i] + SHMEMBUF(15)[i];*/
                  bgq_math_16way(SHMEMBUF(0), SHMEMBUF(0), SHMEMBUF(1), SHMEMBUF(2), SHMEMBUF(3),
                      SHMEMBUF(4), SHMEMBUF(5), SHMEMBUF(6), SHMEMBUF(7), SHMEMBUF(8), SHMEMBUF(9), SHMEMBUF(10),
                      SHMEMBUF(11), SHMEMBUF(12), SHMEMBUF(13), SHMEMBUF(14), SHMEMBUF(15), bytes, opcode, dt);

                }
                else
                {
                  fprintf(stderr,"%s:%u npeers %u sum not yet supported\n",__FILE__,__LINE__,_npeers);
                }

                _my_desc->signal_flag();
              }

              /* Reduction over...start gathering the results */
              if (((PAMI::Topology*)mcomb_params.results_participants)->isRankMember(_task))
              {
                PAMI_assert(dst);
                while (_my_desc->get_flag() == 0) {}; //wait till reduction is done
                char* src = (char*) _my_desc->get_buffer(0);
                char* my_dst = (char*)(dst);
                memcpy((void*)my_dst, (void*)src, bytes);
                rcv->produceBytes(bytes);

                TRACE_ERR((stderr, "Finished gathering results, signalling done\n"));
              }

              _my_desc->signal_done();
              _my_desc->set_my_state(Shmem::DESCSTATE_DONE);
              mcomb_params.cb_done.function(this->_context, mcomb_params.cb_done.clientdata, PAMI_SUCCESS);
              return PAMI_SUCCESS;

            }

          public:
            inline ShortMcombMessage (pami_context_t context, typename T_Device::CollectiveFifo::Descriptor* desc, unsigned local_rank):
              BaseMessage<T_Device>(context, desc, ShortMcombMessage::__advance, (void*)this, local_rank)

          {
            TRACE_ERR((stderr, "<> Shmem::ShortMcombMessage\n"));
          };

            /*pami_context_t                      _context;
              T_Desc                              *_my_desc;
              PAMI::Device::Generic::GenericThread _work;*/

        };  // PAMI::Device::McombMessageShmem class

    };
  };    // PAMI::Device namespace
};      // PAMI namespace
#undef TRACE_ERR
#endif  // __components_devices_shmem_ShortMcombMessageShmem_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
