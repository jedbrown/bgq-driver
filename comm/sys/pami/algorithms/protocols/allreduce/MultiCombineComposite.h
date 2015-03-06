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
 * \file algorithms/protocols/allreduce/MultiCombineComposite.h
 * \brief Simple composite based on multicombine
 */
#ifndef __algorithms_protocols_allreduce_MultiCombineComposite_h__
#define __algorithms_protocols_allreduce_MultiCombineComposite_h__

#include "algorithms/composite/Composite.h"
#include "algorithms/protocols/CollectiveProtocolFactory.h"
#include "algorithms/protocols/allreduce/ReduceFunctions.h"

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"


// Use a local done function for testing
//#define LOCAL_TEST
//#undef  DO_DEBUG
//#define DO_DEBUG(x) x

//#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

//#define CCMI_TRACE_ALL
#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
// Use a local done function for testing
  #undef LOCAL_TEST
  #define LOCAL_TEST
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif



namespace CCMI
{
  namespace Adaptor
  {
    namespace Allreduce
    {
      template <bool T_inline=false, class T_Native=Interfaces::NativeInterface>
      class MultiCombineComposite : public CCMI::Executor::Composite
      {
        protected:
          //Interfaces::NativeInterface        * _native;
          //PAMI_GEOMETRY_CLASS                * _geometry;
          PAMI::PipeWorkQueue                  _srcPwq;
          PAMI::PipeWorkQueue                  _dstPwq;
          //pami_multicombine_t                  _minfo;
          //void                               * _deviceInfo;

        public:
          MultiCombineComposite (pami_context_t               ctxt,
                                 size_t                       ctxt_id,
                                 Interfaces::NativeInterface          * native,
                                 ConnectionManager::SimpleConnMgr     * cmgr,
                                 pami_geometry_t                        g,
                                 pami_xfer_t                          * cmd,
                                 pami_event_function                    fn,
                                 void                                 * cookie) :
            Composite()//, _native(mInterface), _geometry((PAMI_GEOMETRY_CLASS*)g)
          {
            TRACE_FN_ENTER();
            PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS*)g;
            void *deviceInfo                  = geometry->getKey(0,PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID);

            TypeCode * stype_obj = (TypeCode *)cmd->cmd.xfer_allreduce.stype;
            TypeCode * rtype_obj = (TypeCode *)cmd->cmd.xfer_allreduce.rtype;
            char     * sndbuf    = cmd->cmd.xfer_allreduce.sndbuf;
            char     * rcvbuf    = cmd->cmd.xfer_allreduce.rcvbuf;
            if(sndbuf == PAMI_IN_PLACE)
            {
              sndbuf    = rcvbuf;
              stype_obj = rtype_obj;
            }

            uintptr_t op, dt;
            PAMI::Type::TypeFunc::GetEnums((pami_type_t) stype_obj,
                                           cmd->cmd.xfer_allreduce.op,
                                           dt,op);

            TRACE_FORMAT( "type %#zX/%#zX, count %zu/%zu, op %#X, dt %#X",
                           (size_t)cmd->cmd.xfer_allreduce.stype, (size_t)cmd->cmd.xfer_allreduce.rtype,
                          cmd->cmd.xfer_allreduce.stypecount, cmd->cmd.xfer_allreduce.rtypecount, (pami_op)op, (pami_dt)dt);

            /// \todo Support non-contiguous
            PAMI_assert(stype_obj->IsContiguous() &&  stype_obj->IsPrimitive());
            PAMI_assert(rtype_obj->IsContiguous() &&  rtype_obj->IsPrimitive());

            unsigned        ssizeOfType = stype_obj->GetDataSize();
            size_t sbytes = cmd->cmd.xfer_allreduce.stypecount * ssizeOfType;
            unsigned        rsizeOfType = rtype_obj->GetDataSize();
            size_t rbytes = cmd->cmd.xfer_allreduce.rtypecount * rsizeOfType;
            TRACE_FORMAT( "srcPwq.configure %zu",sbytes);
            _srcPwq.configure(sndbuf, sbytes, sbytes, stype_obj, rtype_obj);

            TRACE_FORMAT( "dstPwq.configure %zu",rbytes);
            _dstPwq.configure(rcvbuf, rbytes, 0, stype_obj, rtype_obj);// SSS: Should the types be in this order???

            DO_DEBUG(PAMI::Topology all);
            DO_DEBUG(all = *(PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));

            DO_DEBUG(unsigned j = 0;)
            DO_DEBUG(for (j = 0; j < all.size(); ++j) TRACE_FORMAT("all[%u]=%zu, size %zu", j, (size_t)all.index2Endpoint(j), all.size()));

            pami_multicombine_t minfo;
            minfo.cb_done.function     = fn;
            minfo.cb_done.clientdata   = cookie;
            minfo.connection_id        = 0;
            minfo.roles                = -1U;
            minfo.results_participants = geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
            minfo.data_participants    = geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
            minfo.data                 = (pami_pipeworkqueue_t *) & _srcPwq;
            minfo.results              = (pami_pipeworkqueue_t *) & _dstPwq;
            minfo.optor                = (pami_op)op;
            minfo.dtype                = (pami_dt)dt;
            minfo.count                = cmd->cmd.xfer_allreduce.stypecount;

            if (T_inline) {
              T_Native *t_native = (T_Native *) native;
              t_native->T_Native::multicombine(&minfo, deviceInfo);
            }
            else {
              native->multicombine(&minfo, deviceInfo);
            }

