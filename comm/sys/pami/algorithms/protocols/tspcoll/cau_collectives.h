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
 * \file algorithms/protocols/tspcoll/cau_collectives.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_cau_collectives_h__
#define __algorithms_protocols_tspcoll_cau_collectives_h__

#ifdef XLPGAS_PAMI_CAU


#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <lapi.h>
#include <pami.h>

#include "algorithms/protocols/tspcoll/local/FixedLeader.h"

#ifndef RC0
#define RC0(S) { \
    int rc = S; \
    if (rc != 0) { \
        printf(#S " failed with rc %d, line %d\n", rc, __LINE__); \
        exit(-1); \
    } \
}
#endif

#define PGAS_SHMEM_CTRL_STRUCT 32768 // the size of ctrl structure for pgas hybrid algos

namespace xlpgas{
  template <class T_NI>
  class cau_device_info;

  template <class T_NI, class T_Device>
  void cau_fast_allreduce(lapi_handle_t h,
			  uint bg,
			  int myrank,
			  int64_t* dest, int64_t* src,
			  cau_reduce_op_t& op,
			  int ctxt,T_Device*, pami_context_t);

  template <class OPDT>
  size_t cau_dtype_size(const OPDT&  dtype){
    switch (dtype.operand_type)
    {
    case CAU_SIGNED_INT:   return sizeof(int32_t); break;
    case CAU_UNSIGNED_INT:   return sizeof(uint32_t); break;
    case CAU_SIGNED_LONGLONG:    return sizeof(int64_t);break;
    case CAU_UNSIGNED_LONGLONG:   return sizeof(uint64_t); break;
    case CAU_DOUBLE:    return sizeof(double); break;
    case CAU_FLOAT:    return sizeof(float); break;
    default: xlpgas_fatalerror (-1,"xlpgas::cau_dtype_size :: data type not supported");
    }
    return 0;
  }

  template <class OP, class DT>
  cau_reduce_op_t cau_op_dtype(OP      op,
			       DT   dtype){
    cau_reduce_op_t cau_op;
    switch (dtype)
    {
    case PAMI_SIGNED_INT:    cau_op.operand_type = CAU_SIGNED_INT; break;
    case PAMI_UNSIGNED_INT:   cau_op.operand_type = CAU_UNSIGNED_INT; break;
    case PAMI_SIGNED_LONG_LONG:    cau_op.operand_type = CAU_SIGNED_LONGLONG;break;
    case PAMI_UNSIGNED_LONG_LONG:   cau_op.operand_type = CAU_UNSIGNED_LONGLONG; break;
    case PAMI_DOUBLE:    cau_op.operand_type = CAU_DOUBLE; break;
    case PAMI_FLOAT:    cau_op.operand_type = CAU_FLOAT; break;
    default:
      cau_op.operand_type=0;
      xlpgas_fatalerror (-1,"xlpgas::cau_fast_allreduce :: data type not supported");
    }

  switch(op) {
    case PAMI_SUM:    cau_op.operation = CAU_SUM; break;
    case PAMI_BAND:   cau_op.operation = CAU_AND; break;
    case PAMI_BXOR:   cau_op.operation = CAU_XOR; break;
    case PAMI_MAX:    cau_op.operation = CAU_MAX; break;
    case PAMI_MIN:    cau_op.operation = CAU_MIN; break;
    case PAMI_NOOP:   cau_op.operation = CAU_NOP; break;
    default:
      cau_op.operation=0;
      xlpgas_fatalerror (-1, "xlpgas::cau_fast_allreduce :: OP not implemented");
    }
  return cau_op;
  }


  template<class CAU_RED_OP>
  void reduce_op(int64_t *dst, int64_t *src, const CAU_RED_OP& op, size_t nelems)
  {
    switch (op.operand_type) {
        case CAU_SIGNED_INT:
            reduce_fixed_point((int32_t *)dst, (int32_t *)src, op.operation, nelems);
            break;
        case CAU_UNSIGNED_INT:
            reduce_fixed_point((uint32_t *)dst, (uint32_t *)src, op.operation, nelems);
            break;
        case CAU_SIGNED_LONGLONG:
	    reduce_fixed_point((int64_t *)dst, (int64_t *)src, op.operation,nelems);
            break;
        case CAU_UNSIGNED_LONGLONG:
	    reduce_fixed_point((uint64_t *)dst, (uint64_t *)src, op.operation,nelems);
            break;
        case CAU_FLOAT:
	    reduce_floating_point((float *)dst, (float *)src, op.operation,nelems);
            break;
        case CAU_DOUBLE:
	    reduce_floating_point((double *)dst, (double *)src, op.operation,nelems);
            break;
        default: assert(!"Bogus reduce operand type");
    }
  }

