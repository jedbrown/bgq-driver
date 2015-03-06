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
 * \file algorithms/executor/Gather.h
 * \brief ???
 */
#ifndef __algorithms_executor_Gather_h__
#define __algorithms_executor_Gather_h__

#include "Global.h"
#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/connmgr/ConnectionManager.h"
#include "algorithms/interfaces/NativeInterface.h"

#define MAX_CONCURRENT 32
//#define MAX_PARALLEL 20

#define EXECUTOR_DEBUG(x) // fprintf x

namespace CCMI
{
  namespace Executor
  {
    /*
     * Implements a gather strategy which uses one network link.
     */

    template <class T_Gather_type>
    struct GatherVecType
    {
      // COMPILE_TIME_ASSERT(0==1);
    };

    template<>
    struct GatherVecType<pami_gather_t>
    {
      typedef int base_type;
    };

    template<>
    struct GatherVecType<pami_gatherv_t>
    {
      typedef size_t base_type;
    };

    template<>
    struct GatherVecType<pami_gatherv_int_t>
    {
      typedef int base_type;
    };

    template <class T_Gather_type>
    extern inline void setGatherVectors(T_Gather_type *xfer, void *disps, void *sndcounts)
    {
    }

    template<>
    inline void setGatherVectors<pami_gather_t> (pami_gather_t *xfer, void *disps, void * rcvcounts)
    {
      (void)xfer;(void)disps;(void)rcvcounts;
    }

    template<>
    inline void setGatherVectors<pami_gatherv_t> (pami_gatherv_t *xfer, void *disps, void *rcvcounts)
    {
       *((size_t **)disps)     = xfer->rdispls;
       *((size_t **)rcvcounts) = xfer->rtypecounts;
    }

    template<>
    inline void setGatherVectors<pami_gatherv_int_t> (pami_gatherv_int_t *xfer, void *disps, void *rcvcounts)
    {
       *((int **)disps)     = xfer->rdispls;
       *((int **)rcvcounts) = xfer->rtypecounts;
    }


    template<class T_ConnMgr, class T_Schedule, typename T_Gather_type>
    class GatherExec : public Interfaces::Executor
    {
      public:

        struct RecvStruct
        {
          int                 subsize;
          PAMI::PipeWorkQueue pwq;
          GatherExec         *exec;
        };

      protected:
        T_Schedule                     * _comm_schedule;
        Interfaces::NativeInterface    * _native;
        T_ConnMgr                      * _connmgr;

        int                 _comm;
        pami_endpoint_t     _root;
        int                 _bufcnt;
        int                 _totallen;
        char                *_sbuf;
        char                *_rbuf;
        TypeCode            *_stype;
        TypeCode            *_rtype;
        char                *_tmpbuf;
        unsigned            _myindex;
        unsigned            _rootindex;

        PAMI::PipeWorkQueue _pwq;
        RecvStruct          *_mrecvstr;

        int                 _curphase;
        int                 _nphases;
        int                 _startphase;
        int                 _donecount;
        int                 _mynphases;

        int                 _maxsrcs;
        pami_task_t         _srcranks [MAX_CONCURRENT];
        unsigned            _srclens  [MAX_CONCURRENT];
        pami_endpoint_t     _tmp_ep;
        PAMI::Topology      _dsttopology;
        PAMI::Topology      _selftopology;
        PAMI::Topology      *_gtopology;

        CollHeaderData      _mdata;
        pami_multicast_t    _msend;

        typedef typename GatherVecType<T_Gather_type>::base_type basetype;
        basetype            *_disps;
        basetype            *_rcvcounts;

        //Private method
        void             sendNext ();

      public:
        GatherExec () :
            Interfaces::Executor (),
            _comm_schedule(NULL),
            _comm(-1),
            _sbuf(NULL),
            _rbuf(NULL),
            _stype(NULL),
            _rtype(NULL),
            _tmpbuf(NULL),
            _curphase(0),
            _nphases(0),
            _startphase(0),
            _donecount(-1),
            _mynphases(0),
            _disps(NULL),
            _rcvcounts(NULL)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::GatherExec()\n", this));
        }

        GatherExec (Interfaces::NativeInterface  * mf,
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
            _stype(NULL),
            _rtype(NULL),
            _tmpbuf(NULL),
            _mrecvstr(NULL),
            _curphase(0),
            _nphases(0),
            _startphase(0),
            _donecount (-1),
            _mynphases(0),
            _dsttopology(),
            _selftopology(mf->endpoint()),
            _gtopology(gtopology),
            _disps(NULL),
            _rcvcounts(NULL)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::GatherExec(...)\n", this));
          _clientdata        =  0;
          _root              =  (unsigned) - 1;
          _bufcnt            =  0;

          _mdata._comm       = _comm;
          _mdata._root       = _root;
          _mdata._count      = -1;
          _mdata._phase      = 0;

          pami_quad_t *info    =  (pami_quad_t*)((void*) & _mdata);
          _msend.msginfo       =  info;
          _msend.msgcount      =  1;
          _msend.roles         = -1U;

        }

