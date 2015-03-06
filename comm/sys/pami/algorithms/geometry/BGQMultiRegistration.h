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
 * \file algorithms/geometry/BGQMultiRegistration.h
 * \brief Simple BGQ collectives over shmem/mu multi* interface
 */

#ifndef __algorithms_geometry_BGQMultiRegistration_h__
#define __algorithms_geometry_BGQMultiRegistration_h__

#include <map>
#include <vector>
#include "algorithms/interfaces/CollRegistrationInterface.h"
#include "TypeDefs.h"
#include "algorithms/geometry/Metadata.h"
#include "algorithms/connmgr/SimpleConnMgr.h"
#include "algorithms/connmgr/CommSeqConnMgr.h"
#include "algorithms/protocols/broadcast/MultiCastComposite.h"
#include "algorithms/protocols/allreduce/MultiCombineComposite.h"
#include "algorithms/protocols/barrier/MultiSyncComposite.h"
#include "algorithms/protocols/AllSidedCollectiveProtocolFactoryT.h"
#include "algorithms/protocols/AllSidedCollectiveProtocolFactoryNCOT.h"
#include "algorithms/protocols/CollectiveProtocolFactoryT.h"

#include "algorithms/connmgr/ColorGeometryConnMgr.h"
#include "algorithms/connmgr/ColorConnMgr.h"
#include "algorithms/connmgr/ColorMapConnMgr.h"
#include "algorithms/connmgr/TorusConnMgr.h"
#include "algorithms/protocols/broadcast/BcastMultiColorCompositeT.h"
#include "algorithms/schedule/TorusRect.h"
#include "common/NativeInterface.h"
#include "algorithms/protocols/allgather/AllgatherOnBroadcastT.h"
#include "algorithms/protocols/CachedAllSidedFactoryT.h"

#include "algorithms/protocols/allreduce/MultiColorCompositeT.h"
#include "algorithms/protocols/allreduce/ThreadedMultiColorCompositeT.h"

#include "algorithms/geometry/P2PCCMIRegInfo.h"

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

#include "util/trace.h"

#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif

#ifndef addCollectiveCheckNonContig
// If non_contig is supported then add a query protocol, otherwise it's always works protocol
#define addCollectiveCheckNonContig(geometry,xfer,factory,context,id) \
{                                                                     \
  if(noncontig_support)                                               \
    geometry->addCollectiveCheck(xfer,  factory, context, id);        \
  else                                                                \
    geometry->addCollective(xfer,  factory, context, id);             \
}
#endif
namespace PAMI
{
  namespace CollRegistration
  {
    /// 
    /// Metadata functions
    /// 
    // The protocol requires T_Aligned buffers.  Call some other T_Function (maybe check dt/op?), then check the alignment.
    template <unsigned T_Send_Alignment_Required, unsigned T_Send_Alignment, unsigned T_Recv_Alignment_Required, unsigned T_Recv_Alignment, pami_metadata_function T_Function>
    inline metadata_result_t align_metadata_function(struct pami_xfer_t *in)
    {
      metadata_result_t result = T_Function(in);
      uint64_t  mask;
      if (T_Send_Alignment_Required)
      {
        mask  = T_Send_Alignment - 1; 
        if(in->cmd.xfer_allreduce.sndbuf != PAMI_IN_PLACE)
          result.check.align_send_buffer      |= (((uint64_t)in->cmd.xfer_allreduce.sndbuf & (uint64_t)mask) == 0) ? 0:1;
      }
      if (T_Recv_Alignment_Required)
      {
        mask  = T_Recv_Alignment - 1; 
        if(in->cmd.xfer_allreduce.rcvbuf != PAMI_IN_PLACE)
          result.check.align_recv_buffer      |= (((uint64_t)in->cmd.xfer_allreduce.rcvbuf & (uint64_t)mask) == 0) ? 0:1;
      }
      result.check.nonlocal = 1; // because of alignment, this is a non-local result.
      return result;
    }

    // The protocol supports a limited T_Range_Low->T_Range_High.  Call some other T_Function (maybe check dt/op?), then check the range.
    template <size_t T_Range_Low,size_t T_Range_High, pami_metadata_function T_Function>
    inline metadata_result_t range_metadata_function(struct pami_xfer_t *in)
    {
      metadata_result_t result = T_Function(in);

      PAMI::Type::TypeCode * type_obj = (PAMI::Type::TypeCode *)in->cmd.xfer_allreduce.stype;

      /// \todo Support non-contiguous
      PAMI_assert(type_obj->IsContiguous() &&  type_obj->IsPrimitive());

      size_t dataSent = type_obj->GetAtomSize() * in->cmd.xfer_allreduce.stypecount;
      result.check.range   |= !((dataSent <= T_Range_High) && (dataSent >= T_Range_Low));
      // Is checking rtype really necessary? Eh, why not..
      type_obj = (PAMI::Type::TypeCode *)in->cmd.xfer_allreduce.rtype;

      /// \todo Support non-contiguous
      PAMI_assert(type_obj->IsContiguous() &&  type_obj->IsPrimitive());

      dataSent = type_obj->GetAtomSize() * in->cmd.xfer_allreduce.rtypecount;
      result.check.range   |= !((dataSent <= T_Range_High) && (dataSent >= T_Range_Low));

      return result;
    }

    // The protocol only supports some dt/op's.  
    // Two functions - one for MU dt/ops, one for current SHMEM dt/ops
    namespace MU
    {
      inline metadata_result_t internal_op_dt_metadata_function(struct pami_xfer_t *in, bool Opt_shmem=false)
      {
        const bool support[PAMI_DT_COUNT][PAMI_OP_COUNT] =
        {
          //  PAMI_COPY,    PAMI_NOOP,  PAMI_MAX, PAMI_MIN, PAMI_SUM, PAMI_PROD, PAMI_LAND*,PAMI_LOR*,PAMI_LXOR*,PAMI_BAND, PAMI_BOR, PAMI_BXOR, PAMI_MAXLOC, PAMI_MINLOC,                              
          // * we could support these with binary AND/OR/XOR IF we preprocess the data into '1' and '0'.
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_BYTE                                     
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_SIGNED_CHAR       
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_SIGNED_SHORT      
          {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//  PAMI_SIGNED_INT        
          {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//  PAMI_SIGNED_LONG       
          {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//  PAMI_SIGNED_LONG_LONG  
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_UNSIGNED_CHAR                             
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_UNSIGNED_SHORT     
          {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//  PAMI_UNSIGNED_INT       
          {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//  PAMI_UNSIGNED_LONG      
          {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//  PAMI_UNSIGNED_LONG_LONG 
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_FLOAT      
          {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//  PAMI_DOUBLE                            
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LONG_DOUBLE       
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOGICAL1
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOGICAL2
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOGICAL4
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOGICAL8
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_SINGLE_COMPLEX       
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_DOUBLE_COMPLEX                       
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOC_2INT       
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOC_2FLOAT                            
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOC_2DOUBLE
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOC_SHORT_INT
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOC_FLOAT_INT
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOC_DOUBLE_INT
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//  PAMI_LOC_LONG_INT
          {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false} //  PAMI_LOC_LONGDOUBLE_INT
        };
        metadata_result_t result = {0};
        uintptr_t op;
        uintptr_t dt;
        TRACE_FN_ENTER();
        if (((uintptr_t)in->cmd.xfer_allreduce.stype >= PAMI::Type::TypeCode::PRIMITIVE_TYPE_COUNT) || ((uintptr_t)in->cmd.xfer_allreduce.op >= PAMI::Type::TypeFunc::PRIMITIVE_FUNC_COUNT))
          result.check.datatype_op = 1; // No user-defined dt/op's

        PAMI::Type::TypeFunc::GetEnums(in->cmd.xfer_allreduce.stype,
                                       in->cmd.xfer_allreduce.op,
                                       dt,
                                       op);
        TRACE_FORMAT("(dt %p/%d,op %p/%d) = %s", in->cmd.xfer_allreduce.stype,(pami_dt)dt, in->cmd.xfer_allreduce.op,(pami_op)op, support[dt][op] ? "true" : "false");
        result.check.datatype_op = support[dt][op]?0:1;
        pami_dt pdt = (pami_dt) dt;
        pami_op pop = (pami_op) op;

        /// \todo TEMP: 64 PPN only supports double's?
        //if((__global.topology_local.size() == 64) && (pdt != PAMI_DOUBLE)) 
        //  result.check.datatype_op = 1;
        //else 

        // Shmem_optimized has range limits on non-DOUBLEs
        if (Opt_shmem && pdt != PAMI_DOUBLE)
        {
          PAMI::Type::TypeCode * type_obj = (PAMI::Type::TypeCode *)in->cmd.xfer_allreduce.stype;
          size_t dataSent = type_obj->GetAtomSize() * in->cmd.xfer_allreduce.stypecount;
          result.check.range   |= !(dataSent <= 8192/__global.topology_local.size());

          type_obj = (PAMI::Type::TypeCode *)in->cmd.xfer_allreduce.rtype;
          dataSent = type_obj->GetAtomSize() * in->cmd.xfer_allreduce.rtypecount;
          result.check.range   |= !(dataSent <= 8192/__global.topology_local.size());
        }
        TRACE_FORMAT("(dt %d,op %d) = op %s, range %s", pdt, pop, result.check.datatype_op ? "true" : "false",result.check.range ? "true" : "false");
        TRACE_FN_EXIT();
        return(result);
      }
      extern inline metadata_result_t op_dt_metadata_function(struct pami_xfer_t *in)
      {
        TRACE_FN_ENTER();
        metadata_result_t result = MU::internal_op_dt_metadata_function(in);
        TRACE_FN_EXIT();
        return(result);
      }
    }

    namespace Shmem_Optimized // ONLY SUPPORTS DOUBLES?!
    {
      extern inline metadata_result_t op_dt_metadata_function(struct pami_xfer_t *in)
      {
        TRACE_FN_ENTER();
        metadata_result_t result = MU::internal_op_dt_metadata_function(in,true /* optimized shmem */);
        TRACE_FN_EXIT();
        return(result);
      }
    }
    namespace Shmem
    {
      extern inline metadata_result_t op_dt_metadata_function(struct pami_xfer_t *in)
      {
        TRACE_FN_ENTER();
        metadata_result_t result = {0};
        uintptr_t op;
        uintptr_t dt;
        if (((uintptr_t)in->cmd.xfer_allreduce.stype >= PAMI::Type::TypeCode::PRIMITIVE_TYPE_COUNT) || ((uintptr_t)in->cmd.xfer_allreduce.op >= PAMI::Type::TypeFunc::PRIMITIVE_FUNC_COUNT))
          result.check.datatype_op = 1; // No user-defined dt/op's

        PAMI::Type::TypeFunc::GetEnums(in->cmd.xfer_allreduce.stype,
                                       in->cmd.xfer_allreduce.op,
                                       dt,
                                       op);
        pami_dt pdt = (pami_dt) dt;
        pami_op pop = (pami_op) op;
	// No MATH_OP_FUNCS() for logical or complex or LOCs?
        result.check.datatype_op = ((pdt != PAMI_LOGICAL1) &&
				    (pdt != PAMI_LOGICAL2) &&
				    (pdt != PAMI_LOGICAL4) &&
				    (pdt != PAMI_LOGICAL8) &&
				    (pdt != PAMI_SINGLE_COMPLEX) && 
				    (pdt != PAMI_DOUBLE_COMPLEX) && 
				    (pop != PAMI_MAXLOC) && 
				    (pop != PAMI_MINLOC) && 
                                    (pop < PAMI_OP_COUNT)) ?0:1;
        TRACE_FORMAT("(dt %d,op %d) = %s", pdt, pop, result.check.datatype_op ? "true" : "false");
        /// \todo TEMP: 64 PPN only supports double's
//        if((__global.topology_local.size() == 64) && (pdt != PAMI_DOUBLE)) 
//          result.check.datatype_op = 1;
//        TRACE_FORMAT("(dt %d,op %d) = %s", pdt, pop, result.check.datatype_op ? "true" : "false");
        TRACE_FN_EXIT();
        return(result);
      }
    }

    //----------------------------------------------------------------------------
    /// Declare our protocol factory templates and their metadata templates
    ///
    /// 'Pure' protocols only work on the specified (Shmem or MU) device.
    ///
    /// 'Composite' protocols combine Shmem/MU devices.
    ///
    //----------------------------------------------------------------------------

    //----------------------------------------------------------------------------
    // 'Pure' Shmem allsided multisync
    //----------------------------------------------------------------------------
    extern inline void ShmemMsyncMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiSync:SHMEM:-");
    }

    typedef CCMI::Adaptor::Barrier::BarrierFactoryAllSidedT
    <CCMI::Adaptor::Barrier::MultiSyncComposite<>,
    ShmemMsyncMetaData,
    CCMI::ConnectionManager::SimpleConnMgr,
    PAMI::Geometry::CKEY_BARRIERCOMPOSITE7>
    ShmemMultiSyncFactory;

    //----------------------------------------------------------------------------
    // Optimized (MU) Binomial barrier
    //----------------------------------------------------------------------------
    extern inline void OptBinomialMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:OptBinomial:P2P:P2P");
      m->check_perf.values.hw_accel     = 1;
    }

    extern inline bool opt_binomial_analyze (PAMI_GEOMETRY_CLASS *geometry)
    {
      return true;
    }

    typedef CCMI::Adaptor::Barrier::BarrierT
    < CCMI::Schedule::TopoMultinomial4,
    //CCMI::Schedule::NodeOptTopoMultinomial,
    opt_binomial_analyze,
    PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX,
    PAMI::Geometry::CKEY_BARRIERCOMPOSITE2>
    OptBinomialBarrier;

    typedef CCMI::Adaptor::Barrier::BarrierFactoryT
    < OptBinomialBarrier,
    OptBinomialMetaData,
    CCMI::ConnectionManager::SimpleConnMgr,
    false,
    PAMI::Geometry::CKEY_BARRIERCOMPOSITE2>
    OptBinomialBarrierFactory;


    extern inline void getAlltoallMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:M2MComposite:MU:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
      m->check_correct.values.global_order = 8 ; /* ordered - single threaded */
    }
    typedef CCMI::Adaptor::All2AllProtocol All2AllProtocol;
    typedef CCMI::Adaptor::All2AllFactoryT <All2AllProtocol, getAlltoallMetaData, CCMI::ConnectionManager::CommSeqConnMgr> All2AllFactory;

    extern inline void getAlltoallvMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:M2MComposite:MU:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
      m->check_correct.values.global_order = 8 ; /* ordered - single threaded */
    }
    typedef CCMI::Adaptor::All2AllvProtocolLong All2AllProtocolv;
    typedef CCMI::Adaptor::All2AllvFactoryT <All2AllProtocolv, getAlltoallvMetaData, CCMI::ConnectionManager::CommSeqConnMgr> All2AllvFactory;

    typedef CCMI::Adaptor::All2AllvProtocolInt All2AllProtocolv_int;
    typedef CCMI::Adaptor::All2AllvFactoryT <All2AllProtocolv_int, getAlltoallvMetaData, CCMI::ConnectionManager::CommSeqConnMgr> All2AllvFactory_int;

    //----------------------------------------------------------------------------
    // 'Pure' Shmem allsided multicombine
    //----------------------------------------------------------------------------
    extern inline void ShmemMcombMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCombine:SHMEM:-");
      m->check_correct.values.alldtop     = 0;
      m->check_correct.values.rangeminmax = 1;
      m->range_hi                         = 64; // Msgs > 64 are pseudo-reduce-only, not allreduce
      m->check_fn                         = range_metadata_function<0,64,Shmem::op_dt_metadata_function>;
    }

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < CCMI::Adaptor::Allreduce::MultiCombineComposite<>,
    ShmemMcombMetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_ALLREDUCE > ShmemMultiCombineFactory;

    //----------------------------------------------------------------------------
    // 'Pure' Shmem allsided multicast
    //----------------------------------------------------------------------------
    extern inline void ShmemMcastMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCast:SHMEM:-");
    }

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < CCMI::Adaptor::Broadcast::MultiCastComposite<>,
    ShmemMcastMetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_BROADCAST > ShmemMultiCastFactory;


