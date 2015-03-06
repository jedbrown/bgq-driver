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
 * \file components/devices/shmem/msgs/CNShmemMcastMessage.h
 * \brief ???
 */

#ifndef __components_devices_shmem_msgs_CNShmemMcastMessage_h__
#define __components_devices_shmem_msgs_CNShmemMcastMessage_h__

//#include "ShaddrMcombMessagePipe.h"

#undef TRACE_ERR

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif
#include "components/devices/shmem/CNShmemDesc.h"

#include "Memory.h"

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
        //This class has routines for advancing short/medium/long multicombines and multicasts
        class CNShmemMcastMessage 
        {

          public:

            //Very short multicast..synchronization is done via a flag
            inline static pami_result_t very_short_msg_multicast(CNShmemDesc* my_desc, PipeWorkQueue *dpwq, unsigned total_bytes, unsigned npeers, unsigned local_rank,
                                                                   uint64_t* counter_addr, uint64_t &counter_curr)
            {
              void* buf = (void*)my_desc->get_buffer();
              void* rcvbuf = dpwq->bufferToProduce();

              if (local_rank == 0)
              {
                if (*counter_addr == counter_curr)
                  return PAMI_EAGAIN;

                Memory::sync();
                my_desc->signal_flag();
              }
              else
              {
                if (my_desc->get_flag() == 0) 
                  return PAMI_EAGAIN;
              }

              memcpy(rcvbuf, buf, total_bytes);
              return PAMI_SUCCESS;
            }

            //Short multicast..data is taken from a shared buffer after CN deposits into it, supports pipelining
            inline pami_result_t short_msg_multicast(unsigned total_bytes, unsigned npeers, unsigned local_rank, uint64_t* counter_addr, uint64_t &counter_curr)
            {
              uint64_t  bytes_arrived =0;
              void* buf = (void*)_my_desc->get_buffer();
              unsigned bytes_so_far = total_bytes - counter_curr;

              if (local_rank == 0)
              {
                if (*counter_addr == counter_curr)
                  return PAMI_EAGAIN;

                bytes_arrived = counter_curr - *counter_addr;
                Memory::sync();
                _controlB->bytes_incoming+= (unsigned)bytes_arrived;

                //memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
                Core_memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
              }
              else
              {
                if (_controlB->bytes_incoming > bytes_so_far)
                {
                  bytes_arrived = _controlB->bytes_incoming - bytes_so_far;
                  //memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
                  Core_memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
                }
              }

              counter_curr -= bytes_arrived;
              if (counter_curr)
                return PAMI_EAGAIN;

              return PAMI_SUCCESS;
            }

            // Large message multicast..data is read directly from the master's receive buffer. supports pipelining
            inline pami_result_t large_msg_multicast(unsigned total_bytes, unsigned npeers, unsigned local_rank, volatile uint64_t* counter_addr, uint64_t &counter_curr)
            {
              //uint64_t  bytes_arrived =0;
              //void* buf = (void*) _controlB->GAT.dstbufs[0];
              //unsigned bytes_so_far = total_bytes - counter_curr;

              /* Non blocking until all the peers arrive at the collective */
              if (_my_desc->arrived_peers() != (unsigned) npeers)
              {
                TRACE_ERR((stderr,"arrived_peers:%u waiting for:%u\n", _my_desc->arrived_peers(), (unsigned) npeers));
                return PAMI_EAGAIN;
              }
              uint64_t  bytes_arrived =0;
              void* buf = (void*) _controlB->GAT.dstbufs[0];
              unsigned bytes_so_far = total_bytes - counter_curr;

              if (local_rank == 0)
              {
                if (*counter_addr == counter_curr)
                  if (counter_curr != 0) 
                    return PAMI_EAGAIN;

                bytes_arrived = counter_curr - *counter_addr;
                Memory::sync();
                _controlB->bytes_incoming+= (unsigned)bytes_arrived;
                counter_curr -= bytes_arrived;
              }
              else
              {
                bytes_arrived = _controlB->bytes_incoming - bytes_so_far;
                
                if (bytes_arrived > 0)
                {
                  Core_memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
                  //quad_double_copy((double*)((char*)_rcvbuf + bytes_so_far), (double*)((char*)buf + bytes_so_far), bytes_arrived/sizeof(double));
                  counter_curr -= bytes_arrived;
                }
              }

              if (counter_curr)
                return PAMI_EAGAIN;
     
              _my_desc->signal_done();

              while (_my_desc->in_use()){};

              return PAMI_SUCCESS;
            }


            inline CNShmemMcastMessage () {};
            inline CNShmemMcastMessage (CNShmemDesc* my_desc, uint32_t length):_my_desc(my_desc),_total_bytes(length)
            {};

            inline ~CNShmemMcastMessage() {};
            //Initialize all the Virtual,Global and Physical addreses required in the operation
            //Initialize the shmem descriptor used for staging data and synchronization
            inline void init (void* srcbuf, void* rcvbuf, unsigned local_rank)
            {
              _srcbuf = (double*)srcbuf;
              _rcvbuf = (double*)rcvbuf;
              void* buf = _my_desc->get_buffer();
              _controlB = (ControlBlock*)buf;

              /*_opcode = opcode;
              _controlB->GAT.srcbufs[local_rank] = srcbuf_gva;
              _controlB->GAT.dstbufs[local_rank] = rcvbuf_gva;
              _controlB->phybufs[local_rank] = rcvbuf_phy;
              _shm_phy_addr = shmbuf_phy;*/

              if (local_rank == 0)
              {
                _controlB->bytes_incoming=0;
              }
              //_cur_offset = 0;
            };

            inline  void  set_srcbuf_mcast(void* srcbuf_phy)
            {
              void* buf = _my_desc->get_buffer();
              _controlB = (ControlBlock*)buf;

              _controlB->phybufs[0] = srcbuf_phy;
            }
          
            inline  void* get_srcbuf_mcast()
            {
              if (_my_desc->arrived_peers() ==  __global.mapping.tSize())
                return  _controlB->phybufs[0];

              return NULL;
            }

            inline  void  set_rcvbuf_master(void* rcvbuf_gva)
            {
              void* buf = _my_desc->get_buffer();
              _controlB = (ControlBlock*)buf;
              _controlB->GAT.dstbufs[0] = rcvbuf_gva;
            }

            inline  void* get_rcvbuf_master()
            {
              return _controlB->GAT.dstbufs[0];
            }
            
            //void*     _shm_phy_addr;
          private:

            CNShmemDesc    *_my_desc;
            unsigned  _total_bytes;
            double*     _srcbuf;
            double*     _rcvbuf;
            ControlBlock* _controlB;
            
        };  // PAMI::Device::CNShmemMcastMessage class
    };
  };    // PAMI::Device namespace
};      // PAMI namespace

#undef TRACE_ERR
#endif  // __components_devices_shmem_CNShmemMcastMessage_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
