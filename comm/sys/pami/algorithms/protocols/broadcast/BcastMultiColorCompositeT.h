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
 * \file algorithms/protocols/broadcast/BcastMultiColorCompositeT.h
 * \brief ???
 */
#ifndef __algorithms_protocols_broadcast_BcastMultiColorCompositeT_h__
#define __algorithms_protocols_broadcast_BcastMultiColorCompositeT_h__

#include "algorithms/executor/Broadcast.h"
#include "algorithms/executor/Barrier.h"
#include "algorithms/composite/Composite.h"
#include "algorithms/composite/MultiColorCompositeT.h"
#include "algorithms/connmgr/SimpleConnMgr.h"
#include "algorithms/connmgr/RankBasedConnMgr.h"

#include "util/trace.h"

#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif

namespace CCMI
{
  namespace Adaptor
  {
    namespace Broadcast
    {
      ///
      ///  \brief Base class for synchronous broadcasts
      ///
      template < int NUMCOLORS, class T_Sched, class T_Conn, Executor::GetColorsFn pwcfn, PAMI::Geometry::topologyIndex_t T_Geometry_Index = PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX >
      class BcastMultiColorCompositeT : public Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>
      {
      public:
        BcastMultiColorCompositeT()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>",this);
          TRACE_FN_EXIT();
        }

        BcastMultiColorCompositeT(pami_context_t               ctxt,
                                  size_t                       ctxt_id,
                                  Interfaces::NativeInterface              * mf,
                                  T_Conn                                   * cmgr,
                                  pami_geometry_t                             g,
                                  void                                     * cmd,
                                  pami_event_function                         fn,
                                  void                                     * cookie):
        Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>
           
        ( cmgr,
          fn,
          cookie,
          mf,
          NUMCOLORS),
          _geometry ((PAMI_GEOMETRY_CLASS *)g), _status(INTERNAL_BARRIER)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>",this);
          this->_context = ctxt; /// \todo pass this down the ctor chain
          Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::
          initialize (_geometry->comm(),
                      (PAMI::Topology*)_geometry->getTopology(T_Geometry_Index),
                      ((pami_xfer_t *)cmd)->cmd.xfer_broadcast.root,
                      ((pami_xfer_t *)cmd)->cmd.xfer_broadcast.typecount,
                      (PAMI::Type::TypeCode *)((pami_xfer_t *)cmd)->cmd.xfer_broadcast.type,
                      ((pami_xfer_t *)cmd)->cmd.xfer_broadcast.typecount,/*SSS: send type and recv type are the same in bcast*/
                      (PAMI::Type::TypeCode *)((pami_xfer_t *)cmd)->cmd.xfer_broadcast.type,
                      ((pami_xfer_t *)cmd)->cmd.xfer_broadcast.buf,
                      ((pami_xfer_t *)cmd)->cmd.xfer_broadcast.buf);

          SyncBcastPost();
          PAMI_GEOMETRY_CLASS *geometry = ((PAMI_GEOMETRY_CLASS *)g);
          CCMI::Executor::Composite  *barrier =  (CCMI::Executor::Composite *)
          geometry->getKey((size_t)ctxt_id, /// \todo does NOT support multicontext
                           PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE);
          this->addBarrier(barrier);
          barrier->setDoneCallback(Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::cb_barrier_done, this);

          barrier->start();
          TRACE_FN_EXIT();
        }

        void setContext (pami_context_t context) {}

