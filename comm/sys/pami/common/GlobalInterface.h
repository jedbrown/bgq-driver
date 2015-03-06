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

#ifndef __common_GlobalInterface_h__
#define __common_GlobalInterface_h__

///
/// \file common/GlobalInterface.h
/// \brief Common base global object
///
/// This global object is constructed before main() and is a container class
/// for all other classes that need to be constructed and initialized before
/// the application starts.
///

#include "Platform.h"
#include "util/common.h"
#include "Topology.h"
#include "Ptime.h"
#include "components/memory/heap/HeapMemoryManager.h"
#include "components/memory/shmem/SharedMemoryManager.h"

namespace PAMI
{
  namespace Interface
  {
    template<class T_Global>
    class Global
    {
    public:

      inline Global() :
      time(),
      topology_global(),
      topology_local(),
      topology_master(),
      heap_mm(NULL),
      shared_mm(NULL)
      {
        ENFORCE_CLASS_MEMBER(T_Global,mapping);
      }

      inline ~Global ()
      {
        // hack on top of hack on top of hack.
        topology_master.finalize();
        topology_local.finalize();
        topology_global.finalize();
        PAMI::Memory::SharedMemoryManager *smm =
        (PAMI::Memory::SharedMemoryManager *)shared_mm;
        PAMI::Memory::HeapMemoryManager *hmm =
        (PAMI::Memory::HeapMemoryManager *)heap_mm;
        if(shared_mm != heap_mm)
        {
          smm->PAMI::Memory::SharedMemoryManager::~SharedMemoryManager();
        }
        hmm->PAMI::Memory::HeapMemoryManager::~HeapMemoryManager();
      }

    public:

      PAMI::Time  time;
      PAMI::Topology  topology_global;
      PAMI::Topology  topology_local;
      PAMI::Topology  topology_master;
      PAMI::Memory::MemoryManager *heap_mm;
      PAMI::Memory::MemoryManager *shared_mm;
      /// Global location for the processor name
      char processor_name[128];

    protected:
      uint8_t _heap_mm[sizeof(PAMI::Memory::HeapMemoryManager)];
      uint8_t _shared_mm[sizeof(PAMI::Memory::SharedMemoryManager)];  // possibly
      // constructed as a HeapMemoryManager.
    };   // class Global
  };     // namespace Interface
};     // namespace PAMI

#endif // __pami_common_global_h__
