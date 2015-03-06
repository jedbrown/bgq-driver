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
#ifndef __common_socklinux_Global_h__
#define __common_socklinux_Global_h__
///
/// \file common/socklinux/Global.h
/// \brief Sockets on Linux Global Object
///
/// This global object is constructed before main() and is a container class
/// for all other classes that need to be constructed and initialized before
/// the application starts.
///

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "Platform.h"
#include "util/common.h"
#include "common/GlobalInterface.h"
#include "Mapping.h"
#include "Topology.h"
#include "components/memory/heap/HeapMemoryManager.h"
#include "components/memory/shmem/SharedMemoryManager.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)  // fprintf x
#endif

namespace PAMI
{
    class Global : public Interface::Global<PAMI::Global>
    {
      public:

        inline Global () :
	mapping()//,
	//mm()
        {
          TRACE_ERR((stderr, ">> Global::Global()\n"));

          Interface::Global<PAMI::Global>::time.init(0);
          pami_coord_t ll, ur;
          size_t min, max, num;
          size_t *ranks;
          size_t   bytes     = 1024*1024;
          size_t   pagesize  = 4096;

	  heap_mm = new (_heap_mm) PAMI::Memory::HeapMemoryManager();
	PAMI::Memory::MemoryManager::heap_mm = heap_mm;
	  // get jobid from process manager...
	  size_t jobid = 0;
	  shared_mm = new (_shared_mm) PAMI::Memory::SharedMemoryManager(jobid, heap_mm);
	PAMI::Memory::MemoryManager::shared_mm = shared_mm;
	PAMI::Memory::MemoryManager::shm_mm = &mm; // not initialized yet!

          char shmemfile[PAMI::Memory::MMKEYSIZE];
          snprintf (shmemfile, sizeof(shmemfile) - 1, "/unique-pami-global-shmem-file");

	  bytes += PAMI::Memory::GenMemoryManager::MAX_META_SIZE() * 8;

          // Round up to the page size
          size_t size = (bytes + pagesize - 1) & ~(pagesize - 1);

          mm.init(shared_mm, size, 1, 1, 0, shmemfile);

          mapping.init(min, max, num, &ranks);
          PAMI::Topology::static_init(&mapping);
          new (&topology_global) PAMI::Topology(min, max);
          topology_global.subTopologyLocalToMe(&topology_local);
          TRACE_ERR((stderr, "<< Global::Global()\n"));

          return;
        };



        inline ~Global ()
        {
        };

        inline size_t size ()
        {
          return mapping.size();
        }

     private:

       public:

        PAMI::Mapping         mapping;
	PAMI::Memory::GenMemoryManager mm;

    }; // PAMI::Global
};     // PAMI

extern PAMI::Global __global;
#undef TRACE_ERR
#endif // __common_socklinux_Global_h__
