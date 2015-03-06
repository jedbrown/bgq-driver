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
 * \file algorithms/interfaces/GeometryInterface.h
 * \brief ???
 */

#ifndef __algorithms_interfaces_GeometryInterface_h__
#define __algorithms_interfaces_GeometryInterface_h__

#include <pami.h>
#include "util/queue/MatchQueue.h"
#include "Mapping.h"
#include "components/memory/MemoryAllocator.h"
#include "algorithms/geometry/UnexpBarrierQueueElement.h"
#include "algorithms/protocols/CollectiveProtocolFactory.h"
#include "GeometryPlatform.h"

#define CCMI_EXECUTOR_TYPE void*
#define COMPOSITE_TYPE void*

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

namespace PAMI
{
  namespace Geometry
  {
    typedef CCMI::Adaptor::CollectiveProtocolFactory Factory;
    typedef enum
    {
      CKEY_COLLFACTORY     = 0,
      CKEY_UEBARRIERCOMPOSITE0,    // ?
      CKEY_UEBARRIERCOMPOSITE1,    // ?
      CKEY_BARRIERCOMPOSITE0,      // ?
      CKEY_BARRIERCOMPOSITE1,      // CCMI barrier cache
      CKEY_BARRIERCOMPOSITE2,      // CCMI barrier cache
      CKEY_BARRIERCOMPOSITE3,      // CCMI barrier cache
      CKEY_BARRIERCOMPOSITE4,      // CCMI barrier cache
      CKEY_BARRIERCOMPOSITE5,      // CCMI barrier cache
      CKEY_BARRIERCOMPOSITE6,      // CCMI barrier cache
      CKEY_BARRIERCOMPOSITE7,      // CCMI barrier cache
      CKEY_BARRIERCOMPOSITE8,      // CCMI barrier cache
      CKEY_BARRIERCOMPOSITE9,      // CCMI barrier cache
      CKEY_OPTIMIZEDBARRIERCOMPOSITE, // The optimized algorithm for this geometry
      CKEY_BCASTCOMPOSITE0,
      CKEY_BCASTCOMPOSITE1,
      CKEY_BCASTCOMPOSITE2,
      CKEY_BCASTCOMPOSITE3,
      CKEY_MCAST_CLASSROUTEID,     // Multicast class route id
      CKEY_MCOMB_CLASSROUTEID,     // Multicombine class route id
      CKEY_MSYNC_CLASSROUTEID,     // Multisync class route id
      CKEY_MSYNC_CLASSROUTEID1,    // Multisync class route id1
      CKEY_MSYNC_LOCAL_CLASSROUTEID, //Multisync id for local hw accel barriers
      CKEY_MEMORY_OPTIMIZE,        // Memory optimized collectives requested
      CKEY_NONCONTIG_DISABLE,      // Non-contiguous data collectives disabled
      PAMI_CKEY_PLATEXTENSIONS
      NUM_CKEYS,
    } ckeys_t;                     // context keystore keys
    

    typedef enum
    {
      DEFAULT_TOPOLOGY_INDEX      =  0,  // master/global sub-topology
      MASTER_TOPOLOGY_INDEX       =  1,  // master/global sub-topology
      LOCAL_TOPOLOGY_INDEX        =  2,  // local sub-topology
      LOCAL_MASTER_TOPOLOGY_INDEX =  3,  // local master sub-topology
      COORDINATE_TOPOLOGY_INDEX   =  4,  // coordinate topology (if valid)
      LIST_TOPOLOGY_INDEX         =  5,  // (optional) rank list topology
      MAX_NUM_TOPOLOGIES          =  6   // Number of topologies stored
    } topologyIndex_t;             // indices into _topos[] for special topologies

    struct DispatchInfo
    {
      pami_dispatch_callback_function  fn;
      void                            *cookie;
      pami_collective_hint_t           options;
    };

