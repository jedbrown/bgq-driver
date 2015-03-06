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
 * \file algorithms/protocols/tspcoll/LongAllreduce.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Allreduce.h"
#include "algorithms/protocols/tspcoll/Team.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#ifndef __APPLE__
#include <malloc.h>
#endif

//#define DEBUG_ALLREDUCE 1
#undef TRACE
#ifdef DEBUG_ALLREDUCE
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

/* ************************************************************************* */
/*                       Long allreduce constructor                          */
/* ************************************************************************* */
template <class T_NI>
xlpgas::Allreduce::Long<T_NI>::
Long (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  this->_tmpbuf = NULL;
  this->_tmpredbuf = NULL;
  this->_tmpbuflen = 0;
  this->_dbuf = NULL;
  this->_nelems = 0;
  this->_contig = 1;//SSS: Default is contiguous.
  for (this->_logMaxBF = 0; (1<<(this->_logMaxBF+1)) <= (int)this->_comm->size(); this->_logMaxBF++) ;
  int maxBF  = 1<<this->_logMaxBF;
  int nonBF  = this->_comm->size() - maxBF;
  int phase  = 0;

  /* -------------------------------------------- */
  /* phase 0: gather buffers from ranks > n2prev  */
  /* -------------------------------------------- */

  if (nonBF > 0)
    {
      /* send permission chits to potential senders */

      this->_dest    [phase] = this->_comm->index2Endpoint (this->ordinal() + maxBF);
      this->_sbuf    [phase] = ((int)this->ordinal() <  nonBF) ? &this->_dummy : NULL;
      this->_rbuf    [phase] = ((int)this->ordinal() >= maxBF) ? &this->_dummy : NULL;
      this->_postrcv [phase] = NULL;
      this->_sbufln  [phase] = 1;
      this->_rbufln  [phase] = 1;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      //printf("%d: in phase %d will send to  %d \n", XLPGAS_MYNODE, phase, _dest[phase].node);
      phase ++;

      /* send data */

      this->_dest    [phase] = this->_comm->index2Endpoint (this->ordinal() - maxBF);
      this->_sbuf    [phase] = NULL; /* send buffer not available */
      this->_rbuf    [phase] = NULL; /* receive buffer not available */
      this->_postrcv [phase] = ((int)this->ordinal() < nonBF)  ? cb_allreduce : NULL;
      this->_sbufln  [phase] = 0;    /* data length not available */
      this->_rbufln  [phase] = 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      //sprintf("%d: in phase %d will send to  %d \n", XLPGAS_MYNODE, phase, _dest[phase].node);
      phase ++;
    }

  /* -------------------------------------------- */
  /* butterfly phase                              */
  /* -------------------------------------------- */

  for (int i=0; i<this->_logMaxBF; i++)
    {
      /* send permission chits to senders */

      this->_dest    [phase] = this->_comm->index2Endpoint (this->ordinal() ^ (1<<i));
      this->_sbuf    [phase] = ((int)this->ordinal() < maxBF) ? &this->_dummy : NULL;
      this->_rbuf    [phase] = ((int)this->ordinal() < maxBF) ? &this->_dummy : NULL;
      this->_postrcv [phase] = NULL;
      this->_sbufln  [phase] = 1;
      this->_rbufln  [phase] = 1;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      //printf("%d: in phase %d will send to  %d \n", XLPGAS_MYNODE, phase, _dest[phase].node);
      phase ++;

      /* send data */

      this->_dest    [phase] = this->_comm->index2Endpoint (this->ordinal() ^ (1<<i));
      this->_sbuf    [phase] = NULL; /* send buffer not available */
      this->_rbuf    [phase] = NULL; /* receive buffer not available */
      this->_postrcv [phase] = ((int)this->ordinal() < maxBF) ? cb_allreduce : NULL;
      this->_sbufln  [phase] = 0;    /* data length not available */
      this->_rbufln  [phase] = 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      //printf("%d: in phase %d will send to  %d \n", XLPGAS_MYNODE, phase, _dest[phase].node);
      phase ++;
    }

  /* -------------------------------------------- */
  /*  last phase: rebroadcast to non-power-of-2   */
  /* -------------------------------------------- */

  if (nonBF > 0)
    {
      /* send permission slips */

      this->_dest    [phase] = this->_comm->index2Endpoint (this->ordinal() - maxBF);
      this->_sbuf    [phase] = ((int)this->ordinal() >= maxBF) ? &this->_dummy : NULL;
      this->_rbuf    [phase] = ((int)this->ordinal() < nonBF)  ? &this->_dummy : NULL;
      this->_postrcv [phase] = NULL;
      this->_sbufln  [phase] = 1;
      this->_rbufln  [phase] = 1;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      //printf("%d: in phase %d will send to  %d \n", XLPGAS_MYNODE, phase, _dest[phase].node);
      phase ++;

      /* send data */

      this->_dest    [phase] = this->_comm->index2Endpoint ((int)this->ordinal() + maxBF);
      this->_sbuf    [phase] = NULL; /* send buffer not available */
      this->_rbuf    [phase] = NULL; /* receive buffer not available */
      this->_postrcv [phase] = NULL;
      this->_sbufln  [phase] = 0; /* data length not available */
      this->_rbufln  [phase] = 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      //printf("%d: in phase %d will send to  %d \n", XLPGAS_MYNODE, phase, _dest[phase].node);
      phase ++;
    }

  this->_numphases    = phase;
  this->_phase        = this->_numphases;
  this->_sendcomplete = this->_numphases;
}

