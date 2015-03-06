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
 * \file algorithms/executor/Scatter.h
 * \brief ???
 */
#ifndef __algorithms_executor_Scatter_h__
#define __algorithms_executor_Scatter_h__

#include "Global.h"
#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/connmgr/ConnectionManager.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "common/default/Topology.h"

#define MAX_CONCURRENT 32
//#define MAX_PARALLEL 20

namespace CCMI
{
  namespace Executor
  {
    /*
     * Implements a scatter strategy which uses one network link.
     */

    template <class T_Scatter_type>
    struct ScatterVecType
    {
      // COMPILE_TIME_ASSERT(0==1);
    };

    template<>
    struct ScatterVecType<pami_scatter_t>
    {
      typedef int base_type;
    };

    template<>
    struct ScatterVecType<pami_scatterv_t>
    {
      typedef size_t base_type;
    };

    template<>
    struct ScatterVecType<pami_scatterv_int_t>
    {
      typedef int base_type;
    };

    template <class T_Scatter_type>
    extern inline void setScatterVectors(T_Scatter_type *xfer, void *disps, void *sndcounts)
    {
    }

    template<>
    inline void setScatterVectors<pami_scatter_t> (pami_scatter_t *xfer, void *disps, void * sndcounts)
    {
      (void)xfer;(void)disps;(void)sndcounts;
    }

    template<>
    inline void setScatterVectors<pami_scatterv_t> (pami_scatterv_t *xfer, void *disps, void * sndcounts)
    {
      *((size_t **)disps)     = xfer->sdispls;
      *((size_t **)sndcounts) = xfer->stypecounts;
    }

    template<>
    inline void setScatterVectors<pami_scatterv_int_t> (pami_scatterv_int_t *xfer, void *disps, void * sndcounts)
    {
      *((int **)disps)     = xfer->sdispls;
      *((int **)sndcounts) = xfer->stypecounts;
    }

    template<class T_ConnMgr, class T_Schedule, typename T_Scatter_type, typename T_Coll_header = CollHeaderData>
    class ScatterExec : public Interfaces::Executor
    {
      public:

        struct SendStruct
        {
          pami_multicast_t    msend;
          pami_endpoint_t     ep;
          PAMI::PipeWorkQueue pwq;
          PAMI::Topology      dsttopology;
        } ;

      protected:
        T_Schedule                     * _comm_schedule;
        Interfaces::NativeInterface    * _native;
        T_ConnMgr                      * _connmgr;

        int                 _comm;
        pami_endpoint_t     _root;

        unsigned            _rootindex;
        unsigned            _myindex;

        int                 _sbufcnt;
        int                 _rbufcnt;
        int                 _tmpbufIsContig;
        char                *_sbuf;
        char                *_rbuf;
        char                *_tmpbuf;
        TypeCode            *_stype;
        TypeCode            *_rtype;

        PAMI::PipeWorkQueue _pwq;

        int                 _curphase;
        int                 _nphases;
        int                 _startphase;
        int                 _donecount;

        int                 _maxdsts;
        pami_task_t         _dstranks [MAX_CONCURRENT];
        unsigned            _dstlens  [MAX_CONCURRENT];
        pami_endpoint_t     _self_ep;
        PAMI::Topology      _selftopology;
        PAMI::Topology      *_gtopology;

        T_Coll_header       _mdata;
        SendStruct          *_msendstr;

        typedef typename ScatterVecType<T_Scatter_type>::base_type basetype;

        basetype            *_disps;
        basetype            *_sndcounts;

        //Private method
        void             sendNext ();

      public:
        ScatterExec () :
            Interfaces::Executor (),
            _comm_schedule(NULL),
            _comm(-1),
            _tmpbufIsContig(1),
            _sbuf(NULL),
            _rbuf(NULL),
            _tmpbuf(NULL),
            _stype(NULL),
            _rtype(NULL),
            _curphase(0),
            _nphases(0),
            _startphase(0),
            _disps(NULL),
            _sndcounts(NULL)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::ScatterExec()\n", this));
        }

        ScatterExec (Interfaces::NativeInterface  * mf,
                     T_ConnMgr                    * connmgr,
                     unsigned                       comm,
                     PAMI::Topology               *gtopology) :
            Interfaces::Executor(),
            _comm_schedule (NULL),
            _native(mf),
            _connmgr(connmgr),
            _comm(comm),
            _tmpbufIsContig(1),
            _sbuf(NULL),
            _rbuf(NULL),
            _tmpbuf(NULL),
            _stype(NULL),
            _rtype(NULL),
            _curphase(0),
            _nphases(0),
            _startphase(0),
            _self_ep(mf->endpoint()),
            _selftopology(&_self_ep, 1,PAMI::tag_eplist()),
            _gtopology(gtopology),
            _msendstr(NULL),
            _disps(NULL),
            _sndcounts(NULL)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::ScatterExec(...)\n", this));
          _clientdata         =  0;
          _root               =  (unsigned) - 1;
          _sbufcnt = _rbufcnt =  0;