    //----------------------------------------------------------------------------
    // 'Pure' MU allsided multicast
    //----------------------------------------------------------------------------
    extern inline void MUMcastMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCast:-:MU");
      m->check_perf.values.hw_accel     = 1;
    }

    // Even though MU MultiCast is allsided, it still needs a register call with a dispatch id,
    // so we use the CollectiveProtocolFactoryT instead of the AllSidedCollectiveProtocolFactoryT
    typedef CCMI::Adaptor::CollectiveProtocolFactoryT < CCMI::Adaptor::Broadcast::MultiCastComposite<>,
    MUMcastMetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_BROADCAST > MUMultiCastFactory;


    //----------------------------------------------------------------------------
    // 'Pure' MU allsided multisync
    //----------------------------------------------------------------------------
    extern inline void GIMsyncMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiSync:-:GI");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.global_order = 4 ; /* ordered for GI/single threaded */
    }

    typedef CCMI::Adaptor::Barrier::BarrierFactoryAllSidedT
    <CCMI::Adaptor::Barrier::MultiSyncComposite<true, MUGlobalDputNI>,
    GIMsyncMetaData,
    CCMI::ConnectionManager::SimpleConnMgr,
    PAMI::Geometry::CKEY_BARRIERCOMPOSITE3>
    GIMultiSyncFactory;

    //----------------------------------------------------------------------------
    // MU rectangle multisync
    //----------------------------------------------------------------------------
    extern inline void MURectangleMsyncMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:RectangleMultiSync:-:MU");
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    typedef CCMI::Adaptor::Barrier::BarrierFactoryAllSidedT
    <CCMI::Adaptor::Barrier::MultiSyncComposite<true, MUDputNI,PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX, PAMI::Geometry::CKEY_RECTANGLE_CLASSROUTEID>,
    MURectangleMsyncMetaData,
    CCMI::ConnectionManager::SimpleConnMgr,
    PAMI::Geometry::CKEY_BARRIERCOMPOSITE5>
    MURectangleMultiSyncFactory;

    //----------------------------------------------------------------------------
    // 'Pure' MU allsided multicombine
    //----------------------------------------------------------------------------
    extern inline void MUMcombMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCombine:-:MU");
      m->check_correct.values.alldtop   = 0;
      m->check_fn                       = MU::op_dt_metadata_function;
      m->check_perf.values.hw_accel     = 1;
    }

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < CCMI::Adaptor::Allreduce::MultiCombineComposite<>,
    MUMcombMetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_ALLREDUCE > MUMultiCombineFactory;

    //----------------------------------------------------------------------------
    // 'Pure' SHMEM/MU allsided dput multicast over multicombine OR
    //----------------------------------------------------------------------------
    extern inline void MUMcastCollectiveDputMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCastDput:-:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    extern inline void MUMcastShmemCollectiveDputMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCastDput:SHMEM:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < 
    CCMI::Adaptor::Broadcast::MultiCastComposite<true, 
    MUGlobalDputNI,
    PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID>, //Mcast over Mcomb (key)
    MUMcastCollectiveDputMetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_BROADCAST > MUCollectiveDputMultiCastFactory;

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < 
    CCMI::Adaptor::Broadcast::MultiCastComposite< true, 
    MUShmemGlobalDputNI,
    PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID>, //Mcast over Mcomb (key)
    MUMcastShmemCollectiveDputMetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_BROADCAST > MUShmemCollectiveDputMultiCastFactory;

    struct  MUCollDputMcastStorage
    {
      union
      {
        char dput  [sizeof(MUCollectiveDputMultiCastFactory)];
        char shmemdput [sizeof(MUShmemCollectiveDputMultiCastFactory)];
      };
    };

    //----------------------------------------------------------------------------
    // 'Pure' MU allsided dput multicombine
    //----------------------------------------------------------------------------
    extern inline void MUMcombCollectiveDputMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCombineDput:-:MU");
      m->check_correct.values.alldtop   = 0;
      m->check_fn                       = MU::op_dt_metadata_function;
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < CCMI::Adaptor::Allreduce::MultiCombineComposite<true, MUGlobalDputNI>,
    MUMcombCollectiveDputMetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_ALLREDUCE > MUCollectiveDputMulticombineFactory;

    //----------------------------------------------------------------------------
    // MU+Shmem allsided dput multicombine
    //----------------------------------------------------------------------------
    extern inline void MUShmemMcombCollectiveDputMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCombineDput:SHMEM:MU");
      m->check_correct.values.alldtop       = 0;
      m->check_correct.values.checkrequired = 1; // Must calculate range in the check_fn
      m->check_fn                           = Shmem_Optimized::op_dt_metadata_function; 
      m->check_correct.values.rangeminmax   = 1;
//    m->range_hi                           = 8192;// Non-doubles have a calculated range so can't set constant
      m->check_perf.values.hw_accel         = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < CCMI::Adaptor::Allreduce::MultiCombineComposite<true, MUShmemGlobalDputNI>,
    MUShmemMcombCollectiveDputMetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_ALLREDUCE > MUShmemCollectiveDputMulticombineFactory;

#ifdef PAMI_ENABLE_X0_PROTOCOLS
    //----------------------------------------------------------------------------
    // MU allsided multicast built on multicombine (BOR)
    //----------------------------------------------------------------------------
    extern inline void MUMcast3MetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("X0:MultiCast_MultiCombine:-:MU");
      m->check_correct.values.rangeminmax = 1;
      m->range_hi = 1024*1024; /// \todo arbitrary hack for now - it core dumps > 1M
      m->check_perf.values.hw_accel     = 1;

    }

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < CCMI::Adaptor::Broadcast::MultiCastComposite3,
    MUMcast3MetaData,
    CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_BROADCAST > MUMultiCast3Factory;
#endif
    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU allsided 2 device multisync
    //----------------------------------------------------------------------------
    extern inline void Msync2DMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiSync2Device:SHMEM:MU");
      m->check_perf.values.hw_accel     = 1;
    }

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryNCOT < CCMI::Adaptor::Barrier::MultiSyncComposite2Device<>,
    Msync2DMetaData, CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_BARRIER>  MultiSync2DeviceFactory;

    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU allsided 2 device multisync
    //----------------------------------------------------------------------------
    extern inline void Msync2DGIShmemMetaData(pami_metadata_t *m)
    {
      // Apparently MU dput actually uses GI for msync
      new(m) PAMI::Geometry::Metadata("I0:MultiSync2Device:SHMEM:GI");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.global_order = 4 ; /* ordered for GI/single threaded */
    }

    typedef CCMI::Adaptor::Barrier::BarrierFactoryAllSidedT
    <CCMI::Adaptor::Barrier::MultiSyncComposite2Device<>,
    Msync2DGIShmemMetaData,
    CCMI::ConnectionManager::SimpleConnMgr,
    PAMI::Geometry::CKEY_BARRIERCOMPOSITE4>
    MultiSync2DeviceGIShmemFactory;

    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU allsided 2 device multisync
    //----------------------------------------------------------------------------
    extern inline void Msync2DRectangleDputMetaData(pami_metadata_t *m)
    {
      // Apparently MU dput actually uses GI for msync
      new(m) PAMI::Geometry::Metadata("I0:RectangleMultiSync2Device:SHMEM:MU");
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    typedef CCMI::Adaptor::Barrier::BarrierFactoryAllSidedT
    <CCMI::Adaptor::Barrier::MultiSyncComposite2Device<PAMI::Geometry::CKEY_RECTANGLE_CLASSROUTEID>,
    Msync2DRectangleDputMetaData,
    CCMI::ConnectionManager::SimpleConnMgr,
    PAMI::Geometry::CKEY_BARRIERCOMPOSITE6>
    MultiSync2DeviceRectangleFactory;


#ifdef PAMI_ENABLE_X0_PROTOCOLS
    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU allsided 2 device multicast
    //----------------------------------------------------------------------------
    extern inline void Mcast2DMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("X0:MultiCast2Device:SHMEM:MU");
      m->check_perf.values.hw_accel     = 1;
    }
    typedef CCMI::Adaptor::Broadcast::MultiCastComposite2DeviceFactoryT < CCMI::Adaptor::Broadcast::MultiCastComposite2Device<PAMI_GEOMETRY_CLASS, true, false>,
    Mcast2DMetaData, CCMI::ConnectionManager::SimpleConnMgr > MultiCast2DeviceFactory;
#endif
    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU-DPUT allsided 2 device multicast
    //----------------------------------------------------------------------------
    extern inline void Mcast2DDputMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCast2DeviceDput:SHMEM:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }
    typedef CCMI::Adaptor::Broadcast::MultiCastComposite2DeviceFactoryT < CCMI::Adaptor::Broadcast::MultiCastComposite2DeviceAS<PAMI_GEOMETRY_CLASS>,
    Mcast2DDputMetaData, CCMI::ConnectionManager::SimpleConnMgr > MultiCast2DeviceDputFactory;

