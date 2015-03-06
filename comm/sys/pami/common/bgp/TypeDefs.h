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
 * \file common/bgp/TypeDefs.h
 * \brief ???
 */

#ifndef __common_bgp_TypeDefs_h__
#define __common_bgp_TypeDefs_h__

#define ENABLE_SHMEM_DEVICE
//#define ENABLE_MU_DEVICE

#include "algorithms/geometry/Geometry.h"

#include "components/devices/workqueue/LocalBcastWQMessage.h"

#include "components/devices/workqueue/LocalReduceWQMessage.h"

#include "components/atomic/native/NativeCounter.h"
#include "components/atomic/counter/CounterBarrier.h"
#include "components/atomic/indirect/IndirectBarrier.h"
#include "components/devices/misc/AtomicBarrierMsg.h"

#include "common/bgp/NativeInterface.h"


namespace PAMI
{
  typedef Geometry::Common                     BGPGeometry;


  typedef PAMI::Barrier::IndirectCounter<PAMI::Counter::Indirect<PAMI::Counter::Native> > Barrier_Type;

  typedef PAMI::Device::AtomicBarrierMdl<Barrier_Type> Barrier_Model;

  typedef BGPNativeInterfaceAS <Device::LocalBcastWQModel, Barrier_Model,Device::LocalReduceWQModel> AllSidedNI;
}

//#define PAMI_COLL_MCAST_CLASS
//#define PAMI_COLL_M2M_CLASS
//#define PAMI_NATIVEINTERFACE
#define PAMI_GEOMETRY_CLASS    PAMI::BGPGeometry

#endif
