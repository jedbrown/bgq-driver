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
 * \file algorithms/executor/AllreduceBaseExec.h
 * \brief ???
 */
#ifndef __algorithms_executor_AllreduceBaseExec_h__
#define __algorithms_executor_AllreduceBaseExec_h__

#include <pami.h>
#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/connmgr/ConnectionManager.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "algorithms/executor/ScheduleCache.h"
#include "algorithms/executor/AllreduceCache.h"

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

#include "util/trace.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_ENTEREXIT

#ifdef CCMI_TRACE_ALL
  #define DO_TRACE_ENTEREXIT 1
  #define DO_TRACE_DEBUG     1
#else
  #define DO_TRACE_ENTEREXIT 0
  #define DO_TRACE_DEBUG     0
#endif

#define MAX_IMATH_COUNT  64

namespace CCMI
{
  namespace Executor
  {
    template <class T_Conn, bool T_Single=false>
    class AllreduceBaseExec : public Interfaces::Executor
    {
      private:
        /// \brief Static function to be passed into the done of multisend send
        static void staticNotifySendDone (pami_context_t context, void *cd, pami_result_t err)
        {
          TRACE_FN_ENTER();
          ((AllreduceBaseExec<T_Conn, T_Single> *)(cd))->AllreduceBaseExec<T_Conn, T_Single>::notifySendDone();
          TRACE_FN_EXIT();
        }

        /// \brief Static function to be passed into the done of multisend postRecv
        static void staticNotifyReceiveDone (pami_context_t context, void *cd, pami_result_t err)
        {
          TRACE_FN_ENTER();
          AllreduceBaseExec<T_Conn, T_Single> *exec = (AllreduceBaseExec<T_Conn, T_Single> *)cd;
          TRACE_FORMAT("<%p>", exec);
          if (exec->_isSendDone  && !exec->_inAdvance /*&&  //send has finished
				   exec->_initialized*/)
            exec->advance();
          TRACE_FN_EXIT();
        }

      protected:
        void inline_math_isum (void *dst, void *src1, void *src2, unsigned count)
        {
          TRACE_FN_ENTER();
          int *idst  = (int *) dst;
          int *isrc1 = (int *) src1;
          int *isrc2 = (int *) src2;

          TRACE_HEXDATA((char*)src1, count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)src2, count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)dst, 1); // Just to trace the input pointer

          for (unsigned c = 0; c < count; ++c)
            idst[c] = isrc1[c] + isrc2[c];

          TRACE_HEXDATA((char*)dst, count*_acache.getSizeOfType());
          TRACE_FN_EXIT();
        }

