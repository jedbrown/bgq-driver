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
 * \file algorithms/protocols/tspcoll/Collective.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_Collective_h__
#define __algorithms_protocols_tspcoll_Collective_h__

#include <stdio.h>
#include <vector>
#include "algorithms/protocols/tspcoll/Array.h"
#include "algorithms/ccmi.h"
#include "sys/pami.h"
#include "common/type/TypeCode.h"
#include "common/type/TypeMachine.h"
#include "algorithms/protocols/allreduce/ReduceFunctions.h"
#include "TypeDefs.h"
// PAMI typedefs to avoid including pgas headers
typedef unsigned char*      xlpgas_local_addr_t;
typedef void *              user_func_t;
typedef unsigned            xlpgas_AMHeaderReg_t;
typedef pami_event_function xlpgas_event_function;
struct xlpgas_AMHeader_t
{
  xlpgas_AMHeaderReg_t  handler;    /**< header handler registration */
  unsigned              headerlen;  /**< total bytes in header       */
};
typedef pami_op                            xlpgas_ops_t;
typedef pami_dt                            xlpgas_dtypes_t;
typedef pami_endpoint_t                    xlpgas_endpoint_t;
typedef pami_event_function                xlpgas_LCompHandler_t;

using namespace PAMI::Type;
//typedef PAMI::Type::TypeCode               TypeCode;
//typedef pami_dispatch_ambroadcast_function PAMIAMHeader_bcast;
 typedef struct AMHeader_bcast
 {
   xlpgas_AMHeader_t   hdr;
   int                 senderID;
   int                 root;
   int                 dest_ctxt;
   int                 team_id;
   void*               parent_state;
   int                 len;
   xlpgas_AMHeaderReg_t user_cb;
 } AMHeader_bcast;
typedef struct AMHeader_reduce
{
  xlpgas_AMHeader_t   hdr;
  void*               parent_state;
  int                 senderID;
  int                 root;
  int                 dest_ctxt;
  int                 team_id;
  int                 len;
  xlpgas_dtypes_t     dt;
  xlpgas_ops_t        op;
  xlpgas_AMHeaderReg_t user_cb;
} AMHeader_reduce;

 typedef struct AMHeader_gather
 {
   xlpgas_AMHeader_t   hdr;
   void*               parent_state;
   int                 senderID;
   int                 root;
   int                 dest_ctxt;
   int                 team_id;
   int                 len;
   int                 rlen;
   xlpgas_AMHeaderReg_t user_cb;
 } AMHeader_gather;

#define XLPGAS_AMSEND_BARRIER       -1
#define XLPGAS_AMSEND_COLLEXCHANGE  -2
#define XLPGAS_TSP_AMSEND_COLLA2A   -3
#define XLPGAS_TSP_AMSEND_S         -4
#define XLPGAS_TSP_AMSEND_G         -5
#define XLPGAS_TSP_AMSEND_PERM      -6
#define XLPGAS_TSP_AMSEND_PSUM      -7
#define XLPGAS_TSP_AMSEND_COLLA2AV  -8
//for am collectives
#define XLPGAS_TSP_AMBCAST_PREQ       -9  // One sided collectives; parent request
#define XLPGAS_TSP_AMREDUCE_PREQ      -10
#define XLPGAS_TSP_AMREDUCE_CREQ      -11 // Child request
#define XLPGAS_TSP_AMGATHER_PREQ      -12
#define XLPGAS_TSP_AMGATHER_CREQ      -13 // Child request
#define XLPGAS_TSP_BCAST_TREE  -14

#define XLPGAS_MYNODE -1

static inline void xlpgas_fatalerror (int errcode, const char * strg, ...)
{
  char buffer[1024];
  va_list ap;
  va_start(ap, strg);
  vsnprintf (buffer, sizeof(buffer)-1, strg, ap);
  va_end(ap);
  if (errcode==0) errcode=-1;
  fprintf(stderr, "Collectives Runtime Error %d: %s\n",errcode,buffer);
  _exit(1);
}


namespace xlpgas
{

  typedef PAMI::Topology Team;

  /* *********************************************************** */
  /* *********************************************************** */