/* ************************************************************************* */
/*                     allreduce executor                                    */
/* ************************************************************************* */
template <class T_NI>
void xlpgas::Allreduce::Long<T_NI>::
cb_allreduce (CollExchange<T_NI> *coll, unsigned phase)
{
  xlpgas::Allreduce::Long<T_NI> * ar = (xlpgas::Allreduce::Long<T_NI> *) coll;

  //  int * dbuf   = (int *) ar->_dbuf;
  //  int * pbuf   = (int *) ar->_tmpbuf;
  //printf("%d: Add %d to already existent%d in phase %d\n", XLPGAS_MYNODE, pbuf[0], dbuf[0], phase);
  if(ar->_contig)
  {
    void * inputs[] = {ar->_dbuf, ar->_tmpbuf};
    ar->_cb_allreduce (ar->_dbuf, inputs, 2, ar->_nelems);
  }
  else
  {
    PAMI_Type_transform_data((void*)ar->_dbuf, ar->_rdt, 0,
                                             ar->_tmpredbuf, PAMI_TYPE_BYTE, 0, ar->_nelems * ar->_sdt->GetDataSize(),
                                             PAMI_DATA_COPY, NULL);

    void * inputs[] = {ar->_tmpredbuf, ar->_tmpbuf};
    ar->_cb_allreduce (ar->_tmpredbuf, inputs, 2, ar->_nelems);
    PAMI_Type_transform_data((void*)ar->_tmpredbuf, PAMI_TYPE_BYTE, 0,
                                             ar->_dbuf, ar->_rdt, 0, ar->_nelems * ar->_sdt->GetDataSize(),
                                             PAMI_DATA_COPY, NULL);
  }
}