#ifdef PAMI_ENABLE_X0_PROTOCOLS
    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU allsided 2 device multicombine
    //----------------------------------------------------------------------------
    extern inline void Mcomb2DMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("X0:MultiCombine2Device:SHMEM:MU");
      m->check_correct.values.alldtop   = 0;
      m->check_fn                       = Shmem::op_dt_metadata_function;
      m->check_perf.values.hw_accel     = 1;
    }
    typedef CCMI::Adaptor::Allreduce::MultiCombineComposite2DeviceFactoryT < CCMI::Adaptor::Allreduce::MultiCombineComposite2Device<0>,
    Mcomb2DMetaData, CCMI::ConnectionManager::SimpleConnMgr >    MultiCombine2DeviceFactory;
#endif
    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU-DPUT allsided 2 device multicombine
    //----------------------------------------------------------------------------
    extern inline void Mcomb2DDputMetaData(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:MultiCombine2DeviceDput:SHMEM:MU");
      m->check_correct.values.alldtop       = 0;
      m->check_correct.values.sendminalign  = 1;
      m->check_correct.values.recvminalign  = 1;
      m->check_correct.values.nonlocal      = 1;
      m->check_correct.values.inplace       = 0;
      m->send_min_align                     = 32;
      m->recv_min_align                     = 32;
      m->check_fn                           = align_metadata_function<1,32,1,32,MU::op_dt_metadata_function>;
      m->check_perf.values.hw_accel         = 1;
    }
    typedef CCMI::Adaptor::Allreduce::MultiCombineComposite2DeviceFactoryT < CCMI::Adaptor::Allreduce::MultiCombineComposite2Device<0>,
    Mcomb2DDputMetaData, CCMI::ConnectionManager::SimpleConnMgr >    MultiCombine2DeviceDputFactory;


#ifdef PAMI_ENABLE_X0_PROTOCOLS
    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU allsided 2 device multicombine with no pipelining
    //----------------------------------------------------------------------------
    extern inline void Mcomb2DMetaDataNP(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("X0:MultiCombine2DeviceNP:SHMEM:MU");
      m->check_correct.values.alldtop       = 0;
      m->check_fn                           = Shmem::op_dt_metadata_function;
      m->check_perf.values.hw_accel         = 1;
    }
    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < CCMI::Adaptor::Allreduce::MultiCombineComposite2DeviceNP,
    Mcomb2DMetaDataNP, CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_ALLREDUCE > MultiCombine2DeviceFactoryNP;
#endif
#ifdef PAMI_ENABLE_X0_PROTOCOLS
    //----------------------------------------------------------------------------
    // 'Composite' Shmem/MU-DPUT allsided 2 device multicombine with no pipelining
    //----------------------------------------------------------------------------
    extern inline void Mcomb2DDputMetaDataNP(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("X0:MultiCombine2DeviceDputNP:SHMEM:MU");
      m->check_correct.values.alldtop       = 0;
      m->check_fn                           = Shmem::op_dt_metadata_function;
      m->check_perf.values.hw_accel         = 1;
    }
    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT < CCMI::Adaptor::Allreduce::MultiCombineComposite2DeviceNP,
    Mcomb2DDputMetaDataNP, CCMI::ConnectionManager::SimpleConnMgr, PAMI_XFER_ALLREDUCE > MultiCombine2DeviceDputFactoryNP;
#endif
    //----------------------------------------------------------------------------
    // Rectangle broadcast
    //----------------------------------------------------------------------------
    extern inline void get_colors (PAMI::Topology             * t,
                                   unsigned                    bytes,
                                   unsigned                  * colors,
                                   unsigned                  & ncolors)
    {
      TRACE_FN_ENTER();
      ncolors = 1;
      colors[0] = CCMI::Schedule::TorusRect::NO_COLOR;
      TRACE_FN_EXIT();
    }

    extern inline void get_rect_colors (PAMI::Topology             * t,
                                        unsigned                    bytes,
                                        unsigned                  * colors,
                                        unsigned                  & ncolors)
    {
      TRACE_FN_ENTER();
      unsigned max = 0, ideal = 0;
      unsigned _colors[10];
      CCMI::Schedule::TorusRect::getColors (t, ideal, max, _colors);
      TRACE_FORMAT("bytes %u, ncolors %u, ideal %u, max %u", bytes, ncolors, ideal, max);

      if (bytes <= 4096) //16 packets
        ideal = 1;
      else if (bytes <= 16384 && ideal >= 2)
        ideal = 2;
      else if (bytes <= 65536 && ideal >= 3)
        ideal = 3;
      else if (bytes <= 262144 && ideal >= 5)
        ideal = 5;
      else if (bytes <= 1048576 && ideal >= 7)
        ideal = 7;

      if (ideal < ncolors)
        ncolors = ideal;  //Reduce the number of colors to the relavant colors

      TRACE_FORMAT("ncolors %u, ideal %u", ncolors, ideal);
      memcpy (colors, _colors, ncolors * sizeof(int));
      TRACE_FN_EXIT();
    }

    extern inline void get_rect_ared_colors (PAMI::Topology         * t,
					     unsigned                bytes,
					     unsigned              * colors,
					     unsigned              & ncolors)
    {
      TRACE_FN_ENTER();
      unsigned max = 0, ideal = 0;
      unsigned _colors[10];
      CCMI::Schedule::TorusRect::getColors (t, ideal, max, _colors);
      TRACE_FORMAT("bytes %u, ncolors %u, ideal %u, max %u", bytes, ncolors, ideal, max);

      if (bytes <= 4096) //16 packets
        ideal = 1;
      else if (bytes <= 16384 && ideal >= 2)
	ideal = 2;
      else if (bytes <= 65536 && ideal >= 3)
        ideal = 3;
      else if (bytes <= 262144 && ideal >= 5)
        ideal = 5;
      else if (bytes <= 524288 && ideal >= 6)
        ideal = 6;
      else if (bytes <= 1048576 && ideal >= 9)
	ideal = 9;

      if (__global.mapping.tSize() == 8 && ideal >= 8)
	ideal = 8;

      if (ideal < ncolors)
        ncolors = ideal;  //Reduce the number of colors to the relavant colors

      TRACE_FORMAT("ncolors %u, ideal %u", ncolors, ideal);
      memcpy (colors, _colors, ncolors * sizeof(int));
      TRACE_FN_EXIT();
    }

    extern inline void rectangle_dput_1color_broadcast_metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:RectangleDput1Color:SHMEM:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
    }

    extern inline void mu_rectangle_dput_1color_broadcast_metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:RectangleDput1Color:MU:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
    }

    typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
    < 1,
    CCMI::Schedule::TorusRect,
    CCMI::ConnectionManager::ColorConnMgr,
    get_rect_colors,
    PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX >
    RectangleDput1ColorBroadcastComposite;

    // The only difference between RectangleDput1ColorBroadcastFactory and MURectangleDput1ColorBroadcastFactory
    // is the metadata. Clumsy way to differentiate shmem+mu from mu-only.
    // The real differentiator is which NI gets used, but metadata doesn't know that.
    typedef CCMI::Adaptor::CachedAllSidedFactoryT
    < RectangleDput1ColorBroadcastComposite,
    rectangle_dput_1color_broadcast_metadata,
    CCMI::ConnectionManager::ColorConnMgr,
    PAMI::Geometry::CKEY_BCASTCOMPOSITE2,
    PAMI_XFER_BROADCAST >
    RectangleDput1ColorBroadcastFactory;

    typedef CCMI::Adaptor::CachedAllSidedFactoryT
    < RectangleDput1ColorBroadcastComposite,
    mu_rectangle_dput_1color_broadcast_metadata,
    CCMI::ConnectionManager::ColorConnMgr,
    PAMI::Geometry::CKEY_BCASTCOMPOSITE3,
    PAMI_XFER_BROADCAST >
    MURectangleDput1ColorBroadcastFactory;
    
    extern inline void rectangle_dput_broadcast_metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:RectangleDput:SHMEM:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    extern inline void mu_rectangle_dput_broadcast_metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:RectangleDput:MU:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    typedef CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
    < 10,
    CCMI::Schedule::TorusRect,
    CCMI::ConnectionManager::ColorConnMgr,
    get_rect_colors,
    PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX >
    RectangleDputBroadcastComposite;

    // The only difference between RectangleDputBroadcastFactory and MURectangleDputBroadcastFactory 
    // is the metadata. Clumsy way to differentiate shmem+mu from mu-only.
    // The real differentiator is which NI gets used, but metadata doesn't know that.
    typedef CCMI::Adaptor::CachedAllSidedFactoryT
    < RectangleDputBroadcastComposite,
    rectangle_dput_broadcast_metadata,
    CCMI::ConnectionManager::ColorConnMgr,
    PAMI::Geometry::CKEY_BCASTCOMPOSITE0,
    PAMI_XFER_BROADCAST >
    RectangleDputBroadcastFactory;

    typedef CCMI::Adaptor::CachedAllSidedFactoryT
    < RectangleDputBroadcastComposite,
    mu_rectangle_dput_broadcast_metadata,
    CCMI::ConnectionManager::ColorConnMgr,
    PAMI::Geometry::CKEY_BCASTCOMPOSITE1,
    PAMI_XFER_BROADCAST >
    MURectangleDputBroadcastFactory;

    extern inline void rectangle_dput_allgather_metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("I0:RectangleDput:SHMEM:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }

    typedef CCMI::Adaptor::Allgather::AllgatherOnBroadcastT < 10, 10,
    CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
    < 10,
    CCMI::Schedule::TorusRect,
    CCMI::ConnectionManager::ColorMapConnMgr,
    get_rect_colors,
    PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX > ,
    CCMI::ConnectionManager::ColorMapConnMgr,
    PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX,
    false, false> RectangleDputAllgather;

    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT
    < RectangleDputAllgather,
    rectangle_dput_allgather_metadata,
    CCMI::ConnectionManager::ColorMapConnMgr,
    PAMI_XFER_ALLGATHER >
    RectangleDputAllgatherFactory;

    typedef CCMI::Adaptor::Allgather::AllgatherOnBroadcastT < 10, 10,
    CCMI::Adaptor::Broadcast::BcastMultiColorCompositeT
    < 10,
    CCMI::Schedule::TorusRect,
    CCMI::ConnectionManager::ColorMapConnMgr,
    get_rect_colors,
    PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX > ,
    CCMI::ConnectionManager::ColorMapConnMgr,
    PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX,
    true, true> RectangleDputAllgatherV;

    extern inline void rectangle_dput_allgatherv_metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("X0:RectangleDput:SHMEM:MU");
      m->check_perf.values.hw_accel     = 1;
      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
      m->check_correct.values.global_order = 2 ; /* ordered for MUI/single threaded */
    }
    typedef CCMI::Adaptor::AllSidedCollectiveProtocolFactoryT
    < RectangleDputAllgatherV,
    rectangle_dput_allgatherv_metadata,
    CCMI::ConnectionManager::ColorMapConnMgr,
    PAMI_XFER_ALLGATHERV_INT >
    RectangleDputAllgatherVFactory;

    //--------------------------------------------------
    //-------- Rectangle Allreduce ---------------------
    //--------------------------------------------------

    extern inline void mu_rectangle_allreduce_metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("X0:Rectangle:MU:MU");
    }
        
    extern inline void rectangle_allreduce_metadata(pami_metadata_t *m)
    {
      new(m) PAMI::Geometry::Metadata("X0:Rectangle:SHMEM:MU");
    }

    typedef CCMI::Adaptor::Allreduce::ThreadedMultiColorCompositeT
      < 10, CCMI::Executor::PipelinedAllreduce<CCMI::ConnectionManager::TorusConnMgr>,
      CCMI::Schedule::TorusRect,
      CCMI::ConnectionManager::TorusConnMgr,
      get_rect_ared_colors,
      PAMI::Mutex::BGQ::IndirectL2,
      1,
      PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX> 
      RectangleDputAllreduceComposite;
    
    typedef CCMI::Adaptor::Allreduce::ProtocolFactoryT
      < RectangleDputAllreduceComposite,
      rectangle_allreduce_metadata,
      CCMI::ConnectionManager::TorusConnMgr>
      RectangleDputAllreduceFactory;

    typedef CCMI::Adaptor::Allreduce::ProtocolFactoryT
      < RectangleDputAllreduceComposite,
      mu_rectangle_allreduce_metadata,
      CCMI::ConnectionManager::TorusConnMgr>
      MURectangleDputAllreduceFactory;

    //----------------------------------------------------------------------------
    /// \brief The BGQ Multi* registration class for Shmem and MU.
    //----------------------------------------------------------------------------
    template <class T_Geometry, class T_ShmemDevice, class T_ShmemNativeInterface, class T_MUDevice, class T_MUNativeInterface, class T_AxialDputNativeInterface, class T_AxialShmemDputNativeInterface, class T_Allocator, class T_BigAllocator>
    class BGQMultiRegistration :
    public CollRegistration<PAMI::CollRegistration::BGQMultiRegistration<T_Geometry, T_ShmemDevice, T_ShmemNativeInterface, T_MUDevice, T_MUNativeInterface, T_AxialDputNativeInterface, T_AxialShmemDputNativeInterface, T_Allocator, T_BigAllocator>, T_Geometry>
    {
      template<class T_Factory>
      class GeometryInfo
      {
      public:
        GeometryInfo(pami_context_t              context,
                     CCMI::Executor::Composite  *composite,
                     void                       *allocator):
        _context(context),
        _composite(composite),
        _allocator(allocator)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p> %p, %p, %p",this,_context,_composite,_allocator);
          DO_DEBUG((templateName<T_Factory>()));
          TRACE_FN_EXIT();
        }
        virtual ~GeometryInfo()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p> %p, %p, %p",this,_context,_composite,_allocator);
          DO_DEBUG((templateName<T_Factory>()));
          T_Factory::cleanup_done_fn(_context, _composite, PAMI_SUCCESS);
//            _allocator->returnObject(this); // ?!
          TRACE_FN_EXIT();
        }
        pami_context_t                _context;
        CCMI::Executor::Composite    *_composite;
        void                         *_allocator;
      };

    public:
      inline BGQMultiRegistration(T_ShmemNativeInterface              *shmem_ni,
                                  T_ShmemDevice                       &shmem_device,
                                  T_MUDevice                          &mu_device,
                                  pami_client_t                        client,
                                  pami_context_t                       context,
                                  size_t                               context_id,
                                  size_t                               client_id,
                                  int                                 *dispatch_id,
                                  std::map<unsigned, pami_geometry_t> *geometry_map,
                                  T_Allocator                          &allocator,
                                  T_BigAllocator                       &big_allocator):
      CollRegistration<PAMI::CollRegistration::BGQMultiRegistration<T_Geometry, T_ShmemDevice, T_ShmemNativeInterface, T_MUDevice, T_MUNativeInterface, T_AxialDputNativeInterface, T_AxialShmemDputNativeInterface, T_Allocator, T_BigAllocator>, T_Geometry> (),
      _client(client),
      _context(context),
      _context_id(context_id),
      _dispatch_id(dispatch_id),
      _geometry_map(geometry_map),
      _allocator (allocator),
      _big_allocator (big_allocator),
      _sconnmgr(65535),
      _csconnmgr(),
      _cg_connmgr(65535),
      _color_connmgr(),
      _color_map_connmgr(),
      _torus_connmgr(),
      _shmem_device(shmem_device),
      _shmem_ni(shmem_ni),
      _shmem_msync_factory(_context,_context_id,mapidtogeometry,&_sconnmgr, _shmem_ni),
      _shmem_mcast_factory(_context,_context_id,mapidtogeometry,&_sconnmgr, _shmem_ni),
      _shmem_mcomb_factory(_context,_context_id,mapidtogeometry,&_sconnmgr, _shmem_ni),
      _mu_device(mu_device),
      _mu_ni_msync(NULL),
      _mu_ni_mcomb(NULL),
      _mu_ni_mcast(NULL),
      _mu_ni_mcast3(NULL),
      _mu_ni_msync2d(NULL),
      _mu_ni_mcast2d(NULL),
      _mu_ni_mcomb2d(NULL),
      _mu_ni_mcomb2dNP(NULL),
      _axial_mu_dput_ni(NULL),
      //_axial_dput_mu_1_ni(NULL),
      _gi_msync_factory(NULL),
      _mu_mcast_factory(NULL),
