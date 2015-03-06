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

#ifndef __pipelined_allreduce_executor_h__
#define __pipelined_allreduce_executor_h__

#include <pami.h>
#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/connmgr/ConnectionManager.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "algorithms/executor/ScheduleCache.h"
#include "algorithms/executor/AllreduceCache.h"
#include "math_optimath.h"
#include "math/math_coremath.h"

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"

#include "util/trace.h"

#ifdef CCMI_TRACE_ALL
#define DO_TRACE_ENTEREXIT 1
#define DO_TRACE_DEBUG     1
#else
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0
#endif

//////////////////////////////////////////////////////////////////////
/// Pipelined Allreduce Template                                   ///
/// To keep a simple design we only support reduce+bcast flavor of ///
/// allreduce. Currently we also only support one source per phase ///
//////////////////////////////////////////////////////////////////////

namespace CCMI
{
  namespace Executor
  {
    template <class T_Conn>
      class PipelinedAllreduce : public AllreduceBaseExec<T_Conn, true>    
    {
    private:
      /// \brief Static function to be passed into the done of multisend send
      static void staticPipeNotifySendDone (pami_context_t context, void *cd, pami_result_t err)
      {
	TRACE_FN_ENTER();
	//fprintf(stderr, "Send callback calling process done\n");
	((PipelinedAllreduce<T_Conn> *)(cd))->processDone();
	TRACE_FN_EXIT();
      }

      /// \brief Static function to be passed into the done of multisend send
      static void staticPipeNotifyRecvDone (pami_context_t context, void *cd, pami_result_t err)
      {
	TRACE_FN_ENTER();
	//fprintf(stderr, "Recv callback calling process done\n");
	((PipelinedAllreduce<T_Conn> *)(cd))->processDone();
	TRACE_FN_EXIT();
      }

      static const uint32_t MaxNSums = 24;

    protected:  

      void processDone ()
      {
        _donecount --;	
	//fprintf(stderr, "In process done %d\n", _donecount);		
        if(_donecount <= 0 && this->_cb_done)
        {
	  //fprintf(stderr, "All Done for color %d\n", this->_acache.getColor());
	  this->_initialized = false; //Call application done callback
          this->_cb_done (this->_context, this->_clientdata, PAMI_SUCCESS); 
        }
      }

      //Squeeze local state in one cache line
      unsigned         _curRecvIdx;
      unsigned         _curRecvChunk;
      unsigned         _minbytes;
      unsigned         _curcount;
      unsigned         _curbytes;
      unsigned         _donecount;
      unsigned         _nSums;
      PAMI::PipeWorkQueue *_dstpwq;
      PAMI::PipeWorkQueue *_bcastpwq;
      coremath         _reduceFunc3;
      coremath         _reduceFunc4;
      coremath         _reduceFunc8;
      coremath         _myMathFn;
      void           * _buflist[MaxNSums]; //Permit 24 sums
      unsigned         _bytesProduced[MaxNSums];
      coremath         _myMathFnTable[12];

      ///
      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    sum count
      /// \param [in]    network sums
      ///
      static void nway_sum    (void          * dstbuf, 
			       void         ** buflist,
			       int             nsum,
			       int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	//printf("Execute nway_sum for %d sums %d count\n", nsum, count);

	CCMI_assert (nsum >= 15); //uptill 14 in direct impl

	//for (int i = 0; i < nsum; ++i)
	//printf("src[%d][0] = %g\n", i, *(double*)(buflist[i]));

	int start = 0;
	void *dst = dstbuf;
	exec->_reduceFunc8(dst, buflist, 8, count);
	start += 8;
	nsum -= 8;

	//printf("dst[0] = %g\n", *(double*)dst);

	void *buf[8];
	buf[0] = dst;
	while (nsum >= 7) {
	  buf[1] = buflist[start + 0];
	  buf[2] = buflist[start + 1];
	  buf[3] = buflist[start + 2];
	  buf[4] = buflist[start + 3];
	  buf[5] = buflist[start + 4];
	  buf[6] = buflist[start + 5];
	  buf[7] = buflist[start + 6];
	  exec->_reduceFunc8(dst, buf, 8, count);
	  start += 7;
	  nsum -= 7;
	}

	//printf("dst[0] = %g\n", *(double*)dst);       

	//four way sums
	while(nsum >= 3) {
	  buf[1] = buflist[start + 0];
	  buf[2] = buflist[start + 1];
	  buf[3] = buflist[start + 2];
	  exec->_reduceFunc4(dst, buf, 4, count);
	  start += 3;
	  nsum -= 3;
	}

	//two way sums
	while(nsum) {
	  buf[1] = buflist[start];
	  exec->_reduceFunc(dst, buf, 2, count);
	  start ++;
	  nsum --;
	}

	//printf("dst[0] = %g\n", *(double*)dst);	
      }

