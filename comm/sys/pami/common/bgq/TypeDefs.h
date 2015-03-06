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
 * \file common/bgq/TypeDefs.h
 * \brief BGQ protocol templates and typedefs
 */

#ifndef __common_bgq_TypeDefs_h__
#define __common_bgq_TypeDefs_h__

#include "util/ccmi_debug.h"
#include "util/trace.h"

#include "components/devices/shmem/ShmemDevice.h"
#include "components/devices/shmem/ShmemPacketModel.h"
#include "components/devices/shmem/ShmemDmaModel.h"
#include "components/devices/shmem/shaddr/BgqShaddrReadOnly.h"
#include "components/devices/shmem/shaddr/BgqShaddrPacketModel.h"
#include "components/devices/shmem/wakeup/WakeupBGQ.h"
#include "components/devices/shmem/wakeup/WakeupSemaphore.h"
#include "components/fifo/FifoPacket.h"
#include "components/fifo/linear/LinearFifo.h"
#include "components/fifo/wrap/WrapFifo.h"

#include "components/devices/bgq/mu2/Factory.h"
#include "components/devices/bgq/mu2/Context.h"
#include "components/devices/bgq/mu2/model/PacketModel.h"
#include "components/devices/bgq/mu2/model/DmaModel.h"
#include "components/devices/bgq/mu2/model/DmaModelMemoryFifoCompletion.h"

#include "p2p/protocols/send/eager/Eager.h"
#include "p2p/protocols/send/composite/Composite.h"
#include "p2p/protocols/rget/GetRdma.h"
#include "p2p/protocols/rput/PutRdma.h"
#include "p2p/protocols/SendPWQ.h"

#include "components/atomic/bgq/L2Counter.h"
#include "components/atomic/bgq/L2CounterBounded.h"
#include "components/atomic/native/NativeCounter.h"
#include "components/atomic/counter/CounterBarrier.h"
#include "components/atomic/indirect/IndirectBarrier.h"
#include "components/atomic/indirect/IndirectCounter.h"
#include "components/devices/misc/AtomicBarrierMsg.h"

#include "common/type/TypeMachine.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#ifdef CCMI_TRACE_ALL
 #define DO_TRACE_ENTEREXIT 1
 #define DO_TRACE_DEBUG     1
#else
 #define DO_TRACE_ENTEREXIT 0
 #define DO_TRACE_DEBUG     0
#endif

#include "components/devices/bgq/mu2/model/MulticastDmaModel.h"
#include "components/devices/bgq/mu2/model/Multicast.h"
#include "components/devices/bgq/mu2/model/Multicombine.h"
#include "components/devices/bgq/mu2/model/Multisync.h"
#include "components/devices/bgq/mu2/model/AllreducePacketModel.h"

#include "algorithms/interfaces/NativeInterfaceFactory.h"
#include "common/NativeInterface.h"
#include "common/NativeInterfaceFactory.h"
#include "common/bgq/NativeInterface.h"
#include "common/bgq/NativeInterfaceFactory.h"

#include "algorithms/geometry/Geometry.h"

#define PAMI_GEOMETRY_CLASS    PAMI::Geometry::Common

#include "algorithms/geometry/PGASCollRegistration.h"
#include "algorithms/geometry/PGASLiteCollRegistration.h"

#include "components/devices/shmem/mdls/ShmemMcombModelWorld.h"
#include "components/devices/shmem/mdls/ShmemMcstModelWorld.h"
//#include "components/devices/shmem/ShmemCollDesc.h"
#include "components/devices/shmem/mdls/ShmemColorMcstModel.h"
#include "components/devices/shmem/mdls/bgq/MultiColorMulticastModel.h"

