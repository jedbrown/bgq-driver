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
#ifndef __common_bgp_Global_h__
#define __common_bgp_Global_h__
///
/// \file common/bgp/Global.h
/// \brief Blue Gene/P Global Object
///
/// This global object is constructed before main() and is a container class
/// for all other classes that need to be constructed and initialized before
/// the application starts.
///

#define PERS_SIZE 1024


#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "Platform.h"
#include "util/common.h"
#include "common/GlobalInterface.h"
#include "common/bgp/BgpPersonality.h"
#include "common/bgp/BgpMapCache.h"
#include "Mapping.h"
#include "Topology.h"
#include "common/bgp/LockBoxFactory.h"
#include "components/memory/heap/HeapMemoryManager.h"
#include "components/memory/shmem/SharedMemoryManager.h"

namespace PAMI
{
    class Global : public Interface::Global<PAMI::Global>
    {
      public:

        inline Global () :
          Interface::Global<PAMI::Global>(),
          personality (),
          mapping(personality),
	  mm(),
          _mapcache (),
          lockboxFactory()
        {
          //Interface::Global<PAMI::Global>::time.init(personality.clockMHz());
          time.init (personality.clockMHz());

	  heap_mm = new (_heap_mm) PAMI::Memory::HeapMemoryManager();
	PAMI::Memory::MemoryManager::heap_mm = heap_mm;
	  if (personality.tSize() == 1) {
		// There is no shared memory, so don't try. Fake using heap.
		shared_mm = heap_mm;
	  } else {
		shared_mm = new (_shared_mm) PAMI::Memory::SharedMemoryManager(0, heap_mm);
	  }
	PAMI::Memory::MemoryManager::shared_mm = shared_mm;
	PAMI::Memory::MemoryManager::shm_mm = &mm; // not initialized yet!


          //allocateMemory ();

          char   * shmemfile = "/pami-global-shmem";
          size_t   bytes     = 1024*1024;
          size_t   pagesize  = 4096;

	  // reserve space for 8 sub-mm's...
	  bytes += PAMI::Memory::GenMemoryManager::MAX_META_SIZE() * 8;

          // Round up to the page size
          size_t size = (bytes + pagesize - 1) & ~(pagesize - 1);

	  mm.init(shared_mm, size, 1, 1, 0, shmemfile);
	  _mapcache.init (personality, mm);

	  mapping.init(_mapcache, personality);
          lockboxFactory.init(&mapping);
          pami_coord_t ll, ur;
          size_t min, max;
          unsigned char tl[PAMI_MAX_DIMS];

          PAMI::Topology::static_init(&mapping);
          _mapcache.getMappingInit(ll, ur, min, max);
          size_t rectsize = 1;
          for (unsigned d = 0; d < mapping.globalDims(); ++d) {
                rectsize *= (ur.u.n_torus.coords[d] - ll.u.n_torus.coords[d] + 1);
          }
          tl[0] = personality.isTorusX();
          tl[1] = personality.isTorusY();
          tl[2] = personality.isTorusZ();
          tl[3] = 1;
          if (mapping.size() == rectsize) {
                new (&topology_global) PAMI::Topology(&ll, &ur, tl);
          } else if (mapping.size() == max - min + 1) {
                new (&topology_global) PAMI::Topology(min, max);
          } else {
                PAMI_abortf("failed to build global-world topology %zu::%zu(%d) / %zu..%zu", mapping.size(), rectsize, mapping.globalDims(), min, max);
          }
          topology_global.subTopologyLocalToMe(&topology_local);
        }



        inline ~Global () {};

      public:

        PAMI::BgpPersonality	personality;
        PAMI::Mapping		mapping;
	PAMI::Memory::GenMemoryManager mm;

      private:

        PAMI::BgpMapCache	_mapcache;

      public:

        PAMI::Atomic::BGP::LockBoxFactory lockboxFactory;

  };   // class Global
};     // namespace PAMI

extern PAMI::Global __global;

#endif // __pami_components_sysdep_bgp_bgpglobal_h__