        virtual ~GatherExec ()
        {
           /// Todo: convert this to allocator ?
           if (_maxsrcs) __global.heap_mm->free(_mrecvstr);
           if (!(_disps && _rcvcounts)) __global.heap_mm->free(_tmpbuf);
        }

        /// NOTE: This is required to make "C" programs link successfully with virtual destructors
        void operator delete(void * p)
        {
        }

        // --  Initialization routines
        //------------------------------------------

        void setSchedule (T_Schedule *ct)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::GatherExec::setSchedule()\n", this));
          _comm_schedule = ct;
          _comm_schedule->init (_root, CCMI::Schedule::GATHER, _startphase, _nphases, _maxsrcs);
          CCMI_assert(_maxsrcs <= MAX_CONCURRENT);

          _mynphases = _comm_schedule->getMyNumPhases();

          _myindex    = _gtopology->endpoint2Index(_native->endpoint());
          _rootindex  = _gtopology->endpoint2Index(_root);

          unsigned connection_id = (unsigned) - 1;

          if (_connmgr)
            connection_id = _connmgr->getConnectionId(_comm, _root, 0, (unsigned) - 1, (unsigned) - 1);

          _msend.connection_id = connection_id;

          // todo: this is clearly not scalable, need to use rendezvous similar to
          // that in allgather
          if (_maxsrcs) {
	        pami_result_t rc;
	        rc = __global.heap_mm->memalign((void **)&_mrecvstr, 0,
					_maxsrcs * _mynphases * sizeof(RecvStruct));
	        PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _mrecvstr");
	      }
        }

        void setConnectionID (unsigned cid)
        {

          CCMI_assert(_comm_schedule != NULL);

          //Override the connection id from the connection manager
          _msend.connection_id = cid;

        }

        void setRoot(unsigned root)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::GatherExec::setRoot()\n", this));
          _root              = root;
          _mdata._root       = _root;
        }

        void  updateBuffers(char *src, char *dst, int count, TypeCode *stype, TypeCode *rtype)
        {
          _bufcnt = count;
          _sbuf   = (src == PAMI_IN_PLACE)?
                     dst + ((_disps == NULL)?rtype->GetExtent() * count * _myindex:
                            _disps[_myindex] * rtype->GetExtent()) : src; 
          _rbuf   = dst;
          _stype  = (src == PAMI_IN_PLACE)?rtype:stype;
          _rtype  = rtype;

        }

        void updatePWQ()
        {
          if (_native->endpoint() != _root)
          {
            size_t  bufstride = _totallen * _bufcnt * _stype->GetExtent();
            size_t  buflen    = _totallen * _bufcnt * _stype->GetDataSize();
            if (_mynphases > 1)
            {
              _pwq.configure (_tmpbuf, buflen, buflen);
            }
            else
            {
              _pwq.configure (_sbuf, bufstride, bufstride, _rtype, _stype);
            }
          }
        }

        void  setBuffers (char *src, char *dst, int count, TypeCode *stype, TypeCode *rtype)
        {
          TRACE_ADAPTOR((stderr, "<%p>Executor::GatherExec::setInfo() src %p, dst %p, count %d, _pwq %p\n", this, src, dst, count, &_pwq));

          _bufcnt   = count;
          _sbuf     = (src == PAMI_IN_PLACE)?
                      dst + ((_disps == NULL)?rtype->GetExtent() * count * _myindex:
                            _disps[_myindex] * rtype->GetExtent()) : src;
          _rbuf     = dst;
          _stype    = (src == PAMI_IN_PLACE)?rtype:stype;
          _rtype    = rtype;

          // ship data length info in the header for async protocols
          _mdata._count = _bufcnt * _stype->GetDataSize();
          CCMI_assert(_comm_schedule != NULL);

          if (_native->endpoint() == _root)
          {
            _donecount = _gtopology->size();
            size_t bufcnt = 0;
            if (_disps && _rcvcounts)
            {
                for (unsigned i = 0; i < _gtopology->size() ; ++i)
                {
                  bufcnt += _rcvcounts[i];
                  if (_rcvcounts[i] == 0 && i != _rootindex) _donecount--;
                }
                _bufcnt = bufcnt;
                _tmpbuf = _rbuf;
            }
            else
            {
                bufcnt = _gtopology->size() * count;
                pami_result_t rc;
                rc = __global.heap_mm->memalign((void **)&_tmpbuf, 0, bufcnt * _rtype->GetDataSize());
                PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _tmpbuf (%zu*%zu) (%zu*%u)\n",bufcnt,_rtype->GetDataSize(),_gtopology->size(),count);
            }
          }
          else // setup PWQ
          {
              unsigned ndst;
              _comm_schedule->getLList(_startphase, &_srcranks[0], ndst, &_srclens[0]);
              CCMI_assert(ndst == 1);

              // We make a copy of this endpoint
              // because the src_ranks list must not be
              // overwritten
              _tmp_ep = _srcranks[0];
              new (&_dsttopology) PAMI::Topology(&_tmp_ep, 1, PAMI::tag_eplist());

              _donecount        = _srclens[0];
              size_t  bufcnt    = _srclens[0]  * _bufcnt;
              if (_mynphases > 1)
              {
                pami_result_t rc;
                rc = __global.heap_mm->memalign((void **)&_tmpbuf, 0, bufcnt * _stype->GetDataSize());
                PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _tmpbuf (%zu*%zu) (%u*%u)\n",bufcnt,_stype->GetDataSize(),_srclens[0],_bufcnt);
               _pwq.configure (_tmpbuf, bufcnt * _stype->GetDataSize(), bufcnt * _stype->GetDataSize());
              }
              else
              {
                _pwq.configure (_sbuf, bufcnt * _stype->GetExtent(), bufcnt * _stype->GetExtent(), _rtype, _stype);
              }

              _totallen = _srclens[0];

          }
        }

        void setVectors(T_Gather_type *xfer)
        {

          if (_native->endpoint() == _root)
            {
              setGatherVectors<T_Gather_type>(xfer, (void *)&_disps, (void *)&_rcvcounts);
            }
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

        static void notifyRecvDone( pami_context_t   context,
                                    void           * cookie,
                                    pami_result_t    result )
        {
          TRACE_MSG ((stderr, "<%p>Executor::GatherExec::notifyRecvDone()\n", cookie));
          RecvStruct    *mrecv = (RecvStruct *) cookie;
          GatherExec<T_ConnMgr, T_Schedule, T_Gather_type> *exec =  mrecv->exec;

          EXECUTOR_DEBUG((stderr, "GatherExec::notifyRecvDone, donecount = %d, subsize = %d\n", exec->_donecount, mrecv->subsize);)
          exec->_donecount -= mrecv->subsize;
          if (exec->_donecount == 0) exec->sendNext();
        }


    };  //-- GatherExec
  };   //-- Executor
};  //-- CCMI

