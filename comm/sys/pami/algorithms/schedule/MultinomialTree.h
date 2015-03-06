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
 * \file algorithms/schedule/MultinomialTree.h
 * \brief ???
 */

#ifndef __algorithms_schedule_MultinomialTree_h__
#define __algorithms_schedule_MultinomialTree_h__

#include "algorithms/interfaces/Schedule.h"
#include "algorithms/schedule/MultinomialMap.h"

namespace CCMI
{
  namespace Schedule
  {

    /**
     * @defgroup PHASE_CODES Codes used to represent implicite phases
     *@{
     */
#define UNDEFINED_PHASE ((unsigned)-1)  ///< Undefined phase
#define ALL_PHASES  ((unsigned)-2)  ///< All phases in bino
#define NO_PHASES ((unsigned)-3)  ///< No phase
#define NOT_RECV_PHASE  ((unsigned)-4)  ///< All except recv phases
#define NOT_SEND_PHASE  NOT_RECV_PHASE  ///< All except send phases
    /**@}*/

    /**
     * \brief Convert a node to its peer in the binomial tree
     * \param[in] node  Node to convert, relative to root
     * \param[in] phase Phase of algorithm
     * \return  Converted node
     */
#define BINO(nodes, nranks, mynode, phase, nph, radix, lradix)  \
    nranks = 0;             \
    for (unsigned r=1; r < radix; r ++)       \
      nodes[nranks++] = (mynode ^ (r << (phase * lradix)));

