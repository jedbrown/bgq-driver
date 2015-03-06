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
 * \file algorithms/executor/Alltoallv.h
 * \brief ???
 */
#ifndef __algorithms_executor_Alltoallv_h__
#define __algorithms_executor_Alltoallv_h__


#include "algorithms/interfaces/Schedule.h"
#include "algorithms/interfaces/Executor.h"
#include "algorithms/connmgr/ConnectionManager.h"
#include "algorithms/interfaces/NativeInterface.h"
#include "util/BitVector.h"

// #define CONNECTION_ID_SHFT
#ifdef CONNECTION_ID_SHFT
  #define SHFT_BITS_PHASE 10
  #define SHFT_BITS_SRC   10
  #define SHFT_BITS      (SHFT_BITS_PHASE+SHFT_BITS_SRC)
#endif

//#define MAX_CONCURRENT 32
#define MAX_PARALLEL 20

#undef  EXECUTOR_DEBUG
#define EXECUTOR_DEBUG(x) //fprintf x

namespace CCMI
{
  namespace Executor
  {
    /*
     * Implements a alltoallv strategy which uses one network link.
     */

    template <class T_Alltoall_type>
    struct AlltoallVecType
    {
      // COMPILE_TIME_ASSERT(0==1);
    };

    template<>
    struct AlltoallVecType<pami_alltoall_t>
    {
      typedef int base_type;
    };

    template<>
    struct AlltoallVecType<pami_alltoallv_t>
    {
      typedef size_t base_type;
    };

    template<>
    struct AlltoallVecType<pami_alltoallv_int_t>
    {
      typedef int base_type;
    };

    template <class T_Alltoall_type>
    inline void setAlltoallVec(T_Alltoall_type *xfer, int *buflen, void *sbuf, void *scounts, void *sdisps,
                               void *rbuf, void *rcounts, void *rdisps, TypeCode **stype, TypeCode **rtype, 
                               unsigned gsize, int *in_place)
    {
// Clang performs syntax checks even for non-intantianated templates.
// This assertion fail needs to be deferred from syntax-checking to a possible instantianation attempt.
      COMPILE_TIME_ASSERT(sizeof(T_Alltoall_type) == 0);
    }

    template <>
    inline void setAlltoallVec<pami_alltoall_t> (pami_alltoall_t *xfer, int *buflen,
                                                 void *sbuf, void *scounts, void *sdisps, void *rbuf, void *rcounts, void *rdisps, TypeCode **stype, TypeCode **rtype, unsigned gsize, int *in_place)
    {
      (void)scounts;(void)sdisps;(void)rcounts;(void)rdisps;
      *((char **)rbuf)   = xfer->rcvbuf;
      *((char **)sbuf)   = xfer->sndbuf;
      *stype             = (TypeCode *)xfer->stype;
      *rtype             = (TypeCode *)xfer->rtype;
      *buflen            = xfer->rtypecount * (*rtype)->GetDataSize();
      *in_place          = 0;
      if(xfer->sndbuf == PAMI_IN_PLACE)
      {
        *in_place        = 1;
        size_t datalen   = (*buflen) * gsize;
        *((char **)sbuf) = (char *)__global.heap_mm->malloc (datalen);
        PAMI_assert_alwaysf(*((char **)sbuf) != NULL, "Failed to allocate memory at %s %d",__FILE__,__LINE__);
        *stype           = (TypeCode *)xfer->rtype;
        /* Using a memcpy here since we don't support non-contig. Once non-contig is supported
           this should be modified to be PAMI_Type_transform_data */
        memcpy(*((char **)sbuf), xfer->rcvbuf, datalen);
      }
      return;
    }