#ifdef PAMI_ENABLE_X0_PROTOCOLS
      _mu_mcast3_factory(NULL),
#endif
      _mu_mcomb_factory(NULL),
      _msync2d_composite_factory(NULL),
      _msync2d_gishm_composite_factory(NULL),
#ifdef PAMI_ENABLE_X0_PROTOCOLS
      _mcast2d_composite_factory(NULL),
      _mcomb2d_composite_factory(NULL),
      _mcomb2dNP_composite_factory(NULL),
#endif
      _mcomb2d_dput_composite_factory(NULL),
#ifdef PAMI_ENABLE_X0_PROTOCOLS
      _mcomb2dNP_dput_composite_factory(NULL),
#endif
      _mu_rectangle_1color_dput_broadcast_factory(NULL),
      _mu_rectangle_dput_broadcast_factory(NULL),
      _shmem_mu_rectangle_1color_dput_broadcast_factory(NULL),
      _shmem_mu_rectangle_dput_broadcast_factory(NULL),
      _rectangle_dput_allgather_factory(NULL),      
      _rectangle_dput_allgatherv_factory(NULL),
      _rectangle_dput_allreduce_factory(NULL),
      _mu_rectangle_dput_allreduce_factory(NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>", this);
        //DO_DEBUG((templateName<T_Geometry>()));
        //DO_DEBUG((templateName<T_ShmemNativeInterface>()));
        //DO_DEBUG((templateName<T_MUDevice>()));
        //DO_DEBUG((templateName<T_MUNativeInterface>()));
        //DO_DEBUG((templateName<T_AxialDputNativeInterface>()));

	_torus_connmgr.setComm2Topo(context, bgq_comm2topo);

        //set the mapid functions
        if (__global.useshmem())// && (__global.topology_local.size() > 1))
        {
          TRACE_FORMAT("<%p> useshmem", this);
        }

        if (__global.useMU())
        {
          TRACE_FORMAT("<%p> usemu", this);

          _mu_ni_msync          = new (_mu_ni_msync_storage         ) T_MUNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);
          _mu_ni_mcomb          = new (_mu_ni_mcomb_storage         ) T_MUNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);
          _mu_ni_mcast          = new (_mu_ni_mcast_storage         ) T_MUNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);
          _mu_ni_mcast3         = new (_mu_ni_mcast3_storage        ) T_MUNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);
          _mu_ni_msync2d        = new (_mu_ni_msync2d_storage       ) T_MUNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);
          _mu_ni_mcast2d        = new (_mu_ni_mcast2d_storage       ) T_MUNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);
          _mu_ni_mcomb2d        = new (_mu_ni_mcomb2d_storage       ) T_MUNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);
          _mu_ni_mcomb2dNP      = new (_mu_ni_mcomb2dNP_storage     ) T_MUNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);

          _axial_mu_dput_ni     = new (_axial_mu_dput_ni_storage    ) T_AxialDputNativeInterface(_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);

          if (_axial_mu_dput_ni->status() != PAMI_SUCCESS) _axial_mu_dput_ni = NULL; // Not enough resources?

          if (__global.mapping.t() == 0)
            //We can now construct this on any process (as long as
            //process 0 on each node in the job also calls it
            _mu_global_dput_ni    = new (_mu_global_dput_ni_storage) MUGlobalDputNI (_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);

          _mu_shmem_global_dput_ni    = new (_mu_shmem_global_dput_ni_storage) MUShmemGlobalDputNI (_mu_device, client, context, context_id, client_id, _dispatch_id);

          _mu_ammulticast_ni    = new (_mu_ammulticast_ni_storage) MUAMMulticastNI (_mu_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);

          _mu_m2m_single_ni      = new (_mu_m2m_single_ni_storage) M2MNISingle (_mu_device, client, context, context_id, client_id, _dispatch_id);

          _mu_m2m_vector_long_ni = new (_mu_m2m_vector_long_ni_storage) M2MNIVectorLong (_mu_device, client, context, context_id, client_id, _dispatch_id);

          _mu_m2m_vector_int_ni = new (_mu_m2m_vector_int_ni_storage) M2MNIVectorInt (_mu_device, client, context, context_id, client_id, _dispatch_id);


          if (_mu_ammulticast_ni->status() != PAMI_SUCCESS) _mu_ammulticast_ni = NULL;

          if (_mu_m2m_single_ni->status() != PAMI_SUCCESS)      _mu_m2m_single_ni = NULL;
          if (_mu_m2m_vector_long_ni->status() != PAMI_SUCCESS) _mu_m2m_vector_long_ni = NULL;
          if (_mu_m2m_vector_int_ni->status() != PAMI_SUCCESS) _mu_m2m_vector_int_ni = NULL;

          if (__global.useshmem() && __global.mapping.tSize() < 64)
          {
            _axial_shmem_mu_dput_ni     = new (_axial_shmem_mu_dput_ni_storage    ) T_AxialShmemDputNativeInterface(_mu_device, _shmem_device, _big_allocator, client, context, context_id, client_id, _dispatch_id);

            if (_axial_shmem_mu_dput_ni->status() != PAMI_SUCCESS) _axial_shmem_mu_dput_ni = NULL; // Not enough resources?
          }
          else
            _axial_shmem_mu_dput_ni = NULL;

          _gi_msync_factory = NULL;
          if (_mu_global_dput_ni)
            _gi_msync_factory     = new (_gi_msync_factory_storage    ) GIMultiSyncFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _mu_global_dput_ni);
          _mu_mcomb_factory     = new (_mu_mcomb_factory_storage    ) MUMultiCombineFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _mu_ni_mcomb);

          _mu_rectangle_msync_factory = NULL;
          if (_axial_mu_dput_ni)
            _mu_rectangle_msync_factory = new (_mu_rectangle_msync_factory_storage) MURectangleMultiSyncFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _axial_mu_dput_ni);

          _binomial_barrier_factory = NULL;
          if (_mu_ammulticast_ni)
          {
            _binomial_barrier_factory = new (_binomial_barrier_factory_storage)  OptBinomialBarrierFactory (_context,_context_id,mapidtogeometry,&_sconnmgr, _mu_ammulticast_ni, OptBinomialBarrierFactory::cb_head);
          }

          _alltoall_factory = NULL;
          if (_mu_m2m_single_ni)
          {
            _alltoall_factory = new (_alltoall_factory_storage) All2AllFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, _mu_m2m_single_ni);
          }

          _alltoallv_factory = NULL;
          if (_mu_m2m_vector_long_ni)
          {
            _alltoallv_factory = new (_alltoallv_factory_storage) All2AllvFactory(_context,_context_id,mapidtogeometry,&_csconnmgr, _mu_m2m_vector_long_ni);
          }

          _alltoallv_int_factory = NULL;
          if (_mu_m2m_vector_int_ni)
          {
            _alltoallv_int_factory = new (_alltoallv_int_factory_storage) All2AllvFactory_int(_context,_context_id,mapidtogeometry,&_csconnmgr, _mu_m2m_vector_int_ni);
          }

          _mucollectivedputmulticastfactory         = NULL;  
          _mucollectivedputmulticombinefactory      = NULL; 
          _mushmemcollectivedputmulticastfactory    = NULL;  
          _mushmemcollectivedputmulticombinefactory = NULL; 
          if (__global.mapping.tSize() == 1)
          {
            _mucollectivedputmulticastfactory    = new (_mucolldputmcast_storage.dput ) MUCollectiveDputMultiCastFactory(_context, _context_id, mapidtogeometry, &_sconnmgr, _mu_global_dput_ni);

            _mucollectivedputmulticombinefactory    = new (_mucollectivedputmulticombinestorage ) MUCollectiveDputMulticombineFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _mu_global_dput_ni);
          }
          else
          {
            _mushmemcollectivedputmulticastfactory    = new (_mucolldputmcast_storage.shmemdput ) MUShmemCollectiveDputMultiCastFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _mu_shmem_global_dput_ni);

            _mushmemcollectivedputmulticombinefactory    = new (_mushmemcollectivedputmulticombinestorage ) MUShmemCollectiveDputMulticombineFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _mu_shmem_global_dput_ni);
          }

          TRACE_FORMAT("<%p> _axial_shmem_mu_dput_ni %p", this,_axial_shmem_mu_dput_ni);
          if (_axial_shmem_mu_dput_ni)
          {
            TRACE_FORMAT("<%p>  RectangleDput1ColorBroadcastFactory", this);
            _shmem_mu_rectangle_1color_dput_broadcast_factory = new (_shmem_mu_rectangle_1color_dput_broadcast_factory_storage) RectangleDput1ColorBroadcastFactory(_context,_context_id,mapidtogeometry,&_color_connmgr, _axial_shmem_mu_dput_ni);

            TRACE_FORMAT("<%p>  RectangleDputBroadcastFactory", this);
            _shmem_mu_rectangle_dput_broadcast_factory = new (_shmem_mu_rectangle_dput_broadcast_factory_storage) RectangleDputBroadcastFactory(_context,_context_id,mapidtogeometry,&_color_connmgr, _axial_shmem_mu_dput_ni);

            _rectangle_dput_allgather_factory = new (_rectangle_dput_allgather_factory_storage) RectangleDputAllgatherFactory(_context,_context_id,mapidtogeometry,&_color_map_connmgr, _axial_shmem_mu_dput_ni);

            _rectangle_dput_allgatherv_factory = new (_rectangle_dput_allgatherv_factory_storage) RectangleDputAllgatherVFactory(_context,_context_id,mapidtogeometry,&_color_map_connmgr, _axial_shmem_mu_dput_ni);

	    //Not enough bats at ppn32/64
	    if (__global.mapping.tSize() <= 16)
	      _rectangle_dput_allreduce_factory = new (_rectangle_dput_allreduce_factory_storage) RectangleDputAllreduceFactory(_context, _context_id, mapidtogeometry, &_torus_connmgr, _axial_shmem_mu_dput_ni);
          }

          if (_axial_mu_dput_ni)
          {
            TRACE_FORMAT("<%p> MURectangleDput1ColorBroadcastFactory", this);
            _mu_rectangle_1color_dput_broadcast_factory = new (_mu_rectangle_1color_dput_broadcast_factory_storage) MURectangleDput1ColorBroadcastFactory(_context,_context_id,mapidtogeometry,&_color_connmgr, _axial_mu_dput_ni);

            TRACE_FORMAT("<%p>  MURectangleDputBroadcastFactory", this);
            _mu_rectangle_dput_broadcast_factory = new (_mu_rectangle_dput_broadcast_factory_storage) MURectangleDputBroadcastFactory(_context,_context_id,mapidtogeometry,&_color_connmgr, _axial_mu_dput_ni);

            if (!_rectangle_dput_allgather_factory)
              _rectangle_dput_allgather_factory = new (_rectangle_dput_allgather_factory_storage) RectangleDputAllgatherFactory(_context,_context_id,mapidtogeometry,&_color_map_connmgr, _axial_mu_dput_ni);

	    if (__global.mapping.tSize() == 1)
	      _mu_rectangle_dput_allreduce_factory = new (_mu_rectangle_dput_allreduce_factory_storage) MURectangleDputAllreduceFactory(_context, _context_id, mapidtogeometry, &_torus_connmgr, _axial_mu_dput_ni);

	    if (!_rectangle_dput_allgatherv_factory)
	      _rectangle_dput_allgatherv_factory = new (_rectangle_dput_allgatherv_factory_storage) RectangleDputAllgatherVFactory(_context,_context_id,mapidtogeometry,&_color_map_connmgr, _axial_mu_dput_ni);
          }

          // Can't be ctor'd unless the NI was created
          _mu_mcast_factory  = new (_mu_mcast_factory_storage ) MUMultiCastFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _mu_ni_mcast);
