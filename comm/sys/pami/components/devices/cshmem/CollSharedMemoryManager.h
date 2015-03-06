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
///
/// \file components/devices/cshmem/CollSharedMemoryManager.h
/// \brief Shared memory buffer manager class for CollShm device
///
/// The class defined in this file uses C++ templates. C++ templates
/// require all source code to be #include'd from a header file.
///
#ifndef __components_devices_cshmem_CollSharedMemoryManager_h__
#define __components_devices_cshmem_CollSharedMemoryManager_h__

/// \todo #warning This should move into the Coll/CCMI area - it is not PAMI::Memory!

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "Platform.h"
#include "Memory.h"

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x)  //fprintf x
#endif

#undef TRACE_DBG
#ifndef TRACE_DBG
#define TRACE_DBG(x)  //fprintf x
#endif

#ifndef PAMI_ASSERT
#define PAMI_ASSERT(x) PAMI_assert(x)
#endif

namespace PAMI
{
  namespace Memory
  {

    ///
    /// \brief Memory manager class for collective shared memory optimization
    ///        The shared memory region is composed of a contorl region, a region for
    ///        control structures of collective operations, and a region for data
    ///        buffers of collective operations.
    ///        T_SegSz is the size of the entire region managered by the memory manager
    ///        T_PageSz is the page size backing the shared memory region
    ///        T_WindowSz is the size of allocation unit for collective operation control structures
    ///        T_ShmBufSz is the size of shared memory data buffers




    template <class T_Atomic, class T_Mutex, class T_Counter, 
      unsigned T_CtlCnt,
      unsigned T_BufCnt,
      unsigned T_LgBufCnt,
      unsigned T_WindowSz,//ctrl structure size
      unsigned T_ShmBufSz,
      unsigned T_ShmLgBufSz>
    class CollSharedMemoryManager
    {
      public:

        static const size_t _size     = CACHEBLOCKSZ + 
                                        T_CtlCnt   * T_WindowSz + 
                                        T_BufCnt   * T_ShmBufSz + 
                                        T_LgBufCnt * T_ShmLgBufSz;

        static const size_t _ctl_cnt    = T_CtlCnt;
        static const size_t _buf_cnt    = T_BufCnt;
        static const size_t _lg_buf_cnt = T_LgBufCnt;

        static const size_t _windowsz = T_WindowSz;
        static const size_t _shmbufsz = T_ShmBufSz;
        static const size_t _lgbufsz  = T_ShmLgBufSz;

        inline size_t addr_to_offset(void *addr)
        {
          return (size_t)addr - (size_t)_collshm;
        }
        inline void *offset_to_addr(size_t offset)
        {
          return (void*)((char*)_collshm + offset);
        }
        inline void *shm_null_ptr()
        {
          return _end;
        }
        inline size_t shm_null_offset()
        {
          return _size;
        }
        union shm_ctrl_str_t
        {
          char     ctrl[_windowsz];
          size_t   next_offset;
        } __attribute__ ((__aligned__ (CACHEBLOCKSZ)));
        typedef shm_ctrl_str_t ctlstr_t;

        union shm_data_buf_t
        {
          char           data[_shmbufsz];
          size_t         next_offset;
        } __attribute__ ((__aligned__ (CACHEBLOCKSZ)));
        typedef shm_data_buf_t databuf_t;

        union shm_large_data_buf_t
        {
          char           data[_lgbufsz];
          size_t         next_offset;
        } __attribute__ ((__aligned__ (CACHEBLOCKSZ)));
        typedef shm_large_data_buf_t large_databuf_t;

        /*
          The shared memory available in pami is split in three
          regions: control structures pool, large data buffers pool
          (pgas hybryd algos) and data buffer pool;
         */
        struct CollSharedMemory
        {
          T_Mutex             pool_lock;
          T_Atomic            clientdata_key;
          size_t              ctlstr_offset;
          size_t              buffer_offset;
          size_t              large_buffer_offset;
          volatile size_t     free_ctlstr_list_offset;
          size_t              ctlstr_list_offset;
          size_t              ctlstr_pool_offset;
          size_t              ctlstr_pool_init_offset;
          volatile size_t     free_buffer_list_offset;
          size_t              buffer_list_offset;
          size_t              buffer_pool_offset;
          volatile size_t     free_large_buffer_list_offset;
          size_t              large_buffer_list_offset;
          size_t              large_buffer_pool_offset;
        } __attribute__ ((__aligned__ (CACHEBLOCKSZ)));
        typedef CollSharedMemory collshm_t;