    template <>
    inline void setAlltoallVec<pami_alltoallv_t> (pami_alltoallv_t *xfer, int *buflen,
                                                  void *sbuf, void *scounts, void *sdisps, void *rbuf, void *rcounts, void *rdisps, TypeCode **stype, TypeCode **rtype, unsigned gsize, int *in_place)
    {
      *((char **)sbuf)      = xfer->sndbuf;
      *((size_t **)sdisps)  = xfer->sdispls;
      *((size_t **)scounts) = xfer->stypecounts;
      *((char **)rbuf)      = xfer->rcvbuf;
      *((size_t **)rdisps)  = xfer->rdispls;
      *((size_t **)rcounts) = xfer->rtypecounts;
      *buflen               = 0;
      *stype                = (TypeCode *)xfer->stype;
      *rtype                = (TypeCode *)xfer->rtype;
      *in_place             = 0;
      if(xfer->sndbuf == PAMI_IN_PLACE)
      {
        *in_place        = 1; /* = 1 allocations for this in place, so far */
        size_t * displ = *((size_t **)sdisps)   = (size_t *) xfer->rdispls;
        size_t dsize = (*rtype)->GetDataSize();
        size_t sumcnts   = 0;
        size_t highest_displ = displ[gsize-1];   /* Looking for highest buffer address */
        size_t highest_count =  xfer->rtypecounts[gsize-1];
        unsigned csize;
        for(csize = 0; csize < gsize; csize++)
        {
          if((displ[csize]+xfer->rtypecounts[csize]) > (highest_displ+highest_count))
          {
            highest_displ = displ[csize];
            highest_count = xfer->rtypecounts[csize];
          }
          sumcnts        += xfer->rtypecounts[csize];
          EXECUTOR_DEBUG((stderr,"Pairwise size %u displ[%u]=%zu, count[%u]=%zu\n",
                         gsize,csize,displ[csize],csize, xfer->rtypecounts[csize]));

        }
        size_t datalen      = sumcnts * dsize; /* calculated actual data length*/
        size_t totalbuflen = (highest_displ+highest_count) * dsize; /* calculated end of buffer */
        /* Don't assume recv buffer is continous, we can:
           - allocate new displacements for the new continuous buffer
           - use rdispls on copy of rbuffer 
           depending on which uses less memory */ 
        if((totalbuflen - datalen) > gsize*sizeof(size_t))
        {
          EXECUTOR_DEBUG((stderr,"Pairwise(size_t) allocate sdispls buflen %zu/%zu, datalen %zu\n",(highest_displ+highest_count), totalbuflen, datalen));
          *in_place        = 2; /* = 2 allocations for this in place */
          displ = *((size_t **)sdisps)   = (size_t *)__global.heap_mm->malloc (gsize*sizeof(size_t));
          PAMI_assert_alwaysf(displ != NULL, "Failed to allocate memory at %s %d",__FILE__,__LINE__);
          sumcnts = 0;
          for(csize = 0; csize < gsize; csize++)
          {
            displ[csize] = sumcnts; /* New displacements in new continuous buffer */
            sumcnts        += xfer->rtypecounts[csize];
          }
        }
        else
        {
          EXECUTOR_DEBUG((stderr,"Pairwise(size_t) do not allocate sdispls buflen %zu/%zu, datalen %zu\n",(highest_displ+highest_count), totalbuflen, datalen));

          datalen = totalbuflen; /* use entire buffer with holes */
        }

        EXECUTOR_DEBUG((stderr,"Pairwise allocate sbuf\n"));
        char* scbuf = *((char **)sbuf)    = (char *)__global.heap_mm->malloc(datalen);
        PAMI_assert_alwaysf(scbuf != NULL, "Failed to allocate memory at %s %d",__FILE__,__LINE__);
        *stype              = (TypeCode *)xfer->rtype;
        *((size_t **)scounts)  = xfer->rtypecounts;
        /* Using memcpys here since we don't support non-contig. Once non-contig is supported
           this should be modified to be PAMI_Type_transform_data */

        if(datalen == totalbuflen) /* copy entire buffer with possible holes */
          memcpy(scbuf, xfer->rcvbuf, datalen);
        else  /* Must memcpy each displ since it is non-continuous to continuous */
          for(csize = 0; csize < gsize; csize++)
            memcpy(scbuf+(displ[csize]*dsize),((char*)xfer->rcvbuf)+(xfer->rdispls[csize]*dsize),xfer->rtypecounts[csize]*dsize);
        return;
      }
      return;

    }
    template <>
    inline void setAlltoallVec<pami_alltoallv_int_t> (pami_alltoallv_int_t *xfer, int *buflen,
                                                      void *sbuf, void *scounts, void *sdisps, void *rbuf, void *rcounts, void *rdisps, TypeCode **stype, TypeCode **rtype, unsigned gsize, int *in_place)
    {
      *((char **)sbuf)   = xfer->sndbuf;
      *((int **)sdisps)  = xfer->sdispls;
      *((int **)scounts) = xfer->stypecounts;
      *((char **)rbuf)   = xfer->rcvbuf;
      *((int **)rdisps)  = xfer->rdispls;
      *((int **)rcounts) = xfer->rtypecounts;
      *buflen            = 0;
      *stype             = (TypeCode *)xfer->stype;
      *rtype             = (TypeCode *)xfer->rtype;
      *in_place          = 0;
      if(xfer->sndbuf == PAMI_IN_PLACE)
      {
        *in_place        = 1; /* = 1 allocations for this in place, so far */
        int * displ = *((int **)sdisps)   = (int *) xfer->rdispls;
        size_t dsize = (*rtype)->GetDataSize();
        size_t sumcnts   = 0;
        int highest_displ = displ[gsize-1];   /* Looking for highest buffer address */
        int highest_count =  xfer->rtypecounts[gsize-1];
        unsigned csize;
        for(csize = 0; csize < gsize; csize++)
        {
          if((displ[csize]+xfer->rtypecounts[csize]) > (highest_displ+highest_count))
          {
            highest_displ = displ[csize];
            highest_count = xfer->rtypecounts[csize];
          }
          sumcnts        += xfer->rtypecounts[csize];
          EXECUTOR_DEBUG((stderr,"Pairwise size %u displ[%u]=%u, count[%u]=%u\n",
                          gsize,csize,displ[csize],csize, xfer->rtypecounts[csize]));

        }
        size_t datalen      = sumcnts * dsize; /* calculated actual data length*/
        size_t totalbuflen = (highest_displ+highest_count) * dsize; /* calculated end of buffer */
        /* Don't assume recv buffer is continous, we can:
           - allocate new displacements for the new continuous buffer
           - use rdispls on copy of rbuffer 
           depending on which uses less memory */ 
        if((totalbuflen - datalen) > gsize*sizeof(int))
        {
          EXECUTOR_DEBUG((stderr,"Pairwise(int) allocate sdispls buflen %u/%zu, datalen %zu\n",(highest_displ+highest_count),totalbuflen, datalen));
          *in_place        = 2; /* = 2 allocations for this in place */
          displ = *((int **)sdisps)   = (int *)__global.heap_mm->malloc (gsize*sizeof(int));
          PAMI_assert_alwaysf(displ != NULL, "Failed to allocate memory at %s %d",__FILE__,__LINE__);
          sumcnts = 0;
          for(csize = 0; csize < gsize; csize++)
          {
            displ[csize] = sumcnts; /* New displacements in new continuous buffer */
            sumcnts        += xfer->rtypecounts[csize];
          }
        }
        else
        {
          EXECUTOR_DEBUG((stderr,"Pairwise(int) do not allocate sdispls buflen %u/%zu, datalen %zu\n",(highest_displ+highest_count),totalbuflen, datalen));
          datalen = totalbuflen; /* use entire buffer with holes */
        }

        EXECUTOR_DEBUG((stderr,"Pairwise(int) allocate sbuf\n"));
        char* scbuf = *((char **)sbuf)    = (char *)__global.heap_mm->malloc(datalen);
        PAMI_assert_alwaysf(scbuf != NULL, "Failed to allocate memory at %s %d",__FILE__,__LINE__);
        *stype              = (TypeCode *)xfer->rtype;
        *((int **)scounts)  = xfer->rtypecounts;
        /* Using memcpys here since we don't support non-contig. Once non-contig is supported
           this should be modified to be PAMI_Type_transform_data */

        if(datalen == totalbuflen) /* copy entire buffer with possible holes */
          memcpy(scbuf, xfer->rcvbuf, datalen);
        else  /* Must memcpy each displ since it is non-continuous to continuous */
          for(csize = 0; csize < gsize; csize++)
            memcpy(scbuf+(displ[csize]*dsize),((char*)xfer->rcvbuf)+(xfer->rdispls[csize]*dsize),xfer->rtypecounts[csize]*dsize);
        return;
      }

      return;
    }

