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

#ifndef __common_bgq_L2AtomicFactory_h__
#define __common_bgq_L2AtomicFactory_h__

#include "Mapping.h"
#include "Topology.h"
#include "components/memory/MemoryManager.h"
#include "spi/include/kernel/memory.h"
#include <sys/ioctl.h>
#include "cnk/include/SPI_syscalls.h"

#define BGQ_WACREGION_SIZE	(256)	// the number of L2 Atomics (uint64_t)

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

// These define the range of L2Atomics we're allowed to use.
// We might need extra space for the WAC Region.
// Must take into account (considerable) waste from large alignment value for WAC,
// Using 2x as an upper-bound.
#define L2A_MAX_NUMNODEL2ATOMIC(nproc,nctx)	(16*256+(nproc)*(nctx)*2*BGQ_WACREGION_SIZE) ///< max number of node-scope atomics
#define L2A_MAX_NUMPROCL2ATOMIC	(16*256)	///< max number of proc(etc)-scope atomics

////////////////////////////////////////////////////////////////////////
///  \file common/bgq/L2AtomicFactory.h
///  \brief Implementation of BGQ AtomicFactory scheme(s).
///
///  This object is a portability layer that implements allocation
///  of L2Atomics for use in Mutexes, Barriers, and Atomic (counters).
///
///  Namespace:  PAMI, the messaging namespace
///  Notes:  This is currently indended for use only by the lock manager
///
////////////////////////////////////////////////////////////////////////


namespace PAMI {
namespace Atomic {
namespace BGQ {
        // These may need to be put in a (more) common header... somewhere...
        typedef enum {
                L2A_NODE_SCOPE,
                L2A_NODE_PROC_SCOPE,
                L2A_NODE_PTHREAD_SCOPE,
                L2A_NODE_CORE_SCOPE,
                L2A_NODE_SMT_SCOPE,

                L2A_PROC_SCOPE,
                L2A_PROC_CORE_SCOPE,
                L2A_PROC_SMT_SCOPE,
                L2A_PROC_PTHREAD_SCOPE,

                L2A_CORE_SCOPE,
                L2A_CORE_SMT_SCOPE,

                L2A_SMT_SCOPE,

                L2A_PTHREAD_SCOPE,
        } l2x_scope_t;
        /**
         * \brief Structure used to pass implementation parameters
         */
        struct atomic_factory_t {
                size_t myProc;		/**< current process number */
                size_t masterProc;	/**< chosen master process */
                size_t numProc;		/**< num active processes */
                size_t numCore;		/**< num active cores */
                size_t coreXlat[PAMI_MAX_PROC_PER_NODE]; /**< translate process to core */
                size_t coreShift;	/**< translate core to process */
        };

        class L2AtomicFactory {
        private:
                /// \brief Storage for the implementation parameters
                atomic_factory_t _factory;
                pami_task_t __masterRank;
                size_t __numProc;
                bool __isMasterRank;

		inline void *get_heap_l2atomic(PAMI::Memory::MemoryManager *heap_mm,
						size_t size) {
			pami_result_t rc;
			uint64_t krc;
			struct _l2maptry {
				struct _l2maptry *next;
			} *failed, *next;
			failed = NULL;
			void *try1 = NULL;
			int tries = 0;
			size_t span = 0;
			if (sizeof(uint64_t) * size < 1024*1024) {
				span = 1024*1024 - sizeof(uint64_t) * size;
			}
			krc = 0;
			do {
				rc = heap_mm->memalign(&try1,
					sizeof(uint64_t),
					sizeof(uint64_t) * size,
					NULL,
					PAMI::Memory::MemoryManager::memzero, NULL);
				if (rc != PAMI_SUCCESS) break;
				krc = Kernel_L2AtomicsAllocate(try1,
						sizeof(uint64_t) * size);
				if (krc == 0) break;
				++tries;
				next = (struct _l2maptry *)try1;
				next->next = failed;
				failed = next;
				if (span) {
					// bump 1M...
					rc = heap_mm->memalign(&try1,
						sizeof(uint64_t),
						span,
						NULL,
						PAMI::Memory::MemoryManager::memzero, NULL);
					if (rc != PAMI_SUCCESS) break;
					next = (struct _l2maptry *)try1;
					next->next = failed;
					failed = next;
				}
			} while (true); // might be some max num attempts...

			while (failed) {
				next = failed->next;
				heap_mm->free(failed);
				failed = next;
			}

			PAMI_assert_alwaysf(rc == PAMI_SUCCESS,
					"Failed to get memory for _l2proc "
					"after %d tries, asked size %zu", tries,
					sizeof(uint64_t) * size);
                        PAMI_assert_alwaysf(krc == 0,
				"Failed to map process L2 Atomic region "
				"after %d tries, %p (%zd): %ld", tries,
				try1, sizeof(uint64_t) * size, krc);
#if 0
			// debug message
			if (tries) {
				fprintf(stderr, "Got l2atomic heap memory %p size %zu "
					"after %d tries\n",
					try1, sizeof(uint64_t) * size, tries);
			}
#endif
			return try1;
		}