  enum CollectiveKind
    {
      BarrierKind=0,
      BarrierKind2,
      AllgatherKind,
      AllgathervKind,
      ScatterKind,
      ScattervKind,
      GatherKind,
      GathervKind,
      BcastKind,
      BcastKind2,
      BcastTreeKind,
      ShortAllreduceKind,
      LongAllreduceKind,
      AllreduceKind2,
      AlltoallKind,
      AlltoallvKind,
      AlltoallvintKind,
      PermuteKind,
      PrefixKind,
      AllreducePPKind,//from here down we have point to point versions
      BarrierPPKind,
      BcastPPKind,
      AllgatherPPKind,
      AlltoallPPKind,
#ifdef XLPGAS_PAMI_CAU
      SHMReduceKind,
      SHMBcastKind,
      CAUReduceKind,
      CAUBcastKind,
      SHMLargeBcastKind,
      ShmCauAllReduceKind,
      LeadersBcastKind,
      ShmHybridBcastKind,
      ShmHybridPipelinedBcastKind,
#endif
      MAXKIND
    };

  /* *********************************************************** */
  /*        a generic non-blocking transport collective          */
  /* *********************************************************** */
  template <class T_NI>
  class Collective
    {
    public:
      static void     Initialize();

    public:
      Collective (int                      ctxt,
		  Team           * comm,
		  CollectiveKind           kind,
		  int                      tag,
		  xlpgas_LCompHandler_t     cb_complete,
		  void                   * arg,
                  T_NI                   * ni);
      virtual ~Collective()
        {

        }

    public:
      int            tag  () const { return _tag; }
      int            kind    () const { return _kind; }
      Team * comm    () const { return _comm; }
      virtual void setComplete (xlpgas_LCompHandler_t cb,
				void *arg) { _cb_complete = cb; _arg = arg; }
      virtual void setContext (pami_context_t ctxt) {_pami_ctxt=ctxt;}
      void setDeviceInfo(void* di){_device_info = di;}
    public:
      virtual void   kick   () { } /* force progress */
      virtual bool   isdone () const { return true; } /* check completion */

      virtual void   reset () { printf("ERROR: virtual method called (1)\n");}
      virtual void   reset (int root, const void * sbuf, void * rbuf, unsigned nbytes) {
	printf("ERROR: virtual method called (2)\n");
      }
      virtual void   reset (const void * sbuf, void * rbuf, unsigned nbytes) {
	printf("ERROR: virtual method called (3) \n");
      }
      virtual void reset (const void *sbuf, void * rbuf,
			  xlpgas_ops_t op, xlpgas_dtypes_t dtype,
			  unsigned nelems) {
	printf("ERROR: virtual method called (5)\n");
      }
      virtual void reset (const void *sbuf, void * rbuf,
			  xlpgas_ops_t op, xlpgas_dtypes_t dtype,
			  unsigned nelems, user_func_t*) {
	printf("ERROR: virtual method called (6)\n");
      }
      virtual void reset (const void *sbuf, void * rbuf, size_t*) {
	printf("ERROR: virtual method called (7)\n");
      }
      virtual void reset (const void *sbuf, void * rbuf,
			  const size_t*, const size_t*, const size_t*, const size_t*) {
	printf("ERROR: virtual method called (8)\n");
      }
      //for reduce with root
      virtual void reset (int root, const void *sbuf, void * rbuf,
			  xlpgas_ops_t op, xlpgas_dtypes_t dtype,
			  unsigned nelems, user_func_t*) {
	printf("ERROR: virtual method called (9)\n");
      }

      virtual void setNI(T_NI *p2p_iface) { _p2p_iface = p2p_iface; }

      virtual void setGenericDevice(PAMI::Device::Generic::Device *dev) { _dev = dev;}

      // With the support for endpoints this is not the _my_rank but _my_index
      size_t ordinal(void) const {
        return _my_index;
      }

      pami_endpoint_t rank(void) const {
        return this->_p2p_iface->endpoint();
      }

    protected:
      int                       _ctxt;
      pami_context_t            _pami_ctxt;
      Team                     * _comm;
      CollectiveKind            _kind;
      int                       _tag;
      xlpgas_LCompHandler_t      _cb_complete;
      void                    * _arg;
      T_NI                    * _p2p_iface;
      PAMI::Device::Generic::Device * _dev;
      void                    * _device_info;
      pami_task_t               _my_rank;
      size_t                    _my_index;
      bool                      _is_leader;//caching the is leader information
      DECL_MUTEX(_mutex);
    };

  /* *********************************************************** */
  /*   Managing non-blocking collectives at runtime.             */
  /* The manager is a singleton.                                 */
  /* *********************************************************** */
  template <class T_NI>
  class CollectiveManager
    {
      typedef util::Array<Collective<T_NI> *> CollArray_t;