    template<class T_ConnMgr, class T_Type>
    class AlltoallvExec : public Interfaces::Executor
    {
    public:

    protected:
      Interfaces::Schedule           * _comm_schedule;
      Interfaces::NativeInterface    * _native;
      T_ConnMgr                      * _connmgr;

      int                 _comm;
      int                 _buflen;
      char                *_sbuf;
      char                *_rbuf;
      TypeCode            *_stype;
      TypeCode            *_rtype;

      PAMI::PipeWorkQueue _pwq;
      PAMI::PipeWorkQueue _rpwq [MAX_PARALLEL];

      int                 _curphase;
      int                 _nphases;
      int                 _startphase;
      int                 _lphase;
      PAMI::BitVector     _rphase;

      int                 _maxsrcs;

      unsigned            _parindex;
      unsigned            _myindex;

      int                 _senddone;
      int                 _recvdone [MAX_PARALLEL];
      pami_endpoint_t     _self_ep;
      pami_endpoint_t     _par_ep;

      PAMI::Topology      _partopology;
      PAMI::Topology      _selftopology;
      PAMI::Topology      *_gtopology;

      CollHeaderData      _mldata;
      CollHeaderData      _mrdata;
      pami_multicast_t    _mlsend;
      pami_multicast_t    _mrsend;
      unsigned            _lconnid;
      unsigned            _rconnid;

