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
 * \file algorithms/protocols/broadcast/MultiCastComposite.h
 * \brief Simple composite based on multicast
 */
#ifndef __algorithms_protocols_broadcast_MultiCastComposite_h__
#define __algorithms_protocols_broadcast_MultiCastComposite_h__

#include "algorithms/composite/Composite.h"
#include "algorithms/connmgr/SimpleConnMgr.h"
#include "algorithms/connmgr/CommSeqConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactoryT.h"
#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"
#include "util/queue/MatchQueue.h"
#include "util/queue/Queue.h"

#include "util/trace.h"

#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif


namespace CCMI
{
  namespace Adaptor
  {
    namespace Broadcast
    {

      /// \brief An all-sided multicast composite built on an all-sided
      /// multicast.
      ///
      template < 
        bool T_inline                   = false, 
        class T_Native                  = Interfaces::NativeInterface,
        PAMI::Geometry::ckeys_t T_Gkey  = PAMI::Geometry::CKEY_MCAST_CLASSROUTEID
        >
      class MultiCastComposite : public CCMI::Executor::Composite
      {
      protected:
        pami_endpoint_t                      _root_ep;
        PAMI::Topology                       _root_topo;
        PAMI::PipeWorkQueue                  _pwq;

      public:
        MultiCastComposite (pami_context_t               ctxt,
                            size_t                       ctxt_id,
                            Interfaces::NativeInterface            * native,
                            ConnectionManager::SimpleConnMgr     * cmgr,
                            pami_geometry_t                        g,
                            pami_xfer_t                          * cmd,
                            pami_event_function                    fn,
                            void                                 * cookie) :
        Composite(),
        _root_ep(cmd->cmd.xfer_broadcast.root),
        _root_topo(&_root_ep,1,PAMI::tag_eplist())
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p> type %#zX, count %zu, root %zu", this, (size_t)cmd->cmd.xfer_broadcast.type, cmd->cmd.xfer_broadcast.typecount, (size_t)cmd->cmd.xfer_broadcast.root);

          PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *)g;
          void *deviceInfo                  = geometry->getKey(ctxt_id, T_Gkey);
          PAMI::Topology *destinations = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);


          PAMI::Type::TypeCode * type_obj = (PAMI::Type::TypeCode *)cmd->cmd.xfer_broadcast.type;

          /// \todo Support non-contiguous
          //assert(type_obj->IsContiguous() &&  type_obj->IsPrimitive());

          unsigned        sizeOfType   = type_obj->GetDataSize();
          unsigned        strideOfType = type_obj->GetExtent();

          size_t bytes  = cmd->cmd.xfer_broadcast.typecount * sizeOfType;
          size_t stride = cmd->cmd.xfer_broadcast.typecount * strideOfType;
          size_t pbytes = 0;

          if (cmd->cmd.xfer_broadcast.root == native->endpoint())
            pbytes = stride;

          _pwq.configure(cmd->cmd.xfer_broadcast.buf, stride, pbytes, type_obj, type_obj);

          pami_multicast_t minfo;
          minfo.cb_done.function   = fn;     //_cb_done;
          minfo.cb_done.clientdata = cookie; //_clientdata;
          minfo.dispatch = 0;
          minfo.msginfo  = 0;
          minfo.connection_id      = 0; /// \todo ?
          minfo.roles              = -1U;
          minfo.dst_participants   = (pami_topology_t *) destinations;
          minfo.src_participants   = (pami_topology_t *) & _root_topo;
          minfo.src                = (pami_pipeworkqueue_t *) & _pwq;
          minfo.dst                = (pami_pipeworkqueue_t *) & _pwq;
          minfo.msgcount           = 0;
          minfo.bytes              = bytes;

          if (T_inline)
          {
            T_Native *t_native = (T_Native *) native;
            t_native->T_Native::multicast(&minfo, deviceInfo);
          }
          else
          {
            native->multicast(&minfo, deviceInfo);
          }
        TRACE_FN_EXIT();
        }

        void setContext (pami_context_t context) {}

        virtual void start()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>", this);
          TRACE_FN_EXIT();
        }
      };

      /// \brief An all-sided multicast composite built on an active message
      /// multicast.
      ///
      /// It multisyncs before doing the active message multicast.
      /// When the multisync is complete, the root will multicast and the
      /// non-roots will have buffers ready to receive the data.
      ///
      template <class T_Geometry>
      class MultiCastComposite2 : public CCMI::Executor::Composite
      {
      protected:
        Interfaces::NativeInterface        * _native;
        PAMI_GEOMETRY_CLASS                * _geometry;
        pami_broadcast_t                     _xfer_broadcast;
        pami_endpoint_t                      _root_ep;
        PAMI::Topology                       _all;
        PAMI::Topology                       _root_topo;
        PAMI::Topology                       _destinations;
        PAMI::PipeWorkQueue                  _src;
        PAMI::PipeWorkQueue                  _dst;
        pami_multicast_t                     _minfo;
        pami_multisync_t                     _msync;
        size_t                               _bytes;
        char                               * _buffer;
        void                               * _deviceMcastInfo;
        void                               * _deviceMsyncInfo;

      public:
        ~MultiCastComposite2()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>", this);
          __global.heap_mm->free(_buffer);
          TRACE_FN_EXIT();
        }
        MultiCastComposite2 (Interfaces::NativeInterface          * mInterface,
                             ConnectionManager::CommSeqConnMgr    * cmgr,
                             pami_geometry_t                        g,
                             pami_xfer_t                          * cmd,
                             pami_event_function                    fn,
                             void                                 * cookie) :
        Composite(), _native(mInterface), _geometry((PAMI_GEOMETRY_CLASS*)g),
        _xfer_broadcast(cmd->cmd.xfer_broadcast), _root_ep(cmd->cmd.xfer_broadcast.root),
        _root_topo(&_root_ep,1,PAMI::tag_eplist()),
        _buffer(NULL)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p> type %#zX, count %zu, root %zu", this, (size_t)cmd->cmd.xfer_broadcast.type, cmd->cmd.xfer_broadcast.typecount, cmd->cmd.xfer_broadcast.root);
          PAMI::Type::TypeCode * type_obj = (PAMI::Type::TypeCode *)cmd->cmd.xfer_broadcast.type;

          /// \todo Support non-contiguous
          //assert(type_obj->IsContiguous() &&  type_obj->IsPrimitive());

          unsigned        sizeOfType   = type_obj->GetDataSize();
          unsigned        strideOfType = type_obj->GetExtent();
          _bytes                       = cmd->cmd.xfer_broadcast.typecount * sizeOfType;
          size_t          stride       = cmd->cmd.xfer_broadcast.typecount * strideOfType;

          _deviceMcastInfo                  = _geometry->getKey(mInterface->contextid(),PAMI::Geometry::CKEY_MCAST_CLASSROUTEID);
          _deviceMsyncInfo                  = _geometry->getKey(mInterface->contextid(),PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID);

          _all = *(PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
          _all.subtractTopology(&_destinations,  &_root_topo);

          DO_DEBUG(unsigned j = 0;)
          DO_DEBUG(for ( j = 0; j < _root_topo.size(); ++j) fprintf(stderr, "root[%u]=%zu, size %zu\n", j, (size_t)_root_topo.index2Endpoint(j), _root_topo.size()));

          DO_DEBUG(for ( j = 0; j < _destinations.size(); ++j) fprintf(stderr, "destinations[%u]=%zu, size %zu\n", j, (size_t)_destinations.index2Endpoint(j), _destinations.size()));

          DO_DEBUG(for ( j = 0; j < _all.size(); ++j) fprintf(stderr, "all[%u]=%zu, size %zu\n", j, (size_t)_all.index2Endpoint(j), _all.size()));

          if (cmd->cmd.xfer_broadcast.root == mInterface->endpoint())
          {
            _src.configure(cmd->cmd.xfer_broadcast.buf, stride, stride, type_obj, type_obj);
            /// \todo unless the device lets me toss unwanted data, we need a dummy buffer to receive.
            pami_result_t prc;
            prc = __global.heap_mm->memalign((void **) & _buffer, 0, stride);
            PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _buffer failed");
            _dst.configure(_buffer, stride, 0, type_obj, type_obj);
          }
          else
          {
            //_buffer = (char*) & _bytes; // dummy buffer - unused
            _src.configure((char*)NULL, 0, 0);
            _dst.configure(cmd->cmd.xfer_broadcast.buf, stride, 0, type_obj, type_obj);
          }

          // Initialize the mcast
          //_minfo.cb_done.function   = _cb_done;
          //_minfo.cb_done.clientdata = _clientdata;
          T_Geometry *bgqGeometry = (T_Geometry *)g;
          unsigned comm = bgqGeometry->comm();
          _minfo.connection_id      =  cmgr->updateConnectionId(comm);
          _minfo.roles              = -1U;
          _minfo.dst_participants   = (pami_topology_t *) & _destinations;
          _minfo.src_participants   = (pami_topology_t *) & _root_topo;
          _minfo.src                = (pami_pipeworkqueue_t *) & _src;
          _minfo.dst                = (pami_pipeworkqueue_t *) & _dst;
          _minfo.msgcount           = 0;
          _minfo.bytes              = _bytes;

          // Initialize the msync
          _msync.cb_done.function   = cb_msync_done;
          _msync.cb_done.clientdata = this;
          _msync.connection_id      = 0; /// \todo ?
          _msync.roles              = -1U;
          _msync.participants       = (pami_topology_t *) & _all;

//        pami_dispatch_callback_function lfn;
//        lfn.multicast = dispatch_multicast_fn;
//        _native->setDispatch(lfn, this);

          TRACE_FN_EXIT();
        }
        virtual void start()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>", this);
          _minfo.cb_done.function   = _cb_done;
          _minfo.cb_done.clientdata = _clientdata;

