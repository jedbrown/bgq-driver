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
 * \file components/devices/shmem/CNShmemDesc.h
 * \brief ???
 */
#ifndef __components_devices_shmem_CNShmemDesc_h__
#define __components_devices_shmem_CNShmemDesc_h__

#include "Arch.h"
#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); fprintf x
#endif

#include "components/atomic/bgq/L2Counter.h"

#define SHORT_MSG_CUTOFF      8192
//#define SHORT_MSG_CUTOFF      4096
//#define SHORT_MSG_CUTOFF      2048
////#define SHORT_MSG_CUTOFF      16384
#define VERY_SHORT_MSG_CUTOFF   128
//#define CN_SHMEM_TASKS_PER_NODE 16
#define CN_SHMEM_TASKS_PER_NODE 64

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {

      //Control structure holding all the addreses and 
      //state of the operation
      struct ControlBlock
      {
        volatile char         buffer[SHORT_MSG_CUTOFF];
        struct
        {
          void* srcbufs[CN_SHMEM_TASKS_PER_NODE];
          void* dstbufs[CN_SHMEM_TASKS_PER_NODE];
        }                     GAT;
        void*                 phybufs[CN_SHMEM_TASKS_PER_NODE];
        volatile int32_t     chunk_done[CN_SHMEM_TASKS_PER_NODE];
        volatile uint64_t     bytes_incoming;
      } __attribute__((__aligned__(128)));


      struct  CNShmemRegion 
      {
        volatile  char	        buffer[sizeof(ControlBlock)];
        volatile  unsigned      num_consumers;
        volatile  unsigned      flag;

        public:

        inline CNShmemRegion()
        { 
          num_consumers = __global.topology_local.size();
          flag = 0;
        }
        inline ~CNShmemRegion()
        {}
      }__attribute__((__aligned__(128))); 

      static CNShmemRegion*  _cn_shmem_region = NULL;

      //This descriptor holds the shared and atomic regions and access methods
      //to them during the operation
      class CNShmemDesc
      {

        public:

          struct  atomics_region
          {
            PAMI::Counter::BGQ::IndirectL2	synch_counter;	/* Whether everyone has arrived */
            PAMI::Counter::BGQ::IndirectL2	done_counter; 	/* Whether everyone finished 	*/
          };


        private:

          atomics_region        _atomics;
          CNShmemRegion         *_shared;

        public:

          inline CNShmemDesc() {}

          inline CNShmemDesc(Memory::MemoryManager &mm, size_t clientid, size_t contextid, size_t usageid, size_t index): _shared(NULL)
        {

          char key[PAMI::Memory::MMKEYSIZE];
          sprintf(key, "/CNShmemDesc-synch-%zd-%zd-%zd-%zd", clientid, contextid, usageid, index);
          _atomics.synch_counter.init(&mm, key);
          sprintf(key, "/CNShmemDesc-done-%zd-%zd-%zd-%zd", clientid, contextid, usageid, index);
          _atomics.done_counter.init(&mm, key);

          _shared = _cn_shmem_region + index;

        };
          inline ~CNShmemDesc() {}

          inline void reset()
          {
            _atomics.synch_counter.fetch_and_clear ();
            _atomics.done_counter.fetch_and_clear ();
            _shared->flag = 0;
            _shared->num_consumers = __global.topology_local.size();;
          }


          inline void signal_done()
          {
            _atomics.done_counter.fetch_and_inc();
            TRACE_ERR((stderr, "Done counter:%zu\n", _atomics.done_counter.fetch() ));
          }


          inline unsigned in_use()
          {
            if (_atomics.done_counter.fetch() == _shared->num_consumers) { return 0;}
            return 1;
          }

          inline void signal_flag()
          {
            ++_shared->flag;
          }

          inline unsigned get_flag()
          {
            return _shared->flag;
          }

          inline void* get_buffer(unsigned index)
          {
            return (void*)(_shared->buffer + index*(SHORT_MSG_CUTOFF/_shared->num_consumers));
            //return (void*)(_shared->buffer + index*VERY_SHORT_MSG_CUTOFF);
          }

          inline void* get_buffer()
          {
            return (void*)_shared->buffer;
          }
          inline unsigned arrived_peers()
          {
            return _atomics.synch_counter.fetch();
          }
          inline unsigned done_peers()
          {
            return _atomics.done_counter.fetch();
          }

          inline void signal_arrived()
          {
            _atomics.synch_counter.fetch_and_inc();
          }

          inline void set_consumers(unsigned consumers)
          {
            _shared->num_consumers = consumers;
            TRACE_ERR((stderr, "num_consumers:%u\n", consumers ));
          }

          inline void set_cn_shmem_region(CNShmemRegion* shmemreg)
          {
            _shared = shmemreg;
          }

      };

    }
  }
}

#endif
