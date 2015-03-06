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
 * \file algorithms/executor/Allgatherv.h
 * \brief ???
 */
#ifndef __algorithms_executor_Allgatherv_h__
#define __algorithms_executor_Allgatherv_h__


#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/connmgr/ConnectionManager.h"
#include "algorithms/interfaces/NativeInterface.h"

//#define MAX_CONCURRENT 32
//#define MAX_PARALLEL 20

// #define CONNECTION_ID_SHFT
#ifdef CONNECTION_ID_SHFT
#define SHFT_BITS_PHASE 10
#define SHFT_BITS_SRC   10
#define SHFT_BITS      (SHFT_BITS_PHASE+SHFT_BITS_SRC)
#endif

#if defined EXECUTOR_DEBUG
#undef EXECUTOR_DEBUG
#define EXECUTOR_DEBUG(x)  //fprintf x
#else
#define EXECUTOR_DEBUG(x)  //fprintf x
#endif

namespace CCMI
{
  namespace Executor
  {
    /*
     * Implements a allgatherv strategy which uses one network link.
     */

    template <class T_Allgather_type>
    struct AllgatherVecType
    {
      //COMPILE_TIME_ASSERT(0==1);
    };

    template<>
    struct AllgatherVecType<pami_allgather_t>
    {
      typedef int base_type;
    };

    template<>
    struct AllgatherVecType<pami_allgatherv_t>
    {
      typedef size_t base_type;
    };

    template<>
    struct AllgatherVecType<pami_allgatherv_int_t>
    {
      typedef int base_type;
    };

    template <class T_Allgather_type>
    inline void setAllgatherVec(T_Allgather_type *xfer, int *buflen, char **sbuf, char **rbuf, void *rdisps, void *rcounts, TypeCode **stype, TypeCode **rtype, unsigned myindex)
    {
// Clang performs syntax checks even for non-intantianated templates.
// This assertion fail needs to be deferred from syntax-checking to a possible instantianation attempt.
      COMPILE_TIME_ASSERT(sizeof(T_Allgather_type) == 0);
    }

    template <>
    inline void setAllgatherVec<pami_allgather_t> (pami_allgather_t *xfer,
                                                   int *bufcnt, char **sbuf, char **rbuf, void *rdisps, void *rcounts, TypeCode **stype, TypeCode **rtype, unsigned myindex)
    {
      (void)rdisps;(void)rcounts;
      TRACE_ADAPTOR((stderr, "Executor::AllgathervExec::setAllgatherVec(%p %p)\n", xfer->sndbuf, xfer->rcvbuf));
      *sbuf   = (xfer->sndbuf == PAMI_IN_PLACE)?
                 xfer->rcvbuf+myindex * xfer->rtypecount * ((TypeCode *)xfer->rtype)->GetExtent() : 
                 xfer->sndbuf;
      *rbuf   = xfer->rcvbuf;
      *stype  = (xfer->sndbuf == PAMI_IN_PLACE)?(TypeCode *)xfer->rtype:(TypeCode *)xfer->stype;
      *rtype  = (TypeCode *)xfer->rtype;
      *bufcnt = xfer->rtypecount;
      return;
    }

    template <>
    inline void setAllgatherVec<pami_allgatherv_t> (pami_allgatherv_t *xfer,
                                                    int *bufcnt, char **sbuf, char **rbuf, void *rdisps, void *rcounts, TypeCode **stype, TypeCode **rtype, unsigned myindex)
    {
      TRACE_ADAPTOR((stderr, "Executor::AllgathervExec::setAllgatherVec(%p %p %p %p) size_t\n",xfer->sndbuf, xfer->rcvbuf,xfer->rdispls,xfer->rtypecounts));
      *sbuf = (xfer->sndbuf == PAMI_IN_PLACE)?
               xfer->rcvbuf + xfer->rdispls[myindex] * 
                              ((TypeCode *)xfer->rtype)->GetExtent() :
               xfer->sndbuf;
      *rbuf = xfer->rcvbuf;
      *bufcnt = 0;
      *((size_t **)rdisps)   = xfer->rdispls;
      *((size_t **)rcounts)  = xfer->rtypecounts;
      *stype  = (xfer->sndbuf == PAMI_IN_PLACE)?(TypeCode *)xfer->rtype:(TypeCode *)xfer->stype;
      *rtype  = (TypeCode *)xfer->rtype;
      return;
    }