        void inline_math_dsum (void *dst, void *src1, void *src2, unsigned count)
        {
          TRACE_FN_ENTER();
          double *idst  = (double *) dst;
          double *isrc1 = (double *) src1;
          double *isrc2 = (double *) src2;

          TRACE_HEXDATA((char*)src1, count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)src2, count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)dst, 1); // Just to trace the input pointer

          for (unsigned c = 0; c < count; ++c)
            idst[c] = isrc1[c] + isrc2[c];

          TRACE_HEXDATA((char*)dst, count*_acache.getSizeOfType());
          TRACE_FN_EXIT();
        }

        void inline_math_dmin (void *dst, void *src1, void *src2, unsigned count)
        {
          TRACE_FN_ENTER();
          double *idst  = (double *) dst;
          double *isrc1 = (double *) src1;
          double *isrc2 = (double *) src2;

          TRACE_HEXDATA((char*)src1, count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)src2, count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)dst, 1); // Just to trace the input pointer

          for (unsigned c = 0; c < count; c++)
            idst[c] = (isrc1[c] < isrc2[c]) ? isrc1[c] : isrc2[c];

          TRACE_HEXDATA((char*)dst, count*_acache.getSizeOfType());
          TRACE_FN_EXIT();
        }

        void inline_math_dmax (void *dst, void *src1, void *src2, unsigned count)
        {
          TRACE_FN_ENTER();
          double *idst  = (double *) dst;
          double *isrc1 = (double *) src1;
          double *isrc2 = (double *) src2;

          TRACE_HEXDATA((char*)src1, count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)src2, count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)dst, 1); // Just to trace the input pointer

          for (unsigned c = 0; c < count; c++)
            idst[c] = (isrc1[c] > isrc2[c]) ? isrc1[c] : isrc2[c];

          TRACE_HEXDATA((char*)dst, count*_acache.getSizeOfType());
          TRACE_FN_EXIT();
        }

        void sendMessage (char               * buffer,
                          unsigned             size,
                          PAMI::Topology     * dst_topology,
                          unsigned             phase);

        /// State variables in order of access frequency
        unsigned            _curPhase;
        unsigned            _curIdx;
        unsigned            _endPhase;
        unsigned            _lastReducePhase;
        unsigned            _curnsrc;

        bool                _initialized;
        bool                _isSendDone;
        bool                _inAdvance;
        bool                _earlyArrival; //The local peer has not
                                           //called reset

        const char        * _srcbuf;
        char              * _dstbuf;
        char              * _reducebuf;
        char              * _src1;  //The current first summing buffer
        char              * _usrrcvbuf;   //SSS: A pointer to save the _dstbuf in
        char              * _tmpsndbuf;   //SSS: A temp buf to pack user send data
        int                 _alloctmpsnd; //SSS: Temp send buffer is allocated or not
        char              * _tmprcvbuf;   //SSS: A temp buf to pack user rcvd data
        int                 _alloctmprcv; //SSS: Temp recv buffer is allocated or not
        coremath            _reduceFunc;

	Interfaces::NativeInterface     * _native;
	pami_multicast_t                  _msend;
	ExtCollHeaderData                 _sndInfo __attribute__((__aligned__(16)));

        bool                _postReceives;
        bool                _enablePipelining;

        T_Conn * _rconnmgr;  ///Reduce connection manager
        T_Conn * _bconnmgr;  ///Broadcast connction manager

        pami_context_t                    _context;
        pami_endpoint_t                   _self_ep;
        PAMI::Topology                    _selftopology;
        ScheduleCache                     _scache;
        AllreduceCache<T_Conn>            _acache;	

      public:

        /// Default Destructor
        virtual ~AllreduceBaseExec ()
        {
          TRACE_FN_ENTER();
          if(_alloctmprcv)__global.heap_mm->free(_tmprcvbuf);
          if(_alloctmpsnd)__global.heap_mm->free(_tmpsndbuf);
#ifdef CCMI_DEBUG
          _curPhase = (unsigned) - 1;
          _curIdx = (unsigned) - 1;
          _endPhase = (unsigned) - 1;
          _lastReducePhase = (unsigned) - 1;
          _initialized = false;
          _postReceives = false;
          _enablePipelining = false;
          _srcbuf = NULL;
          _dstbuf = NULL;
          _reduceFunc = NULL;
          _native = NULL;
          _rconnmgr = NULL;
          _bconnmgr = NULL;
#endif
          TRACE_FORMAT("<%p>", this);
          TRACE_FN_EXIT();
        }

        /// Default Constructor
        AllreduceBaseExec () :
            Interfaces::Executor (),
            _curPhase ((unsigned) - 1),
            _curIdx ((unsigned) - 1),
            _lastReducePhase((unsigned) - 1),
            _curnsrc((unsigned)-1),
            _initialized (false),
            _inAdvance(false),
            _earlyArrival(false),
            _srcbuf (NULL), _dstbuf (NULL), _reducebuf(NULL),
            _src1 (NULL),
            _usrrcvbuf(NULL),
            _alloctmpsnd(0),
            _alloctmprcv(0),
            _reduceFunc (NULL),
            _native (NULL),
            _postReceives (false),
            _enablePipelining (false),
            _rconnmgr (NULL),
            _bconnmgr(NULL),
            _context(NULL),
            _selftopology(),
            _scache(),
            _acache(NULL, (unsigned) - 1)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>", this);
          TRACE_FN_EXIT();
        }

        ///  Main constructor to initialize the executor
        ///  By default it only needs one connection manager
        AllreduceBaseExec(Interfaces::NativeInterface    * native,
                          T_Conn                         * connmgr,
                          const unsigned                   commID,
                          bool                             enable_pipe=false):
            Interfaces::Executor(),
            _curPhase ((unsigned) - 1), _curIdx ((unsigned) - 1),
            _initialized (false),
            _isSendDone  (false),
            _inAdvance   (false),
            _earlyArrival(false),
            _srcbuf (NULL), _dstbuf (NULL), _reducebuf(NULL),
            _src1(NULL),
            _usrrcvbuf(NULL),
            _alloctmpsnd(0),
            _alloctmprcv(0),
            _reduceFunc (NULL),
            _native (native),
            _postReceives (false),
            _enablePipelining(false),
            _rconnmgr (connmgr),
            _bconnmgr(connmgr),
            _context(NULL),
            _self_ep(native->endpoint()),
            _selftopology(&_self_ep, 1,PAMI::tag_eplist()),
            _scache(),
            _acache(&_scache, native->endpoint())
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p>", this);

          _msend.msginfo     = NULL;
          _msend.msgcount    = 0;
          _msend.roles       = -1U;
          _msend.src_participants = (pami_topology_t *) & _selftopology;
          _msend.dst              = NULL;
          _msend.cb_done.function   = staticNotifySendDone;
          _msend.cb_done.clientdata = this;

          _sndInfo._comm     = commID;

          _acache.setReduceConnectionManager (connmgr);
          _acache.setBroadcastConnectionManager (connmgr);
	  
          _acache.setExecutor (this);
          _acache.setCommID (commID);

          TRACE_FN_EXIT();
        }

        pami_result_t advance ();	
        //Advance assuming nsrcranks is at most 1
        pami_result_t advance_single ();	
        //Advance for arbitrary nsrcranks
        pami_result_t advance_multiple ();
      
        bool earlyArrival() { return _earlyArrival; }

        void setIteration (unsigned iteration)
        {
          _sndInfo._iteration = iteration;
          _acache.setIteration(iteration);
        }

        void setContext (pami_context_t context) {
          _context = context;
        }

        void setSchedule (Interfaces::Schedule *schedule, unsigned color = 0)
        {
          _scache.setSchedule(schedule);
          _acache.setColor(color);
        }

        void setReduceConnectionManager (T_Conn *cmgr)
        {
          _rconnmgr = cmgr;
          _acache.setReduceConnectionManager(cmgr);
        }

        void setBroadcastConnectionManager (T_Conn *cmgr)
        {
          _bconnmgr = cmgr;
          _acache.setBroadcastConnectionManager(cmgr);
        }

        ///entry method
        void notifySendDone ( );

        ///entry method
        virtual void   notifyRecv  ( unsigned             src,
                                     const pami_quad_t   & info,
                                     PAMI::PipeWorkQueue ** pwq,
                                     pami_callback_t      * cb_done );

        /// entry method : start allreduce
        virtual void start();

        /// \brief Handle async receive headers
        /// \brief  Set the actual parameters for this [all]reduce operation
        ///         and calculate some member data based on them.
        ///
        /// \param[in]   info       Our header (includes fields for:
        ///                           root of the collective
        ///                           communicator
        ///                           length of the message
        ///                           phase of the collective
        /// \param[in]   count      Count of quads in info
        /// \param[in]   peer       Sender's rank
        /// \param[in]   sndlen     Message length
        /// \param[in]   conn_id    Connection id
        /// \param[in]   arg        Not used
        /// \param[out]  rcvlen     size of rcvbuf
        /// \param[out]  rcvbuf     buffer to receive datatype
        /// \param[out]  pipeWidth  pipeline width
        /// \param[out]  cb_done    receive callback function
        ///
        PAMI_Request_t *   notifyRecvHead(const pami_quad_t     * info,
					  unsigned               count,
					  unsigned               conn_id,
					  unsigned               peer,
					  unsigned               sndlen,
					  void                 * arg,
					  size_t               * rcvlen,
					  pami_pipeworkqueue_t ** recvpwq,
					  PAMI_Callback_t       * cb_done);
	
        ///
        /// \brief Set the buffer info for the allreduce collective
        ///        These members are always set.
        /// \param[in]  srcbuf
        /// \param[in]  dstbuf
        /// Note setDataInfo should be called before setReduceInfo!!
        ///

        void setBuffers ( void           * srcbuf,
                          void           * dstbuf,
                          unsigned         bytes,
                          unsigned         strides,
                          TypeCode       * stype,
                          TypeCode       * rtype)
        {
          _srcbuf        = (srcbuf == PAMI_IN_PLACE)?(char *)dstbuf : (char *) srcbuf;
          _dstbuf        = (char *) dstbuf;
	  
          // SSS: bytes, stype, rtype are not needed here..We just need this signature for compiling	    
        }

        /// \brief Set the parameters related to the reduce.  When
        ///  parameters change the allreduce state changes.
        ///
        /// \param[in]  count
        /// \param[in]  pipelineWidth
        /// \param[in]  sizeOfType
        /// \param[in]  func            reduce function
        void setReduceInfo( unsigned         count,
                            unsigned         pipelineWidth,
                            unsigned         sizeOfType,
                            coremath         func,
                            TypeCode       * stype,
                            TypeCode       * rtype,
                            pami_op          op = PAMI_OP_COUNT,
                            pami_dt          dt = PAMI_DT_COUNT )
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT("<%p> count %#X, pipelineWidth %#X, sizeOfType %#X, func %p, op %#X, dt %#X",
                      this, count, pipelineWidth, sizeOfType, func, op, dt);
          CCMI_assert (pipelineWidth % sizeOfType == 0);

          _reduceFunc    = func;

          TypeCode * lstype = stype;
          //override the pipeline width as we do not support
          //pipelining in this simple executor.
          if (!_enablePipelining)
            pipelineWidth = count * sizeOfType;

          if(_srcbuf == _dstbuf)//i.e. PAMI_IN_PLACE
          {
            lstype = rtype;
          }

          if(!lstype->IsContiguous() || !rtype->IsContiguous())
          {
            pami_result_t rc;
            unsigned bytes = count * lstype->GetDataSize();//SSS: DataSize may be different from AtomSize.
            if(!lstype->IsContiguous())
            {
              if(lstype != getStype() || bytes != _acache.getBytes())
              {
                if(_alloctmpsnd) __global.heap_mm->free(_tmpsndbuf);
                rc = __global.heap_mm->memalign((void **)&_tmpsndbuf, 0, bytes);
                _alloctmpsnd = 1;//SSS: Indicating that _tmpsndbuf is allocated.
                PAMI_Type_transform_data((void*)_srcbuf, lstype, 0,
                                             _tmpsndbuf, PAMI_TYPE_BYTE, 0, bytes, PAMI_DATA_COPY, NULL);
              }
            }
            else
              _tmpsndbuf = (char*)_srcbuf;

            if(!rtype->IsContiguous())
            {
              if(rtype != getRtype() || bytes != _acache.getBytes())
              {
                if(_alloctmprcv) __global.heap_mm->free(_tmprcvbuf);
                rc = __global.heap_mm->memalign((void **)&_tmprcvbuf, 0, bytes);
                _alloctmprcv = 1;
                _usrrcvbuf = _dstbuf;
              }
            }
            else
              _tmprcvbuf = _dstbuf;



            _srcbuf    = _tmpsndbuf;
            _dstbuf    = _tmprcvbuf;

            count      = bytes / sizeOfType;
          }
          else
            _usrrcvbuf = NULL;

          _acache.init(count, sizeOfType, op, dt, lstype, rtype, pipelineWidth);
          TRACE_FN_EXIT();
        }

        ///
        ///  \brief Set the mode of collective to be synchronous using post receives
        ///
        void setPostReceives ()
        {
          _postReceives = true;
        }

        /// \brief Set the root for reduction
        ///
        /// \param[in]  root  default/-1 indicates allreduce, any other
        ///                   root indicates reduce
        void setRoot(int root = -1)
        {
          _scache.setRoot(root);
        }

        /// \brief Get the root for reduction
        ///
        /// \return  root -1 indicates allreduce, any other
        ///                   root indicates reduce root
        int getRoot()
        {
          return _scache.getRoot();
        }

        /// \brief Get the async iteration id
        ///
        /// \return  iteration id
        unsigned getIteration()
        {
          return _acache.getIteration();
        }

        void postReceives ();
        //void postRecv (unsigned p, unsigned next);

        /// \brief Register the multicast interface
        ///
        /// \param[in]  mf
        void setNativeInterface(Interfaces::NativeInterface * mf)
        {
          _native = mf;
        }

        Interfaces::NativeInterface *getMulticastInterface()
        {
          return _native;
        }

        virtual void reset ()
        {
          PAMI::Memory::sync();
          _acache.setDstBuf (_dstbuf);

          //Make sure we are not reseting a live executor
          CCMI_assert (_initialized == false);

          bool flag;
          if(_scache.getRoot() == -1)
            flag = _scache.init(ALLREDUCE_OP);
          else
            flag = _scache.init(REDUCE_OP);

          _acache.reset (flag, false);

          _msend.bytes        = _acache.getBytes();
          _sndInfo._count     = _acache.getCount();
          _sndInfo._dt        = _acache.getDt();
          _sndInfo._op        = _acache.getOp();
          _sndInfo._root      = (unsigned)_scache.getRoot();

          if (!_postReceives) {
           _msend.msginfo  =  (pami_quad_t *) &_sndInfo;
           _msend.msgcount =  1;
          }

          //Send done is only sent to true after the first send
          _isSendDone         = false;
          _inAdvance          = false;
          _earlyArrival       = false;
          _curPhase = _scache.getStartPhase();
          _curIdx = 0;
          _endPhase = _scache.getEndPhase();
	  _initialized = false;
	  _lastReducePhase = _scache.getLastReducePhase();
	  _curnsrc = _scache.getNumSrcRanks(_curPhase);

	  if ((this->_scache.getRoot() == -1) || (this->_scache.getRoot() == (int)_native->endpoint()))
	    _reducebuf = _dstbuf;
	  else
	    //Reduce operation and I am not the root
	    _reducebuf = _acache.getTempBuf();	  

	  _src1 = (char *)_srcbuf;	  
	  for (unsigned p=_lastReducePhase+1; p <= _endPhase; ++p)
	  {
	    if (_scache.getNumSrcRanks(p) == 1) {
	      PAMI::PipeWorkQueue *pwq = _acache.getPhasePipeWorkQueues(p,0);
	      pwq->configure(_dstbuf, _acache.getBytes(), 0); 
	    }
	  }
        }

        void operator delete(void * p)
        {
          CCMI_abort();
        }

        pami_op    getOp ()
        {
          return  _acache.getOp();
        }

        pami_dt    getDt ()
        {
          return  _acache.getDt();
        }

        unsigned   getCount ()
        {
          return  _acache.getCount();
        }

        TypeCode * getStype()
        {
          return _acache.getStype();
        }

        TypeCode * getRtype()
        {
          return _acache.getRtype();
        }

        static void _compile_time_assert_ ()
        {
        }
    }; // AllreduceBaseExec
  } // Executor
} // CCMI