        CollSharedMemoryManager (Memory::MemoryManager *mm) :
          _nctrlstrs(0),
          _ndatabufs(0),
          _nlargedatabufs(0),
          _start(NULL),
          _end(NULL),
          _collshm (NULL),
          _localrank(-1),
          _localsize(-1),
          _key(0),
          _mm(mm)
        { }

        static void _collshminit(void *mem, size_t bytes, const char *key, unsigned attrs, void *cookie)
        {
          CollSharedMemoryManager *thus = (CollSharedMemoryManager *)cookie;
          thus->__collshminit(mem, bytes, key, attrs);
        }

        void __collshminit(void *mem, size_t bytes, const char *key, unsigned attrs)
        {

          // both shm_open and shmget should guarantee initial
          // content of the shared memory to be zero
          // _collshm->pool_lock = 0;

          typedef union coll_block_t
          {
            char _collshmblob[sizeof(collshm_t)];
            char _cacheline[CACHEBLOCKSZ];
          }coll_block_t;
          // Assert that the shmem lists fit in a cache line
          COMPILE_TIME_ASSERT(sizeof(coll_block_t) == CACHEBLOCKSZ);
          // Assert that the control structures are cache line sized
          COMPILE_TIME_ASSERT((sizeof(ctlstr_t)%CACHEBLOCKSZ) == 0);

          // Control struct pool starts after the shmem lists
          _collshm->ctlstr_pool_offset      = addr_to_offset(((char  *)_collshm + sizeof(coll_block_t)));
          _collshm->ctlstr_pool_init_offset = _collshm->ctlstr_pool_offset;

          // Large Data buffer pool starts after _init_ctl_cnt of control structs
          _collshm->large_buffer_pool_offset      = _collshm->ctlstr_pool_offset+(sizeof(ctlstr_t)*_ctl_cnt);

          // Data buffer pool starts after large data buffer allocation
          // Rest of the space for data bufs
          _collshm->buffer_pool_offset      = _collshm->ctlstr_pool_offset+(sizeof(ctlstr_t)*_ctl_cnt) + (sizeof(large_databuf_t)*_lg_buf_cnt);

          _collshm->clientdata_key.set(_key);
          _collshm->ctlstr_offset           = (size_t)offset_to_addr(_collshm->ctlstr_pool_offset);
          _collshm->large_buffer_offset     = (size_t)offset_to_addr(_collshm->large_buffer_pool_offset);
          _collshm->buffer_offset           = (size_t)offset_to_addr(_collshm->buffer_pool_offset);

          _collshm->free_ctlstr_list_offset = addr_to_offset(_get_ctrl_str_from_pool());
          _collshm->ctlstr_list_offset      = (size_t)addr_to_offset(new (offset_to_addr(_collshm->free_ctlstr_list_offset)) T_Atomic());

          _collshm->free_buffer_list_offset = addr_to_offset(_get_data_buf_from_pool());
          _collshm->buffer_list_offset      = addr_to_offset(new (offset_to_addr(_collshm->free_buffer_list_offset)) T_Atomic());

          _collshm->free_large_buffer_list_offset = addr_to_offset(_get_large_data_buf_from_pool());
          _collshm->large_buffer_list_offset      = addr_to_offset(new (offset_to_addr(_collshm->free_large_buffer_list_offset)) T_Atomic());


          TRACE_DBG((stderr, "_collshm                  %p,    "
                     "getSGCtrlStrVec() _localsize      %lu, "
                     "_collshm->ctlstr_pool_offset      %lu, "
                     "_collshm->buffer_pool_offset      %lu, "
                     "_collshm->ctlstr_offset           %lu, "
                     "_collshm->buffer_offset           %lu, "
                     "_collshm->free_ctlstr_list_offset %lu, "
                     "_collshm->ctlstr_list_offset      %lu, "
                     "_collshm->free_buffer_list_offset %lu, "
                     "_collshm->buffer_list_offset      %lu  "
                     "\n",
                     _collshm,
                     _localsize,
                     _collshm->ctlstr_pool_offset      ,
                     _collshm->buffer_pool_offset      ,
                     _collshm->ctlstr_offset    ,
                     _collshm->buffer_offset    ,
                     _collshm->free_ctlstr_list_offset ,
                     _collshm->ctlstr_list_offset      ,
                     _collshm->free_buffer_list_offset ,
                     _collshm->buffer_list_offset));
        }

