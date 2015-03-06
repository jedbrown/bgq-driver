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
 * \file algorithms/geometry/P2PCCMIRegInfo.h
 * \brief ???
 */

#ifndef __algorithms_geometry_P2PCCMIRegInfo_h__
#define __algorithms_geometry_P2PCCMIRegInfo_h__

#include <map>
#include <vector>
#include "util/ccmi_debug.h"
#include "TypeDefs.h"
#include "algorithms/geometry/Metadata.h"
#include "algorithms/interfaces/CollRegistrationInterface.h"
#include "algorithms/schedule/MultinomialTree.h"
#include "algorithms/schedule/RingSchedule.h"
#include "algorithms/schedule/GenericTreeT.h"
#include "algorithms/connmgr/ColorGeometryConnMgr.h"
#include "algorithms/connmgr/ColorConnMgr.h"
#include "algorithms/connmgr/CommSeqConnMgr.h"
#include "algorithms/connmgr/RankSeqConnMgr.h"
#include "algorithms/protocols/CollectiveProtocolFactoryT.h"
#include "algorithms/protocols/broadcast/BcastMultiColorCompositeT.h"
#include "algorithms/protocols/barrier/BarrierT.h"
#include "algorithms/protocols/broadcast/AsyncBroadcastT.h"
#include "algorithms/protocols/amcollectives/AMBroadcastT.h"
#include "algorithms/protocols/allreduce/MultiColorCompositeT.h"
#include "algorithms/protocols/allreduce/AsyncOATCompositeT.h"
#include "algorithms/protocols/allreduce/ProtocolFactoryT.h"
#include "algorithms/protocols/allreduce/AsyncAllreduceT.h"
#include "algorithms/protocols/amcollectives/AMReduceT.h"
#include "algorithms/protocols/allreduce/AsyncReduceScatterT.h"
#include "algorithms/protocols/scatter/AsyncScatterT.h"
#include "algorithms/protocols/amcollectives/AMScatterT.h"
#include "algorithms/protocols/gather/AsyncGatherT.h"
#include "algorithms/protocols/gather/AsyncLongGatherT.h"
#include "algorithms/protocols/amcollectives/AMGatherT.h"
#include "algorithms/protocols/allgather/AsyncAllgatherT.h"
#include "algorithms/protocols/allgather/AsyncAllgathervT.h"
#include "algorithms/protocols/alltoall/AsyncAlltoallvT.h"
#include "algorithms/protocols/scan/AsyncScanT.h"
#include "p2p/protocols/SendPWQ.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "algorithms/interfaces/NativeInterfaceFactory.h"
#include "algorithms/protocols/alltoall/All2All.h"
#include "algorithms/protocols/alltoall/All2Allv.h"
#include "algorithms/schedule/TorusRect.h"
#include "algorithms/protocols/onetask/OneTaskT.h"


// CCMI Template implementations for P2P
namespace CCMI
{
  namespace Adaptor
  {
    namespace P2POneTask
    {

      extern inline void onetask_barrier_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskBarrier:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_barrier_t >
      OneTaskBarrier;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskBarrier,
        onetask_barrier_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskBarrierFactory;


      //extern inline void onetask_fence_md(pami_metadata_t *m)
      //{
      //  new(m) PAMI::Geometry::Metadata("I0:OneTaskFence:OneTask:OneTask");
      //}

      //typedef CCMI::Adaptor::OneTask::OneTaskT
      //< pami_fence_t >
      //OneTaskFence;

      //typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      //< OneTaskFence,
      //  onetask_fence_md,
      //  CCMI::ConnectionManager::SimpleConnMgr >
      //OneTaskFenceFactory;


      extern inline void onetask_allreduce_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAllreduce:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_allreduce_t >
      OneTaskAllreduce;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskAllreduce,
        onetask_allreduce_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAllreduceFactory;


      extern inline void onetask_broadcast_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskBroadcast:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_broadcast_t >
      OneTaskBroadcast;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskBroadcast,
        onetask_broadcast_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskBroadcastFactory;


      extern inline void onetask_reduce_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskReduce:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_reduce_t >
      OneTaskReduce;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskReduce,
        onetask_reduce_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskReduceFactory;


      extern inline void onetask_allgather_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAllgather:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_allgather_t >
      OneTaskAllgather;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskAllgather,
        onetask_allgather_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAllgatherFactory;


      extern inline void onetask_allgatherv_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAllgatherv:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_allgatherv_t >
      OneTaskAllgatherv;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskAllgatherv,
        onetask_allgatherv_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAllgathervFactory;


      extern inline void onetask_allgatherv_int_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAllgathervInt:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_allgatherv_int_t >
      OneTaskAllgathervInt;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskAllgathervInt,
        onetask_allgatherv_int_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAllgathervIntFactory;


      extern inline void onetask_scatter_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskScatter:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_scatter_t >
      OneTaskScatter;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskScatter,
        onetask_scatter_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskScatterFactory;


      extern inline void onetask_scatterv_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskScatterv:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_scatterv_t >
      OneTaskScatterv;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskScatterv,
        onetask_scatterv_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskScattervFactory;


      extern inline void onetask_scatterv_int_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskScattervInt:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_scatterv_int_t >
      OneTaskScattervInt;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskScattervInt,
        onetask_scatterv_int_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskScattervIntFactory;


      extern inline void onetask_gather_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskGather:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_gather_t >
      OneTaskGather;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskGather,
        onetask_gather_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskGatherFactory;


      extern inline void onetask_gatherv_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskGatherv:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_gatherv_t >
      OneTaskGatherv;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskGatherv,
        onetask_gatherv_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskGathervFactory;


      extern inline void onetask_gatherv_int_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskGathervInt:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_gatherv_int_t >
      OneTaskGathervInt;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskGathervInt,
        onetask_gatherv_int_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskGathervIntFactory;


      extern inline void onetask_alltoall_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAlltoall:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_alltoall_t >
      OneTaskAlltoall;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskAlltoall,
        onetask_alltoall_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAlltoallFactory;


      extern inline void onetask_alltoallv_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAlltoallv:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_alltoallv_t >
      OneTaskAlltoallv;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskAlltoallv,
        onetask_alltoallv_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAlltoallvFactory;


      extern inline void onetask_alltoallv_int_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAlltoallvInt:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_alltoallv_int_t >
      OneTaskAlltoallvInt;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskAlltoallvInt,
        onetask_alltoallv_int_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAlltoallvIntFactory;


      extern inline void onetask_scan_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskScan:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_scan_t >
      OneTaskScan;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskScan,
        onetask_scan_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskScanFactory;