            TRACE_FORMAT( "count %zu", minfo.count);
            TRACE_FN_EXIT();
          }

          virtual void start()
          {
            TRACE_FN_ENTER();
            TRACE_FN_EXIT();
          }

	  void setContext (pami_context_t context) {}
      };

      /// \brief This is a factory for a 2 device MultiCombine
      /// The two device MultiCombine takes two native interfaces, the first is
      /// the local interface, and the second is the global interface
      template <class T_Composite, MetaDataFn get_metadata, class T_Connmgr, int LookupNI=0>
      class MultiCombineComposite2DeviceFactoryT: public CollectiveProtocolFactory
      {
          class collObj
          {
            public:
              collObj(Interfaces::NativeInterface          *native0,
                      Interfaces::NativeInterface          *native1,
                      T_Connmgr                            *cmgr,
                      pami_geometry_t                       geometry,
                      pami_endpoint_t                       me_ep,
                      pami_xfer_t                          *cmd,
                      pami_event_function                   fn,
                      void                                 *cookie,
                      MultiCombineComposite2DeviceFactoryT *factory):
                  _factory(factory),
                  _user_done_fn(cmd->cb_done),
                  _user_cookie(cmd->cookie),
                  _obj(native0, native1, cmgr, geometry, me_ep, cmd, fn, cookie)
              {
              }
              void done_fn( pami_context_t   context,
                            pami_result_t    result )
              {
                _user_done_fn(context, _user_cookie, result);
              }

              MultiCombineComposite2DeviceFactoryT *_factory;
              pami_event_function                   _user_done_fn;
              void                                 *_user_cookie;
              T_Composite                           _obj;
              unsigned                              _connection_id;
          };
        public:
          MultiCombineComposite2DeviceFactoryT (pami_context_t               ctxt,
                                                size_t                       ctxt_id,
                                                pami_mapidtogeometry_fn      cb_geometry,
                                                T_Connmgr                   *cmgr,
                                                Interfaces::NativeInterface *native_l,
                                                Interfaces::NativeInterface *native_g,
                                                pami_endpoint_t              me_ep):
            CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry),
              _cmgr(cmgr),
              _native_l(native_l),
              _native_g(native_g),
              _me_ep(me_ep)
          {
          }

          virtual ~MultiCombineComposite2DeviceFactoryT()
          {
          }
          virtual void metadata(pami_metadata_t *mdata, pami_geometry_t geometry = PAMI_GEOMETRY_NULL)
          {
            get_metadata(mdata);
          }
          static void done_fn(pami_context_t  context,
                              void           *clientdata,
                              pami_result_t   res)
          {
            collObj *cobj = (collObj *)clientdata;
            cobj->done_fn(context, res);
            cobj->_factory->_alloc.returnObject(cobj);
          }
          virtual Executor::Composite * generate(pami_geometry_t  geometry,
                                                 void            *cmd)
          {
            TRACE_FN_ENTER();

            // This should compile out if native interfaces are scoped
            // globally.
            if(LookupNI)
            {
              PAMI_GEOMETRY_CLASS *g = (PAMI_GEOMETRY_CLASS*)geometry;
              _native_l = _ni_local_map[g->comm()];
              _native_g = _ni_global_map[g->comm()];
            }

            collObj *cobj = (collObj*) _alloc.allocateObject();
            TRACE_FORMAT( "<%p>", cobj);
            new(cobj) collObj(_native_l,          // Native interface local
                              _native_g,          // Native Interface global
                              _cmgr,              // Connection Manager
                              geometry,           // Geometry Object
                              _me_ep,             // My endpoint  
                              (pami_xfer_t*)cmd,  // Parameters
                              done_fn,            // Intercept function
                              cobj,               // Intercept cookie
                              this);
            TRACE_FN_EXIT();
            return (Executor::Composite *)&cobj->_obj;

          }
        inline void setNI(pami_geometry_t geometry,
                           Interfaces::NativeInterface *nativeL,
                           Interfaces::NativeInterface *nativeG)
          {
            PAMI_assert(LookupNI == 1); // no local master?
            PAMI_GEOMETRY_CLASS *g = (PAMI_GEOMETRY_CLASS*)geometry;
            _ni_local_map[g->comm()]  = nativeL;
            _ni_global_map[g->comm()] = nativeG;
            _native_l = NULL;
            _native_g = NULL;
          }

        protected:
          T_Connmgr                                       *_cmgr;
          Interfaces::NativeInterface                     *_native_l;
          Interfaces::NativeInterface                     *_native_g;
          pami_endpoint_t                                  _me_ep;
          PAMI::MemoryAllocator < sizeof(collObj), 16 >    _alloc;
          std::map<size_t,Interfaces::NativeInterface *>   _ni_local_map;
          std::map<size_t,Interfaces::NativeInterface *>   _ni_global_map;
      };

