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
 * \file algorithms/executor/Broadcast.h
 * \brief ???
 */
#ifndef __algorithms_executor_Broadcast_h__
#define __algorithms_executor_Broadcast_h__


#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/connmgr/ConnectionManager.h"
#include "algorithms/interfaces/NativeInterface.h"

#include "util/ccmi_debug.h"
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
    /*
     * Implements a broadcast strategy which uses one network
     * link. With rectangular schedule it will lead to a one color
     * broadcast. Also implements pipelining.
     */
    template<class T, typename T_Coll_header = CollHeaderData, unsigned T_BCAST_MAX_PARALLEL=64>
    class BroadcastExec : public Interfaces::Executor
    {
    protected:
      Interfaces::Schedule           * _comm_schedule;
      Interfaces::NativeInterface    * _native;
      T                              * _connmgr;
      bool                             _postReceives;
      T_Coll_header                    _mdata;
      pami_multicast_t                 _msend;
      PAMI::PipeWorkQueue              _pwq;

      pami_endpoint_t        *_dst_eps;
      pami_endpoint_t        *_dst_eps_allocated;
      pami_endpoint_t         _dst_eps_storage[T_BCAST_MAX_PARALLEL];
      pami_endpoint_t         _src_eps;
      pami_endpoint_t         _self_ep;
      pami_endpoint_t         _root_ep;
      PAMI::Topology          _dsttopology;
      PAMI::Topology          _srctopology;
      PAMI::Topology          _selftopology;
      unsigned                _num_dst_eps;

      //Private method
      void             sendNext ();

    public:
      BroadcastExec () :
      Interfaces::Executor (),
      _comm_schedule(NULL),
      _dst_eps(_dst_eps_storage),
      _dst_eps_allocated(NULL),
      _num_dst_eps(T_BCAST_MAX_PARALLEL)
      //_comm(-1)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p>", this);
        TRACE_FN_EXIT();
      }
      ~BroadcastExec ()
      {
        if(_dst_eps_allocated) free(_dst_eps_allocated);
      }
      
      BroadcastExec (Interfaces::NativeInterface  * mf,
                     T                            * connmgr,
                     unsigned                       comm):
      Interfaces::Executor(),
      _comm_schedule (NULL),
      _native(mf),
      _connmgr(connmgr),
      _postReceives (false),
      _dst_eps(_dst_eps_storage),
      _dst_eps_allocated(NULL),
      _self_ep(mf->endpoint()),
      _dsttopology(),
      _selftopology(&_self_ep,1,PAMI::tag_eplist()),
      _num_dst_eps(T_BCAST_MAX_PARALLEL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p>", this);
        //_root              =  (unsigned) - 1;
        //_buflen            =  0;
        pami_quad_t *info   =  (pami_quad_t*)((void*) & _mdata);
        _msend.msginfo     =  info;
        _msend.msgcount    = sizeof(T_Coll_header)/sizeof(pami_quad_t);
        _msend.roles       = -1U;
        _msend.src_participants  = (pami_topology_t *) & _selftopology;
        _msend.dst_participants  = (pami_topology_t *) & _dsttopology;
        _msend.src               = (pami_pipeworkqueue_t *) & _pwq;
        _msend.dst               =   NULL;
        _mdata._comm       =  comm;
        _mdata._count = -1; // not used on broadcast
        _mdata._phase = 0;
        TRACE_FN_EXIT();
      }

      void setPostReceives ()
      {
        _postReceives    =  true;
        _msend.msginfo   =  NULL;
        _msend.msgcount  =  0; // only async exec needs metadata
      }

      //-----------------------------------------
      // --  Initialization routines
      //------------------------------------------

      void setSchedule (Interfaces::Schedule *ct, unsigned color)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p>", this);
        //_color = color;
        _comm_schedule = ct;
        int nph, phase;
        _comm_schedule->init (_mdata._root, BROADCAST_OP, phase, nph);
        CCMI_assert(_comm_schedule != NULL);
        // ENOMEM means there wasn't enough room in our ep list, so re-malloc and try again
        while(_comm_schedule->getDstUnionTopology (&_dsttopology, _dst_eps, _num_dst_eps) == PAMI_ENOMEM)
        {
          TRACE_FORMAT( "<%p>ENOMEM %u", this, _num_dst_eps);
          if(_dst_eps_allocated) free(_dst_eps_allocated);
          _num_dst_eps *=2;
          _dst_eps = _dst_eps_allocated = (pami_endpoint_t*)malloc(_num_dst_eps*sizeof(*_dst_eps));
          CCMI_assert(_dst_eps);
        }
        // Rectangle protocols do not use the _dst_eps list and will not return ENOMEM so the _dst_eps/_num_dst_eps may
        // safely be left 'too small' and this assert is not appropriate.   
        // CCMI_assert(_dsttopology.size() <= (size_t)_num_dst_eps);
        if(_connmgr)
          _msend.connection_id = _connmgr->getConnectionId(_mdata._comm, _mdata._root, color, (unsigned) - 1, (unsigned) - 1);

