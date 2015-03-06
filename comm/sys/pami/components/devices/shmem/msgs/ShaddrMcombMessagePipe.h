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
 * \file components/devices/shmem/msgs/ShaddrMcombMessagePipe.h
 * \brief ???
 */

#ifndef __components_devices_shmem_msgs_ShaddrMcombMessagePipe_h__
#define __components_devices_shmem_msgs_ShaddrMcombMessagePipe_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"
#include "sys/pami.h"
//#include "../a2qpx_nway_math.h"
#include "assert.h"
#include "BaseMessage.h"

#include "Memory.h"

#ifndef TRACE_ERR
//#define TRACE_ERR(x) fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); fprintf x
#define TRACE_ERR(x)  //fprintf x
#endif

#ifndef PAMI_ASSERT
#define PAMI_ASSERT(x) PAMI_assert(x)
#endif
#include "../bgq_math.h"


namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

//#define	CHUNK_SIZE 	1024
//#define	CHUNK_SIZE	2048
//#define	CHUNK_SIZE 	8192
#define	CHUNK_SIZE 	4096
#define NUM_CHUNKS(bytes)	((bytes%CHUNK_SIZE == 0) ? (bytes/CHUNK_SIZE):(bytes/CHUNK_SIZE + 1))

#define SHADDR_SRCBUF(x) 	((char*)(mcomb_control->GlobalAddressTable.src_bufs[x]))
#define SHADDR_DSTBUF(x) 	((char*)(mcomb_control->GlobalAddressTable.dst_bufs[x]))

#define ALIGNED_SRCBUF(x,y)	((((uint64_t)(mcomb_control->GlobalAddressTable.src_bufs[x])) & ((uint64_t)y)) == 0)
#define ALIGNED_DSTBUF(x,y)	((((uint64_t)(mcomb_control->GlobalAddressTable.dst_bufs[x])) & ((uint64_t)y)) == 0)

#define CURRENT_ITER	(mcomb_control->current_iter)
#define BLOCK_ITER	2

      struct McombControl
      {
        struct
        {
          void* src_bufs[NUM_LOCAL_TASKS];
          void* dst_bufs[NUM_LOCAL_TASKS];
        }GlobalAddressTable;// __attribute__((__aligned__(128)));
        volatile uint16_t	chunks_done[NUM_LOCAL_TASKS];
        volatile uint16_t	chunks_copied[NUM_LOCAL_TASKS];
        volatile uint16_t	current_iter;
      };

      template <class T_Device>
        class ShaddrMcombMessagePipe : public BaseMessage<T_Device>
      {
        protected:

          static pami_result_t __advance (pami_context_t context, void * cookie)
          {
            ShaddrMcombMessagePipe * msg = (ShaddrMcombMessagePipe *) cookie;
            return msg->advance();
          };

          inline void advance_4way_math(Shmem::McombControl* mcomb_control, unsigned _local_rank, unsigned _npeers,
              char* dst, size_t bytes, pami_op opcode, pami_dt dt)
          {
#if  1
            /* local ranks other than 0 do the following quad sum */
            unsigned iter;

            for (iter=0; iter < NUM_CHUNKS(bytes)-1; iter++){
              if ((iter%(_npeers-1) +1) == _local_rank){

                bgq_math_4way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, CHUNK_SIZE, opcode, dt);
                mcomb_control->chunks_done[_local_rank] += 1;
              }
              //TRACE_ERR((stderr,"dst[%zu]:%f\n", iter*CHUNK_SIZE, dst[iter*CHUNK_SIZE]));
            }

            /* summing last chunk which can be of any size */
            if ((iter%(_npeers-1) +1) == _local_rank){
              if (bytes%CHUNK_SIZE == 0)
              {
                bgq_math_4way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, CHUNK_SIZE, opcode, dt);

              }
              else
              {
                bgq_math_4way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, (bytes%CHUNK_SIZE), opcode, dt);
              }
              mcomb_control->chunks_done[_local_rank] += 1;
            }