    public:
      /* ---------------- */
      /* external API     */
      /* ---------------- */
#ifdef XLPGAS_PAMI_CAU
      static void InitializeLapi (lapi_handle_t handle, xlpgas::CollectiveManager<T_NI>* cmgr);
#endif      
      Collective<T_NI> * find (CollectiveKind kind, int tag);

      template <class CollDefs>
      Collective<T_NI> * allocate (Team* comm, CollectiveKind kind, int id, void* device_info, T_NI* ni){
	assert (0 <= kind && kind < MAXKIND);
	//int nextID = _kindlist[kind]->len();
        int nextID = id;
	Collective<T_NI> * b;
	switch (kind)
	  {
	    /* Here I can make a templated allocator that is called
	       from each case statement with the proper type
	    */
	  case BarrierKind:
	    {
	      typedef typename CollDefs::barrier_type barrier_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(barrier_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(barrier_type));
	      new (b) barrier_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }

	  case BarrierPPKind:
	    {
	      typedef typename CollDefs::barrier_pp_type barrier_type;
	      b = (Collective<T_NI> *) __global.heap_mm->malloc (sizeof(barrier_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(barrier_type));
	      new (b) barrier_type (_ctxt, comm, kind, nextID, 0,ni);
	      break;
	    }

	  case LongAllreduceKind:
	    {
	      typedef typename CollDefs::allreduce_type allreduce_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(allreduce_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(allreduce_type));
	      new (b) allreduce_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }

	  case ShortAllreduceKind:
	    {
	      typedef typename CollDefs::short_allreduce_type allreduce_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(allreduce_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(allreduce_type));
	      new (b) allreduce_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }
#ifdef XLPGAS_PAMI_CAU
	  case SHMReduceKind:
	    {
	      typedef typename CollDefs::shm_reduce_type reduce_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(reduce_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(reduce_type));
	      new (b) reduce_type (_ctxt, comm, kind, nextID, 0, device_info, ni);
	      break;
	    }
	  case CAUReduceKind:
	    {
	      typedef typename CollDefs::cau_reduce_type reduce_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(reduce_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(reduce_type));
	      new (b) reduce_type (_ctxt, comm, kind, nextID, 0, device_info, ni);
	      break;
	    }

	  case ShmCauAllReduceKind:
	    {
	      typedef typename CollDefs::shm_cau_allreduce_type reduce_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(reduce_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(reduce_type));
	      new (b) reduce_type (_ctxt, comm, kind, nextID, 0, device_info, ni);
	      break;
	    }
#endif
	  case AllreducePPKind:
	    {
	      typedef typename CollDefs::allreduce_pp_type allreduce_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(allreduce_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(allreduce_type));
	      new (b) allreduce_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }

	  case BcastKind:
	    {
	      typedef typename CollDefs::broadcast_type bcast_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(bcast_type));
	      new (b) bcast_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }
#ifdef XLPGAS_PAMI_CAU
	  case SHMBcastKind:
	    {
	      typedef typename CollDefs::shm_broadcast_type bcast_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(bcast_type));
	      new (b) bcast_type (_ctxt, comm, kind, nextID, 0, device_info, ni);
	      break;
	    }

	  case CAUBcastKind:
	     {
	       typedef typename CollDefs::cau_broadcast_type bcast_type;
	       b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	       assert (b != NULL);
	       memset (b, 0, sizeof(bcast_type));
	       new (b) bcast_type (_ctxt, comm, kind, nextID, 0, device_info, ni);
	       break;
	     }

	  case SHMLargeBcastKind:
	    {
	      typedef typename CollDefs::shm_large_broadcast_type bcast_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(bcast_type));
	      new (b) bcast_type (_ctxt, comm, kind, nextID, 0,device_info,ni);
	      break;
	    }

	  case LeadersBcastKind:
	    {
	      typedef typename CollDefs::leaders_broadcast_type bcast_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(bcast_type));
	      new (b) bcast_type (_ctxt, comm, kind, nextID, 0,ni);
	      break;
	    }

	  case ShmHybridBcastKind:
	    {
	      typedef typename CollDefs::shm_hybrid_broadcast_type bcast_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(bcast_type));
	      new (b) bcast_type (_ctxt, comm, kind, nextID, 0, device_info, ni);
	      break;
	    }
          case ShmHybridPipelinedBcastKind:
	    {
	      typedef typename CollDefs::shm_hybrid_pipelined_broadcast_type bcast_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(bcast_type));
	      new (b) bcast_type (_ctxt, comm, kind, nextID, 0, device_info, ni);
	      break;
	    }