#if 0 // this was a sanity check using an ep array, we assume only one src
        pami_endpoint_t srcranks[64];
        _comm_schedule->getSrcUnionTopology (&_srctopology, srcranks);   
        CCMI_assert (_srctopology.size() <= 1);
#endif
        _comm_schedule->getSrcUnionTopology (&_srctopology, &_src_eps); 
        CCMI_assert (_srctopology.size() <= 1);
        TRACE_FN_EXIT();
      }

      void setConnectionID (unsigned cid)
      {
        //Override the connection id from the connection manager
        _msend.connection_id = cid;
      }

      void setRoot(unsigned root)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> root %u", this, root);
        _mdata._root = root;
        _root_ep     = root;
        TRACE_FN_EXIT();
      }

      void  setBuffers (char *src, char *dst, int bytes, int stride, TypeCode *stype, TypeCode *rtype)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT( "<%p> src %p, dst %p, len %d, _pwq %p", this, src, dst, bytes, &_pwq);
        _msend.bytes = bytes;

        //Setup pipework queue. This depends on setRoot so it better be correct
        size_t buflen = stride;

        if(_native->endpoint() == _mdata._root)
        {
          _pwq.configure (src, buflen, buflen, rtype, stype);
        }
        else
        {
          _pwq.configure (dst, buflen, 0, rtype, stype);
        }
        TRACE_FORMAT( "<%p> _pwq %p, bytes available %zu/%zu", this, &_pwq,
                       _pwq.bytesAvailableToConsume(), _pwq.bytesAvailableToProduce());
        TRACE_FN_EXIT();
      }

      void setCollHeader(const T_Coll_header &hdr)
      {
        _mdata = hdr;
      }

      //------------------------------------------
      // -- Executor Virtual Methods
      //------------------------------------------
      virtual void   start          ();
      virtual void   notifyRecv     (unsigned             src,
                                     const pami_quad_t   & info,
                                     PAMI::PipeWorkQueue ** pwq,
                                     pami_callback_t      * cb_done);

      //-----------------------------------------
      //--  Query functions ---------------------
      //-----------------------------------------
      unsigned       getRoot   ()
      {
        return _mdata._root;
      }
      unsigned       getComm   ()
      {
        return _mdata._comm;
      }

      void postReceives ()
      {
        TRACE_FN_ENTER();
        if(_native->endpoint() == _mdata._root) 
        {  
          TRACE_FN_EXIT();
          return;
        }

        pami_multicast_t mrecv;
        mrecv.msginfo  = _msend.msginfo;
        mrecv.msgcount = _msend.msgcount;
        mrecv.connection_id = _msend.connection_id;
	mrecv.roles         = -1U;

        TRACE_FORMAT( "<%p> ndest %zu, bytes %zu, rank %u, root %u",
                   this, _dsttopology.size(), _msend.bytes, _selftopology.index2Endpoint(0),_srctopology.index2Endpoint(0));
        mrecv.src_participants   = (pami_topology_t *) & _srctopology; 
        mrecv.dst_participants   = (pami_topology_t *) & _selftopology;

        if(_dsttopology.size() == 0)
        {
          mrecv.cb_done.function   = _cb_done;
          mrecv.cb_done.clientdata = _clientdata;
        }
        else
        {
          mrecv.cb_done.function   = NULL;
          mrecv.cb_done.clientdata = NULL;
        }

        mrecv.dst    =  (pami_pipeworkqueue_t *) & _pwq;
        mrecv.src    =  NULL;
        mrecv.bytes  = _msend.bytes;

        _native->multicast(&mrecv);
        TRACE_FN_EXIT();
      }
      static void notifyRecvDone( pami_context_t   context,
                                  void           * cookie,
                                  pami_result_t    result )
      {
        TRACE_FN_ENTER();
        TRACE_MSG ((stderr, "<%p>Executor::BroadcastExec::notifyRecvDone()\n", cookie));
        BroadcastExec<T, T_Coll_header> *exec =  (BroadcastExec<T, T_Coll_header> *) cookie;
        exec->sendNext();
        TRACE_FN_EXIT();
      }


    };  //-- BroadcastExec
  };   //-- Executor
};  //-- CCMI