      typedef typename AlltoallVecType<T_Type>::base_type basetype;

      basetype            *_sdisps;
      basetype            *_scounts;
      basetype            *_rdisps;
      basetype            *_rcounts;

      int                  _in_place; /* 0=not in place, 
                                         1=send buf copied, 
                                         2=send buf copied & sdispls copied */
      //Private method
      void             sendNext ();

    public:
      AlltoallvExec () :
      Interfaces::Executor (),
      _comm_schedule(NULL),
      _comm(-1),
      _sbuf(NULL),
      _rbuf(NULL),
      _curphase(-1),
      _nphases(0),
      _startphase(0),
      _sdisps(NULL),
      _scounts(NULL),
      _rdisps(NULL),
      _rcounts(NULL),
      _in_place(0)
      {
        TRACE_ADAPTOR((stderr, "<%p>Executor::AlltoallvExec()\n", this));
      }

      AlltoallvExec (Interfaces::NativeInterface  * mf,
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
      _rphase(gtopology->size()),
      _self_ep(mf->endpoint()),
      _partopology(),
      _selftopology(&_self_ep,1,PAMI::tag_eplist()),
      _gtopology(gtopology),
      _sdisps(NULL),
      _scounts(NULL),
      _rdisps(NULL),
      _rcounts(NULL),
      _in_place(0)
      {
        TRACE_ADAPTOR((stderr, "<%p>Executor::AlltoallvExec(...)\n", this));
        _clientdata        =  0;
        _buflen            =  0;
        _senddone          =  0;

        for(int i = 0; i < MAX_PARALLEL; ++i) _recvdone[i] = 0;

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

        info    =  (pami_quad_t*)((void*) & _mrdata);
        _mrsend.msginfo       =  info;
        _mrsend.msgcount      =  1;
        _mrsend.roles         = -1U;

      }

      virtual ~AlltoallvExec ()
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
        TRACE_ADAPTOR((stderr, "<%p>Executor::AlltoallvExec::setSchedule()\n", this));
        _comm_schedule = ct;

        // should get the number of phases from a real schedule
        _nphases    = _gtopology->size() - 1 + _gtopology->size() % 2;
        _startphase = 0;
        _curphase   = -1;
        _lphase     = 0;

        _myindex  = _gtopology->endpoint2Index(_native->endpoint());
        _parindex = getPartnerIndex(0, _gtopology->size(), _myindex);

        unsigned connection_id = (unsigned) - 1;

        if(_connmgr)
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

#ifdef CONNECTION_ID_SHFT
        //Override the connection id from the connection manager
        _lconnid = (cid << SHFT_BITS) + (_myindex << SHFT_BITS_SRC);
        _rconnid = (cid << SHFT_BITS) + (_myindex << SHFT_BITS_SRC);
#else
        _lconnid = cid;
        _rconnid = cid;
#endif

      }

      void  setBuffers (char *src, char *dst, int len)
      {
        TRACE_ADAPTOR((stderr, "<%p>Executor::AlltoallvExec::setInfo() src %p, dst %p, len %d, _pwq %p\n", this, src, dst, len, &_pwq));

        _buflen = len;
        _sbuf = src;
        _rbuf = dst;
      }