      extern inline void onetask_reduce_scatter_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskReduceScatter:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskT
      < pami_reduce_scatter_t >
      OneTaskReduceScatter;

      typedef CCMI::Adaptor::OneTask::OneTaskFactoryT
      < OneTaskReduceScatter,
        onetask_reduce_scatter_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskReduceScatterFactory;

      extern inline void onetask_ambroadcast_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAMBroadcast:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskAMFactoryT
      < pami_ambroadcast_t,
        onetask_ambroadcast_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAMBroadcastFactory;

      extern inline void onetask_amscatter_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAMScatter:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskAMFactoryT
      < pami_amscatter_t,
        onetask_amscatter_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAMScatterFactory;

      extern inline void onetask_amgather_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAMGather:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskAMFactoryT
      < pami_amgather_t,
        onetask_amgather_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAMGatherFactory;

      extern inline void onetask_amreduce_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:OneTaskAMReduce:OneTask:OneTask");
      }

      typedef CCMI::Adaptor::OneTask::OneTaskAMFactoryT
      < pami_amreduce_t,
        onetask_amreduce_md,
        CCMI::ConnectionManager::SimpleConnMgr >
      OneTaskAMReduceFactory;

    };

    namespace P2PBarrier
    {
      extern inline void binomial_barrier_md(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
      }

      extern inline bool binomial_analyze (PAMI_GEOMETRY_CLASS *geometry)
      {
        (void)geometry;
        return true;
      }

      typedef CCMI::Adaptor::Barrier::BarrierT
      < CCMI::Schedule::TopoMultinomial,
        binomial_analyze,
        PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX,
        PAMI::Geometry::CKEY_UEBARRIERCOMPOSITE1 >
      BinomialBarrier;

      typedef CCMI::Adaptor::Barrier::BarrierFactoryT
      < BinomialBarrier,
        binomial_barrier_md,
        CCMI::ConnectionManager::SimpleConnMgr,
        true,
        PAMI::Geometry::CKEY_UEBARRIERCOMPOSITE1>
      BinomialBarrierFactory;

      typedef CCMI::Adaptor::Barrier::BarrierT
        < CCMI::Schedule::TopoMultinomial4,
        binomial_analyze,
        PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX,
        PAMI::Geometry::CKEY_BARRIERCOMPOSITE8>
        BinomialBarrierKey2;

      typedef CCMI::Adaptor::Barrier::BarrierFactoryT
        < BinomialBarrierKey2,
        binomial_barrier_md,
        CCMI::ConnectionManager::SimpleConnMgr,
        true,
        PAMI::Geometry::CKEY_BARRIERCOMPOSITE8>
        BinomialBarrierFactoryKey2;      
    };//Barrier


    namespace P2PBroadcast
    {
      extern inline void get_colors (PAMI::Topology             * t,
                       unsigned                    bytes,
                       unsigned                  * colors,
                       unsigned                  & ncolors)
      {
        (void)t;(void)bytes;
        ncolors = 1;
        colors[0] = CCMI::Schedule::TorusRect::NO_COLOR;
      }

#ifdef PAMI_ENABLE_X0_PROTOCOLS // Experimental (X0:) protocols

    extern inline void get_rect_colors (PAMI::Topology             * t,
                          unsigned                    bytes,
                          unsigned                  * colors,
                          unsigned                  & ncolors)
    {

      unsigned max = 0, ideal = 0;
      unsigned _colors[PAMI_MAX_DIMS*2];
      PAMI_assert(ncolors <= (PAMI_MAX_DIMS*2));
      CCMI::Schedule::TorusRect::getColors (t, ideal, max, _colors);
      TRACE_INIT((stderr, "get_rect_colors() bytes %u, ncolors %u, ideal %u, max %u\n", bytes, ncolors, ideal, max));
      
      if (bytes <= 8192) //16 packets
        ideal = 1;
      else if (bytes <= 65536 && ideal >= 2)
        ideal = 2;
      else if (bytes <= 262144 && ideal >= 3)
        ideal = 3;
      else if (bytes <= 1048576 && ideal >= 5)
        ideal = 5;
      else if (bytes <= 4194304 && ideal >= 8)
        ideal = 8;

      if (ideal < ncolors)
        ncolors = ideal;  //Reduce the number of colors to the relavant colors
      
      TRACE_INIT((stderr, "get_rect_colors() ncolors %u, ideal %u\n", ncolors, ideal));
      memcpy (colors, _colors, ncolors * sizeof(int));
    }


      extern inline void rectangle_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("X0:Rectangle:P2P:P2P");
      }

      typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
      < 5,
        CCMI::Schedule::TorusRect,
        CCMI::ConnectionManager::ColorConnMgr,
        get_rect_colors,
        PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX>
      RectangleBroadcastComposite;

      typedef CCMI::Adaptor::CollectiveProtocolFactoryT
      < RectangleBroadcastComposite,
        rectangle_broadcast_metadata,
        CCMI::ConnectionManager::ColorConnMgr,
        PAMI_XFER_BROADCAST >
      RectangleBroadcastFactory;

      extern inline void rectangle_1color_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("X0:Rectangle1Color:P2P:P2P");
      }

      typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
      < 1,
        CCMI::Schedule::TorusRect,
        CCMI::ConnectionManager::ColorConnMgr,
        get_colors,
        PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX>
      RectangleBroadcastComposite1Color;

      typedef CCMI::Adaptor::CollectiveProtocolFactoryT
      < RectangleBroadcastComposite1Color,
        rectangle_1color_broadcast_metadata,
        CCMI::ConnectionManager::ColorConnMgr,
        PAMI_XFER_BROADCAST >
      Rectangle1ColorBroadcastFactory;