          _donecount          =   0;

          _mdata._comm        = _comm;
          _mdata._root        = _root;
          _mdata._count       = -1;
          _mdata._phase       = 0;

        }

        virtual ~ScatterExec ()
        {
           /// Todo: convert this to allocator ?
           if (_maxdsts) __global.heap_mm->free(_msendstr);
           if (_native->endpoint() != _root || (_root != 0 && _gtopology->size() != (unsigned)_nphases+1)) __global.heap_mm->free(_tmpbuf);
        }

        /// NOTE: This is required to make "C" programs link successfully with virtual destructors
        void operator delete(void * p)
        {
        }

        // --  Initialization routines
        //------------------------------------------

        unsigned  getMyIndex() {return _myindex;}

        void setSchedule (T_Schedule *ct)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::ScatterExec::setSchedule()\n", this));
          _comm_schedule = ct;
          _comm_schedule->init (_root, CCMI::Schedule::SCATTER, _startphase, _nphases, _maxdsts);
          CCMI_assert(_maxdsts <= MAX_CONCURRENT);
          // overwrite _nphase since we only care about my own number of phases
          _nphases    = _comm_schedule->getMyNumPhases();
          _myindex    = _gtopology->endpoint2Index(_native->endpoint());
          _rootindex  = _gtopology->endpoint2Index(_root);

          unsigned connection_id = (unsigned) - 1;
          if (_connmgr)
            connection_id = _connmgr->getConnectionId(_comm, _root, 0, (unsigned) - 1, (unsigned) - 1);