        pami_result_t init(size_t rank, size_t size)
        {
          pami_result_t rc = PAMI_SUCCESS;
          _localrank = rank;
          _localsize = size;
          if(_mm)
          {
            PAMI_assertf(T_Counter::checkCtorMm(_mm), "Memory type incorrect for T_Counter class");
            PAMI_assertf(T_Mutex::checkCtorMm(_mm), "Memory type incorrect for T_Mutex class");            
            rc = _mm->memalign((void **)&_collshm, CACHEBLOCKSZ, _size, "/pami-collshmem",
                               _collshminit, (void *)this);
            _start = (void*)_collshm;
            _end   = (void*)((char*)_collshm + _size);
          }
          else
            rc = PAMI_ENOMEM;

          return rc;
        }

        pami_result_t init(size_t rank, size_t size, uint64_t key)
        {
          _key = key;
          return init(rank, size);
        }

        void * getCollShmAddr()
        {
          return (void*) _collshm;
        }

        ~CollSharedMemoryManager()
        {
          // placeholder for other cleanup work
          if (_collshm != NULL)
            _mm->free(_collshm);
        }
        inline pami_result_t allocateKey()
        {
          pami_result_t rc = PAMI_SUCCESS;

          // Note that this lock is not required for correctness
          // But we use it for consistency/error checking
          // (the critical section allows us to fetch and dec atomically
          // if the key is 0).
          _collshm->pool_lock.acquire();
          ssize_t cv, val = _collshm->clientdata_key.fetch_and_sub(1UL);
          cv              = val - 1;
          if(val < 1)
          {
            val = _collshm->clientdata_key.fetch_and_add(1UL);
            cv  = val + 1;
            rc  = PAMI_EAGAIN;
          }
          PAMI_assertf((cv >= 0 && cv <= _key),
                       "allocateKey outside range:  %zd %lld \n",
                       cv,
                       (long long int)_key);
          _collshm->pool_lock.release();
          return rc;
        }
        inline pami_result_t returnKey()
        {
          pami_result_t rc = PAMI_SUCCESS;

          // Note that this lock is not required for correctness
          // But we use it for consistency/error checking
          _collshm->pool_lock.acquire();
          ssize_t cv, val = _collshm->clientdata_key.fetch_and_add(1UL);
          cv              = val + 1;
          PAMI_assertf((val >= 0 && val <= _key),
                       "returnKey outside range: %zd %lld \n",
                       val,
                       (long long)_key);
          _collshm->pool_lock.release();
          return rc;
        }

        // Large data buffers pool management

        large_databuf_t * _get_large_data_buf_from_pool ()
        {
          _collshm->pool_lock.acquire();
          Memory::sync<Memory::instruction>();
          large_databuf_t *new_bufs = (large_databuf_t*) offset_to_addr(_collshm->large_buffer_pool_offset);
          large_databuf_t *bufs     = new_bufs;
          size_t    end      = _collshm->buffer_pool_offset;//the end for large buffer is where normal buffers start

          if ((char *)(new_bufs + _lg_buf_cnt) > ((char *)_collshm + end))
            {
              TRACE_ERR((stderr, "Run out of shm data bufs, base=%p, buffer_offset=%lu, boundary=%p, end=%p\n",
                         _collshm, _collshm->large_buffer_offset, (char *)_collshm + _size,
                         (char *)(new_bufs + 1)));
              Memory::sync();
              _collshm->pool_lock.release();
              return (large_databuf_t*)shm_null_ptr();
            }

          for (size_t i = 0; i < _lg_buf_cnt - 1; ++i)
            {
              new_bufs->next_offset = addr_to_offset(new_bufs + 1);
              new_bufs              = (large_databuf_t*)offset_to_addr(new_bufs->next_offset);
              PAMI_ASSERT(((uintptr_t)new_bufs&(CACHEBLOCKSZ-1UL)) == 0);
            }
          new_bufs->next_offset = shm_null_offset();
          _collshm->large_buffer_pool_offset += (_lg_buf_cnt*sizeof(large_databuf_t));

          Memory::sync();
          _collshm->pool_lock.release();
          return bufs;
        }