  template <class T>
    void reduce_fixed_point(T *dst, T *src, unsigned func, size_t nelems)
    {
    switch (func) {
    case CAU_NOP: break;
    case CAU_SUM: for(size_t i=0;i<nelems;++i) dst[i] += src[i]; break;
    case CAU_MIN: for(size_t i=0;i<nelems;++i) dst[i] = (src[i] < dst[i] ? src[i] : dst[i]); break;
    case CAU_MAX: for(size_t i=0;i<nelems;++i) dst[i] = (src[i] > dst[i] ? src[i] : dst[i]); break;
    case CAU_AND: for(size_t i=0;i<nelems;++i) dst[i] &= src[i]; break;
    case CAU_XOR: for(size_t i=0;i<nelems;++i) dst[i] ^= src[i]; break;
    case CAU_OR:  for(size_t i=0;i<nelems;++i) dst[i] |= src[i]; break;
    default: assert(!"Bogus fixed-point reduce function");
    }
   }

  template <class T>
    void reduce_floating_point(T *dst, T *src, unsigned func, size_t nelems)
    {
      switch (func) {
      case CAU_NOP: break;
      case CAU_SUM: for(size_t i=0;i<nelems;++i) dst[i] += src[i]; break;
      case CAU_MIN: for(size_t i=0;i<nelems;++i) dst[i] = (src[i] < dst[i] ? src[i] : dst[i]); break;
      case CAU_MAX: for(size_t i=0;i<nelems;++i) dst[i] = (src[i] > dst[i] ? src[i] : dst[i]); break;
      default: assert(!"Bogus floating-point reduce function");
      }
    }


  /**
   * When interfacing with CAU we spread and compact the data; below
   * everything is spread to 64 bit data; so the only difference
   * between this version and reduce_op above is the cast for int32
   * case
   */
  template<class CAU_RED_OP>
  void sparse_reduce_op(int64_t *dst, int64_t *src, const CAU_RED_OP& op, size_t nelems)
  {
    switch (op.operand_type) {
        case CAU_SIGNED_INT:
            reduce_fixed_point((int64_t *)dst, (int64_t *)src, op.operation, nelems);
            break;
        case CAU_UNSIGNED_INT:
            reduce_fixed_point((uint64_t *)dst, (uint64_t *)src, op.operation, nelems);
            break;
        case CAU_SIGNED_LONGLONG:
	    reduce_fixed_point((int64_t *)dst, (int64_t *)src, op.operation,nelems);
            break;
        case CAU_UNSIGNED_LONGLONG:
	    reduce_fixed_point((uint64_t *)dst, (uint64_t *)src, op.operation,nelems);
            break;
        case CAU_FLOAT:
	    reduce_floating_point((float *)dst, (float *)src, op.operation,nelems);
            break;
        case CAU_DOUBLE:
	    reduce_floating_point((double *)dst, (double *)src, op.operation,nelems);
            break;
        default: assert(!"Bogus reduce operand type");
    }
  }


  /**
   * utilities to spread/compact data for CAU interaction
   * CAU only operates on 64 bit data
   */

  template <class T>
  void _spread_data(int64_t* data, size_t nelems){
    T* ud = (T*)data;
    for (int s=0;s<(int)nelems;++s) {
      data[nelems - s - 1] = ud[nelems-s-1];
    }
  }

  template <class T>
  void _compact_data(int64_t* data, size_t nelems){
    T* ud = (T*)data;
    for (int s=0;s<(int)nelems;++s) {
      ud[s] = data[s];
    }
  }

  template<class CAU_RED_OP>
  void spread_data(int64_t *dst, size_t nelems, const CAU_RED_OP& op)
  {
    switch (op.operand_type) {
        case CAU_SIGNED_INT:
	  _spread_data<int32_t>(dst, nelems);
          break;
        case CAU_UNSIGNED_INT:
   	  _spread_data<uint32_t>(dst, nelems);
          break;
        default: assert(!"Bogus spread operation requested");
    }
  }