		static int l2atomic_attr(int fd) {
			uint64_t dummy = 1;
			// dummy is ignored, the ioctl always sets the flag 'true'
			return ioctl(fd, FIOBGQATOMIC, &dummy);
		}

        public:
		PAMI::Memory::GenMemoryManager __nodescoped_mm;
		PAMI::Memory::GenMemoryManager __procscoped_mm;

                L2AtomicFactory() { }

      ////////////////////////////////////////////////////////////////////////////////
      /// \envs{pami,bgq,BGQ L2 Atomics and Wakeup}
      /// This is some general documentation about the L2 atomics and wakeup
      /// environment variables.
      ///
      /// \env{bgq,PAMI_BGQ_PROC_L2ATOMICSIZE}
      /// Size, uint64_t, proc-scoped
      /// L2 Atomic region. May use 'K' or 'M' suffix as multiplier.
      /// \default 4K
      ///
      /// \env{bgq,PAMI_BGQ_NODE_L2ATOMICSIZE}
      /// Size, uint64_t, node-scoped
      /// L2 Atomic region. May use 'K' or 'M' suffix as multiplier.
      /// Includes WAC region (PAMI_BGQ_WU_*).
      /// \default 4K + nctx * nproc * 256
      /// \see \ref PAMI_BGQ_WU_BASELINE
      /// \see \ref PAMI_BGQ_WU_CONTEXT_COST
      /// \see \ref PAMI_BGQ_WU_PROC_DISCOUNT
      ////////////////////////////////////////////////////////////////////////////////

