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
 * \file algorithms/protocols/tspcoll/ShmCauAllReduce.h
 * \brief ???
 */
#ifndef __algorithms_protocols_tspcoll_ShmCauAllReduce_h__
#define __algorithms_protocols_tspcoll_ShmCauAllReduce_h__

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
  template <class T_NI, class T_Device>
  class ShmCauAllReduce : public Collective<T_NI>
  {
  public:

    void * operator new (size_t, void * addr) { return addr; }

    ShmCauAllReduce (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, void* device_info, T_NI* ni) :
      Collective<T_NI> (ctxt, comm, kind, tag, NULL, NULL, ni) {
      this->_device_info = device_info;
      //here cache the is leader information and the required topologies;
      typedef xlpgas::cau_device_info<T_NI> device_info_type;
      PAMI_GEOMETRY_CLASS* geometry = ((device_info_type*)(this->_device_info))->geometry();
      team        = (PAMI::Topology*)(geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
      local_team  = (PAMI::Topology*)(geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX));
      leader_team = (PAMI::Topology*)(geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX));
      //the text topology we don't cache for now; used only to extract the is leader info
      PAMI::Topology* my_master_team = (PAMI::Topology*)(geometry->getTopology(PAMI::Geometry::LOCAL_MASTER_TOPOLOGY_INDEX));
      this->_is_leader = my_master_team->isEndpointMember(this->rank());
      //allocate space for temp buffers
    }
    virtual void reset (const void         * sbuf, 
			void               * dbuf, 
			pami_data_function   op,
			TypeCode           * sdt,
			unsigned             nelems,
                        TypeCode           * rdt,
			user_func_t        * uf
			);

    virtual void kick();

    virtual bool isdone           () const;
    virtual void setComplete (xlpgas_LCompHandler_t cb,
			      void *arg);
    virtual void setContext (pami_context_t ctxt);

    void set_internal_coll(xlpgas::Collective<T_NI>* shm_red, 
			   xlpgas::Collective<T_NI>* shm_b,
			   xlpgas::Collective<T_NI>* cau_red, 
			   xlpgas::Collective<T_NI>* cau_b){
      shm_reduce = shm_red;
      shm_bcast = shm_b;
      cau_reduce = cau_red;
      cau_bcast = cau_b;
    }
  public:
    PAMI::Topology *team, *local_team, *leader_team;
    xlpgas::Collective<T_NI> *shm_reduce, *shm_bcast, *cau_reduce, *cau_bcast;
    int64_t s[8];
    int64_t tmp[8];
    int64_t tmp_cau[8];
  }; /* ShmCauAllReduce */
} /* Xlpgas */

#include "algorithms/protocols/tspcoll/ShmCauAllReduce.cc"

#endif /* __xlpgas_ShmCauAllReduce_h__ */