      ///
      /// \brief compute an 5-way sum
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void five_way_sum (void          * dstbuf, 
				void         ** buflist,
				int             nsum,
				int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[4]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc4(dst, buflist, 4, count);
	buf[0] = dst;  
	buf[1] = buflist[4];
	exec->_reduceFunc(dst,  buf, 2, count);
      }

      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void six_way_sum (void          * dstbuf, 
			       void         ** buflist,
			       int             nsum,
			       int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[4]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc4(dst, (void**)buflist, 4, count);	
	buf[0] = dst;  
	buf[1] = buflist[4];
	buf[2] = buflist[5];
	exec->_reduceFunc3(dst, (void**)buf, 3, count);
      }

      ///
      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void seven_way_sum (void          * dstbuf, 
				 void         ** buflist,
				 int             nsum,
				 int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[4]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc4(dst, buflist, 4, count);	
	buf[0] = dst;  
	buf[1] = buflist[4];
	buf[2] = buflist[5];
	buf[3] = buflist[6];
	exec->_reduceFunc4(dst, buf, 4, count);
      }

      ///
      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void nine_way_sum (void          * dstbuf, 
				void         ** buflist,
				int             nsum,
				int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[2]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc8(dst, buflist, 8, count);	

	buf[0] = dst;  
	buf[1] = buflist[8];
	exec->_reduceFunc(dst, buf, 2, count);
      }

      ///
      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void ten_way_sum (void          * dstbuf, 
			       void         ** buflist,
			       int             nsum,
			       int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[4]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc8(dst, buflist, 8, count);	
	buf[0] = dst;  
	buf[1] = buflist[8];
	buf[2] = buflist[9];
	exec->_reduceFunc3(dst, buf, 3, count);
      }

      ///
      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void eleven_way_sum (void          * dstbuf, 
				  void         ** buflist,
				  int             nsum,
				  int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[4]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc8(dst, buflist, 8, count);	
	buf[0] = dst;  
	buf[1] = buflist[8];
	buf[2] = buflist[9];
	buf[3] = buflist[10];
	exec->_reduceFunc4(dst, buf, 4, count);
      }

      ///
      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void twelve_way_sum (void          * dstbuf, 
				  void         ** buflist,
				  int             nsum,
				  int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[4]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc8(dst, buflist, 8, count);	
	buf[0] = dst;  
	buf[1] = buflist[8];
	buf[2] = buflist[9];
	buf[3] = buflist[10];
	exec->_reduceFunc4(dst, buf, 4, count);

	buf[1] = buflist[11];	
	exec->_reduceFunc(dst, buf, 2, count);
      }

      ///
      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void thirteen_way_sum (void          * dstbuf, 
				    void         ** buflist,
				    int             nsum,
				    int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[4]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc8(dst, buflist, 8, count);	
	buf[0] = dst;  
	buf[1] = buflist[8];
	buf[2] = buflist[9];
	buf[3] = buflist[10];
	exec->_reduceFunc4(dst, buf, 4, count);

	buf[1] = buflist[11];	
	buf[2] = buflist[12];	
	exec->_reduceFunc3(dst, buf, 3, count);
      }

