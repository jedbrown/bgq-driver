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
 * \file algorithms/schedule/MultinomialMap.h
 * \brief ???
 */

#ifndef __algorithms_schedule_MultinomialMap_h__
#define __algorithms_schedule_MultinomialMap_h__

#include "algorithms/interfaces/Schedule.h"

namespace CCMI
{
  namespace Schedule
  {

    ///
    /// \brief The Map interface for the Multinomial schedule. The
    ///        multinomial schedule just operates on ranks 0-N, where
    ///        0-m nodes for a multinomial tree of radix k. The remaining
    ///        nodes are auxilliary nodes that send messages to the
    ///        internal nodes.
    ///
    template <class T> class MultinomialMap
    {
      public:
        MultinomialMap () {}

        void setRoot (unsigned r )
        {
          static_cast<T*>(this)->setRoot(r);
        }

	///
        /// \brief Do we have aux phases in addition to binomial
        ///
        bool hasAuxPhases   ()
        {
          return static_cast<T*>(this)->hasAuxPhases();
        }

        ///
        /// \brief Is the rank an auxilary processor
        /// \param [in] rank the rank of the processor
        ///
        bool isAuxProc   ()
        {
          return static_cast<T*>(this)->isAuxProc();
        }

        ///
        /// \brief Is the rank a peer processor that takes the data from
        ///        the auxillary processor and participates in the
        ///        binomial collective. At the end of the collective
        ///        operation result will be returned to the auxillary
        ///        processor
        /// \param [in] the rank of the processor
        ///
        bool isPeerProc  ()
        {
          return static_cast<T*>(this)->isPeerProc();
        }

        ///
        /// \brief Get the aux processor for this peer processor
        /// \param [in] the rank of the aux processor
        /// \retval the rank of the peer processor
        ///
        void  getAuxForPeer (unsigned *ranks, unsigned &nranks)
        {
          return static_cast<T*>(this)->getAuxForPeer(ranks, nranks);
        }

        ///
        /// \brief Get the rank of the peer processor for the aux processor
        /// \param [in] The rank of the peer processor
        /// \retval the rank of the aux processor
        ///
        unsigned getPeerForAux ()
        {
          return static_cast<T*>(this)->getPeerForAux();
        }

        ///
        /// \brief Convert the rank to the global rank for the msend
        ///        interface
        ///
        unsigned getGlobalRank (unsigned relrank)
        {
          return static_cast<T*>(this)->getGlobalRank(relrank);
        }

        ///
        /// \brief Get the root of the collective
        ///
        unsigned getRoot ()
        {
          return static_cast<T*>(this)->getRoot();
        }

        ///
        /// \brief Get my rank in the collective
        ///
        unsigned getMyRank ()
        {
          return static_cast<T*>(this)->getMyRank();
        }

        unsigned getNumRanks ()
        {
          return static_cast<T*>(this)->getNumRanks();
        }

    };  //Multinomial map

    class TopologyMap : public MultinomialMap<TopologyMap>
    {
      public:
        TopologyMap () {}

        TopologyMap (pami_endpoint_t  myendpoint,
                     PAMI::Topology  *topology):
          _topology(topology)
        {
          unsigned nph = 0;
          //_my_ep       = myendpoint;
          _myindex = topology->endpoint2Index ( myendpoint );
          TRACE_SCHEDULE((stderr,  "<%p>TopologyMap myindex %zu ,myep %u\n", this,_myindex,myendpoint));
          //for (unsigned i =0;i< topology->size(); i++ )
          //  {
          //  TRACE_SCHEDULE((stderr,  "<%p> size %zu, topology[%u] rank=%u, ep=%u, index = %zu/%zu\n", this,topology->size(), i, topology->index2Rank(i), topology->index2Endpoint(i),topology->rank2Index(topology->index2Rank(i)), topology->endpoint2Index(topology->index2Endpoint(i))));
          //  }
          for (unsigned i = topology->size(); i > 1; i >>= 1)
            {
              nph++;
            }

          _hnranks = 1 << nph;
          _rootindex = 0;
          TRACE_SCHEDULE((stderr,  "<%p>TopologyMap(%u,%p) myindex %zu ,nph %u\n", this,myendpoint,topology,_myindex,nph));
        }

        ~TopologyMap () {}

	bool hasAuxPhases () {
	  size_t size = _topology->size();
	  return ((size & (size-1)) != 0);
	}	  

        void setRoot (unsigned gr)
        {
          _rootindex = _topology->endpoint2Index((pami_task_t)gr);
          TRACE_SCHEDULE((stderr,  "<%p>TopologyMap::setRoot(unsigned %u) %zu \n", this,gr,_rootindex));
        }

        ///
        /// \brief Is the rank an auxilary processor
        ///
        bool isAuxProc   ()
        {
          if (getMyRank() >= _hnranks)
            return true;

          return false;
        }

        ///
        /// \brief Is the rank a peer processor that takes the data from
        ///        the auxillary processor and participates in the
        ///        binomial collective. At the end of the collective
        ///        operation result will be returned to the auxillary
        ///        processor
        ///
        bool isPeerProc  ()
        {
          if (getMyRank() < (_topology->size() - _hnranks))
            return true;

          return false;
        }

        ///
        /// \brief Get the aux processor for this peer processor
        /// \param [in] the rank of the aux processor
        /// \retval the rank of the peer processor
        ///
        void  getAuxForPeer (unsigned *ranks, unsigned &nranks)
        {
	  CCMI_assert (isPeerProc());
	  nranks = 1;
          ranks[0] = getMyRank() + _hnranks;
        }

