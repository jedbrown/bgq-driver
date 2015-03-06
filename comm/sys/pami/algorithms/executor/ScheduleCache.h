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
/*
 * \file algorithms/executor/ScheduleCache.h
 * \brief ???
 */

#ifndef __algorithms_executor_ScheduleCache_h__
#define __algorithms_executor_ScheduleCache_h__

#include "algorithms/interfaces/Schedule.h"
#include "Topology.h"

namespace CCMI
{
  namespace Executor
  {
    ///\brief The ScheduleCache caches the schedule so that it does
    ///not have to be read in every call to the collective.
    class ScheduleCache
    {
      protected:
        unsigned          _start;      //Start phase
        unsigned          _nphases;    //Number of phases

        unsigned          _ntotalsrcranks;   //total number of source ranks across all phases
        unsigned          _ntotaldstranks;  //total number of destintation ranks across all phases

	unsigned          _lastCombinePhase; //The last phase that
					     //receives data before
					     //the lastReducePhase

	unsigned         *_nextActivePhaseVec; //The next phase that has
	                                       //either a send or a
	                                       //receive

	unsigned         *_activeSendPhaseVec; //Active send phases
	unsigned          _nActiveSendPhases;  //Num active send phases
	unsigned         *_activeRecvPhaseVec; //Active rescv phases
	unsigned          _nActiveRecvPhases;
	unsigned          _nActiveCombinePhases; //Num recv phases
						   //at and before
						   //last combine
						   //phase

        ///A vector of source topologies with one for each phase
        PAMI::Topology          ** _srctopologies;

        ///A vector of destination topologies with one of each phase
        PAMI::Topology          ** _dsttopologies;

        PAMI::Topology             _srcuniontopology; //The uninon of all sources
        PAMI::Topology             _dstuniontopology; //The union of all destinations

        pami_task_t             * _srcranks;  //Cache buffer of source ranks
        pami_task_t             * _dstranks;  //Cache buffer of destination ranks

        ///Buffer to cache the schedule
        char                   * _cachebuf;
        ///Size of the cache buffer
        unsigned                 _cachesize;

        //The root of broadcast and reduce collectives
        int                      _oldroot;
        int                      _root;
        //The collective operation (barrier, broadcast, allreduce, reduce)
        unsigned             _operation;
        Interfaces::Schedule * _schedule;

        static const size_t SC_MAXRANKS  = 128;

        ///Allocate routing that allocates memory for src phases and dst
        ///phases
        void          allocate (unsigned      nphases, 
				unsigned      nsrc, 
				unsigned      ndst,
				unsigned             nactive_sendp,
				unsigned             nactive_recvp);

      public:
        ScheduleCache () :
            _cachebuf(NULL),
            _cachesize(0),
            _oldroot(-2),
            _root(-1),
            _operation((unsigned) - 1),
            _schedule(NULL)
        {
          TRACE_SCHEDULE((stderr, "<%p>Executor::Barrier::ScheduleCache()\n",this));
        }

        virtual ~ScheduleCache() { if (_cachebuf) CCMI_Free (_cachebuf); }

        void operator delete(void * p)
        {
          (void)p;
          CCMI_abort();
        }