#include "components/devices/bgq/mu2/model/CollectiveMulticastDmaModel.h"
#include "components/devices/bgq/mu2/model/CollectiveMulticombineDmaModel.h"
//#include "components/devices/bgq/mu2/model/CollectiveMcomb2Device.h"
#include "components/devices/bgq/mu2/model/CNShmemColl.h"
#include "components/devices/bgq/mu2/model/AMMulticastModel.h"
#include "components/devices/bgq/mu2/model/MUMultisync.h"
#include "components/devices/bgq/mu2/model/RectangleMultisyncModel.h"
#include "components/devices/bgq/mu2/model/ManytomanyModel.h"
#include "components/devices/bgq/mu2/model/NullMultisync.h"
#include "components/devices/bgq/mu2/model/NullMulticombine.h"

namespace PAMI
{
  typedef Geometry::Common                     BGQGeometry;

  typedef MemoryAllocator<224,  64, 1> ProtocolAllocator; /// \todo How much do we really need?  Is there a better way?
  typedef MemoryAllocator<4160, 64, 1> BigProtocolAllocator; /// \todo How much do we really need?  Is there a better way?
  typedef MemoryAllocator<632,  64, 1> MidProtocolAllocator; /// \todo How much do we really need?  Is there a better way?

  typedef Device::MU::Context MUDevice;

  typedef BGQNativeInterface < MUDevice,
  Device::MU::MulticastModel<true>, // all-sided
  Device::MU::MultisyncModel<false, false>,
    Device::MU::MulticombineModel<Device::MU::AllreducePacketModel, false, false>,
  BigProtocolAllocator> MUGlobalNI;

  typedef BGQNativeInterfaceAS < MUDevice,
    Device::MU::MulticastDmaModel,
    Device::MU::Rectangle::MultisyncModel,
    //Device::MU::CollectiveMulticombineDmaModel 
    Device::MU::NullMulticombineModel,
    BigProtocolAllocator> MUDputNI;

  typedef BGQNativeInterfaceAS < MUDevice,
  Device::MU::CollectiveMulticastDmaModel,
  Device::MU::MUMultisyncModel,
  Device::MU::CollectiveMulticombineDmaModel,
  //Device::MU::MulticombineModel<Device::MU::AllreducePacketModel, false, false>
  BigProtocolAllocator
  > MUGlobalDputNI;

/*  typedef BGQNativeInterfaceAS < MUDevice,
  Device::MU::CollectiveMcast2Device,
  Device::MU::CollectiveMsync2Device,
  Device::MU::CollectiveMcomb2Device,
  BigProtocolAllocator
  > MUShmemGlobalDputNI;
*/
/*  typedef BGQNativeInterfaceAS < MUDevice,
  Device::MU::CollectiveMcast2Device,
  Device::MU::CollectiveMsync2Device,
  Device::MU::CNShmemColl,
  MidProtocolAllocator
  > MUShmemGlobalDputNI;
*/
  typedef BGQNativeInterfaceCNShmem < MUDevice, Device::MU::CNShmemColl > MUShmemGlobalDputNI;
  
  //typedef PAMI::Device::Shmem::ShmemCollDesc <PAMI::Counter::Native> ShmemCollDesc;
  //typedef PAMI::Device::Shmem::ShmemCollDesc <PAMI::Counter::BGQ::IndirectL2> ShmemCollDesc;

  typedef BGQNativeInterface < MUDevice,
    Device::MU::ShortAMMulticastModel,
    //Device::MU::MultisyncModel<false, false>,
    //Device::MU::MulticombineModel<Device::MU::AllreducePacketModel, false, false>
    Device::MU::NullMultisyncModel,
    Device::MU::NullMulticombineModel,
    BigProtocolAllocator
    > MUAMMulticastNI;