    //-------------------------------------------------------------
    //------ Supports a generic barrier and allreduce -------------
    //------ The number of phases is logrithmic with a max of 20 --
    //-------------------------------------------------------------
    template <class M, int T_MaxRadix=2> class MultinomialTreeT : public Interfaces::Schedule
    {
      protected:

        void setupContext(unsigned &start, unsigned &nphases);
        unsigned  getLastReducePhase()
        {
          return _lastrdcph;
        }

        /**
         * \brief Get next node in schedule
         *
         * Returns the node we should communicate with in this phase
         *
         * \param[in] parent  whether to go left or right...
         * \param[in] ph  phase to work on
         * \return  Next node in algorithm
         */
        inline void NEXT_NODES(bool parent, unsigned ph, unsigned *nodes, unsigned &nranks)
        {
          // This phase is only for communication
          // between outside and peer. Don't care
          // about 'parent' here - simply flip to
          // other side of what we are.
          if ( ((ph == _auxsendph) && !parent) ||
               ((ph == _auxrecvph) &&  parent) )
            {
              if (_map.isAuxProc())
                {
                  nodes[0] = _map.getPeerForAux();
                  nranks = 1;
                }
              else if (_map.isPeerProc())
                {
		  //There can be multiple aux processors that can point
		  //to the same peer processor
                  _map.getAuxForPeer(nodes, nranks);
                }
            }
          else if (ph != _auxrecvph && ph != _auxsendph)
            {
              // Multinomial part of operation. All of these
              // phases involve a power-of-two set of nodes.
              // Phase numbers start at one here, but the
              // algorithm requires zero.
              ph -= 1;

              BINO(nodes, nranks, _map.getMyRank(), ph, _nphbino, _radix, _logradix);
              CCMI_assert(nranks >= 1);
            }

          if (nranks > 0)
            {TRACE_SCHEDULE ((stderr, "%d: phase %d, node %d\n", _map.getMyRank(), ph, nodes[0]));}
        }

        /**
         * \brief Create Multinomial Schedule for a simple range of indices (or coords on a line)
         *
         * Also called from Rectangle.h when doing a reduce along an axis of the rectangle.
         *
         * \param[in] x   Our coord on axis
         * \param[in] x0  Lowest coord on axis
         * \param[in] xN  Highest coord on axis
         * \param[in] ranks (optional) Pointer to list of ranks (saved until later)
         * \return  nothing
         */
        inline void initBinoSched()
        {
	  _nranks = _map.getNumRanks();
          _op = -1;

          _radix = getRadix (_nranks);
          _logradix = 1;

          if (_radix == 8)
            _logradix = 3;
          else if (_radix == 4)
            _logradix = 2;

          _maxphases = getMaxPhases(_nranks, &_nphbino);
	  _maxphases += _map.hasAuxPhases();

          _hnranks = (1 << (_nphbino * _logradix)); // threshold for special handling
          TRACE_SCHEDULE((stderr, "<%p> initBinoSched()"
                          "_maxphases = %u, _nphbino   = %u, _op        = %u, _radix     = %u, _logradix  = %u, "
                          "_nranks    = %u, _hnranks   = %u, _sendph    = %u, _recvph    = %u, _auxsendph = %u, "
                          "_lastrdcph = %u, _auxrecvph = %u, _startphase= %u, _nphases   = %u\n",
                          this,
                          _maxphases, _nphbino, _op, _radix, _logradix,
                          _nranks, _hnranks, _sendph, _recvph, _auxsendph,
                          _lastrdcph, _auxrecvph, _startphase, _nphases));
        }

      public:

        ///
        /// \brief Enable a higher radix binomial algorithm
        ///
        static unsigned getRadix (unsigned nranks)
        {
          int nph = 0;
          int radix = 2;

          for (unsigned i = nranks; i > 1; i >>= 1) {
	    nph++;
	  }

          if ( (T_MaxRadix >= 8) && (nranks <= 4096) && ((nph % 3) == 0) )  //multiple of 3
            radix = 8;
          else if ( (T_MaxRadix >= 4) && ((nph & 1) == 0) ) //multiple of 2
            radix = 4;
	  
          TRACE_SCHEDULE ((stderr, "<> getRadix() nranks %u, radiz %u\n", nranks, radix));
          return radix;
        }

        static unsigned getMaxPhases(unsigned nranks, unsigned *nbino = NULL)
        {
          unsigned nph;
          unsigned radix = getRadix (nranks);

          /* figure out the number of phases */
          nph = 0;

          if (nranks)
            {
              for (unsigned i = nranks; i > 1; i >>= 1)
                {
                  nph++;
                }

              if (radix == 8)
                nph /= 3;
              else if (radix == 4)
                nph /= 2;
            }

          if (nbino)
            {
              *nbino = nph;
            }

          nph += 1; // power of two starts on phase 1 anyway.

#if 0 //Query map for this
          if ((nranks & (nranks - 1)) != 0)
            {
              nph += 1; // non-power of two adds phase 0 and phase N+1
            }
#endif

          TRACE_SCHEDULE ((stderr, "<> getMaxPhases() nranks %u, nph %u\n", nranks, nph));
          return nph;
        }

        /**
         * \brief Constructor for Multinomial Schedule
         */
        MultinomialTreeT () :
            Schedule (),
            _maxphases(0)
        {
          TRACE_SCHEDULE((stderr,  "<%p>MultinomialTreeT ()\n", this));
        }

        /**
         * \brief Constructor for list of ranks
         *
         * \param[in] nranks  Number of ranks in list
         * \param[in] ranks Ranks list
         */
        MultinomialTreeT (unsigned myendpoint, PAMI::Topology *topo, unsigned c = 0);


        /**
         * \brief Constructor for list of ranks
         *
         * \param[in] ranks Ranks list
         * \param[in] nranks  Number of ranks in list
         */
        MultinomialTreeT (unsigned endpoint, size_t *ranks, unsigned nranks);

        virtual void
        init(int root, int op, int &startphase, int &nphases, int &maxranks)
        {PAMI_abort();}
        virtual void getSrcPeList (unsigned  phase, unsigned *srcpes,
                                   unsigned  &nsrc, unsigned *subtasks = NULL)
        {PAMI_abort();}
        virtual void getDstPeList (unsigned  phase, unsigned *dstpes,
                                   unsigned  &ndst, unsigned *subtasks)
        {PAMI_abort();}

        /**
         * \brief Initialize Multinomial Schedule
         *
         * \param[in] root    Root node of collective
         * \param[in] comm_op   Collective operation to do
         * \param[out] startphase Starting phase for this node
         * \param[out] nphases    Number of phases for this node
         * \param[out] nranks   Max num steps per phase
         */
        virtual void init(int root, int comm_op, int &startphase,
                          int &nphases);

        /**
         * \brief Get source phase steps
         *
         * \param[in] phase Phase to work on
         * \param[INOUT] srcranks are filled in the topology
         */
        virtual void getSrcTopology(unsigned phase, PAMI::Topology *topology, pami_endpoint_t *src_eps)
        {
          unsigned nsrc = 0;
          if ((phase >= 1 && phase <= _nphbino && (_recvph == ALL_PHASES ||
                                                   (_recvph == NOT_SEND_PHASE && phase != _sendph) ||
                                                   phase == _recvph)) || phase == _auxrecvph)
            {
              NEXT_NODES(PARENT, phase, src_eps, nsrc);
            }
          TRACE_SCHEDULE ((stderr, "<%p> getSrcTopology() phase %u, nsrc %u\n", this, phase, nsrc));
          for (unsigned count = 0; count < nsrc; count ++)
            {
              TRACE_SCHEDULE ((stderr, "<%p> getSrcTopology() src_eps %u/%u\n", this, src_eps[count], _map.getGlobalRank(src_eps[count])));
              src_eps[count] = _map.getGlobalRank(src_eps[count]);
            }

          //Convert to a endpoint list topology
          new (topology) PAMI::Topology (src_eps, nsrc, PAMI::tag_eplist());
        }

        /**
         * \brief Get destination phase steps
         *
         * \param[in] phase Phase to work on
         * \param[INOUT] dstranks are filled in the topology
         */
        virtual void getDstTopology(unsigned phase, PAMI::Topology *topology, pami_endpoint_t *dst_eps)
        {
          unsigned ndst = 0;

          if ((phase >= 1 && phase <= _nphbino && (_sendph == ALL_PHASES ||
                                                   (_sendph == NOT_RECV_PHASE && phase != _recvph) ||
                                                   phase == _sendph)) || phase == _auxsendph)
            {
              NEXT_NODES(CHILD, phase, dst_eps, ndst);
            }
          TRACE_SCHEDULE((stderr, "<%p> getDstTopology() phase %u, ndst %u\n", this, phase, ndst));

          for (unsigned count = 0; count < ndst; count ++)
            {
              TRACE_SCHEDULE((stderr, "<%p> getDstTopology() dst_eps %u/%u\n", this, dst_eps[count], _map.getGlobalRank(dst_eps[count])));
              dst_eps[count]   = _map.getGlobalRank(dst_eps[count]);
              TRACE_SCHEDULE((stderr, "%d: phase %d, index %d node %d\n", _map.getMyRank(), phase, count, dst_eps[count]));
            }

          //Convert to a list topology of the accurate size
          new (topology) PAMI::Topology (dst_eps, ndst, PAMI::tag_eplist());
        }

        /**
         * \brief Get the union of all sources across all phases
         * \param[INOUT] topology : the union of all sources
         */
        virtual pami_result_t getSrcUnionTopology (PAMI::Topology  *topology,
                                                   pami_endpoint_t *src_eps)
        {
          unsigned ntotal_src = 0;

          for (unsigned phase = _startphase; phase < _startphase + _nphases; phase++)
            {
              unsigned nsrc = 0;

              if ((phase >= 1 && phase <= _nphbino && (_recvph == ALL_PHASES ||
                                                       (_recvph == NOT_SEND_PHASE && phase != _sendph) ||
                                                       phase == _recvph)) || phase == _auxrecvph)
                {
                  NEXT_NODES(PARENT, phase, src_eps + ntotal_src, nsrc);
                  ntotal_src += nsrc;
                }
            }

          TRACE_SCHEDULE ((stderr, "<%p> getSrcUnionTopology() ntotal_src %u\n", this, ntotal_src));
          for (unsigned count = 0; count < ntotal_src; count ++)
            {
              TRACE_SCHEDULE ((stderr, "<%p> getSrcUnionTopology() src_eps %u/%u\n", this, src_eps[count], _map.getGlobalRank(src_eps[count])));
              src_eps[count] = _map.getGlobalRank(src_eps[count]);
            }
          //Convert to a list topology
          new (topology) PAMI::Topology (src_eps, ntotal_src, PAMI::tag_eplist());
          return PAMI_SUCCESS;
        }

        /**
         * \brief Get the union of all destinations across all phases
         * \param[INOUT] topology : the union of all sources
         */
        pami_result_t getDstUnionTopology (PAMI::Topology  *topology,
                                           pami_endpoint_t *dst_eps,
                                           unsigned num_eps)
        {
          CCMI_assert(dst_eps != NULL);
          unsigned ntotal_dst = 0;
          unsigned phase;

          for (phase = _startphase; phase < _startphase + _nphases; phase++)
            {
              unsigned ndst = 0;

              if ((phase >= 1 && phase <= _nphbino && (_sendph == ALL_PHASES ||
                                                       (_sendph == NOT_RECV_PHASE && phase != _recvph) ||
                                                       phase == _sendph)) || phase == _auxsendph)
                {
                  // one destination per radix per phase
                  if((_radix+ntotal_dst)>num_eps)
                      return PAMI_ENOMEM;
                  NEXT_NODES(CHILD, phase, dst_eps + ntotal_dst, ndst);
                }
              ntotal_dst += ndst;
            }
          TRACE_SCHEDULE ((stderr, "<%p> getDstUnionTopology() ntotal_dst %u\n", this, ntotal_dst));

          for (unsigned count = 0; count < ntotal_dst; count ++)
            {
              TRACE_SCHEDULE ((stderr, "<%p> getDstUnionTopology() dst_eps %u/%u\n", this, dst_eps[count], _map.getGlobalRank(dst_eps[count])));
              dst_eps[count]   = _map.getGlobalRank(dst_eps[count]);
              TRACE_SCHEDULE ((stderr, "%d: phase %d, index %d node %d\n", _map.getMyRank(), phase, count, dst_eps[count]));
            }
          //Convert to a list topology of the accurate size
          new (topology) PAMI::Topology (dst_eps, ntotal_dst,PAMI::tag_eplist());
          return PAMI_SUCCESS;
        }

      protected:
        unsigned     _maxphases;  /// \brief Number of phases total
        unsigned     _nphbino;    /// \brief Num of phases in pow-of-2 sec
        unsigned     _op;         /// \brief Collective op
        unsigned     _radix;      /// \brief Radix of the collective operation (default 2)
        unsigned     _logradix;   /// \brief Log of radix of the collective operation (default 1)
        unsigned     _nranks;     /// \brief Number of ranks[]
        unsigned     _hnranks;    /// \brief Number of ranks in pow-2 sec
        unsigned     _sendph;     /// \brief sending phase for this node
        unsigned     _recvph;     /// \brief recving phase for this node
        unsigned     _auxsendph;  /// \brief outside send phase
        unsigned     _auxrecvph;  /// \brief outside recv phase
        unsigned     _startphase; /// \brief the start phase after init is called
        unsigned     _nphases;    /// \brief the total number of phases after init is called
        unsigned     _lastrdcph;  /// \brief last reduce phase

        PAMI::Topology     _topology;  /// \brief goes away when geoemetries store topologies
        M            _map;

        static const bool PARENT = true;
        static const bool CHILD  = false;

    };    //Multinomial Tree Schedule