    template <class T_Geometry>
      class Geometry
    {
    public:
      inline Geometry (Geometry     *parent,
                       Mapping      *mapping,
                       unsigned      comm,
                       int           numranks,
                       pami_task_t  *ranks)
        {
          (void)parent;(void)mapping;(void)comm;
          (void)numranks;(void)ranks;
          TRACE_ERR((stderr, "<%p>%s\n", this, __PRETTY_FUNCTION__));
        }

      inline Geometry (Geometry         *parent,
                       Mapping          *mapping,
                       unsigned          comm,
                       int               numeps,
                       pami_endpoint_t  *eps,
                       bool)
        {
          (void)parent;(void)mapping;(void)comm;
          (void)numeps;(void)eps;
          TRACE_ERR((stderr, "<%p>%s\n", this, __PRETTY_FUNCTION__));
        }

      inline Geometry (Geometry  *parent,
                       Mapping   *mapping,
                       unsigned   comm,
                       int        numranges,
                       pami_geometry_range_t rangelist[])
      {
        (void)parent;(void)mapping;(void)comm;
        (void)numranges;(void)rangelist;
        TRACE_ERR((stderr, "<%p>%s\n", this, __PRETTY_FUNCTION__));
      }
      inline Geometry (Geometry  *parent,
                       Mapping   *mapping,
                       unsigned   comm,
                       PAMI::Topology *topo)
      {
        (void)parent;(void)mapping;(void)comm;
        (void)topo;
        TRACE_ERR((stderr, "<%p>%s\n", this, __PRETTY_FUNCTION__));
      }

      // These methods were originally from the CCMI Geometry class
      inline unsigned                   comm();
      inline pami_topology_t           *getTopology(topologyIndex_t topo_num);
      inline pami_client_t		getClient();
/** \todo  need to replace by attributes */
      inline void                       incrementAllreduceIteration(size_t context_id);
      inline unsigned                   getAllreduceIteration(size_t context_id);
      inline void                       freeAllocations ();
      inline void                       setGlobalContext(bool context);
      inline void                       setNumColors(unsigned numcolors);
      inline MatchQueue<>              &asyncCollectivePostQ(size_t context_id);
      inline MatchQueue<>              &asyncCollectiveUnexpQ(size_t context_id);
      inline COMPOSITE_TYPE             getAllreduceComposite(size_t context_id,unsigned i);
      inline COMPOSITE_TYPE             getAllreduceComposite(size_t context_id);
      inline void                       setAllreduceComposite(size_t context_id,COMPOSITE_TYPE c);
      inline void                       setAllreduceComposite(size_t context_id,
                                                              COMPOSITE_TYPE c,
                                                              unsigned i);
      inline void                       processUnexpBarrier(MatchQueue<> * ueb_queue,
                                                            MemoryAllocator <sizeof(PAMI::Geometry::UnexpBarrierQueueElement), 16> *ueb_allocator);
      // These methods were originally from the PGASRT Communicator class
      inline size_t                      size       (void);
      inline pami_task_t                 rank       (void); /// \todo remove this?  should endpoint geometry track my rank?
      inline size_t                      ordinal    (pami_endpoint_t ep);
      inline pami_endpoint_t             endpoint   (size_t ordinal);
      inline void                       setKey(size_t context_id, ckeys_t key, void*value);

      inline void                       setDispatch(size_t context_id, size_t key, DispatchInfo* value);
      inline DispatchInfo              *getDispatch(size_t context_id, size_t key);
      inline void                      *getKey(size_t context_id, ckeys_t key);
      inline pami_result_t              default_barrier(pami_event_function,void*,size_t,pami_context_t);
      inline void                       resetDefaultBarrier(size_t ctxt_id);
      inline pami_result_t              setDefaultBarrier(Factory *f, size_t ctxt_id);
      inline pami_result_t              ue_barrier(pami_event_function,void*,size_t,pami_context_t);
      inline void                       resetUEBarrier(size_t ctxt_id);
      inline pami_result_t              setUEBarrier(Factory *f, size_t ctxt_id);