      ///
      /// \brief compute an n-way sum
      /// \param [inout] summed buffer
      /// \param [in]    vector of network buffers
      /// \param [in]    network sums
      /// \param [in]    sum count
      ///
      static void fourteen_way_sum (void          * dstbuf, 
				    void         ** buflist,
				    int             nsum,
				    int             count) 
      {
	//We pass in the allredue executor in the buflist to keep the
	//function signature same as coremath
	PipelinedAllreduce<T_Conn> * exec = 
	  (PipelinedAllreduce<T_Conn> *) buflist[nsum];

	void *buf[4]; //we use max 4-way sums
	void *dst = dstbuf;
	exec->_reduceFunc8(dst, buflist, 8, count);	
	buf[0] = dst;  
	buf[1] = buflist[8];
	buf[2] = buflist[9];
	buf[3] = buflist[10];
	exec->_reduceFunc4(dst, buf, 4, count);

	buf[1] = buflist[11];	
	buf[2] = buflist[12];	
	buf[3] = buflist[13];	
	exec->_reduceFunc4(dst, buf, 4, count);
      }

    public: 
      /// Default Constructor
      PipelinedAllreduce<T_Conn>() : AllreduceBaseExec<T_Conn, true>(),
	_curRecvIdx ((unsigned) -1), _curRecvChunk((unsigned) -1)
      {
        _donecount = 0;
        this->_initialized = false;
        this->_postReceives = false;
      }

      PipelinedAllreduce<T_Conn>(Interfaces::NativeInterface    * native,
				 T_Conn                         * connmgr,
				 const unsigned                   commID):
	AllreduceBaseExec<T_Conn, true>(native, connmgr, commID, true),
	_curRecvIdx ((unsigned) -1), _curRecvChunk((unsigned) -1), 
	_donecount (0)
      {
	//fprintf(stderr, "PipelinedAllreduce Constructor\n");
	this->_postReceives = true;
	this->_enablePipelining = true;

	_myMathFnTable[0]  = five_way_sum; 
	_myMathFnTable[1]  = six_way_sum;
	_myMathFnTable[2]  = seven_way_sum;
	_myMathFnTable[3]  = NULL; 
	_myMathFnTable[4]  = nine_way_sum;
	_myMathFnTable[5]  = ten_way_sum;
	_myMathFnTable[6]  = eleven_way_sum; 
	_myMathFnTable[7]  = twelve_way_sum;
	_myMathFnTable[8]  = thirteen_way_sum;
	_myMathFnTable[9]  = fourteen_way_sum; 
	_myMathFnTable[10] = nway_sum;      
	_myMathFnTable[11] = NULL;
      }

      virtual ~PipelinedAllreduce<T_Conn> () 
      {
	TRACE_FN_ENTER();
	//fprintf(stderr, "Pipelined Allreduce destructor\n");
	TRACE_FN_EXIT();
      }

      pami_result_t advance ();

      /// start allreduce
      virtual void start();
	
