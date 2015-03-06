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
 * \file algorithms/protocols/barrier/HybridBarrierT.h
 * \brief ???
 */

#ifndef __algorithms_protocols_barrier_HybridBarrierT_h__
#define __algorithms_protocols_barrier_HybridBarrierT_h__

#include "algorithms/protocols/CollectiveProtocolFactory.h"

namespace CCMI{namespace Adaptor{namespace Barrier
{
  // ///////////////////////////////////////////////////////////////////////////
  // Hybrid Barrier Composite
  // ///////////////////////////////////////////////////////////////////////////
  template <class T_GlobalFactory,
            class T_GlobalComposite,
            class T_NativeInterface>
  class HybridBarrierCompositeT : public CCMI::Executor::Composite
  {
    typedef void freeFcn(void*, void*);

    static void msync_done(pami_context_t  ctxt,
                           void           *clientdata,
                           pami_result_t   result)
      {
        HybridBarrierCompositeT *c = (HybridBarrierCompositeT*)clientdata;
        c->startGlobalChain(ctxt, result);
      }
    static void global_done(pami_context_t  ctxt,
                            void           *clientdata,
                            pami_result_t   result)
      {
        HybridBarrierCompositeT *c = (HybridBarrierCompositeT*)clientdata;
        c->startLocal(ctxt, result);
      }
    static void msync_done_final(pami_context_t  ctxt,
                                 void           *clientdata,
                                 pami_result_t   result)
      {
        HybridBarrierCompositeT *c = (HybridBarrierCompositeT*)clientdata;
        c->executeCallback(ctxt, result);
      }
  public:
    HybridBarrierCompositeT(pami_xfer_t       cmd,
                            pami_geometry_t   geometry,
                            T_NativeInterface *local_ni,
                            void              *deviceInfo,
                            T_GlobalFactory   *factory,
                            freeFcn           *free_fcn,
                            void              *freeFcnFactory):
      Composite(),
      _cmd(cmd),
      _geometry(geometry),
      _deviceInfo(deviceInfo),
      _t_local(((PAMI_GEOMETRY_CLASS*)_geometry)->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX)),
      _factory(factory),
      _local_ni(local_ni),
      _global_composite(NULL),
      _user_done_function(cmd.cb_done),
      _user_cookie(cmd.cookie),
      _free_fcn(free_fcn),
      _freeFcnFactory(freeFcnFactory)
      {
        if(_factory)
        {
          _global_cmd           = cmd;
          _global_cmd.cb_done   = global_done;
          _global_cmd.cookie    = this;
          _global_composite     = (T_GlobalComposite*)_factory->generate(_geometry, &_global_cmd);
        }
      }
    inline void startGlobalChain(pami_context_t  ctxt,
                                 pami_result_t   res)
      {
        // To get this far, we've synchronized locally
        // If the global composite is null, we're done,
        // just call the user callback
        if(_global_composite)
        {
          _global_composite->setContext(ctxt);
          _global_composite->start();
        }
        else
          startLocal(ctxt, res);
      }
    inline void startGlobal()
      {
        _global_composite->setContext(getContext());
        _global_composite->start();
      }

