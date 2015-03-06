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
 * \file algorithms/schedule/GenericTreeT.h
 * \brief ???
 */

#ifndef __algorithms_schedule_GenericTreeT_h__
#define __algorithms_schedule_GenericTreeT_h__

#include "algorithms/interfaces/Schedule.h"

#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <numeric>

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

#include "util/trace.h"

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG
#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif

//#undef DO_DEBUG
//#define DO_DEBUG(x) x

using namespace std;
// #define DEBUG(x) fprintf x

namespace CCMI
{

  namespace Schedule
  {

    ///
    /// \brief The GenericTreeSchedule is designed to provide schedules on fully connected network.
    ///        One goal is a single schedule that can be used in all the common algorithms,
    ///        including multi-ary and multi-nomial, for rooted collective operations.
    ///        There are either one source and multiple sinks or the other way around, one sink
    ///        multiple sources, depending on the root being the source or sink of the data.
    ///        Shcedules for personalized or non-rooted operation can be built on top of the
    ///        GenericTreeSchedule, and may need to also provide per step data
    ///        offset and size, both in terms of the data chunk contributed by a participant.
    ///        Note that different ranks may have different number of phase in the schedule
    ///        Special cases:
    ///        When _nu = 1 and _de = _nports+1, the schedule is a k-nomial tree where k =_nports+1
    ///        When _nu = 0 and any non-zero _de, the schedule is k-ary tree where k = _nports
    ///        When _nu = _de, the schedule is a flat tree

    // operations that process personalized messages are assigned odd numbers in the enumeration
    enum operations_t
    {
      BROADCAST = 0,
      SCATTER,
      REDUCE,
      GATHER,
      ALLGATHER,
      ALLTOALL,
      ALLREDUCE
    };

    ///
    /// \brief GenericTreeSchedule. Template parameter P is the number of ports
    ///        in the multi-port model, NU and DE are numerator and denominator
    ///        for alpha calculation.
    template < unsigned P = 1, unsigned NU = 1, unsigned DE = 2 >
    class GenericTreeSchedule : public Interfaces::Schedule
    {
    public:
      static const unsigned _nports = P;
      static const unsigned _nu     = NU;
      static const unsigned _de     = DE;

      GenericTreeSchedule () :
      _lstartph(-1),
      _rstartph(-1),
      _nphs(0),
      _mynphs(0),
      _topo(NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p>GenericTreeSchedule ()", this);
        TRACE_FN_EXIT();
      }

      GenericTreeSchedule(int myrank, int nranks) :
      _myrank(myrank),
      _nranks(nranks),
      _lstartph(-1),
      _rstartph(-1),
      _nphs(0),
      _mynphs(0),
      _root(0),
      _op(0),
      _personalized(false),
      _src(0),
      _topo(NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
      }

      /**
       * \brief Constructor for topology
       *
       * \param[in] topo    Topology pointer
       * \param[in] myrank  My rank in topology
       */
      GenericTreeSchedule (unsigned myrank, PAMI::Topology *topo, unsigned c = 0);

      /**
       * \brief Constructor for list of ranks
       *
       * \param[in] ranks     Ranks list
       * \param[in] nranks    Number of ranks in list
       */
      GenericTreeSchedule (unsigned myrank, size_t *ranks, unsigned nranks);