      virtual void reset ()
      {
	////// AllreduceBase minimal reset ////////

	this->_acache.setDstBuf (this->_dstbuf);
	//Make sure we are not reseting a live executor
	CCMI_assert (this->_initialized == false);

	bool flag;
	if(this->_scache.getRoot() == -1)
	  flag = this->_scache.init(ALLREDUCE_OP);
	else
	  flag = this->_scache.init(REDUCE_OP);
	
	this->_acache.reset (flag, false);

	//Send done is only sent to true after the first send
	this->_endPhase = this->_scache.getEndPhase();
	this->_lastReducePhase = this->_scache.getLastReducePhase();

	if ((this->_scache.getRoot() == -1) || (this->_scache.getRoot() == (int)this->_native->endpoint()))
	  this->_reducebuf = this->_dstbuf;
	else
	  //Reduce operation and I am not the root
	  this->_reducebuf = this->_acache.getTempBuf();	  
	
	////// End AllreduceBase minimal reset //////////	

	//Assume one recv per phase only
	_nSums = this->_scache.getNumActiveCombinePhases() + 1;
	CCMI_assert (_nSums < MaxNSums-1); //we leave one for local

	if (this->_scache.getRoot() == -1)
	  //In allredcue, we must receive broadcast
	  CCMI_assert(this->_scache.getNumActiveRecvPhases() >= 1);

	_dstpwq       = NULL;
	_bcastpwq     = NULL;	
	_donecount    = 0;
        _curRecvChunk = 0;
	if (_nSums <= 1)
	  //We dont have any sums to do
	  _curRecvChunk = this->_acache.getLastChunk() + 1;
	else  { //we have to do sums, initialize summing state
	  _curRecvIdx   = 0;
	  unsigned pipe_width = this->_acache.getPipelineWidth(); 
	  unsigned last_chunk = this->_acache.getLastChunk();  
	  _minbytes     = pipe_width;
	  _curbytes     = pipe_width;
	  _curcount     = this->_acache.getFullChunkCount();  
	  if (last_chunk == 0) {
	    _curbytes = _minbytes = this->_acache.getBytes();
	    _curcount = this->_acache.getLastChunkCount();  
	  }

	  this->_reduceFunc = MATH_OP_FUNCS(this->_acache.getDt(), 
					    this->_acache.getOp(), 2);
    if(this->_reduceFunc == NULL) 
    {
      unsigned        sizeOfType;
      coremath        func ;
      CCMI::Adaptor::Allreduce::getReduceFunction(this->_acache.getDt(), 
                        this->_acache.getOp(),
                        sizeOfType,
                        func );
      this->_reduceFunc = func;
      _reduceFunc3 = func;
      _reduceFunc4 = func;
      _reduceFunc8 = func;
      _myMathFnTable[3]  = _reduceFunc8; 
      _myMathFnTable[11] = func;
      if (_nSums <= 4) 
        _myMathFn  = func;
      else if (_nSums <= 16)
        _myMathFn = _myMathFnTable[_nSums - 5];
      else 
        _myMathFn = nway_sum;	
    }
    else
    {
      _reduceFunc3 = MATH_OP_FUNCS(this->_acache.getDt(), 
                 this->_acache.getOp(), 3);
      _reduceFunc4 = MATH_OP_FUNCS(this->_acache.getDt(), 
                 this->_acache.getOp(), 4);
      _reduceFunc8 = MATH_OP_FUNCS(this->_acache.getDt(), 
                 this->_acache.getOp(), 8);
      _myMathFnTable[3]  = _reduceFunc8; 
      _myMathFnTable[11] = MATH_OP_FUNCS(this->_acache.getDt(), 
                                         this->_acache.getOp(), 16);
      if (_nSums <= 4) 
        _myMathFn = MATH_OP_FUNCS(this->_acache.getDt(), 
                                  this->_acache.getOp(), _nSums);
      else if (_nSums <= 16)
        _myMathFn = _myMathFnTable[_nSums - 5];
      else 
        _myMathFn = nway_sum;	
    }
//	  CCMI_assert(this->_reduceFunc != NULL);
//	  CCMI_assert(_reduceFunc3 != NULL);
//	  CCMI_assert(_reduceFunc4 != NULL);

	}
      }      

      void postReceives ();
      
      void sendMessage (PAMI::PipeWorkQueue    * pwq,
			unsigned                 bytes,
			PAMI::Topology          * dst_topology,
			unsigned                 sphase);
      
    };  //-- PipelinedAllreduce
  };  //-- Executor
};  //-- CCMI