    template <>
    inline void setAllgatherVec<pami_allgatherv_int_t> (pami_allgatherv_int_t *xfer,
                                                        int *bufcnt, char **sbuf, char **rbuf, void *rdisps, void *rcounts, TypeCode **stype, TypeCode **rtype, unsigned myindex)
    {
      TRACE_ADAPTOR((stderr, "Executor::AllgathervExec::setAllgatherVec(%p %p %p %p) int\n", xfer->sndbuf, xfer->rcvbuf,xfer->rdispls,xfer->rtypecounts));
      *sbuf = (xfer->sndbuf == PAMI_IN_PLACE)?
               xfer->rcvbuf + xfer->rdispls[myindex] *
                              ((TypeCode *)xfer->rtype)->GetExtent() :
               xfer->sndbuf;
      *rbuf = xfer->rcvbuf;
      *bufcnt = 0;
      *((int **)rdisps)   = xfer->rdispls;
      *((int **)rcounts)  = xfer->rtypecounts;
      *stype  = (xfer->sndbuf == PAMI_IN_PLACE)?(TypeCode *)xfer->rtype:(TypeCode *)xfer->stype;
      *rtype  = (TypeCode *)xfer->rtype;
      return;
    }

    template<class T_ConnMgr, class T_Type>
    class AllgathervExec : public Interfaces::Executor
    {
      public:

      protected:
        Interfaces::Schedule           * _comm_schedule;
        Interfaces::NativeInterface    * _native;
        T_ConnMgr                      * _connmgr;

        int                 _comm;
        int                 _bufcnt;
        char                *_sbuf;
        char                *_rbuf;
        TypeCode            *_stype;
        TypeCode            *_rtype;

        PAMI::PipeWorkQueue _pwq;
        PAMI::PipeWorkQueue _rpwq;

        unsigned            _myindex;

        int                 _curphase;
        int                 _nphases;
        int                 _startphase;
        int                 _lphase;
        int                 _rphase;

        int                 _maxsrcs;

        int                 _senddone;
        int                 _recvdone;
        pami_task_t         _src;
        pami_task_t         _dst;
        pami_endpoint_t     _self_ep;

        PAMI::Topology      _dsttopology;
        PAMI::Topology      _srctopology;
        PAMI::Topology      _selftopology;
        PAMI::Topology      *_gtopology;

        unsigned            _lconnid;
        unsigned            _rconnid;

        CollHeaderData      _mldata;
        CollHeaderData      _mrdata;
        pami_multicast_t    _mlsend;
        pami_multicast_t    _mrsend;

        typedef typename AllgatherVecType<T_Type>::base_type basetype;

        basetype                *_disps;
        basetype                *_rcvcounts;

        int                  _in_send_next;

        //Private method
        void             sendNext ();

      public:
        AllgathervExec () :
            Interfaces::Executor (),
            _comm_schedule(NULL),
            _comm(-1),
            _sbuf(NULL),
            _rbuf(NULL),
            _curphase(-1),
            _nphases(0),
            _startphase(0),
            _disps(NULL),
            _rcvcounts(NULL)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec()\n", this));
        }