        large_databuf_t *getLargeDataBuffer (unsigned count)
        {
          PAMI_ASSERT(count <= _lg_buf_cnt);
          unsigned buf_count = 0;
          large_databuf_t *cur;
          large_databuf_t *next, *tmp, *buffers = (large_databuf_t*)shm_null_ptr();

          T_Atomic *large_buffer_list = (T_Atomic*)offset_to_addr(_collshm->large_buffer_list_offset);
          while (buf_count < count)
            {
              cur = (large_databuf_t *)offset_to_addr(large_buffer_list->fetch());
              if (cur == shm_null_ptr())
                {
                  tmp   = _get_large_data_buf_from_pool();   // Allocate a whold chunk of INIT_BUFCNT new buffers from the pool
                  if (tmp == (large_databuf_t*)shm_null_ptr()){
                    //no more space; we return the null ptr 
                    return tmp;
                  }
                  cur   = tmp + count - buf_count - 1; // End of the list satisfying the requirement
                  next  = (large_databuf_t*)offset_to_addr(cur->next_offset);   // Extra buffers that should be put into free list

                  cur->next_offset = addr_to_offset(buffers);                 // Merge with buffers already allocated
                  buffers = tmp;

                  cur = tmp + _lg_buf_cnt - 1;            // End of the newly allocated chunk
                  // Merge with free list
                  cur->next_offset = large_buffer_list->fetch();

                  while(!large_buffer_list->bool_compare_and_swap(cur->next_offset, addr_to_offset(next)))
                    {
                      cur->next_offset = large_buffer_list->fetch();
                    }

                  buf_count = count;
                  TRACE_DBG((stderr, "new buffer is %p\n", buffers));
                  continue;
                }

              next = (large_databuf_t*)offset_to_addr(cur->next_offset);
              TRACE_DBG((stderr, "start: cur = %p, cur->next = %p and _collshm->free_buffer_list_offset = %lu\n",
                         cur, next, _collshm->free_large_buffer_list_offset));

              while (!large_buffer_list->bool_compare_and_swap(addr_to_offset(cur), addr_to_offset(next)))
                {
                  TRACE_DBG((stderr, "entry cur = %p, cur->next = %p and _collshm->free_buffer_list_offset = %lu\n",
                             cur, next, _collshm->free_buffer_list_offset));

                  cur = (large_databuf_t*)offset_to_addr(large_buffer_list->fetch());
                  if (cur == shm_null_ptr())
                    next = (large_databuf_t*)shm_null_ptr();  // take care of the case in which free list becomes empty
                  else
                    next = (large_databuf_t*)offset_to_addr(cur->next_offset);
                  TRACE_DBG((stderr, "exit cur = %p, cur->next = %p and _collshm->large_free_buffer_list_offset = %lu\n",
                             cur, next, _collshm->large_free_buffer_list_offset));
                }

              if (cur == shm_null_ptr()) continue;  // may need to start over

              TRACE_DBG((stderr, "end cur = %p\n", cur));
              cur->next_offset = addr_to_offset(buffers);
              buffers = (large_databuf_t *)cur;
              buf_count ++;
            }

          if(buffers != (large_databuf_t*)shm_null_ptr()) _nlargedatabufs += count;
          TRACE_DBG((stderr, "_nlargedatabufs = %zu\n", _nlargedatabufs));
          return buffers;
        }


        ///
        /// \brief Return a list of shm data buf to the free list
        ///
        /// \param data_buf pointer to data bufs returned
        ///
        void returnLargeDataBuffer (large_databuf_t *data_buf)
        {
          PAMI_ASSERT(data_buf != shm_null_ptr());
          large_databuf_t *tmp = data_buf;
          T_Atomic  *large_buffer_list = (T_Atomic*)offset_to_addr(_collshm->large_buffer_list_offset);
          while (tmp->next_offset != shm_null_offset())
            {
              tmp = (large_databuf_t*)offset_to_addr(tmp->next_offset);
              --_nlargedatabufs;
            }
          tmp->next_offset = large_buffer_list->fetch();
          while(!large_buffer_list->bool_compare_and_swap(tmp->next_offset, addr_to_offset(tmp)))
            {
              tmp->next_offset = large_buffer_list->fetch();
            }
          TRACE_DBG((stderr, "_nlargedatabufs = %d\n", _nlargedatabufs));
        }

