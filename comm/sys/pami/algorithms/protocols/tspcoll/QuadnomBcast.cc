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
 * \file algorithms/protocols/tspcoll/QuadnomBcast.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/QuadnomBcast.h"
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

template<int X> xlpgas_AMHeaderReg_t xlpgas::CollExchangeX<X>::_regnum;

/* ************************************************************************* */
/*                       start a long broadcast                              */
/* ************************************************************************* */

template <class T_NI>
xlpgas::QuadnomBcast<T_NI>::
QuadnomBcast (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset) :
  CollExchangeX<3> (ctxt, comm, kind, tag, offset)
{
  int n;
  _logN = -1; for (n=2*_comm->size()-1; n>0; n>>=1) _logN++;
  _qN = -1;   for (n=4*_comm->size()-1; n>0; n>>=2) _qN++;
  _numphases = _logN + _qN ;

  /* barrier */
  for (int i=0; i< _logN; i++)
    {
      int destindex = (_comm->ordinal()+2*_comm->size()-(1<<i))%_comm->size();
      _dest[i][0] = _comm->index2Endpoint(destindex);
      _dest[i][1].node = -1;
      _dest[i][2].node = -1;
      _sbuf[i] = &_dummy;
      _rbuf[i] = &_dummy;
      _sbufln[i] = 1;
    }
  _phase        = _numphases;
  _sendcomplete = _logN+3*_qN;
}


/* ************************************************************************* */
/*                      start a bcast operation                         */
/* ************************************************************************* */
template <class T_NI>
void xlpgas::QuadnomBcast<T_NI>::reset (int                  rootindex,
				  const void         * sbuf, 
				  void               * dbuf, 
				  unsigned             nbytes)
{
  if(rootindex == XLPGAS_MYTHREAD) assert (sbuf != NULL);
  assert (dbuf != NULL);

  if (rootindex >= _comm->size())
    xlpgas_fatalerror (-1, "Invalid root index in Bcast");

  /* --------------------------------------------------- */
  /* --------------------------------------------------- */

  if (rootindex==_comm->ordinal() && sbuf!=dbuf) memcpy (dbuf, sbuf, nbytes);

  int myrelrank=(_comm->ordinal()+_comm->size()-rootindex)%_comm->size();
  int N = _comm->size();

  for (int i=0, phase=_logN+_qN-1; i<_qN; i++, phase--)
    {
      _dest   [phase][0].node = -1;
      _dest   [phase][1].node = -1;
      _dest   [phase][2].node = -1;
      _sbuf   [phase]         = dbuf;
      _rbuf   [phase]         = NULL;
      _sbufln [phase]         = nbytes;

      int step       = 1<<(2*i);
      int activemask = 3<<(2*i);
      int idlemask   = (i==0) ? 0: ((1<<(2*i))-1);

      if ((myrelrank&idlemask)!=0)          continue; /* I am idle */
      else if ((myrelrank&activemask)!=0) _rbuf[phase] = dbuf; 
      else /* I am a sender */
	{
	  int destrelrank1 = myrelrank + step;
	  int destindex1 = (destrelrank1+rootindex)%N;
	  if (destrelrank1<N) _dest[phase][0] = _comm->index2Endpoint(destindex1);
	  int destrelrank2 = myrelrank + 2* step;
	  int destindex2 = (destrelrank2+rootindex)%N;
	  if (destrelrank2<N) _dest[phase][1] = _comm->index2Endpoint(destindex2);
	  int destrelrank3 = myrelrank + 3* step;
	  int destindex3 = (destrelrank3+rootindex)%N;
	  if (destrelrank3<N) _dest[phase][2] = _comm->index2Endpoint(destindex3);
	}
    }
  xlpgas::CollExchangeX<T_NI,3>::reset();
  return;

}