///
/// \brief start sending gather data. Only active on the root node
///
template <class T_ConnMgr, class T_Schedule, typename T_Gather_type>
inline void  CCMI::Executor::GatherExec<T_ConnMgr, T_Schedule, T_Gather_type>::start ()
{
  TRACE_ADAPTOR((stderr, "<%p>Executor::GatherExec::start() count%d\n", this, _bufcnt));

  EXECUTOR_DEBUG((stderr, "GatherExec::start, mynphases = %d, buflen = %d, donecount = %d\n",
                  _mynphases, _bufcnt, _donecount);)

  // Nothing to gather? We're done. What if in Gatherv ?
  if ((_bufcnt == 0) && _cb_done)
    {
      _cb_done (NULL, _clientdata, PAMI_SUCCESS);
      return;
    }

  _curphase  = _startphase;

  if (_native->endpoint() == _root)
    {
      if (_disps && _rcvcounts)
        PAMI_Type_transform_data((void*)_sbuf,
                            _stype, 0,
                            _rbuf + _disps[_rootindex] * _rtype->GetExtent(),
                            _rtype, 0,
                            _rcvcounts[_rootindex] * _rtype->GetDataSize(),
                            PAMI_DATA_COPY, NULL);
      else
        PAMI_Type_transform_data((void*)_sbuf,
                            _stype, 0,
                            _rbuf + _rootindex * _bufcnt * _rtype->GetExtent(),
                            _rtype, 0,
                            _bufcnt * _rtype->GetDataSize(),
                            PAMI_DATA_COPY, NULL);
    }
  else if (_mynphases > 1)  PAMI_Type_transform_data((void*)_sbuf, _stype, 0, _tmpbuf,
                                                 PAMI_TYPE_BYTE, 0, _bufcnt * _stype->GetDataSize(),
                                                 PAMI_DATA_COPY, NULL);

  --_donecount;

  if (_donecount == 0) sendNext();
}

template <class T_ConnMgr, class T_Schedule, typename T_Gather_type>
inline void  CCMI::Executor::GatherExec<T_ConnMgr, T_Schedule, T_Gather_type>::sendNext ()
{
  CCMI_assert(_comm_schedule != NULL);
  CCMI_assert(_donecount  == 0);

  // TODO: needs to add noncontiguous datatype handling
  if (_native->endpoint() == _root)
    {
      if (!(_disps && _rcvcounts))
        {
          if (_rootindex != 0)
            {
              PAMI_Type_transform_data((void*)(_tmpbuf + _bufcnt * _rtype->GetDataSize()), PAMI_TYPE_BYTE, 0,
                                  _rbuf + ((_myindex + 1) % _gtopology->size()) * _bufcnt * _rtype->GetExtent(),
                                  _rtype, 0, (_gtopology->size() - _myindex - 1) * _bufcnt * _rtype->GetDataSize(),
                                  PAMI_DATA_COPY, NULL);
              PAMI_Type_transform_data((void*)(_tmpbuf + (_gtopology->size() - _myindex) * _bufcnt * _rtype->GetDataSize()),
                                  PAMI_TYPE_BYTE, 0, _rbuf, _rtype, 0, _myindex * _bufcnt * _rtype->GetDataSize(),
                                  PAMI_DATA_COPY, NULL);
            }
          else
            {
              PAMI_Type_transform_data((void*)(_tmpbuf + _bufcnt * _rtype->GetDataSize()),
                                  PAMI_TYPE_BYTE, 0, _rbuf + _bufcnt * _rtype->GetExtent(),
                                  _rtype, 0, (_gtopology->size() - 1) * _bufcnt * _rtype->GetDataSize(),
                                  PAMI_DATA_COPY, NULL);
            }
        }

      if (_cb_done) _cb_done (NULL, _clientdata, PAMI_SUCCESS);

      return;
    }

  _mdata._phase             = _startphase;
  _msend.src_participants   = (pami_topology_t *) & _selftopology;
  _msend.dst_participants   = (pami_topology_t *) & _dsttopology;
  _msend.cb_done.function   = _cb_done;
  _msend.cb_done.clientdata = _clientdata;
  _msend.src                = (pami_pipeworkqueue_t *) & _pwq;
  _msend.dst                = NULL;
  _msend.bytes              = _totallen * _bufcnt * _stype->GetDataSize();

  EXECUTOR_DEBUG((stderr, "GatherExec::sendNext() to %d, totalcnt = %d, buflen = %d, multicast address %p, %p\n", _dsttopology.index2Endpoint(0), _totallen, _bufcnt, &_msend, &_mdata);)
  _native->multicast(&_msend);
}

template <class T_ConnMgr, class T_Schedule, typename T_Gather_type >
inline void  CCMI::Executor::GatherExec<T_ConnMgr, T_Schedule, T_Gather_type>::notifyRecv
(unsigned             src,
 const pami_quad_t   & info,
 PAMI::PipeWorkQueue ** pwq,
 pami_callback_t      * cb_done)
{

  CollHeaderData *cdata = (CollHeaderData*) & info;

  unsigned i;
  unsigned nsrcs;
  _comm_schedule->getRList(cdata->_phase, &_srcranks[0], nsrcs, &_srclens[0]);

  for (i = 0; i < nsrcs; ++i)
    if (_srcranks[i] == src) break;

  CCMI_assert(i < nsrcs);

  unsigned srcindex = _gtopology->endpoint2Index(_srcranks[i]);
  unsigned size     = _gtopology->size();

  size_t      buflen;
  unsigned    offset;
  TypeCode  * tmpbufType;
  if (_disps && _rcvcounts)
    {
      CCMI_assert(_native->endpoint() == _root);
      _srclens[i] = 1;
      buflen      =  _rcvcounts[srcindex] * _rtype->GetExtent();
      offset      =  _disps[srcindex] * _rtype->GetExtent();
      tmpbufType  =  _rtype;
    }
  else
    {
      buflen      = _srclens[i] * _bufcnt  * _rtype->GetDataSize();
      offset      = ((srcindex + size - _myindex) % size) * _bufcnt * _rtype->GetDataSize();
      tmpbufType  = (TypeCode *)PAMI_TYPE_BYTE;
    }

  // CCMI_assert (buflen == cdata->_count);

  char    *tmpbuf   = _tmpbuf + offset;
  unsigned ind      = (_nphases - cdata->_phase - 1) * _maxsrcs + i;
  *pwq = &_mrecvstr[ind].pwq;
  (*pwq)->configure (tmpbuf, buflen, 0, tmpbufType);
  // (*pwq)->produceBytes(buflen);
  _mrecvstr[ind].subsize  = _srclens[i];
  _mrecvstr[ind].exec     = this;

  EXECUTOR_DEBUG((stderr, "GatherExec::notifyRecv - src = %d, offset = %d, lenth = %d, phase = %d\n", src, offset, buflen, cdata->_phase);)
  cb_done->function    = notifyRecvDone;
  cb_done->clientdata  = &_mrecvstr[ind];
}

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