#ifdef PAMI_ENABLE_X0_PROTOCOLS
          _mu_mcast3_factory = new (_mu_mcast3_factory_storage) MUMultiCast3Factory(_context,_context_id,mapidtogeometry,&_sconnmgr, _mu_ni_mcast3);
#endif
        }

//          if ((__global.useMU()) && (__global.useshmem()))
        {
          pami_endpoint_t me_ep = PAMI_ENDPOINT_INIT(client_id, __global.mapping.task(),_context_id);
          _ni_array[0] = _shmem_ni;
          _ni_array[1] = _mu_ni_msync2d;
          _ni_array[2] = _shmem_ni;
          _ni_array[3] = _mu_ni_mcomb2dNP;
          _msync2d_composite_factory = new (_msync2d_composite_factory_storage) MultiSync2DeviceFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, &_ni_array[0]);

          _ni_array[4] = _shmem_ni;
          _ni_array[5] = _mu_global_dput_ni;
          _msync2d_gishm_composite_factory = new (_msync2d_gishm_composite_factory_storage) MultiSync2DeviceGIShmemFactory(_context, _context_id,mapidtogeometry,&_sconnmgr, &_ni_array[4]);

          _ni_array[6] = _shmem_ni;
          _ni_array[7] = _axial_mu_dput_ni;
          _msync2d_rectangle_composite_factory = new (_msync2d_rectangle_composite_factory_storage) MultiSync2DeviceRectangleFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, &_ni_array[6]);

          _mcomb2d_dput_composite_factory = new (_mcomb2d_dput_composite_factory_storage) MultiCombine2DeviceDputFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _shmem_ni, _mu_global_dput_ni, me_ep);
#ifdef PAMI_ENABLE_X0_PROTOCOLS
          _mcomb2dNP_dput_composite_factory = new (_mcomb2dNP_dput_composite_factory_storage) MultiCombine2DeviceDputFactoryNP(&_sconnmgr,  &_ni_array[4]);
#endif

#ifdef PAMI_ENABLE_X0_PROTOCOLS
          _mcast2d_composite_factory = new (_mcast2d_composite_factory_storage) MultiCast2DeviceFactory(&_sconnmgr, _shmem_ni, false, _mu_ni_mcast2d,  _mu_ni_mcast2d ? true : false);
#endif
          _mcast2d_dput_composite_factory = new (_mcast2d_dput_composite_factory_storage) MultiCast2DeviceDputFactory(_context,_context_id,mapidtogeometry,&_sconnmgr, _shmem_ni, false, _mu_global_dput_ni,  false);

#ifdef PAMI_ENABLE_X0_PROTOCOLS
          _mcomb2d_composite_factory = new (_mcomb2d_composite_factory_storage) MultiCombine2DeviceFactory(&_sconnmgr, _shmem_ni, _mu_ni_mcomb2d, me_ep);
#endif
#ifdef PAMI_ENABLE_X0_PROTOCOLS
          _mcomb2dNP_composite_factory = new (_mcomb2dNP_composite_factory_storage) MultiCombine2DeviceFactoryNP(&_sconnmgr,  &_ni_array[2]);