        ///
        /// \brief Get a whole chunk of INIT_BUFCNT new data buffers from the pool
        ///        Hold buffer pool lock.
        ///
        /// \return A chunk of INIT_BUFCNT new data buffers chained together
        ///

        databuf_t * _get_data_buf_from_pool ()
        {
          _collshm->pool_lock.acquire();
          Memory::sync<Memory::instruction>();
          databuf_t *new_bufs = (databuf_t*) offset_to_addr(_collshm->buffer_pool_offset);
          databuf_t *bufs     = new_bufs;

          if ((char *)(new_bufs + _buf_cnt) > ((char *)_collshm + _size))
            {
              fprintf(stderr, "Run out of shm data bufs, base=%p, buffer_offset=%zu, boundary=%p, end=%p\n",
                         _collshm, _collshm->buffer_offset, (char *)_collshm + _size,
                         (char *)(new_bufs + _buf_cnt));
              PAMI_ASSERT(0);
              return (databuf_t*)shm_null_ptr();
            }

          for (size_t i = 0; i < _buf_cnt - 1; ++i)
            {
              new_bufs->next_offset = addr_to_offset(new_bufs + 1);
              new_bufs              = (databuf_t*)offset_to_addr(new_bufs->next_offset);
              PAMI_ASSERT(((uintptr_t)new_bufs&(CACHEBLOCKSZ-1UL)) == 0);
            }
          new_bufs->next_offset = shm_null_offset();
          _collshm->buffer_pool_offset += (_buf_cnt*sizeof(databuf_t));

          Memory::sync();
          _collshm->pool_lock.release();
          return bufs;
        }


        ///
        /// \brief get shm data buffers either from the free list or from the pool
        ///        when free list is empty. No lock is held, rely on atomic operation
        ///        to guarantee integrity
        ///
        /// \param count Number of buffers requested
        ///
        /// \return point to list of shmem data buffers
        ///

        databuf_t *getDataBuffer (unsigned count)
        {
          PAMI_ASSERT(count <= _buf_cnt);
          unsigned buf_count = 0;
          databuf_t *cur;
          databuf_t *next, *tmp, *buffers = (databuf_t*)shm_null_ptr();

          T_Atomic *buffer_list = (T_Atomic*)offset_to_addr(_collshm->buffer_list_offset);
          while (buf_count < count)
            {
              cur = (databuf_t *)offset_to_addr(buffer_list->fetch());
              if (cur == shm_null_ptr())
                {
                  tmp   = _get_data_buf_from_pool();   // Allocate a whold chunk of INIT_BUFCNT new buffers from the pool
                  cur   = tmp + count - buf_count - 1; // End of the list satisfying the requirement
                  next  = (databuf_t*)offset_to_addr(cur->next_offset);   // Extra buffers that should be put into free list

                  cur->next_offset = addr_to_offset(buffers);                 // Merge with buffers already allocated
                  buffers = tmp;

                  cur = tmp + _buf_cnt - 1;            // End of the newly allocated chunk
                  // Merge with free list
                  cur->next_offset = buffer_list->fetch();

                  while(!buffer_list->bool_compare_and_swap(cur->next_offset, addr_to_offset(next)))
                    {
                      cur->next_offset = buffer_list->fetch();
                    }

                  buf_count = count;
                  TRACE_DBG((stderr, "new buffer is %p\n", buffers));
                  continue;
                }

              next = (databuf_t*)offset_to_addr(cur->next_offset);
              TRACE_DBG((stderr, "start: cur = %p, cur->next = %p and _collshm->free_buffer_list_offset = %lu\n",
                         cur, next, _collshm->free_buffer_list_offset));

              while (!buffer_list->bool_compare_and_swap(addr_to_offset(cur), addr_to_offset(next)))
                {
                  TRACE_DBG((stderr, "entry cur = %p, cur->next = %p and _collshm->free_buffer_list_offset = %lu\n",
                             cur, next, _collshm->free_buffer_list_offset));

                  cur = (databuf_t*)offset_to_addr(buffer_list->fetch());
                  if (cur == shm_null_ptr())
                    next = (databuf_t*)shm_null_ptr();  // take care of the case in which free list becomes empty
                  else
                    next = (databuf_t*)offset_to_addr(cur->next_offset);
                  TRACE_DBG((stderr, "exit cur = %p, cur->next = %p and _collshm->free_buffer_list_offset = %lu\n",
                             cur, next, _collshm->free_buffer_list_offset));
                }

              if (cur == shm_null_ptr()) continue;  // may need to start over

              TRACE_DBG((stderr, "end cur = %p\n", cur));
              cur->next_offset = addr_to_offset(buffers);
              buffers = (databuf_t *)cur;
              buf_count ++;
            }

          _ndatabufs += count;
          TRACE_DBG((stderr, "_ndatabufs = %zu\n", _ndatabufs));
          return buffers;
        }