          _msendstr = NULL;
          if (_maxdsts)
          {
            pami_result_t rc;
            rc = __global.heap_mm->memalign((void **)&_msendstr, 0,
            						_maxdsts * sizeof(SendStruct));
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _msendstr");
            pami_quad_t *info      =  (pami_quad_t*)((void*) & _mdata);
            for (int i = 0; i <_maxdsts; ++i)
            {
              _msendstr[i].msend.msginfo       =  info;
              _msendstr[i].msend.msgcount      =  sizeof(T_Coll_header)/sizeof(pami_quad_t);
              _msendstr[i].msend.roles         = -1U;
              _msendstr[i].msend.connection_id = connection_id;
            }
          }
        }

        void setConnectionID (unsigned cid)
        {

          CCMI_assert(_comm_schedule != NULL);

          //Override the connection id from the connection manager
          for (int i = 0; i < _maxdsts; ++i) _msendstr[i].msend.connection_id = cid;

        }

        void setRoot(unsigned root)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::ScatterExec::setRoot()\n", this));
          _root        = root;
          _mdata._root = root;
        }

        void  setBuffers (char *src, char *dst, int scount, int rcount, TypeCode *stype, TypeCode *rtype)
        {
          _sbufcnt = scount;
          _rbufcnt = rcount;
          _sbuf    = src;
          _rbuf    = (dst==PAMI_IN_PLACE && 
                     _native->endpoint() == _root)?
                     src + (_disps == NULL?_myindex * scount * stype->GetExtent():
                            _disps[_myindex] * stype->GetExtent()) : dst;
          _stype   = stype;
          _rtype   = (dst==PAMI_IN_PLACE &&
                     _native->endpoint() == _root)?
                     stype:rtype;

          // ship data length info in the header for async protocols
          _mdata._count = _native->endpoint() == _root ? scount * stype->GetDataSize():
                                                         rcount * rtype->GetDataSize();

          CCMI_assert(_comm_schedule != NULL);

          // setup PWQ
          if (_native->endpoint() == _root)
          {
            if ((unsigned)_nphases == _gtopology->size() - 1 || _root == 0)
            {
              _tmpbuf = src;
              _tmpbufIsContig = 0;
            }
            else  // allocate temporary buffer and reshuffle the data
            {
               size_t buflen = _gtopology->size() * scount * stype->GetDataSize();
               pami_result_t rc;
	           rc = __global.heap_mm->memalign((void **)&_tmpbuf, 0, buflen);
               PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _tmpbuf");
               PAMI_Type_transform_data((void*)(src + _myindex * scount * stype->GetExtent()),
                          _stype, 0,
                          _tmpbuf,
                          PAMI_TYPE_BYTE, 0,
                          (_gtopology->size() - _myindex) * scount * stype->GetDataSize(),
                          PAMI_DATA_COPY, NULL);
               PAMI_Type_transform_data((void*)src,
                          _stype, 0,
                          _tmpbuf+(_gtopology->size() - _myindex) * scount * stype->GetDataSize(),
                          PAMI_TYPE_BYTE, 0,
                          _myindex * scount * stype->GetDataSize(),
                          PAMI_DATA_COPY, NULL);
            }
          }
          else if (_nphases > 1)
          {
            // schedule's getLList() method can be used for an accurate buffer size
            size_t  buflen = _gtopology->size() * rcount * rtype->GetDataSize();
            pami_result_t rc;
            rc = __global.heap_mm->memalign((void **)&_tmpbuf, 0, buflen);
            PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _tmpbuf");
            _pwq.configure (_tmpbuf, buflen, 0);
          }
          else
            {
              _pwq.configure (dst, rcount * rtype->GetExtent(), 0, _rtype);
            }
        }

        void setVectors(T_Scatter_type *xfer)
        {

          if (_native->endpoint() == _root)
            {
              setScatterVectors<T_Scatter_type>(xfer, (void *)&_disps, (void *)&_sndcounts);
            }
        }

        void setConnmgr(T_ConnMgr *connmgr)
        {
          _connmgr = connmgr;
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
          return _root;
        }
        unsigned       getComm   ()
        {
          return _comm;
        }

        static void notifySendDone (pami_context_t context, void *cookie, pami_result_t result)
        {
          TRACE_MSG ((stderr, "<%p>Executor::ScatterExec::notifySendDone()\n", cookie));
          ScatterExec<T_ConnMgr, T_Schedule, T_Scatter_type, T_Coll_header> *exec =  (ScatterExec<T_ConnMgr, T_Schedule, T_Scatter_type, T_Coll_header> *) cookie;
          exec->_donecount --;
          exec->_curphase  ++;
          exec->sendNext();
        }

        static void notifyRecvDone( pami_context_t   context,
                                    void           * cookie,
                                    pami_result_t    result )
        {
          TRACE_MSG ((stderr, "<%p>Executor::ScatterExec::notifyRecvDone()\n", cookie));
          ScatterExec<T_ConnMgr, T_Schedule, T_Scatter_type, T_Coll_header> *exec =  (ScatterExec<T_ConnMgr, T_Schedule, T_Scatter_type, T_Coll_header> *) cookie;
          exec->_curphase      =  exec->_startphase + 1;
          exec->sendNext();
        }


    };  //-- ScatterExec
  };   //-- Executor
};  //-- CCMI

///
/// \brief start sending scatter data. Only active on the root node
///
template <class T_ConnMgr, class T_Schedule, typename T_Scatter_type, typename T_Coll_header>
inline void  CCMI::Executor::ScatterExec<T_ConnMgr, T_Schedule, T_Scatter_type, T_Coll_header>::start ()
{
  TRACE_ADAPTOR((stderr, "<%p>Executor::ScatterExec::start() count%d\n", this, _sbufcnt));

  _curphase  = 0;

  if (_native->endpoint() == _root)
    {
      sendNext ();
    }
}