template <class T_Conn, bool T_Single>
  inline pami_result_t CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::advance () {  

  if (T_Single)
    return advance_single();
  else 
    return advance_multiple();
}


template <class T_Conn, bool T_Single>
inline pami_result_t CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::advance_multiple ()
{
  TRACE_FN_ENTER();
  TRACE_FORMAT("<%p>_curPhase %d,_endPhase %d, lastReducePhase %d, _curIdx %d, nsrcranks %u", this,
                  _curPhase, _endPhase,_scache.getLastReducePhase(), _curIdx, _scache.getNumSrcRanks(_curPhase));
  CCMI_assert_debug (_initialized);
  pami_result_t  rc = PAMI_EAGAIN;

  _inAdvance = true;

  CCMI_assert (_reducebuf != NULL);
  char * reducebuf = _reducebuf;  
  size_t msg_bytes = _acache.getBytes();
  pami_op op = _acache.getOp();
  pami_dt dt = _acache.getDt();
  unsigned count = _acache.getCount();
  TRACE_FORMAT("<%p>op %u, dt %u, count %u", this,
	       op, dt, count);
  CCMI_assert (_src1 != NULL);
  void *src1 = _src1;

  unsigned nsrcranks = _curnsrc; 
  char **src2 = NULL;

  while (_curPhase <= _endPhase)
    {      
      PAMI::PipeWorkQueue *pwq = NULL;
      for (; _curIdx < nsrcranks; _curIdx ++) {
        pwq = _acache.getPhasePipeWorkQueues(_curPhase, _curIdx);
        if (pwq->bytesAvailableToConsume() < msg_bytes)
        {
          TRACE_FN_EXIT();
          goto fn_end;
        }
      }
      
      size_t idx = 0;
      src2 = _acache.getPhaseRecvBufsVec (_curPhase);
      if (_curPhase <= _lastReducePhase)
      {
        if (op == PAMI_SUM && dt == PAMI_SIGNED_INT)
        {
            for (idx = 0; idx < nsrcranks; ++idx)
            {
              inline_math_isum (reducebuf, src1, src2[idx], count);
              src1 = reducebuf;
            }
        }
        else if (op == PAMI_SUM && dt == PAMI_DOUBLE && count < MAX_IMATH_COUNT)
        {
	      for (idx = 0; idx < nsrcranks; ++idx)
          {
            inline_math_dsum (reducebuf, src1, src2[idx], count);
            src1 = reducebuf;
          }
        }
        else if (op == PAMI_MAX && dt == PAMI_DOUBLE && count < MAX_IMATH_COUNT)
        {
	      for (idx = 0; idx < nsrcranks; ++idx)
                {
                  inline_math_dmax (reducebuf, src1, src2[idx], count);
                  src1 = reducebuf;
                }
        }
        else if (op == PAMI_MIN && dt == PAMI_DOUBLE && count < MAX_IMATH_COUNT)
        {
	      for (idx = 0; idx < nsrcranks; ++idx)
          {
            inline_math_dmin (reducebuf, src1, src2[idx], count);
            src1 = reducebuf;
          }
        }
        else
        {
	      for (idx = 0; idx < nsrcranks; ++idx)
          {
            void * bufs[2];
            bufs[0] = src1;
            bufs[1] = src2[idx];

            TRACE_HEXDATA((char*)bufs[0], count*_acache.getSizeOfType());
            TRACE_HEXDATA((char*)bufs[1], count*_acache.getSizeOfType());
            TRACE_HEXDATA((char*)reducebuf, 1); // Just to trace the input pointer
            _reduceFunc( reducebuf, bufs, 2, count);

            TRACE_HEXDATA((char*)reducebuf, count*_acache.getSizeOfType());
            src1 = reducebuf;
          }
        }
      }

      for (idx = 0; idx < nsrcranks; idx ++) {
        pwq = _acache.getPhasePipeWorkQueues(_curPhase, idx);
        pwq->reset_nosync();
      }
      
      if (_curPhase == _endPhase)
      {
        if(_usrrcvbuf)//SSS: rtype is not contig
        {
          PAMI_Type_transform_data(_tmprcvbuf, PAMI_TYPE_BYTE, 0,
                                   _usrrcvbuf, getRtype(), 0,
                                   _acache.getBytes(), PAMI_DATA_COPY, NULL);
        }

        rc = PAMI_SUCCESS;

        _isSendDone = false; //Process an early arrival packet
        _initialized = false; //Call application done callback
        _earlyArrival = true;

        if (_cb_done)
          _cb_done (_context, _clientdata, PAMI_SUCCESS);

        return rc;
      }

      TRACE_FORMAT("<%p>_curPhase %d,_endPhase %d,_curIdx %d nsrcranks %d", this, _curPhase, _scache.getEndPhase(), _curIdx, nsrcranks);

      nsrcranks = _scache.getNumSrcRanks(_curPhase + 1);
      _curIdx = 0; //Start the next phase
      _curPhase ++;

      PAMI::Topology *dst_topology   = _scache.getDstTopology(_curPhase); 
      if (dst_topology->size() > 0)
      {
        sendMessage (reducebuf, _acache.getBytes(), dst_topology,
                       _curPhase);
	  
        //If the send has not immediately completed wait for it to
        //complete
        if (!_isSendDone)
          break;
      }
    }
  
 fn_end:
  _src1 = (char *)src1;
  _curnsrc = nsrcranks;
  _inAdvance = false;
  TRACE_FN_EXIT();
  return rc;
}