        ///
        /// \brief Return a list of shm data buf to the free list
        ///
        /// \param data_buf pointer to data bufs returned
        ///

        void returnDataBuffer (databuf_t *data_buf)
        {
          PAMI_ASSERT(data_buf != shm_null_ptr());

          databuf_t *tmp = data_buf;
          T_Atomic  *buffer_list = (T_Atomic*)offset_to_addr(_collshm->buffer_list_offset);
          while (tmp->next_offset != shm_null_offset())
            {
              tmp = (databuf_t*)offset_to_addr(tmp->next_offset);
              --_ndatabufs;
            }
          tmp->next_offset = buffer_list->fetch();
          while(!buffer_list->bool_compare_and_swap(tmp->next_offset, addr_to_offset(tmp)))
            {
              tmp->next_offset = buffer_list->fetch();
            }

          TRACE_DBG((stderr, "_ndatabufs = %d\n", _ndatabufs));

        }

        ///
        /// \brief Get a whole chunk of INIT_CTLCNT new ctrl struct from the pool
        ///        Hold buffer pool lock.
        ///
        /// \return A chunk of INIT_CTLCNT new ctrl struct chained together
        ///

      inline void ctl_str_check(void* in)
        {
          ctlstr_t *ctlstr = (ctlstr_t*)in;

          if(addr_to_offset(ctlstr) >=
             _collshm->ctlstr_pool_init_offset+(_ctl_cnt*sizeof(*ctlstr)))
          {
            fprintf(stderr, "Control String=%p too large(offset=%zd), start=%zd end=%zd\n",
                    in,
                    addr_to_offset(ctlstr),
                    _collshm->ctlstr_pool_init_offset,
                    _collshm->ctlstr_pool_init_offset+(_ctl_cnt*sizeof(*ctlstr)));            
          }
          if(addr_to_offset(ctlstr) < _collshm->ctlstr_pool_init_offset)
          {
            fprintf(stderr, "Control String=%p too small(offset=%zd), start=%zd end=%zd\n",
                    ctlstr,
                    addr_to_offset(ctlstr),
                    _collshm->ctlstr_pool_init_offset,
                    _collshm->ctlstr_pool_init_offset+(_ctl_cnt*sizeof(*ctlstr)));
          }
          PAMI_ASSERT(addr_to_offset(ctlstr) <
                      _collshm->ctlstr_pool_init_offset+(_ctl_cnt*sizeof(*ctlstr)));
          PAMI_ASSERT(addr_to_offset(ctlstr) > _collshm->ctlstr_pool_init_offset);
          
        }
      
        ctlstr_t * _get_ctrl_str_from_pool ()
        {
          // require implementation of check_lock and clear_lock in atomic class
          PAMI_ASSERT(&_collshm->pool_lock != NULL);
          _collshm->pool_lock.acquire();
          // is Memory::sync<Memory::instruction>() equivalent to isync() on PERCS ?
          Memory::sync<Memory::instruction>();
          ctlstr_t *ctlstr   = (ctlstr_t*)offset_to_addr(_collshm->ctlstr_pool_offset);
          ctlstr_t *tmp      = ctlstr;
          size_t    end      = _collshm->large_buffer_pool_offset;

          if ((char *)(ctlstr + _ctl_cnt) > ((char *)_collshm + end))
            {
              fprintf(stderr, "Run out of shm ctrl structs: base=%p, ctrl_offset=%zu, boundary=%p, end=%p\n",
                      _collshm, _collshm->ctlstr_offset, (char *)_collshm + end,
                      (char *)(ctlstr + _ctl_cnt));
              PAMI_ASSERT(0);
              return (ctlstr_t*)shm_null_ptr();
            }
          for (size_t i = 0; i < _ctl_cnt - 1; ++i)
            {
              tmp->next_offset = addr_to_offset(tmp + 1);
              tmp              = (ctlstr_t*)offset_to_addr(tmp->next_offset);
              PAMI_ASSERT(((uintptr_t)tmp&(CACHEBLOCKSZ-1UL)) == 0);
            }
          tmp->next_offset = shm_null_offset();

          _collshm->ctlstr_pool_offset += (_ctl_cnt*sizeof(ctlstr_t));
          //COLLSHM_CLEAR_LOCK((atomic_p)&(_collshm->pool_lock),0);
          Memory::sync();
          _collshm->pool_lock.release();
          return ctlstr;
        }

