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

#ifndef __common_bgp_LockBoxFactory_h__
#define __common_bgp_LockBoxFactory_h__

#include "Mapping.h"
#include <spi/bgp_SPI.h>

// These define the range of lockboxes we're allowed to use.
#define LBX_MIN_LOCKBOX		0
#define LBX_MAX_NUMLOCKBOX	256

////////////////////////////////////////////////////////////////////////
///  \file common/bgp/LockBoxFactory.h
///  \brief Implementation of BGP AtomicFactory scheme(s).
///
///  This object is a portability layer that implements allocation
///  of lockboxes for use in Mutexes, Barriers, and Atomic (counters).
///
///  Namespace:  PAMI, the messaging namespace
///  Notes:  This is currently indended for use only by the lock manager
///
////////////////////////////////////////////////////////////////////////
namespace PAMI {
namespace Atomic {
namespace BGP {
        // These may need to be put in a (more) common header... somewhere...
        typedef enum {
                LBX_NODE_SCOPE,
                LBX_NODE_PROC_SCOPE,
                LBX_NODE_CORE_SCOPE,
                LBX_PROC_SCOPE,
        } lbx_scope_t;
        /**
         * \brief Structure used to pass implementation parameters
         */
        struct atomic_factory_t {
                unsigned myProc;	/**< current process number */
                unsigned masterProc;	/**< chosen master process */
                int numProc;		/**< num active processes */
                int numCore;		/**< num active cores */
                unsigned coreXlat[NUM_CORES]; /**< translate process to core */
                int coreShift;		/**< translate core to process */
        };
        static const int MAX_NUMLOCKBOXES = 5;

        class LockBoxFactory {
        private:
                /// \brief Storage for the implementation parameters
                atomic_factory_t _factory;
                size_t __masterRank;
                int __numProc;
                bool __isMasterRank;
        public:
                LockBoxFactory() { }

                ~LockBoxFactory() {}

                inline void init(PAMI::Mapping *mapping) {
                        // Compute all implementation parameters,
                        // i.e. fill-in _factory struct.
                        pami_result_t rc;
                        pami_coord_t coord;
                        size_t i;
                        int ncores = Kernel_ProcessorCount();
                        size_t t;
                        rc = mapping->nodePeers(t);
                        PAMI_assert(rc == PAMI_SUCCESS);
                        _factory.numCore = 0;
                        _factory.numProc = 0;
                        _factory.masterProc = (unsigned)-1;
                        //
                        // t | VN core | DUAL | SMP |
                        //---+---------+------+-----+---
                        // 0 |    0    |  0   |  0  |
                        // 1 |    1    |  2   |  -  |
                        // 2 |    2    |  -   |  -  |
                        // 3 |    3    |  -   |  -  |
                        //
                        int shift = (ncores == 4 ? 2 : (ncores == 2 ? 1 : 0));
                        _factory.coreShift = shift;
                        PAMI::Interface::Mapping::nodeaddr_t n;
                        mapping->nodeAddr(n);
                        for (i = 0; i < t; ++i) {
                                size_t r;
                                n.local = i;
                                rc = mapping->node2task(n, r);
                                PAMI_assert(rc == PAMI_SUCCESS);
                                if (r != (size_t)-1) {
                                        _factory.numCore += ncores;
                                        ++_factory.numProc;
                                        mapping->task2network(r, &coord, PAMI_N_TORUS_NETWORK);
                                        _factory.coreXlat[i] = coord.u.n_torus.coords[3] << shift;
                                        if (r == mapping->task()) {
                                                _factory.myProc = i;
                                        }
                                        if (_factory.masterProc == (unsigned)-1) {
                                                __masterRank = r;
                                                _factory.masterProc = i;
                                        }
                                }
                        }
                        __numProc = _factory.numProc;
                        __isMasterRank = (__masterRank == mapping->task());
                }

                inline unsigned masterProc() { return _factory.masterProc; }
                inline int coreShift() { return _factory.coreShift; }
                inline int numCore() { return _factory.numCore; }
                inline int numProc() { return _factory.numProc; }
                inline unsigned coreXlat(unsigned x) { return _factory.coreXlat[x]; }
                inline bool isMasterRank() { return __isMasterRank; }

