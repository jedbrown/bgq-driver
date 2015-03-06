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
 * \file algorithms/protocols/barrier/MultiSyncComposite.h
 * \brief Simple composite based on multisync
 */
#ifndef __algorithms_protocols_barrier_MultiSyncComposite_h__
#define __algorithms_protocols_barrier_MultiSyncComposite_h__

#include "algorithms/executor/Barrier.h"
#include "algorithms/composite/Composite.h"
#include "util/trace.h"

#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     

#ifdef CCMI_TRACE_ALL
#define DO_TRACE_ENTEREXIT 1
#define DO_TRACE_DEBUG     1
#else
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0
#endif

namespace CCMI{namespace Adaptor{namespace Barrier{
  ///
  /// Barrier Factory All Sided for generate routine
  ///
  template <class      T_Composite,
            MetaDataFn get_metadata,
            class      T_Conn,
            int        LookupNI=0>
  class BarrierFactory2DeviceMsync: public CollectiveProtocolFactory
  {
  public:
    class collObj
    {
    public:
      collObj(Interfaces::NativeInterface  * nativeL,
              Interfaces::NativeInterface  * nativeG,
              T_Conn                       * cmgr,
              pami_geometry_t                geometry,
              pami_xfer_t                  * cmd,
              pami_event_function            fn,
              void                         * cookie,
              BarrierFactory2DeviceMsync   * factory):
        _factory(factory),
        _user_done_fn(cmd->cb_done),
        _user_cookie(cmd->cookie),
        _obj(nativeL, nativeG,cmgr,geometry,cmd,fn,cookie)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>",this);
          DO_DEBUG((templateName<T_Composite>()));
          TRACE_FN_EXIT();
        }
      BarrierFactory2DeviceMsync         * _factory;
      pami_event_function                  _user_done_fn;
      void                               * _user_cookie;
      T_Composite                          _obj;
    };

    BarrierFactory2DeviceMsync (pami_context_t                ctxt,
                                size_t                        ctxt_id,
                                pami_mapidtogeometry_fn       cb_geometry,
                                T_Conn                      * cmgr,
                                Interfaces::NativeInterface * nativeL,
                                Interfaces::NativeInterface * nativeG):
      CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry),
      _cmgr(cmgr),
      _nativeL(nativeL),
      _nativeG(nativeG)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>",this);
        TRACE_FN_EXIT();
      }