template <class T_Conn, bool T_Single>
inline pami_result_t CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::advance_single ()
{
  TRACE_FN_ENTER();
  TRACE_FORMAT("<%p>_curPhase %d,_endPhase %d, lastReducePhase %d, _curIdx %d, nsrcranks %u", this,
	       _curPhase, _endPhase,_scache.getLastReducePhase(), _curIdx, _scache.getNumSrcRanks(_curPhase));
  
  CCMI_assert_debug (_initialized);
  pami_result_t  rc = PAMI_EAGAIN;

  _inAdvance = true;
  
  char * reducebuf = _reducebuf;
  size_t msg_bytes = _acache.getBytes();
  pami_op op = _acache.getOp();
  pami_dt dt = _acache.getDt();
  unsigned count = _acache.getCount();
  TRACE_FORMAT("<%p>op %u, dt %u, count %u", this,
	       op, dt, count);
  
  void *src1 = _src1;
  unsigned nsrcranks = _curnsrc;

  while (_curPhase <= _endPhase)
  {
    PAMI::PipeWorkQueue *pwq = NULL;

    if (nsrcranks > 0) {
      pwq = _acache.getPhasePipeWorkQueues(_curPhase, 0);
	

      //fprintf (stderr, "bytes available %ld bytes expected %ld phase %d\n",
      // pwq->bytesAvailableToConsume(),
      // msg_bytes,
      // _curPhase);

      if (pwq->bytesAvailableToConsume() < msg_bytes)
      {
        TRACE_FN_EXIT();
        goto fn_end;
      }


      char *src2 = _acache.getPhaseRecvBufs (_curPhase, 0);
      if (_curPhase <= _lastReducePhase)
      {
        if (op == PAMI_SUM && dt == PAMI_SIGNED_INT)
        {
          inline_math_isum (reducebuf, src1, src2, count);
          src1 = reducebuf;
        }
        else if (op == PAMI_SUM && dt == PAMI_DOUBLE && count < MAX_IMATH_COUNT)
        {
          inline_math_dsum (reducebuf, src1, src2, count);
          src1 = reducebuf;
        }
        else if (op == PAMI_MAX && dt == PAMI_DOUBLE && count < MAX_IMATH_COUNT)
        {
          inline_math_dmax (reducebuf, src1, src2, count);
          src1 = reducebuf;
        }
        else if (op == PAMI_MIN && dt == PAMI_DOUBLE && count < MAX_IMATH_COUNT)
        {
          inline_math_dmin (reducebuf, src1, src2, count);
          src1 = reducebuf;
        }
        else
        {
          void * bufs[2];
          bufs[0] = src1;
          bufs[1] = src2;
          TRACE_HEXDATA((char*)bufs[0], count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)bufs[1], count*_acache.getSizeOfType());
          TRACE_HEXDATA((char*)reducebuf, 1); // Just to trace the input pointer
          _reduceFunc( reducebuf, bufs, 2, count);

          TRACE_HEXDATA((char*)reducebuf, count*_acache.getSizeOfType());
          src1 = reducebuf;
        }
      }

      pwq->reset_nosync();
    }

    if (_curPhase == _endPhase)
    {
      if(_usrrcvbuf)//SSS: rtype is not contig
      {
        PAMI_Type_transform_data(_tmprcvbuf, PAMI_TYPE_BYTE, 0,
                                 _usrrcvbuf, getRtype(), 0,
                                 _acache.getBytes(), PAMI_DATA_COPY, NULL);
      }
      rc = PAMI_SUCCESS;
      _isSendDone = false; //Process an early arrival packet
      _initialized = false;
      _earlyArrival = true;

      // Call application done callback
      if (_cb_done)
      {
        _cb_done (_context, _clientdata, PAMI_SUCCESS);
      }

      break;
    }

    nsrcranks = _scache.getNumSrcRanks(_curPhase + 1);
    _curPhase ++;

    PAMI::Topology *dst_topology   = _scache.getDstTopology(_curPhase);
    if (dst_topology->size() > 0)
    {
      sendMessage (reducebuf, _acache.getBytes(), dst_topology,
                       _curPhase);
	  
      //If the send has not immediately completed wait for it to
      //complete
      if (!_isSendDone)
        break;
    }
  }
  
 fn_end:
  _src1 = (char *)src1;
  _curnsrc = nsrcranks;
  _inAdvance = false;
  TRACE_FN_EXIT();
  return rc;
}