#endif
	  case BcastPPKind:
	    {
	      typedef typename CollDefs::broadcast_pp_type bcast_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(bcast_type));
	      new (b) bcast_type (_ctxt, comm, kind, nextID, 0,ni);
	      break;
	    }

	  case BcastTreeKind:
	    {
	      typedef typename CollDefs::broadcast_tree_type bcast_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(bcast_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(bcast_type));
	      new (b) bcast_type (_ctxt, comm, kind, nextID, 0,ni);
	      break;
	    }

	  case AllgatherKind:
	    {
	      typedef typename CollDefs::allgather_type allgather_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(allgather_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(allgather_type));
	      new (b) allgather_type (_ctxt, comm, kind, nextID, 0,ni);
	      break;
	    }
	  case AllgatherPPKind:
	    {
	      typedef typename CollDefs::allgather_pp_type allgather_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(allgather_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(allgather_type));
	      new (b) allgather_type (_ctxt, comm, kind, nextID, 0,ni);
	      break;
	    }

	  case AllgathervKind:
	    {
	      typedef typename CollDefs::allgatherv_type allgatherv_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(allgatherv_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(allgatherv_type));
	      new (b) allgatherv_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }
	  case AlltoallKind:
	    {
	      typedef typename CollDefs::alltoall_type alltoall_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(alltoall_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(alltoall_type));
	      new (b) alltoall_type (_ctxt, comm, kind, nextID, 0,ni);
	      break;
	    }
	  case AlltoallPPKind:
	    {
	      typedef typename CollDefs::alltoall_pp_type alltoall_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(alltoall_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(alltoall_type));
	      new (b) alltoall_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }
	  case AlltoallvKind:
	    {
	      typedef typename CollDefs::alltoallv_type alltoallv_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(alltoallv_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(alltoallv_type));
	      new (b) alltoallv_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }
	  case AlltoallvintKind:
	    {
	      typedef typename CollDefs::alltoallvint_type alltoallvint_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(alltoallvint_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(alltoallvint_type));
	      new (b) alltoallvint_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }

	  case GatherKind:
	    {
	      typedef typename CollDefs::gather_type gather_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(gather_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(gather_type));
	      new (b) gather_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }

	  case ScatterKind:
	    {
	      typedef typename CollDefs::scatter_type scatter_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(scatter_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(scatter_type));
	      new (b) scatter_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }

	  case PermuteKind:
	    {
	      typedef typename CollDefs::permute_type permute_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(permute_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(permute_type));
	      new (b) permute_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }

	  case PrefixKind:
	    {
	      typedef typename CollDefs::prefixsums_type prefixsums_type;
	      b = (Collective<T_NI> *)__global.heap_mm->malloc (sizeof(prefixsums_type));
	      assert (b != NULL);
	      memset (b, 0, sizeof(prefixsums_type));
	      new (b) prefixsums_type (_ctxt, comm, kind, nextID, 0, ni);
	      break;
	    }

	  default:
	    {
	      // xlpgas_fatalerror (-1, "Internal: invalid collective instance");
	      return NULL;
	    }
	  }
	(*_kindlist[kind])[nextID] = b;
        b->setGenericDevice(_genericDevice);
	return b;
      }

      template <class CollDefs>
        inline void          multisend_reg(CollectiveKind kind, T_NI *p2p_iface, int* dispatch_id = NULL, void* comm_handle = NULL)
        {
          switch (kind)
            {
	  case BarrierKind:
	    {
              typedef typename CollDefs::barrier_type barrier_type;
              p2p_iface->setSendPWQDispatch(barrier_type::cb_incoming, this);
	      break;
	    }

	  case BarrierPPKind:
	    {
	      break;
	    }

	  case LongAllreduceKind:
	    {
              typedef typename CollDefs::allreduce_type allreduce_type;
              p2p_iface->setSendPWQDispatch(allreduce_type::cb_incoming, this);
	      break;
	    }

	  case ShortAllreduceKind:
	    {
              typedef typename CollDefs::short_allreduce_type short_allreduce_type;
              p2p_iface->setSendPWQDispatch(short_allreduce_type::cb_incoming, this);
	      break;
	    }

	  case AllreducePPKind:
	    {
	      break;
	    }

	  case BcastKind:
	    {
              typedef typename CollDefs::broadcast_type broadcast_type;
              p2p_iface->setSendPWQDispatch(broadcast_type::cb_incoming, this);
	      break;
	    }

#ifdef XLPGAS_PAMI_CAU
	  case LeadersBcastKind:
	    {
              typedef typename CollDefs::leaders_broadcast_type broadcast_type;
              p2p_iface->setSendPWQDispatch(broadcast_type::cb_incoming, this);
	      break;
	    }
#endif
	  case BcastPPKind:
	    {
	      break;
	    }

	  case BcastTreeKind:
	    {
	      break;
	    }

	  case AllgatherKind:
	    {
              typedef typename CollDefs::allgather_type allgather_type;
              p2p_iface->setSendPWQDispatch(allgather_type::cb_incoming, this);
	      break;
	    }
	  case AllgatherPPKind:
	    {
	      break;
	    }

	  case AllgathervKind:
	    {
              typedef typename CollDefs::allgatherv_type allgatherv_type;
              p2p_iface->setSendPWQDispatch(allgatherv_type::cb_incoming, this);
	      break;
	    }
	  case AlltoallKind:
	    {
              typedef typename CollDefs::alltoall_type alltoall_type;
              p2p_iface->setSendPWQDispatch(alltoall_type::cb_incoming, this);
	      break;
	    }
	  case AlltoallPPKind:
	    {
	      break;
	    }
	  case AlltoallvKind:
	    {
              typedef typename CollDefs::alltoallv_type alltoallv_type;
              p2p_iface->setSendPWQDispatch(alltoallv_type::cb_incoming_v, this);
	      break;
	    }
	  case AlltoallvintKind:
	    {
              typedef typename CollDefs::alltoallvint_type alltoallvint_type;
              p2p_iface->setSendPWQDispatch(alltoallvint_type::cb_incoming_v, this);
	      break;
	    }

	  case GatherKind:
	    {
              typedef typename CollDefs::gather_type gather_type;
              p2p_iface->setSendPWQDispatch(gather_type::cb_incoming, this);
	      break;
	    }

	  case ScatterKind:
	    {
              typedef typename CollDefs::scatter_type scatter_type;
              p2p_iface->setSendPWQDispatch(scatter_type::cb_incoming, this);
	      break;
	    }

	  case PermuteKind:
	    {
	      break;
	    }

	  case PrefixKind:
	    {
              typedef typename CollDefs::prefixsums_type prefixsums_type;
              p2p_iface->setSendPWQDispatch(prefixsums_type::cb_incoming, this);
	      break;
	    }
#ifdef XLPGAS_PAMI_CAU
	    //shm/hybrid
            //registration for hybrid cau collectives happens in the cau algorithms class
            //It requires dispatch id and lapi handle;
	   case SHMReduceKind:
	    {
	      break;
	    }
	   case SHMBcastKind:
	    {
	      break;
	    }
	   case CAUReduceKind:
	    {
              typedef typename CollDefs::cau_reduce_type reduce_type;
              reduce_type::register_dispatch(dispatch_id,*((lapi_handle_t*)comm_handle));
	      break;
	    }
	   case CAUBcastKind:
	    {
              typedef typename CollDefs::cau_broadcast_type bcast_type;
              bcast_type::register_dispatch(dispatch_id,*((lapi_handle_t*)comm_handle));
	      break;
	    }

	   case SHMLargeBcastKind:
	    {
	      break;
	    }
	   case ShmCauAllReduceKind:
	    {
	      break;
	    }
	   case ShmHybridBcastKind:
	    {
	      break;
	    }
            case ShmHybridPipelinedBcastKind:
	    {
	      break;
	    }
#endif
	  default:
	    {
	      xlpgas_fatalerror (-1, "Internal: invalid collective registration");
	    }
	  }
        }
      void setGenericDevice(PAMI::Device::Generic::Device *g)
        {
          _genericDevice = g;
        }
    private:
      /* ------------ */
      /* data members */
      /* ------------ */

      CollArray_t                   *_kindlist[MAXKIND];
      int                            _ctxt;
      PAMI::Device::Generic::Device *_genericDevice;
    private:
      /* ------------ */
      /* constructors */
      /* ------------ */
    public:
      CollectiveManager (int ctxt);
      void * operator new (size_t, void * addr) { return addr; }
      PAMI::Device::Generic::Device* device() {return _genericDevice;}
    };
}

#include "algorithms/protocols/tspcoll/Collective.cc"
#include "algorithms/protocols/tspcoll/CollectiveManager.cc"
#endif