#endif

      extern inline void binomial_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
      }

      extern inline void ring_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("X0:Ring:P2P:P2P");
      }

      typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
      < 1,
        CCMI::Schedule::TopoMultinomial,
        CCMI::ConnectionManager::ColorGeometryConnMgr,
        get_colors >
      BinomialBroadcastComposite;

      typedef CCMI::Adaptor::CollectiveProtocolFactoryT
      < BinomialBroadcastComposite,
        binomial_broadcast_metadata,
        CCMI::ConnectionManager::ColorGeometryConnMgr,
        PAMI_XFER_BROADCAST >
      BinomialBroadcastFactory;

      typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
      < 1,
        CCMI::Schedule::RingSchedule,
        CCMI::ConnectionManager::ColorGeometryConnMgr,
        get_colors >
        RingBroadcastComposite;

      typedef CCMI::Adaptor::CollectiveProtocolFactoryT
      < RingBroadcastComposite,
        ring_broadcast_metadata,
        CCMI::ConnectionManager::ColorGeometryConnMgr,
        PAMI_XFER_BROADCAST >
        RingBroadcastFactory;
      
      extern inline void binomial_broadcast_metadata_singleth(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:BinomialSingleTh:P2P:P2P");
        m->check_correct.values.global_order = 8 ; /* ordered - single threaded */
        m->check_correct.values.contigsflags = 1;
        m->check_correct.values.contigrflags = 1;
      }

      extern inline void ring_broadcast_metadata_singleth(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("X0:RingSingleTh:P2P:P2P");
        m->check_correct.values.global_order = 8 ; /* ordered - single threaded */
      }      

      typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
      < 1,
        CCMI::Schedule::TopoMultinomial,
        CCMI::ConnectionManager::ColorConnMgr,
        get_colors >
      BinomialBroadcastSingleThComposite;

      typedef CCMI::Adaptor::CollectiveProtocolFactoryT
      < BinomialBroadcastSingleThComposite,
        binomial_broadcast_metadata_singleth,
        CCMI::ConnectionManager::ColorConnMgr,
        PAMI_XFER_BROADCAST >
      BinomialBroadcastSingleThFactory;

      typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
      < 1,
        CCMI::Schedule::RingSchedule,
        CCMI::ConnectionManager::ColorConnMgr,
        get_colors >
      RingBroadcastSingleThComposite;

      typedef CCMI::Adaptor::CollectiveProtocolFactoryT
      < RingBroadcastSingleThComposite,
        ring_broadcast_metadata_singleth,
        CCMI::ConnectionManager::ColorConnMgr,
        PAMI_XFER_BROADCAST >
      RingBroadcastSingleThFactory;

      extern inline void am_rb_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("X0:RankBased_Binomial:P2P:P2P");
	m->check_correct.values.asyncflowctl = 1;
      }

      extern inline void am_cs_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:SequenceBased_Binomial:P2P:P2P");
      }

      extern inline void create_schedule(void                        * buf,
                           unsigned                      size,
                           unsigned                      root,
                           Interfaces::NativeInterface * native,
                           PAMI_GEOMETRY_CLASS         * g)
      {
        (void)size;(void)root;
        new (buf) CCMI::Schedule::TopoMultinomial(native->endpoint(),
                                                  (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX),
                                                  0);
      }

      extern inline unsigned getKey(unsigned                                                root,
                      unsigned                                                connid,
                      PAMI_GEOMETRY_CLASS                                    *geometry,
                      ConnectionManager::BaseConnectionManager              **connmgr)
      {
        (void)connid;(void)geometry;(void)connmgr;
        return root;
      }

      extern inline void create_schedule_as(void                        * buf,
                              unsigned                      size,
                              unsigned                      root,
                              Interfaces::NativeInterface * native,
                              PAMI_GEOMETRY_CLASS          * g)
      {
        (void)size;(void)root;
        new (buf) CCMI::Schedule::TopoMultinomial(native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX), 0);
      }

      extern inline unsigned getKey_as(unsigned                                   root,
                         unsigned                                   connid,
                         PAMI_GEOMETRY_CLASS                      * geometry,
                         ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        if (connid != (unsigned) - 1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }

        return cm->updateConnectionId( geometry->comm() );
      }

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastT
      < CCMI::Schedule::TopoMultinomial,
        CCMI::ConnectionManager::RankBasedConnMgr,
        create_schedule>
      AsyncRBBinomialBroadcastComposite;

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastFactoryT
      < AsyncRBBinomialBroadcastComposite,
        am_rb_broadcast_metadata,
        CCMI::ConnectionManager::RankBasedConnMgr,
        getKey >
      AsyncRBBinomialBroadcastFactory;


      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastT
      < CCMI::Schedule::TopoMultinomial,
        CCMI::ConnectionManager::CommSeqConnMgr,
        create_schedule_as > AsyncCSBinomialBroadcastComposite;

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastFactoryT
      < AsyncCSBinomialBroadcastComposite,
        am_cs_broadcast_metadata,
        CCMI::ConnectionManager::CommSeqConnMgr,
        getKey_as> AsyncCSBinomialBroadcastFactory;

      // Generic tree 2-nomial broadcast
      extern inline void am_2nomial_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:2-nomial:P2P:P2P");
        m->check_correct.values.asyncflowctl = 1;
      }
      extern inline void create_schedule_2nomial(void                        * buf,
                                                  unsigned                      size,
                                                  unsigned                      root,
                                                  Interfaces::NativeInterface * native,
                                                  PAMI_GEOMETRY_CLASS          * g)
      {
        (void)root;(void)size;
        new (buf) CCMI::Schedule::KnomialBcastSchedule<2>(native->endpoint(),
                                                          ((PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)));
          }
      extern inline unsigned getKey_2nomial(unsigned                                   root,
                                            unsigned                                   connid,
                                            PAMI_GEOMETRY_CLASS                      * geometry,
                                            ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        if (connid != (unsigned) - 1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
        return cm->updateConnectionId( geometry->comm() );
      }
      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastT
      < CCMI::Schedule::KnomialBcastSchedule<2>,
        CCMI::ConnectionManager::CommSeqConnMgr,
        create_schedule_2nomial > Async2nomialBroadcastComposite;

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastFactoryT
      < Async2nomialBroadcastComposite,
        am_2nomial_broadcast_metadata,
        CCMI::ConnectionManager::CommSeqConnMgr,
        getKey_2nomial> Async2nomialBroadcastFactory;


      // Generic tree 3-nomial broadcast
      extern inline void am_3nomial_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:3-nomial:P2P:P2P");
      }
      extern inline void create_schedule_3nomial(void                        * buf,
                                                  unsigned                      size,
                                                  unsigned                      root,
                                                  Interfaces::NativeInterface * native,
                                                  PAMI_GEOMETRY_CLASS          * g)
      {
        (void)size;(void)root;
        new (buf) CCMI::Schedule::KnomialBcastSchedule<3>(native->endpoint(),
                                                          ((PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)));
          }
      extern inline unsigned getKey_3nomial(unsigned                                   root,
                                            unsigned                                   connid,
                                            PAMI_GEOMETRY_CLASS                      * geometry,
                                            ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        if (connid != (unsigned) - 1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
        return cm->updateConnectionId( geometry->comm() );
      }
      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastT
      < CCMI::Schedule::KnomialBcastSchedule<3>,
        CCMI::ConnectionManager::CommSeqConnMgr,
        create_schedule_3nomial > Async3nomialBroadcastComposite;

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastFactoryT
      < Async3nomialBroadcastComposite,
        am_3nomial_broadcast_metadata,
        CCMI::ConnectionManager::CommSeqConnMgr,
        getKey_3nomial> Async3nomialBroadcastFactory;

      // Generic tree 4-nomial broadcast
      extern inline void am_4nomial_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:4-nomial:P2P:P2P");
      }
      extern inline void create_schedule_4nomial(void                        * buf,
                                                  unsigned                      size,
                                                  unsigned                      root,
                                                  Interfaces::NativeInterface * native,
                                                  PAMI_GEOMETRY_CLASS          * g)
      {
        (void)root;(void)size;
        new (buf) CCMI::Schedule::KnomialBcastSchedule<4>(native->endpoint(),
                                                          ((PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)));
      }
      extern inline unsigned getKey_4nomial(unsigned                                   root,
                                            unsigned                                   connid,
                                            PAMI_GEOMETRY_CLASS                      * geometry,
                                            ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        if (connid != (unsigned) - 1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
        return cm->updateConnectionId( geometry->comm() );
      }
      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastT
      < CCMI::Schedule::KnomialBcastSchedule<4>,
        CCMI::ConnectionManager::CommSeqConnMgr,
        create_schedule_4nomial > Async4nomialBroadcastComposite;

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastFactoryT
      < Async4nomialBroadcastComposite,
        am_4nomial_broadcast_metadata,
        CCMI::ConnectionManager::CommSeqConnMgr,
        getKey_4nomial> Async4nomialBroadcastFactory;

      // Generic tree 4-nary broadcast
      extern inline void am_2nary_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:2-nary:P2P:P2P");
      }

      extern inline void sync_2nary_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:Sync2-nary:P2P:P2P");
      }

      extern inline void create_schedule_2nary(void                        * buf,
                                                  unsigned                      size,
                                                  unsigned                      root,
                                                  Interfaces::NativeInterface * native,
                                                  PAMI_GEOMETRY_CLASS          * g)
      {
        (void)size;(void)root;
        new (buf) CCMI::Schedule::KnaryBcastSchedule<2>(native->endpoint(),
                                                          ((PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)));
      }
      extern inline unsigned getKey_2nary(unsigned                                   root,
                                            unsigned                                   connid,
                                            PAMI_GEOMETRY_CLASS                      * geometry,
                                            ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        if (connid != (unsigned) - 1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
        return cm->updateConnectionId( geometry->comm() );
      }
      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastT
      < CCMI::Schedule::KnaryBcastSchedule<2>,
        CCMI::ConnectionManager::CommSeqConnMgr,
        create_schedule_2nary > Async2naryBroadcastComposite;

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastFactoryT
      < Async2naryBroadcastComposite,
        am_2nary_broadcast_metadata,
        CCMI::ConnectionManager::CommSeqConnMgr,
        getKey_2nary> Async2naryBroadcastFactory;      
      
      typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
      < 1,
        CCMI::Schedule::KnaryBcastSchedule<2>,
        CCMI::ConnectionManager::ColorConnMgr,
        get_colors >
      TwoNaryBroadcastComposite;

      typedef CCMI::Adaptor::CollectiveProtocolFactoryT
      < TwoNaryBroadcastComposite,
        sync_2nary_broadcast_metadata,
        CCMI::ConnectionManager::ColorConnMgr,
        PAMI_XFER_BROADCAST >
      TwoNaryBroadcastFactory;

      // Generic tree 3-nary broadcast
      extern inline void am_3nary_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:3-nary:P2P:P2P");
      }
      extern inline void create_schedule_3nary(void                        * buf,
                                                  unsigned                      size,
                                                  unsigned                      root,
                                                  Interfaces::NativeInterface * native,
                                                  PAMI_GEOMETRY_CLASS          * g)
      {
        (void)size;(void)root;
        new (buf) CCMI::Schedule::KnaryBcastSchedule<3>(native->endpoint(),
                                                        ((PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)));
      }
      extern inline unsigned getKey_3nary(unsigned                                   root,
                                            unsigned                                   connid,
                                            PAMI_GEOMETRY_CLASS                      * geometry,
                                            ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        if (connid != (unsigned) - 1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
        return cm->updateConnectionId( geometry->comm() );
      }
      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastT
      < CCMI::Schedule::KnaryBcastSchedule<3>,
        CCMI::ConnectionManager::CommSeqConnMgr,
        create_schedule_3nary > Async3naryBroadcastComposite;

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastFactoryT
      < Async3naryBroadcastComposite,
        am_3nary_broadcast_metadata,
        CCMI::ConnectionManager::CommSeqConnMgr,
        getKey_3nary> Async3naryBroadcastFactory;      

      // Generic tree 4-nary broadcast
      extern inline void am_4nary_broadcast_metadata(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:4-nary:P2P:P2P");
      }
      extern inline void create_schedule_4nary(void                        * buf,
                                                  unsigned                      size,
                                                  unsigned                      root,
                                                  Interfaces::NativeInterface * native,
                                                  PAMI_GEOMETRY_CLASS          * g)
      {
        (void)size;(void)root;
        new (buf) CCMI::Schedule::KnaryBcastSchedule<4>(native->endpoint(),
                                                          ((PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX)));
      }
      extern inline unsigned getKey_4nary(unsigned                                   root,
                                            unsigned                                   connid,
                                            PAMI_GEOMETRY_CLASS                      * geometry,
                                            ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        if (connid != (unsigned) - 1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
        return cm->updateConnectionId( geometry->comm() );
      }
      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastT
      < CCMI::Schedule::KnaryBcastSchedule<4>,
        CCMI::ConnectionManager::CommSeqConnMgr,
        create_schedule_4nary > Async4naryBroadcastComposite;

      typedef CCMI::Adaptor::Broadcast::AsyncBroadcastFactoryT
      < Async4naryBroadcastComposite,
        am_4nary_broadcast_metadata,
        CCMI::ConnectionManager::CommSeqConnMgr,
        getKey_4nary> Async4naryBroadcastFactory;      
    }//Broadcast

    namespace P2PAMBroadcast
    {
      extern inline unsigned getKey(unsigned                     root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                       *geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;(void)geometry;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::RankSeqConnMgr *cm = (ConnectionManager::RankSeqConnMgr *)*connmgr;
        return cm->updateConnectionId(root);
      }

      namespace Binomial
      {
        extern inline void am_broadcast_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
        }

        extern inline void create_schedule(void                        * buf,
                                           unsigned                      size,
                                           unsigned                      root,
                                           Interfaces::NativeInterface * native,
                                           PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::TopoMultinomial(native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX), 0);
        }

        typedef CCMI::Adaptor::AMBroadcast::AMBroadcastT
        < CCMI::Schedule::TopoMultinomial,
          CCMI::ConnectionManager::RankSeqConnMgr,
          create_schedule>
        Composite;

        typedef CCMI::Adaptor::AMBroadcast::AMBroadcastFactoryT
        < Composite,
          am_broadcast_metadata,
          CCMI::ConnectionManager::RankSeqConnMgr,
          getKey>
        Factory;
      }
    }//AMBroadcast

    namespace P2PAllreduce
    {
      /// New Binomial algorithms
      /// class Binomial::Composite and Binomial::Factory
      ///
      /// \brief Binomial allreduce protocol
      ///
      /// Use the BinomialTreeSchedule
      ///
      namespace Binomial
      {
        extern inline void get_colors (PAMI::Topology             * t,
                         unsigned                    bytes,
                         unsigned                  * colors,
                         unsigned                  & ncolors)
        {
          (void)t;(void)bytes;
          ncolors = 1;
          colors[0] = CCMI::Schedule::TorusRect::NO_COLOR;
        }

        extern inline void binomial_allreduce_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
          m->check_correct.values.oneatatime = 1; /* OAT is oneatatime */
        }

        typedef CCMI::Adaptor::Allreduce::AsyncOATCompositeT
          < CCMI::Executor::AllreduceBaseExec<CCMI::ConnectionManager::RankBasedConnMgr, true>,
          CCMI::Schedule::TopoMultinomial,
          CCMI::ConnectionManager::RankBasedConnMgr > Composite;	

        typedef CCMI::Adaptor::Allreduce::ProtocolFactoryT
          < Composite,
          binomial_allreduce_metadata,
          CCMI::ConnectionManager::RankBasedConnMgr,
          PAMI_XFER_ALLREDUCE >
          Factory;
        typedef CCMI::Adaptor::Allreduce::OAT<Composite, Factory> OAT;

        extern inline void binomial4_allreduce_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("X0:MultinomialRadix4:P2P:P2P");
          m->check_correct.values.oneatatime = 1; /* OAT is oneatatime */
        }

        typedef CCMI::Adaptor::Allreduce::AsyncOATCompositeT
          < CCMI::Executor::AllreduceBaseExec<CCMI::ConnectionManager::RankBasedConnMgr>,
          CCMI::Schedule::TopoMultinomial4,
          CCMI::ConnectionManager::RankBasedConnMgr > Composite4;

        typedef CCMI::Adaptor::Allreduce::ProtocolFactoryT
          < Composite4,
          binomial4_allreduce_metadata,
          CCMI::ConnectionManager::RankBasedConnMgr,
          PAMI_XFER_ALLREDUCE >
        Factory4;
        typedef CCMI::Adaptor::Allreduce::OAT<Composite4, Factory4> OAT4;

        extern inline void binomial8_allreduce_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("X0:MultinomialRadix8:P2P:P2P");
          m->check_correct.values.oneatatime = 1; /* OAT is oneatatime */
        }

        typedef CCMI::Adaptor::Allreduce::AsyncOATCompositeT
          < CCMI::Executor::AllreduceBaseExec<CCMI::ConnectionManager::RankBasedConnMgr>,
          CCMI::Schedule::TopoMultinomial8,
          CCMI::ConnectionManager::RankBasedConnMgr> Composite8;

        typedef CCMI::Adaptor::Allreduce::ProtocolFactoryT
        < Composite8,
          binomial8_allreduce_metadata,
          CCMI::ConnectionManager::RankBasedConnMgr,
          PAMI_XFER_ALLREDUCE >
        Factory8;

        typedef CCMI::Adaptor::Allreduce::OAT<Composite8, Factory8> OAT8;

       extern inline unsigned getKey(unsigned                                   root,
                       unsigned                                   connid,
                       PAMI_GEOMETRY_CLASS                      * geometry,
                       ConnectionManager::BaseConnectionManager **connmgr)
       {
         (void)geometry;(void)root;
         if (connid != (unsigned)-1)
         {
           *connmgr = NULL; //use this key as connection id
           return connid;
         }
         ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
         return cm->updateConnectionId( geometry->comm() );
       }

       /// New asynchronous CommSeq binomial allreduce algorithms
       /// class Binomial::AsyncAllreduceT and Binomial::AsyncAllreduceFactoryT
       ///
       /// \brief Binomial allreduce protocol
       ///
       /// Use the BinomialTreeSchedule
       ///
       extern inline void ascs_binomial_allreduce_metadata(pami_metadata_t *m)
       {
         new(m) PAMI::Geometry::Metadata("I0:SequenceBased_Binomial:P2P:P2P");
       }

       typedef CCMI::Adaptor::Allreduce::AsyncAllreduceT<CCMI::Schedule::TopoMultinomial,
         CCMI::ConnectionManager::CommSeqConnMgr, pami_allreduce_t>
         AsyncCSBinomAllreduceComposite;
       typedef CCMI::Adaptor::Allreduce::AsyncAllreduceFactoryT<AsyncCSBinomAllreduceComposite,
         ascs_binomial_allreduce_metadata, CCMI::ConnectionManager::CommSeqConnMgr,
         pami_allreduce_t, getKey> AsyncCSBinomAllreduceFactory;

       extern inline void ascs_binomial_reduce_metadata(pami_metadata_t *m)
       {
         new(m) PAMI::Geometry::Metadata("I0:RankBased_Binomial:P2P:P2P");
         m->check_correct.values.asyncflowctl = 1;
       }

       typedef CCMI::Adaptor::Allreduce::AsyncAllreduceT<CCMI::Schedule::TopoMultinomial,
         CCMI::ConnectionManager::CommSeqConnMgr, pami_reduce_t>
         AsyncCSBinomReduceComposite;
       typedef CCMI::Adaptor::Allreduce::AsyncAllreduceFactoryT<AsyncCSBinomReduceComposite,
         ascs_binomial_reduce_metadata, CCMI::ConnectionManager::CommSeqConnMgr,
         pami_reduce_t, getKey> AsyncCSBinomReduceFactory;

      };//Binomial

      namespace Ring {
        extern inline void ring_allreduce_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("X0:Ring:P2P:P2P");
        }

        typedef CCMI::Adaptor::Allreduce::MultiColorCompositeT
        < 1, CCMI::Executor::PipelinedAllreduce<CCMI::ConnectionManager::SimpleConnMgr>,
          CCMI::Schedule::RingSchedule,
          CCMI::ConnectionManager::SimpleConnMgr,
          Binomial::get_colors,
          1 > Composite;

        typedef CCMI::Adaptor::Allreduce::ProtocolFactoryT
          < Composite,
          ring_allreduce_metadata,
          CCMI::ConnectionManager::SimpleConnMgr,
          PAMI_XFER_ALLREDUCE >
          Factory;
      }; //Ring

    };//Allreduce

    namespace P2PAMReduce
    {
      extern inline unsigned getKey(unsigned                     root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                       *geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)geometry;(void)root;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::RankSeqConnMgr *cm = (ConnectionManager::RankSeqConnMgr *)*connmgr;
        return cm->updateConnectionId(root);
      }
      namespace Binomial
      {
        extern inline void am_reduce_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
        }

        extern inline void create_schedule(void                * buf,
                                   unsigned                      size,
                                   unsigned                      root,
                                   Interfaces::NativeInterface * native,
                                   PAMI_GEOMETRY_CLASS         * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::TopoMultinomial(native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX), 0);
        }

        typedef CCMI::Adaptor::AMReduce::AMReduceT
        < CCMI::Schedule::TopoMultinomial,
          CCMI::Schedule::TopoMultinomial,
          CCMI::ConnectionManager::RankSeqConnMgr,
          create_schedule,
          create_schedule > Composite;

        typedef CCMI::Adaptor::AMReduce::AMReduceFactoryT
        < Composite,
          am_reduce_metadata,
          CCMI::ConnectionManager::RankSeqConnMgr,
          getKey >
        Factory;
      }//Binomial
    }//AMReduce

    namespace P2PScatter
    {

      extern inline unsigned getKey(unsigned                                   root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                      * geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        return cm->updateConnectionId( geometry->comm() );
      }

      namespace Binomial
      {

        extern inline void create_schedule(void                        * buf,
                             unsigned                      size,
                             unsigned                      root,
                             Interfaces::NativeInterface * native,
                             PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<>(native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void binomial_scatter_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
          m->check_correct.values.asyncflowctl = 1;
        }

        typedef CCMI::Adaptor::Scatter::AsyncScatterT
        < CCMI::Schedule::GenericTreeSchedule<>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_schedule, pami_scatter_t > Composite;

        typedef CCMI::Adaptor::Scatter::AsyncScatterFactoryT
        < Composite,
          binomial_scatter_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey > Factory;

      } // Binomial

      namespace Flat
      {

        extern inline void create_schedule(void                        * buf,
                             unsigned                      size,
                             unsigned                      root,
                             Interfaces::NativeInterface * native,
                             PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,1> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void flat_scatter_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Flat:P2P:P2P");
        }

        typedef CCMI::Adaptor::Scatter::AsyncScatterT
        < CCMI::Schedule::GenericTreeSchedule<1, 1, 1>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_schedule, pami_scatter_t > Composite;

        typedef CCMI::Adaptor::Scatter::AsyncScatterFactoryT
        < Composite,
          flat_scatter_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        Factory;

      } // Flat
    } // P2PScatter

    namespace P2PScatterv
      {

      extern inline unsigned getKey(unsigned                                   root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                      * geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        return cm->updateConnectionId( geometry->comm() );
      }


        extern inline void create_schedule(void                        * buf,
                             unsigned                      size,
                             unsigned                      root,
                             Interfaces::NativeInterface * native,
                             PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,1> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void scatterv_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:SoftwareTree:P2P:P2P");
        }

        typedef CCMI::Adaptor::Scatter::AsyncScatterT
        < CCMI::Schedule::GenericTreeSchedule<1, 1, 1>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_schedule, pami_scatterv_t > Composite;

        typedef CCMI::Adaptor::Scatter::AsyncScatterFactoryT
        < Composite,
          scatterv_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        Factory;

        extern inline void scatterv_int_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:SoftwareTree:P2P:P2P");
        }

        typedef CCMI::Adaptor::Scatter::AsyncScatterT
        < CCMI::Schedule::GenericTreeSchedule<1, 1, 1>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_schedule, pami_scatterv_int_t > IntComposite;

        typedef CCMI::Adaptor::Scatter::AsyncScatterFactoryT
        < IntComposite,
          scatterv_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        IntFactory;

    } // P2PScatterv

    namespace P2PReduceScatter
    {

      extern inline unsigned getKey(unsigned                                   root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                      * geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        return cm->updateConnectionId( geometry->comm() );
      }


      extern inline void create_schedule(void                        * buf,
                           unsigned                      size,
                           unsigned                      root,
                           Interfaces::NativeInterface * native,
                           PAMI_GEOMETRY_CLASS          * g)
      {
        (void)size;(void)root;
        new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,1> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
      }


       extern inline void ascs_reduce_scatter_metadata(pami_metadata_t *m)
       {
         new(m) PAMI::Geometry::Metadata("I0:Sequence_Tree:P2P:P2P");
       }

       typedef CCMI::Adaptor::Allreduce::AsyncReduceScatterT<CCMI::Schedule::TopoMultinomial,
         CCMI::Schedule::GenericTreeSchedule<1, 1, 1>,
         CCMI::ConnectionManager::CommSeqConnMgr, create_schedule>
         AsyncCSReduceScatterComposite;

       typedef CCMI::Adaptor::Allreduce::AsyncReduceScatterFactoryT<AsyncCSReduceScatterComposite,
         ascs_reduce_scatter_metadata, CCMI::ConnectionManager::CommSeqConnMgr,getKey>
         AsyncCSReduceScatterFactory;

    } // P2PReduceScatter

    namespace P2PAMScatter
    {
      extern inline unsigned getKey(unsigned                     root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                       *geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;(void)geometry;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::RankSeqConnMgr *cm = (ConnectionManager::RankSeqConnMgr *)*connmgr;
        return cm->updateConnectionId(root);
      }
      namespace Binomial
      {
        extern inline void am_scatter_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
        }

        extern inline void create_scatter_schedule(void        * buf,
                                   unsigned                      size,
                                   unsigned                      root,
                                   Interfaces::NativeInterface * native,
                                   PAMI_GEOMETRY_CLASS         * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,2> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        typedef CCMI::Adaptor::AMScatter::AMScatterT
        < CCMI::Schedule::GenericTreeSchedule<1, 1, 2>,
          CCMI::ConnectionManager::RankSeqConnMgr,
          create_scatter_schedule > Composite;

        typedef CCMI::Adaptor::AMScatter::AMScatterFactoryT
        < Composite,
          am_scatter_metadata,
          CCMI::ConnectionManager::RankSeqConnMgr,
          getKey >
        Factory;
      }//Binomial
    }//AMScatter

    namespace P2PGather
    {

      extern inline unsigned getKey(unsigned                                   root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                      * geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        return cm->updateConnectionId( geometry->comm() );
      }

      namespace Binomial
      {

        extern inline void create_schedule(void                        * buf,
                             unsigned                      size,
                             unsigned                      root,
                             Interfaces::NativeInterface * native,
                             PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void binomial_gather_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
        }

        typedef CCMI::Adaptor::Gather::AsyncGatherT
        < CCMI::Schedule::GenericTreeSchedule<>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_schedule, pami_gather_t > Composite;

        typedef CCMI::Adaptor::Gather::AsyncGatherFactoryT
        < Composite,
          binomial_gather_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        Factory;

      }// Binomial

      namespace Flat
      {

        extern inline void create_gather_schedule(void                        * buf,
                                    unsigned                      size,
                                    unsigned                      root,
                                    Interfaces::NativeInterface * native,
                                    PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,1> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void create_bcast_schedule(void                        * buf,
                                   unsigned                      size,
                                   unsigned                      root,
                                   Interfaces::NativeInterface * native,
                                   PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,2> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void flat_gather_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Flat:P2P:P2P");
        }

        typedef CCMI::Adaptor::Gather::AsyncLongGatherT
        < pami_gather_t,
          CCMI::Schedule::GenericTreeSchedule<1, 1, 2>,
          CCMI::Schedule::GenericTreeSchedule<1, 1, 1>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_bcast_schedule,
          create_gather_schedule > Composite;

        typedef CCMI::Adaptor::Gather::AsyncLongGatherFactoryT
        < Composite,
          flat_gather_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        Factory;

      } // Flat
    }// P2PGather

    namespace P2PAMGather
    {
      extern inline unsigned getKey(unsigned                     root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                       *geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)geometry;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::RankSeqConnMgr *cm = (ConnectionManager::RankSeqConnMgr *)*connmgr;
        return cm->updateConnectionId(root);
      }
      namespace Binomial
      {
        extern inline void am_gather_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
        }

        extern inline void create_scatter_schedule(void        * buf,
                                   unsigned                      size,
                                   unsigned                      root,
                                   Interfaces::NativeInterface * native,
                                   PAMI_GEOMETRY_CLASS         * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,2> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void create_gather_schedule(void   * buf,
                             unsigned                      size,
                             unsigned                      root,
                             Interfaces::NativeInterface * native,
                             PAMI_GEOMETRY_CLASS         * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,2> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        typedef CCMI::Adaptor::AMGather::AMGatherT
        < CCMI::Schedule::GenericTreeSchedule<1, 1, 2>,
          CCMI::Schedule::GenericTreeSchedule<1, 1, 2>,
          CCMI::ConnectionManager::RankSeqConnMgr,
          create_scatter_schedule,
          create_gather_schedule > Composite;

        typedef CCMI::Adaptor::AMGather::AMGatherFactoryT
        < Composite,
          am_gather_metadata,
          CCMI::ConnectionManager::RankSeqConnMgr,
          getKey >
        Factory;
      }//Binomial
    }//AMGather

    namespace P2PGatherv
    {

      extern inline unsigned getKey(unsigned                                   root,
                      unsigned                                   connid,
                      PAMI_GEOMETRY_CLASS                      * geometry,
                      ConnectionManager::BaseConnectionManager **connmgr)
      {
        (void)root;
        if (connid != (unsigned)-1)
        {
          *connmgr = NULL; //use this key as connection id
          return connid;
        }
        ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
        return cm->updateConnectionId( geometry->comm() );
      }


        extern inline void create_gatherv_schedule(void                        * buf,
                                     unsigned                      size,
                                     unsigned                      root,
                                     Interfaces::NativeInterface * native,
                                     PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,1> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void create_bcast_schedule(void                        * buf,
                                   unsigned                      size,
                                   unsigned                      root,
                                   Interfaces::NativeInterface * native,
                                   PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<1,1,2> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void gatherv_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Tree:P2P:P2P");
        }

        typedef CCMI::Adaptor::Gather::AsyncLongGatherT
        < pami_gatherv_t,
          CCMI::Schedule::GenericTreeSchedule<1, 1, 2>,
          CCMI::Schedule::GenericTreeSchedule<1, 1, 1>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_bcast_schedule,
          create_gatherv_schedule > Composite;

        typedef CCMI::Adaptor::Gather::AsyncLongGatherFactoryT
        < Composite,
          gatherv_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        Factory;

        extern inline void gatherv_int_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Tree:P2P:P2P");
        }

        typedef CCMI::Adaptor::Gather::AsyncLongGatherT
        < pami_gatherv_int_t,
          CCMI::Schedule::GenericTreeSchedule<1, 1, 2>,
          CCMI::Schedule::GenericTreeSchedule<1, 1, 1>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_bcast_schedule,
          create_gatherv_schedule > IntComposite;

        typedef CCMI::Adaptor::Gather::AsyncLongGatherFactoryT
        < IntComposite,
          gatherv_int_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        IntFactory;

     } // P2PGatherv

    namespace P2PScan
    {
      namespace Binomial
      {

        extern inline unsigned getKey(unsigned                                   root,
                        unsigned                                   connid,
                        PAMI_GEOMETRY_CLASS                      * geometry,
                        ConnectionManager::BaseConnectionManager **connmgr)
        {
          (void)root;
          if (connid != (unsigned)-1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
          ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
          return cm->updateConnectionId( geometry->comm() );
        }

        extern inline void create_schedule(void                        * buf,
                             unsigned                      size,
                             unsigned                      root,
                             Interfaces::NativeInterface * native,
                             PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<> (native->endpoint(), (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void binomial_scan_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
        }

        typedef CCMI::Adaptor::Scan::AsyncScanT
        < CCMI::Schedule::GenericTreeSchedule<>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_schedule > Composite;

        typedef CCMI::Adaptor::Scan::AsyncScanFactoryT
        < Composite,
          binomial_scan_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        Factory;

      }// Binomial
    }// P2PScan

    namespace P2PAllgather
    {

      namespace Binomial
      {

        extern inline unsigned getKey(unsigned                                   root,
                                      unsigned                                   connid,
                                      PAMI_GEOMETRY_CLASS                      * geometry,
                                      ConnectionManager::BaseConnectionManager **connmgr)
        {
          (void)root;
          if (connid != (unsigned)-1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
          ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
          return cm->updateConnectionId( geometry->comm() );
        }

        extern inline void create_schedule(void                        * buf,
                                           unsigned                      size,
                                           unsigned                      root,
                                           Interfaces::NativeInterface * native,
                                           PAMI_GEOMETRY_CLASS          * g)
        {
          (void)size;(void)root;
          new (buf) CCMI::Schedule::GenericTreeSchedule<> (native->endpoint(),
                                                           (PAMI::Topology *)g->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX));
        }

        extern inline void binomial_allgather_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Binomial:P2P:P2P");
        }

        typedef CCMI::Adaptor::Allgather::AsyncAllgatherT
        < CCMI::Schedule::GenericTreeSchedule<>,
          CCMI::ConnectionManager::CommSeqConnMgr,
          create_schedule > Composite;

        typedef CCMI::Adaptor::Allgather::AsyncAllgatherFactoryT
        < Composite,
          binomial_allgather_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        Factory;

      }// Binomial
    }// P2PAllgather

    namespace P2PAllgatherv
    {

      namespace Ring
      {

        extern inline unsigned getKey(unsigned                                   root,
                        unsigned                                   connid,
                        PAMI_GEOMETRY_CLASS                      * geometry,
                        ConnectionManager::BaseConnectionManager **connmgr)
        {
          (void)root;
          if (connid != (unsigned)-1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
          ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
          return cm->updateConnectionId( geometry->comm() );
        }

        extern inline void ring_allgather_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Ring:P2P:P2P");
        }

        extern inline void ring_allgatherv_int_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Ring:P2P:P2P");
        }

        extern inline void ring_allgatherv_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Ring:P2P:P2P");
        }

        typedef CCMI::Adaptor::Allgatherv::AsyncAllgathervT
        < CCMI::ConnectionManager::CommSeqConnMgr,
          pami_allgather_t > AllgatherComposite;

        typedef CCMI::Adaptor::Allgatherv::AsyncAllgathervFactoryT
        < pami_allgather_t,
          AllgatherComposite,
          ring_allgather_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        AllgatherFactory;

        typedef CCMI::Adaptor::Allgatherv::AsyncAllgathervT
        < CCMI::ConnectionManager::CommSeqConnMgr,
          pami_allgatherv_int_t > AllgathervIntComposite;

        typedef CCMI::Adaptor::Allgatherv::AsyncAllgathervFactoryT
        < pami_allgatherv_int_t,
          AllgathervIntComposite,
          ring_allgatherv_int_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        AllgathervIntFactory;

        typedef CCMI::Adaptor::Allgatherv::AsyncAllgathervT
        < CCMI::ConnectionManager::CommSeqConnMgr,
          pami_allgatherv_t > AllgathervComposite;

        typedef CCMI::Adaptor::Allgatherv::AsyncAllgathervFactoryT
        < pami_allgatherv_t,
          AllgathervComposite,
          ring_allgatherv_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        AllgathervFactory;


      }// Ring
    }// P2PAllgatherv

    namespace P2PAlltoallv
    {

      namespace Pairwise
      {

        extern inline unsigned getKey(unsigned                                   root,
                        unsigned                                   connid,
                        PAMI_GEOMETRY_CLASS                      * geometry,
                        ConnectionManager::BaseConnectionManager **connmgr)
        {
          (void)root;
          if (connid != (unsigned)-1)
          {
            *connmgr = NULL; //use this key as connection id
            return connid;
          }
          ConnectionManager::CommSeqConnMgr *cm = (ConnectionManager::CommSeqConnMgr *)*connmgr;
          return cm->updateConnectionId( geometry->comm() );
        }

        extern inline void pairwise_alltoall_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Pairwise:P2P:P2P");
          m->check_correct.values.contigsflags = 1;
          m->check_correct.values.contigrflags = 1;
        }

        extern inline void pairwise_alltoallv_int_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Pairwise:P2P:P2P");
          m->check_correct.values.contigsflags = 1;
          m->check_correct.values.contigrflags = 1;
        }

        extern inline void pairwise_alltoallv_metadata(pami_metadata_t *m)
        {
          new(m) PAMI::Geometry::Metadata("I0:Pairwise:P2P:P2P");
          m->check_correct.values.contigsflags = 1;
          m->check_correct.values.contigrflags = 1;
        }

        typedef CCMI::Adaptor::Alltoallv::AsyncAlltoallvT
        < CCMI::ConnectionManager::CommSeqConnMgr,
          pami_alltoall_t > AlltoallComposite;

        typedef CCMI::Adaptor::Alltoallv::AsyncAlltoallvFactoryT
        < pami_alltoall_t,
          AlltoallComposite,
          pairwise_alltoall_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        AlltoallFactory;

        typedef CCMI::Adaptor::Alltoallv::AsyncAlltoallvT
        < CCMI::ConnectionManager::CommSeqConnMgr,
          pami_alltoallv_int_t > AlltoallvIntComposite;

        typedef CCMI::Adaptor::Alltoallv::AsyncAlltoallvFactoryT
        < pami_alltoallv_int_t,
          AlltoallvIntComposite,
          pairwise_alltoallv_int_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        AlltoallvIntFactory;

        typedef CCMI::Adaptor::Alltoallv::AsyncAlltoallvT
        < CCMI::ConnectionManager::CommSeqConnMgr,
          pami_alltoallv_t > AlltoallvComposite;

        typedef CCMI::Adaptor::Alltoallv::AsyncAlltoallvFactoryT
        < pami_alltoallv_t,
          AlltoallvComposite,
          pairwise_alltoallv_metadata,
          CCMI::ConnectionManager::CommSeqConnMgr,
          getKey >
        AlltoallvFactory;

      }// Pairwise
    }// P2PAlltoallv

    namespace P2PAlltoall
    {
      extern inline void getAlltoallMetaData(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:M2MComposite:P2P:P2P");
        m->check_correct.values.contigsflags = 1;
        m->check_correct.values.contigrflags = 1;
      }
      typedef CCMI::Adaptor::All2AllProtocol All2AllProtocol;
      typedef CCMI::Adaptor::All2AllFactoryT <All2AllProtocol, getAlltoallMetaData, CCMI::ConnectionManager::CommSeqConnMgr> All2AllFactory;

    };//P2PAlltoall
    namespace P2PAlltoallv
    {
      extern inline void getAlltoallvMetaData(pami_metadata_t *m)
      {
        new(m) PAMI::Geometry::Metadata("I0:M2MComposite:P2P:P2P");
        m->check_correct.values.contigsflags = 1;
        m->check_correct.values.contigrflags = 1;
      }
      typedef CCMI::Adaptor::All2AllvProtocolLong All2AllProtocolv;
      typedef CCMI::Adaptor::All2AllvFactoryT <All2AllProtocolv, getAlltoallvMetaData, CCMI::ConnectionManager::CommSeqConnMgr> All2AllvFactory;

      typedef CCMI::Adaptor::All2AllvProtocolInt All2AllProtocolv_int;
      typedef CCMI::Adaptor::All2AllvFactoryT <All2AllProtocolv_int, getAlltoallvMetaData, CCMI::ConnectionManager::CommSeqConnMgr> All2AllvFactory_int;

    };//P2PAlltoallv


  }//Adaptor
}//CCMI

#endif