///
///  \brief Send the next message by calling the msend interface
///
template <class T_Conn, bool T_Single>
inline void CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::sendMessage
(char                   * buf,
 unsigned                 bytes,
 PAMI::Topology         * dst_topology,
 unsigned                 sphase)
{
  TRACE_FN_ENTER();
  CCMI_assert (dst_topology->size() > 0);

  PAMI::PipeWorkQueue *pwq = _acache.getDestPipeWorkQueue();
  pwq->configure(buf, bytes, bytes);  

  _msend.connection_id = _acache.getPhaseSendConnectionId (sphase);
  _msend.src              = (pami_pipeworkqueue_t *) pwq;
  _msend.dst_participants = (pami_topology_t *) _scache.getDstTopology(sphase);
  
  //the message has been sent and send state slot is unavailable
  _isSendDone = false;
  _sndInfo._phase     = sphase;
  
  TRACE_FORMAT("<%p>connid %#X curphase:%#X "
               "bytes:%#X destPe:%#X ndst %zu cData:%p ",
               this,
               _acache.getPhaseSendConnectionId (sphase),
               sphase, bytes, dst_topology->index2Endpoint(0), dst_topology->size(),
               &_sndInfo);


  _native->multicast (&_msend);
  TRACE_FN_EXIT();
}