  template<class CAU_RED_OP>
  void compact_data(int64_t *dst, size_t nelems, const CAU_RED_OP& op)
  {
    switch (op.operand_type) {
        case CAU_SIGNED_INT:
	  _compact_data<int32_t>(dst, nelems);
          break;
        case CAU_UNSIGNED_INT:
   	  _compact_data<uint32_t>(dst, nelems);
          break;
        default: assert(!"Bogus compact operation requested");
    }
  }

    /////////////////////////////////////////////////////////////////
 struct reduce_hdr_t {
   cau_reduce_op_t op;
   xlpgas::CollectiveKind      kind;
   int                 tag;
 };

template<class T_NI>
class CAUReduce: public CollExchange<T_NI>
  {
    typedef cau_device_info<T_NI> device_info_type;
    //synchronization vars
    bool    _done;
    long instance_id;
    long reduce_sent;
    long reduce_received;

    //data buf; one elem
    int64_t* reduce_data;
    int64_t temp_reduce_data[8];//temporary buffer for reduction
    int64_t* mcast_data; //output; typically used by the next mcast

    //op type
    cau_reduce_op_t cau_op;

    //header for am sends
    reduce_hdr_t  reduce_hdr;

    //cau/lapi specific
    lapi_handle_t lapi_handle;
    int  base_group_id;
    static int _dispatch_id;

  public:
    //boolean indicating if the data needs to be spread compacted
    //to match the 64 bit requirement of cau
    bool spread;

  public:
    void * operator new (size_t, void * addr) { return addr; }
    CAUReduce (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni);
    virtual bool isdone           () const;
    virtual void  kick            (void);
    virtual void reset (int rootindex,
                        const void         * sbuf,
                        void               * dbuf,
                        xlpgas_ops_t       op,
                        xlpgas_dtypes_t    dt,
                        unsigned           nelems,
                        user_func_t*       uf);
    static void register_dispatch(int* dispatch_id,lapi_handle_t);
    static void on_reduce_sent(lapi_handle_t *hndl, void *cookie) ;
    static void *recv_reduce(lapi_handle_t *hndl, void *uhdr, uint *uhdr_len,
			     ulong *msg_len, compl_hndlr_t **comp_h, void **uinfo);
  }; /* CAUreduce */

  struct mcast_hdr_t {
    xlpgas::CollectiveKind      kind;
    int                 tag;
  };


  template<class T_NI>
  class CAUBcast: public CollExchange<T_NI>
  {
    typedef cau_device_info<T_NI> device_info_type;

    int    _root;
    size_t _nbytes;
    bool _done;

    //data buffer
    int64_t* mcast_data;

    //synchronization vars
    long instance_id;
    long mcast_sent;
    long mcast_received;

    //header for am messages
    mcast_hdr_t  mcast_hdr;

    //cau/lapi sppecific
    lapi_handle_t lapi_handle;
    int  base_group_id;
    static int _dispatch_id;

  public:
    void * operator new (size_t, void * addr) { return addr; }
    CAUBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni);

    virtual bool isdone           () const;
    virtual void  kick            (void);
    virtual void reset (int rootindex,
                        const void         * sbuf,
                        void               * dbuf,
                        unsigned           nbytes);