      /// \brief All sided allreduce over active message multicombines
      /// A local device will chain into a global multicombine
      template <int ReduceOnly>
      class MultiCombineComposite2Device : public CCMI::Executor::Composite
      {
        public:
          static void composite_done(pami_context_t  context,
                                     void           *cookie,
                                     pami_result_t   result)
          {
            TRACE_FN_ENTER();
            MultiCombineComposite2Device *m = (MultiCombineComposite2Device*) cookie;
            m->_count--;
            TRACE_FORMAT( "count=%zd", m->_count);

            if (m->_count == 0)
            {
              m->_fn(context, m->_cookie, result);
              TRACE_FORMAT( "Delivered Callback Function To User=%zd", m->_count);
              if(m->_temp_results)
                __global.heap_mm->free(m->_temp_results);
              if(m->_throwaway_results)
                __global.heap_mm->free(m->_throwaway_results);
            }
            TRACE_FORMAT( "Phases Left: count=%zd", m->_count);
            TRACE_FN_EXIT();
          }


        template <class T_ReduceType>
        inline void setupReduceCommon(Interfaces::NativeInterface      *native_l,
                                      Interfaces::NativeInterface      *native_g,
                                      ConnectionManager::SimpleConnMgr *cmgr,
                                      pami_geometry_t                   g,
                                      pami_xfer_t                      *cmd,
                                      pami_event_function               fn,
                                      void                             *cookie,
                                      pami_endpoint_t                   root)
          {

            T_ReduceType    * xfer       = (T_ReduceType*)&cmd->cmd;
            TRACE_FN_ENTER();
            pami_result_t    rc          = PAMI_SUCCESS;
            PAMI::Topology  *t_master    = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);
            PAMI::Topology  *t_local     = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
            PAMI::Topology  *t_my_master = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_MASTER_TOPOLOGY_INDEX);
            bool             amMaster    = t_my_master->isEndpointMember(_me_ep);
            _deviceInfo                  = _geometry->getKey(0,PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID);
            // todo:  shared mem may need its own devinfo
            unsigned        typesize;
            coremath        func;
            uintptr_t op, dt;
            TypeCode * stype = (TypeCode *)xfer->stype;
            TypeCode * rtype = (TypeCode *)xfer->rtype;
            char     * sndbuf    = xfer->sndbuf;
            char     * rcvbuf    = xfer->rcvbuf;
            if(sndbuf == PAMI_IN_PLACE)
            {
              sndbuf    = rcvbuf;
              stype = rtype;
            }

            PAMI::Type::TypeFunc::GetEnums((pami_type_t) stype,
                                           xfer->op,
                                           dt,op);

            TRACE_FORMAT( "type %#zX/%#zX, count %zu/%zu, op %#X, dt %#X",
                           (size_t)xfer->stype, (size_t)xfer->rtype,
                          xfer->stypecount, xfer->rtypecount, (pami_op)op, (pami_dt)dt);


            getReduceFunction((pami_dt)dt,
                              (pami_op)op,
                              typesize,
                              func);
            size_t           sizeOfType  = stype->GetDataSize();
            size_t           sbytes      = xfer->stypecount * sizeOfType;
            size_t           scountDt    = xfer->stypecount;
            bool             amRoot      = false;

            if(root != 0xFFFFFFFF)
            {
              _root_ep    = root;
              amRoot      = (_me_ep == root);
              new(&_t_root) PAMI::Topology(&_root_ep, 1, PAMI::tag_eplist());
              PAMI_assert(rc == PAMI_SUCCESS);
            }

            // Create a "flat pwq" for the send buffer
            _pwq_src.configure(sndbuf,                             // buffer
                               sbytes,                             // buffer bytes
                               sbytes,                             // amount initially in buffer
                               stype,
                               rtype);
            if(amRoot || root == 0xFFFFFFFF)  // I am the real root of a Reduce, or I am in an allreduce
              ;
            else                              // I am a non-root, and I must throw away the results
            {
              rc = __global.heap_mm->memalign((void **)&_throwaway_results, 16, sbytes);
              if(sbytes)PAMI_assert(rc == PAMI_SUCCESS && _throwaway_results != NULL);
              rcvbuf = _throwaway_results;
            }
            _pwq_dest.configure(rcvbuf,                          // buffer
                                sbytes,                          // buffer bytes
                                0,                               // amount initially in buffer
                                stype,
                                rtype);

            _user_done.clientdata = cmd->cookie;
            _user_done.function   = cmd->cb_done;

            // The "Only Local" case
            // This means the geometry only contains local tasks
            // A single multicombine will suffice to handle the allreduce
            if (t_local->size() == _geometry->size() && amMaster)
              {
                _pwq_inter0.configure(
                                      rcvbuf,  // buffer
                                      sbytes,                          // buffer bytes
                                      0,                               // amount initially in buffer
                                      stype,
                                      rtype);
                _mcombine_l.cb_done.clientdata   = this;
                _mcombine_l.cb_done.function     = composite_done;
                _mcombine_l.connection_id        = _geometry->comm();
                _mcombine_l.roles                = -1U;
                _mcombine_l.data                 = (pami_pipeworkqueue_t*) & _pwq_src;
                _mcombine_l.data_participants    = (pami_topology_t*)t_local;
                _mcombine_l.results              = (pami_pipeworkqueue_t*) & _pwq_inter0;
                _mcombine_l.results_participants = (pami_topology_t*)t_my_master;
                _mcombine_l.optor                = (pami_op)op;
                _mcombine_l.dtype                = (pami_dt)dt;
                _mcombine_l.count                = scountDt;

                // Also, prepare the local multicast, with the root as the local master
                // We are guaranteed to not have an early arrival with this multicast if we post
                // it first.  This protocol expects the local multicast to be 2 sided
                _mcast_l.cb_done.function        = composite_done;
                _mcast_l.cb_done.clientdata      = this;
                _mcast_l.connection_id           = _geometry->comm();
                _mcast_l.roles                   = -1U;
                _mcast_l.bytes                   = xfer->stypecount*stype->GetDataSize();
                _mcast_l.src                     = (pami_pipeworkqueue_t*) & _pwq_inter0;
                _mcast_l.src_participants        = (pami_topology_t*)t_my_master;
                _mcast_l.dst                     = (pami_pipeworkqueue_t*) & _pwq_dest;
                _mcast_l.dst_participants        = (pami_topology_t*)t_local;
                _mcast_l.msginfo                 = 0;
                _mcast_l.msgcount                = 0;
                _startFcn                        = &MultiCombineComposite2Device::start2;
                _count                           = 2;

                TRACE_FORMAT( "<%p>Local Only MASTER Setting up start2: local native() %p", this, _native_l);
                return;
              }