  typedef Fifo::FifoPacket <32, 160> ShmemPacket;
  //typedef Fifo::LinearFifo<ShmemPacket, PAMI::Counter::BGQ::IndirectL2> ShmemFifo;
  //typedef Fifo::LinearFifo<ShmemPacket, PAMI::Counter::BGQ::IndirectL2, 128, Wakeup::BGQ> ShmemFifo;
  typedef Fifo::WrapFifo<ShmemPacket, PAMI::BoundedCounter::BGQ::IndirectL2Bounded, 128, Wakeup::BGQ> ShmemFifo;
  typedef Device::ShmemDevice<ShmemFifo, Counter::BGQ::IndirectL2, Device::Shmem::BgqShaddrReadOnly> ShmemDevice;
  //typedef Device::Shmem::PacketModel<ShmemDevice> ShmemPacketModel;
  typedef Device::Shmem::BgqShaddrPacketModel<ShmemDevice> ShmemPacketModel;
  typedef Device::Shmem::DmaModel<ShmemDevice> ShmemDmaModel;

  typedef Protocol::Send::Eager <ShmemPacketModel> ShmemEagerBase;
  typedef PAMI::Protocol::Send::SendWrapperPWQ < ShmemEagerBase > ShmemEager;

  typedef PAMI::Device::Shmem::ShmemColorMcstModel<ShmemDevice> ShaddrMcstModel;

  typedef PAMI::Device::Shmem::MultiColorMulticastModel<ShmemDevice, PAMI::Counter::BGQ::IndirectL2> ShaddrMultiColorMulticastModel;

  typedef  PAMI::BGQNativeInterfaceASMultiDevice < MUDevice,
  ShmemDevice,
  Device::MU::MulticastDmaModel,
    /*ShaddrMcstModel,*/
  ShaddrMultiColorMulticastModel,
  Device::MU::MultisyncModel<false, false>,
  //Device::MU::MulticombineModel<PAMI::Device::MU::AllreducePacketModel, false, false>,
  Device::MU::NullMulticombineModel,  
  BigProtocolAllocator > MUShmemAxialDputNI;

  typedef BGQNativeInterfaceASMultiDevice < MUDevice,
    ShmemDevice,
    Device::MU::MulticastDmaModel,
    ShaddrMultiColorMulticastModel,
    Device::MU::NullMultisyncModel,
    Device::MU::NullMulticombineModel,
    BigProtocolAllocator,
    false> MUShmemDputNI;

  // shmem active message over p2p eager
  typedef PAMI::NativeInterfaceActiveMessage<ShmemEager> ShmemNI_AM;
  // shmem allsided over p2p eager
  typedef PAMI::NativeInterfaceAllsided<ShmemEager> ShmemNI_AS;

  // shmem active message over p2p eager
  typedef PAMI::NativeInterfaceActiveMessage<ShmemEager, 2> ShmemNI_AM_AMC;
  // shmem allsided over p2p eager
  typedef PAMI::NativeInterfaceAllsided<ShmemEager, 2> ShmemNI_AS_AMC;

  typedef Protocol::Send::Eager <Device::MU::PacketModel> MUEagerBase;
  typedef PAMI::Protocol::Send::SendWrapperPWQ < MUEagerBase > MUEager;

  // MU active message over p2p eager
  typedef PAMI::NativeInterfaceActiveMessage<MUEager> MUNI_AM;
  // MU allsided over p2p eager
  typedef PAMI::NativeInterfaceAllsided<MUEager> MUNI_AS;

  // MU active message over p2p eager
  typedef PAMI::NativeInterfaceActiveMessage<MUEager, 2> MUNI_AM_AMC;
  // MU allsided over p2p eager
  typedef PAMI::NativeInterfaceAllsided<MUEager, 2> MUNI_AS_AMC;

  // shmem + MU composite active message over p2p eager
  typedef PAMI::NativeInterfaceActiveMessage< PAMI::Protocol::Send::SendPWQ< Protocol::Send::Send> > CompositeNI_AM;
  // shmem + MU composite allsided over p2p eager
  typedef PAMI::NativeInterfaceAllsided< PAMI::Protocol::Send::SendPWQ< Protocol::Send::Send> > CompositeNI_AS;

  // shmem + MU composite active message over p2p eager
  typedef PAMI::NativeInterfaceActiveMessage< PAMI::Protocol::Send::SendPWQ< Protocol::Send::Send>, 2 > CompositeNI_AM_AMC;
  // shmem + MU composite allsided over p2p eager
  typedef PAMI::NativeInterfaceAllsided< PAMI::Protocol::Send::SendPWQ< Protocol::Send::Send>, 2 > CompositeNI_AS_AMC;