template <class T_ConnMgr, class T_Schedule, typename T_Scatter_type, typename T_Coll_header>
inline void  CCMI::Executor::ScatterExec<T_ConnMgr, T_Schedule, T_Scatter_type, T_Coll_header>::sendNext ()
{
  unsigned ndst = 0;
  unsigned size = _gtopology->size();

  CCMI_assert(_comm_schedule != NULL);
  CCMI_assert(_curphase >= _startphase);

  if (_curphase == _startphase + _nphases)
    {
      // all parents copy from send buffer to application destination buffer
      if (_disps && _sndcounts)
        PAMI_Type_transform_data((void*)(_sbuf + _disps[_myindex] * _stype->GetExtent()),
                     _stype, 0,
                     _rbuf,
                     _rtype, 0,
                     _sbufcnt * _stype->GetDataSize(),
                     PAMI_DATA_COPY, NULL);
      else if (_native->endpoint() == _root)
        PAMI_Type_transform_data((void*)(_sbuf + _myindex * _sbufcnt * _stype->GetExtent()),
                     _stype, 0,
                     _rbuf,
                     _rtype, 0,
                     _sbufcnt * _stype->GetDataSize(),
                     PAMI_DATA_COPY, NULL);
      else if (_nphases > 1)
        PAMI_Type_transform_data((void*)(_tmpbuf),
                     PAMI_TYPE_BYTE, 0,
                     _rbuf,
                     _rtype, 0,
                     _rbufcnt * _rtype->GetDataSize(),
                     PAMI_DATA_COPY, NULL);

      if (_cb_done) _cb_done (NULL, _clientdata, PAMI_SUCCESS);

      return;
    }
  _comm_schedule->getRList(_curphase, &_dstranks[0], ndst, &_dstlens[0]);

  CCMI_assert(_donecount  == 0);
  _donecount = ndst;

  for (unsigned i = 0; i < ndst; ++i)
    {

      SendStruct *sendstr = &(_msendstr[i]);
      pami_multicast_t *msend = &sendstr->msend;
      sendstr->ep             = _dstranks[i];
      new (&sendstr->dsttopology) PAMI::Topology(&sendstr->ep, 1, PAMI::tag_eplist());
      unsigned dstindex = _gtopology->endpoint2Index(_dstranks[i]);
      size_t     buflen;
      size_t     pwqlen;
      unsigned   offset;
      TypeCode * tmpbufType;

      if (_disps && _sndcounts)
        {
          CCMI_assert(_native->endpoint() == _root);
          CCMI_assert(ndst == 1);
          buflen   =  _sndcounts[dstindex] * _stype->GetDataSize();
          pwqlen   =  _sndcounts[dstindex] * _stype->GetExtent();
          offset   =  _disps[dstindex] * _stype->GetExtent();
          _mdata._count = buflen;
          tmpbufType = _stype;
        }
      else if ((unsigned)_nphases == _gtopology->size() - 1)
        {
          pwqlen   = _sbufcnt  * _stype->GetExtent();
          buflen   = _sbufcnt  * _stype->GetDataSize();
          offset   = dstindex * _sbufcnt * _stype->GetExtent();
          tmpbufType = _stype;
        }
      else
        {
          if(_tmpbufIsContig)
          {
            buflen   = _dstlens[i] * _sbufcnt * _stype->GetDataSize();
            pwqlen   =  buflen;
            offset   = ((dstindex + size - _myindex) % size) * _sbufcnt * _stype->GetDataSize();
            tmpbufType = (TypeCode *)PAMI_TYPE_BYTE;
          }
          else
          {
            buflen   = _dstlens[i] * _sbufcnt * _stype->GetDataSize();
            pwqlen   = _dstlens[i] * _sbufcnt * _stype->GetExtent();;
            offset   = ((dstindex + size - _myindex) % size) * _sbufcnt * _stype->GetExtent();
            tmpbufType = _stype;
          }
        }

      char    *tmpbuf   = _tmpbuf + offset;

      sendstr->pwq.configure (tmpbuf, pwqlen, pwqlen, NULL, tmpbufType);

      msend->src_participants   = (pami_topology_t *) & _selftopology;
      msend->dst_participants   = (pami_topology_t *) & sendstr->dsttopology;
      msend->cb_done.function   = notifySendDone;
      msend->cb_done.clientdata = this;
      msend->src                = (pami_pipeworkqueue_t *) & sendstr->pwq;
      msend->dst                = NULL;
      msend->bytes              = buflen;

      _native->multicast(&_msendstr[i].msend);
    }
}

template <class T_ConnMgr, class T_Schedule, typename T_Scatter_type, typename T_Coll_header>
inline void  CCMI::Executor::ScatterExec<T_ConnMgr, T_Schedule, T_Scatter_type, T_Coll_header>::notifyRecv
(unsigned             src,
 const pami_quad_t   & info,
 PAMI::PipeWorkQueue ** pwq,
 pami_callback_t      * cb_done)
{
  TRACE_MSG((stderr, "<%p> Executor::ScatterExec::notifyRecv() from %d\n", this, src));

  *pwq = &_pwq;

  if (_nphases > 1)
    {
      TRACE_ADAPTOR((stderr, "<%p> Executor::ScatterExec::notifyRecv() \n", this));
      /// \todo this sendNext() should have worked but MPI platform didn't support it (yet).
      //    cb_done->function = NULL;  //There is a send here that will notify completion
      //    sendNext ();
      cb_done->function = notifyRecvDone;
      cb_done->clientdata = this;
    }
  else
    {
      TRACE_ADAPTOR((stderr, "<%p> Executor::ScatterExec::notifyRecv() Nothing to send, receive completion indicates completion\n", this));
      cb_done->function   = _cb_done;
      cb_done->clientdata = _clientdata;
    }
}

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