		/// \brief Initialize the L2AtomicFactory
		///
		/// \param[in] mm	Shmem MemoryManager
		/// \param[in] mapping	Mapping object
		/// \param[in] local	Topology for tasks local to node
		///
                inline void init(PAMI::Memory::MemoryManager *shared_mm,
                		PAMI::Memory::MemoryManager *heap_mm,
                                PAMI::Mapping *mapping, PAMI::Topology *local) {
                        pami_result_t rc;
			char *s;

                        // Must coordinate with all other processes on this node,
                        // and arrive at a common chunk of physical address memory
                        // which we all will use for allocating "L2Atomics" from.
                        // One sure way to do this is to allocate shared memory.
			size_t size;
                        size_t t = local->size();

                        size = L2A_MAX_NUMPROCL2ATOMIC;
			if ((s = getenv("PAMI_BGQ_PROC_L2ATOMICSIZE"))) {
                		char *u = NULL;
                		size = strtoull(s, &u, 0);
                		if (*u == 'm' || *u == 'M') size *= 1024 * 1024;
                		else if (*u == 'k' || *u == 'K') size *= 1024;
			}
			void *mem = get_heap_l2atomic(heap_mm, size);

			rc = __procscoped_mm.init2(heap_mm, mem,
					sizeof(uint64_t) * size,
					"/L2AtomicFactory-priv",
					sizeof(uint64_t),
					PAMI::Memory::PAMI_MM_L2ATOMIC);
			PAMI_assert_alwaysf(rc == PAMI_SUCCESS,
				"Failed to init mm for _l2proc, %p size %zu",
				mem, sizeof(uint64_t) * size);

#if 0
        size_t num_ctx = __MUGlobal.getMuRM().getPerProcessMaxPamiResources();
        // may need to factor in others such as shmem?
#else
        size_t num_ctx = 256 / Kernel_ProcessCount();
#endif
                        size = L2A_MAX_NUMNODEL2ATOMIC(t,num_ctx);
			if ((s = getenv("PAMI_BGQ_NODE_L2ATOMICSIZE"))) {
                		char *u = NULL;
                		size = strtoull(s, &u, 0);
                		if (*u == 'm' || *u == 'M') size *= 1024 * 1024;
                		else if (*u == 'k' || *u == 'K') size *= 1024;
			}
			if ((shared_mm->attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) != 0) {
				rc = static_cast<PAMI::Memory::SharedMemoryManager *>(shared_mm)->
					memalign_attrfn(&mem, sizeof(uint64_t),
							sizeof(uint64_t) * size, 
							"/L2AtomicFactory-shared",
							PAMI::Memory::MemoryManager::memzero,
							NULL, l2atomic_attr);
                        	PAMI_assert_alwaysf(rc == PAMI_SUCCESS,
                                	"Failed to get shmem for _l2node, asked size %zu",
                                	sizeof(uint64_t) * size);
			} else {
				mem = get_heap_l2atomic(heap_mm, size);
			}

			rc = __nodescoped_mm.init2(shared_mm, mem,
					sizeof(uint64_t) * size,
					"/L2AtomicFactory-shared",
					sizeof(uint64_t),
					PAMI::Memory::PAMI_MM_L2ATOMIC);
                        PAMI_assert_alwaysf(rc == PAMI_SUCCESS,
                                "Failed to get init mm for _l2node, %p size %zu",
                                mem, sizeof(uint64_t) * size);

#if 0
fprintf(stderr, "L2 Atomics, node=%zd @ %p (%zd), proc=%zd @ %p (%zd)\n",
__nodescoped_mm.size(),
__nodescoped_mm.base(),
__nodescoped_mm.available(),
__procscoped_mm.size(),
__procscoped_mm.base(),
__procscoped_mm.available());
#endif
                        // Compute all implementation parameters,
                        // i.e. fill-in _factory struct.
                        PAMI::Interface::Mapping::nodeaddr_t addr;
                        pami_task_t ranks[PAMI_MAX_PROC_PER_NODE];
                        size_t i;

                        /** \todo #warning This needs a proper CNK function for number of threads per process, when it exists... */
                        int ncores = (PAMI_MAX_PROC_PER_NODE / Kernel_ProcessCount());
                        // int ncores = Kernel_ThreadCount();

                        //int t = mapping->vnpeers(ranks);
                        // There should be a more elegent way to do this... yuk.
                        for (i = 0; i < t; ++i) {
                                ranks[i] = local->index2Rank(i);
                        }

                        _factory.numCore = 0;
                        _factory.numProc = 0;
                        _factory.masterProc = (size_t)-1;
                        //
                        // t | VN core | DUAL | SMP |
                        //---+---------+------+-----+---
                        // 0 |    0    |  0   |  0  |
                        // 1 |    1    |  2   |  -  |
                        // 2 |    2    |  -   |  -  |
                        // 3 |    3    |  -   |  -  |
                        //
                        // PAMI_assert((ncores & (ncores - 1) == 0);
                        int shift = ffs(ncores);
                        PAMI_assert_alwaysf(shift > 0, "Internal error: no cores in process?");
                        --shift;
                        _factory.coreShift = shift;
                        for (i = 0; i < t; ++i) {
                                if (ranks[i] >= 0) {
                                        _factory.numCore += ncores;
                                        ++_factory.numProc;
                                        rc = mapping->task2node(ranks[i], addr);
                                        PAMI_assert_alwaysf(rc == PAMI_SUCCESS, "[%zu] task2node(%d, addr) failed\n", i, ranks[i]);
                                        size_t p;
                                        rc = mapping->node2peer(addr, p);
                                        PAMI_assert_alwaysf(rc == PAMI_SUCCESS, "[%zu] node2peer(addr, p) failed\n", i);
                                        _factory.coreXlat[i] = p << shift;
                                        if (ranks[i] == mapping->task()) {
                                                _factory.myProc = i;
                                        }
                                        if (_factory.masterProc == (size_t)-1) {
                                                __masterRank = ranks[i];
                                                _factory.masterProc = i;
                                        }
                                }
                        }
                        __numProc = _factory.numProc;
                        __isMasterRank = (__masterRank == mapping->task());
                }

                ~L2AtomicFactory() {}

                inline size_t masterProc() { return _factory.masterProc; }
                inline size_t coreShift() { return _factory.coreShift; }
                inline size_t numCore() { return _factory.numCore; }
                inline size_t numProc() { return _factory.numProc; }
                inline size_t coreXlat(size_t x) { return _factory.coreXlat[x]; }
                inline bool isMasterRank() { return __isMasterRank; }

                /// callers must ensure all use the same order
                inline pami_result_t l2x_alloc(void **p, int numAtomics, l2x_scope_t scope) {
                        PAMI::Memory::MemoryManager *arena = &__procscoped_mm;
			l2x_scope_t arena_scope = L2A_PROC_SCOPE;
                        switch(scope) {
                        case L2A_NODE_SCOPE:
                        case L2A_NODE_PROC_SCOPE:
                        case L2A_NODE_PTHREAD_SCOPE:
                        case L2A_NODE_CORE_SCOPE:
                        case L2A_NODE_SMT_SCOPE:
                                // Node-scoped L2Atomics...
                                // barrier... ??
                                arena = &__nodescoped_mm;
				arena_scope = L2A_NODE_SCOPE;
                                break;
                        case L2A_PROC_SCOPE:
                        case L2A_PROC_CORE_SCOPE:
                        case L2A_PROC_SMT_SCOPE:
                        case L2A_PROC_PTHREAD_SCOPE:
                                // Process-scoped L2Atomics...
                                // Allocate the entire block on all processes, but
                                // only return our specific lock(s).
                                // ensure all get different L2Atomics
                                break;
                        case L2A_CORE_SCOPE:
                        case L2A_CORE_SMT_SCOPE:
                                /// \todo what are core-scoped atomics?
                                //break;
                        case L2A_SMT_SCOPE:
                                /// \todo what are smt-scoped atomics?
                                //break;
                        case L2A_PTHREAD_SCOPE:
                                /// \todo what are pthread-scoped atomics?
                                //break;
                        default:
                                PAMI_abortf("Invalid L2Atomic scope");
                                break;
                        }
			return l2x_mm_alloc(arena, arena_scope, p, numAtomics, scope);
                }