      // API support
      inline pami_result_t algorithms_num(pami_xfer_type_t  colltype,
                                          size_t           *lists_lengths);


      inline pami_result_t algorithms_info (pami_xfer_type_t   colltype,
                                           pami_algorithm_t  *algs0,
                                           pami_metadata_t   *mdata0,
                                           size_t               num0,
                                           pami_algorithm_t  *algs1,
                                           pami_metadata_t   *mdata1,
                                            size_t            num1);
      // List management
      inline pami_result_t addCollective(pami_xfer_type_t                            xfer_type,
                                         Factory          *factory,
                                         pami_context_t    context,
                                        size_t                                     context_id);

      inline pami_result_t rmCollective(pami_xfer_type_t                            xfer_type,
                                        Factory          *factory,
                                        pami_context_t    context,
                                        size_t                                     context_id);

      inline pami_result_t addCollectiveCheck(pami_xfer_type_t                            xfer_type,
                                              Factory          *factory,
                                              pami_context_t    context,
                                             size_t                                     context_id);

      inline pami_result_t rmCollectiveCheck(pami_xfer_type_t                            xfer_type,
                                             Factory          *factory,
                                             pami_context_t    context,
                                             size_t                                     context_id);

      inline void setCleanupCallback(pami_event_function fcn, void *data);
      inline void resetCleanupCallback(pami_event_function fcn, void *data);

    }; // class Geometry

    template <class T_Geometry>
    inline pami_topology_t* Geometry<T_Geometry>::getTopology(topologyIndex_t topo_num)
    {
      return static_cast<T_Geometry*>(this)->getTopology_impl(topo_num);
    }

    template <class T_Geometry>
    inline unsigned Geometry<T_Geometry>::comm()
    {
      return static_cast<T_Geometry*>(this)->comm_impl();
    }

    template <class T_Geometry>
    inline pami_client_t Geometry<T_Geometry>::getClient()
    {
      return static_cast<T_Geometry*>(this)->getClient_impl();
    }

    template <class T_Geometry>
    inline void Geometry<T_Geometry>::incrementAllreduceIteration(size_t context_id)
    {
      static_cast<T_Geometry*>(this)->incrementAllreduceIteration_impl(context_id);
    }

    template <class T_Geometry>
    inline unsigned Geometry<T_Geometry>::getAllreduceIteration(size_t context_id)
    {
      return static_cast<T_Geometry*>(this)->getAllreduceIteration_impl(context_id);
    }

    template <class T_Geometry>
    inline void Geometry<T_Geometry>::freeAllocations ()
    {
      return static_cast<T_Geometry*>(this)->freeAllocations_impl();
    }
    template <class T_Geometry>
    inline void Geometry<T_Geometry>::setGlobalContext(bool context)
    {
      return static_cast<T_Geometry*>(this)->setGlobalContext_impl(context);
    }

    template <class T_Geometry>
    inline void Geometry<T_Geometry>::setNumColors(unsigned numcolors)
    {
      return static_cast<T_Geometry*>(this)->setNumColors_impl(numcolors);
    }

    template <class T_Geometry>
    inline MatchQueue<> &Geometry<T_Geometry>::asyncCollectivePostQ(size_t context_id)
    {
      return static_cast<T_Geometry*>(this)->asyncCollectivePostQ_impl(context_id);
    }

    template <class T_Geometry>
    inline MatchQueue<> &Geometry<T_Geometry>::asyncCollectiveUnexpQ(size_t context_id)
    {
      return static_cast<T_Geometry*>(this)->asyncCollectiveUnexpQ_impl(context_id);
    }

    template <class T_Geometry>
    inline COMPOSITE_TYPE Geometry<T_Geometry>::getAllreduceComposite(size_t context_id,
                                                                      unsigned i)
    {
      return static_cast<T_Geometry*>(this)->getAllreduceComposite_impl(context_id,i);
    }

    template <class T_Geometry>
    inline void Geometry<T_Geometry>::setAllreduceComposite(size_t context_id,
                                                            COMPOSITE_TYPE c)
    {
      return static_cast<T_Geometry*>(this)->setAllreduceComposite_impl(context_id, c);
    }

