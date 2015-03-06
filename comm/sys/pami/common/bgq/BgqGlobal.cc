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
///
/// \file common/bgq/BgqGlobal.cc
/// \brief Blue Gene/Q Global Object implementation
///

#include "Global.h"
#include "Topology.h" // need to make static members...

PAMI::Global __global;

PAMI::Mapping *PAMI::Topology::mapping = NULL;
pami_coord_t PAMI::Topology::my_coords;

PAMI::Memory::MemoryManager *PAMI::Memory::MemoryManager::heap_mm;
PAMI::Memory::MemoryManager *PAMI::Memory::MemoryManager::shared_mm;
PAMI::Memory::MemoryManager *PAMI::Memory::MemoryManager::shm_mm;

#include "components/devices/misc/ProgressFunctionMsg.h"
PAMI::Device::ProgressFunctionDev _g_progfunc_dev;

#include "common/bgq/ResourceManager.h"
PAMI::ResourceManager __pamiRM;

#include "components/devices/bgq/mu2/global/Global.h"
PAMI::Device::MU::Global __MUGlobal ( __pamiRM, __global.mapping, __global.personality, __global.mm );
void *__mu_context_cache;

const double PAMI::Time::seconds_per_cycle = 6.25e-10;

#ifdef ENABLE_MAMBO_WORKAROUNDS
extern "C" unsigned __isMambo() {return __global.personality._is_mambo? 1:0;};
#endif

#include "components/devices/bgq/mu2/model/CollectiveMulticastDmaModel.h"
#include "components/devices/bgq/mu2/model/Collective2DeviceBase.h"
#include "components/devices/bgq/mu2/model/MulticastDmaModel.h"
#include "components/devices/bgq/mu2/model/RectangleMultisyncModel.h"

char *PAMI::Device::MU::CollectiveMulticastDmaModel::_zeroBuf;
uint64_t PAMI::Device::MU::CollectiveMulticastDmaModel::_zeroBufPA;

PAMI::Device::MU::CollectiveDmaModelBase::CollState PAMI::Device::MU::CollectiveDmaModelBase::_collstate;
PAMI::Device::MU::Collective2DeviceBase::CollState PAMI::Device::MU::Collective2DeviceBase::_collstate;

PAMI::Device::MU::MUCounterSet PAMI::Device::MU::MulticastDmaModel::_mu_counterset;

PAMI::Device::MU::Rectangle::MultisyncModel::CounterState PAMI::Device::MU::Rectangle::MultisyncModel::_counterstate;


#ifdef USE_COMMTHREADS
#include "components/devices/bgq/commthread/CommThreadWakeup.h"

PAMI::Device::CommThread::Factory
		__commThreads(&__global.mm, &__global.l2atomicFactory.__nodescoped_mm);
#endif // USE_COMMTHREADS

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