        AllgathervExec (Interfaces::NativeInterface  * mf,
                        T_ConnMgr                    * connmgr,
                        unsigned                       comm,
                        PAMI::Topology               *gtopology) :
            Interfaces::Executor(),
            _comm_schedule (NULL),
            _native(mf),
            _connmgr(connmgr),
            _comm(comm),
            _sbuf(NULL),
            _rbuf(NULL),
            _curphase(-1),
            _nphases(0),
            _startphase(0),
            _self_ep(mf->endpoint()),
            _dsttopology(),
            _srctopology(),
	    _selftopology(&_self_ep,1,PAMI::tag_eplist()),
            _gtopology(gtopology),
            _disps(NULL),
            _rcvcounts(NULL)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec(...)\n", this));
          _clientdata        =  0;
          _bufcnt            =  0;

          _in_send_next      = 0;

          _senddone  = _recvdone = 0;
          _lconnid   = _rconnid  = 0;

          _mldata._comm       = _comm;
          _mldata._root       = -1;
          _mldata._count      = -1; // indicating this is only a sync message
          _mldata._phase      = 0;

          pami_quad_t *info    =  (pami_quad_t*)((void*) & _mldata);
          _mlsend.msginfo       =  info;
          _mlsend.msgcount      =  1;
          _mlsend.roles         = -1U;

          _mrdata._comm       = _comm;
          _mrdata._root       = -1;
          _mrdata._count      = -1;
          _mrdata._phase      = 0;

          info                  =  (pami_quad_t*)((void*) & _mrdata);
          _mrsend.msginfo       =  info;
          _mrsend.msgcount      =  1;
          _mrsend.roles         = -1U;

        }

        virtual ~AllgathervExec ()
        {
        }

        /// NOTE: This is required to make "C" programs link successfully with virtual destructors
        void operator delete(void * p)
        {
        }

        // --  Initialization routines
        //------------------------------------------

        void setSchedule (Interfaces::Schedule *ct)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::setSchedule()\n", this));
          _comm_schedule = ct;

          _nphases    = _gtopology->size() - 1;
          _startphase = 0;
          _curphase   = -1;
          _lphase     = 0;
          _rphase     = -1;

          _myindex  = _gtopology->endpoint2Index(_native->endpoint());

          unsigned dstindex = (_myindex + 1) % _gtopology->size();
          _dst              = _gtopology->index2Endpoint(dstindex);
          new (&_dsttopology) PAMI::Topology(&_dst, 1, PAMI::tag_eplist());

          unsigned srcindex = (_myindex + _gtopology->size() - 1) % _gtopology->size();
          _src              = _gtopology->index2Endpoint(srcindex);
          new (&_srctopology) PAMI::Topology(&_src, 1, PAMI::tag_eplist());


          unsigned connection_id = (unsigned) - 1;