#endif
        }

        TRACE_FN_EXIT();
      }

      inline pami_result_t register_local_impl (size_t context_id, T_Geometry *geometry, uint64_t *in, int &n) 
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("_axial_mu_dput_ni %p",_axial_mu_dput_ni);
        if (_axial_mu_dput_ni == NULL)
          return PAMI_SUCCESS;

        if ((_mu_rectangle_msync_factory && __global.topology_local.size() == 1) || 
            (_msync2d_rectangle_composite_factory && __global.topology_local.size() > 1 &&
             __global.useMU() && __global.useshmem()))
        {
          *in = _axial_mu_dput_ni->getMsyncModel().getAllocationVector();
          n  = 1;
        }
        TRACE_FORMAT("*in %zu, _axial_mu_dput_ni %p",*in,_axial_mu_dput_ni);
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }    

      inline pami_result_t receive_global_impl (size_t context_id, T_Geometry *geometry, uint64_t *in, int n) 
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("*in %zu, _axial_mu_dput_ni %p",*in,_axial_mu_dput_ni);
        if (_axial_mu_dput_ni == NULL)
          return PAMI_SUCCESS;

        PAMI_assert (n == 1);

        TRACE_FORMAT("<%p>(%u)topology is coord? %s", this, __LINE__,(((PAMI::Topology *)geometry->getTopology(PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX))->type() == PAMI_COORD_TOPOLOGY)?"YES":"NO");
        TRACE_FORMAT("<%p>(%u)factories available %p, %p", this, __LINE__,_mu_rectangle_msync_factory,_msync2d_rectangle_composite_factory);
        if ((((PAMI::Topology *)geometry->getTopology(PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX))->type() == PAMI_COORD_TOPOLOGY)
            &&
            ((_mu_rectangle_msync_factory && __global.topology_local.size() == 1) ||
             (_msync2d_rectangle_composite_factory && __global.topology_local.size() > 1 && 
              __global.useMU() && __global.useshmem())
            )
           )
        {
          TRACE_FORMAT("<%p>(%u)CKEY_RECTANGLE_CLASSROUTEID ", this, __LINE__);
          uint64_t result = *in;
          for (size_t i = 0; i < 64; ++i)
            if ((result & (0x1 << i)) != 0)
            {
              //fprintf (, "Calling configure with class route %ld, in 0x%lx", i, result);
              pami_result_t rc = _axial_mu_dput_ni->getMsyncModel().configureClassRoute(i, (PAMI::Topology *)geometry->getTopology(PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX));
              if (rc == PAMI_SUCCESS)
                geometry->setKey (context_id, PAMI::Geometry::CKEY_RECTANGLE_CLASSROUTEID, (void*)(i+1));
              TRACE_FORMAT("<%p>(%u)CKEY_RECTANGLE_CLASSROUTEID %zu", this, __LINE__,(i+1));
              break;
            }
        }

        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      inline pami_result_t analyze_impl(size_t context_id, T_Geometry *geometry, int phase, uint64_t * inout_val=NULL)
      {
        TRACE_FN_ENTER();
        PAMI_assertf(context_id == _context_id,"%zu != %zu\n",context_id,_context_id);
        TRACE_FORMAT("<%p>phase %d, context_id %zu, geometry %p, msync %p, mcast %p, mcomb %p", this, phase, context_id, geometry, &_shmem_msync_factory, &_shmem_mcast_factory, &_shmem_mcomb_factory);
        if (geometry->size() == 1) // Disable BGQ protocols on 1 task geometries.
        {
          TRACE_FN_EXIT();
          return PAMI_SUCCESS;
        }
        bool noncontig_support = false;
        {
          void *val = geometry->getKey(context_id, PAMI::Geometry::CKEY_NONCONTIG_DISABLE);
          TRACE_FORMAT(" CKEY_NONCONTIG_DISABLE %p",val);
          if(!val)      // If noncontig is not disabled
            noncontig_support = true;
        }
        // BE CAREFUL! It's not ok to make registration decisions based on local topology unless you know that all nodes will make the same decision.
        // We use local_sub_topology on single node registrations 
        // and on MU/Classroute registrations (because they are rectangular so all nodes have the same local subtopology).
        PAMI::Topology * topology = (PAMI::Topology*) geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX);
        PAMI::Topology * local_sub_topology = (PAMI::Topology*) geometry->getTopology(PAMI::Geometry::LOCAL_TOPOLOGY_INDEX);
        PAMI::Topology * master_sub_topology = (PAMI::Topology*) geometry->getTopology(PAMI::Geometry::MASTER_TOPOLOGY_INDEX);
        // Is there a coordinate (rectangular) topology? 
        PAMI::Topology * rectangle = (PAMI::Topology*)geometry->getTopology(PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX);
        bool rectangle_topo = (rectangle->type() == PAMI_COORD_TOPOLOGY); // could be EMPTY if not valid on this geometry

        TRACE_FORMAT("<%p>topology: size() %zu, isLocal() %u/%zu, isGlobal #u/%zu, is rectangle %u", this, topology->size(),  topology->isLocalToMe(), local_sub_topology->size(), master_sub_topology->size(),rectangle_topo);

        //DO_DEBUG(for (unsigned i = 0; i < topology->size(); ++i) fprintf(, "<%p>PAMI::CollRegistration::BGQMultiregistration::analyze_impl() topology[%u] = %u", this, i, topology->index2Endpoint(i)););

        if (phase == 0)
        {

          if (_binomial_barrier_factory)
          {
            //Set optimized barrier to binomial. May override optimized barrier later
            pami_xfer_t xfer = {0};
            OptBinomialBarrier *opt_binomial = (OptBinomialBarrier *)
                                               _binomial_barrier_factory->generate(geometry, &xfer);
            PAMI_assert(geometry->getKey(context_id, PAMI::Geometry::CKEY_BARRIERCOMPOSITE2)==opt_binomial);
            opt_binomial->getExecutor()->setContext(_context);
            geometry->setKey(context_id, PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE,
                             (void*)opt_binomial);

            COMPILE_TIME_ASSERT(sizeof(GeometryInfo<OptBinomialBarrierFactory>) <= ProtocolAllocator::objsize);
            GeometryInfo<OptBinomialBarrierFactory>    *gi = (GeometryInfo<OptBinomialBarrierFactory>*) _geom_allocator.allocateObject();
            new(gi) GeometryInfo<OptBinomialBarrierFactory>(_context, opt_binomial, &_geom_allocator);

            geometry->setCleanupCallback(cleanupCallback, gi);
          }


          if ((__global.useshmem())  && (__global.topology_local.size() > 1)
#ifndef PAMI_ENABLE_SHMEM_SUBNODE
              && (__global.topology_local.size() == local_sub_topology->size()) /// \todo might ease this restriction later - when shmem supports it
#endif
             )
          {
            TRACE_FORMAT("<%p>Register Shmem local barrier", this);

            // If the geometry is all local nodes, we can use pure shmem composites.
            if (topology->isLocalToMe())
            {
              TRACE_FORMAT("<%p>Register Local Shmem factories", this);

              CCMI::Executor::Composite *composite;
              pami_xfer_t xfer = {0};
              composite = _shmem_msync_factory.generate(geometry, &xfer);
              PAMI_assert(geometry->getKey(context_id, PAMI::Geometry::CKEY_BARRIERCOMPOSITE7)==composite);
              // Add Barriers
              geometry->addCollective(PAMI_XFER_BARRIER, &_shmem_msync_factory, _context, _context_id);

              COMPILE_TIME_ASSERT(sizeof(GeometryInfo<ShmemMultiSyncFactory>) <= ProtocolAllocator::objsize);
              GeometryInfo<ShmemMultiSyncFactory>    *gi = (GeometryInfo<ShmemMultiSyncFactory>*) _geom_allocator.allocateObject();
              new(gi) GeometryInfo<ShmemMultiSyncFactory>(_context, composite, &_geom_allocator);
              geometry->setCleanupCallback(cleanupCallback, gi);

              // Add Broadcasts
              geometry->addCollectiveCheck(PAMI_XFER_BROADCAST, &_shmem_mcast_factory, _context, _context_id);

              // Add Allreduces
              if ((__global.topology_local.size() ==  4) ||  
                  (__global.topology_local.size() ==  8) ||
                  (__global.topology_local.size() == 16))
                //||  
                //(__global.topology_local.size() == 64))
                geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE, &_shmem_mcomb_factory, _context, _context_id);
            }
          }

          // (Maybe) Add rectangle broadcasts
          TRACE_FORMAT("<%p>Analyze Rectangle factories %p/%p, %p/%p, isLocal? %u, rectangle_topo %u", this,
                       _mu_rectangle_1color_dput_broadcast_factory, _mu_rectangle_dput_broadcast_factory,
                       _shmem_mu_rectangle_1color_dput_broadcast_factory, _shmem_mu_rectangle_dput_broadcast_factory,
                       topology->isLocal(),rectangle_topo);
          if (rectangle_topo)
          {
            // Add rectangle protocols:
            if ((_shmem_mu_rectangle_1color_dput_broadcast_factory)
#ifndef PAMI_ENABLE_SHMEM_SUBNODE
                && (__global.topology_local.size() == local_sub_topology->size()) /// \todo might ease this restriction later - when shmem supports it
#endif
               )
                addCollectiveCheckNonContig(geometry,PAMI_XFER_BROADCAST,  _shmem_mu_rectangle_1color_dput_broadcast_factory, _context, _context_id);
            if (_mu_rectangle_1color_dput_broadcast_factory)
                addCollectiveCheckNonContig(geometry,PAMI_XFER_BROADCAST,  _mu_rectangle_1color_dput_broadcast_factory, _context, _context_id);

            if ((_shmem_mu_rectangle_dput_broadcast_factory) 
#ifndef PAMI_ENABLE_SHMEM_SUBNODE
                && (__global.topology_local.size() == local_sub_topology->size()) /// \todo might ease this restriction later - when shmem supports it
#endif
               )
              geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,  _shmem_mu_rectangle_dput_broadcast_factory, _context, _context_id);
            if (_mu_rectangle_dput_broadcast_factory)
              geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,  _mu_rectangle_dput_broadcast_factory, _context, _context_id);

            if (local_sub_topology->size() < 32) {
	      if(_rectangle_dput_allgather_factory)
          geometry->addCollectiveCheck(PAMI_XFER_ALLGATHER,  _rectangle_dput_allgather_factory, _context, _context_id);
	      if (_rectangle_dput_allgatherv_factory)
          geometry->addCollectiveCheck(PAMI_XFER_ALLGATHERV_INT,  _rectangle_dput_allgatherv_factory, _context, _context_id);
	    }

	    if (_mu_rectangle_dput_allreduce_factory)
              geometry->addCollective(PAMI_XFER_ALLREDUCE,  _mu_rectangle_dput_allreduce_factory, _context,  _context_id);	   

	    if (_rectangle_dput_allreduce_factory)
              geometry->addCollective(PAMI_XFER_ALLREDUCE,  _rectangle_dput_allreduce_factory, _context,  _context_id);	   

          }
          geometry->setKey(_context_id, PAMI::Geometry::CKEY_PHASE_1_DONE,(void*)0); /* Phase 1 not done */
        }
        else if (phase == 1)
        {
          /* Check if phase 1 was previously done at least once. This affects some opt/de-opt protocols */
          void *valP = geometry->getKey(_context_id,PAMI::Geometry::CKEY_PHASE_1_DONE);
          bool phase_1_done = (valP && valP != PAMI_CR_CKEY_FAIL); 
          /// A simple check (of sizes) to see if this subgeometry is all global,
          /// then the geometry topology is usable by 'pure' MU protocols
          bool usePureMu = topology->size() == master_sub_topology->size() ? true : false;
          usePureMu = usePureMu && !topology->isLocalToMe();
          TRACE_FORMAT("<%p>usePureMu = %u (size %zu/%zu)", this, usePureMu, topology->size(), master_sub_topology->size());

          // Add optimized binomial barrier
          if (_binomial_barrier_factory)
          {  
          /* With analyze, de-optimize, analyze, optimize, analyze, ... processing, the factory may have
                     already been added to the algorithm list, so check first  */
            if(!phase_1_done)
            {  
              geometry->addCollective(PAMI_XFER_BARRIER, _binomial_barrier_factory, _context, _context_id);
            }
          }

          if (_alltoall_factory && !topology->isLocalToMe())
            geometry->addCollective(PAMI_XFER_ALLTOALL, _alltoall_factory, _context, _context_id);

          if (_alltoallv_factory)
            geometry->addCollective(PAMI_XFER_ALLTOALLV, _alltoallv_factory, _context, _context_id);

          if (_alltoallv_int_factory)
            geometry->addCollective(PAMI_XFER_ALLTOALLV_INT, _alltoallv_int_factory, _context, _context_id);

          // Check for class routes before enabling MU collective network protocols
          void *val;
          val = geometry->getKey(_context_id,PAMI::Geometry::CKEY_MSYNC_CLASSROUTEID);
          TRACE_FORMAT("<%p>CKEY_MSYNC_CLASSROUTEID %p", this, val);
          void *valr;
          valr = geometry->getKey(_context_id, PAMI::Geometry::CKEY_RECTANGLE_CLASSROUTEID); // Rectangle 'class route'
          TRACE_FORMAT("(%u)CKEY_RECTANGLE_CLASSROUTEID %p %s, rectangle_topo %u", __LINE__,valr, valr != PAMI_CR_CKEY_FAIL?" ":"PAMI_CR_CKEY_FAIL",rectangle_topo);

          if (val && val != PAMI_CR_CKEY_FAIL && rectangle_topo)// We have a class route
          {
            // If we can use pure MU composites, add them
            if (usePureMu)
            {
              // Direct MU/GI only on one context per node (lowest T, context 0) lowest T is guaranteed by classroute code
              if ((_context_id == 0) // (__global.mapping.isLowestT())
                  && (_gi_msync_factory))
              {
                TRACE_FORMAT("<%p>Register MU barrier", this);
                geometry->setKey(context_id, PAMI::Geometry::CKEY_BARRIERCOMPOSITE3, NULL);
                CCMI::Executor::Composite* composite;
                pami_xfer_t xfer = {0};
                composite = _gi_msync_factory->generate(geometry, &xfer);
                PAMI_assert(geometry->getKey(context_id, PAMI::Geometry::CKEY_BARRIERCOMPOSITE3)==composite);

                // Add Barriers
                geometry->addCollective(PAMI_XFER_BARRIER, _gi_msync_factory, _context, _context_id);

                COMPILE_TIME_ASSERT(sizeof(GeometryInfo<GIMultiSyncFactory>) <= ProtocolAllocator::objsize);
                GeometryInfo<GIMultiSyncFactory>    *gi = (GeometryInfo<GIMultiSyncFactory>*) _geom_allocator.allocateObject();
                new(gi) GeometryInfo<GIMultiSyncFactory>(_context, composite, &_geom_allocator);
                geometry->setCleanupCallback(cleanupCallback, gi);
              }

              if ((_mu_rectangle_msync_factory) && 
                  (valr && valr != PAMI_CR_CKEY_FAIL))
              {
                TRACE_FORMAT("<%p>Register MU Rectangle msync 2", this);
                CCMI::Executor::Composite* composite;
                pami_xfer_t xfer = {0};
                composite = _mu_rectangle_msync_factory->generate(geometry, &xfer);
                PAMI_assert(geometry->getKey(context_id, PAMI::Geometry::CKEY_BARRIERCOMPOSITE5)==composite);
                // Add Barriers
                geometry->addCollective(PAMI_XFER_BARRIER, _mu_rectangle_msync_factory, _context, _context_id);
                geometry->setKey(context_id, PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE,
                                 (void*)composite);

                COMPILE_TIME_ASSERT(sizeof(GeometryInfo<MURectangleMultiSyncFactory>) <= ProtocolAllocator::objsize);
                GeometryInfo<MURectangleMultiSyncFactory>    *gi = (GeometryInfo<MURectangleMultiSyncFactory>*) _geom_allocator.allocateObject();
                new(gi) GeometryInfo<MURectangleMultiSyncFactory>(_context, composite, &_geom_allocator);
                geometry->setCleanupCallback(cleanupCallback, gi);
              }
            }
          }
          if ((val && val != PAMI_CR_CKEY_FAIL) || // We have a class route or
              (topology->isLocalToMe()))           // It's all local - we might use 2 device protocol in shmem-only mode
          {
            // Add 2 device composite protocols
#ifndef PAMI_ENABLE_SHMEM_SUBNODE
            if (__global.topology_local.size() == local_sub_topology->size()) /// \todo might ease this restriction later - when shmem supports it
#endif
            {
              if (_msync2d_gishm_composite_factory)
              {
                geometry->setKey(context_id, PAMI::Geometry::CKEY_BARRIERCOMPOSITE4, NULL);
                CCMI::Executor::Composite* composite;
                pami_xfer_t xfer = {0};
                composite = _msync2d_gishm_composite_factory->generate(geometry, &xfer);
                geometry->addCollective(PAMI_XFER_BARRIER, _msync2d_gishm_composite_factory, _context, _context_id);
                PAMI_assert(geometry->getKey(context_id, PAMI::Geometry::CKEY_BARRIERCOMPOSITE4)==composite);

                COMPILE_TIME_ASSERT(sizeof(GeometryInfo<MultiSync2DeviceGIShmemFactory>) <= ProtocolAllocator::objsize);
                GeometryInfo<MultiSync2DeviceGIShmemFactory>    *gi = (GeometryInfo<MultiSync2DeviceGIShmemFactory>*) _geom_allocator.allocateObject();
                new(gi) GeometryInfo<MultiSync2DeviceGIShmemFactory>(_context, composite, &_geom_allocator);
                geometry->setCleanupCallback(cleanupCallback, gi);
              }

              Topology master = *master_sub_topology;
              master.convertTopology(PAMI_COORD_TOPOLOGY);
              if ((master.type() == PAMI_COORD_TOPOLOGY) && 
                  (_msync2d_rectangle_composite_factory) && 
                  (valr && valr != PAMI_CR_CKEY_FAIL)
                 )
              {
                TRACE_FORMAT("<%p>Register 2D MU Rectangle msync 2", this);
                /* With analyze, de-optimize, analyze, optimize, analyze, ... processing, the composite may have
                   already been created and added to the algorithm list, so check first  */
                if(!phase_1_done)
                {
                  CCMI::Executor::Composite *composite;
                  pami_xfer_t xfer = {0};
                  composite = _msync2d_rectangle_composite_factory->generate(geometry, &xfer);
                  PAMI_assert(geometry->getKey(context_id, PAMI::Geometry::CKEY_BARRIERCOMPOSITE6)==composite);
                  geometry->addCollective(PAMI_XFER_BARRIER, _msync2d_rectangle_composite_factory, _context, _context_id);
                  geometry->setKey(context_id, PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE,
                                   (void*)composite);
  
                  COMPILE_TIME_ASSERT(sizeof(GeometryInfo<MultiSync2DeviceRectangleFactory>) <= ProtocolAllocator::objsize);
                  GeometryInfo<MultiSync2DeviceRectangleFactory>    *gi = (GeometryInfo<MultiSync2DeviceRectangleFactory>*) _geom_allocator.allocateObject();
                  new(gi) GeometryInfo<MultiSync2DeviceRectangleFactory>(_context, composite, &_geom_allocator);
                  geometry->setCleanupCallback(cleanupCallback, gi);
                }
              }
            }
          }

          val = geometry->getKey(context_id,PAMI::Geometry::CKEY_MCAST_CLASSROUTEID);
          TRACE_FORMAT("<%p>CKEY_MCAST_CLASSROUTEID %p", this, val);

          if ((val && val != PAMI_CR_CKEY_FAIL && rectangle_topo) || // We have a class route or
              (topology->isLocalToMe()))           // It's all local - we might use 2 device protocol in shmem-only mode
          {
            // Add 1 PPN MU Dput (over allreduce) protocols
            if (_mucollectivedputmulticastfactory)
              geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,  _mucollectivedputmulticastfactory, _context, _context_id);

            if (_mucollectivedputmulticombinefactory)
              geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,  _mucollectivedputmulticombinefactory, _context, _context_id);

            // If we can use pure MU composites, add them
            if (usePureMu)
            {
              // Direct MU only on one context per node (lowest T, context 0) lowest T is guaranteed by classroute code
              if (_context_id == 0) // (__global.mapping.isLowestT())
              {
                TRACE_FORMAT("<%p>Register MU bcast", this);
                // Add Broadcasts
                geometry->addCollective(PAMI_XFER_BROADCAST,  _mu_mcast_factory, _context,  _context_id);
#ifdef PAMI_ENABLE_X0_PROTOCOLS
                geometry->addCollectiveCheck(PAMI_XFER_BROADCAST,  _mu_mcast3_factory, _context, _context_id);
#endif
              }
            }
            // Add 2 device composite protocols