    static void register_dispatch(int* dispatch_id,lapi_handle_t);
    static void on_mcast_sent(lapi_handle_t *hndl, void *cookie);
    static void *recv_mcast(lapi_handle_t *hndl, void *uhdr, uint *uhdr_len,
			    ulong *msg_len, compl_hndlr_t **comp_h, void **uinfo);

  }; /* CAUbcast */


  /* 
     The state must be 128 bytes total to be aligned with a cache
     size; This is tuned for PERCS; On other machines this value needs
     to be adjusted
   */
  struct _State
  {
    unsigned char buffer [124];
    int32_t counter;                   /*   4 bytes */
  }
  __attribute__((__aligned__(16)));


  struct pgas_shm_buffers{
    void* _reduce_buf;
    void* _bcast_buf;
    void* _large_bcast_buf;
    void* _large_bcast_data_buf;

    bool _valid;
    size_t _bcast_buf_sz;
    pgas_shm_buffers():_reduce_buf(NULL), 
                       _bcast_buf(NULL), 
                       _large_bcast_buf(NULL),
                       _large_bcast_data_buf(NULL),
                       _valid(false), _bcast_buf_sz(0){}
    bool valid(){
      return _valid;
    }
  };

    /**
       Hybrid pgas collectives uses shared memory within a node and
       either CAU or P2P across nodes; The manage_shmem_buffers class
       below is used to set up the necessary shmem buffers and cau
       group id; The shared memory buffer is stored with a key in
       geometry and it is computed in the CAUCollRegistration class;
     */

    template <class T_NI, class MM>
    void allocate_shmem_buf(xlpgas::pgas_shm_buffers& bfs, PAMI_GEOMETRY_CLASS* g, MM* mm, lapi_handle_t& _lapi_handle, uint64_t data_offset){
	if(mm != NULL) {
          PAMI::Topology* t = (PAMI::Topology*)(g->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX));
	  size_t local_tasks = t->size();

	  size_t state_sz = sizeof(_State);

	  size_t mem_sz = 3 * local_tasks * state_sz;
          //typically the shmbufsz is 32k; if more space is required
          //by a large number of threads we disable shmem collectives
	  if(mem_sz >= PGAS_SHMEM_CTRL_STRUCT) {
	    //memory required for all threads is bigger than the
	    //amount allocated; hybrid collectives will be disabled;
	    bfs._valid = false;
	    return;
	  }
         
          if(data_offset == mm->shm_null_offset()){
            //there were no more buffers; disable shared memory
            bfs._valid = false;
            return;
          }
 
	  uintptr_t pgas_shmem = (uintptr_t)mm->offset_to_addr(data_offset);
          
          // set the buffers starting at the address computed above
	  bfs._reduce_buf      = (void*)(pgas_shmem);
	  bfs._bcast_buf       = (void*)(pgas_shmem + local_tasks*state_sz);
	  bfs._large_bcast_buf = (void*)(pgas_shmem + 2 * local_tasks*state_sz);
          bfs._large_bcast_data_buf = (void*)(pgas_shmem + PGAS_SHMEM_CTRL_STRUCT );
	  bfs._bcast_buf_sz    = mm->_lgbufsz - PGAS_SHMEM_CTRL_STRUCT;

          bfs._valid = true;
	}
	else {
	  //hybrid disabled
	  bfs._valid = false;
	}
      }

  /**
   * Device specific info required by the hybrod cau/shmem collectives
   */
  template <class T_NI>
  class cau_device_info{
  public:
    int *_dispatch_id;
    int  _cau_group;
    lapi_handle_t _lapi_handle;
    int _reduce_dispatch_id;
    int _bcast_dispatch_id;
    pami_task_t _my_rank;
    PAMI_GEOMETRY_CLASS*   _geometry;
    pgas_shm_buffers       _bfs;

    void * operator new (size_t, void * addr) { return addr; }

    /**
       Default constructor; Init all to default values
    */
    cau_device_info() : _dispatch_id(NULL),
      _cau_group(-1),
      _lapi_handle(-1),
      _reduce_dispatch_id(-1),
      _bcast_dispatch_id(-1),_geometry(NULL) {}

    /**
     * Constructor with defined values
     */
    template <class T_CSMemoryManager>
    cau_device_info(int* dispatch_id,
                    int cau_group,
                    lapi_handle_t lapi_handle,
                    PAMI_GEOMETRY_CLASS* geometry,
                    T_CSMemoryManager* mm,
                    uint64_t data_offset) :
      _dispatch_id(dispatch_id),
      _cau_group(cau_group),
      _lapi_handle(lapi_handle),
      _geometry(geometry) {
      //set up the shared memory buffers
      allocate_shmem_buf<T_NI,T_CSMemoryManager>(_bfs,geometry,mm,lapi_handle,data_offset);
      _my_rank = _geometry->rank();
    }

    inline pami_task_t my_rank() const {
      return _my_rank;
    }

    inline int cau_group() const {
      return _cau_group;
    }

    inline lapi_handle_t lapi_handle() const {
      return _lapi_handle;
    }

    inline pgas_shm_buffers& shm_buffers(void) {
      return _bfs;
    }

    inline PAMI_GEOMETRY_CLASS* geometry(void) {
      return _geometry;
    }

  };
  
}//end namespace

#include "algorithms/protocols/tspcoll/cau_collectives.cc"

#endif  // XLPGAS_PAMI_CAU

#endif