#endif
          }

          inline void advance_8way_math(Shmem::McombControl* mcomb_control, unsigned _local_rank, unsigned _npeers,
              char* dst, size_t bytes, pami_op opcode, pami_dt  dt)
          {
#if 1
            /* local ranks other than 0 do the following quad sum */
            unsigned iter;

            for (iter=0; iter < NUM_CHUNKS(bytes)-1; iter++){
              if ((iter%(_npeers-1) +1) == _local_rank){
                bgq_math_8way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, CHUNK_SIZE, opcode, dt);
                mcomb_control->chunks_done[_local_rank] += 1;
              }
            }

            /* summing last chunk which can be of any size */
            if ((iter%(_npeers-1) +1) == _local_rank){
              if (bytes%CHUNK_SIZE == 0)
              {
                bgq_math_8way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, CHUNK_SIZE, opcode, dt);

              }
              else
              {
                bgq_math_8way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, (bytes%CHUNK_SIZE), opcode, dt);
              }

              mbar();
              mcomb_control->chunks_done[_local_rank] += 1;
            }
#endif
          }

          inline void advance_16way_math(Shmem::McombControl* mcomb_control, unsigned _local_rank, unsigned _npeers,
              char* dst, size_t bytes, pami_op opcode, pami_dt dt)
          {
            /* local ranks other than 0 do the following quad sum */
            unsigned iter;
            typename T_Device::CollectiveFifo::Descriptor* _my_desc = this->_my_desc;

            for (iter=0; iter < NUM_CHUNKS(bytes)-1; iter++){
              if ((iter%(_npeers-1) +1) == _local_rank){

                if (likely(_my_desc->_in_place == 0))
                {
                  bgq_math_16way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                      SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                      SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                      SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                      SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, SHADDR_SRCBUF(8)+iter*CHUNK_SIZE,
                      SHADDR_SRCBUF(9)+iter*CHUNK_SIZE, SHADDR_SRCBUF(10)+iter*CHUNK_SIZE,
                      SHADDR_SRCBUF(11)+iter*CHUNK_SIZE, SHADDR_SRCBUF(12)+iter*CHUNK_SIZE,
                      SHADDR_SRCBUF(13)+iter*CHUNK_SIZE, SHADDR_SRCBUF(14)+iter*CHUNK_SIZE,
                      SHADDR_SRCBUF(15)+iter*CHUNK_SIZE, CHUNK_SIZE, opcode, dt);
                }
                else
                {
                  char* srcs[16] = {SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, SHADDR_SRCBUF(8)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(9)+iter*CHUNK_SIZE, SHADDR_SRCBUF(10)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(11)+iter*CHUNK_SIZE, SHADDR_SRCBUF(12)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(13)+iter*CHUNK_SIZE, SHADDR_SRCBUF(14)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(15)+iter*CHUNK_SIZE};
                  coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, opcode, 16);
                  func((void*)(dst+ iter* CHUNK_SIZE), (void**)srcs,  16, CHUNK_SIZE/sizeof(double));
                }
                mcomb_control->chunks_done[_local_rank] += 1;
              }

              //TRACE_ERR((stderr,"dst[%zu]:%f\n", iter*CHUNK_SIZE, dst[iter*CHUNK_SIZE]));
            }

            /* summing last chunk which can be of any size */
            if ((iter%(_npeers-1) +1) == _local_rank){
              if (bytes%CHUNK_SIZE == 0)
              {
                if (likely(_my_desc->_in_place == 0))
                {
                bgq_math_16way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, SHADDR_SRCBUF(8)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(9)+iter*CHUNK_SIZE, SHADDR_SRCBUF(10)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(11)+iter*CHUNK_SIZE, SHADDR_SRCBUF(12)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(13)+iter*CHUNK_SIZE, SHADDR_SRCBUF(14)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(15)+iter*CHUNK_SIZE, CHUNK_SIZE, opcode, dt);
                }
                else
                {
                  char* srcs[16] = {SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, SHADDR_SRCBUF(8)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(9)+iter*CHUNK_SIZE, SHADDR_SRCBUF(10)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(11)+iter*CHUNK_SIZE, SHADDR_SRCBUF(12)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(13)+iter*CHUNK_SIZE, SHADDR_SRCBUF(14)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(15)+iter*CHUNK_SIZE};
                  coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, opcode, 16);
                  func((void*)(dst+ iter* CHUNK_SIZE), (void**)srcs,  16, CHUNK_SIZE/sizeof(double));
                }

              }
              else
              {
                if (likely(_my_desc->_in_place == 0))
                {
                bgq_math_16way(dst+ iter* CHUNK_SIZE, SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, SHADDR_SRCBUF(8)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(9)+iter*CHUNK_SIZE, SHADDR_SRCBUF(10)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(11)+iter*CHUNK_SIZE, SHADDR_SRCBUF(12)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(13)+iter*CHUNK_SIZE, SHADDR_SRCBUF(14)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(15)+iter*CHUNK_SIZE, (bytes%CHUNK_SIZE), opcode, dt);
                }
                else
                {
                  char* srcs[16] = {SHADDR_SRCBUF(0)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(1)+iter*CHUNK_SIZE, SHADDR_SRCBUF(2)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(3)+iter*CHUNK_SIZE, SHADDR_SRCBUF(4)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(5)+iter*CHUNK_SIZE, SHADDR_SRCBUF(6)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(7)+iter*CHUNK_SIZE, SHADDR_SRCBUF(8)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(9)+iter*CHUNK_SIZE, SHADDR_SRCBUF(10)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(11)+iter*CHUNK_SIZE, SHADDR_SRCBUF(12)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(13)+iter*CHUNK_SIZE, SHADDR_SRCBUF(14)+iter*CHUNK_SIZE,
                    SHADDR_SRCBUF(15)+iter*CHUNK_SIZE};
                  coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, opcode, 16);
                  func((void*)(dst+ iter* CHUNK_SIZE), (void**)srcs,  16, (bytes%CHUNK_SIZE)/sizeof(double));
                }
              }
              mcomb_control->chunks_done[_local_rank] += 1;
            }

          }


          inline virtual pami_result_t advance ()
          {

            typename T_Device::CollectiveFifo::Descriptor* _my_desc = this->_my_desc;
            unsigned _npeers = __global.topology_local.size();

            unsigned _local_rank = this->_local_rank;

            pami_multicombine_t & mcomb_params = _my_desc->get_mcomb_params();
            size_t num_src_ranks = ((PAMI::Topology*)mcomb_params.data_participants)->size();

            /* Non blocking until all the peers arrive at the collective */
            if (_my_desc->arrived_peers() != (unsigned) num_src_ranks)
            {
              TRACE_ERR((stderr,"arrived_peers:%u waiting for:%u\n", _my_desc->arrived_peers(), (unsigned) num_src_ranks));
              return PAMI_EAGAIN;
            }

            Memory::sync();
            TRACE_ERR((stderr, "all peers:%zu arrived, starting the blocking Mcomb protocol\n", num_src_ranks));

            /* Start the protocol here..blocking version since everyone arrived */

            //TRACE_ERR((stderr,"size of desc:%ld sizeof mcomb_control:%ld", sizeof(typename T_Device::CollectiveFifo::Descriptor), sizeof(McombControl)));

            Shmem::McombControl* mcomb_control = NULL;
            mcomb_control = (Shmem::McombControl*) _my_desc->get_buffer();
            assert(mcomb_control != NULL);

            PAMI::PipeWorkQueue *rcv = (PAMI::PipeWorkQueue *)mcomb_params.results;
            char* dst = (char*)(rcv->bufferToConsume());
            size_t bytes = mcomb_params.count << pami_dt_shift[mcomb_params.dtype];

            TRACE_ERR((stderr, "bytes of data to be multicombined:%zu\n", bytes));

            const uint64_t 	alignment = 64;
            uint64_t	mask1	= 0;
	    mask1 = (alignment - 1);

            pami_op opcode = mcomb_params.optor;
            pami_dt dt  = mcomb_params.dtype;

            /* All nodes except master(local rank 0), do the math */

            if (_local_rank != 0){

              if (_npeers == 4)
              {

                advance_4way_math(mcomb_control, _local_rank, _npeers, dst, bytes, opcode, dt);

              }
              else if (_npeers == 8){

                advance_8way_math(mcomb_control, _local_rank, _npeers, dst, bytes, opcode, dt);
              }
              else if (_npeers == 16){

                advance_16way_math(mcomb_control, _local_rank, _npeers, dst, bytes, opcode, dt);

              }
              else{

                fprintf(stderr,"%s:%u npeers %u sum not yet supported\n",__FILE__,__LINE__,_npeers);
                exit(0);

              }
            }

            /* Reduction over...start gathering the results, local_rank == 0, the master, gathers the results */
            else{
              unsigned iter, my_peer, loops=0;
              for (iter= (unsigned)CURRENT_ITER; (iter < NUM_CHUNKS(bytes)-1) && (iter < (unsigned)(CURRENT_ITER+BLOCK_ITER)); iter++,loops++)
              {
                my_peer = iter%(_npeers-1)+1;
                while (mcomb_control->chunks_done[my_peer] <= mcomb_control->chunks_copied[my_peer]){};

                {
                  memcpy((void*)(dst + CHUNK_SIZE*iter),
                  (void*)(SHADDR_DSTBUF(my_peer)+CHUNK_SIZE*iter), CHUNK_SIZE);
                  rcv->produceBytes(CHUNK_SIZE);
                  TRACE_ERR((stderr,"produced bytes%u\n",(unsigned)CHUNK_SIZE));
                  mcomb_control->chunks_copied[my_peer] += 1;
                }
              }

              if (loops > 0)
              {
                CURRENT_ITER+=loops;
                TRACE_ERR((stderr,"CURRENT_ITER:%d loops:%u\n",CURRENT_ITER, loops));
                return PAMI_EAGAIN;		//breaking the advance so that the master can advance network
              }

              /* copying last chunk */
              my_peer = CURRENT_ITER %(_npeers-1)+1;
              while (mcomb_control->chunks_done[my_peer] <= mcomb_control->chunks_copied[my_peer]){};
              Memory::sync();


              if (bytes%CHUNK_SIZE == 0)
              {
                memcpy((void*)(dst + CHUNK_SIZE*CURRENT_ITER),
                  (void*)(SHADDR_DSTBUF(my_peer)+CHUNK_SIZE*CURRENT_ITER), CHUNK_SIZE);
                rcv->produceBytes(CHUNK_SIZE);
                  TRACE_ERR((stderr,"produced bytes%u\n",(unsigned)CHUNK_SIZE));
              }
              else
              {
                memcpy((void*)(dst + CHUNK_SIZE*CURRENT_ITER),
                    (void*)(SHADDR_DSTBUF(my_peer)+CHUNK_SIZE*CURRENT_ITER), bytes%CHUNK_SIZE);
                rcv->produceBytes(bytes%CHUNK_SIZE);
                  TRACE_ERR((stderr,"produced bytes:%u\n",(unsigned)bytes%CHUNK_SIZE));
              }
            }

            _my_desc->signal_done();
            while (_my_desc->in_use()){}; //wait for everyone to signal done
            _my_desc->set_my_state(Shmem::DESCSTATE_DONE);
            mcomb_params.cb_done.function(this->_context, mcomb_params.cb_done.clientdata, PAMI_SUCCESS);
            return PAMI_SUCCESS;

            }

            public:
          inline ShaddrMcombMessagePipe (pami_context_t context, typename T_Device::CollectiveFifo::Descriptor* my_desc, unsigned local_rank) :
            BaseMessage<T_Device>(context, my_desc, ShaddrMcombMessagePipe::__advance, (void*)this, local_rank)
            {
              TRACE_ERR((stderr, "<> ShaddrMcombMessagePipe::ShaddrMcombMessagePipe()\n"));
            };


          };  // PAMI::Device::ShaddrMcombMessagePipe class

      };
    };    // PAMI::Device namespace
  };      // PAMI namespace
#undef TRACE_ERR
#endif  // __components_devices_shmem_ShaddrMcombMessagePipe_h__

  //
  // astyle info    http://astyle.sourceforge.net
  //
  // astyle options --style=gnu --indent=spaces=2 --indent-classes
  // astyle options --indent-switches --indent-namespaces --break-blocks
  // astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
  //