#ifndef PAMI_ENABLE_SHMEM_SUBNODE
              if (__global.topology_local.size() == local_sub_topology->size()) /// \todo might ease this restriction later - when shmem supports it
#endif
              {
#ifdef PAMI_ENABLE_X0_PROTOCOLS
                if (_mcast2d_composite_factory)
                {
                  TRACE_FORMAT("<%p>Register mcast 2D", this);
                  geometry->addCollective(PAMI_XFER_BROADCAST, _mcast2d_composite_factory, _context, _context_id);
                }
#endif
                if(((__global.topology_local.size() ==  2) ||  
                    (__global.topology_local.size() ==  4) ||  
                    (__global.topology_local.size() ==  8) ||
                    (__global.topology_local.size() == 16) || 
                    (__global.topology_local.size() == 32) || 
                    (__global.topology_local.size() == 64) ) &&
                   (_mcast2d_dput_composite_factory))
                {
                  TRACE_FORMAT("<%p>Register mcast dput 2D", this);
                  geometry->addCollectiveCheck(PAMI_XFER_BROADCAST, _mcast2d_dput_composite_factory, _context, _context_id);
                }
              }
          }

          val = geometry->getKey(context_id,PAMI::Geometry::CKEY_MCOMB_CLASSROUTEID);
          TRACE_FORMAT("<%p>CKEY_MCOMB_CLASSROUTEID %p", this, val);

          if ((val && val != PAMI_CR_CKEY_FAIL && rectangle_topo) || // We have a class route or
              (topology->isLocalToMe()))           // It's all local - we might use 2 device protocol in shmem-only mode
          {
            // Add 2 device msync over mcombine (thus using CKEY_MCOMB_CLASSROUTEID not CKEY_MSYNC_CLASSROUTEID) composite protocol
#ifndef PAMI_ENABLE_SHMEM_SUBNODE
            if (__global.topology_local.size() == local_sub_topology->size()) /// \todo might ease this restriction later - when shmem supports it
#endif
            {
              if (_msync2d_composite_factory)
              {
                CCMI::Executor::Composite* composite;
                pami_xfer_t xfer = {0};
                composite = _msync2d_composite_factory->generate(geometry, &xfer);
                geometry->addCollective(PAMI_XFER_BARRIER, _msync2d_composite_factory, _context, _context_id);

                COMPILE_TIME_ASSERT(sizeof(GeometryInfo<MultiSync2DeviceFactory>) <= ProtocolAllocator::objsize);
                GeometryInfo<MultiSync2DeviceFactory>    *gi = (GeometryInfo<MultiSync2DeviceFactory>*) _geom_allocator.allocateObject();
                new(gi) GeometryInfo<MultiSync2DeviceFactory>(_context, composite, &_geom_allocator);
                geometry->setCleanupCallback(cleanupCallback, gi);
              }
            }

            // If we can use pure MU composites, add them
            if (usePureMu)
            {
              // Direct MU allreduce only on one context per node (lowest T, context 0) lowest T is guaranteed by classroute code
              if (_context_id == 0) // (__global.mapping.isLowestT())
              {
                // Add Allreduces
                TRACE_FORMAT("<%p>Register MU allreduce", this);
                geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE, _mu_mcomb_factory, _context, _context_id);
              }
            }

            // Add 2 device composite protocols
            if ((__global.topology_local.size() ==  2) ||  
                (__global.topology_local.size() ==  4) ||  
                (__global.topology_local.size() ==  8) ||
                (__global.topology_local.size() == 16) || 
                (__global.topology_local.size() == 32) || 
                (__global.topology_local.size() == 64))
#ifndef PAMI_ENABLE_SHMEM_SUBNODE
              if (__global.topology_local.size() == local_sub_topology->size()) /// \todo might ease this restriction later - when shmem supports it
#endif
              {
                // New optimized MU+Shmem protocol requires a class route
                if (val && val != PAMI_CR_CKEY_FAIL && rectangle_topo)
                {
                  if (_mushmemcollectivedputmulticombinefactory && 
                      (__global.topology_local.size() == __global.mapping.tSize())) /// \todo temp until the protocol is fixed
                    geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE,  _mushmemcollectivedputmulticombinefactory, _context,_context_id);
                  if(_mushmemcollectivedputmulticastfactory && 
                     (__global.topology_local.size() == __global.mapping.tSize())) /// \todo temp until the protocol is fixed
                    geometry->addCollectiveCheck(PAMI_XFER_BROADCAST, _mushmemcollectivedputmulticastfactory, _context,_context_id);
                }


#ifdef PAMI_ENABLE_X0_PROTOCOLS
                // NP (non-pipelining) 2 device protocols
                if ((_mcomb2dNP_dput_composite_factory) && (master_sub_topology->size() > 1))  // \todo Simple NP protocol doesn't like 1 master - fix it later
                  geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE, _mcomb2dNP_dput_composite_factory, _context, _context_id);
#endif
#ifdef PAMI_ENABLE_X0_PROTOCOLS
                if ((_mcomb2dNP_composite_factory) && (master_sub_topology->size() > 1))  // \todo Simple NP protocol doesn't like 1 master - fix it later
                  geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE, _mcomb2dNP_composite_factory, _context, _context_id);
#endif
                //  2 device protocols
                if ((_mcomb2d_dput_composite_factory) && (__global.topology_local.size() != 64)
                    && (__global.topology_local.size() != 2) && (__global.topology_local.size() != 32))
                  geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE, _mcomb2d_dput_composite_factory, _context, _context_id);

#ifdef PAMI_ENABLE_X0_PROTOCOLS
                if (_mcomb2d_composite_factory)
                  geometry->addCollectiveCheck(PAMI_XFER_ALLREDUCE, _mcomb2d_composite_factory, _context, _context_id);