//          pami_dispatch_callback_function fn;
//          fn.multicast = dispatch_multicast_fn;
//          _native->setDispatch(fn, this);

          // Start the msync. When it completes, it will start the mcast.
          _native->multisync(&_msync, _deviceMsyncInfo);
          TRACE_FN_EXIT();
        }
        void startMcast()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>", this);

          if (_xfer_broadcast.root == _native->endpoint())
            _native->multicast(&_minfo, _deviceMcastInfo);
          TRACE_FN_EXIT();
        }
        static void cb_msync_done(pami_context_t context, void *me, pami_result_t err)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>", me);
          MultiCastComposite2 * composite = (MultiCastComposite2 *) me;
          CCMI_assert (composite != NULL);

          // Msync is done, start the active message mcast on the root
          composite->startMcast();
          TRACE_FN_EXIT();
        }

//      static void dispatch_multicast_fn(const pami_quad_t     * msginfo,       // \param[in] msginfo    Metadata
//                                        unsigned                msgcount,      // \param[in] msgcount Count of metadata
//                                        unsigned                connection_id, // \param[in] connection_id  Stream ID of data
//                                        size_t                  root,          // \param[in] root        Sending task
//                                        size_t                  sndlen,        // \param[in] sndlen      Length of data sent
//                                        void                  * clientdata,    // \param[in] clientdata  Opaque arg
//                                        size_t                * rcvlen,        // \param[out] rcvlen     Length of data to receive
//                                        pami_pipeworkqueue_t ** rcvpwq,        // \param[out] rcvpwq     Where to put recv data
//                                        pami_callback_t       * cb_done)       // \param[out] cb_done    Completion callback to invoke when data received
//      {
//          TRACE_FORMAT( "<%p>", this);
//        MultiCastComposite2 * composite = (MultiCastComposite2 *)clientdata;
//        composite->dispatch_multicast(rcvlen,  rcvpwq,  cb_done);
//      }
//
        void notifyRecv(size_t                  root,          // \param[in]  root       Sending task
                        const pami_quad_t      *msginfo,       // \param[in]  msginfo    Metadata
                        pami_pipeworkqueue_t ** rcvpwq,        // \param[out] rcvpwq     Where to put recv data
                        pami_callback_t       * cb_done)       // \param[out] cb_done    Completion callback to invoke when data received
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>root %zu", this, root);
          *rcvpwq  = (pami_pipeworkqueue_t *) & _dst;
          cb_done->function   = _cb_done;
          cb_done->clientdata = _clientdata;
          TRACE_FN_EXIT();
        }

      };


      template < class T_Geometry, bool T_Endpoint_support = false >
      class MultiCastComposite2DeviceAS : public CCMI::Executor::Composite
      {
      public:

        static void composite_done(pami_context_t  context,
                                   void           *cookie,
                                   pami_result_t   result)
        {
          TRACE_FN_ENTER();
          MultiCastComposite2DeviceAS *m = (MultiCastComposite2DeviceAS*) cookie;
          /* 999 is special case. It means the local mcast is done, start global mcast. */
          if(m->_count == 999)
          {
            TRACE_FORMAT( "<%p>MultiCastComposite2DeviceAS:  local multicast done:  start global->multicast",m);
            PAMI_assert(m->_native_g); /* This is intentionally null except this special case. */
            m->_count = 1; /* One mcast left to finish, after we start it. */
            m->_native_g->multicast(&m->_minfo_g,m->_deviceInfo);
            m->_native_g = NULL;
          }
          else
          {
            m->_count--;
            TRACE_FORMAT( "MultiCastComposite2DeviceAS:  composite done:  count=%ld", m->_count);
  
            if (m->_count == 0)
              m->_master_done.function(context, m->_master_done.clientdata, result);
          }
          TRACE_FN_EXIT();
        }

        ~MultiCastComposite2DeviceAS()
        {
        }

        MultiCastComposite2DeviceAS (Interfaces::NativeInterface                         *native_l,
                                     Interfaces::NativeInterface                         *native_g,
                                     ConnectionManager::SimpleConnMgr                    *cmgr,
                                     pami_geometry_t                                      g,
                                     pami_xfer_t                                         *cmd,
                                     pami_event_function                                  fn,
                                     void                                                *cookie,
                                     PAMI::Queue                       *ue,
                                     PAMI::Queue                       *posted):
        Composite(),
        _geometry((PAMI_GEOMETRY_CLASS*)g),
        _root_ep(cmd->cmd.xfer_broadcast.root),
        _justme_ep(native_l->endpoint()),
        _root_topo(_root_ep),    // assume !T_Endpoint_support so a single ep/rank topology
        _justme_topo(_justme_ep),// assume !T_Endpoint_support so a single ep/rank topology
        _count(0),
        _native_g(NULL),         // NULL unless it is specifically setup for count=999
        _deviceInfo(NULL)
        {
          TRACE_FN_ENTER();
          if(T_Endpoint_support) // replace assumption above with ep_list topology
          {
            new (&_root_topo)   PAMI::Topology(&_root_ep,1,PAMI::tag_eplist());
            new (&_justme_topo) PAMI::Topology(&_justme_ep,1,PAMI::tag_eplist());
          }
          pami_multicast_t                    minfo_l;

          PAMI::Topology  *t_master    = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);
          PAMI::Topology  *t_local     = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
          PAMI::Topology  *t_my_master = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_MASTER_TOPOLOGY_INDEX);

          // Discover the root node and interesting topology information
          bool             amRoot      = (_root_ep == native_l->endpoint());
          bool             amMaster    = false;
          bool             isRootLocal = false;
          if(T_Endpoint_support) 
          {  
            amMaster    = t_my_master->isEndpointMember(native_l->endpoint());
            isRootLocal = t_local->isEndpointMember(_root_ep);
          }
          else
          {
            amMaster    = t_my_master->isRankMember((pami_task_t)native_l->endpoint());
            isRootLocal = t_local->isRankMember((pami_task_t)_root_ep);
          }

          _deviceInfo             = _geometry->getKey(native_l->contextid(),PAMI::Geometry::CKEY_MCAST_CLASSROUTEID);
          PAMI::Type::TypeCode *tc     = (PAMI::Type::TypeCode*)cmd->cmd.xfer_broadcast.type;
          size_t           bytes       = cmd->cmd.xfer_broadcast.typecount * tc->GetDataSize();
          size_t           stride      = cmd->cmd.xfer_broadcast.typecount * tc->GetExtent();
          size_t           numMasters  = t_master->size();
          size_t           numLocal    = t_local->size();

          if (numLocal > 1) _count++;

          TRACE_FORMAT( "MultiCastComposite2DeviceAS:  In Composite Constructor, setting up PWQ's %p %p, bytes=%ld buf=%p",
                         &_pwq0, &_pwq1, bytes, cmd->cmd.xfer_broadcast.buf);

          TRACE_FORMAT( "Native Interfaces %lx %lx", (uint64_t)native_g, (uint64_t)native_l);

          if (bytes == 0)
          {
            fn(this->_context,  cookie, PAMI_SUCCESS);
            TRACE_FN_EXIT();
            return;
          }

          size_t           initBytes;   
          if (amRoot)
            initBytes = stride;
          else
            initBytes = 0;

          // Create a "flat pwq" for the send buffer
          _pwq0.configure(
                         cmd->cmd.xfer_broadcast.buf,     // buffer
                         stride,                          // buffer bytes
                         initBytes,                       // amount initially in buffer
                         tc,
                         tc);

          _pwq1.configure(
                         cmd->cmd.xfer_broadcast.buf,     // buffer
                         stride,                          // buffer bytes
                         initBytes,                       // amount initially in buffer
                         tc,
                         tc);

          _minfo_g.cb_done.function   = composite_done;
          minfo_l.cb_done.function   = composite_done;
          _master_done.function       = fn;
          _master_done.clientdata     = cookie;   

          if (amRoot && amMaster)
          {
            // Participate in local and global multicast as the source
            TRACE_STRING( "MultiCastComposite2DeviceAS:  Root/Master");       

            if (numMasters > 1)
            {
              //_minfo_g.client             = NULL;              // Not used by device
              //_minfo_g.context            = NULL;              // Not used by device
              _minfo_g.cb_done.clientdata = this;
              _minfo_g.connection_id      = _geometry->comm();
              _minfo_g.roles              = -1U;
              _minfo_g.bytes              = bytes;
              _minfo_g.src                = (pami_pipeworkqueue_t*) & _pwq1;
              _minfo_g.src_participants   = (pami_topology_t*) & _root_topo;
              _minfo_g.dst                = NULL;
              _minfo_g.dst_participants   = (pami_topology_t*)t_master;
              _minfo_g.msginfo            = 0;
              _minfo_g.msgcount           = 0;     
              _count++;
              TRACE_STRING( "MultiCastComposite2Device:  Global NI, start multicast");
              PAMI_assert (native_g != NULL);
              native_g->multicast(&_minfo_g, _deviceInfo);
            }

            if (numLocal > 1)
            {
              //minfo_l.client             = NULL;              // Not used by device
              //minfo_l.context            = NULL;              // Not used by device
              minfo_l.cb_done.clientdata = this;
              minfo_l.connection_id      = _geometry->comm();
              minfo_l.roles              = -1U;
              minfo_l.bytes              = bytes;
              minfo_l.src                = (pami_pipeworkqueue_t*) & _pwq0;
              minfo_l.src_participants   = (pami_topology_t*) & _root_topo;
              minfo_l.dst                = NULL;
              minfo_l.dst_participants   = (pami_topology_t*)t_local;
              minfo_l.msginfo            = 0;
              minfo_l.msgcount           = 0;
              TRACE_STRING( "MultiCastComposite2Device:  Local NI, start multicast");
              PAMI_assert (native_l != NULL);
              native_l->multicast(&minfo_l, _deviceInfo);
            }
          }
          else if (amRoot)
          {
            TRACE_STRING( "MultiCastComposite2DeviceAS:  Root only");       
            // I am a root, but not the master, participate in
            // the local broadcast only, as the source
            if (numLocal > 1)
            {
              //minfo_l.client             = NULL;              // Not used by device
              //minfo_l.context            = NULL;              // Not used by device
              minfo_l.cb_done.clientdata = this;
              minfo_l.connection_id      = _geometry->comm();
              minfo_l.roles              = -1U;
              minfo_l.bytes              = bytes;
              minfo_l.src                = (pami_pipeworkqueue_t*) & _pwq0;
              minfo_l.src_participants   = (pami_topology_t*) & _root_topo;
              minfo_l.dst                = NULL;
              minfo_l.dst_participants   = (pami_topology_t*)t_local;
              minfo_l.msginfo            = 0;
              minfo_l.msgcount           = 0;
              TRACE_STRING( "MultiCastComposite2Device:  Local NI, start multicast");
              PAMI_assert (native_l != NULL);
              native_l->multicast(&minfo_l, _deviceInfo);
            }
          }
          else if (amMaster && isRootLocal)
          {
            TRACE_STRING( "MultiCastComposite2DeviceAS:  Master, Root is Local");

            // I am the master task, and on the same node as the root
            // so I will participate in the local broadcast (as a receiver)
            // and I will participate in the global broadcast as a sender
            if (numMasters > 1)
            {
              //_minfo_g.client             = NULL;              // Not used by device
              //_minfo_g.context            = NULL;              // Not used by device
              _minfo_g.cb_done.clientdata = this;
              _minfo_g.connection_id      = _geometry->comm();
              _minfo_g.roles              = -1U;
              _minfo_g.bytes              = bytes;
              _minfo_g.src                = (pami_pipeworkqueue_t*) & _pwq0;  // <--- src buffer
              _minfo_g.src_participants   = (pami_topology_t*) & _justme_topo;
              _minfo_g.dst                = NULL;
              _minfo_g.dst_participants   = (pami_topology_t*)t_master;
              _minfo_g.msginfo            = 0;
              _minfo_g.msgcount           = 0;
              TRACE_STRING( "MultiCastComposite2Device:  Global NI, start multicast");
              PAMI_assert (native_g != NULL);
              if (numLocal > 1) /* Global is started when local finishes.*/
              {  /* We wait for local data before starting global */
                _count = 999;  /* flag this special case with 999 */
                _native_g = native_g; 
              }
              else /* No local, start global now*/
              {  
                _count ++;
                native_g->multicast(&_minfo_g, _deviceInfo);
              }
            }

            if (numLocal > 1)
            {
              //minfo_l.client             = NULL;              // Not used by device
              //minfo_l.context            = NULL;              // Not used by device
              minfo_l.cb_done.clientdata = this;
              minfo_l.connection_id      = _geometry->comm();
              minfo_l.roles              = -1U;
              minfo_l.bytes              = bytes;
              minfo_l.src                = NULL;
              minfo_l.src_participants   = (pami_topology_t*) & _root_topo;
              minfo_l.dst                = (pami_pipeworkqueue_t*) & _pwq0;  // <--- target buffer
              minfo_l.dst_participants   = (pami_topology_t*)t_local;
              minfo_l.msginfo            = 0;
              minfo_l.msgcount           = 0;
              TRACE_STRING( "MultiCastComposite2Device:  Local NI, start multicast");
              PAMI_assert (native_l != NULL);
              native_l->multicast(&minfo_l, _deviceInfo);
            }
          }
          else if (amMaster)
          {
            TRACE_STRING( "MultiCastComposite2DeviceAS:  Master, Root is nonlocal");

            // I am the master task, but root is on a different node
            // I will recieve from the global multicast and forward
            // to the local multicast
            // Do not explicitly participate in the multicast because it is active
            if (numMasters > 1)
            {
              //_minfo_g.client             = NULL;              // Not used by device
              //_minfo_g.context            = NULL;              // Not used by device
              _minfo_g.cb_done.clientdata = this;
              _minfo_g.connection_id      = _geometry->comm();
              _minfo_g.roles              = -1U;
              _minfo_g.bytes              = bytes;
              _minfo_g.src                = NULL;
              _minfo_g.src_participants   = (pami_topology_t*) & _root_topo;
              _minfo_g.dst                = (pami_pipeworkqueue_t*) & _pwq0;  // <--- src buffer
              _minfo_g.dst_participants   = (pami_topology_t*)t_master;
              _minfo_g.msginfo            = 0;
              _minfo_g.msgcount           = 0;
              _count ++;
              TRACE_STRING( "MultiCastComposite2Device:  Global NI, start multicast");
              PAMI_assert (native_g != NULL);
              native_g->multicast(&_minfo_g, _deviceInfo);
            }

            if (numLocal > 1)
            {
              //minfo_l.client             = NULL;              // Not used by device
              //minfo_l.context            = NULL;              // Not used by device
              minfo_l.cb_done.clientdata = this;
              minfo_l.connection_id      = _geometry->comm();
              minfo_l.roles              = -1U;
              minfo_l.bytes              = bytes;
              minfo_l.src                = (pami_pipeworkqueue_t*) & _pwq0;
              minfo_l.src_participants   = (pami_topology_t*)t_my_master;
              minfo_l.dst                = NULL;
              minfo_l.dst_participants   = (pami_topology_t*)t_local;
              minfo_l.msginfo            = 0;
              minfo_l.msgcount           = 0;
              TRACE_STRING( "MultiCastComposite2Device:  Local NI, start multicast");
              PAMI_assert (native_l != NULL);
              native_l->multicast(&minfo_l, _deviceInfo);
            }
          }
          else
          {
            // I am the final case  I will only receive from the master
            // task on my node
            TRACE_FORMAT( "MultiCastComposite2DeviceAS:  Non-master, Non-root, numLocal=%zu", numLocal);

            if (numLocal > 1)
            {
              //minfo_l.client             = NULL;              // Not used by device
              //minfo_l.context            = NULL;              // Not used by device
              minfo_l.cb_done.clientdata = this;
              minfo_l.connection_id      = _geometry->comm();
              minfo_l.roles              = -1U;
              minfo_l.bytes              = bytes;
              minfo_l.src                = NULL;
              minfo_l.src_participants   = (pami_topology_t*)t_my_master;
              minfo_l.dst                = (pami_pipeworkqueue_t*) & _pwq0;
              minfo_l.dst_participants   = (pami_topology_t*)t_local;
              minfo_l.msginfo            = 0;
              minfo_l.msgcount           = 0;
              TRACE_STRING( "MultiCastComposite2Device:  Local NI, start multicast");
              PAMI_assert (native_l != NULL);
              native_l->multicast(&minfo_l, _deviceInfo);
            }

            TRACE_FORMAT( "MultiCastComposite2DeviceAS:  Non-master, Non-root, numLocal=%zu, src %p, dst %p", numLocal, minfo_l.src_participants, minfo_l.dst_participants);
          }
          TRACE_FN_EXIT();
        }

        virtual void start()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "%p",this);
          TRACE_FN_EXIT();
        }

      public:
        PAMI_GEOMETRY_CLASS               * _geometry;
        PAMI::PipeWorkQueue                 _pwq0;
        PAMI::PipeWorkQueue                 _pwq1;
        pami_endpoint_t                     _root_ep;
        pami_endpoint_t                     _justme_ep;
        PAMI::Topology                      _root_topo;
        PAMI::Topology                      _justme_topo;
        pami_callback_t                     _master_done;
        size_t                              _count;
        Interfaces::NativeInterface       * _native_g;
        pami_multicast_t                    _minfo_g;
        void                              * _deviceInfo;
      };


      class PWQBuffer : public PAMI::Queue::Element
      {
      public:
        PWQBuffer(int total_len):
        _target_pwq(&_ue_pwq),
        _complete(false),
        _total_len(total_len)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "PWQBuffer total len %zu", _total_len);
          TRACE_FN_EXIT();
        }

        inline void pwqCopy(PAMI::PipeWorkQueue *dst, PAMI::PipeWorkQueue *src)
        {
          (void)dst;(void)src;
          PAMI_abort();
        }

        inline void executeCAPost()
        {
        }

        inline void executeCAComplete()
        {
          TRACE_FN_ENTER();
          TRACE_STRING( "executeCAComplete:  delivering callback");
          _user_callback(NULL, _user_cookie, PAMI_SUCCESS);
          TRACE_FN_EXIT();
        }

        PAMI::PipeWorkQueue  _ue_pwq;
        PAMI::PipeWorkQueue *_target_pwq;
        pami_event_function  _user_callback;
        void                *_user_cookie;
        bool                 _complete;
        size_t               _total_len;
      };

      /// A composite for a 2 device multicast
      ///
      // The T_Sync boolean forces an msync before the global multicast.  This is for
      // devices that do not support buffering unexpected messages.
      //  \todo BGQ/MU will use this until MU broadcast 'features' are resolved (Issue 1714)
      //
      // The T_Allsided boolean indicates the global multicast is all-sided.
      //
      template < class T_Geometry, bool T_Allsided = false, bool T_Sync = false >
      class MultiCastComposite2Device : public CCMI::Executor::Composite
      {
      public:

        static void composite_done(pami_context_t  context,
                                   void           *cookie,
                                   pami_result_t   result)
        {
          TRACE_FN_ENTER();
          MultiCastComposite2Device *m = (MultiCastComposite2Device*) cookie;
          m->_count--;
          TRACE_FORMAT( "MultiCastComposite2Device:  composite done:  count=%zd", m->_count);

          if (m->_count == 0)
            m->_master_done.function(context, m->_master_done.clientdata, result);
          TRACE_FN_EXIT();
        }


        ~MultiCastComposite2Device()
        {
        }
        MultiCastComposite2Device (Interfaces::NativeInterface                         *native_l,
                                   Interfaces::NativeInterface                         *native_g,
                                   ConnectionManager::SimpleConnMgr                    *cmgr,
                                   pami_geometry_t                                      g,
                                   pami_xfer_t                                         *cmd,
                                   pami_event_function                                  fn,
                                   void                                                *cookie,
                                   PAMI::Queue                                         *ue,
                                   PAMI::Queue                                         *posted) :
        Composite(),
        _native_l(native_l),
        _native_g(native_g),
        _geometry((PAMI_GEOMETRY_CLASS*)g),
        _deviceInfo(NULL),
        _root_ep(cmd->cmd.xfer_broadcast.root),
        _justme_ep(native_l->endpoint()),
        _root_topo(&_root_ep,1,PAMI::tag_eplist()),
        _justme_topo(&_justme_ep,1,PAMI::tag_eplist()),
        _pwqBuf(0),
        _activePwqBuf(NULL)
        {
          TRACE_FN_ENTER();
          _active_native[0] = NULL;
          _active_native[1] = NULL;
          PAMI::Topology  *t_master    = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);
          PAMI::Topology  *t_local     = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
          PAMI::Topology  *t_my_master = (PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::LOCAL_MASTER_TOPOLOGY_INDEX);

          // Discover the root node and intereesting topology information
          size_t           root        = cmd->cmd.xfer_broadcast.root;
          bool             amRoot      = (root == native_l->endpoint());
          bool             amMaster    = t_my_master->isEndpointMember(native_l->endpoint());
          bool             isRootLocal = t_local->isEndpointMember(root);
          _deviceInfo                  = _geometry->getKey(native_l->contextid(),PAMI::Geometry::CKEY_MCAST_CLASSROUTEID);
          PAMI::Type::TypeCode *tc     = (PAMI::Type::TypeCode*)cmd->cmd.xfer_broadcast.type;
          size_t           bytes       = cmd->cmd.xfer_broadcast.typecount * tc->GetDataSize();
          size_t           stride      = cmd->cmd.xfer_broadcast.typecount * tc->GetExtent();
          size_t           numMasters  = t_master->size();
          size_t           numLocal    = t_local->size();
          TRACE_FORMAT( "MultiCastComposite2Device:  In Composite Constructor, setting up PWQ's %p %p, bytes=%zd buf=%p",
                         &_pwq0, &_pwq1, bytes, cmd->cmd.xfer_broadcast.buf);

          DO_DEBUG(unsigned j = 0;)
          DO_DEBUG(for ( j = 0; j < numMasters; ++j) fprintf(stderr, "MultiCastComposite2Device() <%p>localMasterTopo[%u]=%zu, size %zu\n", t_master, j, (size_t)t_master->index2Endpoint(j), numMasters));

          DO_DEBUG(for ( j = 0; j < numLocal; ++j) fprintf(stderr, "MultiCastComposite2Device() <%p>localTopo[%u]=%zu, size %zu\n", t_local, j, (size_t)t_local->index2Endpoint(j), numLocal));

          DO_DEBUG(for ( j = 0; j < t_my_master->size(); ++j) fprintf(stderr, "MultiCastComposite2Device() <%p>myMasterTopo[%u]=%zu, size %zu\n", t_my_master, j, (size_t)t_my_master->index2Endpoint(j), t_my_master->size()));

          if (bytes == 0)
          {
            fn(this->_context,  cookie, PAMI_SUCCESS); /// \todo, deliver the context
            TRACE_FN_EXIT();
            return;
          }

          size_t           initBytes;

          if (amRoot)
            initBytes = stride;
          else
            initBytes = 0;

          // Create a "flat pwq" for the send buffer
          _pwq0.configure(
                         cmd->cmd.xfer_broadcast.buf,     // buffer
                         stride,                          // buffer bytes
                         initBytes,                       // amount initially in buffer
                         tc,
                         tc);

          _pwq1.configure(
                         cmd->cmd.xfer_broadcast.buf,     // buffer
                         stride,                          // buffer bytes
                         initBytes,                       // amount initially in buffer
                         tc,
                         tc);

          int cb_count = 0;

          if (amRoot && amMaster)
          {
            // Participate in local and global multicast as the source
            TRACE_STRING( "MultiCastComposite2Device:  Root/Master");
            

            if (numLocal > 1)
            {
              //_minfo_l.client             = NULL;              // Not used by device
              //_minfo_l.context            = NULL;              // Not used by device
              _minfo_l.cb_done.clientdata = this;
              _minfo_l.connection_id      = _geometry->comm();
              _minfo_l.roles              = -1U;
              _minfo_l.bytes              = bytes;
              _minfo_l.src                = (pami_pipeworkqueue_t*) & _pwq0;
              _minfo_l.src_participants   = (pami_topology_t*) & _root_topo;
              _minfo_l.dst                = NULL;
              _minfo_l.dst_participants   = (pami_topology_t*)t_local;
              _minfo_l.msginfo            = 0;
              _minfo_l.msgcount           = 0;
              _active_minfo[0]            = &_minfo_l;
              _active_native[0]           = _native_l;
              cb_count++;
            }

            if (numMasters > 1)
            {
              //_minfo_g.client             = NULL;              // Not used by device
              //_minfo_g.context            = NULL;              // Not used by device
              _minfo_g.cb_done.clientdata = this;
              _minfo_g.connection_id      = _geometry->comm();
              _minfo_g.roles              = -1U;
              _minfo_g.bytes              = bytes;
              _minfo_g.src                = (pami_pipeworkqueue_t*) & _pwq1;
              _minfo_g.src_participants   = (pami_topology_t*) & _root_topo;
              _minfo_g.dst                = NULL;
              _minfo_g.dst_participants   = (pami_topology_t*)t_master;
              _minfo_g.msginfo            = 0;
              _minfo_g.msgcount           = 0;

              _active_minfo[1]            = &_minfo_g;
              _active_native[1]           = _native_g;
              cb_count++;
            }
          }
          else if (amRoot)
          {
            TRACE_STRING( "MultiCastComposite2Device:  Root only");

            // I am a root, but not the master, participate in
            // the local broadcast only, as the source
            if (numLocal > 1)
            {
              //_minfo_l.client             = NULL;              // Not used by device
              //_minfo_l.context            = NULL;              // Not used by device
              _minfo_l.cb_done.clientdata = this;
              _minfo_l.connection_id      = _geometry->comm();
              _minfo_l.roles              = -1U;
              _minfo_l.bytes              = bytes;
              _minfo_l.src                = (pami_pipeworkqueue_t*) & _pwq0;
              _minfo_l.src_participants   = (pami_topology_t*) & _root_topo;
              _minfo_l.dst                = NULL;
              _minfo_l.dst_participants   = (pami_topology_t*)t_local;
              _minfo_l.msginfo            = 0;
              _minfo_l.msgcount           = 0;
              _active_minfo[0]            = &_minfo_l;
              _active_native[0]           = _native_l;
              cb_count++;
            }
          }
          else if (amMaster && isRootLocal)
          {
            TRACE_STRING( "MultiCastComposite2Device:  Master, Root is Local");

            // I am the master task, and on the same node as the root
            // so I will participate in the local broadcast (as a receiver)
            // and I will participate in the global broadcast as a sender
            if (numLocal > 1)
            {
              //_minfo_l.client             = NULL;              // Not used by device
              //_minfo_l.context            = NULL;              // Not used by device
              _minfo_l.cb_done.clientdata = this;
              _minfo_l.connection_id      = _geometry->comm();
              _minfo_l.roles              = -1U;
              _minfo_l.bytes              = bytes;
              _minfo_l.src                = NULL;
              _minfo_l.src_participants   = (pami_topology_t*) & _root_topo;
              _minfo_l.dst                = (pami_pipeworkqueue_t*) & _pwq0;  // <--- target buffer
              _minfo_l.dst_participants   = (pami_topology_t*)t_local;
              _minfo_l.msginfo            = 0;
              _minfo_l.msgcount           = 0;

              _active_minfo[0]            = &_minfo_l;
              _active_native[0]           = _native_l;
              cb_count++;
            }

            if (numMasters > 1)
            {
              //_minfo_g.client             = NULL;              // Not used by device
              //_minfo_g.context            = NULL;              // Not used by device
              _minfo_g.cb_done.clientdata = this;
              _minfo_g.connection_id      = _geometry->comm();
              _minfo_g.roles              = -1U;
              _minfo_g.bytes              = bytes;
              _minfo_g.src                = (pami_pipeworkqueue_t*) & _pwq0;  // <--- src buffer
              _minfo_g.src_participants   = (pami_topology_t*) & _justme_topo;
              _minfo_g.dst                = NULL;
              _minfo_g.dst_participants   = (pami_topology_t*)t_master;
              _minfo_g.msginfo            = 0;
              _minfo_g.msgcount           = 0;

              _active_minfo[1]            = &_minfo_g;
              _active_native[1]           = _native_g;
              cb_count++;
            }
          }
          else if (amMaster)
          {
            TRACE_STRING( "MultiCastComposite2Device:  Master, Root is nonlocal");

            // I am the master task, but root is on a different node
            // I will recieve from the global multicast and forward
            // to the local multicast
            // If not allsided, do not explicitly participate in the multicast because it is active
            if (numLocal > 1)
            {
              //_minfo_l.client             = NULL;              // Not used by device
              //_minfo_l.context            = NULL;              // Not used by device
              _minfo_l.cb_done.clientdata = this;
              _minfo_l.connection_id      = _geometry->comm();
              _minfo_l.roles              = -1U;
              _minfo_l.bytes              = bytes;
              _minfo_l.src                = (pami_pipeworkqueue_t*) & _pwq0;
              _minfo_l.src_participants   = (pami_topology_t*)t_my_master;
              _minfo_l.dst                = NULL;
              _minfo_l.dst_participants   = (pami_topology_t*)t_local;
              _minfo_l.msginfo            = 0;
              _minfo_l.msgcount           = 0;
              _active_minfo[0]            = &_minfo_l;
              _active_native[0]           = _native_l;
              cb_count++;
            }

            if (T_Allsided)
            {
              // Allsided so participate in the global multicast and receive into target buffer
              // for the local multicast to use.
              if (numMasters > 1)
              {
                //_minfo_g.client             = NULL;              // Not used by device
                //_minfo_g.context            = NULL;              // Not used by device
                _minfo_g.cb_done.clientdata = this;
                _minfo_g.connection_id      = _geometry->comm();
                _minfo_g.roles              = -1U;
                _minfo_g.bytes              = bytes;
                _minfo_g.src                = NULL;
                _minfo_g.src_participants   = (pami_topology_t*) & _root_topo;
                _minfo_g.dst                = (pami_pipeworkqueue_t*) & _pwq0;  // <--- target buffer
                _minfo_g.dst_participants   = (pami_topology_t*)t_master;
                _minfo_g.msginfo            = 0;
                _minfo_g.msgcount           = 0;

                _active_minfo[1]            = &_minfo_g;
                _active_native[1]           = _native_g;
                cb_count++;
              }
            }
            else
            {
              // This node will be receiving data from the master via an active message
              // This means that I need to post this into a queue or find one that is UE
              // 1)  Find in the UE queue
              // 2)  If found, copy the data and complete the message
              PWQBuffer *pwqBuf = (PWQBuffer*) ue->dequeue();

              if (pwqBuf)
              {
                TRACE_FORMAT( "MultiCastComposite2Device:  Found in UE queue queue:  target_pwq=%p",
                               &pwqBuf->_ue_pwq);
                pwqBuf->_target_pwq           = &pwqBuf->_ue_pwq;
                pwqBuf->_ue_pwq.configure(cmd->cmd.xfer_broadcast.buf, stride, 0, tc, tc);
                _minfo_l.src                = (pami_pipeworkqueue_t*) & pwqBuf->_ue_pwq;
                _activePwqBuf                 = pwqBuf;

                cb_count++;
                _activePwqBuf->_user_callback = composite_done;
                _activePwqBuf->_user_cookie   = this;
              }
              else
              {
                // No UE message
                // The completion action is to complete the message
                // and deliver the callback
                TRACE_FORMAT( "MultiCastComposite2Device:  Posting to posted queue:  _pwqBuf=%p",
                               &_pwqBuf);
                posted->enqueue((PAMI::Queue::Element*)&_pwqBuf);
                _pwqBuf._target_pwq           = &_pwq0;
                _activePwqBuf                 = &_pwqBuf;

                cb_count++;
                _activePwqBuf->_user_callback = composite_done;
                _activePwqBuf->_user_cookie   = this;
              }
            }
          }
          else
          {
            // I am the final case  I will receive from the root
            // task when I am on the same node as root; and I will receive from
            // my master when I am not on the same node as root.
            TRACE_FORMAT( "MultiCastComposite2Device:  Non-master, Non-root, numLocal=%zu", numLocal);

            if (numLocal > 1)
            {
              //_minfo_l.client             = NULL;              // Not used by device
              //_minfo_l.context            = NULL;              // Not used by device
              _minfo_l.cb_done.clientdata = this;
              _minfo_l.connection_id      = _geometry->comm();
              _minfo_l.roles              = -1U;
              _minfo_l.bytes              = bytes;
              _minfo_l.src                = NULL;
              _minfo_l.src_participants   = (pami_topology_t*) (isRootLocal ?  &_root_topo : t_my_master);
              _minfo_l.dst                = (pami_pipeworkqueue_t*) & _pwq0;
              _minfo_l.dst_participants   = (pami_topology_t*)t_local;
              _minfo_l.msginfo            = 0;
              _minfo_l.msgcount           = 0;
              _active_minfo[0]            = &_minfo_l;
              _active_native[0]           = _native_l;
              cb_count++;
            }

            TRACE_FORMAT( "MultiCastComposite2Device:  Non-master, Non-root, numLocal=%zu, src %p, dst %p", numLocal, _minfo_l.src_participants, _minfo_l.dst_participants);
          }

          _minfo_g.cb_done.function   = composite_done;
          _minfo_l.cb_done.function   = composite_done;
          _count                      = cb_count;

          _master_done.function       = fn;
          _master_done.clientdata     = cookie;

          if (T_Sync)
          {
            _deviceMsyncInfo          = _geometry->getKey(native_l->contextid(),PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID);
            // Initialize the msync
            _msync.cb_done.function   = cb_msync_done;
            _msync.cb_done.clientdata = this;
            _msync.connection_id      = 0; /// \todo ?
            _msync.roles              = -1U;
            _msync.participants       = amMaster && (t_master->size() > 1) ? (pami_topology_t *) t_master : (pami_topology_t *) NULL;
          }

          TRACE_FN_EXIT();
        }
        static void cb_msync_done(pami_context_t context, void *me, pami_result_t err)
        {
          TRACE_FN_ENTER();
          TRACE_STRING( "MultiCastComposite2Device::cb_msync_done()");
          MultiCastComposite2Device *m = (MultiCastComposite2Device*) me;
          CCMI_assert (m != NULL);

          // Msync is done, start the active message mcast on the root
          m->startMcast();
          TRACE_FN_EXIT();
        }

        virtual void start()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "%p",this);

          if (T_Sync)
          {
            if (_msync.participants)
            {
              _native_g->multisync(&_msync, _deviceMsyncInfo);
              TRACE_FN_EXIT();
              return;
            }
          }

          startMcast();
          TRACE_FN_EXIT();
        }
        void startMcast()
        {
          TRACE_FN_ENTER();
          TRACE_STRING( "MultiCastComposite2Device:  Multicast start()");

          if (_active_native[0])
          {
            TRACE_STRING( "MultiCastComposite2Device:  Local NI, start multicast");
            _active_native[0]->multicast(_active_minfo[0], _deviceInfo);
          }

          if (_active_native[1])
          {
            TRACE_STRING( "MultiCastComposite2Device:  Global NI, start multicast");
            _active_native[1]->multicast(_active_minfo[1], _deviceInfo);
          }

          if (_activePwqBuf)
          {
            _activePwqBuf->executeCAPost();
          }
          TRACE_FN_EXIT();
        }
      public:
        Interfaces::NativeInterface        *_native_l;
        Interfaces::NativeInterface        *_native_g;
        Interfaces::NativeInterface        *_active_native[2];
        pami_multicast_t                   *_active_minfo[2];
        PAMI_GEOMETRY_CLASS                *_geometry;
        void                               *_deviceInfo;
        PAMI::Topology                     *_l_topology;
        PAMI::Topology                     *_g_topology;
        pami_multicast_t                    _minfo_l;
        pami_multicast_t                    _minfo_g;
        PAMI::PipeWorkQueue                 _pwq0;
        PAMI::PipeWorkQueue                 _pwq1;
        pami_endpoint_t                     _root_ep;
        pami_endpoint_t                     _justme_ep;
        PAMI::Topology                      _root_topo;
        PAMI::Topology                      _justme_topo;
        pami_callback_t                     _master_done;
        size_t                              _count;
        PWQBuffer                           _pwqBuf;
        PWQBuffer                          *_activePwqBuf;
        void                               * _deviceMsyncInfo;
        pami_multisync_t                     _msync;
      };


      /// \brief This is a factory for a 2 device multicast
      /// The two device multicast takes two native interfaces, the first is
      /// the local interface, and the second is the global interface
      template < class T_Composite, MetaDataFn get_metadata, class T_Connmgr, int LookupNI = 0 >
      class MultiCastComposite2DeviceFactoryT: public CollectiveProtocolFactory
      {
        class collObj
        {
        public:
          collObj(Interfaces::NativeInterface       *native0,
                  Interfaces::NativeInterface       *native1,
                  T_Connmgr                         *cmgr,
                  pami_geometry_t                    geometry,
                  pami_xfer_t                       *cmd,
                  pami_event_function                fn,
                  void                              *cookie,
                  MultiCastComposite2DeviceFactoryT *factory,
                  PAMI::Queue                       *ue,
                  PAMI::Queue                       *posted):
          _factory(factory),
          _user_done_fn(cmd->cb_done),
          _user_cookie(cmd->cookie),
          _obj(native0, native1, cmgr, geometry, cmd, fn, cookie, ue, posted)
          {
          }
          void done_fn( pami_context_t   context,
                        pami_result_t    result )
          {
            TRACE_FN_ENTER();
            TRACE_STRING( "MultiCastComposite2DeviceFactoryT::done_fn()");
            _user_done_fn(context, _user_cookie, result);
            TRACE_FN_EXIT();
          }

          MultiCastComposite2DeviceFactoryT *_factory;
          pami_event_function                _user_done_fn;
          void                              *_user_cookie;
          T_Composite                        _obj;
          unsigned                           _connection_id;
        };


        static void cb_async_done(pami_context_t context, void *cookie, pami_result_t err)
        {
          TRACE_FN_ENTER();
          TRACE_STRING( "MultiCastComposite2DeviceFactoryT: cb_async_done");
          PWQBuffer* pbuf = (PWQBuffer*) cookie;
          pbuf->executeCAComplete();
          TRACE_FN_EXIT();
        }

        static void cb_async_g(pami_context_t          ctxt,
                               const pami_quad_t     * info,
                               unsigned                count,
                               unsigned                conn_id,
                               size_t                  peer,
                               size_t                  sndlen,
                               void                  * arg,
                               size_t                * rcvlen,
                               pami_pipeworkqueue_t ** rcvpwq,
                               pami_callback_t       * cb_done)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "MultiCastComposite2DeviceFactoryT: cb_async_g:  arg=%p", arg);
          MultiCastComposite2DeviceFactoryT *f = (MultiCastComposite2DeviceFactoryT *) arg;
          PWQBuffer* pbuf = (PWQBuffer*) f->_posted.dequeue();

          if (pbuf)
          {
            // A message has been posted, and the PWQ has been set up
            // So we can receive into this existing target PWQ
            TRACE_FORMAT( "MultiCastComposite2DeviceFactoryT: cb_async_g, posted buffer, sndlen=%zd, pwq=%p, bytesAvailableToProduce %zu",
                           sndlen, pbuf->_target_pwq, pbuf->_target_pwq->bytesAvailableToProduce());
          }
          else
          {
            // A message has not been been posted, so we set up a temporary pwq
            // That will be filled in with the target PWQ when the user calls in
            // to the collective and specifies it.  We'll reconfigure the pwq when
            // the user gets around to posting
            TRACE_STRING( "MultiCastComposite2DeviceFactoryT: cb_async_g, unexpected buffer");
            pbuf = f->allocatePbuf(sndlen);
            pbuf->_ue_pwq.configure((char *)NULL, 0, 0);
            f->_ue.enqueue((PAMI::Queue::Element*)pbuf);
          }

          *rcvlen = sndlen;
          *rcvpwq = (pami_pipeworkqueue_t*)pbuf->_target_pwq;
          cb_done->function   = cb_async_done;
          cb_done->clientdata = pbuf;
          TRACE_FN_EXIT();
        }

        static void cb_async_l(pami_context_t          ctxt,
                               const pami_quad_t     * info,
                               unsigned                count,
                               unsigned                conn_id,
                               size_t                  peer,
                               size_t                  sndlen,
                               void                  * arg,
                               size_t                * rcvlen,
                               pami_pipeworkqueue_t ** rcvpwq,
                               pami_callback_t       * cb_done)
        {
          /// we don't support active message local yet
          PAMI_abortf("Local async callback is unimpl");
        }

      public:
        MultiCastComposite2DeviceFactoryT (pami_context_t               ctxt,
                                           size_t                       ctxt_id,
                                           pami_mapidtogeometry_fn      cb_geometry,
                                           T_Connmgr                   *cmgr,
                                           Interfaces::NativeInterface *native_l,
                                           bool                         active_message_l,
                                           Interfaces::NativeInterface *native_g,
                                           bool                         active_message_g):
        CollectiveProtocolFactory(ctxt,ctxt_id,cb_geometry),
        _cmgr(cmgr),
        _native_l(native_l),
        _native_g(native_g)
        {
          if (active_message_g)
            native_g->setMulticastDispatch(cb_async_g, this);

          if (active_message_l)
            native_l->setMulticastDispatch(cb_async_l, this);

        }

        virtual ~MultiCastComposite2DeviceFactoryT()
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
          TRACE_FN_ENTER();
          TRACE_STRING( "MultiCastComposite2DeviceFactoryT::done_fn");
          collObj *cobj = (collObj *)clientdata;
          cobj->done_fn(context, res);
          cobj->_factory->_alloc.returnObject(cobj);
          TRACE_FN_EXIT();
        }

        PWQBuffer * allocatePbuf(size_t sndlen)
        {
          TRACE_FN_ENTER();
          PWQBuffer *pbuf = (PWQBuffer*) _alloc_pbuf.allocateObject();
          TRACE_STRING( "MultiCastComposite2DeviceFactoryT:: Allocating pbuf");
          new(pbuf)PWQBuffer(sndlen);
          TRACE_FN_EXIT();
          return pbuf;
        }


        virtual Executor::Composite * generate(pami_geometry_t  geometry,
                                               void            *cmd)
        {
          TRACE_FN_ENTER();
          // This should compile out if native interfaces are scoped
          // globally.
          if (LookupNI)
          {
            PAMI_GEOMETRY_CLASS *g = (PAMI_GEOMETRY_CLASS*)geometry;
            _native_l = _ni_local_map[g->comm()];
            _native_g = _ni_global_map[g->comm()];
          }

          collObj *cobj = (collObj*) _alloc.allocateObject();
          TRACE_FORMAT( "<%p>MultiCastComposite2DeviceFactoryT::generate()", cobj);
          new(cobj) collObj(_native_l?_native_l:_native_g,// Native interface local (use global is local is null)
                            _native_g,          // Native Interface global
                            _cmgr,              // Connection Manager
                            geometry,           // Geometry Object
                            (pami_xfer_t*)cmd,  // Parameters
                            done_fn,            // Intercept function
                            cobj,               // Intercept cookie
                            this,
                            &_ue,
                            &_posted);              // Factory
          TRACE_FN_EXIT();
          return(Executor::Composite *)&cobj->_obj;

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
        PAMI::MemoryAllocator < sizeof(collObj), 16 >    _alloc;
        PAMI::MemoryAllocator < sizeof(PWQBuffer), 16 >  _alloc_pbuf;
        PAMI::Queue                                      _ue;
        PAMI::Queue                                      _posted;
        std::map<size_t, Interfaces::NativeInterface *>   _ni_local_map;
        std::map<size_t, Interfaces::NativeInterface *>   _ni_global_map;
      };





      ///
      /// \brief An all-sided multicast composite built on an all-sided
      /// multicombine ('binary or' operation).
      ///
      class MultiCastComposite3 : public CCMI::Executor::Composite
      {
      protected:
        Interfaces::NativeInterface        * _native;
        PAMI_GEOMETRY_CLASS                * _geometry;
        pami_broadcast_t                     _xfer_broadcast;
        PAMI::Topology                       _all;
        PAMI::PipeWorkQueue                  _data;
        PAMI::PipeWorkQueue                  _results;
        pami_multicombine_t                  _minfo;
        size_t                               _bytes;
        size_t                               _buffer_size;
        char                               * _buffer;
        void                               *_deviceInfo;

      public:
        ~MultiCastComposite3()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>", this);
          _buffer_size = 0;
          free(_buffer);
          TRACE_FN_EXIT();
        }
        MultiCastComposite3 (Interfaces::NativeInterface          * mInterface,
                             ConnectionManager::SimpleConnMgr     * cmgr,
                             pami_geometry_t                        g,
                             pami_xfer_t                          * cmd,
                             pami_event_function                    fn,
                             void                                 * cookie) :
        Composite(), _native(mInterface), _geometry((PAMI_GEOMETRY_CLASS*)g),
        _xfer_broadcast(cmd->cmd.xfer_broadcast),
        _buffer_size(0),
        _buffer(NULL)
        {
          (void)cmgr;(void)fn;(void)cookie;
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p> type %#zX, count %zu, root %zu", this,  (size_t)cmd->cmd.xfer_broadcast.type, cmd->cmd.xfer_broadcast.typecount, (size_t)cmd->cmd.xfer_broadcast.root);
          PAMI::Type::TypeCode * type_obj = (PAMI::Type::TypeCode *)cmd->cmd.xfer_broadcast.type;

          /// \todo Support non-contiguous
          //assert(type_obj->IsContiguous() &&  type_obj->IsPrimitive());

          unsigned        sizeOfType = type_obj->GetDataSize();
          _bytes        = cmd->cmd.xfer_broadcast.typecount * sizeOfType;
          //SSS: This is probably not needed since type is CHAR but we will modify here for consistency
          size_t stride = cmd->cmd.xfer_broadcast.typecount * type_obj->GetExtent();

          _deviceInfo                  = _geometry->getKey(mInterface->contextid(),PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID);

          _all = *(PAMI::Topology*)_geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);

          DO_DEBUG(unsigned j = 0;)
          DO_DEBUG(for (j = 0; j < _all.size(); ++j) fprintf(stderr, "all[%u]=%zu, size %zu\n", j, (size_t)_all.index2Endpoint(j), _all.size()));

          if (cmd->cmd.xfer_broadcast.root == mInterface->endpoint())
          {
            _buffer_size = stride;
            _buffer = (char*) malloc(_buffer_size);
            _data.configure(cmd->cmd.xfer_broadcast.buf, stride, stride, type_obj, type_obj);
            _results.configure(_buffer, stride, 0, type_obj, type_obj);
          }
          else
          {
            /// \todo consume/produce from one buffer and avoid the temp _buffer -does this work everywhere?
            memset(cmd->cmd.xfer_broadcast.buf,  0x00,  stride);
            _data.configure(cmd->cmd.xfer_broadcast.buf, stride, stride, type_obj, type_obj);
            _results.configure(cmd->cmd.xfer_broadcast.buf, stride, 0, type_obj, type_obj);
          }

          //_minfo.cb_done.function   = _cb_done;
          //_minfo.cb_done.clientdata = _clientdata;
          _minfo.connection_id      = 0; /// \todo ?
          _minfo.roles              = -1U;
          _minfo.results_participants   = (pami_topology_t *) & _all;  /// \todo not the root?
          _minfo.data_participants  = (pami_topology_t *) & _all;
          _minfo.data               = (pami_pipeworkqueue_t *) & _data;
          _minfo.results            = (pami_pipeworkqueue_t *) & _results;
          _minfo.optor              = PAMI_BOR;
          _minfo.dtype              = PAMI_UNSIGNED_CHAR;
          _minfo.count              = _bytes;
          TRACE_FN_EXIT();
        }

        virtual void start()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>", this);
          _minfo.cb_done.function   = _cb_done;
          _minfo.cb_done.clientdata = _clientdata;
          _native->multicombine(&_minfo, _deviceInfo);
          TRACE_FN_EXIT();
        }

        virtual unsigned restart (void *pcmd)
        {
          TRACE_FN_ENTER();
          pami_xfer_t *cmd = (pami_xfer_t *)pcmd;

          _xfer_broadcast = cmd->cmd.xfer_broadcast;
          TRACE_FORMAT( "<%p> type %#zX, count %zu, root %zu", this,  (size_t)cmd->cmd.xfer_broadcast.type, cmd->cmd.xfer_broadcast.typecount,(size_t)cmd->cmd.xfer_broadcast.root);

          PAMI::Type::TypeCode * type_obj = (PAMI::Type::TypeCode *)cmd->cmd.xfer_broadcast.type;

          /// \todo Support non-contiguous
          //assert(type_obj->IsContiguous() &&  type_obj->IsPrimitive());

          unsigned        sizeOfType = type_obj->GetDataSize();
          _bytes        = cmd->cmd.xfer_broadcast.typecount * sizeOfType;
          size_t stride = cmd->cmd.xfer_broadcast.typecount * type_obj->GetExtent();

          if (_buffer_size < stride)
          {
            free(_buffer);
            _buffer_size = stride;
            _buffer = (char*) malloc(_buffer_size);
          }

          if (cmd->cmd.xfer_broadcast.root == _native->endpoint())
          {
            _data.configure(cmd->cmd.xfer_broadcast.buf, stride, stride, type_obj, type_obj);
            _results.configure(_buffer, stride, 0, type_obj, type_obj);
          }
          else
          {
            /// \todo would be nice to consume/produce from one buffer and avoid the temp _buffer
            memset(_buffer,  0x00,  stride);
            _data.configure(_buffer, stride, stride, type_obj, type_obj);
            _results.configure(cmd->cmd.xfer_broadcast.buf, stride, 0, type_obj, type_obj);
          }

          //_minfo.cb_done.function   = _cb_done;
          //_minfo.cb_done.clientdata = _clientdata;
          _minfo.connection_id      = 0; /// \todo ?
          _minfo.roles              = -1U;
          _minfo.results_participants   = (pami_topology_t *) & _all; /// \todo not the root?
          _minfo.data_participants  = (pami_topology_t *) & _all;
          _minfo.data               = (pami_pipeworkqueue_t *) & _data;
          _minfo.results            = (pami_pipeworkqueue_t *) & _results;
          _minfo.optor              = PAMI_BOR;
          _minfo.dtype              = PAMI_UNSIGNED_CHAR;
          _minfo.count              = _bytes;
          TRACE_FN_EXIT();
          return 0;
        };

      };


    };
  };
};

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