                /**
                 * \brief Generic LockBox allocation
                 *
                 *
                 * Warning: this allocation scheme must be used in the same way,
                 * same sequence, on every process in the node. This is the only
                 * way to ensure that each process has the same lockbox(es) for
                 * a given object. This is why all lockboxes (mutexes, barriers, ...)
                 * are allocated all at once at init time.
                 *
                 * When process-scoped lockboxes are requested, each
                 * process on the node will request a different lockbox
                 * number. This is accomplished by adding
                 * numLockBoxes * lm->myProc to the desired lockbox
                 * number and multiplying lockSpan by lm->numProc.
                 * In this way, each process will be asking for different
                 * lockboxes, even if they have to iterate to find a
                 * free lockbox (set). In order to ensure all processes
                 * stay in agreement on the next lockbox number, after
                 * allocation the (next) lockbox number will have
                 * numLockBoxes * lm->myProc subtracted from it.
                 *
                 * Only one core from each process can allocate
                 * lockboxes, and it must be the lowest-numbered
                 * core of the process.  This is to ensure that
                 * Kernel_AllocateLockBox() can properly handle the
                 * LOCKBOX_ORDERED_ALLOC flag.
                 *
                 * \param[in] lm	Implementation parameters
                 * \param[out] p	Place to record lockbox id(s)
                 * \param[in] numLockBoxes Number of lockboxes to get
                 * \param[in] scope	Scope of lockboxes
                 */
                inline void lbx_alloc(void **p, int numLockBoxes,
                                        lbx_scope_t scope) {
                        static int desiredLock = LBX_MIN_LOCKBOX;
                        static uint32_t *lockp[NUM_CORES * MAX_NUMLOCKBOXES];
                        int lockSpan = numLockBoxes;
                        unsigned flags = 0;
                        PAMI_assert_debug(numLockBoxes <= MAX_NUMLOCKBOXES);
                        switch(scope) {
                        case LBX_NODE_SCOPE:
                        case LBX_NODE_PROC_SCOPE:
                        case LBX_NODE_CORE_SCOPE:
                                // Node-scoped lockboxes...
                                flags = (_factory.coreXlat[_factory.masterProc] << 4) | _factory.numProc | LOCKBOX_ORDERED_ALLOC;

                                break;
                        case LBX_PROC_SCOPE:
                                // Process-scoped lockboxes...
                                // Allocate the entire block on all processes, but
                                // only return our specific lock(s).
                                flags = (_factory.coreXlat[_factory.myProc] << 4) | 1;
                                // ensure all get different lockboxes
                                lockSpan *= _factory.numProc;
                                break;
                        default:
                                PAMI_abortf("Invalid lockbox scope");
                                break;
                        }
                        *p = NULL;
                        int rc = 0;
                        while (desiredLock < LBX_MAX_NUMLOCKBOX) {
                                rc = Kernel_AllocateLockBox(desiredLock,
                                                lockSpan,
                                                lockp,
                                                flags);
                                desiredLock += lockSpan;
                                if (rc == EAGAIN) {
                                        continue;
                                } else if(rc != 0) {
                                        perror("Kernel_AllocateLockBox");
                                        PAMI_abortf("Fatal: allocLockBoxes(%d) rc=%d p=%p", desiredLock-1, rc, p);
                                }
                                switch(scope) {
                                case LBX_NODE_SCOPE:
                                case LBX_NODE_PROC_SCOPE:
                                case LBX_NODE_CORE_SCOPE:
                                        // Node-scoped lockboxes...
                                        // we get exactly what we asked for.
                                        memcpy(p, &lockp[0], numLockBoxes * sizeof(*p));
                                        break;
                                case LBX_PROC_SCOPE:
                                        // Process-scoped lockboxes...
                                        // Take our specific lock out of the entire block.
                                        memcpy(p, &lockp[numLockBoxes * _factory.myProc], numLockBoxes * sizeof(*p));
                                        break;
                                default:
                                        PAMI_abortf("Unsupported lockbox scope");
                                        break;
                                }
                                return; // success.
                        }
                        PAMI_abortf("Fatal: Kernel_AllocateLockBox: no available lockboxes");
                }
        }; // class LockBoxFactory

}; // namespace BGP
}; // namespace Atomic
}; // namespace PAMI

#endif /* __pami_bgp_lockboxfactory_h__ */