/* ************************************************************************* */
/*                      start a long allreduce operation                     */
/* ************************************************************************* */
#define MAXOF(a,b) (((a)>(b))?(a):(b))
template <class T_NI>
void xlpgas::Allreduce::Long<T_NI>::reset (const void         * sbuf,
                     void               * dbuf,
                     pami_data_function   op,
                     TypeCode           * sdt,
                     size_t               nelems,
                     TypeCode           * rdt,
                     user_func_t        * uf)
{
  assert (sbuf != NULL);
  assert (dbuf != NULL);
  _sdt    = sdt;
  _rdt    = rdt;
  _dbuf   = dbuf;
  if(sbuf == PAMI_IN_PLACE)
  {
    sbuf = dbuf;
    _sdt = rdt;
  }
  xlpgas::CollExchange<T_NI>::reset();//to lock

  _uf = uf; //user function pointer
  /* --------------------------------------------------- */
  /*         copy source to destination if necessary     */
  /* --------------------------------------------------- */
  /*
    printf("L%d: long allreduce [teamid=%d rank=%d sizeworld=%d nelems=%d]\n",
	 XLPGAS_MYNODE,
	 _comm->commID(),
	 ordinal(),
	 _comm->size(),
	 nelems
	 );
  */
  size_t rdataWidth  = rdt->GetDataSize();
  size_t rdataExtent = rdt->GetExtent();
  if(rdataExtent != rdataWidth || _sdt->GetDataSize() != _sdt->GetExtent()) this->_contig = 0;
  _nelems = (nelems * rdataWidth) / _sdt->GetDataSize();// NJ: Number of elements of send type

  if (sbuf != dbuf) PAMI_Type_transform_data((void*)sbuf, _sdt, 0,
                                             dbuf, rdt, 0, nelems * rdataWidth,
                                             PAMI_DATA_COPY, NULL);

  /* need more memory in temp buffer? */
  /* SSS: We allocate double the buffer size instead of allocating twice.
          We use the second half of the allocated buffer to pack _dbuf
          in case of non contig data for reduction. */
  if (_tmpbuflen < nelems * rdataWidth * 2)
  {
    if (_tmpbuf) {
      __global.heap_mm->free (_tmpbuf);
      _tmpbuf = NULL;
	//printf("L%d: AR __global.heap_mm->free null \n",XLPGAS_MYNODE);
    }
    assert (nelems * rdataWidth > 0);

      /*
#if TRANSPORT == bgp
      //int alignment = MAXOF(sizeof(void*), datawidth);
      int alignment = sizeof(void*);
      int rc = __global.heap_mm->memalign (&_tmpbuf, alignment, nelems*datawidth);
      //printf("L%d: AR bgp alment=%d sz=%d %d \n",XLPGAS_MYNODE, alignment,nelems*datawidth,rc);
#else
      */
    _tmpbuf = __global.heap_mm->malloc (nelems * rdataWidth * 2);
    int rc = 0;

      //#endif


    if (rc || !_tmpbuf)
      xlpgas_fatalerror (-1, "Allreduce: memory allocation error, rc=%d", rc);
    _tmpbuflen = nelems * rdataWidth * 2;

    _tmpredbuf = (void*)((char *)_tmpbuf + (nelems * rdataWidth));
  }

  /* --------------------------------------------------- */
  /*  set source and destination buffers and node ids    */
  /* We deal with non-power-of-2 nodes                   */
  /* --------------------------------------------------- */

  int maxBF  = 1<<_logMaxBF;
  int nonBF  = this->_comm->size() - maxBF;
  int phase  = 0;

  if (nonBF > 0)   /* phase 0: gather buffers from ranks > n2prev */
    {
      phase ++;
      this->_sbuf    [phase] = ((int)this->ordinal() >= maxBF) ? this->_dbuf : NULL;
      this->_rbuf    [phase] = ((int)this->ordinal() < nonBF)  ? this->_tmpbuf : NULL;
      this->_sbufln  [phase] = this->_rbufln  [phase] = nelems * rdataWidth;
      this->_spwqln  [phase] = nelems * rdataExtent;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, rdt);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      phase ++;
    }

  for (int i=0; i<this->_logMaxBF; i++)   /* middle phases: butterfly pattern */
    {
      phase ++;
      this->_sbuf    [phase] = ((int)this->ordinal() < maxBF) ? this->_dbuf : NULL;
      this->_rbuf    [phase] = ((int)this->ordinal() < maxBF) ? this->_tmpbuf : NULL;
      this->_sbufln  [phase] = this->_rbufln  [phase] = nelems * rdataWidth;
      this->_spwqln  [phase] = nelems * rdataExtent;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, rdt);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      phase ++;
    }

  if (nonBF > 0)   /*  last phase: collect results */
    {
      phase ++;
      this->_sbuf    [phase] = ((int)this->ordinal() < nonBF)  ? this->_dbuf  : NULL;
      this->_rbuf    [phase] = ((int)this->ordinal() >= maxBF) ? this->_dbuf  : NULL;
      this->_sbufln  [phase] = this->_rbufln  [phase] = nelems * rdataWidth;
      this->_spwqln  [phase] = nelems * rdataExtent;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, rdt);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_spwqln[phase], 0, rdt);
      phase ++;
    }

  assert (phase == this->_numphases);
  this->_cb_allreduce = getcallback (op, _sdt);// NJ: Math op is done based on send type
}