    template <class T_Geometry>
    inline void Geometry<T_Geometry>::setAllreduceComposite(size_t context_id,
                                                            COMPOSITE_TYPE c,
                                                            unsigned i)
    {
      return static_cast<T_Geometry*>(this)->setAllreduceComposite_impl(context_id,c, i);
    }

    template <class T_Geometry>
    inline COMPOSITE_TYPE Geometry<T_Geometry>::getAllreduceComposite(size_t context_id)
    {
      return static_cast<T_Geometry*>(this)->getAllreduceComposite_impl(context_id);
    }

    template <class T_Geometry>
    inline void Geometry<T_Geometry>::processUnexpBarrier(MatchQueue<> * ueb_queue,
                                                          MemoryAllocator <sizeof(PAMI::Geometry::UnexpBarrierQueueElement), 16> *ueb_allocator)
    {
      return static_cast<T_Geometry*>(this)->processUnexpBarrier_impl(ueb_queue,ueb_allocator);
    }

    // These methods were originally from the PGASRT Communicator class
    template <class T_Geometry>
    inline size_t  Geometry<T_Geometry>::size       (void)
    {
      return static_cast<T_Geometry*>(this)->size_impl();
    }
    template <class T_Geometry>
    inline pami_task_t  Geometry<T_Geometry>::rank       (void)
    {
      return static_cast<T_Geometry*>(this)->rank_impl();
    }
    template <class T_Geometry>
    inline size_t Geometry<T_Geometry>::ordinal (pami_endpoint_t ep)
    {
      return static_cast<T_Geometry*>(this)->ordinal_impl(ep);
    }
    template <class T_Geometry>
    inline pami_endpoint_t Geometry<T_Geometry>::endpoint (size_t ordinal)
    {
      return static_cast<T_Geometry*>(this)->endpoint_impl(ordinal);
    }
    template <class T_Geometry>
    inline void                        Geometry<T_Geometry>::setDispatch (size_t context_id, size_t key, DispatchInfo *value)
    {
      static_cast<T_Geometry*>(this)->setDispatch_impl(context_id, key, value);
    }
    template <class T_Geometry>
    inline void                        Geometry<T_Geometry>::setKey (size_t context_id, ckeys_t key, void *value)
    {
      static_cast<T_Geometry*>(this)->setKey_impl(context_id, key, value);
    }
    template <class T_Geometry>
    inline DispatchInfo*               Geometry<T_Geometry>::getDispatch (size_t context_id, size_t key)
    {
      return static_cast<T_Geometry*>(this)->getDispatch_impl(context_id, key);
    }
    template <class T_Geometry>
    inline void*                       Geometry<T_Geometry>::getKey (size_t context_id, ckeys_t key)
    {
      return static_cast<T_Geometry*>(this)->getKey_impl(context_id, key);
    }
    template <class T_Geometry>
    inline pami_result_t               Geometry<T_Geometry>::default_barrier(pami_event_function      cb_done,
                                                                             void               *cookie,
                                                                             size_t              ctxt_id,
                                                                             pami_context_t      context)
    {
      return static_cast<T_Geometry*>(this)->default_barrier_impl(cb_done, cookie, ctxt_id, context);
    }

    template <class T_Geometry>
    inline void                        Geometry<T_Geometry>::resetDefaultBarrier(size_t ctxt_id)
    {
      return static_cast<T_Geometry*>(this)->resetDefaultBarrier_impl(ctxt_id);
    }

    template <class T_Geometry>
    inline pami_result_t               Geometry<T_Geometry>::setDefaultBarrier(Factory *f,
                                                                               size_t   ctxt_id)
    {
      return static_cast<T_Geometry*>(this)->setDefaultBarrier_impl(f, ctxt_id);
    }