          if (_connmgr)
            connection_id = _connmgr->getConnectionId(_comm, (unsigned) - 1, 0, (unsigned) - 1, (unsigned) - 1);

#ifdef CONNECTION_ID_SHFT
          _lconnid = (connection_id << SHFT_BITS) + (_myindex << SHFT_BITS_SRC);
          _rconnid = (connection_id << SHFT_BITS) + (_myindex << SHFT_BITS_SRC);
#else
          _lconnid = connection_id;
          _rconnid = connection_id;
#endif
        }

        void setConnectionID (unsigned cid)
        {

          //Override the connection id from the connection manager
#ifdef CONNECTION_ID_SHFT
          _lconnid = (cid << SHFT_BITS) + (_myindex << SHFT_BITS_SRC);
          _rconnid = (cid << SHFT_BITS) + (_myindex << SHFT_BITS_SRC);
#else
          _lconnid = cid;
          _rconnid = cid;
#endif
        }

        void  updateBuffers(char *src, char *dst, int cnt)
        {
          _bufcnt = cnt;
          _sbuf   = src;
          _rbuf   = dst;
        }

        void  setBuffers (char *src, char *dst, int cnt)
        {
          // TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::setInfo() src %p, dst %p, len %d, _pwq %p\n", this, src, dst, len, &_pwq));

          _bufcnt = cnt;
          _sbuf   = src;
          _rbuf   = dst;

        }

        void setVectors(int *disps, int *rcvcounts)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::setVectors(int *disps %p, int *rcvcounts %p)\n", this, disps, rcvcounts));
          _disps     = disps;
          _rcvcounts = rcvcounts;
        }

        void setVectors(T_Type *xfer)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::setVectors\n",this));
          setAllgatherVec<T_Type> (xfer, &_bufcnt, &_sbuf, &_rbuf, &_disps, &_rcvcounts, &_stype, &_rtype, _myindex);
        }

        void  updateVectors(T_Type *xfer)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::updateVectors\n",this));
          setAllgatherVec<T_Type> (xfer, &_bufcnt, &_sbuf, &_rbuf, &_disps, &_rcvcounts, &_stype, &_rtype, _myindex);
        }


        size_t getSendLength(int phase)
        {
          int index = (_myindex + _gtopology->size() - phase) % _gtopology->size();
          return (_rcvcounts) ? (_rcvcounts[index] * _rtype->GetDataSize()) : _bufcnt * _rtype->GetDataSize();
        }

        size_t getRecvLength(int phase)
        {
          int index = (_myindex + _gtopology->size() -  phase - 1) % _gtopology->size();
          return (_rcvcounts) ? (_rcvcounts[index] * _rtype->GetDataSize()) : _bufcnt * _rtype->GetDataSize();
        }

        size_t getSendPWQLength(int phase)
        {
          int index = (_myindex + _gtopology->size() - phase) % _gtopology->size();
          return (_rcvcounts) ? (_rcvcounts[index] * _rtype->GetExtent()) : _bufcnt * _rtype->GetExtent();
        }

        size_t getRecvPWQLength(int phase)
        {
          int index = (_myindex + _gtopology->size() -  phase - 1) % _gtopology->size();
          return (_rcvcounts) ? (_rcvcounts[index] * _rtype->GetExtent()) : _bufcnt * _rtype->GetExtent();
        }

        size_t getSendDisp(int phase)
        {
          int index = (_myindex + _gtopology->size() - phase) % _gtopology->size();
          return (_disps) ? _disps[index] * _rtype->GetExtent() : index * _bufcnt * _rtype->GetExtent();
        }

        size_t getRecvDisp(int phase)
        {
          int index = (_myindex + _gtopology->size() -  phase - 1) % _gtopology->size();
          return (_disps) ? _disps[index] * _rtype->GetExtent() : index * _bufcnt * _rtype->GetExtent();
        }

        PAMI::PipeWorkQueue *getSendPWQ(int phase)
        {
          size_t spwqleng = getSendPWQLength(phase);
          size_t sdisp    = getSendDisp(phase);
          _pwq.configure (_rbuf + sdisp, spwqleng, spwqleng, NULL, _rtype);
          return &_pwq;
        }

        PAMI::PipeWorkQueue *getRecvPWQ(int phase)
        {
          size_t rpwqleng = getRecvPWQLength(phase);
          size_t rdisp    = getRecvDisp(phase);
          _rpwq.configure (_rbuf + rdisp, rpwqleng, 0, _rtype);
          return &_rpwq;
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
        /*
        unsigned       getRoot   ()
        {
          return _root;
        }
        */
        unsigned       getComm   ()
        {
          return _comm;
        }

        static void notifySendDone (pami_context_t context, void *cookie, pami_result_t result)
        {
          TRACE_MSG ((stderr, "<%p>Executor::AllgathervExec::notifySendDone()\n", cookie));
          AllgathervExec<T_ConnMgr, T_Type> *exec =  (AllgathervExec<T_ConnMgr, T_Type> *) cookie;
          EXECUTOR_DEBUG((stderr, "notifySendDone, phase %d, recvdone %d\n", exec->_curphase, exec->_recvdone);)
          exec->_senddone = 1;

          if (exec->_recvdone == 1)
            {
              exec->_recvdone = exec->_senddone = 0;
              exec->_curphase ++;
              EXECUTOR_DEBUG((stderr, "notify send done - calling sendNext\n");)
              exec->sendNext();
            }
        }

        static void notifyRecvDone( pami_context_t   context,
                                    void           * cookie,
                                    pami_result_t    result )
        {
          TRACE_MSG ((stderr, "<%p>Executor::AllgathervExec::notifyRecvDone()\n", cookie));
          AllgathervExec<T_ConnMgr, T_Type> *exec =  (AllgathervExec<T_ConnMgr, T_Type> *) cookie;
          EXECUTOR_DEBUG((stderr, "notifyRecvDone, phase %d, senddone %d\n", exec->_curphase, exec->_senddone);)
          exec->_recvdone = 1;

          if (exec->_senddone == 1)
            {
              exec->_recvdone = exec->_senddone = 0;
              exec->_curphase ++;
              EXECUTOR_DEBUG((stderr, "notify recv done - calling sendNext\n");)
              exec->sendNext();
            }
        }

        static void notifyAvailRecvDone( pami_context_t   context,
                                         void           * cookie,
                                         pami_result_t    result )
        {
          TRACE_MSG ((stderr, "<%p>Executor::AllgathervExec::notifyRecvDone()\n", cookie));
          AllgathervExec<T_ConnMgr, T_Type> *exec =  (AllgathervExec<T_ConnMgr, T_Type> *) cookie;
          EXECUTOR_DEBUG((stderr, "notifyAvailRecvDone, phase %d, rphase%d\n", exec->_curphase, exec->_rphase);)
          exec->_rphase ++;

          if (exec->_curphase >= exec->_startphase && !exec->_in_send_next)
            {
              EXECUTOR_DEBUG((stderr, "notifyAvail - calling sendNext();\n");)
              exec->sendNext();
            }
        }


    };  //-- AllgathervExec
  };   //-- Executor
};  //-- CCMI