////////////////////////////////////////////////////////
///
///  Public methods that can be called externally
///
////////////////////////////////////////////////////////

template <class T_Conn, bool T_Single>
inline void CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::start()
{
  TRACE_FN_ENTER();
  TRACE_FORMAT("<%p>", this);

  //CCMI_assert (_srcbuf != NULL);
  _initialized = true;
  _isSendDone = true;
  CCMI_assert_debug (_curPhase != (unsigned) -1);

  // Skip bogus initial phase(s)
  while (!_scache.getNumDstRanks(_curPhase) && !_scache.getNumSrcRanks(_curPhase))
    ++ _curPhase;

  unsigned ndstranks = _scache.getNumDstRanks (_curPhase);
  if (ndstranks) {
    PAMI::Topology *dst_topology   = _scache.getDstTopology(_curPhase);
    sendMessage ((char*)_srcbuf, _acache.getBytes(), dst_topology, _curPhase);
  }
  else
    advance ();
  TRACE_FN_EXIT();
}

template <class T_Conn, bool T_Single>
inline void CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::notifyRecv
( unsigned               peer,
  const pami_quad_t    & info,
  PAMI::PipeWorkQueue ** rcvpwq,
  pami_callback_t      * cb_done )
{
  TRACE_FN_ENTER();

  ExtCollHeaderData *cdata = (ExtCollHeaderData*) &info;
  CCMI_assert_debug(cdata);

  TRACE_FORMAT ("notifyRecv:<%p> phase:%u root:%u local root:%u src %u,_state->getBytes():%u _state->getPipelineWidth():%u \n",
		this,
		cdata->_phase, 
		(int) cdata->_root,
		_scache.getRoot(),
		peer,
		_acache.getBytes(),
		_acache.getPipelineWidth());

  CCMI_assert_debug(cdata->_comm == _sndInfo._comm);
  CCMI_assert_debug(cdata->_root == (unsigned) _scache.getRoot());
  CCMI_assert_debug(cdata->_phase >= (unsigned)_scache.getStartPhase());
  
  if (cdata->_phase > _scache.getEndPhase())
    cdata->_phase = _scache.getEndPhase();

#ifdef CCMI_DEBUG
  if (_scache.getNumSrcRanks(cdata->_phase) > 0)
    {
      // Schedule misbehavior patch:
      // src and dst phases *should* match but some schedules
      // send (dst) in phase n and receive (src) in phase n+1.
      // Allow this, I guess, by looking for the next src phase
      // and assuming that's the one we want.  This only works if
      // there are no src's in the specified phase.  If there are,
      // then we expect a match.
      while ((_scache.getNumSrcRanks(cdata->_phase) == 0) &&
             (cdata->_phase < (unsigned)_scache.getEndPhase()))
        {
          TRACE_FORMAT("<%p>no src in phase %#X",
                     this, cdata->_phase);
          cdata->_phase++;
        }
    }
#endif

  CCMI_assert(_scache.getNumSrcRanks(cdata->_phase) > 0);
  
  int srcPeIndex = -1;
  if (T_Single)
    srcPeIndex = 0;
  else {
    PAMI::Topology *srctopology = _scache.getSrcTopology(cdata->_phase);
    srcPeIndex = srctopology->endpoint2Index (peer);
    CCMI_assert (srcPeIndex >= 0);
  }

  PAMI::PipeWorkQueue *pwq = _acache.getPhasePipeWorkQueues(cdata->_phase, srcPeIndex);
  //pwq->reset(); //reset in AllreuceBaseExec::advance

  * rcvpwq    = pwq;

  cb_done->function   =  staticNotifyReceiveDone;
  cb_done->clientdata = this;

  TRACE_FN_EXIT();
}

