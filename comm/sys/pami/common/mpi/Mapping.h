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
 * \file common/mpi/Mapping.h
 * \brief ???
 */

#ifndef __common_mpi_Mapping_h__
#define __common_mpi_Mapping_h__

#include "Platform.h"
#include "common/BaseMappingInterface.h"
#include "common/TorusMappingInterface.h"
#include "common/NodeMappingInterface.h"
#include <pami.h>
#include "util/common.h"
#include <mpi.h>
#include <errno.h>
#include <unistd.h>
#include "components/memory/MemoryManager.h"

#define PAMI_MAPPING_CLASS PAMI::Mapping

namespace PAMI
{
#define MPI_TDIMS 1
#define MPI_LDIMS 1
    class Mapping : public Interface::Mapping::Base<Mapping>,
                       public Interface::Mapping::Torus<Mapping, MPI_TDIMS>,
                       public Interface::Mapping::Node<Mapping, MPI_LDIMS>
    {

    public:
      inline Mapping () :
        Interface::Mapping::Base<Mapping>(),
        Interface::Mapping::Torus<Mapping, MPI_TDIMS>(),
        Interface::Mapping::Node<Mapping, MPI_LDIMS>()
        {
        };
      inline ~Mapping () {};
    protected:
        size_t    _task;
        size_t    _size;
        size_t   *_peers;
        size_t    _npeers;
        uint32_t *_mapcache;
        size_t    _nSize;
        size_t    _tSize;

        static int rank_compare(const void *a, const void *b) {
                size_t *aa = (size_t *)a, *bb = (size_t *)b;
                size_t r1 = *aa;
                size_t r2 = *bb;
                int x = r1 - r2;
                return x;
        }
    public:
      inline pami_result_t init(size_t &min_rank, size_t &max_rank,
                                size_t &num_local, size_t **local_ranks)
        {
                int num_ranks;
                size_t r, q;
                int err;
                int nz, tz;
                char *host, *hosts, *s;
                int str_len = 128;

                // global process/rank info
                MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
                _size = num_ranks;
                MPI_Comm_rank(MPI_COMM_WORLD, &num_ranks);
                _task = num_ranks;

                // local node process/rank info
		pami_result_t rc;
                rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&_mapcache,
					sizeof(void *), sizeof(*_mapcache) * _size);
                PAMI_assertf(rc == PAMI_SUCCESS, "memory alloc failed, err %d", rc);
                rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&_peers,
					sizeof(void *), sizeof(*_peers) * _size);
                PAMI_assertf(rc == PAMI_SUCCESS, "memory alloc failed, err %d", rc);
                rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&host,
					sizeof(void *), str_len);
                PAMI_assertf(rc == PAMI_SUCCESS, "memory alloc failed, err %d", rc);
                rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&hosts,
					sizeof(void *), str_len * _size);
                PAMI_assertf(rc == PAMI_SUCCESS, "memory alloc failed, err %d", rc);
                err = gethostname(host, str_len);
                PAMI_assertf(err == 0, "gethostname failed, errno %d", errno);

                err = MPI_Allgather(host, str_len, MPI_BYTE, hosts, str_len, MPI_BYTE, MPI_COMM_WORLD);
                PAMI_assertf(err == 0, "allgather failed, err %d", err);

                _nSize = 0;
                _tSize = 1;
                _npeers = 0;
                for (r = 0; r < _size; ++r) {
                        // search backwards for anyone with the same hostname...
                        for (q = r - 1; (int)q >= 0 && strcmp(hosts + str_len * r, hosts + str_len * q) != 0; --q);
                        if ((int)q >= 0) {
                                // already saw this hostname... add new peer...
                                uint32_t u = _mapcache[q];
                                uint32_t t = (u & 0x0000ffff) + 1;
                                _mapcache[r] = (u & 0xffff0000) | t;
                                if (t >= _tSize) _tSize = t + 1;
                        } else {
                                // new hostname... first one for that host... give it T=0
                                _mapcache[r] = (_nSize << 16) | 0;
                                ++_nSize;
                        }
                        if (strcmp(host, hosts + str_len * r) == 0) {
                                _peers[_npeers++] = r;
                        }
                }
                PAMI::Memory::MemoryManager::heap_mm->free(host);
                PAMI::Memory::MemoryManager::heap_mm->free(hosts);

                // if all ranks are local, then see if an ENV variable
                // gives us permission to spice things up.
                nz = tz = 0;
                s = getenv("PAMI_MAPPING_TSIZE");
                if (s) {
                        tz = strtol(s, NULL, 0);
                        //fprintf(stderr,"PAMI_MAPPING_TSIZE = %s/%d\n",s,tz);
                }
                s = getenv("PAMI_MAPPING_NSIZE");
                if (s) {
                        nz = strtol(s, NULL, 0);
                        //fprintf(stderr,"PAMI_MAPPING_NSIZE = %s/%d\n",s,nz);
                }
                if (_nSize == 1 && (nz > 0 || tz > 0)) {
                        uint32_t t = 0;
                        uint32_t n = 0;
                        if (nz > 0) {
                                tz = 0;
                                // remap using N-first sequence
                                _nSize = nz;
                                for (r = 0; r < _size; ++r) {
                                        if (n >= _nSize) { ++t; n = 0; }
                                        _mapcache[r] = (n << 16) | t;
                                        ++n;
                                }
                                _tSize = t + 1;
                        } else if (tz > 0) {
                                // remap using T-first sequence
                                _tSize = tz;
                                for (r = 0; r < _size; ++r) {
                                        if (t >= _tSize) { ++n; t = 0; }
                                        _mapcache[r] = (n << 16) | t;
                                        ++t;
                                }
                                _nSize = n + 1;
                        }
                        // now, must recompute _peers, _npeers...
                        _npeers = 0;
                        n = _mapcache[_task] & 0xffff0000;
                        for (r = 0; r < _size; ++r) {
                                if ((_mapcache[r] & 0xffff0000) == n) {
                                        _peers[_npeers++] = r;
                                }
                        }
                }

                // local ranks could be represented as rectangle...
                // but, let Global.h use Topology analyze if it wants.
                *local_ranks = _peers;
                num_local = _npeers;
                // global ranks could be represented as rectangle...
                min_rank = 0;
                max_rank = _size-1;
                //
                // At this point, _mapcache[rank] -> [index1]|[index2], where:
                // (at target node)_peers[index2] -> rank
                // coordinates = (index1,index2)
                //fprintf(stderr,"_size %zu, _peers %zu, _npeers %zu, _tSize %zu, _nSize %zu\n",_size, *_peers, _npeers, _tSize, _nSize);

                return PAMI_SUCCESS;
        }
      inline size_t task_impl()
        {
          return _task;
        }
      inline size_t size_impl()
        {
          return _size;
        }
      inline pami_result_t nodeTasks_impl (size_t global, size_t & tasks)
        {
          return PAMI_UNIMPL;
        }
      inline pami_result_t nodePeers_impl (size_t & peers)
        {
                peers = _npeers;
                return PAMI_SUCCESS;
        }
      inline bool isLocal_impl (size_t task)
      {
        return isPeer (task, task_impl());
      }
      inline bool isPeer_impl (size_t task1, size_t task2)
        {
                /** \todo isPeer does not support PAMI_MAPPING_TSIZE */
                // this really needs to be global, but if we only check locally it
                // will at least work for Topology.
                Interface::Mapping::nodeaddr_t addr;
                size_t peer;

                task2node_impl(task1, addr);
                pami_result_t err1 = node2peer_impl(addr, peer);
                task2node_impl(task2, addr);
                pami_result_t err2 = node2peer_impl(addr, peer);
                return (err1 == PAMI_SUCCESS && err2 == PAMI_SUCCESS);
        }
      inline void nodeAddr_impl (Interface::Mapping::nodeaddr_t & address)
        {
                pami_result_t err = task2node_impl(_task, address);
                PAMI_assertf(err == PAMI_SUCCESS, "Internal error, my task does not exist");
        }
      inline pami_result_t task2node_impl (size_t task, Interface::Mapping::nodeaddr_t & address)
        {
                uint32_t x = _mapcache[task];
                address.global = x >> 16;
                address.local = x & 0x0000ffff;
                return PAMI_SUCCESS;
        }
      ///
      /// \brief Get torus link information
      /// \see PAMI::Interface::Mapping::Torus::torusInformation()
      ///
      inline void torusInformation_impl(pami_coord_t &ll, pami_coord_t &ur, unsigned char info[])
      {
        memset(info, 0x00, MPI_TDIMS*sizeof(unsigned char));
      }
      inline size_t torusSize_impl (size_t i) 
      {
        PAMI_assert (i < (MPI_TDIMS+MPI_LDIMS)); /// \todo this is a mess..
        size_t sizes[2];
        sizes[0] = _nSize/_npeers;
        sizes[1] = _npeers;

        return sizes[i];
      }

      inline pami_result_t node2task_impl (Interface::Mapping::nodeaddr_t & address, size_t & task)
        {
                uint32_t x = (address.global << 16) | address.local;
                // since we expect this to be small, searching is probably the easiest way
                size_t r;
                for (r = 0; r < _size && _mapcache[r] != x; ++r);
                if (r < _size) {
                        task = r;
                        return PAMI_SUCCESS;
                }
                // never happens?
                return PAMI_ERROR;
        }
      inline pami_result_t node2peer_impl (Interface::Mapping::nodeaddr_t & address, size_t & peer)
        {
                size_t x, r = address.local;
                for (x = 0; x < _npeers && r != _peers[x]; ++x);
                if (x < _npeers) {
                        peer = x;
                        return PAMI_SUCCESS;
                }
                return PAMI_ERROR;
        }
      inline void torusAddr_impl (size_t (&addr)[MPI_TDIMS])
        {
          task2torus_impl(_task, addr);
        }
      inline pami_result_t task2torus_impl (size_t task, size_t (&addr)[MPI_TDIMS])
        {
                if (task >= _size) {
                        return PAMI_ERROR;
                }
                uint32_t x = _mapcache[task];
                addr[0] = x >> 16;
                return PAMI_SUCCESS;
        }
      /** \todo These need to be added to a public interface */
      inline pami_result_t task2global (size_t task, size_t (&addr)[MPI_TDIMS + MPI_LDIMS])
        {
                if (task >= _size) {
                        return PAMI_ERROR;
                }
                uint32_t x = _mapcache[task];
                addr[0] = x >> 16;
                addr[1] = x & 0x0000ffff;
                return PAMI_SUCCESS;
        }

      inline pami_result_t torus2task_impl (size_t (&addr)[MPI_TDIMS], size_t & task)
        {
                PAMI_abort();
        return PAMI_ERROR;
        }
      inline pami_result_t global2task(size_t (&addr)[MPI_TDIMS + MPI_LDIMS], size_t & task)
        {
                uint32_t x = (addr[0] << 16) | addr[1];
                // since we expect this to be small, searching is probably the easiest way
                size_t r;
                for (r = 0; r < _size && _mapcache[r] != x; ++r);
                if (r < _size) {
                        task = r;
                        return PAMI_SUCCESS;
                }
                // never happens?
                return PAMI_ERROR;
        }
      inline size_t       torusgetcoord_impl (size_t dimension)
        {
          if(dimension >= MPI_TDIMS + MPI_LDIMS)
            abort();
          return _task;
        }
      inline size_t globalDims()
        {
          return MPI_TDIMS + MPI_LDIMS;
        }
      inline pami_result_t task2network (pami_task_t task, pami_coord_t *addr, pami_network type)
        {
                if (task >= _size ||
                        (type != PAMI_N_TORUS_NETWORK && type != PAMI_DEFAULT_NETWORK)) {
			memset(addr, 0, sizeof(*addr));
                        return PAMI_ERROR;
                }
                uint32_t x = _mapcache[task];
                addr->u.n_torus.coords[0] = x >> 16;
                addr->u.n_torus.coords[1] = x & 0x0000ffff;
                addr->network = PAMI_N_TORUS_NETWORK;
                return PAMI_SUCCESS;
        }
      inline pami_result_t network2task_impl(const pami_coord_t *addr,
                                                pami_task_t *task,
                                                pami_network *type)
        {
                if (addr->network != PAMI_N_TORUS_NETWORK) {
                        return PAMI_ERROR;
                }

                uint32_t x = (addr->u.n_torus.coords[0] << 16) | addr->u.n_torus.coords[1];
                // since we expect this to be small, searching is probably the easiest way
                size_t r;
                for (r = 0; r < _size && _mapcache[r] != x; ++r);
                if (r < _size) {
                        *type = PAMI_N_TORUS_NETWORK;
                        *task = r;
                        return PAMI_SUCCESS;
                }
                // never happens?
                return PAMI_ERROR;
        }
    }; // class Mapping
};  // namespace PAMI
#endif // __components_mapping_mpi_mpimapping_h__