            // The "Only Global Master" case
            // My task only belongs to a global master, with no other local task
            // A single multicombine will suffice in this case as well
            // To detect this case, the local size will be only me
            if (t_local->size() == 1)
              {
                _mcombine_g.cb_done.clientdata   = this;
                _mcombine_g.cb_done.function     = composite_done;
                _mcombine_g.connection_id        = _geometry->comm();
                _mcombine_g.roles                = -1U;
                _mcombine_g.data                 = (pami_pipeworkqueue_t*) & _pwq_src;
                _mcombine_g.data_participants    = (pami_topology_t*)t_master;
                _mcombine_g.results              = (pami_pipeworkqueue_t*) & _pwq_dest;
                _mcombine_g.results_participants = (pami_topology_t*)t_master;
                _mcombine_g.optor                = (pami_op)op;
                _mcombine_g.dtype                = (pami_dt)dt;
                _mcombine_g.count                = scountDt;
                _startFcn                        = &MultiCombineComposite2Device::start1;
                _count                           = 1;
                TRACE_FORMAT( "<%p>Global Only Setting up start1:", this);
                return;
              }

            // This is the non-master participant in the reduction
            // In this case, the task will participate in a local multicombine as a contributer(only)
            // followed by a local multicast(as a recipient).  To detect this case
            // we must not be a master task

            // This extra PWQ is pointing at the reception buffer.  This means that the
            // reception buffers will be overwritten, maybe more than once
            // \todo Do we need some scratch space if we want to do something like in place?
            _pwq_inter0.configure(
                                  rcvbuf,  // buffer
                                  sbytes,                          // buffer bytes
                                  0,                               // amount initially in buffer
                                  stype,
                                  rtype);

            if (!amMaster)
              {
                // The local multicombine
                // Source is local topology
                // Destination is the global master, a reduction
                //
                // \note we supply a result pwq in case the mcombine_l expects reduce
                // and can't do a simple reduction (probably ignores results_participants)
                //
                _mcombine_l.cb_done.clientdata   = this;
                _mcombine_l.cb_done.function     = composite_done;
                _mcombine_l.connection_id        = _geometry->comm();
                _mcombine_l.roles                = -1U;
                _mcombine_l.data                 = (pami_pipeworkqueue_t*) & _pwq_src;
                _mcombine_l.data_participants    = (pami_topology_t*)t_local;
                _mcombine_l.results              = (pami_pipeworkqueue_t*) & _pwq_inter0; // results can go in dest buffer
                _mcombine_l.results_participants = (pami_topology_t*)t_my_master;
                _mcombine_l.optor                = (pami_op)op;
                _mcombine_l.dtype                = (pami_dt)dt;
                _mcombine_l.count                = scountDt;

                // Also, prepare the local multicast, with the root as the local master
                // We are guaranteed to not have an early arrival with this multicast if we post
                // it first.  This protocol expects the local multicast to be 2 sided
                _mcast_l.cb_done.function        = composite_done;
                _mcast_l.cb_done.clientdata      = this;
                _mcast_l.connection_id           = _geometry->comm();
                _mcast_l.roles                   = -1U;
                _mcast_l.bytes                   = xfer->stypecount * stype->GetDataSize();
                _mcast_l.src                     = NULL;
                _mcast_l.src_participants        = (pami_topology_t*)t_my_master;
                _mcast_l.dst                     = (pami_pipeworkqueue_t*) & _pwq_dest;
                _mcast_l.dst_participants        = (pami_topology_t*)t_local;
                _mcast_l.msginfo                 = 0;
                _mcast_l.msgcount                = 0;
                _startFcn                        = &MultiCombineComposite2Device::start2;
                _count                           = 2;
                TRACE_FORMAT( "<%p>Non Master Setting up start2:", this);
                return;
              }

            // The final case:  this task a global master, with local nodes as well
            // In this case, we will be the target of a local reduction, and participate
            // in this reduction.  The results will chain into a global reduction, which
            // will chain into a local mcast:
            // local_mc[local_topo,me]-->global_mc[master topo, master topo]-->local_mcast[me, local_topo]

            // This extra PWQ is pointing at the reception buffer.  This means that the
            // reception buffers will be overwritten, maybe more than once
            // \todo Do we need some scratch space if we want to do something like in place?
            _pwq_inter1.configure(
                                  rcvbuf,  // buffer
                                  sbytes,                          // buffer bytes
                                  0,                               // amount initially in buffer
                                  stype,
                                  rtype);

