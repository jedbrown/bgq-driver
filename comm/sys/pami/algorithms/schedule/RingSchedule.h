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
 * \file algorithms/schedule/RingSchedule.h
 * \brief ???
 */

#ifndef __algorithms_schedule_RingSchedule_h__
#define __algorithms_schedule_RingSchedule_h__

#include "algorithms/interfaces/Schedule.h"

#include "util/ccmi_debug.h"
#include "util/trace.h"
#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif


/////////////////////////////////////////////////////////////////////
///
///  In this schedule we place the nodes on a topological Ring in the
///  order in which the nodes are listed. The root node is the head
///  node and modulo root-1 is the tail. Data moves from the tail to
///  the head along the Ring. The arithmatic is loadbalanced while
///  network performance is only optimal when the nodes are on meshes
///  and tori in the correct order.
///
////////////////////////////////////////////////////////////////////


namespace CCMI
{
  namespace Schedule
  {
    class RingSchedule : public Interfaces::Schedule
    {
    public:
      /**
       * \brief Constructor
       */
      RingSchedule () : Schedule ()
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
      }

      RingSchedule (pami_endpoint_t my_ep, PAMI::Topology *topology, unsigned c = 0);

      void configure (pami_endpoint_t my_ep, unsigned nranks);

      //Ring broadcast: Send to next and recv from prev
      void getBroadcastSources (unsigned  phase, unsigned *srcpes,
                                unsigned  &nsrc)
      {
        TRACE_FN_ENTER();
        nsrc = 0;

        if (!_isHead && phase == _bcastStart)
        {
          nsrc    = 1;
          *srcpes = (!_dir) ? _prev : _next;
        }

        TRACE_FORMAT("<%p>%d", this, *srcpes);
        TRACE_FN_EXIT();
      }


      //Ring broadcast: Send to next and recv from prev
      void getBroadcastDestinations (unsigned phase, unsigned *dstpes,
                                     unsigned &ndest)
      {
        TRACE_FN_ENTER();
        unsigned sendphase = _bcastStart + ((_isHead) ? 0 : 1);

        ndest = 0;

        if (!_isTail && phase == sendphase)
        {
          ndest     = 1;
          *dstpes   = (!_dir) ? _next : _prev;

          TRACE_FORMAT("<%p>%d", this, *dstpes);
        }

        TRACE_FN_EXIT();
      }


      ///\brief Reduce methods
      ///Recv from next and send to prev
      void getReduceSources (unsigned  phase, unsigned *srcpes,
                             unsigned  &nsrc)
      {
        TRACE_FN_ENTER();
        nsrc = 0;

        if (!_isTail && phase == _startPhase)
        {
          nsrc    = 1;
          *srcpes = (!_dir) ? _next : _prev;
        }

        TRACE_FORMAT("<%p>%d", this, *srcpes);
        TRACE_FN_EXIT();
      }


      ///Recv from next and send to prev
      void getReduceDestinations (unsigned phase, unsigned *dstpes,
                                  unsigned &ndest)
      {
        TRACE_FN_ENTER();
        unsigned sendphase = _startPhase + ((_isTail) ? 0 : 1);

        ndest = 0;

        if (!_isHead && phase == sendphase)
        {
          ndest     = 1;
          *dstpes   = (!_dir) ? _prev : _next;
          TRACE_FORMAT("<%p>%d", this, *dstpes);
        }

        TRACE_FN_EXIT();
      }

      unsigned idxToRank (unsigned idx)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>index %u-> rank %u", this, idx, _topology->index2Endpoint(idx));
        TRACE_FN_EXIT();
        return _topology->index2Endpoint(idx);
      }