template <class T_Conn, bool T_Single>
inline void CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::notifySendDone ()
{
  TRACE_FN_ENTER();
  // update state
  TRACE_FORMAT("<%p>cur phase %#X",
	       this, _curPhase);

  _isSendDone = true;
  //Call advance
  if (!_inAdvance)
    advance ();
  TRACE_FN_EXIT();
}

template <class T_Conn, bool T_Single>
inline PAMI_Request_t *
CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::notifyRecvHead
(const pami_quad_t     * info,
 unsigned               count,
 unsigned               conn_id,
 unsigned               peer,
 unsigned               sndlen,
 void                 * arg,
 size_t               * rcvlen,
 pami_pipeworkqueue_t ** rcvpwq,
 PAMI_Callback_t       * cb_done)
{
  TRACE_FN_ENTER();
  CCMI_assert_debug(count == 1);
  *rcvlen    = sndlen;
  AllreduceBaseExec<T_Conn, T_Single>::notifyRecv
    (peer,
     *info, 
     (PAMI::PipeWorkQueue **)rcvpwq, 
     cb_done);
  
  TRACE_FN_EXIT();
  return NULL; 
}

template <class T_Conn, bool T_Single>
inline void CCMI::Executor::AllreduceBaseExec<T_Conn, T_Single>::postReceives ()
{
  TRACE_FN_ENTER();
  // post receives for each expected incoming message
  _postReceives = true;
  
  CCMI_abort(); //Post recieves not supported here
  
  TRACE_FORMAT("<%p>", this);
  TRACE_FN_EXIT();
}

#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG

#include "algorithms/executor/PipelinedAllreduce.h"

#endif /* __simple_allreduce_executor_h__ */
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