    //typedef MultinomialTreeT<LinearMap>  LinearMultinomial;
    //typedef MultinomialTreeT<ListMap> ListMultinomial;
    typedef MultinomialTreeT<TopologyMap> TopoMultinomial;
    typedef MultinomialTreeT<TopologyMap, 4> TopoMultinomial4;
    typedef MultinomialTreeT<TopologyMap, 8> TopoMultinomial8;
    typedef MultinomialTreeT<NodeOptTopoMap, 8> NodeOptTopoMultinomial;

  };   //Schedule
}; //CCMI


//--------------------------------------------------------------
//------  Multinomial Schedule Functions -------------------------
//--------------------------------------------------------------

/**
 * \brief Constructor for list of ranks
 *
 * \param[in] myendpoint  My rank in COMM_WORLD
 * \param[in] topology  topology across which the binomial must be performed
 */
template <class M, int T_MaxRadix>
  inline CCMI::Schedule::MultinomialTreeT<M, T_MaxRadix>::
MultinomialTreeT(unsigned        myendpoint,
                 PAMI::Topology *topology,
                 unsigned        c):
  _map(myendpoint, topology)
{
  TRACE_SCHEDULE((stderr,  "<%p>MultinomialTreeT(unsigned myendpoint, PAMI::Topology *topology, unsigned c): _map(myendpoint, topology) myendpoint %u, index %zu, nranks %zu, c %u\n", this, myendpoint, topology->endpoint2Index(myendpoint), topology->size(), c));

  DO_DEBUG(unsigned i = 0;)
  DO_DEBUG(for (i = 0; i < topology->size(); ++i) fprintf(stderr, "<%p> topology[%u] = %u\n", this, i, topology->index2Endpoint(i)););

  initBinoSched();
}


