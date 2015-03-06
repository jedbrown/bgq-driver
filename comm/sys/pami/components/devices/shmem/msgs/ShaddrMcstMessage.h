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
 * \file components/devices/shmem/msgs/ShaddrMcstMessage.h
 * \brief ???
 */

#ifndef __components_devices_shmem_msgs_ShaddrMcstMessage_h__
#define __components_devices_shmem_msgs_ShaddrMcstMessage_h__

#include <errno.h>
#include <sys/uio.h>

#include "Arch.h"
#include "Memory.h"

#include "sys/pami.h"
//#include "opt_copy_a2.h"
#include "math/Memcpy.x.h"
#include "BaseMessage.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0

//#define COPY_BY_CHUNKS

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
#define MCST_CHUNK_SIZE	8192

      struct McstControl
      {
        volatile void         *glob_src_buffer;
        volatile uint32_t	incoming_bytes;	
      };

      template <class T_Device>
        class ShaddrMcstMessage : public BaseMessage<T_Device>
      {
        public:

          static pami_result_t __advance (pami_context_t context, void * cookie)
          {
            TRACE_FN_ENTER();
            ShaddrMcstMessage * msg = (ShaddrMcstMessage *) cookie;
            TRACE_FN_EXIT();
            return msg->advance();
          };

          static pami_result_t __advance_color (pami_context_t context, void * cookie)
          {
            TRACE_FN_ENTER();
            ShaddrMcstMessage * msg = (ShaddrMcstMessage *) cookie;
            TRACE_FN_EXIT();
            return msg->advance_color();
          };

        protected:

          inline virtual pami_result_t advance()
          {

            TRACE_FN_ENTER();
            typename T_Device::CollectiveFifo::Descriptor* _my_desc = this->_my_desc;

            unsigned master = _my_desc->get_master();
            void* mybuf;

            Shmem::McstControl* mcst_control = (Shmem::McstControl*) _my_desc->get_buffer();
            assert(mcst_control != NULL);

            pami_multicast_t & mcast_params = _my_desc->get_mcast_params();
            PAMI::PipeWorkQueue *rcv = (PAMI::PipeWorkQueue*) mcast_params.dst;
            PAMI::PipeWorkQueue *src = (PAMI::PipeWorkQueue*) mcast_params.src;
            unsigned bytes = mcast_params.bytes; 

            if (this->_local_rank == master){
              if (this->_bytes_consumed == bytes)
              {
                if (_my_desc->in_use()) 
                { 
                  TRACE_FN_EXIT();
                  return PAMI_EAGAIN; //wait for everyone to signal done
                }
                TRACE_STRING("everyone arrived calling done");

                mcst_control->incoming_bytes = 0;
                mcst_control->glob_src_buffer = NULL;
                Memory::sync();
                mcast_params.cb_done.function(this->_context, mcast_params.cb_done.clientdata, PAMI_SUCCESS);
                _my_desc->set_my_state(Shmem::DESCSTATE_DONE);
                TRACE_FN_EXIT();
                return PAMI_SUCCESS;
              }

              size_t bytes_to_consume = src->bytesAvailableToConsume() - this->_bytes_consumed;
              if (bytes_to_consume > 0) 
              {
                TRACE_FORMAT("bytes_to_consume:%zd", bytes_to_consume);
                mcst_control->incoming_bytes += bytes_to_consume;
                this->_bytes_consumed+= bytes_to_consume;
                TRACE_FORMAT("_bytes_consumed:%zd bytes:%u", bytes_to_consume, bytes);
                TRACE_FN_EXIT();
                return PAMI_EAGAIN;
              }
            }
            else
            {
              if (this->_my_desc->get_flag() == 0)
              {  
                TRACE_FN_EXIT();
                return PAMI_EAGAIN;
              }

              if (this->_bytes_consumed == bytes)
              {
                _my_desc->signal_done();
                mcast_params.cb_done.function(this->_context, mcast_params.cb_done.clientdata, PAMI_SUCCESS);
                _my_desc->set_my_state(Shmem::DESCSTATE_DONE);
                TRACE_FN_EXIT();
                return PAMI_SUCCESS;
              }

              TRACE_FORMAT("incoming_bytes:%d, _bytes_consumed:%u", mcst_control->incoming_bytes, this->_bytes_consumed);
              if (mcst_control->incoming_bytes > this->_bytes_consumed)
              {
                Memory::sync();
                TRACE_FORMAT("incoming_bytes:%d, _bytes_consumed:%u", mcst_control->incoming_bytes, this->_bytes_consumed);
                size_t	bytes_to_copy = mcst_control->incoming_bytes - this->_bytes_consumed;
                mybuf = rcv->bufferToProduce();
                volatile void*	glob_src_buf = mcst_control->glob_src_buffer;
                TRACE_FORMAT("copying from gva:%p bytes_consumed:%u incoming_bytes:%d bytes_to_copy:%zd", glob_src_buf,this->_bytes_consumed, mcst_control->incoming_bytes, bytes_to_copy);
                assert(glob_src_buf != NULL);

#ifdef COPY_BY_CHUNKS
                /* check if bytes_to_copy is a multiple of CHUNK_SIZE */
                if (bytes - this->_bytes_consumed >= MCST_CHUNK_SIZE)
                {
                  size_t chunks_to_copy = bytes_to_copy/MCST_CHUNK_SIZE;
                  if (chunks_to_copy != 0) //atleast one chunk received
                  {
                    opt_bgq_memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), 
                        chunks_to_copy*MCST_CHUNK_SIZE);
                    this->_bytes_consumed+= chunks_to_copy*MCST_CHUNK_SIZE;
                    rcv->produceBytes(chunks_to_copy*MCST_CHUNK_SIZE);
                  }
                }
                else
                {
                  opt_bgq_memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                  //memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                  this->_bytes_consumed+= bytes_to_copy;
                  TRACE_FORMAT("incoming_bytes:%d, _bytes_consumed:%zd", mcst_control->incoming_bytes, this->_bytes_consumed);
                  rcv->produceBytes(bytes_to_copy);
                }

#else
                Core_memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                //opt_bgq_memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                //memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                this->_bytes_consumed+= bytes_to_copy;
                TRACE_FORMAT("incoming_bytes:%d, _bytes_consumed:%u", mcst_control->incoming_bytes, this->_bytes_consumed);
                rcv->produceBytes(bytes_to_copy);
#endif

              }
              TRACE_FN_EXIT();
              return PAMI_EAGAIN;
            }
            TRACE_FN_EXIT();
            return PAMI_EAGAIN;
          }

          inline pami_result_t advance_color()
          {
            TRACE_FN_ENTER();

            typename T_Device::CollectiveFifo::Descriptor* _my_desc = this->_my_desc;

            unsigned master = _my_desc->get_master();
            void* mybuf;

            Shmem::McstControl* mcst_control = (Shmem::McstControl*) _my_desc->get_buffer();
            assert(mcst_control != NULL);

            pami_multicast_t & mcast_params = _my_desc->get_mcast_params();
            PAMI::PipeWorkQueue *rcv = (PAMI::PipeWorkQueue*) mcast_params.dst;
            PAMI::PipeWorkQueue *src = (PAMI::PipeWorkQueue*) mcast_params.src;
            unsigned bytes = mcast_params.bytes; 

            if (master){
              if (this->_bytes_consumed == bytes)
              {
                if (_my_desc->in_use()) 
                { 
                  TRACE_FN_EXIT();
                  return PAMI_EAGAIN; //wait for everyone to signal done
                }
                TRACE_STRING("everyone arrived calling done");
                _my_desc->reset();
                mcst_control->incoming_bytes = 0;
                mcst_control->glob_src_buffer = NULL;

                mcast_params.cb_done.function(this->_context, mcast_params.cb_done.clientdata, PAMI_SUCCESS);
                _my_desc->set_my_seq_id(_my_desc->get_my_seq_id()+1);
                _my_desc->set_my_state(Shmem::DESCSTATE_DONE);
                return PAMI_SUCCESS;
              }

              size_t bytes_to_consume = src->bytesAvailableToConsume() - this->_bytes_consumed;
              if (bytes_to_consume > 0) 
              {
                mcst_control->incoming_bytes += bytes_to_consume;
                this->_bytes_consumed+= bytes_to_consume;
                TRACE_FORMAT("bytes_to_consume:%zd total_bytes_consumed:%u bytes:%u", bytes_to_consume,this->_bytes_consumed, bytes);

                if (this->_bytes_consumed == bytes) 
                  _my_desc->signal_done();
                TRACE_FN_EXIT();
                return PAMI_EAGAIN;
              }
            }
            else
            {
              if ((_my_desc->get_my_seq_id() + 1) != _my_desc->get_seq_id())
              {  
                TRACE_FN_EXIT();
                return PAMI_EAGAIN;
              }

              if (this->_bytes_consumed == bytes)
              {
                mcast_params.cb_done.function(this->_context, mcast_params.cb_done.clientdata, PAMI_SUCCESS);
                _my_desc->set_my_seq_id(_my_desc->get_my_seq_id()+1);
                _my_desc->set_my_state(Shmem::DESCSTATE_DONE);
                TRACE_FN_EXIT();
                return PAMI_SUCCESS;
              }

              if (mcst_control->incoming_bytes > this->_bytes_consumed)
              {
                Memory::sync();
                TRACE_FORMAT("incoming_bytes:%d, _bytes_consumed:%u", mcst_control->incoming_bytes, this->_bytes_consumed);
                size_t	bytes_to_copy = mcst_control->incoming_bytes - this->_bytes_consumed;
                mybuf = rcv->bufferToProduce();
                volatile void*	glob_src_buf = mcst_control->glob_src_buffer;
                TRACE_FORMAT("copying from gva:%p bytes_consumed:%u incoming_bytes:%d bytes_to_copy:%zd", glob_src_buf,this->_bytes_consumed, mcst_control->incoming_bytes, bytes_to_copy);
                assert(glob_src_buf != NULL);

#ifdef COPY_BY_CHUNKS
                /* check if bytes_to_copy is a multiple of CHUNK_SIZE */
                if (bytes - this->_bytes_consumed >= MCST_CHUNK_SIZE)
                {
                  size_t chunks_to_copy = bytes_to_copy/MCST_CHUNK_SIZE;
                  if (chunks_to_copy != 0) //atleast one chunk received
                  {
                    opt_bgq_memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), 
                        chunks_to_copy*MCST_CHUNK_SIZE);
                    this->_bytes_consumed+= chunks_to_copy*MCST_CHUNK_SIZE;
                    rcv->produceBytes(chunks_to_copy*MCST_CHUNK_SIZE);
                  }
                }
                else
                {
                  opt_bgq_memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                  //memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                  this->_bytes_consumed+= bytes_to_copy;
                  TRACE_FORMAT("incoming_bytes:%d, _bytes_consumed:%u", mcst_control->incoming_bytes, this->_bytes_consumed);
                  rcv->produceBytes(bytes_to_copy);
                }

#else
                Core_memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                //opt_bgq_memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                //memcpy((void*)((char*)mybuf), (void*)((char*)glob_src_buf+this->_bytes_consumed), bytes_to_copy);
                this->_bytes_consumed+= bytes_to_copy;
                TRACE_FORMAT("incoming_bytes:%d, _bytes_consumed:%u", mcst_control->incoming_bytes, this->_bytes_consumed);
                rcv->produceBytes(bytes_to_copy);
#endif
                if (this->_bytes_consumed == bytes) 
                  _my_desc->signal_done();

              }
              TRACE_FN_EXIT();
              return PAMI_EAGAIN;
            }
            TRACE_FN_EXIT();
            return PAMI_EAGAIN;
          }


        public:
          inline ShaddrMcstMessage (pami_context_t context, typename T_Device::CollectiveFifo::Descriptor* my_desc, unsigned local_rank) :
            BaseMessage <T_Device>(context, my_desc,ShaddrMcstMessage::__advance,(void*)this,local_rank)

        {
          TRACE_FN_ENTER();
          TRACE_FN_EXIT();
        };


          inline ShaddrMcstMessage (pami_context_t context, typename T_Device::CollectiveFifo::Descriptor* my_desc, pami_work_function work_fn, void* cookie, unsigned local_rank) : 
            BaseMessage <T_Device>(context, my_desc, work_fn, (void*)this, local_rank)

        {
          TRACE_FN_ENTER();
          TRACE_FN_EXIT();
        };

      };  // PAMI::Device::ShaddrMcstMessage class

    };
  };    // PAMI::Device namespace
};      // PAMI namespace
#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#endif  // __components_devices_shmem_ShaddrMcstMessage_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