      // provide alpha value
      double alpha(int nranks)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%f",((double(nranks)) * _nu / _de));
        TRACE_FN_EXIT();
        return((double(nranks)) * _nu / _de);
      }
      // calculate relative rank with respect to root
      int    rrank(int rank)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%u",(((rank) + _nranks - _root) % _nranks));
        TRACE_FN_EXIT();
        return(((rank) + _nranks - _root) % _nranks);
      }
      // calculate absolute rank
      int    arank(int rank)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%u",(((rank) + _root) % _nranks));
        TRACE_FN_EXIT();
        return(((rank) + _root) % _nranks);
      }
      // calculate rank in topology
      int    toporank(int index)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%u",(_topo == NULL) ? index : _topo->index2Endpoint(index));
        TRACE_FN_EXIT();
        return(_topo == NULL) ? index : _topo->index2Endpoint(index);
      }
      // calculate index in topology
      int    topoindex(int rank)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%zu",(_topo == NULL) ? rank : _topo->endpoint2Index(rank));
        TRACE_FN_EXIT();
        return(_topo == NULL) ? rank : _topo->endpoint2Index(rank);
      }

      void init(int root, int op, int &lstartphase, int &rstartphase, int &nphases, int &maxranks)
      {
        unsigned nodes_left, cur_pos, phase;
        unsigned pbig, psmall;
        TRACE_FN_ENTER();
        TRACE_FORMAT("int root %d, int op %d, int &lstartphase %d, int &nphases %d",root,op,lstartphase,nphases);
        _root = topoindex(root);
        CCMI_assert(_root >= 0);
        _op           = op;
        _personalized = (op % 2);

        cur_pos    = rrank(_myrank);    // position in the current subtree
        nodes_left = _nranks;               // ranks that are not covered yet

        for (phase = 0; nodes_left > 1; ++ phase)
        {
          // pbig is the size of the next big subtree
          pbig = max(1, min((int)CEILING(alpha(nodes_left)), (int)(nodes_left - _nports)));
          // psmall is the number in each small subtree
          psmall = (int) CEILING((double)(nodes_left - pbig) / _nports);

          if (cur_pos == 0)     // if I am root of the current subtree
          {
            if (_rstartph == -1) _rstartph = phase;

            for (unsigned i = 0; i < _nports; ++i )
            {
              if (pbig + psmall*i < nodes_left)
              {
                _partners.push_back((_myrank + pbig + psmall*i) % _nranks);
                _subsizes.push_back((pbig + psmall*i > nodes_left) ? psmall : nodes_left - pbig - psmall*i);
              }
            }
          }

          if (cur_pos >= pbig)  // if should get data from left neighbors in the current phase
          {
            for (unsigned i = 0; i < _nports; ++i)
            {
              if (cur_pos == pbig + i*psmall)
              {
                _src      = (_myrank + _nranks - (pbig + i * psmall)) % _nranks;

                if ( _lstartph == -1) _lstartph = phase;
              }
            }
          }

          if (cur_pos < pbig)
          {
            nodes_left = pbig;
          }
          else
          {
            int t = (cur_pos - pbig) / psmall;

            if (pbig + (t + 1)*psmall > nodes_left)
              nodes_left -= (pbig + t * psmall);
            else
              nodes_left = psmall;

            cur_pos -= (pbig + t * psmall);
          }
        }

        if (_myrank == _root) _lstartph = 0;

        _mynphs    = phase - _lstartph;

        nodes_left = _nranks;

        for (int phase = 0; nodes_left > 1; ++ phase)
          nodes_left = max(1, min((int)CEILING(alpha(nodes_left)), (int)(nodes_left - _nports)));

        _nphs      = phase;

        if (_rstartph == -1) _rstartph = _nphs;

        lstartphase = _lstartph;
        rstartphase = _rstartph;
        nphases     = _nphs;
        maxranks    = _nports;
        TRACE_FORMAT("int root %d, int op %d, int &lstartphase %d, int &rstartphase %d, int &nphases %d, int &maxranks %d",root,op,lstartphase,rstartphase,nphases,maxranks);
        TRACE_FN_EXIT();
      }

      void init(int root, int op, int &startphase, int &nphases, int &maxranks)
      {
        int rstartphase;
        TRACE_FN_ENTER();
        init(root, op, startphase, rstartphase, nphases, maxranks);
        TRACE_FN_EXIT();
      }


      void init(int root, int op, int &startphase, int &nphases)
      {
        int maxranks, rstartphase;
        TRACE_FN_ENTER();
        init(root, op, startphase, rstartphase, nphases, maxranks);
        TRACE_FN_EXIT();
      }

      int getMyNumPhases()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%u",_mynphs);
        TRACE_FN_EXIT();
        return _mynphs;
      }


      ///
      /// \brief Get the src ranks for one phase of the communication schedule
      ///        The schedule is calculated assuming Broadcast type of flow.
      ///        So Reduce (Gather too) will need to flip the step index.
      ///        For the non-rooted ones, Allgather is simiar to gather.
      ///
      void getSrcPeList (unsigned  uphase, unsigned *srcpes,
                         unsigned  &nsrc, unsigned *subtasks = NULL)
      {
        TRACE_FN_ENTER();
        if (_op == BROADCAST || _op == SCATTER)
        {
          getLList(uphase, srcpes, nsrc);
        }
        else if (_op == REDUCE || _op == GATHER)
        {
          getRList(_lstartph + _mynphs - uphase - 1, srcpes, nsrc);
        }
        else
        {
          // DEBUG((stderr, "not implemented yet\n");)
          CCMI_assert(0);
        }
        TRACE_FN_EXIT();
      }


      void getDstPeList (unsigned  uphase, unsigned *dstpes,
                         unsigned  &ndst, unsigned *subtasks = NULL)
      {
        TRACE_FN_ENTER();
        if (_op == BROADCAST || _op == SCATTER)
        {
          getRList(uphase, dstpes, ndst);
        }
        else if (_op == REDUCE || _op == GATHER )
        {
          getLList(_lstartph + _mynphs - uphase - 1, dstpes, ndst);
        }
        else
        {
          // DEBUG((stderr,"not implemented yet\n");)
          CCMI_assert(0);
        }
        TRACE_FN_EXIT();
      }

      ///
      /// \brief get number of children, if rooted, of right neighbors list
      ///
      unsigned getRListNpes(unsigned uphase)
      {
        TRACE_FN_ENTER();
        // vector<int>::iterator iter;
        // for_each (_partners.begin(), _partners.end(), print_partners);
        int phase = (int)uphase;
        unsigned nrpes = 0;

        if (phase >= _rstartph)
        {
          nrpes = (phase < _nphs - 1) ? _nports : _partners.size() - (phase - _rstartph) * _nports;
        }
        TRACE_FORMAT("phase %u, nrpes %u, _nphs %u, _nports %u, _partners.size %zu , _rstartph %u",phase, nrpes, _nphs, _nports, _partners.size(), _rstartph);
        TRACE_FN_EXIT();
        return nrpes;
      }
      ///
      /// \brief get children, if rooted, or right neighbors list
      ///
      void getRList (unsigned uphase, unsigned *rpes, unsigned &nrpes, unsigned *rlens = NULL)
      {
        TRACE_FN_ENTER();
        // vector<int>::iterator iter;
        // for_each (_partners.begin(), _partners.end(), print_partners);
        int phase = (int)uphase;
        nrpes = 0;

        if (phase >= _rstartph)
        {
          nrpes = (phase < _nphs - 1) ? _nports : _partners.size() - (phase - _rstartph) * _nports;
          TRACE_FORMAT("phase %u, nrpes %u, _nphs %u, _nports %u, _partners.size %zu , _rstartph %u",phase, nrpes, _nphs, _nports, _partners.size(), _rstartph);

          for (unsigned i = 0; i < _nports && i < nrpes; ++i)
          {
            rpes[i] = toporank(_partners[(phase - _rstartph)*_nports + i]);
            TRACE_FORMAT("rpes[%u] %u = toporank(_partners[(phase %u - _rstartph %u)*_nports %u+ i %u] = %u);",i,rpes[i],phase,_rstartph,_nports ,i,_partners[(phase - _rstartph)*_nports + i]);

            if (rlens)
            {
              rlens[i] = _subsizes[(phase - _rstartph)*_nports + i];
              TRACE_FORMAT("rlens[%u]=%u, _subsizes[(phase %u - _rstartph %u)*_nports %u + i %u]=%u", i,rlens[i],phase,_rstartph,_nports,i,_subsizes[(phase - _rstartph)*_nports + i]);
            }
          }
        }

        TRACE_FN_EXIT();
      }

      ///
      /// \brief Get parent, if rooted, or left neighbors list
      ///        The parent rank is saved _src. The left neighbor list
      ///        however is the same _partners list.
      ///
      void getLList (unsigned uphase, unsigned *lpes, unsigned &nlpes, unsigned *llens = NULL)
      {
        int phase = (int)uphase;
        TRACE_FN_ENTER();
        nlpes = 0;

        if (_myrank != _root && (int)phase == _lstartph)
        {
          lpes[0]    = toporank(_src);
          nlpes      = 1;

          if (llens) llens[0] = accumulate(_subsizes.begin(), _subsizes.end(), 1);
        }
        TRACE_FN_EXIT();
      }

      ///
      /// \brief Get both left and right neighbors
      ///        Since the partner list are pre-calculated and cached in the
      ///        Schedule object, this may simply call getRList() and getLList()
      ///        Should not be very expensive
      ///
      void getList (unsigned uphase, unsigned *srcpes, unsigned &nsrc,
                    unsigned *dstpes, unsigned &ndst,
                    unsigned *srclens = NULL, unsigned *dstlens = NULL)
      {
        CCMI_assert(0);
      }

      void  getSrcTopology (unsigned phase, PAMI::Topology *topology, pami_endpoint_t *src_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(src_eps != NULL);

        unsigned nsrc = 0;
        getSrcPeList(phase, src_eps, nsrc);
        TRACE_FORMAT("<%p> getSrcTopology() phase %u, nsrc %u", this, phase, nsrc);

        //Convert to a list topology
        new (topology) PAMI::Topology (src_eps, nsrc, PAMI::tag_eplist());
        DO_DEBUG(unsigned i = 0;)
        DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
        TRACE_FN_EXIT();
      }

      void getDstTopology (unsigned phase, PAMI::Topology *topology, pami_endpoint_t *dst_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(dst_eps != NULL);

        unsigned ndst = 0;
        getDstPeList(phase, dst_eps, ndst);
        CCMI_assert(ndst <= topology->size());

        TRACE_FORMAT("<%p> getDstTopology() phase %u, nsrc %u", this, phase, ndst);

        //Convert to a list topology
        new (topology) PAMI::Topology (dst_eps, ndst,PAMI::tag_eplist());
        DO_DEBUG(unsigned i = 0;)
        DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
        TRACE_FN_EXIT();
      }

      pami_result_t getSrcUnionTopology (PAMI::Topology *topology, pami_endpoint_t *src_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(src_eps != NULL);

        unsigned ntotal_src = 0;

        for (int phase = _lstartph; phase < _lstartph + _mynphs; phase++)
        {
          unsigned nsrc = 0;
          getLList(phase, src_eps + ntotal_src, nsrc);
          ntotal_src += nsrc;
        }

        TRACE_FORMAT("<%p> getSrcUnionTopology() ntotal_src %u", this, ntotal_src);

        //Convert to a list topology
        new (topology) PAMI::Topology (src_eps, ntotal_src, PAMI::tag_eplist());
        DO_DEBUG(unsigned i = 0;)
        DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      pami_result_t getDstUnionTopology (PAMI::Topology *topology,
                                         pami_endpoint_t *dst_eps,
                                         unsigned num_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(dst_eps != NULL);

        unsigned ntotal_dst = 0;

        for (int phase = _lstartph; phase < _lstartph + _mynphs; phase++)
        {
          unsigned ndst = 0;
          TRACE_FORMAT("<%p>_lstartph %u, _mynphs %u, phase %u", this,_lstartph, _mynphs, phase);
          // Check how many destinations this phase and total num destinations doesnt' exceed max
          if((getRListNpes(phase)+ntotal_dst)>num_eps)
            return PAMI_ENOMEM;
          getRList(phase, dst_eps + ntotal_dst, ndst);
          ntotal_dst += ndst;
          TRACE_FORMAT("<%p> ntotal_dst %u, ndst %u", this,ntotal_dst, ndst);
        }

        TRACE_FORMAT("<%p> getDstUnionTopology() ntotal_dst %u", this, ntotal_dst);

        //Convert to a list topology
        new (topology) PAMI::Topology (dst_eps, ntotal_dst,PAMI::tag_eplist());
        DO_DEBUG(unsigned i = 0;)
        DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }



    protected :
      int _myrank;      // my rank (index in topology) in the group
      int _nranks;      // number of ranks in the group
      int _lstartph;      // first phase messages arrive from the left neighbors
      int _rstartph;      // first phase messages are sent to right neighbors
      int _nphs;      // number of phases, whether I am active or not
      int _mynphs;                    // number of phases taht I am active
      int _root;      // rank (index in topology) of root in rooted collective operations
      int _op;      // operation type
      bool _personalized;           // distinct message to/from a partner
      // PAMI::Topology _topology;    // place for topology if initlized by a rank list
      int _src;     // rank (index in topology) of the left neighbor or parent
      PAMI::Topology  *_topo;   // pointer to topology
      vector<int> _partners;          // rank (index in topology) of partners in this phase
      vector<int> _subsizes;          // number of ranks each partner covers
    };

    /**
     * \brief Constructor for list of ranks
     *
     * \param[in] myrank    My rank in COMM_WORLD
     * \param[in] topology  topology across which the tree is constructed
     */
    template <unsigned P, unsigned NU, unsigned DE>
    inline CCMI::Schedule::GenericTreeSchedule<P, NU, DE>::
    GenericTreeSchedule(unsigned myrank, PAMI::Topology *topology, unsigned c):
    _myrank(topology->endpoint2Index(myrank)),
    _nranks(topology->size()),
    _lstartph(-1),
    _rstartph(-1),
    _nphs(0),
    _mynphs(0),
    _root(0),
    _op(0),
    _personalized(false),
    _src(0),
    _topo(topology)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT( "<%p>GenericTreeSchedule(unsigned myrank, PAMI::Topology *topology): myrank %u, nranks %zu", this, myrank, topology->size());

      DO_DEBUG(unsigned i = 0;)
      DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
      TRACE_FN_EXIT();
    }


    /**
     * \brief Constructor for list of ranks
     *
     * \param[in] myrank    My rank in COMM_WORLD
     * \param[in] nranks    Number of ranks in list
     * \param[in] ranks     Ranks list
     */
    template <unsigned P, unsigned NU, unsigned DE>
    inline CCMI::Schedule::GenericTreeSchedule<P, NU, DE>::
    GenericTreeSchedule(unsigned myrank, size_t *ranks, unsigned nranks)// : _topology(ranks, nranks), _myrank(_topology.endpoint2Index(myrank))
    {
      CCMI_assert(0);
      // _topo = &_topology;
    }


    // the denominator should never be 0
    template <unsigned P, unsigned NU>
    class GenericTreeSchedule <P, NU, 0>
    {
      // COMPILE_TIME_ERROR(0)
    };



    // special treatment is needed when
    // _nu == _de, a very big partners array
    // may be needed to store all
    // partners. In this case, the partner
    // list is calculated on the fly
    template <unsigned P, unsigned NU>
    class GenericTreeSchedule <P, NU, NU> : public Interfaces::Schedule
    {
    public:
      static const unsigned _nports = P;
      static const unsigned _nu     = NU;
      static const unsigned _de     = NU;

      GenericTreeSchedule () :
      _lstartph(0),
      _rstartph(0),
      _nphs(0),
      _mynphs(0),
      _topo(NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p>GenericTreeSchedule ()", this);
        TRACE_FN_EXIT();
      }

      GenericTreeSchedule(int myrank, int nranks) :
      _myrank(myrank),
      _nranks(nranks),
      _lstartph(0),
      _rstartph(0),
      _nphs(0),
      _mynphs(0),
      _root(0),
      _op(0),
      _personalized(false),
      _topo(NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
        cout << "from the flat tree specialization - ctor" << endl;
      }

      /**
       * \brief Constructor for topology
       *
       * \param[in] topo    Topology pointer
       * \param[in] myrank  My rank in topology
       */
      GenericTreeSchedule (unsigned myrank, PAMI::Topology *topo);


      /**
       * \brief Constructor for list of ranks
       *
       * \param[in] ranks     Ranks list
       * \param[in] nranks    Number of ranks in list
       */
      GenericTreeSchedule (unsigned myrank, size_t *ranks, unsigned nranks);

      // calculate rank in topology
      int    toporank(int index)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%u",(_topo == NULL) ? index : _topo->index2Endpoint(index));
        TRACE_FN_EXIT();
        return(_topo == NULL) ? index : _topo->index2Endpoint(index);
      }
      // calculate index in topology
      int    topoindex(int rank)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%zu",(_topo == NULL) ? rank : _topo->endpoint2Index(rank));
        TRACE_FN_EXIT();
        return(_topo == NULL) ? rank : _topo->endpoint2Index(rank);
      }

      void init(int root, int op, int &lstartphase, int &rstartphase, int &nphases, int &maxranks)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("int root %d, int op %d, int &lstartphase %d, int &nphases %d",root,op,lstartphase,nphases);
        _root         = topoindex(root);
        CCMI_assert(_root >= 0);
        _op           = op;
        _personalized = (op % 2);

        _nphs   =  _mynphs  = (_nranks - 1 + _nports - 1) / _nports;

        if (_myrank != _root)
        {
          _rstartph     = _nphs;
          _lstartph     = (((_myrank + _nranks - _root) % _nranks) + _nports - 1) / _nports - 1;
          _mynphs       = 1;
        }

        lstartphase = _lstartph;
        rstartphase = _rstartph;
        nphases     = _nphs;
        maxranks    = _nports;
        TRACE_FORMAT("int root %d, int op %d, int &lstartphase %d, int &rstartphase %d, int &nphases %d, int &maxranks %d",root,op,lstartphase,rstartphase,nphases,maxranks);
        TRACE_FN_EXIT();
      }

      void init(int root, int op, int &startphase, int &nphases, int &maxranks)
      {
        int rstartphase;
        TRACE_FN_ENTER();
        init(root, op, startphase, rstartphase, nphases, maxranks);
        TRACE_FN_EXIT();
      }

      void init(int root, int op, int &startphase, int &nphases)
      {
        int maxranks, rstartphase;
        TRACE_FN_ENTER();
        init(root, op, startphase, rstartphase, nphases, maxranks);
        TRACE_FN_EXIT();
      }

      int getMyNumPhases()
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("%u",_mynphs);
        TRACE_FN_EXIT();
        return _mynphs;
      }

      ///
      /// \brief Get the src ranks for one step of the communication schedule
      ///        The schedule is calculated assuming Broadcast type of flow.
      ///        So Reduce (Gather too) will need to flip the step index.
      ///        For the non-rooted ones, Allgather is simiar to gather.
      ///
      void getSrcPeList (unsigned  uphase, unsigned *srcpes,
                         unsigned  &nsrc, unsigned *subtasks = NULL)
      {
        TRACE_FN_ENTER();
        if (_op == BROADCAST || _op == SCATTER)
        {
          getLList(uphase, srcpes, nsrc);
        }
        else if (_op == REDUCE || _op == GATHER)
        {
          getRList(_nphs - uphase - 1, srcpes, nsrc);
        }
        else
        {
          // DEBUG((stderr, "not implemented yet\n");)
          CCMI_assert(0);
        }
        TRACE_FN_EXIT();
      }


      void getDstPeList (unsigned  uphase, unsigned *dstpes,
                         unsigned  &ndst, unsigned *subtasks = NULL)
      {
        TRACE_FN_ENTER();
        if (_op == BROADCAST || _op == SCATTER)
        {
          getRList(uphase, dstpes, ndst);
        }
        else if (_op == REDUCE || _op == GATHER )
        {
          getLList(_nphs - uphase - 1, dstpes, ndst);
        }
        else
        {
          // DEBUG((stderr, "not implemented yet\n");)
          CCMI_assert(0);
        }
        TRACE_FN_EXIT();
      }

      ///
      /// \brief get number of children, if rooted, of right neighbors list
      ///
      unsigned getRListNpes(unsigned uphase)
      {
        int phase = (int)uphase;
        TRACE_FN_ENTER();
        unsigned nrpes = 0;

        if (_myrank == _root)
        {
          nrpes = (phase < _nphs) ? _nports : _nranks - phase * _nports;
        }
        TRACE_FORMAT("phase %u, nrpes %u, _nphs %u, _nports %u,  _rstartph %u",phase, nrpes, _nphs, _nports,  _rstartph);
        TRACE_FN_EXIT();
        return nrpes;
      }
      ///
      /// \brief get children, if rooted, or right neighbors list
      ///
      void getRList (unsigned uphase, unsigned *rpes, unsigned &nrpes,
                     unsigned *rlens = NULL, unsigned *roffs = NULL)
      {
        int phase = (int)uphase;
        TRACE_FN_ENTER();
        nrpes = 0;

        if (_myrank == _root)
        {
          nrpes = (phase < _nphs) ? _nports : _nranks - phase * _nports;

          for (int i = 0; i < (int)nrpes; ++i)
          {
            rpes[i] = toporank((phase * _nports + i + 1 + _myrank) % _nranks);

            if (rlens) rlens[i] = 1;
          }
        }
        TRACE_FN_EXIT();
      }

      ///
      /// \brief Get parent, if rooted, or left neighbors list
      ///        The parent rank is saved _src. The left neighbor list
      ///        however is the same _partners list.
      ///
      void getLList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                     unsigned *llens = NULL, unsigned *loffs = NULL)
      {
        int phase = (int)uphase;
        TRACE_FN_ENTER();
        nlpes = 0;

        if (_myrank != _root && (int)phase == _lstartph)
        {
          lpes[0]    = toporank(_root);
          nlpes      = 1;

          if (llens) llens[0] = 1;
        }
        TRACE_FN_EXIT();
      }

      void  getSrcTopology (unsigned phase, PAMI::Topology *topology, pami_endpoint_t *src_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(src_eps != NULL);

        unsigned nsrc = 0;
        getSrcPeList(phase, src_eps, nsrc);
        CCMI_assert(nsrc <= topology->size());

        TRACE_FORMAT("<%p> getSrcTopology() phase %u, nsrc %u", this, phase, nsrc);

        //Convert to a list topology
        new (topology) PAMI::Topology (src_eps, nsrc,PAMI::tag_eplist());
        DO_DEBUG(unsigned i = 0;)
        DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););

        TRACE_FN_EXIT();
      }

      void getDstTopology (unsigned phase, PAMI::Topology *topology, pami_endpoint_t *dst_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(dst_eps != NULL);

        unsigned ndst = 0;
        getDstPeList(phase, dst_eps, ndst);
        TRACE_FORMAT("<%p> getDstTopology() phase %u, nsrc %u", this, phase, ndst);

        //Convert to a list topology
        new (topology) PAMI::Topology (dst_eps, ndst,PAMI::tag_eplist());
        DO_DEBUG(unsigned i = 0;)
        DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
        TRACE_FN_EXIT();
      }

      pami_result_t getSrcUnionTopology (PAMI::Topology *topology,pami_endpoint_t *src_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(src_eps != NULL);
        unsigned ntotal_src = 0;

        for (int phase = _lstartph; phase < _lstartph + _mynphs; phase++)
        {
          unsigned nsrc = 0;
          getLList(phase, src_eps + ntotal_src, nsrc);
          ntotal_src += nsrc;
        }

        TRACE_FORMAT("<%p> getSrcUnionTopology() ntotal_src %u", this, ntotal_src);

        //Convert to a list topology
        new (topology) PAMI::Topology (src_eps, ntotal_src,PAMI::tag_eplist());
        DO_DEBUG(unsigned i = 0;)
        DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }

      pami_result_t getDstUnionTopology (PAMI::Topology *topology,
                                         pami_endpoint_t *dst_eps,
                                         unsigned num_eps)
      {
        TRACE_FN_ENTER();
        CCMI_assert(dst_eps != NULL);

        unsigned ntotal_dst = 0;

        for (int phase = _lstartph; phase < _lstartph + _mynphs; phase++)
        {
          unsigned ndst = 0;
          // Check how many destinations this phase and total num destinations doesnt' exceed max
          if((getRListNpes(phase)+ntotal_dst)>num_eps)
            return PAMI_ENOMEM;
          getRList(phase, dst_eps + ntotal_dst, ndst);
          ntotal_dst += ndst;

          CCMI_assert (ntotal_dst <= topology->size());
        }

        TRACE_FORMAT("<%p> getDstUnionTopology() ntotal_dst %u", this, ntotal_dst);

        //Convert to a list topology
        new (topology) PAMI::Topology (dst_eps, ntotal_dst,PAMI::tag_eplist());
        DO_DEBUG(unsigned i = 0;)
        DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
        TRACE_FN_EXIT();
        return PAMI_SUCCESS;
      }


    protected :
      int _myrank;
      int _nranks;
      int _lstartph;
      int _rstartph;
      int _nphs;
      int _mynphs;
      int _root;
      int _op;
      bool _personalized;
      // PAMI::Topology _topology;
      PAMI::Topology *_topo;

    };

    /**
      * \brief Constructor for list of ranks
      *
      * \param[in] myrank    My rank in COMM_WORLD
      * \param[in] topology  topology across which the tree is constructed
      */
    template <unsigned P, unsigned NU>
    inline CCMI::Schedule::GenericTreeSchedule<P, NU, NU>::
    GenericTreeSchedule(unsigned myrank, PAMI::Topology *topology):
    _myrank(topology->endpoint2Index(myrank)),
    _nranks(topology->size()),
    _lstartph(0),
    _rstartph(0),
    _nphs(0),
    _mynphs(0),
    _root(0),
    _op(0),
    _personalized(false),
    _topo(topology)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p>Flat specialization: GenericTreeSchedule(unsigned myrank, PAMI::Topology *topology): myrank %u, nranks %zu", this, myrank, topology->size());

      DO_DEBUG(unsigned i = 0;)
      DO_DEBUG(for (i = 0; ((i < topology->size()) && (i < 32)); ++i) TRACE_FORMAT("<%p> topology[%u] = %u", this, i, topology->index2Endpoint(i)););
      TRACE_FN_EXIT();
    };


    /**
     * \brief Constructor for list of ranks
     *
     * \param[in] myrank    My rank in COMM_WORLD
     * \param[in] nranks    Number of ranks in list
     * \param[in] ranks     Ranks list
     */
    template <unsigned P, unsigned NU>
    inline CCMI::Schedule::GenericTreeSchedule<P, NU, NU>::
    GenericTreeSchedule(unsigned myrank, size_t *ranks, unsigned nranks)
    // : _topology(ranks, nranks), _myrank(_topology.endpoint2Index(myrank))
    {
      CCMI_assert(0);
      // _topo = &_topology;
    };



    ///
    /// \brief Simple K-nary tree Broadcast schedule
    ///
    template <unsigned P>
    class KnaryBcastSchedule : public GenericTreeSchedule < P, 0, P + 1 >
    {
    public:
      KnaryBcastSchedule () : GenericTreeSchedule < P, 0, P + 1 > ()
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
      }

      KnaryBcastSchedule (int myrank, int nranks) : GenericTreeSchedule < P, 0, P + 1 > (myrank, nranks)
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
      }

      KnaryBcastSchedule (unsigned myrank, PAMI::Topology *topo, unsigned c = 0) : GenericTreeSchedule < P, 0, P + 1 > (myrank, topo)
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
      }

      using GenericTreeSchedule < P, 0, P + 1 >::getLList;
      using GenericTreeSchedule < P, 0, P + 1 >::getRList;
      using GenericTreeSchedule < P, 0, P + 1 >::getRListNpes;

      ///
      /// \brief Get both left and right neighbors
      ///        Since the partner list are pre-calculated and cached in the
      ///        Schedule object, this simply calls getRList() and getLList()
      ///
      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        TRACE_FN_ENTER();
        //this->getLList(uphase, lpes, nlpes);
        getLList(uphase, lpes, nlpes);
        assert(nlpes == 1 || nlpes == 0);

        if (loffs && llens)
        {
          for (int i = 0; i < nlpes; ++i)
          {
            loffs[i] = 0;
            llens[i] = 1; // Single message for the entire data
          }
        }

        //this->getRList(uphase, rpes, nrpes);
        getRList(uphase, rpes, nrpes);
        assert(nrpes <= this->_nports);

        if (roffs && rlens)
        {
          for (int i = 0; i < nrpes; ++i)
          {
            roffs[i] = 0;
            rlens[i] = 1; // Single message for the entire data
          }
        }
        TRACE_FN_EXIT();
      }
    };



    ///
    /// \brief Simple K-nomial tree Broadcast schedule
    ///
    template <unsigned P>
    class KnomialBcastSchedule : public GenericTreeSchedule < P, 1, P + 1 >
    {
    public:
      KnomialBcastSchedule () : GenericTreeSchedule < P, 1, P + 1 > ()
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
      }

      KnomialBcastSchedule (int myrank, int nranks) : GenericTreeSchedule < P, 1, P + 1 > (myrank, nranks)
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
      }

      KnomialBcastSchedule (unsigned myrank, PAMI::Topology *topo) : GenericTreeSchedule < P, 1, P + 1 > (myrank, topo)
      {
        TRACE_FN_ENTER();
        TRACE_FN_EXIT();
      }

      using GenericTreeSchedule < P, 1, P + 1 >::getLList;
      using GenericTreeSchedule < P, 1, P + 1 >::getRList;
      using GenericTreeSchedule < P, 1, P + 1 >::getRListNpes;

      ///
      /// \brief Get both left and right neighbors
      ///        Since the partner list are pre-calculated and cached in the
      ///        Schedule object, this simply calls getRList() and getLList()
      ///
      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        TRACE_FN_ENTER();
        //this->getLList(uphase, lpes, nlpes);
        getLList(uphase, lpes, nlpes);
        assert(nlpes == 1 || nlpes == 0);

        if (loffs && llens)
        {
          for (int i = 0; i < nlpes; ++i)
          {
            loffs[i] = 0;
            llens[i] = 1; // Single message for the entire data
          }
        }

        //this->getRList(uphase, rpes, nrpes);
        getRList(uphase, rpes, nrpes);
        assert(nrpes <= this->_nports);

        if (roffs && rlens)
        {
          for (int i = 0; i < nrpes; ++i)
          {
            roffs[i] = 0;
            rlens[i] = 1; // Single message for the entire data
          }
        }

        TRACE_FN_EXIT();
      }
    };

    ///
    /// \brief Simple K-nomial tree Reduce schedule
    ///
    template <unsigned P>
    class KnomialReduceSchedule : public GenericTreeSchedule < P, 1, P + 1 >
    {
    public:
      KnomialReduceSchedule (int myrank, int nranks) : GenericTreeSchedule < P, 1, P + 1 > (myrank, nranks)
      {
      }

      using GenericTreeSchedule < P, 1, P + 1 >::getLList;
      using GenericTreeSchedule < P, 1, P + 1 >::getRList;
      using GenericTreeSchedule < P, 1, P + 1 >::getRListNpes;
      using GenericTreeSchedule < P, 1, P + 1 >::init;

      void init(int root, int op, int &startphase, int &nphases)
      {
        int maxranks, lstartphase;
        init(root, op, lstartphase, startphase, nphases, maxranks);
      }

      ///
      /// \brief Get both left and right neighbors
      ///        Since the partner list are pre-calculated and cached in the
      ///        Schedule object, this simply calls getRList() and getLList()
      ///
      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        //this->getRList(this->_nphs - uphase - 1, lpes, nlpes);
        getRList(this->_nphs - uphase - 1, lpes, nlpes);
        assert(nlpes <= this->_nports);

        if (loffs && llens)
        {
          for (int i = 0; i < nlpes; ++i)
          {
            loffs[i] = 0;
            llens[i] = 1; // Single message for the entire data
          }
        }

        //this->getLList(this->_nphs - uphase - 1, rpes, nrpes);
        getLList(this->_nphs - uphase - 1, rpes, nrpes);
        assert(nrpes == 1 || nrpes == 0);

        if (roffs && rlens)
        {
          for (int i = 0; i < nrpes; ++i)
          {
            roffs[i] = 0;
            rlens[i] = 1; // Single message for the entire data
          }
        }

      }
    };

    ///
    /// \attention For Scatter(v) and Gather(v), the convention is pes
    ///            contain absolute ranks, but the offsets contain relative (to sender)
    ///            ranks, since offsets calculate is easier if buffer
    ///            wrap around can be avoided. This means the actual root may need
    ///            buffer reshuffle before/after the schedule is carried out

    ///
    /// \brief Simple K-nomial tree Scatter schedule
    ///
    template <unsigned P>
    class KnomialScatterSchedule : public GenericTreeSchedule < P, 1, P + 1 >
    {
    public:
      KnomialScatterSchedule (int myrank, int nranks) : GenericTreeSchedule < P, 1, P + 1 > (myrank, nranks)
      {
      }

      using GenericTreeSchedule < P, 1, P + 1 >::getLList;
      using GenericTreeSchedule < P, 1, P + 1 >::getRList;
      using GenericTreeSchedule < P, 1, P + 1 >::getRListNpes;

      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        //this->getLList(uphase, lpes, nlpes, llens);
        getLList(uphase, lpes, nlpes, llens);
        assert(nlpes == 1 || nlpes == 0);

        if (loffs && llens)
        {
          for (int i = 0; i < nlpes; ++i)
          {
            // always receive into the begining of the buffer
            // loffs[i] = arank(_myrank);
            loffs[i] = 0;
          }
        }

        //this->getRList(uphase, rpes, nrpes, rlens);
        getRList(uphase, rpes, nrpes, rlens);
        assert(nrpes <= this->_nports);

        if (roffs && rlens)
        {
          for (int i = 0; i < nrpes; ++i)
          {
            // expect the executor to reshuffle the buffer for root
            // always use relative rank to calculate the offset
            roffs[i] = (rpes[i]  + this->_nranks - this->_myrank) % this->_nranks;
          }
        }
      }
    };


    ///
    /// \brief Simple K-nomial tree Gather schedule
    ///
    template <unsigned P>
    class KnomialGatherSchedule : public GenericTreeSchedule < P, 1, P + 1 >
    {
    public:

      KnomialGatherSchedule (int myrank, int nranks) : GenericTreeSchedule < P, 1, P + 1 > (myrank, nranks)
      {
      }

      using GenericTreeSchedule < P, 1, P + 1 >::getLList;
      using GenericTreeSchedule < P, 1, P + 1 >::getRList;
      using GenericTreeSchedule < P, 1, P + 1 >::getRListNpes;
      using GenericTreeSchedule < P, 1, P + 1 >::init;

      void init(int root, int op, int &startphase, int &nphases)
      {
        int maxranks, lstartphase;
        init(root, op, lstartphase, startphase, nphases, maxranks);
      }

      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        //this->getRList(this->_nphs - uphase -1, lpes, nlpes, llens);
        getRList(this->_nphs - uphase - 1, lpes, nlpes, llens);
        assert(nlpes <= this->_nports);

        if (loffs && llens)
        {
          for (int i = 0; i < nlpes; ++i)
          {
            loffs[i] = (rpes[i]  + this->_nranks - this->_myrank) % this->_nranks;
          }
        }

        //this->getLList(this->_nphs - uphase - 1, rpes, nrpes, rlens);
        getLList(this->_nphs - uphase - 1, rpes, nrpes, rlens);
        assert(nrpes == 1 || nrpes == 0);

        if (roffs && rlens)
        {
          for (int i = 0; i < nrpes; ++i)
          {
            roffs[i] = 0;
          }
        }
      }
    };

    template <unsigned P>
    class S_FlatGatherScatterSchedule : public GenericTreeSchedule<P, 1, 1>
    {
    public:
      S_FlatGatherScatterSchedule (int myrank, int nrank) :
      GenericTreeSchedule<P, 1, 1>(myrank, nrank)
      {

      }

    };

    ///
    /// \brief Simple flat tree Gatter(v) Scatter(v) schedule. For Gatterv,
    ///        and Scatterv, the executor needs to adjust lens and offsets.
    ///        And the send buffer at the root needs to be properly
    ///        reshuffled when root is not rank 0
    ///
    template < unsigned P = 1 >
    class FlatGatherScatterSchedule
    {
    public:
      static const unsigned _nports  = P;

      FlatGatherScatterSchedule (int myrank, int nranks) :
      _myrank (myrank),
      _nranks (nranks),
      _root (0),
      _op (-1)
      {
      }

      // calculate relative rank with respect to root
      int    rrank(int rank)
      {
        return(((rank) + _nranks - _root) % _nranks);
      }
      // calculate absolute rank
      int    arank(int rank)
      {
        return(((rank) + _root) % _nranks);
      }

      void init(int root, int op, int &startphase, int &nphases)
      {
        int maxranks;
        init(root, op, startphase, nphases, maxranks);
      }

      void init(int root, int op, int &startphase, int &nphases, int &maxranks)
      {
        _root       = root;
        _op         = op;
        nphases     = (_nranks + _nports - 2) / _nports;

        if (_myrank != _root)
        {
          startphase = (rrank(_myrank) - 1) / _nports;
          maxranks   = 1;
        }
        else
        {
          startphase = 0;
          maxranks   = _nports;
        }
      }

      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        if (_op == SCATTER)
        {
          _getList(uphase, lpes, nlpes, rpes, nrpes, loffs, llens, roffs, rlens) ;
        }
        else if (_op == GATHER)
        {
          _getList(uphase, rpes, nrpes, lpes, nlpes, roffs, rlens, loffs, llens);
        }
        else
        {
          assert(0);
        }
      }

      void _getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                     unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                     size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {

        if (_myrank == _root)
        {
          nlpes = 0;
          int starts = (int)uphase * _nports + 1;
          nrpes = (starts + _nports <= _nranks) ? _nports : _nranks - starts;

          for (int i = 0; i < nrpes; ++i)
          {
            rpes[i]  = arank(starts + i);

            if (roffs && rlens)
            {
              roffs[i] = starts + i;
              rlens[i] = 1;
            }
          }
        }
        else
        {
          nlpes = 0;
          nrpes = 0;

          if (uphase == (rrank(_myrank) - 1) / _nports)
          {
            nlpes = 1;
            lpes[0] = _root;

            if (loffs && llens)
            {
              loffs[0] = 0;
              llens[0] = 1;
            }
          }
        }

      }

    protected:
      int _myrank;
      int _nranks;
      int _root;
      int _op;
    };