///
/// \brief start sending allgatherv data. Only active on the root node
///
template <class T_ConnMgr, class T_Type>
inline void  CCMI::Executor::AllgathervExec<T_ConnMgr, T_Type>::start ()
{
  TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::start() count %d\n", this, _bufcnt));

  _curphase  = _startphase;

  if (_rphase == -1)
    _rphase = _startphase;
  else
    _rphase ++;

  if(isTypeSame<T_Type, pami_allgather_t>::result)
    {
      // We are in Allgather
      // Nothing to gather? Invoke the callback and return
      if ((_bufcnt == 0) && _cb_done)
        {
          _cb_done (NULL, _clientdata, PAMI_SUCCESS);
          return;
        }
      TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::start() _rbuf %p,_sbuf %p,_buflen %d\n", this,_rbuf,_sbuf,_bufcnt));
      PAMI_Type_transform_data((void*)_sbuf,
                          _stype, 0,
                          _rbuf + _myindex * _bufcnt * _rtype->GetExtent(),
                          _rtype, 0,
                          _bufcnt * _rtype->GetDataSize(),
                          PAMI_DATA_COPY, NULL);
    }
  else
    {
      TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::start()_rbuf %p,_disps %p, _sbuf %p, _rcvcounts %p\n", this,_rbuf,_disps,_sbuf, _rcvcounts));
      TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::start()_rbuf %p,_disps[%zu] %zu, _rbuf + _disps[_myindex] %p, _sbuf %p, _rcvcounts[_myindex] %zu\n", this,_rbuf,(size_t)_myindex, (size_t)_disps[_myindex], _rbuf + _disps[_myindex], _sbuf, (size_t)_rcvcounts[_myindex]));
      PAMI_Type_transform_data((void*)_sbuf,
                          _stype, 0,
                          _rbuf + _disps[_myindex] * _rtype->GetExtent(),
                          _rtype, 0,
                          _rcvcounts[_myindex] * _rtype->GetDataSize(),
                          PAMI_DATA_COPY, NULL);
    }

  sendNext ();
}

