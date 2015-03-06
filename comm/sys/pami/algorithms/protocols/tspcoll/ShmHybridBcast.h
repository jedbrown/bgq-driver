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
 * \file algorithms/protocols/tspcoll/ShmHybridBcast.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_ShmHybridBcast_h__
#define __algorithms_protocols_tspcoll_ShmHybridBcast_h__

#include "algorithms/protocols/tspcoll/Team.h"
#include "algorithms/protocols/tspcoll/CollExchange.h"

#include "algorithms/protocols/tspcoll/cau_collectives.h"

#undef TRACE
#ifdef DEBUG_COLL
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

namespace xlpgas
{

  template <class T_NI>
  class if_else_coll_continuation{
  public:
    void*  shm_bcast;
    void*  p2p_bcast;
    size_t left_to_send;
    void*  sbuf;
    void*  dbuf;
    size_t buf_size;
    size_t nbytes;
    TypeCode* ttype;
    size_t tcount;

    int    local_root;
    int    leader_root;
    //call back handler and its argument
    pami_context_t         pami_ctxt;
    xlpgas_LCompHandler_t  cb;
    void                  *arg;
    bool   shmem_finished;
    void* o_sbuf;//original send buf
    void* o_dbuf;
  };


  template <class T_NI, class T_Device>
  class ShmHybridBcast : public Collective<T_NI>
  {
    typedef if_else_coll_continuation<T_NI> ARGS;

  public:
    void * operator new (size_t, void * addr) { return addr; }

    ShmHybridBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni);
    virtual bool isdone           () const;
    virtual void reset (int root,
			const void         * sbuf, 
			void               * dbuf,
			TypeCode           * type,
			size_t               typecount);
    virtual void kick(); 

    void set_internal_coll(xlpgas::Broadcast<T_NI>* ld_coll, 
			   xlpgas::Collective<T_NI>* shm_coll){
      p2p_bcast  = ld_coll;
      shm_bcast = shm_coll;
    }

  protected:
    PAMI::Topology *team, *local_team, *leader_team;
    ARGS                      args; //arguments passed for continuation
    xlpgas::Broadcast<T_NI>  *p2p_bcast;
    xlpgas::Collective<T_NI> *shm_bcast;
    size_t                   _buf_size;
    int                      _root;
    pami_endpoint_t _root_endpoint;
  }; /* ShmHybridBcast */



  template <class T_NI>
  class args_pipeline{
  public:
    void*  onebuf_bcast;// the bcast that will push one buffer
    void*  pipelined_bcast;// the bcast that will push pipelined
    int    root;
    void*  sbuf;        // adjusted sbuf
    void*  dbuf;
    size_t buf_size;
    size_t nbytes;
    size_t left_to_send;
    TypeCode* ttype;
    size_t tcount;
    pami_context_t         pami_ctxt;
    xlpgas_LCompHandler_t  cb;
    void* arg;
    void* o_sbuf;//original send buf
    void* o_dbuf;
  };




  template <class T_NI, class T_Device>
  class ShmHybridPipelinedBcast : public Collective<T_NI>
  {
    typedef args_pipeline<T_NI> ARGS;
  public:
    void * operator new (size_t, void * addr) { return addr; }

    ShmHybridPipelinedBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni);
    virtual bool isdone           () const;
    virtual void reset (int root,
			const void         * sbuf, 
			void               * dbuf,
			TypeCode           * type,
			size_t               typecount);
    virtual void kick(); 
    void set_done(){
      _flag = true;
    }

    void set_internal_coll(ShmHybridBcast<T_NI,T_Device>  * coll){
      _onebuf_bcast = coll;
    }
 
  protected:
    
    ARGS args;
    bool _flag;
    ShmHybridBcast<T_NI,T_Device>  *_onebuf_bcast;
    int                      _root;
    size_t                   _buf_size;
  }; /* ShmHybridPipelinedBcast */

} /* Xlpgas */

#include "algorithms/protocols/tspcoll/ShmHybridBcast.cc"

#endif /* __xlpgas_ShmHybridBcast_h__ */