        ///
        /// \brief Get the rank of the peer processor for the aux processor
        /// \param [in] The rank of the peer processor
        /// \retval the rank of the aux processor
        ///
        unsigned getPeerForAux ()
        {
	  CCMI_assert (isAuxProc());
          return getMyRank() - _hnranks;
        }

        ///
        /// \brief Convert the rank to the global rank for the msend
        ///        interface
        ///
        pami_endpoint_t getGlobalRank (unsigned relrank)
        {
          relrank += _rootindex;
         if (relrank >= _topology->size())
           relrank -= _topology->size();
          TRACE_SCHEDULE((stderr,  "<%p>TopologyMap::getGlobalRank(unsigned %u) %u \n", this,relrank,_topology->index2Endpoint(relrank)));
          return _topology->index2Endpoint(relrank);
        }

        ///
        /// \brief Get my rank in the collective
        ///
        unsigned getMyRank ()
        {
          TRACE_SCHEDULE((stderr,  "<%p>TopologyMap::getMyRank() myindex %zu, size %zu, root %zu, my rank = %zu \n", this,_myindex,_topology->size(),_rootindex,_myindex >= _rootindex? _myindex - _rootindex:_myindex + _topology->size() - _rootindex));
          if (_myindex >= _rootindex)
            return _myindex - _rootindex;

          return _myindex + _topology->size() - _rootindex;
        }

        unsigned getNumRanks () { return _topology->size(); }

      protected:
        size_t              _hnranks;   /** Nearest power of 2 */
        size_t              _rootindex; /** Index of the root */
        size_t              _myindex;   /** Index of my node */
        PAMI::Topology      * _topology;
        //pami_endpoint_t      _my_ep;
    };

    class NodeOptTopoMap : public MultinomialMap<NodeOptTopoMap>
    {
      public:
        NodeOptTopoMap () {}
        NodeOptTopoMap (unsigned        myindex,
                        pami_endpoint_t myendpoint,
                        PAMI::Topology *all_topology,
                        PAMI::Topology *local_topology,
                        PAMI::Topology *master_topology) :
          _myindex(myindex),
          _myendpoint(myendpoint),
          _all_topology(all_topology),
          _master_topology(master_topology),
          _local_topology(local_topology)
        {
          unsigned nph = 0;
	  _local_master = _local_topology->index2Endpoint(0);
	  _master_index = _master_topology->endpoint2Index(_local_master);
          for (unsigned i = _master_topology->size(); i > 1; i >>= 1)
	    nph++;
	  //Current we do not support non powers of two
	  PAMI_assert (_master_topology->size() == (size_t)(1 << nph));
	  _hnranks = 1 << nph;
	  _rootindex = 0;
        }
	
        ~NodeOptTopoMap () {}

	bool hasAuxPhases () {
	  return (_master_topology->size() != _all_topology->size());
	}

        void setRoot (unsigned gr)
        {
	  CCMI_abort(); //Currently, we only support barrier and allreduce
          _rootindex = _all_topology->endpoint2Index((pami_task_t)gr);
        }

        ///
        /// \brief Is the rank an auxilary processor
        /// \param [in] rank the rank of the processor
        ///
        bool isAuxProc   ()
        {
          if (_myendpoint != _local_master)
            return true;

          return false;
        }

        ///
        /// \brief Is the rank a peer processor that takes the data from
        ///        the auxillary processor and participates in the
        ///        binomial collective. At the end of the collective
        ///        operation result will be returned to the auxillary
        ///        processor
        /// \param [in] the rank of the processor
        ///
        bool isPeerProc  ()
        {
          if (_myendpoint == _local_master && _local_topology->size() > 1)
            return true;
	  
          return false;
        }

        ///
        /// \brief Get the aux processor for this peer processor
        /// \param [in] the rank of the aux processor
        /// \retval the rank of the peer processor
        ///
        void  getAuxForPeer (unsigned *ranks, unsigned &nranks)
        {
	  nranks = 0;
          for (size_t i = 1; i < _local_topology->size(); i++)
	    //1 ... master_size -1 is for the binomial tree and the rest are peer ranks
	    ranks[nranks ++] = (unsigned)(_master_topology->size() + i);
        }

        ///
        /// \brief Get the rank of the peer processor for the aux processor
        /// \param [in] The rank of the peer processor
        /// \retval the rank of the aux processor
        ///
        unsigned  getPeerForAux ()
        {
	  return _master_index;
        }

        ///
        /// \brief Convert the rank to the global rank for the msend
        ///        interface
        ///
        unsigned getGlobalRank (unsigned relrank)
        {
	  unsigned grank = 0;
	  if (relrank < _master_topology->size())
	    grank = _master_topology->index2Rank((size_t)relrank);
	  else
	    grank = _local_topology->index2Rank((size_t)relrank - _master_topology->size());

	  //fprintf(stderr, "Global rank %d = %d\n", relrank, grank);
	  return grank;
        }

        ///
        /// \brief Get my rank in the collective
        ///
        unsigned getMyRank ()
        {
          return _master_index;
        }

        unsigned getNumRanks () { return _master_topology->size(); }

      protected:
        unsigned            _hnranks;   /** Nearest power of 2 */
        unsigned            _rootindex; /** Index of the root */
        unsigned            _myindex;   /** Index of my node */
        pami_endpoint_t     _myendpoint;    /** Global rank of my node */
	size_t              _master_index;  /** Index of the master task */
        pami_endpoint_t     _local_master;  /*  Global rank of this task's master*/
        PAMI::Topology    * _all_topology;
	PAMI::Topology    * _master_topology;
	PAMI::Topology    * _local_topology;
    };
  };
};

#endif