///
/// \brief start sending broadcast data. Only active on the root node
///
template <class T, typename T_Coll_header, unsigned T_BCAST_MAX_PARALLEL>
inline void  CCMI::Executor::BroadcastExec<T, T_Coll_header,T_BCAST_MAX_PARALLEL>::start ()
{
  TRACE_FN_ENTER();
  TRACE_FORMAT( "<%p> count %zu", this, _msend.bytes);

  if(_native->endpoint() == _mdata._root || _postReceives)
  {
    sendNext ();
  }
  TRACE_FN_EXIT();
}

template <class T, typename T_Coll_header, unsigned T_BCAST_MAX_PARALLEL>
inline void  CCMI::Executor::BroadcastExec<T, T_Coll_header,T_BCAST_MAX_PARALLEL>::sendNext ()
{
  TRACE_FN_ENTER();
  //CCMI_assert (_dsttopology.size() != 0); //We have nothing to send
  if(_dsttopology.size() == 0)
  {
    TRACE_FORMAT( "<%p> bytes %zu, ndsts %zu bytes available to consume %zu",
               this, _msend.bytes, _dsttopology.size(), _pwq.bytesAvailableToConsume());
    //_cb_done(NULL, _clientdata, PAMI_SUCCESS);
    TRACE_FN_EXIT();
    return;
  }

#ifdef CCMI_DEBUG
  char tbuf[1024];
  char sbuf[16384];
  sprintf(sbuf, "<%p>Executor::BroadcastExec::sendNext() from %zu: bytes %zu, ndsts %zu bytes available to consume %zu ",
          this,__global.mapping.task(), _msend.bytes, _dsttopology.size(), _pwq.bytesAvailableToConsume());

  for(unsigned i = 0; i < _dsttopology.size(); ++i)
  {
    sprintf(tbuf, " me: 0x%x dstrank[%d] = %d ", _native->endpoint(),i, _dsttopology.index2Endpoint(i));
    strcat (sbuf, tbuf);
  }

  fprintf (stderr, " %s\n", sbuf);
#endif

  TRACE_FORMAT( "<%p> bytes %zu, ndsts %zu bytes available to consume %zu",
             this, _msend.bytes, _dsttopology.size(), _pwq.bytesAvailableToConsume());

  //Sending message header to setup receive of an async message
  //  _mdata._comm  = _comm;
  //_mdata._root  = _root;
  _msend.cb_done.function   = _cb_done;
  _msend.cb_done.clientdata = _clientdata;
  //_msend.bytes  = _msend.bytes;

  _native->multicast(&_msend);
  TRACE_FN_EXIT();
}

template <class T, typename T_Coll_header, unsigned T_BCAST_MAX_PARALLEL>
inline void  CCMI::Executor::BroadcastExec<T, T_Coll_header,T_BCAST_MAX_PARALLEL>::notifyRecv
(unsigned             src,
 const pami_quad_t   & info,
 PAMI::PipeWorkQueue ** pwq,
 pami_callback_t      * cb_done)
{
  TRACE_FN_ENTER();
  TRACE_FORMAT( "<%p> from %d, dsttopology.size %zu", this, src, _dsttopology.size());

  *pwq = &_pwq;

  if(_dsttopology.size() > 0)
  {
    TRACE_FORMAT( "<%p>  dsttopology.size %zu", this, _dsttopology.size());
    /// \todo this sendNext() should have worked but MPI platform didn't support it (yet).
    //    cb_done->function = NULL;  //There is a send here that will notify completion
    //    sendNext ();
    cb_done->function = notifyRecvDone;
    cb_done->clientdata = this;
  }
  else
  {
    TRACE_FORMAT( "<%p>  Nothing to send, receive completion indicates completion", this);
    cb_done->function   = _cb_done;
    cb_done->clientdata = _clientdata;
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
