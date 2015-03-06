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
 * \file algorithms/protocols/barrier/BarrierT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_barrier_BarrierT_h__
#define __algorithms_protocols_barrier_BarrierT_h__

#include "algorithms/connmgr/SimpleConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactory.h"
#include "algorithms/protocols/AllSidedCollectiveProtocolFactoryT.h"
#include "algorithms/executor/Barrier.h"
#include "algorithms/protocols/AllSidedCollectiveProtocolFactoryNCOT.h"

#include "util/trace.h"

#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif


extern void registerunexpbarrier(pami_context_t context,
                                 unsigned       comm,
                                 pami_quad_t   &info,
                                 unsigned       peer,
                                 unsigned       algorithm);


namespace CCMI
{
namespace Adaptor
{
namespace Barrier
{
typedef bool (*AnalyzeFn) (PAMI_GEOMETRY_CLASS *g);

// Barrier Factory for generate routine
// generate
//
template < class T_Composite, MetaDataFn get_metadata, class T_Conn, bool T_Unexp, PAMI::Geometry::ckeys_t T_Key >
class BarrierFactoryT : public CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, PAMI_XFER_BARRIER >
{
public:
    BarrierFactoryT(pami_context_t               ctxt,
                    size_t                       ctxt_id,
                    pami_mapidtogeometry_fn      cb_geometry,
                    T_Conn                      *cmgr,
                    Interfaces::NativeInterface *native,
                    pami_dispatch_multicast_function cb_head = NULL):
        CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, PAMI_XFER_BARRIER >(ctxt,ctxt_id,cb_geometry,cmgr, native, cb_head),
        _cached_id((unsigned) -1),
        _cached_object(NULL)
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "%p",this);
        TRACE_FN_EXIT();
    }
    ~BarrierFactoryT()
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "%p",this);
        TRACE_FN_EXIT();
    }
    virtual Executor::Composite * generate(pami_geometry_t              geometry,
                                           void                      * cmd)

    {
        // Use the cached barrier or generate a new one if the cached barrier doesn't exist
        TRACE_FN_ENTER();
        PAMI_GEOMETRY_CLASS  *g = ( PAMI_GEOMETRY_CLASS *)geometry;
        TRACE_FORMAT( "Generating Barrier with key=%d, ctxt_id=%zu", T_Key, this->_context_id);
        Executor::Composite *composite = (Executor::Composite *) g->getKey(this->_context_id,
                                         T_Key);

        TRACE_FORMAT( "<%p>generate composite %p geometry %p, T_Key %u",this,composite, geometry, T_Key);
        if (!composite)
        {
            composite = CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, PAMI_XFER_BARRIER>::generate(geometry, cmd);
            TRACE_FORMAT( "<%p> composite %p",this,composite);
            g->setKey(this->_context_id,
                      T_Key,
                      (void*)composite);
            TRACE_FORMAT( "<%p>generate composite %p geometry %p, T_Key %u",this,composite, geometry, T_Key);
        }

        pami_xfer_t *xfer = (pami_xfer_t *)cmd;
        composite->setDoneCallback(xfer->cb_done, xfer->cookie);
        pami_metadata_t mdata;
        this->metadata(&mdata);
        TRACE_FORMAT( "<%p>generate composite %p geometry %p, T_Key %u, name %s",this,composite, geometry, T_Key, mdata.name);
        TRACE_FN_EXIT();
        return composite;
    }
    static void cleanup_done_fn(pami_context_t  context,
                          void           *clientdata,
                          pami_result_t   res)
    {
      TRACE_FN_ENTER();
      T_Composite *obj = (T_Composite *)clientdata;
      typedef typename CollectiveProtocolFactoryT<T_Composite, get_metadata, T_Conn, PAMI_XFER_BARRIER >::collObj collObj;
      collObj * cobj = (collObj*)obj->getCollObj();
      cobj->~collObj();
      cobj->_factory->_alloc.returnObject(cobj);
      TRACE_FN_EXIT();
    }
    ///
    ///  \brief Cache the barrier executor for the most recent geometry
    ///  \brief They pami context
    ///  \param[in] id communicator id of the geometry
    ///
    void * getGeometryObject (pami_context_t ctxt, unsigned id)
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> cached_object %p, ctxt %p,id %u, cached_id %u",this,_cached_object,ctxt,id,_cached_id);
        if (likely(_cached_object && id  == _cached_id))
        {
            TRACE_FORMAT( "<%p> cached_object %p",this,_cached_object);
            TRACE_FN_EXIT();
            return _cached_object;
        }

        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) this->getGeometry(ctxt, id);
        _cached_object =  (geometry) ? (geometry->getKey(this->_context_id, T_Key)) : NULL;
	_cached_id     =  id;

        TRACE_FORMAT( "<%p> cached_object %p",this,_cached_object);
        TRACE_FN_EXIT();
        return _cached_object;
    }

    virtual void clearCache () {
      CollectiveProtocolFactory::clearCache();
      _cached_id = (unsigned) -1;
      _cached_object = NULL;
    }


    static void    cb_head   (pami_context_t         ctxt,
                              const pami_quad_t    * info,
                              unsigned              count,
                              unsigned              conn_id,
                              size_t                peer,
                              size_t                sndlen,
                              void                * arg,
                              size_t              * rcvlen,
                              pami_pipeworkqueue_t **recvpwq,
                              PAMI_Callback_t  *     cb_done)
    {
        TRACE_FN_ENTER();
        ExtCollHeaderData  *cdata = (ExtCollHeaderData *) info;
        BarrierFactoryT *factory = (BarrierFactoryT *) arg;
        TRACE_FORMAT( "<%p>cdata %p, _comm %u, _count %u, _dt %u, _iteration %u, _op %u, _phase %u,_root %u",factory,cdata, cdata->_comm, cdata->_count, cdata->_dt, cdata->_iteration, cdata->_op, cdata->_phase,cdata->_root);
        *rcvlen    = 0;
        *recvpwq   = 0;
        cb_done->function   = NULL;
        cb_done->clientdata = NULL;

        PAMI_assert (factory != NULL);
        //PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) factory->getGeometry (ctxt, cdata->_comm);
        void *object = factory->getGeometryObject(ctxt, cdata->_comm);
        TRACE_FORMAT("<%p>composite %p, T_Unexp %u, T_Key %u",factory, object, T_Unexp, T_Key);
        if (T_Unexp) {
            if (object == NULL)
            {
                //Geometry doesn't exist
                registerunexpbarrier(ctxt,
                                     cdata->_comm,
                                     (pami_quad_t&)*info,
                                     peer,
                                     T_Key);
                TRACE_FN_EXIT();
                return;
            }
        }

        T_Composite *composite = (T_Composite*) object;
        TRACE_FORMAT("<%p>comm %d composite %p)",
                    factory, cdata->_comm, composite);

        //Override poly morphism
        PAMI_assert(ctxt != NULL);
        PAMI_assert(composite != NULL);
        composite->_myexecutor.setContext(ctxt);
        composite->_myexecutor.notifyRecv (peer, *info, NULL, 0);
        TRACE_FN_EXIT();
    }

