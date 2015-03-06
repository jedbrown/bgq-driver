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
 * \file algorithms/interfaces/Schedule.h
 * \brief ???
 */

#ifndef __algorithms_interfaces_Schedule_h__
#define __algorithms_interfaces_Schedule_h__

#include "util/ccmi_util.h"
#include "util/ccmi_debug.h"
#include "algorithms/ccmi.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "Topology.h"

/*-------------------------------------------------*/
/*     Basic utility classes collectives           */
/*-------------------------------------------------*/

namespace CCMI
{
  template<class T_Geometry>
    struct SFunc
  {
    typedef void      (*ScheduleFn)   (void                        * buf,
				       unsigned                      size,
				       unsigned                      root,
				       Interfaces::NativeInterface * native,
				       T_Geometry         * g);
  };


  /**
   * \brief The different collective operations supported
   */
  typedef enum
  {
    BROADCAST_OP  =      1,
    BARRIER_OP    =      2,
    ALLREDUCE_OP  =      4,
    REDUCE_OP     =      8
  } CollectiveOperation;

  namespace Interfaces
  {
    /**
     * \brief Abstract reference class to define the schedule of a collective
     */
    class Schedule
    {
    public:

      /**
       * \brief Constructor
       */
      Schedule ()
      {
      }

      /**
       * \brief Destructor
       */
      virtual ~Schedule ()
      {
      }

      /// NOTE: This is required to make "C" programs link successfully with virtual destructors
      void operator delete(void * p)
      {
        (void)p;
        CCMI_abort();
      }

      /**
       * \brief Initialize the schedule for collective operation
       * \param root : the root of the collective
       * \param startphase : The phase where I become active
       * \param nphases : number of phases
       */
      virtual void
      init(int root, int op, int &startphase, int &nphases)=0 ;

      /**
       * \brief Get the upstream processors. Source processors
       * that send messages to me in this collective operation
       * \param[IN] phase : phase of the collective
       * \param[INOUT] topology : the topolgy that sends messages to me in this phase
       */
      virtual void
      getSrcTopology (unsigned phase, PAMI::Topology *topology, pami_endpoint_t *src_eps)=0;

      /**
       * \brief Get the downstream processors to send data to.
       * \param phase : phase of the collective
       * \param[INOUT] topology : The topology to send messages to in this phase
       */
      virtual void
      getDstTopology (unsigned phase, PAMI::Topology *topology, pami_endpoint_t *dst_eps)=0;

      /**
       * \brief Get the union of all sources across all phases
       * \param[INOUT] topology : the union of all sources
       */
      virtual pami_result_t
      getSrcUnionTopology (PAMI::Topology  *topology,
                           pami_endpoint_t *src_eps)=0;

      /**
       * \brief Get the union of all destinations across all phases
       * \param[INOUT] topology : the union of all sources
       */
      virtual pami_result_t
      getDstUnionTopology (PAMI::Topology *topology,
                           pami_endpoint_t *dst_eps=NULL,
                           unsigned num_eps=0)=0;

      /**
       * \brief Get the last reduce phase
       * 
       */
      virtual unsigned  
      getLastReducePhase()
      {
          return -1;
      }

    };  //-- Schedule
  };  //-- Schedule
};  //-- CCMI

#endif