    template <class T_Geometry>
    inline pami_result_t               Geometry<T_Geometry>::ue_barrier(pami_event_function      cb_done,
                                                                        void               *cookie,
                                                                        size_t              ctxt_id,
                                                                        pami_context_t      context)
    {
      return static_cast<T_Geometry*>(this)->ue_barrier_impl(cb_done, cookie, ctxt_id, context);
    }

    template <class T_Geometry>
    inline void                        Geometry<T_Geometry>::resetUEBarrier(size_t ctxt_id)
    {
      return static_cast<T_Geometry*>(this)->resetUEBarrier_impl(ctxt_id);
    }

    template <class T_Geometry>
    inline pami_result_t               Geometry<T_Geometry>::setUEBarrier(Factory *f,
                                                                          size_t   ctxt_id)
    {
      return static_cast<T_Geometry*>(this)->setUEBarrier_impl(f, ctxt_id);
    }

    template <class T_Geometry>
    inline pami_result_t Geometry<T_Geometry>::algorithms_num(pami_xfer_type_t  colltype,
                                                              size_t           *lists_lengths)
    {
      return static_cast<T_Geometry*>(this)->algorithms_num_impl(colltype,lists_lengths);
    }

    template <class T_Geometry>
    inline pami_result_t  Geometry<T_Geometry>::algorithms_info (pami_xfer_type_t   colltype,
                                                                pami_algorithm_t  *algs0,
                                                                pami_metadata_t   *mdata0,
                                                                size_t               num0,
                                                                pami_algorithm_t  *algs1,
                                                                pami_metadata_t   *mdata1,
                                                                 size_t             num1)
    {
      return static_cast<T_Geometry*>(this)->algorithms_info_impl(colltype,
                                                                  algs0,
                                                                  mdata0,
                                                                  num0,
                                                                  algs1,
                                                                  mdata1,
                                                                  num1);
    }

    template <class T_Geometry>
    inline pami_result_t Geometry<T_Geometry>::addCollective(pami_xfer_type_t                            xfer_type,
                                                             Factory  *factory,
                                                             pami_context_t                             context,
                                                            size_t                                     context_id)
    {
      return static_cast<T_Geometry*>(this)->addCollective_impl(xfer_type,factory,context,context_id);
    }

    template <class T_Geometry>
    inline pami_result_t Geometry<T_Geometry>::rmCollective(pami_xfer_type_t                            xfer_type,
                                                            Factory  *factory,
                                                            pami_context_t                             context,
                                                            size_t                                     context_id)
    {
      return static_cast<T_Geometry*>(this)->rmCollective_impl(xfer_type,factory,context,context_id);
    }


    template <class T_Geometry>
    inline pami_result_t Geometry<T_Geometry>::addCollectiveCheck(pami_xfer_type_t                            xfer_type,
                                                                 Factory  *factory,
                                                                  pami_context_t                             context,
                                                                 size_t                                     context_id)
    {
      return static_cast<T_Geometry*>(this)->addCollectiveCheck_impl(xfer_type,factory,context,context_id);
    }

    template <class T_Geometry>
    inline pami_result_t Geometry<T_Geometry>::rmCollectiveCheck(pami_xfer_type_t                            xfer_type,
                                                                 Factory  *factory,
                                                                 pami_context_t                             context,
                                                                 size_t                                     context_id)
    {
      return static_cast<T_Geometry*>(this)->rmCollectiveCheck_impl(xfer_type,factory,context,context_id);
    }

    template <class T_Geometry>
    inline void Geometry<T_Geometry>::setCleanupCallback(pami_event_function   fcn,
							 void                * data)
    {
      return static_cast<T_Geometry*>(this)->setCleanupCallback_impl (fcn, data);
    }

    template <class T_Geometry>
    inline void Geometry<T_Geometry>::resetCleanupCallback(pami_event_function   fcn,
							 void                * data)
    {
      return static_cast<T_Geometry*>(this)->resetCleanupCallback_impl (fcn, data);
    }

  }; // namespace Geometry
}; // namespace PAMI

#undef TRACE_ERR

#endif