#if 0
    ///
    /// \brief Simple K-nomial dissemination schedule. It can be used
    ///        by Barrier and Allgather. Note the schedule must be
    ///        initialized as if every task is the root.
    ///
    template <unsigned P>
    class KnomialAllgatherSchedule : public GenericTreeSchedule < P, 1, P + 1 >
    {
    public:

      KnomialAllgatherSchedule (unsigned myrank, PAMI::Topology *topo) :
      GenericTreeSchedule < P, 1, P + 1 > (myrank, topo)
      {
      }

      void getList (unsigned uphase, unsigned *srcpes, unsigned &nsrc,
                    unsigned *dstpes, unsigned &ndst,
                    size_t *srclens = NULL, size_t *dstlens = NULL)
      {
        int phase = (int)_nphs - uphase - 1;
        ndst = 0;

        if (phase >= _rstartph)
        {
          ndst = (phase < _nphs - 1) ? _nports : _partners.size() - (phase - _rstartph) * _nports;

          for (unsigned i = 0; i < _nports && ((i<(int)nsrc) || (i<(int)ndst)); ++i)
          {
            unsigned  ind = _partners[(phase - _rstartph)*_nports + i];
            if(i<(int)ndst) dstpes[i] = toporank(ind);
            if(i<(int)nsrc) srcpes[i] = toporank((2 * _myrank + _nranks - ind) % _nranks);

            if (dstlens && (i<(int)ndst) )
            {
              dsrlens[i] = _subsizes[(phase - _rstartph)*_nports + i];
            }

            if (srclens && (i<(int)nsrc) )
            {
              srclens[i] = _subsizes[(phase - _rstartph)*_nports + i];
            }
          }
        }

        nsrc = ndst;
      }

    };

    ///
    /// \brief K-port chain algorithm of Allgather
    ///
    // template <unsigned P>
    // class ChainAllgatherSchedule : public GenericTreeSchedule<P, 0, P>
    class ChainAllgatherSchedule
    {
    public:

      ChainAllgatherSchedule (int myrank, int nranks) :
      _myrank(myrank),
      _nranks(nranks)
      {
      }

      void init (int root, int op, int &startphase, int &nphases, int &maxranks)
      {
        startphase = 0;
        nphases   = _nranks - 1;
        maxranks  = 1;
      }

      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes,
                    size_t *llens = NULL, size_t *rlens = NULL)
      {
        int phase = (int)uphase;
        nlpes = nrpes = 1;
        lpes[0] = (_myrank + _nranks - 1) % _nranks;
        rpes[0] = (_myrank + 1) % _nranks;

        if (llens && rlens)
          llens[0] = rlens[0] = 1;
      }

    protected:
      int _myrank;
      int _nranks;

    };