template <class T_ConnMgr, class T_Type>
inline void  CCMI::Executor::AllgathervExec<T_ConnMgr, T_Type>::sendNext ()
{
  TRACE_ADAPTOR((stderr, "<%p>Executor::AllgathervExec::sendNext() _curphase %d,_startphase %d,_nphases %d\n", this, _curphase,_startphase,_nphases));
  if (_curphase == _startphase + _nphases)
    {
      if (_cb_done) _cb_done (NULL, _clientdata, PAMI_SUCCESS);

      return;
    }

  _in_send_next = 1;

  // send buffer available msg to left neighbor
  if (_lphase == _curphase)
    {
      _lphase ++;
      _mldata._phase             = _curphase + 1;
      _mrdata._count             = -1;
      _mlsend.src_participants   = (pami_topology_t *) & _selftopology;
      _mlsend.dst_participants   = (pami_topology_t *) & _srctopology;
      _mlsend.cb_done.function   = NULL;
      _mlsend.cb_done.clientdata = 0;
      _mlsend.src                = NULL;
      _mlsend.dst                = NULL;
      _mlsend.bytes              = 0;
#ifdef CONNECTION_ID_SHFT
      _mlsend.connection_id      = _lconnid + 2 * _curphase + 1;
#else
      _mlsend.connection_id      = _lconnid;
#endif
      EXECUTOR_DEBUG((stderr, "key %d, phase %d, send to %d\n", _mlsend.connection_id, _curphase, _srctopology.index2Endpoint(0));)
      _native->multicast(&_mlsend);
    }

  if (_rphase == _curphase + 1) // buffer available at the right neighbor
    {
      _mrdata._phase             = _curphase;
      _mrdata._count             = 0; // indicating this is data message
      _mrsend.src_participants   = (pami_topology_t *) & _selftopology;
      _mrsend.dst_participants   = (pami_topology_t *) & _dsttopology;
      _mrsend.cb_done.function   = notifySendDone;
      _mrsend.cb_done.clientdata = this;
      _mrsend.src                = (pami_pipeworkqueue_t *) getSendPWQ(_curphase);
      _mrsend.dst                = NULL;
      _mrsend.bytes              = getSendLength(_curphase);
#ifdef CONNECTION_ID_SHFT
      _mrsend.connection_id      = _rconnid + 2 * _curphase;
#else
      _mrsend.connection_id      = _rconnid;
#endif
      EXECUTOR_DEBUG((stderr, "key %d, data phase %d, send to %d\n", _mrsend.connection_id, _curphase, _dsttopology.index2Endpoint(0));)
      _native->multicast(&_mrsend);
    }

  _in_send_next = 0;

  return;
}

template <class T_ConnMgr, class T_Type>
inline void  CCMI::Executor::AllgathervExec<T_ConnMgr, T_Type>::notifyRecv
(unsigned             src,
 const pami_quad_t   & info,
 PAMI::PipeWorkQueue ** pwq,
 pami_callback_t      * cb_done)
{

  CollHeaderData *cdata = (CollHeaderData*) & info;

  if ((int)cdata->_count == -1) {
    CCMI_assert(src == _dst);
    if (_rphase == _curphase && _curphase >= _startphase) {
      CCMI_assert(cdata->_phase == (unsigned)_curphase+1);
    } else if (_rphase == _curphase+1 || _curphase < _startphase) {
      CCMI_assert(cdata->_phase == (unsigned)_curphase+2);
    } else {
      CCMI_assert(0);
    }
    *pwq = NULL;
    cb_done->function   = notifyAvailRecvDone;
    cb_done->clientdata = this;
  } else {
    CCMI_assert(src == _src);
    CCMI_assert(cdata->_phase == (unsigned)_curphase);
    CCMI_assert(cdata->_count == 0);
    *pwq = getRecvPWQ(_curphase);
    cb_done->function   = notifyRecvDone;
    cb_done->clientdata = this;
  }

  return;
}

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