        void reset (pami_geometry_t                             g,
                    void                                     * cmd) 
        {
          pami_xfer_t *xfer = (pami_xfer_t *)cmd;
          Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::
            reset (_geometry->comm(),
                   (PAMI::Topology*)_geometry->getTopology(T_Geometry_Index),
                   xfer->cmd.xfer_broadcast.root,
                   xfer->cmd.xfer_broadcast.typecount,
                   (PAMI::Type::TypeCode*)xfer->cmd.xfer_broadcast.type,
                   xfer->cmd.xfer_broadcast.typecount,
                   (PAMI::Type::TypeCode*)xfer->cmd.xfer_broadcast.type,
                   xfer->cmd.xfer_broadcast.buf,
                   xfer->cmd.xfer_broadcast.buf);

          SyncBcastPost();

          PAMI_GEOMETRY_CLASS *geometry = ((PAMI_GEOMETRY_CLASS *)g);
          CCMI::Executor::Composite  *barrier =  (CCMI::Executor::Composite *)
          geometry->getKey((size_t)0, /// \todo does NOT support multicontext
                           PAMI::Geometry::CKEY_OPTIMIZEDBARRIERCOMPOSITE);
          this->addBarrier(barrier);
          barrier->setDoneCallback(Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::cb_barrier_done, this);

          barrier->start();
        }

        /// \brief constructor for allgather/allgatherv
        BcastMultiColorCompositeT(Interfaces::NativeInterface              *  mf,
                                  T_Conn                                   *  cmgr,
                                  pami_geometry_t                             g,
                                  pami_event_function                         fn,
                                  void                                     *  cookie):
        Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>
        ( cmgr,
          fn,
          cookie,
          mf,
          NUMCOLORS),
          _geometry ((PAMI_GEOMETRY_CLASS *)g),_status(EXTERNAL_BARRIER)
        {
          TRACE_FN_ENTER();
          TRACE_FN_EXIT();
        }

        /// \brief initialize routing for allgatherv
        unsigned initialize (size_t                       root,
                             size_t                       typecount,
                             PAMI::Type::TypeCode       * type,
                             char                       * src,
                             char                       * dst,
			     int                          max_colors=NUMCOLORS)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "<%p>",this);

          Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::initialize 
          (_geometry->comm(),           
           (PAMI::Topology*)_geometry->getTopology(T_Geometry_Index),
           root,
           typecount,
           type,
           typecount,
           type,
           src,
           dst );

	  if (this->getNumColors() > (unsigned)max_colors)
	    return -1;

          SyncBcastPost();
	  
          TRACE_FN_EXIT();
          return 0;
        }

        void start ()
        {
          TRACE_FN_ENTER();
          //If composite calls barrier then do nothing. The barrier
          //completion will trigger data movement
          if (_status == EXTERNAL_BARRIER)
            //Start all executors
            Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::cb_barrier_done(this->_context, this, PAMI_SUCCESS);

          TRACE_FN_EXIT();
        }

        void SyncBcastPost ()
        {
          TRACE_FN_ENTER();
          //fprintf(stderr, "<%p>BcastMultiColorCompositeT::SyncBcastPost()\n",this);
          unsigned ncolors = Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::_numColors;
          //fprintf(stderr,"SyncBcastPost ncolors %d\n",ncolors);
          for (unsigned c = 0; c < ncolors; c++)
          {
            Executor::BroadcastExec<T_Conn> *exec = (Executor::BroadcastExec<T_Conn> *) Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::getExecutor(c);
            unsigned root = exec->getRoot();
            TRACE_FORMAT( "<%p> root %u, exec %p",this, root, exec);

            if (Executor::MultiColorCompositeT<NUMCOLORS, CCMI::Executor::Composite, CCMI::Executor::BroadcastExec<T_Conn>, T_Sched, T_Conn, pwcfn>::_native->endpoint() != root)
            {
              //fprintf(stderr, "<%p>BcastMultiColorCompositeT::SyncBcastPost() post receives for color %u, root %u\n", this,c,root);
              exec->setPostReceives();
              exec->postReceives();
            }
          }
          TRACE_FN_EXIT();
        }

      protected:
        PAMI_GEOMETRY_CLASS     * _geometry;
        unsigned                  _status;

        static const unsigned INTERNAL_BARRIER = 0;
        static const unsigned EXTERNAL_BARRIER = 1;
      };
    };
  };
};

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