      void setVectors(T_Type *xfer)
      {
        setAlltoallVec<T_Type> (xfer, &_buflen, &_sbuf, &_scounts, &_sdisps, &_rbuf, &_rcounts,  &_rdisps, &_stype, &_rtype, this->_gtopology->size(), &_in_place);
        EXECUTOR_DEBUG((stderr, "setVector gets called, rbuf = %p, rdisp = %p, _rounts = %zu\n", _rbuf, _rdisps,(size_t) _rcounts);)
      }

      void  updateVectors(T_Type *xfer)
      {
        setAlltoallVec<T_Type> (xfer, &_buflen, &_sbuf, &_scounts, &_sdisps, &_rbuf, &_rcounts,  &_rdisps, &_stype, &_rtype, this->_gtopology->size(), &_in_place);
        EXECUTOR_DEBUG((stderr, "updateVector gets called, rbuf = %p, rdisp = %p, _rounts = %zu\n", _rbuf, _rdisps, (size_t)_rcounts);)
      }

      /// \todo: this should be moved to the schedule
      unsigned getPartnerIndex(unsigned uphase, unsigned utasks, unsigned umyindex)
      {

        int phase   = (int)uphase;
        int tasks   = (int)utasks;
        int myindex = (int)umyindex;

        int partner;

        if(tasks & 1)
        {
          partner = (tasks + 2 * phase - myindex) % tasks;

          if(partner == myindex)  partner = (unsigned) - 1;
        }
        else
        {
          tasks--;

          if(myindex == tasks)
            partner = phase;
          else
            partner = (tasks + 2 * phase - myindex) % tasks;

          if(partner == myindex)
            partner = tasks;
        }

        EXECUTOR_DEBUG((stderr, "phase %d, myindex %d, partner %d\n", phase, myindex, partner);)
        return(unsigned)partner;
      }

      size_t getSendLength(unsigned index)
      {
        EXECUTOR_DEBUG((stderr, "index = %d, scounts = %p, buflen = %d\n", index, _scounts, _buflen);)
        return(_scounts) ? _scounts[index] * _stype->GetDataSize() : _buflen;
      }

      size_t getRecvLength(unsigned index)
      {
        EXECUTOR_DEBUG((stderr, "index = %d, rcounts = %p, buflen = %d\n", index, _rcounts, _buflen);)
        return(_rcounts) ? _rcounts[index] * _rtype->GetDataSize() : _buflen;
      }

      size_t getSendDisp(unsigned index)
      {
        return(_sdisps) ? _sdisps[index] * _stype->GetExtent(): index * _buflen;
      }

      size_t getRecvDisp(unsigned index)
      {
        return(_rdisps) ? _rdisps[index] * _rtype->GetExtent(): index * _buflen;
      }

      PAMI::PipeWorkQueue *getSendPWQ(unsigned index)
      {
        size_t sleng = getSendLength(index);
        size_t sdisp = getSendDisp(index);
        _pwq.configure (_sbuf + sdisp, sleng, 0, _stype, _rtype);
        _pwq.produceBytes(sleng);
        EXECUTOR_DEBUG((stderr, "send index = %u, disp = %zu, leng = %zu\n", index, sdisp, sleng);)
        return &_pwq;
      }

      PAMI::PipeWorkQueue *getRecvPWQ(unsigned index, int phase)
      {
        size_t rleng = getRecvLength(index);
        size_t rdisp = getRecvDisp(index);
        _rpwq[phase % MAX_PARALLEL].configure (_rbuf + rdisp, rleng, 0, _stype, _rtype);
        EXECUTOR_DEBUG((stderr, "receive index = %u, phase = %d, disp = %zu, leng = %zu\n", index, phase, rdisp, rleng);)
        return &_rpwq[phase % MAX_PARALLEL];
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
        TRACE_MSG ((stderr, "<%p>Executor::AlltoallvExec::notifySendDone()\n", cookie));
        AlltoallvExec<T_ConnMgr, T_Type> *exec =  (AlltoallvExec<T_ConnMgr, T_Type> *) cookie;

        EXECUTOR_DEBUG((stderr, "notifySendDone for phase %d\n", exec->_curphase);)
        exec->_senddone = 1;

        if(exec->_recvdone[exec->_curphase % MAX_PARALLEL] == 1)
        {
          exec->_recvdone[exec->_curphase % MAX_PARALLEL] = 0;
          exec->_senddone = 0;
          exec->_curphase ++;
          exec->_parindex =  exec->getPartnerIndex(exec->_curphase, exec->_gtopology->size(), exec->_myindex);
          exec->sendNext();
        }
      }