#endif
    ///
    /// \brief Single-port send/recv Alltoall schedule
    ///
    class SRAlltoallSchedule
    {
    public:

      SRAlltoallSchedule (int myrank, int nranks) :
      _myrank(myrank),
      _nranks(nranks)
      {
      }

      void init (int root, int op, int &startphase, int &nphases, int &maxranks)
      {
        (void)root;(void)op;
        startphase = 0;
        nphases   = _nranks - 1;
        maxranks  = 1;
      }

      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        int phase = (int)uphase;
        nlpes = nrpes = 1;
        lpes[0] = (_myrank + _nranks - phase - 1) % _nranks;
        rpes[0] = (_myrank + _nranks - phase) % _nranks;

        if (loffs && llens && roffs && rlens)
        {
          loffs[0] = lpes[0];
          roffs[0] = rpes[0];
          llens[0] = rlens[0] = 1;
        }
      }

    protected:
      int _myrank;
      int _nranks;

    };

    ///
    /// \brief Single-port exchange Alltoall schedule
    ///
    class ExchangeAlltoallSchedule
    {
    public:

      ExchangeAlltoallSchedule (int myrank, int nranks) :
      _myrank(myrank),
      _nranks(nranks)
      {
      }

      void init (int root, int op, int &startphase, int &nphases, int &maxranks)
      {
        (void)root;(void)op;
        startphase = 0;
        nphases   = _nranks - 1;
        maxranks  = 1;
      }

      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        int phase = (int)uphase;
        int tasks = _nranks;
        int partner;

        if (tasks & 1)
        {
          partner = (tasks + 2 * phase - _myrank) % tasks;

          if (partner == _myrank)  partner = -1;
        }
        else
        {
          tasks--;

          if (_myrank == tasks)
            partner = phase;
          else
            partner = (tasks + 2 * phase - _myrank) % tasks;

          if (partner == _myrank)
            partner = tasks;
        }

        if (partner != -1)
        {
          nlpes = nrpes = 1;
          lpes[0] = rpes[0] = partner;

          if (loffs && llens && roffs && rlens)
          {
            loffs[0] = partner;
            roffs[0] = _myrank;
            llens[0] = rlens[0] = 1;
          }
        }
        else
        {
          nlpes = nrpes = 0;
        }
      }

    protected:
      int _myrank;
      int _nranks;

    };

    ///
    /// \brief K-port dissemination Alltoall schedule, based on Bruck's algorithm
    ///
    template <unsigned P>
    class DisseminationAlltoallSchedule
    {
    public:

      static const unsigned _nports = P;

      DisseminationAlltoallSchedule (int myrank, int nranks) :
      _myrank(myrank),
      _nranks(nranks),
      _pow(1),
      _nphs(0)
      {
      }

      void init (int root, int op, int &startphase, int &nphases, int &maxranks)
      {
        for (; _pow < _nranks; _pow *= (_nports + 1)) ++_nphs;

        nphases    = _nphs;
        startphase = 0;
        maxranks   = _nports;
      }

      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        int phase = (int)uphase;

        if (phase < _nphs - 1)
        {
          nlpes = nrpes = _nports;
        }
        else
        {
          nlpes = nrpes = (int) CEILING((double)(_nranks - _pow) / _pow);
        }

        int pow, ph;

        for (pow = 1, ph = 0; ph < phase - 1; pow *= (_nports + 1), ++ph);

        for (int i = 0; i < nrpes; ++i)
        {
          rpes[i] = (_myrank + pow * (i + 1)) % _nranks;
          lpes[i] = (_myrank + _nranks - pow * (i + 1)) % _nranks;

          if (loffs && llens && roffs && rlens)
          {
            loffs[i] = i; // element divide by pow and then modulo _nport equals i
            roffs[i] = i; // element divide by pow and then modulo _nport equals i
            llens[i] = rlens[i] = ((phase == _nphs - 1) && (i == nrpes - 1)) ? _nranks - i * pow : pow;
          }
        }
      }

      void getList (unsigned uphase, unsigned ustep, unsigned &lpe, unsigned &rpe)
      {
        int phase = (int)uphase;
        int step  = (int)ustep;
        int pow, ph;

        for (pow = 1, ph = 0; ph < phase - 1; pow *= (_nports + 1), ++ph);

        rpe = (_myrank + pow * (step + 1)) % _nranks;
        lpe = (_myrank + _nranks - pow * (step + 1)) % _nranks;
      }


    protected:
      int _myrank;
      int _nranks;
      int _pow;     // floor power of (_nports + 1)
      int _nphs;

    };

    ///
    /// \brief Recursive multiplexing schedule, generalization to the recursive doubling
    ///        schedule, can be used by Barrier, Allreduce, Allgather on power of k+1
    ///        participants
    ///
    template <unsigned P>
    class GenericRecursiveSchedule
    {
    public:

      static const unsigned _nports = P;

      GenericRecursiveSchedule (int myrank, int nranks) :
      _myrank(myrank),
      _nranks(nranks),
      _nphs(0)
      {
      }

      void init (int root, int op, int &startphase, int &nphases, int &maxranks)
      {
        int pow;

        for (pow = 1; pow < _nranks; pow *= (_nports + 1)) ++_nphs;

        assert(pow == _nranks);
        nphases    = _nphs;
        startphase = 0;
        maxranks   = _nports;
      }

      void getList (unsigned uphase, unsigned *lpes, unsigned &nlpes,
                    unsigned *rpes, unsigned &nrpes, size_t *loffs = NULL,
                    size_t *llens = NULL, size_t *roffs = NULL, size_t *rlens = NULL)
      {
        int phase = (int)uphase;
        nlpes = nrpes = _nports;
        int prev_pow, pow, ph, first;

        for (prev_pow = 1, ph = 0; ph < phase - 1; prev_pow *= (_nports + 1), ++ph);

        pow = prev_pow * (_nports + 1);
        first = _myrank / pow * pow;

        for (int i = 0; i < nrpes; ++i)
        {
          lpes[i] = rpes[i] = lpes[i] = (first + ((_myrank % pow) + prev_pow * (i + 1)) % pow) % _nranks;

          if (loffs && llens && roffs && rlens)
          {
            loffs[i] = roffs[i] = 0;
            llens[i] = rlens[i] =  prev_pow;
          }
        }
      }

    protected:
      int _myrank;
      int _nranks;
      int _nphs;

    };

  } // Schedule
} // CCMI

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#undef DO_DEBUG
#define DO_DEBUG(x) //x

#endif /* !__algorithms_schedule_GenericTreeT_h__ */
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