        ///
        /// \brief get shm ctrl structs either from the free list or from the pool
        ///        when free list is empty. No lock is held, rely on atomic operation
        ///        to guarantee integrity
        ///
        /// \param count Number of ctrl structs requested
        ///
        /// \return point to list of shmem ctrl structs
        ///

        ctlstr_t *getCtrlStr (unsigned count)
        {
          PAMI_ASSERT(count <= _ctl_cnt);
          unsigned ctlstr_count = 0;
          ctlstr_t * cur;
          ctlstr_t *next, *tmp, *ctlstr = (ctlstr_t*)shm_null_ptr();

          T_Atomic *ctlstr_list = (T_Atomic *) offset_to_addr(_collshm->ctlstr_list_offset);
          while (ctlstr_count < count)
            {
              PAMI_ASSERT(_collshm != NULL);
              PAMI_ASSERT(ctlstr_list != shm_null_ptr());
              cur = (ctlstr_t *)offset_to_addr(ctlstr_list->fetch());

              if (cur == shm_null_ptr())
                {
                  tmp   = _get_ctrl_str_from_pool();       // Allocate a whold chunk of INIT_CTLCNT new buffers from the pool
                  cur   = tmp + count - ctlstr_count - 1;  // End of the list satisfying the requirement
                  next  = (ctlstr_t*)offset_to_addr(cur->next_offset);// Extra buffers that should be put into free list

                  cur->next_offset = addr_to_offset(ctlstr);                    // Merge with ctrl structs already allocated
                  ctlstr   = tmp;

                  cur = tmp + _ctl_cnt - 1;             // End of the newly allocated chunk
                  // Merge with free list
                  cur->next_offset = ctlstr_list->fetch();

                  while(!ctlstr_list->bool_compare_and_swap(cur->next_offset, addr_to_offset(next)))
                    {
                      cur->next_offset = ctlstr_list->fetch();
                    }

                  ctlstr_count = count;
                  continue;
                }

              next = (ctlstr_t*)offset_to_addr(cur->next_offset);

              while (!ctlstr_list->bool_compare_and_swap(addr_to_offset(cur), addr_to_offset(next)))
                {
                  cur = (ctlstr_t*)offset_to_addr(ctlstr_list->fetch());

                  if (cur == shm_null_ptr())
                    next = (ctlstr_t*)shm_null_ptr();  // take care of the case in which free list becomes empty
                  else
                    next = (ctlstr_t*)offset_to_addr(cur->next_offset);

                  TRACE_DBG((stderr, "cur = %p\n", cur));
                }

              if (cur == shm_null_ptr()) continue;  // may need to start over

              cur->next_offset = addr_to_offset(ctlstr);
              ctlstr = (ctlstr_t *)cur;
              ctlstr_count ++;
            }

          _nctrlstrs += count;
          TRACE_DBG((stderr, "_nctrlstrs = %zu, ctlstr %p\n", _nctrlstrs, ctlstr));
          return (ctlstr_t *)ctlstr;
        }