      static void notifyRecvDone( pami_context_t   context,
                                  void           * cookie,
                                  pami_result_t    result )
      {
        TRACE_MSG ((stderr, "<%p>Executor::AlltoallvExec::notifyRecvDone()\n", cookie));
        AlltoallvExec<T_ConnMgr, T_Type> *exec =  (AlltoallvExec<T_ConnMgr, T_Type> *) cookie;

        EXECUTOR_DEBUG((stderr, "notifyRecvDone for phase %d\n", exec->_curphase);)
        exec->_recvdone[exec->_curphase % MAX_PARALLEL] = 1;

        if(exec->_senddone == 1)
        {
          exec->_recvdone[exec->_curphase % MAX_PARALLEL] = 0;
          exec->_senddone = 0;
          exec->_curphase ++;
          exec->_parindex =  exec->getPartnerIndex(exec->_curphase, exec->_gtopology->size(), exec->_myindex);
          exec->sendNext();
        }
      }

      static void notifyAvailRecvDone( pami_context_t   context,
                                       void           * cookie,
                                       pami_result_t    result )
      {
        TRACE_MSG ((stderr, "<%p>Executor::AlltoallvExec::notifyAvailRecvDone()\n", cookie));
        AlltoallvExec<T_ConnMgr, T_Type> *exec =  (AlltoallvExec<T_ConnMgr, T_Type> *) cookie;
        EXECUTOR_DEBUG((stderr, "notifyAvailRecvDone for phase %d\n", exec->_curphase);)

        if(exec->_curphase >= exec->_startphase)
          exec->sendNext();
      }

      static void notifyAvailSendDone( pami_context_t   context,
                                       void           * cookie,
                                       pami_result_t    result )
      {
        TRACE_MSG ((stderr, "<%p>Executor::AlltoallvExec::notifyAvailSendDone()\n", cookie));
        AlltoallvExec<T_ConnMgr, T_Type> *exec =  (AlltoallvExec<T_ConnMgr, T_Type> *) cookie;
        EXECUTOR_DEBUG((stderr, "notifyAvailSendDone for phase %d\n", exec->_curphase);)

        if(exec->_curphase >= exec->_startphase)
          exec->sendNext();
      }


    };  //-- AlltoallvExec
  };   //-- Executor
};  //-- CCMI

///
/// \brief start sending alltoallv data. Only active on the root node
///
template <class T_ConnMgr, class T_Type>
inline void  CCMI::Executor::AlltoallvExec<T_ConnMgr, T_Type>::start ()
{
  TRACE_ADAPTOR((stderr, "<%p>Executor::AlltoallvExec::start() count%d\n", this, _buflen));

  _lphase = _curphase  = _startphase;

  memcpy(_rbuf + getRecvDisp(_myindex), _sbuf + getSendDisp(_myindex), getRecvLength(_myindex));

  sendNext ();
}