    inline void startLocal(pami_context_t ctxt, pami_result_t res)
      {
        // If we got this far, global sync has happened
        // we must sync locally again (if > 1 task/_local_ni is set)
        _minfo_local.cb_done.function   = msync_done_final;
        _minfo_local.cb_done.clientdata = this;
        _minfo_local.roles              = 0;
        _minfo_local.participants       = _t_local;
        if(_local_ni)
          _local_ni->multisync(&_minfo_local, _deviceInfo);
        else
          executeCallback(ctxt, res);
      }
    inline void executeCallback(pami_context_t ctxt, pami_result_t res)
      {
        if(_user_done_function)
          _user_done_function(ctxt, _user_cookie, res);
        _free_fcn(_freeFcnFactory, this);
      }
    virtual void start()
      {
        // Optimized path, if only one node
        if(((PAMI::Topology*)_t_local)->size() ==
           ((PAMI_GEOMETRY_CLASS*)_geometry)->size())
        {
          _minfo_local.cb_done.function   = msync_done_final;
          _minfo_local.cb_done.clientdata = this;
          _minfo_local.roles              = 0;
          _minfo_local.participants       = _t_local;
          if(_local_ni)
            _local_ni->multisync(&_minfo_local, _deviceInfo);
          else
            executeCallback(getContext(),PAMI_SUCCESS);
          return;
        }
        // Generalized Path
        _minfo_local.cb_done.function   = msync_done;
        _minfo_local.cb_done.clientdata = this;
        _minfo_local.roles              = 0;
        _minfo_local.participants       = _t_local;
        if(_local_ni)
          _local_ni->multisync(&_minfo_local, _deviceInfo);
        else if(_global_composite)
          startGlobal();
        else
          executeCallback(getContext(),PAMI_SUCCESS);
      }
  private:
    pami_xfer_t          _cmd;
    pami_geometry_t      _geometry;
    void                *_deviceInfo;
    pami_topology_t     *_t_local;
    T_GlobalFactory     *_factory;
    T_NativeInterface   *_local_ni;
    T_GlobalComposite   *_global_composite;
    pami_event_function  _user_done_function;
    void                *_user_cookie;
    freeFcn             *_free_fcn;
    void                *_freeFcnFactory;
    pami_multisync_t     _minfo_local;
    pami_xfer_t          _global_cmd;
  };

  // ///////////////////////////////////////////////////////////////////////////
  // Hybrid Barrier Factory Class
  // Inputs:  global:  barrier protocol, local:  device
  // ///////////////////////////////////////////////////////////////////////////
  template < class                   T_Composite,
             MetaDataFn              get_metadata,
             class                   T_GlobalFactory,
             class                   T_NativeInterface,
             PAMI::Geometry::ckeys_t T_Local_DevInfoKey = PAMI::Geometry::CKEY_MSYNC_LOCAL_CLASSROUTEID>
  class HybridBarrierFactoryT : public CollectiveProtocolFactory
  {
  public:
    static void freeFunction(void *factory, void *composite)
      {
        HybridBarrierFactoryT *f = (HybridBarrierFactoryT*)factory;
        f->freeComposite(composite);
      }

    inline HybridBarrierFactoryT(pami_context_t          ctxt,
                                 size_t                  ctxt_id,
                                 pami_mapidtogeometry_fn cb_geometry):
      CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry)
      {
      }

    inline void setInfo(pami_geometry_t    geometry,
                        T_NativeInterface *local_ni,
                        T_GlobalFactory   *global_factory)
      {
        PAMI_GEOMETRY_CLASS *g = (PAMI_GEOMETRY_CLASS*)geometry;
        _ni_local_map[g->comm()]       = local_ni;
        _global_factory_map[g->comm()] = global_factory;
      }

    inline T_NativeInterface* getNI(pami_geometry_t geometry)
      {
        PAMI_GEOMETRY_CLASS *g = (PAMI_GEOMETRY_CLASS*)geometry;
        return _ni_local_map[g->comm()];
      }

    inline T_GlobalFactory* getFactory(pami_geometry_t geometry)
      {
        PAMI_GEOMETRY_CLASS *g = (PAMI_GEOMETRY_CLASS*)geometry;
        return _global_factory_map[g->comm()];
      }

    virtual Executor::Composite * generate(pami_geometry_t  geometry,
                                           void            *cmd)

      {
        void        *device_info = ((PAMI_GEOMETRY_CLASS*)geometry)->getKey(_context_id,
                                                                            T_Local_DevInfoKey);
        T_Composite *composite   = (T_Composite*)_composite_allocator.allocateObject();
        pami_xfer_t *c           = (pami_xfer_t *) cmd;
        return new(composite) T_Composite(*c,
                                          geometry,
                                          getNI(geometry),
                                          device_info,
                                          getFactory(geometry),
                                          freeFunction,
                                          this);
      }
    virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
    {
      get_metadata(mdata);
    }

    void freeComposite(void* composite)
      {
        _composite_allocator.returnObject(composite);
      }
  private:
    PAMI::MemoryAllocator<sizeof(T_Composite),16>  _composite_allocator;
    T_GlobalFactory                               *_global;
    T_NativeInterface                             *_local_ni;
    std::map<size_t,T_NativeInterface *>           _ni_local_map;
    std::map<size_t,T_GlobalFactory *>             _global_factory_map;
  };
}}}

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