            _mcombine_l.cb_done.clientdata   = this;
            _mcombine_l.cb_done.function     = composite_done;
            _mcombine_l.connection_id        = _geometry->comm();
            _mcombine_l.roles                = -1U;
            _mcombine_l.data                 = (pami_pipeworkqueue_t*) & _pwq_src;
            _mcombine_l.data_participants    = (pami_topology_t*)t_local;
            _mcombine_l.results              = (pami_pipeworkqueue_t*) & _pwq_inter0; // results can go in dest buffer
            _mcombine_l.results_participants = (pami_topology_t*)t_my_master;       // me!
            _mcombine_l.optor                = (pami_op)op;
            _mcombine_l.dtype                = (pami_dt)dt;
            _mcombine_l.count                = scountDt;

            _mcombine_g.cb_done.clientdata   = this;
            _mcombine_g.cb_done.function     = composite_done;
            _mcombine_g.connection_id        = _geometry->comm();
            _mcombine_g.roles                = -1U;
            _mcombine_g.data                 = (pami_pipeworkqueue_t*) & _pwq_inter0;
            _mcombine_g.data_participants    = (pami_topology_t*)t_master;
            _mcombine_g.results              = (pami_pipeworkqueue_t*) & _pwq_inter1;
            _mcombine_g.results_participants = (pami_topology_t*)t_master;
            _mcombine_g.optor                = (pami_op)op;
            _mcombine_g.dtype                = (pami_dt)dt;
            _mcombine_g.count                = scountDt;