        bool init(int op)
        {
          TRACE_SCHEDULE((stderr, "<%p>Executor::Barrier::ScheduleCache::init\n",
                        this));
          if (_oldroot == _root)
            return false;

          _operation = op;
          _oldroot      = _root;

          int start, nph;
          _nextActivePhaseVec = NULL;
          _schedule->init (_root, _operation, start, nph);
          _start = start;
          _nphases = nph;
          _lastCombinePhase = (unsigned)-1;

	  _activeSendPhaseVec = NULL;
	  _activeRecvPhaseVec = NULL;
	  _nActiveSendPhases = 0;
	  _nActiveRecvPhases = 0;
	  _nActiveCombinePhases = 0;

          TRACE_SCHEDULE((stderr, "<%p>Executor::Barrier::ScheduleCache::init _start %d, nph %d\n",
                        this, _start, _nphases));

          unsigned ntotal_src = 0, ntotal_dst = 0, count = 0;
	  unsigned nactive_sendp = 0, nactive_recvp = 0;
          unsigned lastReducePhase = getLastReducePhase();

          for (count = _start; count < (_start + _nphases); count ++)
            {
              pami_task_t srcranks[SC_MAXRANKS], dstranks[SC_MAXRANKS];
              PAMI::Topology src_topology;
              PAMI::Topology dst_topology;

              _schedule->getSrcTopology(count, &src_topology,&srcranks[0]);
              ntotal_src += src_topology.size();

              //Find the last phase that receives data before the
              //reductions are over
              if (src_topology.size() > 0) {
		if (count <= lastReducePhase) {
		  _lastCombinePhase = count;
		  _nActiveCombinePhases ++;
		}
		nactive_recvp ++;
	      }

              _schedule->getDstTopology(count, &dst_topology,&dstranks[0]);
              ntotal_dst += dst_topology.size();

	      if(dst_topology.size() > 0)
		nactive_sendp ++;

	      TRACE_SCHEDULE((stderr, "Schedule Cache take_1 phase %d ndst %zu dst %u nsrc %zu src %u\n", count, dst_topology.size(), dst_topology.index2Rank(0), src_topology.size(), src_topology.index2Rank(0)));
            }

          _ntotalsrcranks = ntotal_src;
          _ntotaldstranks = ntotal_dst;
	  _nActiveSendPhases = nactive_sendp;
	  _nActiveRecvPhases = nactive_recvp;

          allocate (_start + _nphases, ntotal_src, ntotal_dst, 
		    nactive_sendp, nactive_recvp);

          unsigned srcindex = 0, dstindex = 0, asindex =0, arindex=0;

          for (count = _start; count < (_start + _nphases); count ++)
            {
              TRACE_SCHEDULE((stderr, "Schedule Cache : construct topology of size src %d dst %d\n", ntotal_src - srcindex, ntotal_dst - dstindex));

              new (_srctopologies[count]) PAMI::Topology (_srcranks + srcindex, ntotal_src - srcindex, PAMI::tag_eplist());
              new (_dsttopologies[count]) PAMI::Topology (_dstranks + dstindex, ntotal_dst - dstindex, PAMI::tag_eplist());

              _schedule->getSrcTopology(count, _srctopologies[count],_srcranks+srcindex);
              _schedule->getDstTopology(count, _dsttopologies[count],_dstranks+dstindex);
              srcindex += _srctopologies[count]->size();
              dstindex += _dsttopologies[count]->size();

	      if (_srctopologies[count]->size() > 0) {
		CCMI_assert(_activeRecvPhaseVec != NULL);
		_activeRecvPhaseVec[arindex++] = count;
	      }
	      
	      if (_dsttopologies[count]->size() > 0) {
		CCMI_assert(_activeSendPhaseVec != NULL);
		_activeSendPhaseVec[asindex++] = count;
	      }

	      TRACE_SCHEDULE((stderr, "Schedule Cache take_2 phase %d ndst %zu dst %u nsrc %zu src %u\n", count, _dsttopologies[count]->size(), _dsttopologies[count]->index2Rank(0), _srctopologies[count]->size(), _srctopologies[count]->index2Rank(0)));
            }

	  CCMI_assert (arindex == _nActiveRecvPhases);
	  CCMI_assert (asindex == _nActiveSendPhases);

          //Build the next active phase list
          unsigned endphase = _start + _nphases - 1;
          unsigned next_active = endphase;
          int p = 0;
          for (p = (int)endphase; p >= (int)_start; p--) {
            _nextActivePhaseVec[p] = next_active;
            if (getNumSrcRanks(p) > 0 || getNumDstRanks(p) > 0)
              next_active = p; //Use current phase as active phase
            //for previous phases
            }

          //_schedule->getSrcUnionTopology(&_srcuniontopology);
          //_schedule->getDstUnionTopology(&_dstuniontopology);

#if 0 //def CCMI_DEBUG_SCHEDULE

          TRACE_SCHEDULE((stderr, "Schedule Cache debug schedule\n"));
//#warning CCMI DEBUG SCHEDULE
          for ( count = _start; count < (_start + _nphases); count ++)
            if (getSrcTopology(count)->size() > 0)
              {
                pami_task_t *srcranks = 0;
                getSrcTopology(count)->rankList(&srcranks);
                TRACE_SCHEDULE((stderr, "Schedule Cache take_2 phase %d nsrc %zu srcrank %d\n", count, _srctopologies[count]->size(), srcranks[0]));
              }

          for ( count = _start; count < (_start + _nphases); count ++)
            if (getDstTopology(count)->size() > 0)
              {
                pami_task_t *dstranks = 0;
                getDstTopology(count)->rankList(&dstranks);
                TRACE_SCHEDULE ((stderr, "Schedule Cache take_2 phase %d ndst %zu dstrank %d\n", count, _dsttopologies[count]->size(), dstranks[0]));
              }

#endif
          return true;
        }

        unsigned getNumTotalSrcRanks() { return _ntotalsrcranks; }
        unsigned getNumTotalDstRanks() { return _ntotaldstranks; }

        unsigned getNumSrcRanks(unsigned p) { return getSrcTopology(p)->size(); }
        unsigned getNumDstRanks(unsigned p) { return getDstTopology(p)->size(); }