/**
 * \brief Constructor for list of ranks
 *
 * \param[in] myendpoint  My rank in COMM_WORLD
 * \param[in] nranks  Number of ranks in list
 * \param[in] ranks Ranks list
 */
template <class M, int T_MaxRadix>
  inline CCMI::Schedule::MultinomialTreeT<M, T_MaxRadix>::
MultinomialTreeT(unsigned       myendpoint,
                 size_t        *ranks,
                 unsigned       nranks): _topology(ranks, nranks), _map()
{
  TRACE_SCHEDULE((stderr,  "<%p> MultinomialTreeT(unsigned myendpoint, size_t *ranks, unsigned nranks): _topology(ranks, nranks), _map() myendpoint %u, nranks %u\n", this, myendpoint, nranks));

  DO_DEBUG(unsigned i = 0;)
  DO_DEBUG(for (i = 0; i < _topology.size(); ++i) fprintf(stderr, "<%p> topology[%u] = %u\n", this, i, _topology.index2Rank(i)););

  CCMI_assert (_topology.type() == PAMI_LIST_TOPOLOGY);

  new (&_map) M (myendpoint, &_topology);
  initBinoSched();
}


/**
 * \brief Setup binomial schedule context
 *
 * Computes important values for later use in building schedule
 * phases/steps. This includes values used to handle non-powers of two.
 * This computes values which require knowing the root node and/or
 * type of collective (_op).
 *
 * \param[in] node  Our node number (index) in ranks[] array,
 *      or position along line of nodes.
 */