template <class T_Conn>
inline void CCMI::Executor::PipelinedAllreduce<T_Conn>::start()
{
  TRACE_FN_ENTER();

  this->_initialized = true; 

  unsigned bytes = this->_acache.getBytes();
  unsigned *phases = this->_scache.getActiveSendPhaseVec();
  unsigned nsp = this->_scache.getNumActiveSendPhases();
  CCMI_assert (nsp <= 2); //we must have 0-2 sends
  _donecount += nsp;

  _dstpwq = this->_acache.getDestPipeWorkQueue();
  if (_nSums <= 1)
    //We don't have a sum operation
    _dstpwq->configure((char*)this->_srcbuf, bytes, bytes);
  else if (_bcastpwq == NULL) { //root
    _bcastpwq = this->_acache.getBcastPipeWorkQueue();
    _bcastpwq->configure(this->_dstbuf, bytes, 0);
    _dstpwq = _bcastpwq;
  }    
  else //summing intermediate node
    //We have a sum and its reduce/allreduce
    _dstpwq->configure(this->_reducebuf, bytes, 0);

  unsigned idx = 0;
  PAMI::PipeWorkQueue *pwq = _dstpwq;
  while (idx < nsp) {
    unsigned p = phases[idx];        
    idx ++;
    if (p > this->_lastReducePhase)
      pwq = _bcastpwq;
    
    PAMI::Topology *dst_topology   = this->_scache.getDstTopology(p);    
    // printf("phase %d, ndstranks %d dstrank[0] %d\n", p, ndstranks,
    // dst_topology->index2Endpoint(0));    
    sendMessage (pwq, bytes, dst_topology, p);
  }
  
  //fprintf(stderr, "After PipelinedAllreduce::start()\n");
  TRACE_FN_EXIT();
}


///
///  \brief Send the next message by calling the msend interface
///
template <class T_Conn>
inline void CCMI::Executor::PipelinedAllreduce<T_Conn>::sendMessage
(PAMI::PipeWorkQueue                      * pwq,
 unsigned                                   bytes,
 PAMI::Topology                           * dst_topology,
 unsigned                                   sphase)
{
  TRACE_FN_ENTER();
  CCMI_assert (dst_topology->size() > 0);

  this->_msend.connection_id = this->_acache.getPhaseSendConnectionId(sphase);
  this->_msend.bytes         = bytes;
  this->_msend.src           = (pami_pipeworkqueue_t *) pwq;
  this->_msend.dst           = NULL;
  this->_msend.src_participants = (pami_topology_t *) &this->_selftopology;
  this->_msend.dst_participants = (pami_topology_t *) dst_topology;
  this->_msend.cb_done.function =  staticPipeNotifySendDone;
  this->_msend.cb_done.clientdata = this;    
  this->_msend.roles = SendRecvRole;
  
  this->_native->multicast (&this->_msend);
  TRACE_FN_EXIT();
}

template <class T_Conn>
pami_result_t CCMI::Executor::PipelinedAllreduce<T_Conn>::advance()
{
  TRACE_FN_ENTER();

  //this executor works via post recvs
  CCMI_assert (this->_initialized);
  unsigned last_chunk = this->_acache.getLastChunk();  

  if (unlikely(_curRecvChunk > last_chunk))
  {
    TRACE_FN_EXIT();
    return PAMI_SUCCESS;
  }

  do {
    do {      
      if (_bytesProduced[_curRecvIdx] < _minbytes) {
	_bytesProduced[_curRecvIdx] = this->_acache.getPipeWorkQueueByIdx(_curRecvIdx)->getBytesProduced();
	
	if (_bytesProduced[_curRecvIdx] < _minbytes)
	{
	  TRACE_FN_EXIT();
	  return PAMI_EAGAIN;
	}
      }
      _curRecvIdx ++;
    } while (_curRecvIdx < _nSums - 1);
    
    unsigned idx = 0;
    if (unlikely(_curRecvChunk == 0)) {
      while (idx < _nSums - 1) {
	if (_buflist[idx] == NULL) {
	  //Check if the M* reset the src buffer to the remote readonly buffer
	  PAMI::PipeWorkQueue *pwq = this->_acache.getPipeWorkQueueByIdx(idx);
	  _buflist[idx] = pwq->bufferToConsume();
	}
	idx ++;
      }
    }   

    _myMathFn(this->_reducebuf, _buflist, _nSums, _curcount); 
    
    //If we are here we did some math, update pwqs
    PAMI::Memory::sync(); 
    _dstpwq->produceBytes(_curbytes);

    idx = 0;
    while (idx < _nSums) {
      _buflist[idx] = (char *)_buflist[idx] + _curbytes;    
      idx++;
    }
    this->_reducebuf += _curbytes;

    _curRecvChunk ++;      
    _curRecvIdx = 0; 
    if (unlikely(_curRecvChunk == last_chunk)) {
      _curbytes = this->_acache.getBytes() - _minbytes;
      _curcount = this->_acache.getLastChunkCount();
    }
    _minbytes += _curbytes;
  } while (_curRecvChunk <= last_chunk);

  TRACE_FN_EXIT();
  return PAMI_SUCCESS;
}