        ///
        /// \brief Return a list of shm ctrl struct to the free list
        ///
        /// \param ctlstr pointer to a list of ctrl structs to be returned
        ///
        void returnCtrlStr (ctlstr_t *ctlstr)
        {
          PAMI_ASSERT(ctlstr != shm_null_ptr());
          ctlstr_t *tmp         = ctlstr;
          T_Atomic *ctlstr_list = (T_Atomic *) offset_to_addr(_collshm->ctlstr_list_offset);

          // Decrement the number of control strings
          // and count the number in the list
          int cnt = 0;
          while (tmp != shm_null_ptr())
            { 
              tmp = (ctlstr_t*)offset_to_addr(tmp->next_offset);
              -- _nctrlstrs;
              cnt++;
            }
          ctlstr_t *cur=ctlstr;
          for(int i=0; i<cnt; i++)
          {
            ctlstr_t *next = (ctlstr_t*)offset_to_addr(cur->next_offset);
            memset(cur, 0, sizeof(*cur));
            Memory::sync();
            cur->next_offset = ctlstr_list->fetch();
            while(!ctlstr_list->bool_compare_and_swap(cur->next_offset, addr_to_offset(cur)))
            {
              cur->next_offset = ctlstr_list->fetch();
            }
            cur=next;
          }
          TRACE_DBG((stderr, "_nctrlstrs = %d\n", _nctrlstrs));
        }

        // get coll shmem control struture address for world geometry
        ctlstr_t *getWGCtrlStr()
        {
          TRACE_DBG((stderr, "_collshm %p \n", _collshm));
          TRACE_DBG((stderr, "WGCtrlStr = %p\n", ((ctlstr_t *)((char*)_collshm + _collshm->ctlstr_offset) + (_localsize-1))));
          return ((ctlstr_t *)((char*)_collshm + _collshm->ctlstr_offset)) + (_localsize-1);
        }

        // fill in a vector of coll shmem control structure address offsets for sub-geometries
        // perform allreduce on the vector during geometry analyze()
        void getSGCtrlStrVec(pami_geometry_t geo,
                             pami_endpoint_t my_ep,
                             uint64_t       *vec,
                             uint64_t       *ctlstr_offset)
        {

          PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)geo;
          PAMI::Topology *local_topo    = (PAMI::Topology *)geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
          PAMI::Topology *lm_topo       = (PAMI::Topology *)geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);

          uint master_rank              =  local_topo->index2Endpoint(0);
          uint master_size              =  lm_topo->size();
          uint master_index             =  lm_topo->endpoint2Index(master_rank);
          uint local_size               =  local_topo->size();
          uint local_index              =  local_topo->endpoint2Index(my_ep);
          TRACE_DBG((stderr, "getSGCtrlStrVec() geometry %p, master_rank %u, master_size %u, master_index %u, local_size %u, local_index %u\n",
                     geo, master_rank, master_size, master_index, local_size, local_index));

          for (uint i = 0; i < master_size; ++i) vec[i] = 0xFFFFFFFFFFFFFFFFULL;

          if (local_index == 0)
          {
            ctlstr_t *prev = (ctlstr_t*)shm_null_ptr(), * ctlstr = (ctlstr_t*)shm_null_ptr();
            for(uint i=0; i<local_size; i++)
            {
              ctlstr = getCtrlStr(1);
              ctlstr->next_offset = addr_to_offset(prev);
              ctl_str_check(ctlstr);
              prev = ctlstr;
            }
            vec[master_index] = (uint64_t)addr_to_offset(ctlstr);
          }
          *ctlstr_offset = vec[master_index];
        }
      
      void returnSGCtrlStr(uint64_t ctlstr_offset)
        {
          if(ctlstr_offset != 0xFFFFFFFFFFFFFFFFULL)
          {
            ctlstr_t *ctlstr = (ctlstr_t *)offset_to_addr(ctlstr_offset);
            // Acquire this lock to return all elements at once without contention
            // for the atomics
            _collshm->pool_lock.acquire();
            returnCtrlStr(ctlstr);
            _collshm->pool_lock.release();
          }
        }
      
      protected:
        size_t                    _nctrlstrs;
        size_t                    _ndatabufs;
        size_t                    _nlargedatabufs;
        void                     *_start;
        void                     *_end;
        collshm_t                *_collshm;       // base pointer of the shared memory segment
        size_t                    _localrank;      // rank in the local topology
        size_t                    _localsize;      // size of the local topology
        int64_t                   _key;
        Memory::MemoryManager    *_mm;

    };  // class CollSharedmemoryManager
  };  // namespace Memory
};   // namespace PAMI

#undef PAMI_ASSERT
#undef TRACE_ERR
#undef TRACE_DBG
#endif // __pami_components_devices_cshmem_collsharedmemorymanager_h__