            _mcast_l.cb_done.function        = composite_done;
            _mcast_l.cb_done.clientdata      = this;
            _mcast_l.connection_id           = _geometry->comm();
            _mcast_l.roles                   = -1U;
            _mcast_l.bytes                   = xfer->stypecount * stype->GetDataSize();
            _mcast_l.src                     = (pami_pipeworkqueue_t*) & _pwq_inter1;
            _mcast_l.src_participants        = (pami_topology_t*)t_my_master;  // me!
            _mcast_l.dst                     = (pami_pipeworkqueue_t*) & _pwq_dest;
            _mcast_l.dst_participants        = (pami_topology_t*)t_local;
            _mcast_l.msginfo                 = 0;
            _mcast_l.msgcount                = 0;
            _count                           = 3;
            _startFcn                        = &MultiCombineComposite2Device::start3;
            TRACE_FORMAT( "<%p>Master(local and global) Setting up start3:", this);
            TRACE_FN_EXIT();
          }


        // This protocol currently has some issues with the CollShm device.
        // It is currently disabled in favor of the "over allreduce" template
        inline void setupReduce(Interfaces::NativeInterface      *native_l,
                                Interfaces::NativeInterface      *native_g,
                                ConnectionManager::SimpleConnMgr *cmgr,
                                pami_geometry_t                   g,
                                pami_xfer_t                      *cmd,
                                pami_event_function               fn,
                                void                             *cookie)
          {
            TRACE_FN_ENTER();
            pami_result_t    rc             = PAMI_SUCCESS;
            PAMI::Topology  *t_master       = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);
            PAMI::Topology  *t_local        = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
            PAMI::Topology  *t_my_master    = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_MASTER_TOPOLOGY_INDEX);
            bool             amMaster       = t_my_master->isEndpointMember(native_l->endpoint());
            bool             amRoot         = native_l->endpoint() == cmd->cmd.xfer_reduce.root;
            bool             sameNodeAsRoot = false;
            _deviceInfo                     = _geometry->getKey(0,PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID);
            unsigned        typesize;
            coremath        func;
            uintptr_t op, dt;
            PAMI::Type::TypeFunc::GetEnums(cmd->cmd.xfer_reduce.stype,
                                           cmd->cmd.xfer_reduce.op,
                                           dt,op);

            getReduceFunction((pami_dt)dt,(pami_op)op,
                              typesize,
                              func);
            TypeCode       * stype       = (TypeCode *) cmd->cmd.xfer_reduce.stype;
            TypeCode       * rtype       = (TypeCode *) cmd->cmd.xfer_reduce.rtype;
            size_t           sizeOfType  = stype->GetDataSize();
            size_t           sbytes      = cmd->cmd.xfer_reduce.stypecount * sizeOfType;
            size_t           scountDt    = cmd->cmd.xfer_reduce.stypecount;

            // TODO:  fix for multi-ep
            new(&_t_root) PAMI::Topology(cmd->cmd.xfer_reduce.root);
            new(&_t_me)   PAMI::Topology(native_l->endpoint());

            TRACE_FORMAT( "setting up PWQ's %p %p, sbytes=%zd buf=%p:  root=%d, me=%u, amRoot=%d _t_root=%d _t_me=%d _t_root=%p _t_me=%p",
                          &_pwq_src, &_pwq_dest, sbytes, cmd->cmd.xfer_reduce.sndbuf,
                          cmd->cmd.xfer_reduce.root, native_l->endpoint(), amRoot,
                          _t_root.index2Endpoint(0),_t_me.index2Endpoint(0),
                          &_t_root, &_t_me);

            // Create a "flat pwq" for the send buffer
            _pwq_src.configure(cmd->cmd.xfer_reduce.sndbuf,  // buffer
                               sbytes,                       // buffer bytes
                               sbytes,                       // amount initially in buffer
                               stype,
                               rtype);
            _pwq_dest.configure(cmd->cmd.xfer_reduce.rcvbuf, // buffer
                                sbytes,                      // buffer bytes
                                0,                           // amount initially in buffer
                                stype,
                                rtype);
            _user_done.clientdata = cmd->cookie;
            _user_done.function   = cmd->cb_done;

            // The "Only Local" case
            // This means the geometry only contains local tasks
            // A single multicombine will suffice to handle the reduction
            if (t_local->size() == _geometry->size())
              {
                _mcombine_l.cb_done.clientdata   = this;
                _mcombine_l.cb_done.function     = composite_done;
                _mcombine_l.connection_id        = _geometry->comm();
                _mcombine_l.roles                = -1U;
                _mcombine_l.data                 = (pami_pipeworkqueue_t*) & _pwq_src;
                _mcombine_l.data_participants    = (pami_topology_t*)t_local;
                _mcombine_l.results              = (pami_pipeworkqueue_t*) & _pwq_dest;
                _mcombine_l.results_participants = (pami_topology_t*)&_t_root;
                _mcombine_l.optor                = (pami_op)op;
                _mcombine_l.dtype                = (pami_dt)dt;
                _mcombine_l.count                = scountDt;
                _startFcn                        = &MultiCombineComposite2Device::start0;
                _count                           = 1;
                TRACE_FORMAT( "<%p>Local Only MASTER Setting up start0: local native() %p", this, _native_l);
                return;
              }

            // Find the master proxy task
            // TODO:  Fix for endpoints, do not use mapping!
            size_t count, total = t_local->size();
            size_t masterNode=0;
            total  = t_master->size();
            for(count=0; count<total; count++)
            {
              fprintf(stderr, "FATAL!  This code should be fixed when enabled!");
              PAMI_assert(0);
              size_t rank = t_master->index2Endpoint(count);
              if(__global.mapping.isPeer(rank, cmd->cmd.xfer_reduce.root))
              {
                masterNode = rank;
                break;
              }
            }
            PAMI_assert(count < total); // no local master?
            new(&_t_masterproxy) PAMI::Topology(masterNode);
            TRACE_FORMAT( "<%p>Master Proxy is %zd:", this, masterNode);


            // The "Only Global Master" case
            // My task only belongs to a global master, with no other local task
            // A single multicombine will suffice in this case as well
            // To detect this case, the local size will be only me
            // Do not store any results unless I am root
            if (t_local->size() == 1)
              {
                _mcombine_g.cb_done.clientdata   = this;
                _mcombine_g.cb_done.function     = composite_done;
                _mcombine_g.connection_id        = _geometry->comm();
                _mcombine_g.roles                = -1U;
                _mcombine_g.data                 = (pami_pipeworkqueue_t*) & _pwq_src;
                _mcombine_g.data_participants    = (pami_topology_t*)t_master;
                if(amRoot)
                  _mcombine_g.results              = (pami_pipeworkqueue_t*)&_pwq_dest;
                else
                  _mcombine_g.results              = NULL;
                _mcombine_g.results_participants = (pami_topology_t*)&_t_masterproxy;
                _mcombine_g.optor                = (pami_op)op;
                _mcombine_g.dtype                = (pami_dt)dt;
                _mcombine_g.count                = scountDt;
                _startFcn                        = &MultiCombineComposite2Device::start1;
                _count                           = 1;
                TRACE_FORMAT( "<%p>Global Only Setting up start1:", this);
                return;
              }


            // Detect if we are on the same node as the root
            // TODO:  This search is linear and should be optimized
            // for large numbers of local nodes
            total = t_local->size();
            for(count=0; count<total; count++)
            {
              if(t_local->index2Endpoint(count) == cmd->cmd.xfer_reduce.root)
              {
                sameNodeAsRoot = true;
                break;
              }
            }
            // This is the non-master participant in the reduction
            // We have two cases here.  If we are the root task, and not the master,
            // we have to receive the data via a multicast from the global master
            // If we are not the root task, a simple local reduction will suffice

            // This extra PWQ is pointing at the reception buffer.  This means that the
            // reception buffers will be overwritten, maybe more than once
            // \todo Do we need some scratch space if we want to do something like in place?

            if (!amMaster)
              {
                rc = __global.heap_mm->memalign((void **)&_temp_results, 16, sbytes);
                PAMI_assert(rc == PAMI_SUCCESS && _temp_results != NULL);
                PAMI_assert(_temp_results != NULL);   // no local master?
                _pwq_inter0.configure(_temp_results,  // buffer
                                      sbytes,         // buffer bytes
                                      0,              // amount initially in buffer
                                      stype,
                                      rtype);
                _pwq_inter1.configure(_temp_results,  // buffer
                                      sbytes,         // buffer bytes
                                      0,              // amount initially in buffer
                                      stype,
                                      rtype);

                if(sameNodeAsRoot)
                {
                  _mcombine_l.cb_done.clientdata   = this;
                  _mcombine_l.cb_done.function     = composite_done;
                  _mcombine_l.connection_id        = _geometry->comm();
                  _mcombine_l.roles                = -1U;
                  _mcombine_l.data                 = (pami_pipeworkqueue_t*) &_pwq_src;
                  _mcombine_l.data_participants    = (pami_topology_t*)t_local;
                  _mcombine_l.results              = (pami_pipeworkqueue_t*) & _pwq_inter0; // results can go in dest buffer
                  _mcombine_l.results_participants = (pami_topology_t*)t_my_master;
                  _mcombine_l.optor                = (pami_op)op;
                  _mcombine_l.dtype                = (pami_dt)dt;
                  _mcombine_l.count                = scountDt;

                  _mcast_l.cb_done.function        = composite_done;
                  _mcast_l.cb_done.clientdata      = this;
                  _mcast_l.connection_id           = _geometry->comm();
                  _mcast_l.roles                   = -1U;
                  _mcast_l.bytes                   = cmd->cmd.xfer_reduce.stypecount * stype->GetDataSize();
                  _mcast_l.src_participants        = (pami_topology_t*)t_my_master;
                  if(amRoot)
                    _mcast_l.dst                   = (pami_pipeworkqueue_t*)&_pwq_dest;
                  else
                    _mcast_l.dst                   = (pami_pipeworkqueue_t*)&_pwq_inter1;
                  _mcast_l.dst_participants        = (pami_topology_t*)t_local;
                  _mcast_l.msginfo                 = 0;
                  _mcast_l.msgcount                = 0;

                  _startFcn                        = &MultiCombineComposite2Device::start2;
                  _count                           = 2;
                  TRACE_FORMAT( "<%p>Non Master, same node as root (local multicombine+local multicast):start2", this);
                  return;
                }
                else
                {
                  _mcombine_l.cb_done.clientdata   = this;
                  _mcombine_l.cb_done.function     = composite_done;
                  _mcombine_l.connection_id        = _geometry->comm();
                  _mcombine_l.roles                = -1U;
                  _mcombine_l.data                 = (pami_pipeworkqueue_t*) &_pwq_src;
                  _mcombine_l.data_participants    = (pami_topology_t*)t_local;
                  _mcombine_l.results              = (pami_pipeworkqueue_t*) & _pwq_inter0; // results can go in dest buffer
                  _mcombine_l.results_participants = (pami_topology_t*)t_my_master;
                  _mcombine_l.optor                = (pami_op)op;
                  _mcombine_l.dtype                = (pami_dt)dt;
                  _mcombine_l.count                = scountDt;
                  _startFcn                        = &MultiCombineComposite2Device::start0;
                  _count                           = 1;
                  TRACE_FORMAT( "<%p>Non Master, different node than root (local multicombine only):start0", this);
                  return;
                }
              }

            // This task a global master, with local nodes as well
            // In this case, we will be the target of a local reduction, and participate
            // in this reduction.  The results will chain into a global reduction, which
            // will chain into a local mcast:
            // local_mc[local_topo,me]-->global_mc[master topo, master topo]-->local_mcast[me, local_topo]

            // This extra PWQ is pointing at the reception buffer.  This means that the
            // reception buffers will be overwritten, maybe more than once
            // \todo Do we need some scratch space if we want to do something like in place?
            if(!_temp_results)
            {
              rc = __global.heap_mm->memalign((void **)&_temp_results, 16, sbytes);
              PAMI_assert(rc == PAMI_SUCCESS);
              PAMI_assert(_temp_results != NULL);   // no local master?
            }
            _pwq_inter0.configure(_temp_results,  // buffer
                                  sbytes,         // buffer bytes
                                  0,              // amount initially in buffer
                                  stype,
                                  rtype);
            _pwq_inter1.configure(_temp_results,  // buffer
                                  sbytes,         // buffer bytes
                                  0,              // amount initially in buffer
                                  stype,
                                  rtype);

            _mcombine_l.cb_done.clientdata   = this;
            _mcombine_l.cb_done.function     = composite_done;
            _mcombine_l.connection_id        = _geometry->comm();
            _mcombine_l.roles                = -1U;
            _mcombine_l.data                 = (pami_pipeworkqueue_t*) & _pwq_src;
            _mcombine_l.data_participants    = (pami_topology_t*)t_local;
            _mcombine_l.results              = (pami_pipeworkqueue_t*) & _pwq_inter0; // results can go in dest buffer
            _mcombine_l.results_participants = (pami_topology_t*)&_t_me;       // me!
            _mcombine_l.optor                = (pami_op)op;
            _mcombine_l.dtype                = (pami_dt)dt;
            _mcombine_l.count                = scountDt;

            _mcombine_g.cb_done.clientdata   = this;
            _mcombine_g.cb_done.function     = composite_done;
            _mcombine_g.connection_id        = _geometry->comm();
            _mcombine_g.roles                = -1U;
            _mcombine_g.data                 = (pami_pipeworkqueue_t*) & _pwq_inter0;
            _mcombine_g.data_participants    = (pami_topology_t*)t_master;
            if(amRoot)
              _mcombine_g.results              = (pami_pipeworkqueue_t*) & _pwq_dest;
            else
              _mcombine_g.results              = (pami_pipeworkqueue_t*) & _pwq_inter1;
            _mcombine_g.results_participants = (pami_topology_t*)&_t_masterproxy;
            _mcombine_g.optor                = (pami_op)op;
            _mcombine_g.dtype                = (pami_dt)dt;
            _mcombine_g.count                = scountDt;

            if(sameNodeAsRoot) // Am global master am on same node as root
            {
              _pwq_inter2.configure(_temp_results,  // buffer
                                    sbytes,         // buffer bytes
                                    0,              // amount initially in buffer
                                    stype,
                                    rtype);
              _mcast_l.cb_done.function        = composite_done;
              _mcast_l.cb_done.clientdata      = this;
              _mcast_l.connection_id           = _geometry->comm();
              _mcast_l.roles                   = -1U;
              _mcast_l.bytes                   = cmd->cmd.xfer_reduce.stypecount * stype->GetDataSize();
              _mcast_l.src                     = (pami_pipeworkqueue_t*) & _pwq_inter1;
              _mcast_l.src_participants        = (pami_topology_t*)t_my_master;  // me!
              _mcast_l.dst                     = (pami_pipeworkqueue_t*) & _pwq_inter2;
              _mcast_l.dst_participants        = (pami_topology_t*)t_local;
              _mcast_l.msginfo                 = 0;
              _mcast_l.msgcount                = 0;
              _count                           = 3;
              _startFcn                        = &MultiCombineComposite2Device::start3;
              TRACE_FORMAT( "<%p>Master(local multicombine + global multicombine + local multicast) Setting up start3:", this);
              return;
            }
            else // Am global master, am NOT root, different node than root
            {
              _count                           = 2;
              _startFcn                        = &MultiCombineComposite2Device::start4;
              TRACE_FORMAT( "<%p>Master(local multicombine and global multicombine):Setting up start4:", this);
              return;
            }
            PAMI_abort();
            TRACE_FN_EXIT();
            return;
          }

          MultiCombineComposite2Device (Interfaces::NativeInterface      *native_l,
                                        Interfaces::NativeInterface      *native_g,
                                        ConnectionManager::SimpleConnMgr *cmgr,
                                        pami_geometry_t                   g,
                                        pami_endpoint_t                   me_ep,
                                        pami_xfer_t                      *cmd,
                                        pami_event_function               fn,
                                        void                             *cookie) :
              Composite(),
              _native_l(native_l),
              _native_g(native_g),
              _geometry((PAMI_GEOMETRY_CLASS*)g),
              _me_ep(me_ep),
              _temp_results(NULL),
              _throwaway_results(NULL),
              _fn(fn),
              _cookie(cookie)
          {
            // This should not generate a branch
            // ReduceOnly is a template parameter
            if(ReduceOnly==1)
            {
              PAMI_abortf("ReduceOnly %d not supported\n",ReduceOnly);
              setupReduce(native_l, native_g, cmgr, g, cmd, fn, cookie);
            }
            else if(ReduceOnly==2)
              setupReduceCommon<pami_reduce_t> (native_l, native_g, cmgr, g, cmd,
                                                fn, cookie,cmd->cmd.xfer_reduce.root);
            else
              setupReduceCommon<pami_allreduce_t> (native_l, native_g, cmgr, g, cmd,
                                                   fn, cookie,0xFFFFFFFF);
          }


          void start0()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "<%p>local native multicombine %p", this, _native_l);
            _native_l->multicombine(&_mcombine_l, _deviceInfo);
            TRACE_FN_EXIT();
          }
          void start1()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "<%p>global native multicombine %p", this, _native_g);
            _native_g->multicombine(&_mcombine_g, _deviceInfo);
            TRACE_FN_EXIT();
          }
          void start2()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "<%p>local mcast+local multicombine %p", this, _native_l);
            _native_l->multicombine(&_mcombine_l, _deviceInfo);
            _native_l->multicast(&_mcast_l, _deviceInfo);
            TRACE_FN_EXIT();
          }
          void start3()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "<%p>local mcast+local multicombine+global multicombine l=%p g=%p"
                   , this, _native_l, _native_g);
            _native_l->multicombine(&_mcombine_l, _deviceInfo);
            _native_g->multicombine(&_mcombine_g, _deviceInfo);
            _native_l->multicast(&_mcast_l, _deviceInfo);
            TRACE_FN_EXIT();
          }

        void start4()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "<%p>local multicombine+global multicombine l=%p g=%p"
                   , this, _native_l, _native_g);
            _native_l->multicombine(&_mcombine_l, _deviceInfo);
            _native_g->multicombine(&_mcombine_g, _deviceInfo);
            TRACE_FN_EXIT();
          }

          virtual void start()
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT( "<%p>", this);
            (this->*_startFcn)();
            TRACE_FN_EXIT();
          }
          void (MultiCombineComposite2Device::*_startFcn)();
          Interfaces::NativeInterface         *_native_l;
          Interfaces::NativeInterface         *_native_g;
          PAMI_GEOMETRY_CLASS                 *_geometry;
          void                                *_deviceInfo;
          PAMI::Topology                      *_l_topology;
          PAMI::Topology                      *_g_topology;
          pami_multicombine_t                  _mcombine_l;
          pami_multicombine_t                  _mcombine_g;
          pami_multicast_t                     _mcast_l;
          PAMI::PipeWorkQueue                  _pwq_src;
          PAMI::PipeWorkQueue                  _pwq_dest;
          PAMI::PipeWorkQueue                  _pwq_inter0;
          PAMI::PipeWorkQueue                  _pwq_inter1;
          PAMI::PipeWorkQueue                  _pwq_inter2;
          pami_callback_t                      _user_done;
          size_t                               _count;
          pami_endpoint_t                      _root_ep;
          pami_endpoint_t                      _me_ep;
          PAMI::Topology                       _t_root;
          PAMI::Topology                       _t_me;
          PAMI::Topology                       _t_masterproxy;
          char                                *_temp_results;
          char                                *_throwaway_results;
          pami_event_function                  _fn;
          void                                *_cookie;
      };

    };
  };
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif
