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
 * \file algorithms/executor/Scan.h
 * \brief ???
 */
#ifndef __algorithms_executor_Scan_h__
#define __algorithms_executor_Scan_h__


#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/connmgr/ConnectionManager.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "common/default/Topology.h"

#define MAX_CONCURRENT_SCAN 32

#if defined EXECUTOR_DEBUG
#undef EXECUTOR_DEBUG
#define EXECUTOR_DEBUG(x)  // fprintf x
#else
#define EXECUTOR_DEBUG(x)  // fprintf x
#endif

namespace CCMI
{
  namespace Executor
  {
    /*
     * Implements an scan strategy
     */
    template<class T_ConnMgr, class T_Schedule>
    class ScanExec : public Interfaces::Executor
    {
      public:

        struct RecvStruct
        {
          pami_task_t         rank;
          size_t              subsize;
          PAMI::PipeWorkQueue pwq;
        };

        struct PhaseRecvStr
        {
          int             donecount;
          int             partnercnt;
          ScanExec        *exec;
          RecvStruct      recvstr[MAX_CONCURRENT_SCAN];
        };

      protected:
        T_Schedule                     * _comm_schedule;
        Interfaces::NativeInterface    * _native;
        T_ConnMgr                      * _connmgr;

        int                 _comm;
        int                 _buflen;   // byte count of a single message, not really buffer length
        char                *_sbuf;
        char                *_rbuf;
        char                *_tmpbuf;
        TypeCode            *_stype;
        TypeCode            *_rtype;
        char                *_usrrcvbuf;   //SSS: A pointer to save the _dstbuf in
        char                *_tmpsndbuf;   //SSS: A temp buf to pack user send data
        int                  _alloctmpsnd; //SSS: Temp send buffer is allocated or not
        char                *_tmprcvbuf;   //SSS: A temp buf to pack user rcvd data
        int                  _alloctmprcv; //SSS: Temp recv buffer is allocated or not

        coremath            _reduceFunc;
        unsigned            _sizeOfType;

        unsigned            _myindex;

        PhaseRecvStr        *_mrecvstr;

        int                 _curphase;
        int                 _nphases;
        int                 _startphase;
        int                 _endphase;
        int                 _exclusive;// 0 = Inclusive, 1 = Exclusive
        int                 _donecount;

        unsigned            _connection_id;

        int                 _maxsrcs;
        pami_task_t         _dstranks [MAX_CONCURRENT_SCAN];
        unsigned            _dstlens  [MAX_CONCURRENT_SCAN];
        pami_task_t         _srcranks [MAX_CONCURRENT_SCAN];
        unsigned            _srclens  [MAX_CONCURRENT_SCAN];
        pami_endpoint_t     _tmp_ep   [MAX_CONCURRENT_SCAN];
        pami_endpoint_t     _self_ep;
        PAMI::Topology      _selftopology;
        PAMI::Topology      _dsttopology [MAX_CONCURRENT_SCAN];
        PAMI::Topology      *_gtopology;

        PAMI::PipeWorkQueue _pwq [MAX_CONCURRENT_SCAN];
        ExtCollHeaderData   _mdata [MAX_CONCURRENT_SCAN];
        pami_multicast_t    _msend [MAX_CONCURRENT_SCAN];

        //Private method
        void             sendNext ();
        void             localReduce ();

      public:
        ScanExec () :
            Interfaces::Executor (),
            _comm_schedule(NULL),
            _comm(-1),
            _sbuf(NULL),
            _rbuf(NULL),
            _tmpbuf(NULL),
            _usrrcvbuf(NULL),
            _alloctmpsnd(0),
            _alloctmprcv(0),
            _reduceFunc(NULL),
            _curphase(0),
            _nphases(0),
            _startphase(0),
            _endphase(-1),
            _exclusive(0)
        {
          EXECUTOR_DEBUG((stderr, "<%p>Executor::ScanExec()\n", this);)
        }

        ScanExec (Interfaces::NativeInterface  * mf,
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
            _tmpbuf(NULL),
            _usrrcvbuf(NULL),
            _alloctmpsnd(0),
            _alloctmprcv(0),
            _reduceFunc(NULL),
            _mrecvstr(NULL),
            _curphase(0),
            _nphases(0),
            _startphase(0),
            _endphase(-1),
            _exclusive(0),
            _self_ep(mf->endpoint()),
            _selftopology(&_self_ep, 1,PAMI::tag_eplist()),
            _gtopology(gtopology)
        {
          EXECUTOR_DEBUG((stderr, "<%p>Executor::ScanExec(...)\n", this);)
          _clientdata        =  0;
          _buflen            =  0;
        }

        virtual ~ScanExec ()
        {
          /// Todo: convert this to allocator ?
          __global.heap_mm->free (_mrecvstr);
          __global.heap_mm->free (_tmpbuf);
          if(_alloctmprcv)__global.heap_mm->free(_tmprcvbuf);
          if(_alloctmpsnd)__global.heap_mm->free(_tmpsndbuf);
        }

        /// NOTE: This is required to make "C" programs link successfully with virtual destructors
        void operator delete(void * p)
        {
        }

        // --  Initialization routines
        //------------------------------------------

        void setSchedule (T_Schedule *ct)
        {
          EXECUTOR_DEBUG((stderr, "<%p>Executor::ScanExec::setSchedule()\n", this);)
          _comm_schedule = ct;
          // initialize schedule as if everybody is root
          _comm_schedule->init (_native->endpoint(), CCMI::Schedule::SCATTER, _startphase, _nphases, _maxsrcs);
          CCMI_assert(_startphase == 0);
          CCMI_assert(_maxsrcs != 0);
          CCMI_assert(_maxsrcs <= MAX_CONCURRENT_SCAN);

          pami_result_t rc;
          rc = __global.heap_mm->memalign((void **)&_mrecvstr, 0, (_nphases + 1) * sizeof(PhaseRecvStr));
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _mrecvstr");

          for (int i = 0; i < _nphases; ++i)
            {
              _mrecvstr[i].donecount  = 0;
              _mrecvstr[i].partnercnt = 0;
              _mrecvstr[i].exec       = NULL;
            }

          for (int i = 0; i < _maxsrcs; ++i)
            {
              _mdata[i]._comm       = _comm;
              _mdata[i]._root       = -1; // not used on scan
              _mdata[i]._count      = 0;
              _mdata[i]._phase      = 0;

              pami_quad_t *info    =  (pami_quad_t*)((void*) & _mdata);
              _msend[i].msginfo       =  info;
              _msend[i].msgcount      =  1;
              _msend[i].roles         = -1U;
            }

          _myindex    = _gtopology->endpoint2Index(_native->endpoint());

          for (unsigned i = 1; i < _gtopology->size(); i *= 2)
            {
              if (_myindex  >= i)
                _endphase ++;
              else
                break;
            }

          if (_connmgr)
            _connection_id = _connmgr->getConnectionId(_comm, (unsigned) - 1, 0, (unsigned) - 1, (unsigned) - 1);

          for (int i = 0; i < MAX_CONCURRENT_SCAN; ++i)
            _msend[i].connection_id = _connection_id;

        }

        void setConnectionID (unsigned cid)
        {

          CCMI_assert(_comm_schedule != NULL);

          _connection_id = cid;

          //Override the connection id from the connection manager
          for (int i = 0; i < MAX_CONCURRENT_SCAN; ++i)
            _msend[i].connection_id = cid;

        }

        void updateReduceInfo(unsigned         count,
                              unsigned         sizeOfType,
                              coremath         func,
                              TypeCode        *stype,
                              TypeCode        *rtype,
                              pami_op          op = PAMI_OP_COUNT,
                              pami_dt          dt = PAMI_DT_COUNT)
        {

          CCMI_assert(count * sizeOfType == (unsigned)_buflen);
          _reduceFunc    = func;
          _sizeOfType    = sizeOfType;
          _stype         = (_sbuf == _rbuf)?rtype:stype;
          _rtype         = rtype;

          if(!_stype->IsContiguous() || !rtype->IsContiguous())
          {
            pami_result_t rc;
            unsigned bytes = count * _stype->GetDataSize();
            if(!_stype->IsContiguous())
            {
              if(_alloctmpsnd) __global.heap_mm->free(_tmpsndbuf);
              rc = __global.heap_mm->memalign((void **)&_tmpsndbuf, 0, bytes);
              _alloctmpsnd = 1;//SSS: Indicating that _tmpsndbuf is allocated.
              PAMI_Type_transform_data((void*)_sbuf, _stype, 0,
                                             _tmpsndbuf, PAMI_TYPE_BYTE, 0, bytes, PAMI_DATA_COPY, NULL);
            }
            else
              _tmpsndbuf = (char*)_sbuf;

            if(!rtype->IsContiguous())
            {
              if(_alloctmprcv) __global.heap_mm->free(_tmprcvbuf);
              rc = __global.heap_mm->memalign((void **)&_tmprcvbuf, 0, bytes);
              _alloctmprcv = 1;
              _usrrcvbuf = _rbuf;
            }
            else
              _tmprcvbuf = _rbuf;



            _sbuf    = _tmpsndbuf;
            _rbuf    = _tmprcvbuf;
          }
          else
            _usrrcvbuf = NULL;

          for (int i = 0; i < _maxsrcs; ++i)
            {
              _mdata[i]._dt      = dt;
              _mdata[i]._op      = op;
            }
        }


        // must be called after setBuffers or updateBuffers
        void setReduceInfo( unsigned         count,
                            unsigned         sizeOfType,
                            coremath         func,
                            TypeCode        *stype,
                            TypeCode        *rtype,
                            pami_op          op = PAMI_OP_COUNT,
                            pami_dt          dt = PAMI_DT_COUNT)
        {
          CCMI_assert(count * sizeOfType == (unsigned)_buflen);
          _reduceFunc    = func;
          _sizeOfType    = sizeOfType;
          _stype         = (_sbuf == _rbuf)?rtype:stype;
          _rtype         = rtype;

          if(!_stype->IsContiguous() || !rtype->IsContiguous())
          {
            pami_result_t rc;
            unsigned bytes = count * _stype->GetDataSize();
            if(!_stype->IsContiguous())
            {
              if(_alloctmpsnd) __global.heap_mm->free(_tmpsndbuf);
              rc = __global.heap_mm->memalign((void **)&_tmpsndbuf, 0, bytes);
              _alloctmpsnd = 1;//SSS: Indicating that _tmpsndbuf is allocated.
              PAMI_Type_transform_data((void*)_sbuf, _stype, 0,
                                             _tmpsndbuf, PAMI_TYPE_BYTE, 0, bytes, PAMI_DATA_COPY, NULL);
            }
            else
              _tmpsndbuf = (char*)_sbuf;

            if(!rtype->IsContiguous())
            {
              if(_alloctmprcv) __global.heap_mm->free(_tmprcvbuf);
              rc = __global.heap_mm->memalign((void **)&_tmprcvbuf, 0, bytes);
              _alloctmprcv = 1;
              _usrrcvbuf = _rbuf;
            }
            else
              _tmprcvbuf = _rbuf;



            _sbuf    = _tmpsndbuf;
            _rbuf    = _tmprcvbuf;
          }
          else
            _usrrcvbuf = NULL;

          for (int i = 0; i < _maxsrcs; ++i)
            {
              _mdata[i]._dt      = dt;
              _mdata[i]._op      = op;
            }
        }


        void  updateBuffers(char *src, char *dst, int len)
        {
          _buflen = len;
          _sbuf = (src == PAMI_IN_PLACE)?dst:src;
          _rbuf   = dst;

          PAMI_assertf(_tmpbuf != NULL, "tmpbuf is NULL\n");
        }

        void  setBuffers (char *src, char *dst, int len)
        {
          EXECUTOR_DEBUG((stderr, "<%p>Executor::ScanExec::setBuffers: src = %p, dst = %p, len = %d, _pwq = %p\n",
                          this, src, dst, len, &_pwq);)

          _buflen = len;
          _sbuf = (src == PAMI_IN_PLACE)?dst:src;
          _rbuf = dst;

          CCMI_assert(_comm_schedule != NULL);
          size_t buflen = (_nphases + 1) * len;
          pami_result_t rc;
          rc = __global.heap_mm->memalign((void **)&_tmpbuf, 0, buflen);
          PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc _tmpbuf");
        }

        void setExclusive(int exclusive)
        {
          _exclusive = exclusive;
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
        unsigned       getComm   ()
        {
          return _comm;
        }

        //------------------------------------------
        // -- Get the source for the current phase
        // -- Temporary workaround until we fix GenericTreeSchedule
        //------------------------------------------
        void           getSource(unsigned *src, unsigned *nsrc, unsigned *srclen)
        {
          *src    = _gtopology->index2Endpoint((_myindex + _gtopology->size() - (1U << _curphase)) % _gtopology->size());
          *nsrc   = 1;
          *srclen = _buflen;
        }

        static void notifySendDone (pami_context_t context, void *cookie, pami_result_t result)
        {
          TRACE_MSG ((stderr, "<%p>Executor::ScanExec::notifySendDone\n", cookie));
          ScanExec<T_ConnMgr, T_Schedule> *exec =  (ScanExec<T_ConnMgr, T_Schedule> *) cookie;

          EXECUTOR_DEBUG((stderr, "Executor::ScanExec::notifySendDone: _curphase = %d, _donecount = %d, rcv donecount = %d, total recv = %d\n",
                          exec->_curphase, exec->_donecount, exec->_mrecvstr[exec->_curphase].donecount,
                          exec->_mrecvstr[exec->_curphase].partnercnt); )
          exec->_donecount --;

          if (exec->_donecount == 0)
            {
              exec->_mrecvstr[exec->_curphase].donecount -= exec->_mrecvstr[exec->_curphase].partnercnt;

              if (exec->_mrecvstr[exec->_curphase].donecount == 0)
                {
                  exec->_mrecvstr[exec->_curphase].partnercnt = 0;
                  exec->_curphase  ++;
                  exec->_donecount  = 0;
                  exec->localReduce();
                  exec->sendNext();
                }
            }
        }

        static void notifyRecvDone( pami_context_t   context,
                                    void           * cookie,
                                    pami_result_t    result )
        {
          TRACE_MSG ((stderr, "<%p>Executor::ScanExec::notifyRecvDone\n", cookie));
          PhaseRecvStr  *mrecv = (PhaseRecvStr *) cookie;
          ScanExec<T_ConnMgr, T_Schedule> *exec =  mrecv->exec;

          EXECUTOR_DEBUG((stderr, "Executor::ScanExec::notifyRecvDone: _curphase = %d, _donecount = %d, rcv donecount = %d, total recv =%d\n",
                          exec->_curphase, exec->_donecount, mrecv->donecount, mrecv->partnercnt); )

          mrecv->donecount ++;

          if (mrecv->donecount == 0)
            {
              exec->_curphase  ++;
              exec->_donecount  = 0;
              exec->localReduce();
              exec->sendNext();
            }
        }


    };  //-- ScanExec
  };   //-- Executor
};  //-- CCMI

///
/// \brief start sending scan data. Only active on the root node
///
template <class T_ConnMgr, class T_Schedule>
inline void  CCMI::Executor::ScanExec<T_ConnMgr, T_Schedule>::start ()
{
  EXECUTOR_DEBUG((stderr, "<%p>Executor::ScanExec::start: _buflen = %d\n", this, _buflen);)


  memcpy(_tmpbuf, _sbuf, _buflen);

  _curphase   = _startphase;
  _donecount  = 0;
  sendNext ();
}

template <class T_ConnMgr, class T_Schedule>
inline void  CCMI::Executor::ScanExec<T_ConnMgr, T_Schedule>::sendNext ()
{
  CCMI_assert(_comm_schedule != NULL);
  CCMI_assert(_donecount  == 0);

  unsigned srcindex, dstindex;
  unsigned dist;

  EXECUTOR_DEBUG((stderr, "Executor::ScanExec::sendNext: _curphase = %d, _startphase = %d, _nphases = %d\n",
                  _curphase, _startphase, _nphases);)

  if (_curphase < _startphase + _nphases)
    {

      unsigned ndsts, nsrcs;
      // _comm_schedule->getList(_curphase, &_srcranks[0], nsrcs, &_dstranks[0], ndsts, &_srclens[0], &_dstlens[0]);
      //_comm_schedule->getRList(_nphases - _curphase - 1, &_srcranks[0], nsrcs, &_srclens[0]);

      // Workaround until we fix GenericTreeSchedule
      getSource(&_srcranks[0], &nsrcs, &_srclens[0]);

      // only support binomial tree for now
      CCMI_assert(nsrcs == 1);

      _donecount = ndsts = nsrcs;

      if (_mrecvstr[_curphase].exec == NULL)
        {
          CCMI_assert(_mrecvstr[_curphase].donecount == 0);

          for (unsigned i = 0; i < nsrcs; ++i)
            {
              srcindex            = _gtopology->endpoint2Index(_srcranks[i]);

              if (srcindex < _myindex)
                {
                  RecvStruct *recvstr = &_mrecvstr[_curphase].recvstr[i];
                  recvstr->pwq.configure (_tmpbuf + (_curphase + 1)* _buflen, _buflen, 0);
                  recvstr->subsize    = _buflen;
                  recvstr->rank       = _srcranks[i];
                }
              else
                {
                  _mrecvstr[_curphase].donecount ++;
                }
            }

          _mrecvstr[_curphase].partnercnt = nsrcs;
          _mrecvstr[_curphase].exec       = this;
        }

      for (unsigned i = 0; i < nsrcs; ++i)
        {
          srcindex     = _gtopology->endpoint2Index(_srcranks[i]);
          dist         = (_myindex + _gtopology->size() - srcindex) % _gtopology->size();
          dstindex     = (_myindex + _gtopology->size() + dist) % _gtopology->size();

          if (dstindex > _myindex)
            {
              _dstranks[i] = _gtopology->index2Endpoint(dstindex);

              _tmp_ep[i] = _dstranks[i];
              new (&_dsttopology[i]) PAMI::Topology(&_tmp_ep[i], 1, PAMI::tag_eplist());

              size_t buflen = _buflen;
              _pwq[i].configure (_tmpbuf, buflen, 0);
              _pwq[i].produceBytes(buflen);


              _mdata[i]._phase             = _curphase;
              _mdata[i]._count             = _buflen;
              _msend[i].src_participants   = (pami_topology_t *) & _selftopology;
              _msend[i].dst_participants   = (pami_topology_t *) & _dsttopology[i];
              _msend[i].cb_done.function   = notifySendDone;
              _msend[i].cb_done.clientdata = this;
              _msend[i].src                = (pami_pipeworkqueue_t *) & _pwq[i];
              _msend[i].dst                = NULL;
              _msend[i].bytes              = buflen;

              EXECUTOR_DEBUG((stderr, "Executor::ScanExec::sendNext: send to %d during phase %d\n",
                              _dstranks[i], _curphase);)

              _native->multicast(&_msend[i]);

            }
          else
            {
              _donecount --;

              if (_donecount == 0)
                {
                  _mrecvstr[_curphase].donecount -= _mrecvstr[_curphase].partnercnt;

                  if (_mrecvstr[_curphase].donecount == 0)
                    {
                      _mrecvstr[_curphase].partnercnt = 0;
                      _curphase  ++;
                      _donecount  = 0;

                      localReduce();
                      sendNext();
                    }
                }
            }
        }

      return;
    }

  if(_exclusive == 0)
    {
      memcpy(_rbuf, _tmpbuf, _buflen);
    }

    if(_usrrcvbuf)//SSS: rtype is not contig
    {
       PAMI_Type_transform_data(_tmprcvbuf, PAMI_TYPE_BYTE, 0,
                                _usrrcvbuf, _rtype, 0,
                                _buflen, PAMI_DATA_COPY, NULL);
    }

  if (_cb_done) _cb_done (NULL, _clientdata, PAMI_SUCCESS);

  return;
}

template <class T_ConnMgr, class T_Schedule>
inline void  CCMI::Executor::ScanExec<T_ConnMgr, T_Schedule>::notifyRecv
(unsigned             src,
 const pami_quad_t   & info,
 PAMI::PipeWorkQueue ** pwq,
 pami_callback_t      * cb_done)
{

  ExtCollHeaderData *cdata = (ExtCollHeaderData*) & info;

  EXECUTOR_DEBUG((stderr, "Executor::ScanExec::notifyRecv: received from %d phase = %d, count = %d, _endphase = %d\n",
		          src, cdata->_phase, cdata->_count, (unsigned)_endphase);)

  unsigned sindex = 0;
  unsigned nsrcs;

  if (_mrecvstr[cdata->_phase].exec == NULL) {
    CCMI_assert(_mrecvstr[cdata->_phase].donecount == 0);
    CCMI_assert(cdata->_phase <= (unsigned)_endphase);
    // _comm_schedule->getRList(_nphases - cdata->_phase - 1, &_srcranks[0], nsrcs, &_srclens[0]);
    getSource(&_srcranks[0], &nsrcs, &_srclens[0]);

    CCMI_assert(nsrcs == 1);

      for (unsigned i = 0; i < nsrcs; ++i)
        {
          size_t buflen       = _buflen;
          EXECUTOR_DEBUG((stderr, "Executor::ScanExec::notifyRecv: Packet arrived before recv posted."
			          " phase  = %d, _buflen = %d, _srclens[%d] = %d, _srcranks[%d] = %d\n",
                          cdata->_phase, _buflen, i, _srclens[i], i, _srcranks[i]);)
          RecvStruct *recvstr = &_mrecvstr[cdata->_phase].recvstr[i];
          recvstr->pwq.configure (_tmpbuf + (cdata->_phase + 1) * _buflen, buflen, 0);
          recvstr->subsize = buflen;
          recvstr->rank    = _srcranks[i];

          if (_srcranks[i] == src)
            {
              sindex = i;
              // fprintf(stderr, "found index %d, for src %d\n", i, src);
            }

          CCMI_assert(i == 0);
        }

      _mrecvstr[cdata->_phase].exec       = this;
      _mrecvstr[cdata->_phase].partnercnt = nsrcs;
    }
  else
    {
      for (int i = 0; i < _mrecvstr[cdata->_phase].partnercnt; ++i)
        if (src == _mrecvstr[cdata->_phase].recvstr[i].rank)
          {
            sindex = i;
            break;
          }
    }

  *pwq = &_mrecvstr[cdata->_phase].recvstr[sindex].pwq;
  // fprintf(stderr, "phase %d, sindex %d, src pwq address %p\n", cdata->_phase, sindex, *pwq);

  cb_done->function = notifyRecvDone;
  cb_done->clientdata = &_mrecvstr[cdata->_phase];
}

template <class T_ConnMgr, class T_Schedule>
inline void CCMI::Executor::ScanExec<T_ConnMgr, T_Schedule>::localReduce() {
  // Task 0 need not perform a reduce
  if (_endphase != -1 && _curphase - 1 <= _endphase)
    {
      // Perform reduce operation before moving to the next phase
      void *bufs[2];
      bufs[1] = _tmpbuf + _curphase  * _buflen;
      // Check if we are performing an exclusive scan
      if(_exclusive == 1)
        {
          if(_curphase == 1)
            {
              memcpy(_rbuf, bufs[1], _buflen);
            }
          else
            {
              bufs[0] = _rbuf;
              _reduceFunc(_rbuf, bufs, 2, _buflen / _sizeOfType);
            }
        }
      bufs[0] = _tmpbuf;
      _reduceFunc(_tmpbuf, bufs, 2, _buflen / _sizeOfType);
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