template <class M, int T_MaxRadix>
  void CCMI::Schedule::MultinomialTreeT<M, T_MaxRadix>::
setupContext(unsigned &startph, unsigned &nph)
{
  unsigned st, np;

  _recvph = NO_PHASES;
  _sendph = NO_PHASES;

  // Assume (default) we are only performing as a
  // power-of-two (standard binomial).
  st = 1;
  np = _nphbino;
  _lastrdcph = UNDEFINED_PHASE;
  _auxrecvph = NO_PHASES;
  _auxsendph = NO_PHASES;
  TRACE_SCHEDULE((stderr, "<%p> setupContext() _map.getMyRank() %u, _map.isPeerProc() %u,  _map.isAuxProc() %u\n", this,
                  _map.getMyRank(), _map.isPeerProc(), _map.isAuxProc() ));

  if (_map.isPeerProc())
    {
      /* non-power of two */
      switch (_op)
        {
          case BARRIER_OP:
          case ALLREDUCE_OP:
            st = 0;
            np += 2;
            _auxrecvph = st;
            _auxsendph = _maxphases - 1;
            break;
          case REDUCE_OP:
            st = 0;
            np += 1;
            _auxrecvph = st;
            break;
          case BROADCAST_OP:
            st = 1;
            np += 1;
            _auxsendph = _maxphases - 1;
            break;
        }
    }

  if (_map.isAuxProc())
    {
      /* non-power of two */
      switch (_op)
        {
          case ALLREDUCE_OP:
            _lastrdcph = _maxphases - 2;//_auxrecvph -1;
          case BARRIER_OP:
            st = 0;
            np += 2;
            _auxsendph = 0;
            _auxrecvph = _maxphases - 1;
            break;
          case REDUCE_OP:
            st = 0;
            np = 1;
            _auxsendph = 0;
            break;
          case BROADCAST_OP:
            st = _maxphases - 1;
            np = 1;
            _auxrecvph = st;
            break;
        }
    }
  else
    {
      /* might be power of two */
      switch (_op)
        {
          case BARRIER_OP:
          case ALLREDUCE_OP:
            _sendph = ALL_PHASES;
            _recvph = ALL_PHASES;
            break;
          case REDUCE_OP:

            if (_map.getMyRank() == 0) // root
              {
                _recvph = ALL_PHASES;
                _sendph = NO_PHASES;
              }
            else
              {
                np = ffs(_map.getMyRank()) + 1 - st;
                _sendph = st + np - 1;
                _recvph = NOT_SEND_PHASE;
              }

            break;
          case BROADCAST_OP:

            if (_map.getMyRank() == 0) // root
              {
                _recvph = NO_PHASES;
                _sendph = ALL_PHASES;
              }
            else
              {
                //int n = (_nphbino - ffs(_map.getMyRank()));
                int distance = _map.getMyRank() + 1;
                int n = 0, d = 1;

                //We are computing Log(distance)
                while (d < distance)
                  {
                    d *= _radix; //radix of the collective
                    n ++;
                  }

                n --; //first phase is the recv phase
                st += n;
                np -= n;
                _recvph = st;
                _sendph = NOT_RECV_PHASE;
              }

            break;
        }
    }

  startph = st;
  nph = np;
  TRACE_SCHEDULE((stderr, "<%p> setupContext() startph %u, nph %u, "
	  "_maxphases = %u, _nphbino   = %u, _op        = %u, _radix     = %u, _logradix  = %u, "
	  "_nranks    = %u, _hnranks   = %u, _sendph    = %u, _recvph    = %u, _auxsendph = %u, "
	  "_lastrdcph = %u, _auxrecvph = %u, _startphase= %u, _nphases   = %u\n",
	  this, startph, nph,
	  _maxphases, _nphbino, _op, _radix, _logradix,
	  _nranks, _hnranks, _sendph, _recvph, _auxsendph,
	  _lastrdcph, _auxrecvph, _startphase, _nphases));
}