template <class T_ConnMgr, class T_Type>
inline void  CCMI::Executor::AlltoallvExec<T_ConnMgr, T_Type>::sendNext ()
{

  EXECUTOR_DEBUG((stderr, "sendNext - curphase = %d, startphase = %d, and nphases = %d\n", _curphase, _startphase, _nphases);)

  if(_curphase == _startphase + _nphases)
  {
    if(_cb_done) _cb_done (NULL, _clientdata, PAMI_SUCCESS);
    if(_in_place)
    {
      EXECUTOR_DEBUG((stderr,"Pairwise free sbuf\n"));
      __global.heap_mm->free((void*)this->_sbuf); 
      if(_in_place == 2)
      {
        EXECUTOR_DEBUG((stderr,"Pairwise free sdispls\n"));
        __global.heap_mm->free((void*)this->_sdisps); 
      }
    }
    return;
  }

  // setup destination topology
  if(_parindex == (unsigned) - 1) // skip this phase
  {
    _lphase   ++;
    _curphase ++;

    if(_curphase == _startphase + _nphases)
    {
      if(_cb_done) _cb_done (NULL, _clientdata, PAMI_SUCCESS);
      if(_in_place)
      {
        EXECUTOR_DEBUG((stderr,"Pairwise free sbuf\n"));
        __global.heap_mm->free((void*)this->_sbuf); 
        if(_in_place == 2)
        {
          EXECUTOR_DEBUG((stderr,"Pairwise free sdispls\n"));
          __global.heap_mm->free((void*)this->_sdisps); 
        }
      }
      return;
    }

    _parindex         = getPartnerIndex(_curphase, _gtopology->size(), _myindex);
  }
  _par_ep = _gtopology->index2Endpoint(_parindex);
  new (&_partopology) PAMI::Topology(&_par_ep, 1, PAMI::tag_eplist());

  // send buffer available msg to left neighbor
  if(_lphase == _curphase)
  {
    _lphase ++;
    _mldata._phase             = _curphase + 1;
    _mldata._count             = -1;
    _mlsend.src_participants   = (pami_topology_t *) & _selftopology;
    _mlsend.dst_participants   = (pami_topology_t *) & _partopology;
    _mlsend.cb_done.function   = notifyAvailSendDone;
    _mlsend.cb_done.clientdata = this;
    _mlsend.src                = NULL;
    _mlsend.dst                = NULL;
    _mlsend.bytes              = 0;
#ifdef CONNECTION_ID_SHFT
    _mlsend.connection_id      = _lconnid + 1;
#else
    _mlsend.connection_id      = _lconnid;
#endif
    _native->multicast(&_mlsend);

    EXECUTOR_DEBUG((stderr, "phase %d, send buffer available msg to %d\n", _curphase, _partopology.index2Endpoint(0));)
  }
  else if(_rphase.get(_parindex)) // buffer available at the right neighbor
  {
    _rphase.clear(_parindex);
    _mrdata._phase             = _curphase;
    _mrdata._count             = 0; // indicating this is data message
    _mrsend.src_participants   = (pami_topology_t *) & _selftopology;
    _mrsend.dst_participants   = (pami_topology_t *) & _partopology;
    _mrsend.cb_done.function   = notifySendDone;
    _mrsend.cb_done.clientdata = this;
    _mrsend.src                = (pami_pipeworkqueue_t *)getSendPWQ(_parindex);
    _mrsend.dst                = NULL;
    _mrsend.bytes              = getSendLength(_parindex);
#ifdef CONNECTION_ID_SHFT
    _mrsend.connection_id      = _rconnid;
#else
    _mrsend.connection_id      = _rconnid;
#endif
    _native->multicast(&_mrsend);

    EXECUTOR_DEBUG((stderr, "phase %d, send data msg to %d\n", _curphase, _partopology.index2Endpoint(0));)
  }

  return;
}

template <class T_ConnMgr, class T_Type>
inline void  CCMI::Executor::AlltoallvExec<T_ConnMgr, T_Type>::notifyRecv
(unsigned             src,
 const pami_quad_t   & info,
 PAMI::PipeWorkQueue ** pwq,
 pami_callback_t      * cb_done)
{

  CollHeaderData *cdata = (CollHeaderData*) & info;

  if((int)cdata->_count == -1)
  {
#if ASSERT_LEVEL > 0
    unsigned pindex  = getPartnerIndex(cdata->_phase - 1, _gtopology->size(), _myindex);
    CCMI_assert(pindex != (unsigned) - 1);
    EXECUTOR_DEBUG((stderr, "notifyRecv - phase = %d, src = %d, expected %d\n", cdata->_phase - 1, src, _gtopology->index2Endpoint(pindex));)
    CCMI_assert(src == _gtopology->index2Endpoint(pindex));
#endif
    _rphase.set(_gtopology->endpoint2Index(src));
    *pwq = NULL;
    cb_done->function   = notifyAvailRecvDone;
    cb_done->clientdata = this;
  }
  else
  {
    EXECUTOR_DEBUG((stderr, "notifyRecv - data phase = %d, src = %d, expected %d\n", _curphase, src, _gtopology->index2Endpoint(_parindex));)
    CCMI_assert(cdata->_count == 0);
    CCMI_assert(src == _gtopology->index2Endpoint(_parindex));
    CCMI_assert(cdata->_phase == (unsigned)_curphase);
    *pwq = getRecvPWQ(_parindex, _curphase);
    cb_done->function   = notifyRecvDone;
    cb_done->clientdata = this;
  }

  return;
}

#undef  EXECUTOR_DEBUG
#define EXECUTOR_DEBUG(x) 

#endif
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