#endif
              }
          }
          geometry->setKey(_context_id, PAMI::Geometry::CKEY_PHASE_1_DONE,(void*)1);/* Phase 1 done */
        }
        else if (phase == -1)
        {
          /// \todo remove MU collectives algorithms... TBD... only remove phase 1 algorithms??
          geometry->rmCollective(PAMI_XFER_ALLTOALLV_INT,  _alltoallv_int_factory,                    _context, _context_id);
          geometry->rmCollective(PAMI_XFER_ALLTOALLV,      _alltoallv_factory,                        _context, _context_id);
          geometry->rmCollective(PAMI_XFER_ALLTOALL,       _alltoall_factory,                         _context, _context_id);

          geometry->rmCollectiveCheck(PAMI_XFER_BROADCAST, _mu_mcast_factory,                         _context, _context_id);
          geometry->rmCollectiveCheck(PAMI_XFER_BROADCAST, _mcast2d_dput_composite_factory,           _context, _context_id);
          if (_mucollectivedputmulticastfactory)
            geometry->rmCollectiveCheck(PAMI_XFER_BROADCAST,_mucollectivedputmulticastfactory,        _context, _context_id);
          if (_mushmemcollectivedputmulticastfactory)
            geometry->rmCollectiveCheck(PAMI_XFER_BROADCAST,_mushmemcollectivedputmulticastfactory,   _context, _context_id);

          geometry->rmCollective(PAMI_XFER_BARRIER,        _mu_rectangle_msync_factory,               _context, _context_id);
          geometry->rmCollective(PAMI_XFER_BARRIER,        _gi_msync_factory,                         _context, _context_id);
/* Do not remove this barrier - it isn't de-optimized.  The addCollective will check CKEY_PHASE_1_DONE before (re)adding it. 
          geometry->rmCollective(PAMI_XFER_BARRIER,        _binomial_barrier_factory,                 _context, _context_id);
*/
          geometry->rmCollective(PAMI_XFER_BARRIER,        _msync2d_composite_factory,                _context, _context_id);
          geometry->rmCollective(PAMI_XFER_BARRIER,        _msync2d_gishm_composite_factory,          _context, _context_id);
/* Do not remove this barrier - it isn't de-optimized.  The addCollective will check CKEY_PHASE_1_DONE before (re)adding it. 
          geometry->rmCollective(PAMI_XFER_BARRIER,        _msync2d_rectangle_composite_factory,      _context, _context_id);
*/

          if (_mucollectivedputmulticombinefactory)
            geometry->rmCollectiveCheck(PAMI_XFER_ALLREDUCE,_mucollectivedputmulticombinefactory, _context, _context_id);
          if (_mushmemcollectivedputmulticombinefactory)
            geometry->rmCollectiveCheck(PAMI_XFER_ALLREDUCE,_mushmemcollectivedputmulticombinefactory,_context, _context_id);
          geometry->rmCollectiveCheck(PAMI_XFER_ALLREDUCE, _mcomb2d_dput_composite_factory,           _context, _context_id);
          geometry->rmCollectiveCheck(PAMI_XFER_ALLREDUCE, _mu_mcomb_factory,                         _context, _context_id);

#ifdef PAMI_ENABLE_X0_PROTOCOLS
          geometry->rmCollectiveCheck(PAMI_XFER_BROADCAST, _mu_mcast3_factory,                _context, _context_id);
          geometry->rmCollectiveCheck(PAMI_XFER_BROADCAST, _mcast2d_composite_factory,        _context, _context_id);

          geometry->rmCollectiveCheck(PAMI_XFER_ALLREDUCE, _mcomb2dNP_dput_composite_factory, _context, _context_id);
          geometry->rmCollectiveCheck(PAMI_XFER_ALLREDUCE, _mcomb2d_composite_factory,        _context, _context_id);
          geometry->rmCollectiveCheck(PAMI_XFER_ALLREDUCE, _mcomb2dNP_composite_factory,      _context, _context_id);
#endif

        }

        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }
    public:
      pami_client_t                                   _client;
      pami_context_t                                  _context;
      size_t                                          _context_id;
      // This is a pointer to the current dispatch id of the context
      // This will be decremented by the ConstructNativeInterface routines
      int                                            *_dispatch_id;
      std::map<unsigned, pami_geometry_t>            *_geometry_map;

      T_Allocator                                    &_allocator;
      T_BigAllocator                                 &_big_allocator;
      PAMI::MemoryAllocator<sizeof(GeometryInfo<CCMI::Adaptor::CollectiveProtocolFactory>),16>  _geom_allocator;

      // CCMI Connection Manager Class
      CCMI::ConnectionManager::SimpleConnMgr          _sconnmgr;
      CCMI::ConnectionManager::CommSeqConnMgr         _csconnmgr;
      CCMI::ConnectionManager::ColorGeometryConnMgr   _cg_connmgr;
      CCMI::ConnectionManager::ColorConnMgr           _color_connmgr;
      CCMI::ConnectionManager::ColorMapConnMgr        _color_map_connmgr;
      CCMI::ConnectionManager::TorusConnMgr           _torus_connmgr;

      //* SHMEM interfaces:
      // Shmem Device
      T_ShmemDevice                                  &_shmem_device;

      // Native Interface
      T_ShmemNativeInterface                         *_shmem_ni;

      // CCMI Barrier Interface
      ShmemMultiSyncFactory                           _shmem_msync_factory;

      // CCMI Broadcast Interfaces
      ShmemMultiCastFactory                           _shmem_mcast_factory;

      // CCMI Allreduce Interface
      ShmemMultiCombineFactory                        _shmem_mcomb_factory;

      // MU Device
      T_MUDevice                                     &_mu_device;

      // MU Native Interface

      T_MUNativeInterface                            *_mu_ni_msync;
      uint8_t                                         _mu_ni_msync_storage[sizeof(T_MUNativeInterface)];
      T_MUNativeInterface                            *_mu_ni_mcomb;
      uint8_t                                         _mu_ni_mcomb_storage[sizeof(T_MUNativeInterface)];
      T_MUNativeInterface                            *_mu_ni_mcast;
      uint8_t                                         _mu_ni_mcast_storage[sizeof(T_MUNativeInterface)];
      T_MUNativeInterface                            *_mu_ni_mcast3;
      uint8_t                                         _mu_ni_mcast3_storage[sizeof(T_MUNativeInterface)];
      T_MUNativeInterface                            *_mu_ni_msync2d;
      uint8_t                                         _mu_ni_msync2d_storage[sizeof(T_MUNativeInterface)];
      T_MUNativeInterface                            *_mu_ni_mcast2d;
      uint8_t                                         _mu_ni_mcast2d_storage[sizeof(T_MUNativeInterface)];
      T_MUNativeInterface                            *_mu_ni_mcomb2d;
      uint8_t                                         _mu_ni_mcomb2d_storage[sizeof(T_MUNativeInterface)];
      T_MUNativeInterface                            *_mu_ni_mcomb2dNP;
      uint8_t                                         _mu_ni_mcomb2dNP_storage[sizeof(T_MUNativeInterface)];

      T_AxialDputNativeInterface                     *_axial_mu_dput_ni;
      uint8_t                                         _axial_mu_dput_ni_storage[sizeof(T_AxialDputNativeInterface)];

      T_AxialShmemDputNativeInterface                *_axial_shmem_mu_dput_ni;
      uint8_t                                         _axial_shmem_mu_dput_ni_storage[sizeof(T_AxialShmemDputNativeInterface)];

      MUGlobalDputNI                                 *_mu_global_dput_ni;
      uint8_t                                         _mu_global_dput_ni_storage [sizeof(MUGlobalDputNI)];

      MUShmemGlobalDputNI                            *_mu_shmem_global_dput_ni;
      uint8_t                                         _mu_shmem_global_dput_ni_storage [sizeof(MUShmemGlobalDputNI)];

      MUAMMulticastNI                                 *_mu_ammulticast_ni;
      uint8_t                                         _mu_ammulticast_ni_storage [sizeof(MUAMMulticastNI)];

      M2MNISingle                                    *_mu_m2m_single_ni;
      uint8_t                                         _mu_m2m_single_ni_storage [sizeof(M2MNISingle)];

      M2MNIVectorLong                                *_mu_m2m_vector_long_ni;
      uint8_t                                         _mu_m2m_vector_long_ni_storage [sizeof(M2MNIVectorLong)];

      M2MNIVectorInt                                 *_mu_m2m_vector_int_ni;
      uint8_t                                         _mu_m2m_vector_int_ni_storage [sizeof(M2MNIVectorInt)];

      // Barrier factories
      GIMultiSyncFactory                             *_gi_msync_factory;
      uint8_t                                         _gi_msync_factory_storage[sizeof(GIMultiSyncFactory)];

      MURectangleMultiSyncFactory                    *_mu_rectangle_msync_factory;
      uint8_t                                         _mu_rectangle_msync_factory_storage[sizeof(MURectangleMultiSyncFactory)];

      OptBinomialBarrierFactory                      *_binomial_barrier_factory;
      uint8_t                                         _binomial_barrier_factory_storage[sizeof(OptBinomialBarrierFactory)];

      // Broadcast factories
      MUMultiCastFactory                             *_mu_mcast_factory;
      uint8_t                                         _mu_mcast_factory_storage[sizeof(MUMultiCastFactory)];
#ifdef PAMI_ENABLE_X0_PROTOCOLS
      MUMultiCast3Factory                            *_mu_mcast3_factory;
      uint8_t                                         _mu_mcast3_factory_storage[sizeof(MUMultiCast3Factory)];
#endif
      // Allreduce factories
      MUMultiCombineFactory                          *_mu_mcomb_factory;
      uint8_t                                         _mu_mcomb_factory_storage[sizeof(MUMultiCombineFactory)];

      // Barrier factories

      // 2 device composite factories
      CCMI::Interfaces::NativeInterface              *_ni_array[8];
      MultiSync2DeviceFactory                        *_msync2d_composite_factory;
      uint8_t                                         _msync2d_composite_factory_storage[sizeof(MultiSync2DeviceFactory)];

      MultiSync2DeviceGIShmemFactory                 *_msync2d_gishm_composite_factory;
      uint8_t                                         _msync2d_gishm_composite_factory_storage[sizeof(MultiSync2DeviceGIShmemFactory)];

      MultiSync2DeviceRectangleFactory               *_msync2d_rectangle_composite_factory;
      uint8_t                                         _msync2d_rectangle_composite_factory_storage[sizeof(MultiSync2DeviceRectangleFactory)];

#ifdef PAMI_ENABLE_X0_PROTOCOLS
      MultiCast2DeviceFactory                        *_mcast2d_composite_factory;
      uint8_t                                         _mcast2d_composite_factory_storage[sizeof(MultiCast2DeviceFactory)];
#endif
      MultiCast2DeviceDputFactory                    *_mcast2d_dput_composite_factory;
      uint8_t                                         _mcast2d_dput_composite_factory_storage[sizeof(MultiCast2DeviceDputFactory)];

#ifdef PAMI_ENABLE_X0_PROTOCOLS
      MultiCombine2DeviceFactory                     *_mcomb2d_composite_factory;
      uint8_t                                         _mcomb2d_composite_factory_storage[sizeof(MultiCombine2DeviceFactory)];
#endif
#ifdef PAMI_ENABLE_X0_PROTOCOLS
      MultiCombine2DeviceFactoryNP                   *_mcomb2dNP_composite_factory;
      uint8_t                                         _mcomb2dNP_composite_factory_storage[sizeof(MultiCombine2DeviceFactoryNP)];
#endif
      MultiCombine2DeviceDputFactory                 *_mcomb2d_dput_composite_factory;
      uint8_t                                         _mcomb2d_dput_composite_factory_storage[sizeof(MultiCombine2DeviceDputFactory)];

#ifdef PAMI_ENABLE_X0_PROTOCOLS
      MultiCombine2DeviceDputFactoryNP               *_mcomb2dNP_dput_composite_factory;
      uint8_t                                         _mcomb2dNP_dput_composite_factory_storage[sizeof(MultiCombine2DeviceDputFactoryNP)];
#endif

      MUCollectiveDputMultiCastFactory               *_mucollectivedputmulticastfactory;      
      MUShmemCollectiveDputMultiCastFactory          *_mushmemcollectivedputmulticastfactory;
      MUCollDputMcastStorage                          _mucolldputmcast_storage;

      MUCollectiveDputMulticombineFactory            *_mucollectivedputmulticombinefactory;
      uint8_t                                         _mucollectivedputmulticombinestorage[sizeof(MUCollectiveDputMulticombineFactory)];

      MUShmemCollectiveDputMulticombineFactory       *_mushmemcollectivedputmulticombinefactory;
      uint8_t                                         _mushmemcollectivedputmulticombinestorage[sizeof(MUShmemCollectiveDputMulticombineFactory)];

      MURectangleDput1ColorBroadcastFactory          *_mu_rectangle_1color_dput_broadcast_factory;
      uint8_t                                         _mu_rectangle_1color_dput_broadcast_factory_storage[sizeof(MURectangleDput1ColorBroadcastFactory)];

      MURectangleDputBroadcastFactory                *_mu_rectangle_dput_broadcast_factory;
      uint8_t                                         _mu_rectangle_dput_broadcast_factory_storage[sizeof(MURectangleDputBroadcastFactory)];

      RectangleDput1ColorBroadcastFactory            *_shmem_mu_rectangle_1color_dput_broadcast_factory;
      uint8_t                                         _shmem_mu_rectangle_1color_dput_broadcast_factory_storage[sizeof(RectangleDput1ColorBroadcastFactory)];

      RectangleDputBroadcastFactory                  *_shmem_mu_rectangle_dput_broadcast_factory;
      uint8_t                                         _shmem_mu_rectangle_dput_broadcast_factory_storage[sizeof(RectangleDputBroadcastFactory)];

      RectangleDputAllgatherFactory                  *_rectangle_dput_allgather_factory;
      uint8_t                                         _rectangle_dput_allgather_factory_storage[sizeof(RectangleDputAllgatherFactory)];

      RectangleDputAllgatherVFactory                 *_rectangle_dput_allgatherv_factory;
      uint8_t                                         _rectangle_dput_allgatherv_factory_storage[sizeof(RectangleDputAllgatherVFactory)];

      //Rectangle Dput Allreduce
      RectangleDputAllreduceFactory                  *_rectangle_dput_allreduce_factory;
      uint8_t                                         _rectangle_dput_allreduce_factory_storage[sizeof(RectangleDputAllreduceFactory)];      

      //Rectangle Dput Allreduce
      MURectangleDputAllreduceFactory                  *_mu_rectangle_dput_allreduce_factory;
      uint8_t                                         _mu_rectangle_dput_allreduce_factory_storage[sizeof(MURectangleDputAllreduceFactory)];
      
      // Alltoall
      All2AllFactory                                *_alltoall_factory;
      uint8_t                                        _alltoall_factory_storage[sizeof(All2AllFactory)];

      All2AllvFactory                               *_alltoallv_factory;
      uint8_t                                        _alltoallv_factory_storage[sizeof(All2AllvFactory)];

      All2AllvFactory_int                           *_alltoallv_int_factory;
      uint8_t                                        _alltoallv_int_factory_storage[sizeof(All2AllvFactory_int)];

      static inline void cleanupCallback(pami_context_t ctxt, void *data, pami_result_t res)
      {
        TRACE_FN_ENTER();
        GeometryInfo<CCMI::Adaptor::CollectiveProtocolFactory> *gi = (GeometryInfo<CCMI::Adaptor::CollectiveProtocolFactory>*) data;
        //CCMI::Executor::Composite *opt_barrier_composite = (CCMI::Executor::Composite *)gi->_opt_barrier_composite;

        PAMI::MemoryAllocator<sizeof(GeometryInfo<CCMI::Adaptor::CollectiveProtocolFactory>),16>  *geom_allocator =
        (PAMI::MemoryAllocator<sizeof(GeometryInfo<CCMI::Adaptor::CollectiveProtocolFactory>),16>  *)gi->_allocator;

        //if(opt_barrier_composite) OptBinomialBarrierFactory::cleanup_done_fn(ctxt, opt_barrier_composite, res);
        //opt_barrier_composite->_factory->_alloc.returnObject(opt_barrier_composite);

        gi->~GeometryInfo<CCMI::Adaptor::CollectiveProtocolFactory>();
        geom_allocator->returnObject(gi);

        TRACE_FN_EXIT();
      }

      static PAMI::Topology *bgq_comm2topo  (pami_context_t context, 
					     unsigned       comm) 
      {
	PAMI_GEOMETRY_CLASS *geometry = (PAMI_GEOMETRY_CLASS *) mapidtogeometry(context, comm);
	return (PAMI::Topology *)geometry->getTopology(PAMI::Geometry::COORDINATE_TOPOLOGY_INDEX);
      }

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