template <class T_Conn>
inline void CCMI::Executor::PipelinedAllreduce<T_Conn>::postReceives()
{
  TRACE_FN_ENTER();
  // post receives for each expected incoming message
  this->_postReceives = true;
  
  pami_multicast_t mrecv;
  mrecv.msginfo     = NULL;
  mrecv.msgcount    = 0;
  mrecv.cb_done.function   = staticPipeNotifyRecvDone;
  mrecv.cb_done.clientdata = this;
  mrecv.bytes         = this->_acache.getBytes();
  mrecv.src           = NULL;
  mrecv.dst_participants = (pami_topology_t *) &this->_selftopology;     
  
  char **allrecv_bufs = this->_acache.getAllrecvBufs();
  unsigned idx = 0;
  unsigned pidx = 0;
  unsigned p    = 0;
  unsigned nrp = this->_scache.getNumActiveRecvPhases();
  unsigned ncp = this->_scache.getNumActiveCombinePhases();
  _donecount += nrp;

  _bytesProduced[_nSums - 1] = mrecv.bytes;
  _buflist [_nSums - 1]  = (char*) this->_srcbuf;
  _buflist [_nSums]      = (void*)this;

  //Assume only one recv per phase
  while (pidx < ncp) {
    p = this->_scache.getActiveRecvPhaseVec()[pidx];
    mrecv.src_participants =(pami_topology_t*)this->_scache.getSrcTopology(p);
    mrecv.roles = SendRecvRole;
    
    PAMI::PipeWorkQueue *pwq = this->_acache.getPipeWorkQueueByIdx(idx);
    pwq->reset_nosync();
    mrecv.dst = (pami_pipeworkqueue_t *) pwq;    

    _buflist[idx] = allrecv_bufs[idx];
    _bytesProduced[idx] = 0;
    //For local reduce we directly read peer's memory
    if (this->_acache.getSrcIsPeerVec()[idx]) {
      mrecv.roles = RecvReadOnlyRole;
      _buflist[idx] = NULL;
    }
    mrecv.connection_id = this->_acache.getRecvConnectionIdVec()[idx];

    this->_native->multicast(&mrecv);
    pidx ++;
    idx ++;     
  }

  if (nrp > ncp) {
    //this is the broadcast
    p = this->_scache.getActiveRecvPhaseVec()[pidx];
    mrecv.src_participants =(pami_topology_t*)this->_scache.getSrcTopology(p);
    mrecv.roles = SendRecvRole;
    
    PAMI::PipeWorkQueue *pwq = this->_acache.getBcastPipeWorkQueue();
    pwq->configure(this->_dstbuf, this->_acache.getBytes(), 0); 	
    _bcastpwq = pwq;
    mrecv.dst = (pami_pipeworkqueue_t *) pwq;    
    mrecv.connection_id = this->_acache.getPhaseRecvConnectionId(p, 0);
    this->_native->multicast(&mrecv);
  }

  TRACE_FORMAT("<%p>", this);
  TRACE_FN_EXIT();
}

//#undef DO_TRACE_ENTEREXIT 
//#undef DO_TRACE_DEBUG     

#endif /* __simple_allreduce_executor_h__ */