        PAMI::Topology  *getSrcTopology (unsigned phase)
        {
          //if ((phase < _start) || (phase >= _start + _nphases))
          TRACE_SCHEDULE ((stderr, "<%p>phase %u, range %u, %u\n", this, phase, _start, _start + _nphases));
          //printf("phase %u start %u nphases %u end %u\n",
	  //     phase, _start, _nphases, _start+_nphases-1);
          CCMI_assert ((phase >= _start) && (phase < _start + _nphases));
          return _srctopologies[phase];
        }

        PAMI::Topology  *getDstTopology (unsigned phase)
        {
          CCMI_assert ((phase >= _start) && (phase < _start + _nphases));
          return _dsttopologies[phase];
        }

        unsigned  getStartPhase()
        {
          return _start;
        }

        unsigned  getNumPhases()
        {
          return _nphases;
        }

        unsigned  getEndPhase ()
        {
          return _start + _nphases - 1;
        }

	unsigned getLastCombinePhase() {
	  return _lastCombinePhase;  //-1U if no data is combined on this rank
	}

        unsigned  getLastReducePhase()
        {
          unsigned p = _schedule->getLastReducePhase();
          // If the schedule doesn't support it (-1), hardcode to end phase
          return p==-1U?_start + _nphases - 1:p;
        }

	unsigned getNextActivePhase(unsigned p) {
	  CCMI_assert ((p >= _start) && (p < _start + _nphases));
	  return _nextActivePhaseVec[p];
	}

	unsigned getNumActiveRecvPhases() { 
	  return _nActiveRecvPhases;
	}

	unsigned getNumActiveSendPhases() { 
	  return _nActiveSendPhases;
	}

	unsigned getNumActiveCombinePhases() { 
	  return _nActiveCombinePhases;
	}

	unsigned* getActiveRecvPhaseVec() { 
	  return _activeRecvPhaseVec;
	}

	unsigned* getActiveSendPhaseVec() { 
	  return _activeSendPhaseVec;
	}

        void setRoot(int root)
        {
          _root = root;
        }

        int  getRoot()
        {
          return _root;
        }

        void setSchedule(Interfaces::Schedule *schedule)
        {
          _schedule = schedule;
        }

        Interfaces::Schedule   *getSchedule() { return _schedule; }
    };
  };
};

///
/// \brief Allocate buffer to cache the schedule
///
inline void CCMI::Executor::ScheduleCache::allocate
(unsigned             nphases,
 unsigned             nsrc,
 unsigned             ndst,
 unsigned             nactive_sendp,
 unsigned             nactive_recvp)
{
  //Compute space for Topology pointers and topologies, srcranks and dstranks,
  //and next active phase vector
  unsigned buf_size = 
    2 * (sizeof(PAMI::Topology *) + sizeof(pami_topology_t)) * nphases + 
    (nsrc + ndst) * sizeof(pami_task_t) + 
    (nphases + nactive_sendp + nactive_recvp) * sizeof(int);

  if (_cachesize < buf_size)
    {
      if (_cachebuf != NULL)
        CCMI_Free (_cachebuf);

      CCMI_Alloc (_cachebuf, buf_size);
      _cachesize = buf_size;

      memset (_cachebuf, 0, _cachesize);
    }

  unsigned offset = 0, count = 0;
  _srctopologies =  (PAMI::Topology **)(_cachebuf + offset);
  offset += nphases * sizeof(PAMI::Topology *);

  for (count = 0; count < nphases; count ++)
    {
      _srctopologies[count] = (PAMI::Topology *)(_cachebuf + offset);
      offset += sizeof(pami_topology_t);
    }

  _dsttopologies =  (PAMI::Topology **)(_cachebuf + offset);
  offset += nphases * sizeof(PAMI::Topology *);

  for (count = 0; count < nphases; count ++)
    {
      _dsttopologies[count] = (PAMI::Topology *)(_cachebuf + offset);
      offset += sizeof(pami_topology_t);
    }

  _srcranks   =  (pami_task_t *)(_cachebuf + offset);
  offset     += nsrc * sizeof(pami_task_t);

  _dstranks   =  (pami_task_t *)(_cachebuf + offset);
  offset     += ndst * sizeof(pami_task_t);

  _nextActivePhaseVec = (unsigned *) (_cachebuf + offset);
  offset     += nphases * sizeof(int);

  if (nactive_sendp) {
    _activeSendPhaseVec = (unsigned *) (_cachebuf + offset);
    offset     += nactive_sendp * sizeof(int);
  }

  if (nactive_recvp) {
    _activeRecvPhaseVec = (unsigned *) (_cachebuf + offset);
    offset     += nactive_recvp * sizeof(int);
  }

  CCMI_assert (offset == buf_size);
}

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
