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
 * \file algorithms/protocols/tspcoll/Broadcast.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Broadcast.h"
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
/*                       start a long broadcast                              */
/* ************************************************************************* */
template <class T_NI>
xlpgas::Broadcast<T_NI>::
Broadcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  this->_tmpbuf = NULL;
  this->_tmpbuflen = 0;
  this->_dbuf = NULL;
  this->_nbytes = 0;
  this->_numphases = -1; for (int n=2*this->_comm->size()-1; n>0; n>>=1) this->_numphases++;
  for (int i=0; i< this->_numphases; i++)
    {
      int destindex = (this->ordinal()+2*this->_comm->size()-(1<<i))%this->_comm->size();
      this->_dest[i] = this->_comm->index2Endpoint(destindex);
      this->_sbuf[i] = &this->_dummy;
      this->_rbuf[i] = &this->_dummy;
      this->_sbufln[i] = 1;
      this->_rbufln[i] = 1;
      this->_sndpwq[i].configure((char *)this->_sbuf[i], this->_sbufln[i], this->_sbufln[i]);
      this->_rcvpwq[i].configure((char *)this->_rbuf[i], this->_rbufln[i], 0);
    }
  this->_numphases   *= 2;
  this->_phase        = this->_numphases;
  this->_sendcomplete = this->_numphases;
}


/* ************************************************************************* */
/*                      start a bcast operation                         */
/* ************************************************************************* */
template <class T_NI>
void xlpgas::Broadcast<T_NI>::reset (int rootindex,
			       const void         * sbuf,
			       void               * dbuf,
			       TypeCode           * type,
			       size_t               typecount)
{
  if(rootindex == (int)this->ordinal()) {
    assert (sbuf != NULL);
  }

  assert (dbuf != NULL);
  size_t nbytes  = type->GetDataSize() * typecount;
  size_t nstride = type->GetExtent() * typecount;
  if (rootindex >= (int)this->_comm->size())
    xlpgas_fatalerror (-1, "Invalid root index in Bcast");

  /* --------------------------------------------------- */
  /* --------------------------------------------------- */

  if (rootindex == (int)this->ordinal() && sbuf != dbuf){
    PAMI_Type_transform_data((void*)sbuf, type, 0, dbuf, type, 0, nbytes, PAMI_DATA_COPY, NULL);
  }

  int myrelrank = (this->ordinal() + this->_comm->size() - rootindex) % this->_comm->size();
  for (int i=0, phase=this->_numphases/2; i<this->_numphases/2; i++, phase++)
    {
      int  dist       = 1<<(this->_numphases/2-1-i);
      int  sendmask   = (1<<(this->_numphases/2-i))-1;
      int  destrelrank= myrelrank + dist;
      int  srcrelrank = myrelrank - dist;
      bool dosend     = ((myrelrank&sendmask)==0)&&(destrelrank<(int)this->_comm->size());
      bool dorecv     = ((srcrelrank&sendmask)==0)&&(srcrelrank>=0);
      int  destindex  = (destrelrank + rootindex)%this->_comm->size();
      this->_dest[phase]    = this->_comm->index2Endpoint(destindex);
      this->_sbuf[phase]    = dosend ? dbuf    : NULL;
      this->_sbufln[phase]  = dosend ? nbytes  : 0;
      this->_spwqln[phase]  = dosend ? nstride : 0;
      this->_rbuf[phase]    = dorecv ? dbuf    : NULL;
      this->_rbufln[phase]  = dorecv ? nbytes  : 0;
      this->_rpwqln[phase]  = dorecv ? nstride : 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, type);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rpwqln[phase], 0, type);
    }
  xlpgas::CollExchange<T_NI>::reset();
  return;

  /* --------------------------------------------------- */
  /* --------------------------------------------------- */
  /*
  for (_logMaxBF = 0; (1<<(_logMaxBF+1)) <= _comm->size(); _logMaxBF++) ;
  int maxBF  = 1<<_logMaxBF;
  int nonBF  = _comm->size() - maxBF;
  int phase  = 0;

  _numphases = -1; for (int n=2*_comm->size()-1; n>0; n>>=1) _numphases++;
  _numphases   *= 2;

  int myrelrank = (ordinal() + _comm->size() - rootindex) % _comm->size();
  phase=0;
  for (int i=0; i<_numphases/2; i++)
    {
      int  dist       = 1<<(_numphases/2-1-i);
      int  sendmask   = (1<<(_numphases/2-i))-1;
      int  destrelrank= myrelrank + dist;
      int  srcrelrank = myrelrank - dist;
      bool dosend     = ((myrelrank&sendmask)==0)&&(destrelrank<_comm->size());
      bool dorecv     = ((srcrelrank&sendmask)==0)&&(srcrelrank>=0);
      int  destindex  = (destrelrank + rootindex)%_comm->size();

      //_dest[phase]    = _comm->index2Endpoint(destindex);
      //_sbuf[phase]    = dosend ? dbuf : NULL;
      //_sbufln[phase]  = dosend ? nbytes : 0;
      //_rbuf[phase]    = dorecv ? dbuf : NULL;

      //ask permision
      //_dest[phase]    = _comm->index2Endpoint(destindex);
      //_sbuf[phase]    = dosend ? &_dummy : NULL;
      //_sbufln[phase]  = dosend ? 1 : 0;
      //_rbuf[phase]    = dorecv ? &_dummy : NULL;
      //_postrcv[phase] = NULL;

      //printf("L%d: in phase %d send to %d : dosend=%d dorecv=%d\n", ordinal(), phase, _dest[phase].node, dosend, dorecv);
      //phase++;

      //broadcast data
      _dest[phase]    = _comm->index2Endpoint(destindex);
      _sbuf[phase]    = dosend ? dbuf : NULL;
      _sbufln[phase]  = dosend ? nbytes : 0;
      _rbuf[phase]    = dorecv ? dbuf : NULL;
      _postrcv[phase] = NULL;

      //printf("L%d: in phase %d send to %d : dosend=%d dorecv=%d\n", ordinal(), phase, _dest[phase].node, dosend, dorecv);

      phase++;
    }

  _numphases = phase;
  //printf("%d: NUMPHASES =%d\n", ordinal(), _numphases);
  xlpgas::CollExchange::reset();
  */
}
