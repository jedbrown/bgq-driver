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
 * \file algorithms/executor/Barrier.h
 * \brief ???
 */

#ifndef __algorithms_executor_Barrier_h__
#define __algorithms_executor_Barrier_h__

#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/interfaces/NativeInterface.h"

#include "algorithms/executor/ScheduleCache.h"

#define CCMI_BARRIER_MAXPHASES  24
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
  namespace Executor
  {
    class BarrierExec : public Interfaces::Executor
    {
      public:
        unsigned             _iteration: 1; /// The Red or black iteration
        unsigned             _phase;     /// Which phase am I in ?
        bool                 _senddone;  /// has send finished or not?
        unsigned             _start;     /// Start phase (don't assume 0)
        unsigned             _nphases;   /// Number of phases

        /// pointer to the multicast interface to send messages
        Interfaces::NativeInterface    * _native;

        ///\brief A red/black vector for each neigbor which is incremented
        ///when the neighbor's message arrives
        char                   _phasevec[CCMI_BARRIER_MAXPHASES][2];	

        ExtCollHeaderData      _cdata;  /// Info passed as meta data
        pami_multicast_t       _minfo;  /// The multicast info structure	

        ///\brief A cache of the barrier schedule
        ScheduleCache          _cache;

        ///\brief The self topology
        PAMI::Topology         _srctopology;

        pami_context_t         _context; //Context id of the collective 

        ///
        /// \brief core internal function to initiate the next phase
        ///
        void             sendNext();

        ///
        /// \brief : Internal function to reset entries in the
        /// phase vector table. It is called at the start of each
        /// phase.
        ///
        void resetVector()
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>phase %u, start %u, nphases %u",this, _phase, _start, _nphases);
          CCMI_assert(_phase == _start + _nphases);
          _phase     =   _start;
          _iteration ++;  //1 bit itertation count that can wrap

          for (unsigned count = _start; count < (_start + _nphases); count ++)
            {
              _phasevec[count][_iteration] += _cache.getSrcTopology(count)->size();

              TRACE_FORMAT("<%p>phase %d, nranks %zu, vec %d",
                          this, count, _cache.getSrcTopology(count)->size(),  _phasevec[count][_iteration]);
            }

          _senddone  =   false;
          TRACE_FN_EXIT();
        }

        /// Static function to be passed into the done of multisend
        static void staticNotifySendDone(pami_context_t context, void *cd, pami_result_t err)
        {
          (void)context;(void)err;
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>", cd);

          BarrierExec *barrier = (BarrierExec *) cd;
          barrier->internalNotifySendDone();
          TRACE_FN_EXIT();
        }

      public:

        /// Default Constructor
        BarrierExec() : Interfaces::Executor()
        {
          _start    = 0;
          _phase    = 0;
          _nphases  = 0;
        }

        /// Main constructor to initialize the executor
        BarrierExec(unsigned comm,
                    unsigned connid,
                    Interfaces::NativeInterface *ninterface):
            Interfaces::Executor(),
            _native(ninterface),
            _srctopology(ninterface->endpoint()),
          _context (NULL)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>(comm %X,connid %d)",
                      this, comm, connid);
          _start          =  0;
          _phase          =  0;
          _nphases        =  0;
          _senddone       =  false;
          _cdata._phase   = 0;
          _cdata._comm    = comm;
          MEMSET(_phasevec, 0, sizeof(_phasevec));

          _minfo.msginfo       = (pami_quad_t *)(void *) & _cdata;
          _minfo.msgcount      = 1;
          _minfo.src           = NULL;
          _minfo.dst           = NULL;
          _minfo.bytes         = 0;
          _minfo.roles         = -1U;
          _minfo.dst_participants  = NULL;
          _minfo.src_participants  = (pami_topology_t *) & _srctopology;
          _minfo.connection_id     = connid;
          _iteration           = 0;
          TRACE_FN_EXIT();
        }

        void setCommSchedule(Interfaces::Schedule *sch)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>",this);
          //_cache.init (sch);
          _cache.setSchedule (sch);
          _cache.setRoot (-1);
          _cache.init (BARRIER_OP);

          _start     =   _cache.getStartPhase();
          _nphases   =   _cache.getNumPhases();
          _phase     =   _start + _nphases;    //so that the resetVector assert passes
          TRACE_FORMAT("<%p>_start %u + _nphases %u  <=  CCMI_BARRIER_MAXPHASES %u",this,_start,_nphases,CCMI_BARRIER_MAXPHASES);
          CCMI_assert(_start + _nphases  <=  CCMI_BARRIER_MAXPHASES);
          TRACE_FN_EXIT();
        }

        void setContext (pami_context_t context) { _context = context; }

        /**
         * \brief notify when a message has been recived
         * \param src : source of the message
         * \param buf : address of the pipeworkqueue to produce incoming message
         * \param cb_done: completion callback
         */
        virtual void   notifyRecv     (unsigned             src,
                                       const pami_quad_t   & info,
                                       PAMI::PipeWorkQueue ** pwq,
                                       pami_callback_t      * cb_done);


        void internalNotifySendDone();

        /// Start sending barrier operation
        virtual void start();

    };  //-- BarrierExec
  };
};  // namespace CCMI::Executor