      unsigned myIdx ()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> rank %d, index %zu", this,_my_ep,_topology->endpoint2Index(_my_ep));
        TRACE_FN_EXIT();
        return _topology->endpoint2Index(_my_ep);
      }

      ///
      ///\brief Get the id of the root node. It should be called
      ///after _root has been set.
      ///
      unsigned headIdx ()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> rank %d, index %zu", this,_root,_topology->endpoint2Index(_my_ep));
        TRACE_FN_EXIT();
        return _topology->endpoint2Index(_root);
      }

      unsigned prevIdx ()
      {
        TRACE_FN_ENTER();
        unsigned my_idx = myIdx();
        TRACE_FORMAT("<%p> my_idx %d, prevIdx %d", this, my_idx, (my_idx > 0) ? (my_idx - 1) : (_nranks - 1));
        TRACE_FN_EXIT();
        return(my_idx > 0) ? (my_idx - 1) : (_nranks - 1);
      }

      unsigned nextIdx ()
      {
        TRACE_FN_ENTER();
        unsigned my_idx = myIdx();
        TRACE_FORMAT("<%p> my_idx %d, nextIdx %d", this,my_idx, (my_idx < (_nranks - 1)) ? (my_idx + 1) : 0);
        TRACE_FN_EXIT();
        return(my_idx < (_nranks - 1)) ? (my_idx + 1) : 0;
      }

      void local_init (int root, int op, int &startphase,
                       int &nphases)
      {
        TRACE_FN_ENTER();

        if (root >= 0)
          _root = root;
        else
          _root = idxToRank(0); //allreduce!

        unsigned head_idx = 0, tail_idx = 0, my_idx = 0;

        my_idx = myIdx ();
        head_idx = headIdx ();

        // compute tail
        if (head_idx != _nranks - 1)
        {
          tail_idx = (head_idx + _nranks - 1) % _nranks;
          _dir = 0;
        }
        else// We flip the direction of the ring, to allow
        {
          // rectangle mesh broadcasts dedicated access links
          tail_idx = 0;
          _dir = 1;
        }

        //set flags
        if (my_idx == head_idx)
          _isHead = true;

        if (my_idx == tail_idx)
          _isTail = true;

        _prev = idxToRank(prevIdx());
        _next = idxToRank(nextIdx());

        if (op == REDUCE_OP || op == ALLREDUCE_OP)
        {
          if (_isTail)
            _startPhase = 0;
          else if (!_dir)
            _startPhase = (tail_idx - my_idx + _nranks - 1) % _nranks;
          else //tail == 0
            _startPhase = my_idx - 1;

          if (op == REDUCE_OP)
          {
            // The tail node does one send and no recv. Others receive
            // on one phase and send in the next phase
            _nphases = (_isTail || _isHead) ? 1 : 2;
          }

          if (op == ALLREDUCE_OP)
          {
            if (_isHead)
            {
              _bcastStart = _startPhase + 1; //on the head start
              //phase is the reduce
              //phase
              _nphases = 2; //1 reduce phase and 1 bcast phase
            }
            else if (_isTail)
            {
	      if (_nranks > 2)
		_bcastStart = _startPhase + 2 * (_nranks - 2 - _startPhase);
	      else 
		_bcastStart = _startPhase + 1;
              _nphases = _bcastStart + 1;
            }
            else //everyone else
            {
              _bcastStart = _startPhase + 2 * (_nranks - 2 - _startPhase);
              _nphases = _bcastStart + 2 - _startPhase;
            }
          }
        }
        else if (op == BROADCAST_OP)
        {
          if (_isHead)
            _startPhase = 0;
          else if (!_dir)
            _startPhase = (my_idx - head_idx + _nranks - 1) % _nranks;
          else //tail == 0
            _startPhase = head_idx - my_idx - 1;

          _bcastStart = _startPhase;

          // The tail node does one send and no recv. Others receive
          // on one phase and send in the next phase
          _nphases = (_isTail || _isHead) ? 1 : 2;
        }
        else
          PAMI_abort();

        startphase = _startPhase;
        nphases    = _nphases;

        TRACE_FORMAT("<%p>schedule _prev %d, _next %d, _startphase %d, "
                     "idxes = (my %d, head %d, tail %d) ", this,
                     _prev, _next, _startPhase,
                     my_idx, head_idx, tail_idx);
        TRACE_FN_EXIT();
      }

      virtual unsigned getLastReducePhase () {
	if (_op == ALLREDUCE_OP)
	  return _bcastStart - 1;
	return -1;
      }

      /**
       * \brief Get the upstream processors. Source processors
       * that send messages to me in this collective operation
       * \param [in] phase  : phase of the collective
       */

      virtual void getSrcTopology (unsigned phase, PAMI::Topology *topology,pami_endpoint_t *src_eps)
      {
        TRACE_FN_ENTER();
        unsigned nranks = 0;

        switch (_op)
        {
        case REDUCE_OP:
          getReduceSources (phase, src_eps, nranks);
          break;
        case BROADCAST_OP:
          getBroadcastSources (phase, src_eps, nranks);
          break;
        case ALLREDUCE_OP:

          if (phase < _bcastStart)
            getReduceSources (phase, src_eps, nranks);
          else
            getBroadcastSources (phase, src_eps, nranks);

          break;

        case BARRIER_OP:
        default:
          CCMI_abort();
        }

        //Convert to a list topology
        new (topology) PAMI::Topology (src_eps, nranks, PAMI::tag_eplist());
#if DO_DEBUG(1)+0
        TRACE_FORMAT("<%p>topology size %zu",this, topology->size());
        for (unsigned j = 0;  j < topology->size(); ++j)
        {
          TRACE_FORMAT("<%p>topology[%d]=%u",this, j, topology->index2Endpoint(j));
        }
#endif
        TRACE_FN_EXIT();
      }

      /**
       * \brief Get the downstream processors to send data to.
       * \param phase : phase of the collective
       */
      virtual void getDstTopology(unsigned phase, PAMI::Topology *topology, pami_endpoint_t *dst_ep)
      {
        TRACE_FN_ENTER();
        unsigned ndst = 0;

        switch (_op)
        {
        case REDUCE_OP:
          getReduceDestinations (phase, dst_ep, ndst);
          break;
        case BROADCAST_OP:
          getBroadcastDestinations (phase, dst_ep, ndst);
          break;
        case ALLREDUCE_OP:

          if (phase < _bcastStart)
            getReduceDestinations (phase, dst_ep, ndst);
          else
            getBroadcastDestinations (phase, dst_ep, ndst);

          break;

        case BARRIER_OP:
        default:
          CCMI_abort();
        }

        //Convert to a list topology
        new (topology) PAMI::Topology (dst_ep, ndst,PAMI::tag_eplist());
#if DO_DEBUG(1)+0
        TRACE_FORMAT("<%p>topology size %zu",this, topology->size());
        for (unsigned j = 0;  j < topology->size(); ++j)
        {
          TRACE_FORMAT("<%p>topology[%d]=%u",this, j, topology->index2Endpoint(j));
        }
#endif
        TRACE_FN_EXIT();
        return;
      }

      /**
       * \brief Get the union of all sources across all phases
       * \param[INOUT] topology : the union of all sources
       */
      virtual pami_result_t getSrcUnionTopology (PAMI::Topology *topology,
                                                 pami_endpoint_t *src_ep)
      {
        TRACE_FN_ENTER();
        CCMI_assert(src_ep != NULL);

        unsigned nranks = 0, ntotal_ranks = 0;

        for (unsigned p = _startPhase; p < _startPhase + _nphases; p++)
        {
          switch (_op)
          {
          case REDUCE_OP:
            getReduceSources (p, src_ep + ntotal_ranks, nranks);
            break;
          case BROADCAST_OP:
            getBroadcastSources (p, src_ep + ntotal_ranks, nranks);
            break;
          case ALLREDUCE_OP:

            if (p < _bcastStart)
              getReduceSources (p, src_ep + ntotal_ranks, nranks);
            else
              getBroadcastSources (p, src_ep + ntotal_ranks, nranks);

            break;

          case BARRIER_OP:
          default:
            CCMI_abort();
          }

          ntotal_ranks += nranks;
          nranks = 0;
          //CCMI_assert (ntotal_ranks <= topology->size());
        }

        //Convert to a list topology
        new (topology) PAMI::Topology (src_ep, ntotal_ranks);
#if DO_DEBUG(1)+0
        TRACE_FORMAT("<%p>topology size %zu",this, topology->size());
        for (unsigned j = 0;  j < topology->size(); ++j)
        {
          TRACE_FORMAT("<%p>topology[%d]=%u",this, j, topology->index2Endpoint(j));
        }
#endif
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }


      /**
       * \brief Get the union of all sources across all phases
       * \param[INOUT] topology : the union of all sources
       */
      virtual pami_result_t getDstUnionTopology (PAMI::Topology  *topology,
                                                 pami_endpoint_t *dst_eps,
                                                 unsigned num_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(dst_eps != NULL);

        unsigned nranks = 0, ntotal_ranks = 0;

        // One destination per phase
        if(_nphases>num_eps)
          return PAMI_ENOMEM;

        for (unsigned p = _startPhase; p < _startPhase + _nphases; p++)
        {
          switch (_op)
          {
          case REDUCE_OP:
            getReduceDestinations (p, dst_eps + ntotal_ranks, nranks);
            break;
          case BROADCAST_OP:
            getBroadcastDestinations (p, dst_eps + ntotal_ranks, nranks);
            break;
          case ALLREDUCE_OP:

            if (p < _bcastStart)
              getReduceDestinations (p, dst_eps + ntotal_ranks, nranks);
            else
              getBroadcastDestinations (p, dst_eps + ntotal_ranks, nranks);

            break;

          case BARRIER_OP:
          default:
            CCMI_abort();
          }
          ntotal_ranks += nranks;
          nranks = 0;
        }

        //Convert to a list topology
        new (topology) PAMI::Topology (dst_eps, ntotal_ranks,PAMI::tag_eplist());
#if DO_DEBUG(1)+0
        TRACE_FORMAT("<%p>topology size %zu",this, topology->size());
        for (unsigned j = 0;  j < topology->size(); ++j)
        {
          TRACE_FORMAT("<%p>topology[%d]=%u",this, j, topology->index2Endpoint(j));
        }
#endif
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }


      /**
       * \brief Initialize the schedule for collective operation
       * \param root : the root of the collective
       * \param startphase : The phase where I become active
       * \param nphases : number of phases
       */

      virtual void init (int root, int op, int &startphase, int &nphases)
      {
        TRACE_FN_ENTER();
        startphase = 0;
        nphases = 0;

        CCMI_assert (op != BARRIER_OP);

        _op  =  op;
        _startPhase = ((unsigned) - 1);
        _bcastStart = ((unsigned) - 1);
        _root       = ((unsigned) - 1);
        _isHead     = false;
        _isTail     = false;
        _prev       = ((unsigned) - 1);
        _next       = ((unsigned) - 1);
        _dir        = 0;

        local_init (root, op, startphase, nphases);
        TRACE_FORMAT("<%p>_prev = %d, _next = %d", this, _prev, _next);
        TRACE_FN_EXIT();
      }

      static unsigned getMaxPhases (unsigned nranks)
      {
        return nranks - 1;
      }


    protected:
      unsigned   short       _op;
      unsigned               _root;
      unsigned               _startPhase;
      unsigned               _nphases;
      unsigned               _bcastStart;
      bool                   _isHead, _isTail;

      //for the ring reduce
      unsigned           _next;
      unsigned           _prev;
      PAMI::Topology    *_topology;
      unsigned           _nranks;
      pami_endpoint_t    _my_ep;
      unsigned           _dir;
    };
  };
};

inline CCMI::Schedule::RingSchedule::RingSchedule
(pami_endpoint_t my_ep, PAMI::Topology *topology, unsigned c)
{
  (void)c;
  TRACE_FN_ENTER();
  _topology = topology;
  configure (my_ep, topology->size());
  TRACE_FN_EXIT();
}


inline void CCMI::Schedule::RingSchedule::configure
(pami_endpoint_t my_ep,
 unsigned        nranks)
{
  TRACE_FN_ENTER();
  TRACE_FORMAT("my_ep %d, nranks %d", my_ep,  nranks);
  _my_ep  = my_ep;
  _isHead  = false;
  _isTail  = false;
  _nranks  = nranks;

  _startPhase = ((unsigned) - 1);
  _root = ((unsigned) - 1);
  _dir = 0;
  TRACE_FN_EXIT();
}


#endif
