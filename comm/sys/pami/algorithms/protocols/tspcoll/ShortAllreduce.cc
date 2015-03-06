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
 * \file algorithms/protocols/tspcoll/ShortAllreduce.cc
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
/*                       start a short allreduce                              */
/* ************************************************************************* */
template <class T_NI>
xlpgas::Allreduce::Short<T_NI>::
Short (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  this->_dbuf = NULL;
  this->_nelems = 0;
  for (this->_logMaxBF = 0; (1<<(this->_logMaxBF+1)) <= (int)this->_comm->size(); this->_logMaxBF++) ;
  int maxBF  = 1<<this->_logMaxBF;
  int nonBF  = this->_comm->size() - maxBF;
  int phase  = 0;
  /* -------------------------------------------- */
  /* phase 0: gather buffers from ordinals > n2prev  */
  /* -------------------------------------------- */

  if (nonBF > 0)
    {
      //xlpgas_endpoint_t rdest = comm->index2Endpoint (comm->ordinal() - maxBF);
      //_dest    [phase] = (this->ordinal() >= maxBF) ? rdest : -1;
      this->_dest    [phase] = comm->index2Endpoint (this->ordinal() - maxBF);
      this->_sbuf    [phase] = NULL;    /* unknown */
      this->_rbuf    [phase] = ((int)this->ordinal() < nonBF)  ? this->_phasebuf[phase][0] : NULL;
      this->_cb_rcvhdr[phase] = ((int)this->ordinal() < nonBF)  ? cb_switchbuf : NULL;
      this->_postrcv [phase]  = ((int)this->ordinal() < nonBF)  ? cb_allreduce : NULL;
      this->_sbufln  [phase]  = 0;       /* unknown */
      this->_rbufln  [phase]  = 0;
      this->_bufctr  [phase]  = 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      phase ++;
    }

  /* -------------------------------------------- */
  /* butterfly phases                             */
  /* -------------------------------------------- */

  for (int i=0; i<_logMaxBF; i++)
    {
      //unsigned rdest   = comm->index2Endpoint (this->ordinal() ^ (1<<i));
      //_dest    [phase] = ((int)this->ordinal() < maxBF) ? rdest : -1;
      this->_dest [phase] = comm->index2Endpoint (this->ordinal() ^ (1<<i));
      this->_sbuf    [phase] = NULL;     /* unknown */
      this->_rbuf    [phase] = ((int)this->ordinal() < maxBF) ? this->_phasebuf[phase][0] : NULL;
      this->_cb_rcvhdr[phase] = ((int)this->ordinal() < maxBF) ? cb_switchbuf : NULL;
      this->_postrcv [phase]  = ((int)this->ordinal() < maxBF) ? cb_allreduce : NULL;
      this->_sbufln  [phase]  = 0;        /* unknown */
      this->_rbufln  [phase]  = 0;
      this->_bufctr  [phase]  = 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      phase ++;
    }

  /* -------------------------------------------- */
  /*  last phase: rebroadcast to non-power-of-2   */
  /* -------------------------------------------- */

  if (nonBF > 0)
    {
      ///unsigned rdest   = comm->index2Endpoint (this->ordinal() + maxBF);
      //_dest    [phase] = ((int)this->ordinal() < nonBF)  ? rdest : -1;
      this->_dest    [phase] = comm->index2Endpoint (this->ordinal() + maxBF);
      this->_sbuf    [phase] = NULL;     /* unknown */
      this->_rbuf    [phase] = NULL;     /* unknown */
      this->_cb_rcvhdr[phase] = NULL;
      this->_postrcv [phase]  = NULL;
      this->_sbufln  [phase]  = 0;        /* unknown */
      this->_rbufln  [phase]  = 0;
      this->_bufctr  [phase]  = 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
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
void xlpgas::Allreduce::Short<T_NI>::
cb_allreduce (CollExchange<T_NI> *coll, unsigned phase)
{
  xlpgas::Allreduce::Short<T_NI> * ar = (xlpgas::Allreduce::Short<T_NI> *) coll;
  int c = (ar->_counter+1) & 1;
  /*int *s = (int*)(ar->_phasebuf[phase][c]);
  int *d = (int*)(ar->_dbuf);
  for(int i=0;i<ar->_nelems;++i){
    printf("L%d Add %d %d\n",XLPGAS_MYNODE, s[i],d[i]);
  }
  */
  void * inputs[] = {ar->_dbuf, ar->_phasebuf[phase][c]};
  ar->_cb_allreduce (ar->_dbuf, inputs, 2, ar->_nelems);
}

template <class T_NI>
PAMI::PipeWorkQueue * xlpgas::Allreduce::Short<T_NI>::
cb_switchbuf (CollExchange<T_NI> * coll, unsigned phase, unsigned counter, size_t data_size)
{
  xlpgas::Allreduce::Short<T_NI> * ar = (xlpgas::Allreduce::Short<T_NI> *) coll;
  int c = (counter+1) & 1;
  PAMI::PipeWorkQueue * rcvpwq = &(ar->_rcvpwq[phase]);
  //SSS: Setting the pwq length to the size of the data coming in since we may
  //     have an early arrival. This protocol doesn't support non-contig data
  //     so we are ok with setting the length based on incoming contig data.
  rcvpwq->configure(ar->_phasebuf[phase][c], data_size, 0, ar->_rtype);
  return rcvpwq;
}

/* ************************************************************************* */
/*                      start a short allreduce operation                     */
/* ************************************************************************* */
template <class T_NI>
void xlpgas::Allreduce::Short<T_NI>::reset (const void         * sbuf,
				     void               * dbuf,
				     pami_data_function   op,
				     TypeCode           * sdt,
				     size_t               nelems,
				     TypeCode           * rdt,
				     user_func_t        * uf)
{
  assert (sbuf != NULL);
  assert (dbuf != NULL);

  xlpgas::CollExchange<T_NI>::reset();//to lock
  TypeCode           * lsdt = sdt;
  if(sbuf == PAMI_IN_PLACE)
  {
    sbuf    = dbuf;
    lsdt     = rdt;
  }

  _uf = uf; //user function pointer
  /* --------------------------------------------------- */
  /*         copy source to destination if necessary     */
  /* --------------------------------------------------- */
  /*
    printf("L%d: short allreduce [teamid=%d rank=%d sizeworld=%d nelems=%d]\n",
	 XLPGAS_MYNODE,
	 this->_comm->commID(),/_stype
	 this->ordinal(),
	 this->_comm->size(),
	 nelems
	 );
  */
  size_t rdataWidth  = rdt->GetDataSize();
  size_t rdataExtent = rdt->GetExtent();
  _dbuf   = dbuf;
  _nelems = (nelems * rdataWidth) / lsdt->GetDataSize();// NJ: Number of elements of send type
  _rtype  = rdt;

  if (sbuf != dbuf) PAMI_Type_transform_data((void*)sbuf, lsdt, 0,
                                             dbuf, rdt, 0, nelems * rdataWidth,
                                             PAMI_DATA_COPY, NULL);
  /* --------------------------------------------------- */
  /*  set source and destination buffers and node ids    */
  /* We deal with non-power-of-2 nodes                   */
  /* --------------------------------------------------- */

  int maxBF  = 1<<_logMaxBF;
  int nonBF  = this->_comm->size() - maxBF;
  int phase  = 0;

  if (nonBF > 0)   /* phase 0: gather buffers from ordinals > n2prev */
    {
      this->_sbuf    [phase] = ((int)this->ordinal() >= maxBF) ? dbuf  : NULL;
      this->_sbufln  [phase] = this->_rbufln  [phase] = nelems * rdataWidth;
      this->_spwqln  [phase] = nelems * rdataExtent;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, rdt);
      phase ++;
    }

  /* -------------------------------------------- */
  /* butterfly phases                             */
  /* -------------------------------------------- */

  for (int i=0; i<this->_logMaxBF; i++)   /* middle phases: butterfly pattern */
    {
      this->_sbuf    [phase] = ((int)this->ordinal() < maxBF) ? dbuf  : NULL;
      this->_sbufln  [phase] = this->_rbufln  [phase] = nelems * rdataWidth;
      this->_spwqln  [phase] = nelems * rdataExtent;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, rdt);
      phase ++;
    }


  /* -------------------------------------------- */
  /*  last phase: rebroadcast to non-power-of-2   */
  /* -------------------------------------------- */

  if (nonBF > 0)   /*  last phase: collect results */
    {
      this->_sbuf    [phase] = ((int)this->ordinal() < nonBF)  ? dbuf  : NULL;
      this->_rbuf    [phase] = ((int)this->ordinal() >= maxBF) ? dbuf  : NULL;
      this->_sbufln  [phase] = this->_rbufln  [phase] = nelems * rdataWidth;
      this->_spwqln  [phase] = nelems * rdataExtent;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, rdt);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_spwqln[phase], 0, rdt);
      phase ++;
    }
  assert (phase == this->_numphases);
  this->_cb_allreduce = getcallback (op, lsdt);// NJ: Math op is done based on send type
}