/**
 * \brief Initialize the final schedule
 *
 * \param[in] root  The root node (rank)
 * \param[in] comm_op The collective operation to perform
 * \param[out] start  The starting phase for this node
 * \param[out] nph  The number of phases for this node
 * \param[out] nranks The largest number of steps per phase
 */
template <class M, int T_MaxRadix>
  void CCMI::Schedule::MultinomialTreeT<M, T_MaxRadix>::
init(int root, int comm_op, int &start, int &nph)
{
  TRACE_SCHEDULE ((stderr, "<%p> init() root %d, comm_op %d, start %d, nph %d\n", this, root, comm_op, _startphase, _nphases));
  CCMI_assert(comm_op == BARRIER_OP ||
              comm_op == ALLREDUCE_OP ||
              comm_op == REDUCE_OP ||
              comm_op == BROADCAST_OP);

  _op = comm_op;

  if (comm_op == REDUCE_OP ||
      comm_op == BROADCAST_OP)
    _map.setRoot (root);

  setupContext(_startphase, _nphases);

  nph   = _nphases;
  start = _startphase;
  TRACE_SCHEDULE ((stderr, "<%p> init() root %d, comm_op %d, start %d, nph %d\n", this, root, comm_op, start, nph));
}


#endif /* !__multinomial_tree_schedule__ */
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