    virtual ~BarrierFactory2DeviceMsync ()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "%p",this);
        TRACE_FN_EXIT();
      }

    /// NOTE: This is required to make "C" programs link successfully with virtual destructors
    void operator delete(void * p)
      {
        CCMI_abort();
      }

    static void done_fn(pami_context_t  context,
                        void          * clientdata,
                        pami_result_t   res)
      {
        TRACE_FN_ENTER();
        collObj *cobj = (collObj *)clientdata;
        TRACE_FORMAT("<%p> cobj %p",cobj->_factory, cobj);
        cobj->_user_done_fn(context, cobj->_user_cookie, res);
        cobj->_factory->_alloc.returnObject(cobj);
        TRACE_FN_EXIT();
      }

    virtual Executor::Composite * generate(pami_geometry_t             geometry,
                                           void                      * cmd)
      {
        TRACE_FN_ENTER();


        // This should compile out if native interfaces are scoped
        // globally.
        if(LookupNI)
        {
          PAMI_GEOMETRY_CLASS *g = (PAMI_GEOMETRY_CLASS*)geometry;
          _nativeL = _ni_local_map[g->comm()];
          _nativeG = _ni_global_map[g->comm()];
        }

        collObj *cobj = (collObj*)  _alloc.allocateObject();
        TRACE_FORMAT("<%p> cobj %p",this, cobj);
        new(cobj) collObj(_nativeL,         // Native interface
                          _nativeG,         // Native interface
                          _cmgr,            // Connection Manager
                          geometry,         // Geometry Object
                          (pami_xfer_t*)cmd,// Parameters
                          done_fn,          // Intercept function
                          cobj,             // Intercept cookie
                          this);            // Factory
        TRACE_FN_EXIT();
        return(Executor::Composite *)(&cobj->_obj);
      }

    virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("mdata=%p",mdata);
        get_metadata(mdata);
        TRACE_FN_EXIT();
      }

    inline void setNI(pami_geometry_t              geometry,
                      Interfaces::NativeInterface *nativeL,
                      Interfaces::NativeInterface *nativeG)
      {
        PAMI_assert(LookupNI == 1); // no local master?
        PAMI_GEOMETRY_CLASS *g = (PAMI_GEOMETRY_CLASS*)geometry;
        _ni_local_map[g->comm()]  = nativeL;
        _ni_global_map[g->comm()] = nativeG;
        _nativeL = NULL;
        _nativeG = NULL;
      }
  private:
    T_Conn                                        *_cmgr;
    Interfaces::NativeInterface                   *_nativeL;
    Interfaces::NativeInterface                   *_nativeG;
    PAMI::MemoryAllocator<sizeof(collObj), 16>     _alloc;
    std::map<size_t,Interfaces::NativeInterface *> _ni_local_map;
    std::map<size_t,Interfaces::NativeInterface *> _ni_global_map;
  };




  template < bool                            T_inline         =false,
             class                           T_Native         =Interfaces::NativeInterface,
             PAMI::Geometry::topologyIndex_t T_Geometry_Index =PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX,
             PAMI::Geometry::ckeys_t         T_Gkey           =PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID,
             PAMI::Geometry::ckeys_t         T_Lkey           =PAMI::Geometry::CKEY_MSYNC_LOCAL_CLASSROUTEID
             >

  class MultiSyncComposite : public CCMI::Executor::Composite
  {
  public:
    MultiSyncComposite (Interfaces::NativeInterface          * native,
                        ConnectionManager::SimpleConnMgr     * cmgr,
                        pami_geometry_t                         g,
                        void                                 * cmd,
                        pami_event_function                     fn,
                        void                                 * cookie) :
      Composite(), _native(native) //, _geometry((PAMI_GEOMETRY_CLASS*)g)
      {
        TRACE_FN_ENTER();

        PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
        _deviceInfo          = geometry->getKey(native->contextid(), T_Gkey);
        TRACE_FORMAT( "_devinfo %p",_deviceInfo);

        _minfo.cb_done.function   = NULL;
        _minfo.cb_done.clientdata = NULL;
        _minfo.connection_id      = 0;
        _minfo.roles              = -1U;
        _minfo.participants       = geometry->getTopology(T_Geometry_Index);
        TRACE_FN_EXIT();
      }

    ///Barrier composite is created and cached
    virtual void start()
      {
        TRACE_FN_ENTER();
        _minfo.cb_done.function   = _cb_done;
        _minfo.cb_done.clientdata = _clientdata;
        if (T_inline)
        {
          T_Native *t_native = (T_Native *)_native;
          t_native->T_Native::multisync (&_minfo, _deviceInfo);
        }
        else
          _native->multisync(&_minfo, _deviceInfo);
        TRACE_FN_EXIT();
      }

  protected:
    Interfaces::NativeInterface        * _native;
    pami_multisync_t                     _minfo;
    void                               * _deviceInfo;
  };


  template < PAMI::Geometry::ckeys_t         T_Gkey           =PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID,
             PAMI::Geometry::ckeys_t         T_Lkey           =PAMI::Geometry::CKEY_MSYNC_LOCAL_CLASSROUTEID
             >
  class MultiSyncComposite2Device : public CCMI::Executor::Composite
  {

    static void local_done_fn(pami_context_t  context,
                              void           *cookie,
                              pami_result_t   result )
      {
        TRACE_FN_ENTER();
        MultiSyncComposite2Device *m = (MultiSyncComposite2Device*) cookie;
        m->_native_g->multisync(&m->_minfo_g, m->_deviceInfoG);
        TRACE_FN_EXIT();
      }

    static void global_done_fn(pami_context_t  context,
                               void           *cookie,
                               pami_result_t   result )
      {
        TRACE_FN_ENTER();
        MultiSyncComposite2Device *m = (MultiSyncComposite2Device*) cookie;
        m->_native_l->multisync(&m->_minfo_l1, m->_deviceInfoL);
        TRACE_FN_EXIT();
      }

  public:
    MultiSyncComposite2Device (Interfaces::NativeInterface      *mInterface,
                               ConnectionManager::SimpleConnMgr *cmgr,
                               pami_geometry_t                   g,
                               void                             *cmd,
                               pami_event_function               fn,
                               void                             *cookie) :
      Composite(),
      // This is a small hack to get around making a whole new set of factories
      // and classes for a 2 device NI.  We just treat the first parameter like a
      // 2 element pointer array.
      _native_l((Interfaces::NativeInterface*)((void **)mInterface)[0]),
      _native_g((Interfaces::NativeInterface*)((void **)mInterface)[1]),
      _geometry((PAMI_GEOMETRY_CLASS*)g),
      _deviceInfoG(NULL),
      _deviceInfoL(NULL)
      {
        TRACE_FN_ENTER();
        setup(_native_l,
              _native_g,
              cmgr,
              g,
              cmd,
              fn,
              cookie);
        TRACE_FN_EXIT();
      }


    MultiSyncComposite2Device (Interfaces::NativeInterface      *mInterfaceL,
                               Interfaces::NativeInterface      *mInterfaceG,
                               ConnectionManager::SimpleConnMgr *cmgr,
                               pami_geometry_t                   g,
                               void                             *cmd,
                               pami_event_function               fn,
                               void                             *cookie) :
      Composite(),
      _native_l(mInterfaceL),
      _native_g(mInterfaceG),
      _geometry((PAMI_GEOMETRY_CLASS*)g),
      _deviceInfoG(NULL),
      _deviceInfoL(NULL)
      {
        TRACE_FN_ENTER();
        setup(_native_l,
              _native_g,
              cmgr,
              g,
              cmd,
              fn,
              cookie);
        TRACE_FN_EXIT();
      }

    void setup(Interfaces::NativeInterface      *mInterfaceL,
               Interfaces::NativeInterface      *mInterfaceG,
               ConnectionManager::SimpleConnMgr *cmgr,
               pami_geometry_t                   g,
               void                             *cmd,
               pami_event_function               fn,
               void                             *cookie)
      {
        TRACE_FN_ENTER();
        PAMI::Topology  *t_master    = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);
        PAMI::Topology  *t_local     = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
	//        PAMI::Topology  *t_local_master = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_MASTER_TOPOLOGY_INDEX);

        DO_DEBUG(unsigned j = 0;)
        DO_DEBUG(for (j = 0; j < t_master->size(); ++j) TRACE_FORMAT("DEBUG t_master[%2.2u]=%#X, size %zu", j, t_master->index2Endpoint(j), t_master->size()));
        DO_DEBUG(for (j = 0; j < t_local->size(); ++j) TRACE_FORMAT("DEBUG t_local[%2.2u]=%zu, size %zu", j, (size_t)t_local->index2Endpoint(j), t_local->size()));
//        DO_DEBUG(for (unsigned j = 0; j < t_local_master->size(); ++j) TRACE_FORMAT("DEBUG t_local_master[%2.2u]=%zu, size %zu", j, (size_t)t_local_master->index2Endpoint(j), t_local_master->size()));

        _cb_done                     = fn;
        _clientdata                  = cookie;

        _g_reset_cb                   = false;
        _l0_reset_cb                  = false;
        _l1_reset_cb                  = false;

        TRACE_FORMAT( "master size %zu, local size %zu",t_master->size(),  t_local->size());
        _minfo_l0.connection_id      = 0;
        _minfo_l0.roles              = -1U;
        _minfo_l0.participants       = (pami_topology_t*)t_local;

        _minfo_g.connection_id       = 0;
        _minfo_g.roles               = -1U;
        _minfo_g.participants        = (pami_topology_t*)t_master;

        _minfo_l1.connection_id      = 0;
        _minfo_l1.roles              = -1U;
        _minfo_l1.participants       = (pami_topology_t*)t_local;

        // If the global "master" topology has only one rank, the local barrier will
        // suffice to implement the barrier
        _deviceInfoG = mInterfaceG? _geometry->getKey(mInterfaceG->contextid(),T_Gkey):NULL;
        _deviceInfoL = mInterfaceL? _geometry->getKey(mInterfaceL->contextid(),T_Lkey):NULL;
        if (t_master->size() == 1 && t_local->size() != 1)
        {
          _minfo_l0.cb_done.function   =  NULL; //fn;
          _minfo_l0.cb_done.clientdata =  NULL; //cookie;
          _active_native               =  _native_l;
          _activeDeviceInfo            = _deviceInfoL;
          _active_minfo                = &_minfo_l0;
          TRACE_FORMAT( "g %p, l0 %p, l1 %p",_minfo_g.cb_done.function ,_minfo_l0.cb_done.function ,_minfo_l1.cb_done.function );
        }

        // If we have more than one master, but we are the only local process
        // we are guaranteed to be a "local master", so we will just
        // issue the collective on the global device
        if (t_master->size() > 1 && t_local->size() == 1)
        {
          _minfo_g.cb_done.function    =  NULL; //fn;
          _minfo_g.cb_done.clientdata  =  NULL; //cookie;
          _active_native               =  _native_g;
          _activeDeviceInfo            = _deviceInfoG;
          _active_minfo                = &_minfo_g;
          TRACE_FORMAT( "g %p, l0 %p, l1 %p",_minfo_g.cb_done.function ,_minfo_l0.cb_done.function ,_minfo_l1.cb_done.function );
        }

        // We have a mix of both local nodes and master nodes
        // We need to determine if we are the master.
        // If this node is the master, then it participates in
        // the standard local, global, local flow.  If the node
        // is not the master, it participates in only the
        // local, local flow.
        if (t_master->size() > 1 && t_local->size() > 1)
        {
          bool participant = mInterfaceG && t_master->isEndpointMember(mInterfaceG->endpoint());
          TRACE_FORMAT( "participant %u, mInterfaceG %p t_master->isEndpointMember(%X) %d",participant, mInterfaceG,mInterfaceG? mInterfaceG->endpoint():-1,mInterfaceG? t_master->isEndpointMember(mInterfaceG->endpoint()):-1 );
          if (participant)
          {
            _minfo_l0.cb_done.function   = local_done_fn;
            _minfo_l0.cb_done.clientdata = this;
            _minfo_g.cb_done.function    = global_done_fn;
            _minfo_g.cb_done.clientdata  = this;
            TRACE_FORMAT( "g %p, l0 %p, l1 %p",_minfo_g.cb_done.function ,_minfo_l0.cb_done.function ,_minfo_l1.cb_done.function );
          }
          else
          {
            _minfo_l0.cb_done.function   = global_done_fn;
            _minfo_l0.cb_done.clientdata = this;
            TRACE_FORMAT( "g %p, l0 %p, l1 %p",_minfo_g.cb_done.function ,_minfo_l0.cb_done.function ,_minfo_l1.cb_done.function );
          }

          _minfo_l1.cb_done.function   = NULL; //fn;
          _minfo_l1.cb_done.clientdata = NULL; //cookie;
          _active_native               =  _native_l;
          _activeDeviceInfo            = _deviceInfoL;
          _active_minfo                = &_minfo_l0;
          TRACE_FORMAT( "g %p, l0 %p, l1 %p",_minfo_g.cb_done.function ,_minfo_l0.cb_done.function ,_minfo_l1.cb_done.function );
        }

        if (_minfo_g.cb_done.function == NULL)
          _g_reset_cb = true;

        if (_minfo_l0.cb_done.function == NULL)
          _l0_reset_cb = true;

        if (_minfo_l1.cb_done.function == NULL)
          _l1_reset_cb = true;
        TRACE_FORMAT( "g %p, l0 %p, l1 %p",_minfo_g.cb_done.function ,_minfo_l0.cb_done.function ,_minfo_l1.cb_done.function );
        TRACE_FN_EXIT();
      }

    virtual void start()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "g %u, l0 %u, l1 %u",_g_reset_cb, _l0_reset_cb, _l1_reset_cb);
        if (_g_reset_cb)
        {
          _minfo_g.cb_done.function   = _cb_done;
          _minfo_g.cb_done.clientdata = _clientdata;
        }
        if (_l0_reset_cb)
        {
          _minfo_l0.cb_done.function   = _cb_done;
          _minfo_l0.cb_done.clientdata = _clientdata;
        }
        if (_l1_reset_cb)
        {
          _minfo_l1.cb_done.function   = _cb_done;
          _minfo_l1.cb_done.clientdata = _clientdata;
        }

        _active_native->multisync(_active_minfo, _activeDeviceInfo);
        TRACE_FN_EXIT();
      }
  protected:
    Interfaces::NativeInterface        *_native_l;
    Interfaces::NativeInterface        *_native_g;
    Interfaces::NativeInterface        *_active_native;
    PAMI_GEOMETRY_CLASS                *_geometry;
    void                               *_deviceInfoG;
    void                               *_deviceInfoL;
    void                               *_activeDeviceInfo;
    pami_multisync_t                    _minfo_l0;
    pami_multisync_t                    _minfo_g;
    pami_multisync_t                    _minfo_l1;
    bool                                _g_reset_cb;
    bool                                _l0_reset_cb;
    bool                                _l1_reset_cb;
    pami_multisync_t                   *_active_minfo;
  };

};
};
};

#undef DO_TRACE_ENTEREXIT 
#undef DO_TRACE_DEBUG     

#endif