  // PGAS over MU
  typedef xlpgas::CollectiveManager<MUNI_AM> MU_NBCollManager;
  typedef CollRegistration::PGASRegistration < BGQGeometry,
  MUNI_AM,
  MidProtocolAllocator,
  MUEager,
  ShmemEager,
  MUDevice,
  ShmemDevice,
  MU_NBCollManager,void > MU_PGASCollreg;

  // PGAS over Shmem
  typedef xlpgas::CollectiveManager<ShmemNI_AM> Shmem_NBCollManager;
  typedef CollRegistration::PGASRegistration < BGQGeometry,
  ShmemNI_AM,
  MidProtocolAllocator,
  ShmemEager,
  ShmemEager,
  ShmemDevice,
  ShmemDevice,
  Shmem_NBCollManager, void> Shmem_PGASCollreg;

  // PGAS over Composite Shmem+MU
  typedef xlpgas::CollectiveManager<CompositeNI_AM> Composite_NBCollManager;
  typedef CollRegistration::PGASRegistration < BGQGeometry,
  CompositeNI_AM,
  MidProtocolAllocator,
  MUEager,
  ShmemEager,
  MUDevice,
  ShmemDevice,
  Composite_NBCollManager, void > Composite_PGASCollreg;

  // PGAS Lite
  // PGAS over MU
  typedef CollRegistration::Lite::PGASRegistration < BGQGeometry,
  MUNI_AM,
  MidProtocolAllocator,
  MUEager,
  ShmemEager,
  MUDevice,
  ShmemDevice,
  MU_NBCollManager,void > MU_PGASLiteCollreg;

  // PGAS over Shmem
  typedef CollRegistration::Lite::PGASRegistration < BGQGeometry,
  ShmemNI_AM,
  MidProtocolAllocator,
  ShmemEager,
  ShmemEager,
  ShmemDevice,
  ShmemDevice,
  Shmem_NBCollManager, void> Shmem_PGASLiteCollreg;

  // PGAS over Composite Shmem+MU
  typedef CollRegistration::Lite::PGASRegistration < BGQGeometry,
  CompositeNI_AM,
  MidProtocolAllocator,
  MUEager,
  ShmemEager,
  MUDevice,
  ShmemDevice,
  Composite_NBCollManager, void > Composite_PGASLiteCollreg;


  typedef PAMI::Barrier::IndirectCounter<PAMI::Counter::BGQ::IndirectL2NoWakeup> Barrier_Type;
//typedef PAMI::Barrier::Indirect<PAMI::Barrier::Counter<PAMI::Counter::BGQ::L2> > Barrier_Type;

  typedef PAMI::Device::AtomicBarrierMdl<Barrier_Type>                           ShmemMsyncModel;

  typedef PAMI::Device::Shmem::ShmemMcombModelWorld <ShmemDevice> ShmemMcombModel;
  typedef PAMI::Device::Shmem::ShmemMcstModelWorld <ShmemDevice> ShmemMcstModel;
  typedef PAMI::Device::Shmem::ShortMcombMessage <ShmemDevice> ShmemMcombMessage;
  
  typedef BGQNativeInterfaceAS <ShmemDevice, ShmemMcstModel, ShmemMsyncModel, ShmemMcombModel,
  BigProtocolAllocator> AllSidedShmemNI;

  typedef BGQNativeInterfaceM2M <MUDevice, Device::MU::ManytomanyModel<size_t, 1> > M2MNISingle;
  typedef BGQNativeInterfaceM2M <MUDevice, Device::MU::ManytomanyModel<size_t, 0> > M2MNIVectorLong;
  typedef BGQNativeInterfaceM2M <MUDevice, Device::MU::ManytomanyModel<int,    0> > M2MNIVectorInt;

}

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#endif