		/// \todo #warning HACK to workaround using MemoryManager for L2Atomics
		///
                /// callers must ensure all use the same order
                inline pami_result_t l2x_mm_alloc(PAMI::Memory::MemoryManager *mm,
					l2x_scope_t mmscope,
					void **p, int numAtomics, l2x_scope_t scope) {
                        int lockSpan = numAtomics;
                        switch(mmscope) {
                        case L2A_NODE_SCOPE:
                                break;
                        case L2A_PROC_SCOPE:
                                break;
                        default:
                                PAMI_abortf("Invalid L2Atomic mm scope");
                                break;
                        }
                        switch(scope) {
                        case L2A_NODE_SCOPE:
                        case L2A_NODE_PROC_SCOPE:	// barrier only
                        case L2A_NODE_PTHREAD_SCOPE:	// barrier only
                        case L2A_NODE_CORE_SCOPE:	// barrier only
                        case L2A_NODE_SMT_SCOPE:	// barrier only
                                // Node-scoped L2Atomics...
                                // barrier... ??
				PAMI_assert_alwaysf(mmscope == L2A_NODE_SCOPE,
					"l2x_mm_alloc called with incompatible mm");
                                break;
                        case L2A_PROC_SCOPE:
                        case L2A_PROC_CORE_SCOPE:	// barrier only
                        case L2A_PROC_SMT_SCOPE:	// barrier only
                        case L2A_PROC_PTHREAD_SCOPE:	// barrier only
                                // Process-scoped L2Atomics...
                                // Allocate the entire block on all processes, but
                                // only return our specific lock(s).
                                // ensure all get different L2Atomics
				if (mmscope == L2A_NODE_SCOPE) {
                                	lockSpan *= _factory.numProc;
				}
                                break;
                        case L2A_CORE_SCOPE:
                        case L2A_CORE_SMT_SCOPE:	// barrier only
                                /// \todo what are core-scoped atomics?
                                //break;
                        case L2A_SMT_SCOPE:
                                /// \todo what are smt-scoped atomics?
                                //break;
                        case L2A_PTHREAD_SCOPE:
                                /// \todo what are pthread-scoped atomics?
                                //break;
                        default:
                                PAMI_abortf("Invalid L2Atomic scope");
                                break;
                        }
			uint64_t *v = NULL;
			mm->memalign((void **)&v, sizeof(uint64_t),
						lockSpan * sizeof(uint64_t));
			if (v == NULL) {
                                return PAMI_EAGAIN;
                        }
			int i;
                        switch(scope) {
                        case L2A_NODE_SCOPE:
                        case L2A_NODE_PROC_SCOPE:
                        case L2A_NODE_PTHREAD_SCOPE:
                        case L2A_NODE_CORE_SCOPE:
                        case L2A_NODE_SMT_SCOPE:
                                // Node-scoped L2Atomics...
                                // we get exactly what we asked for.
				*p = v;
                                // barrier... ??
                                break;
                        case L2A_PROC_SCOPE:
                        case L2A_PROC_CORE_SCOPE:
                        case L2A_PROC_SMT_SCOPE:
                        case L2A_PROC_PTHREAD_SCOPE:
                                // Process-scoped L2Atomics...
                                // Take our specific lock out of the entire block.
				if (mmscope == L2A_NODE_SCOPE) {
                        		i = (numAtomics * _factory.myProc);
					*p = &v[i];
				} else {
					*p = v;
				}
                                break;
                        case L2A_CORE_SCOPE:
                        case L2A_CORE_SMT_SCOPE:
                                /// \todo what are core-scoped atomics?
                                //break;
                        case L2A_SMT_SCOPE:
                                /// \todo what are smt-scoped atomics?
                                //break;
                        case L2A_PTHREAD_SCOPE:
                                /// \todo what are pthread-scoped atomics?
                                //break;
                        default:
                                PAMI_abortf("Invalid L2Atomic scope");
                                break;
                        }
                        return PAMI_SUCCESS;
                }

        }; // class L2AtomicFactory

}; // namespace BGQ
}; // namespace Atomic
}; // namespace PAMI

#endif // __pami_bgq_l2atomicfactory_h__