inline void CCMI::Executor::BarrierExec::sendNext()
{
  TRACE_FN_ENTER();
  CCMI_assert(_phase <= (_start + _nphases));
  TRACE_FORMAT("<%p> _phase %d, _start %d, _nphases %d",
              this, _phase, _start, _nphases);

  if (_phase == (_start + _nphases))
    {
      TRACE_FORMAT("<%p>DONE _cb_done %p, _phase %d, _clientdata %p",
                  this, _cb_done, _phase, _clientdata);

      if (_cb_done) _cb_done(_context, _clientdata, PAMI_SUCCESS);

      _senddone = false;

      TRACE_FN_EXIT();
      return;
    }

  _senddone = false;
  PAMI::Topology *topology = _cache.getDstTopology(_phase);
  int ndest = topology->size();
  _minfo.dst_participants = (pami_topology_t *)topology;

  ///We can now send any number of messages in barrier
  if (ndest > 0)
    {
#ifdef CCMI_DEBUG
      TRACE_FORMAT("<%p>_phase %d, ndest %zd,connid %d, _clientdata %p", this, _phase, topology->size(), _minfo.connection_id, _clientdata);
      size_t count;
      for (count = 0; count < topology->size(); count++)
        TRACE_FORMAT("<%p>_dstranks[%zu] %u/%u", this, count, topology->index2Endpoint(count),topology->index2Rank(count));

#endif
      _minfo.connection_id = _phase; //set connection id to phase
      _cdata._phase     = _phase;
      _cdata._iteration = _iteration;  //Send the last bit of iteration

      //if last receive has arrived before the last send dont call executor notifySendDone rather app done callback
      if ( (_phase == (_start + _nphases - 1)) && (_phasevec[_phase][_iteration] <= 0) )
        {
          TRACE_FORMAT("<%p>set callback %p", this, _cb_done);
          _minfo.cb_done.function   = _cb_done;
          _minfo.cb_done.clientdata = _clientdata;
          _phase ++;
        }
      else
        {
          _minfo.cb_done.function   = staticNotifySendDone;
          _minfo.cb_done.clientdata = this;
        }

      ///Initiate multisend
      _native->multicast(&_minfo);
    }
  else
    {
      //nothing to do, skip this phase
      internalNotifySendDone();
    }
  TRACE_FN_EXIT();
}

/// Entry function called to start the barrier
inline void  CCMI::Executor::BarrierExec::start()
{
  TRACE_FN_ENTER();
  TRACE_FORMAT("<%p>", this);
  resetVector();
  sendNext();
  TRACE_FN_EXIT();
}


/**
 * \brief notify when a message has been recived
 * \param src : source of the message
 * \param buf : address of the pipeworkqueue to produce incoming message
 * \param cb_done: completion callback
 */
inline void  CCMI::Executor::BarrierExec::notifyRecv  (unsigned             src,
                                                       const pami_quad_t   & info,
                                                       PAMI::PipeWorkQueue ** pwq,
                                                       pami_callback_t      * cb_done)
{
  (void)src;(void)pwq;(void)cb_done;
  TRACE_FN_ENTER();
  ExtCollHeaderData *hdr = (ExtCollHeaderData *) (& info);
  CCMI_assert (hdr->_iteration <= 1);
  //Process this message by incrementing the phase vec
  _phasevec[hdr->_phase][hdr->_iteration] --;
  //fprintf(stderr,"CCMI RECEIVED from %d\n", src);
  TRACE_FORMAT("<%p>phase %d/%d(%d,%d), vec %d expected vec %zu", this,
              hdr->_phase, _phase, _start, _nphases, _phasevec[hdr->_phase][hdr->_iteration],  _cache.getSrcTopology(hdr->_phase)->size());

  //Start has not been called, just record recv and return
  if (unlikely(_phase == _start + _nphases))
    {
      TRACE_FN_EXIT();
      return;
    }

  ///Check the current iteration's number of messages received. Have we received all messages
  if ( (_phasevec[_phase][_iteration] <= 0) && _senddone )
    {
      _phase ++;
      sendNext();
    }
  TRACE_FN_EXIT();
}


///
/// \brief Entry function to indicate the send has finished
///
inline void CCMI::Executor::BarrierExec::internalNotifySendDone( )
{
  TRACE_FN_ENTER();
  TRACE_FORMAT("<%p>phase %d, vec %d", this, _phase, _phasevec[_phase][_iteration]);

  _senddone = true;

  //Message for that phase has been received
  if (_phasevec[_phase][_iteration] <= 0)
    {
      _phase ++;
      sendNext();
    }
  TRACE_FN_EXIT();
}

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