protected:
    unsigned                             _cached_id;
    void                               * _cached_object;
};


///
/// Barrier Factory All Sided for generate routine
///
template < class T_Composite, MetaDataFn get_metadata, class T_Conn, PAMI::Geometry::ckeys_t T_Key >
class BarrierFactoryAllSidedT : public AllSidedCollectiveProtocolFactoryNCOT<T_Composite, get_metadata, T_Conn, PAMI_XFER_BARRIER>
{
public:
    BarrierFactoryAllSidedT(pami_context_t               ctxt,
                            size_t                       ctxt_id,
                            pami_mapidtogeometry_fn      cb_geometry,
                            T_Conn                      *cmgr,
                            Interfaces::NativeInterface *native):
      AllSidedCollectiveProtocolFactoryNCOT<T_Composite, get_metadata, T_Conn, PAMI_XFER_BARRIER>(ctxt,ctxt_id,cb_geometry,cmgr, native)
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "%p",this);
        TRACE_FN_EXIT();
    }
    BarrierFactoryAllSidedT(pami_context_t               ctxt,
                            size_t                       ctxt_id,
                            pami_mapidtogeometry_fn      cb_geometry,
                            T_Conn                       *cmgr,
                            Interfaces::NativeInterface **native):
      AllSidedCollectiveProtocolFactoryNCOT<T_Composite, get_metadata, T_Conn, PAMI_XFER_BARRIER>(ctxt,ctxt_id,cb_geometry,cmgr, native)
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "%p",this);
        TRACE_FN_EXIT();
    }

    virtual Executor::Composite * generate(pami_geometry_t              geometry,
                                           void                       * cmd)
    {
        // Use the cached barrier or generate a new one if the cached barrier doesn't exist
        TRACE_FN_ENTER();
        TRACE_FORMAT( "%p",this);
        PAMI_GEOMETRY_CLASS  *g = ( PAMI_GEOMETRY_CLASS *)geometry;
        /// \todo does NOT support multicontext
        Executor::Composite *composite = (Executor::Composite *) g->getKey(this->_context_id,
                                         T_Key);

        if (!composite)
        {
            composite = AllSidedCollectiveProtocolFactoryNCOT<T_Composite, get_metadata, T_Conn, PAMI_XFER_BARRIER>::generate(geometry, cmd);
            g->setKey(this->_context_id,
                      T_Key,
                      (void*)composite);
        }

        pami_xfer_t *xfer = (pami_xfer_t *)cmd;
        composite->setDoneCallback(xfer->cb_done, xfer->cookie);
        TRACE_FN_EXIT();
        return composite;
    }
};



///
/// \brief barrier template
///
template < class T_Schedule, AnalyzeFn afn, PAMI::Geometry::topologyIndex_t T_Geometry_Index, PAMI::Geometry::ckeys_t T_Key >
class BarrierT : public CCMI::Executor::Composite
{
public:
    ///
    /// \brief The executor for barrier protocol
    ///
    CCMI::Executor::BarrierExec         _myexecutor;
    ///
    /// \brief The schedule for barrier protocol
    ///
    T_Schedule                          _myschedule;

    ///
    /// \brief Constructor for non-blocking barrier protocols.
    ///
    /// \param[in] mInterface  The multicast Interface
    /// \param[in] geometry    Geometry object
    ///
    BarrierT  (pami_context_t                         ctxt,
               size_t                                 ctxt_id,
               Interfaces::NativeInterface          * mInterface,
               ConnectionManager::SimpleConnMgr     * cmgr,
               pami_geometry_t                         geometry,
               void                                 * cmd,
               pami_event_function                     fn,
               void                                 * cookie):
        _myexecutor(((PAMI_GEOMETRY_CLASS *)geometry)->comm(),
                    0,                                        // connection id?
                    mInterface),
        _myschedule (mInterface->endpoint(),(PAMI::Topology *)((PAMI_GEOMETRY_CLASS *)geometry)->getTopology(T_Geometry_Index))
    {
        TRACE_FN_ENTER();
        DO_DEBUG((templateName<BarrierT>()));
        TRACE_FORMAT( "%p, endpoint %u, topo %p",this,mInterface->endpoint(),(PAMI::Topology *)((PAMI_GEOMETRY_CLASS *)geometry)->getTopology(T_Geometry_Index));
        _myexecutor.setCommSchedule (&_myschedule);
        this->_collObj = cookie;//SSS: cookie is the collObj that contains this composite. I need to set it here so I can free it later.
        TRACE_FN_EXIT();
    }
    ~BarrierT()
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "%p",this);
        TRACE_FN_EXIT();
    }
    CCMI::Executor::BarrierExec *getExecutor() {
        return &_myexecutor;
    }

    static bool analyze (PAMI_GEOMETRY_CLASS *geometry)
    {
        return((AnalyzeFn) afn)(geometry);
    }

    void setContext (pami_context_t context) {
      _myexecutor.setContext(context);
    }

    virtual void start()
    {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "%p %p %p",this, _cb_done, _clientdata);
        _myexecutor.setDoneCallback (_cb_done, _clientdata);
        _myexecutor.start();
        TRACE_FN_EXIT();
    }

    virtual void   notifyRecv  (unsigned              src,
                                const pami_quad_t   & metadata,
                                PAMI::PipeWorkQueue ** pwq,
                                pami_callback_t      * cb_done,
                                void                 * cookie)
    {
        _myexecutor.notifyRecv (src, metadata, NULL, NULL);
    }

    inline void * getCollObj()
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p> %p",this,_collObj);
      TRACE_FN_EXIT();
      return _collObj;
    }

    void   *_collObj;

}; //-BarrierT

//////////////////////////////////////////////////////////////////////////////
};
};
};  //namespace CCMI::Adaptor::Barrier

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
