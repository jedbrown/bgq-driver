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
 * \file algorithms/protocols/tspcoll/Allgather.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Allgather.h"
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

#undef TRACE
#ifdef DEBUG_COLL
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

/* ************************************************************************* */
/*                       start Allgather                              */
/* ************************************************************************* */
template <class T_NI>
xlpgas::Allgather<T_NI>::
Allgather (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  this->_tmpbuf = NULL;
  this->_tmpbuflen = 0;
  this->_dbuf = NULL;
  this->_nbytes = 0;
  this->_numphases = -1; for (int n=2*this->_comm->size()-1; n>0; n>>=1) this->_numphases++;
  for (int i=0; i< this->_numphases; i++)
    {
      //this first part of the collective makes a barrier;
      //int destindex = (this->ordinal()+2*this->_comm->size()-(1<<i))%this->_comm->size();
      int destindex = (this->ordinal()+(1<<i))%comm->size();
      this->_dest[i] = this->_comm->index2Endpoint(destindex);
      this->_sbuf[i] = &this->_dummy;
      this->_rbuf[i] = &this->_dummy;
      this->_sbufln[i] = 1;
      this->_rbufln[i] = 1;
      this->_sndpwq[i].configure((char *)this->_sbuf[i], this->_sbufln[i], this->_sbufln[i]);
      this->_rcvpwq[i].configure((char *)this->_rbuf[i], this->_sbufln[i], 0);
    }
  this->_numphases   *= 3;
  this->_phase        = this->_numphases;
  this->_sendcomplete = this->_numphases;
}

/* ************************************************************************* */
/*                      start allgather operation                         */
/* ************************************************************************* */
template <class T_NI>
void xlpgas::Allgather<T_NI>::reset (const void         * sbuf,
			       void               * dbuf,
			       TypeCode           * stype,
			       size_t               stypecount,
			       TypeCode           * rtype,
			       size_t               rtypecount)
{
  assert (sbuf != NULL);
  assert (dbuf != NULL);
  size_t nrSize   = rtype->GetDataSize() * rtypecount;
  size_t nrExtent = rtype->GetExtent()   * rtypecount;
  if(sbuf == PAMI_IN_PLACE)
  {
    sbuf       = (void *)((char*)dbuf + nrExtent * this->ordinal());
    stype      = rtype;
    stypecount = rtypecount;
  }

  /* --------------------------------------------------- */
  /*    copy source buffer to dest buffer                */
  /* --------------------------------------------------- */

  // SSS: In the offset and size for PAMI_Type_transform we use the size not the extent. The packer and unpacker
  //      used to move data around care aboue data size not the extent.
  PAMI_Type_transform_data((void*)sbuf, stype, 0, dbuf, rtype, nrSize * this->ordinal(), nrSize, PAMI_DATA_COPY, NULL);
  //memcpy ((char *)dbuf + nrbytes * this->_comm->ordinal(), sbuf, nsbytes);

  /* --------------------------------------------------- */
  /* initialize destinations, offsets and buffer lengths */
  /* --------------------------------------------------- */

  for (int i=0, phase=this->_numphases/3; i<this->_numphases/3; i++, phase+=2)
    {
      int previndex  = (this->ordinal()+2*this->_comm->size()-(1<<i))%this->_comm->size();
      size_t nextindex  = (this->ordinal()+(1<<i))%this->_comm->size();

      this->_dest[phase]   = this->_comm->index2Endpoint (previndex);
      this->_dest[phase+1] = this->_dest[phase];

      this->_sbuf[phase]   = (char *)dbuf + this->ordinal() * nrExtent;/*SSS: I copied from sbuf to dbuf and I am using dbuf.
                                                                                     The layout now is based on dbuf layout */
      this->_sbuf[phase+1] = (char *)dbuf;

      this->_rbuf[phase]   = (char *)dbuf + nextindex*nrExtent;
      this->_rbuf[phase+1] = (char *)dbuf;

      if (this->ordinal() + (1<<i) >= this->_comm->size())
        {
          this->_sbufln[phase]      = nrSize * (this->_comm->size() - this->ordinal());          //SSS: Shouldn't matter whether nsSize or nrSize since
          this->_sbufln[phase+1]    = nrSize * (this->ordinal() + (1<<i) - this->_comm->size()); //     data should be the same size.
          this->_spwqln[phase]      = nrExtent * (this->_comm->size() - this->ordinal());
          this->_spwqln[phase+1]    = nrExtent * (this->ordinal() + (1<<i) - this->_comm->size());
	  //if(this->_sbufln[phase+1] == 0) this->_sbuf[phase+1] = NULL;//mark that there is no data to send
        }
      else
        {
          this->_sbufln[phase]      = nrSize * (1<<i);
          this->_sbufln[phase+1]    = 0;
          this->_spwqln[phase]      = nrExtent * (1<<i);
          this->_spwqln[phase+1]    = 0;
	  //this->_sbuf[phase+1] = NULL;
        }
      if (nextindex + (1<<i) >= this->_comm->size())
        {
          this->_rbufln[phase]      = nrSize * (this->_comm->size() - nextindex);          //SSS: Shouldn't matter whether nsSize or nrSize since
          this->_rbufln[phase+1]    = nrSize * (nextindex + (1<<i) - this->_comm->size()); //     data should be the same size.
          this->_rpwqln[phase]      = nrExtent * (this->_comm->size() - nextindex);
          this->_rpwqln[phase+1]    = nrExtent * (nextindex + (1<<i) - this->_comm->size());
        }
      else
        {
          this->_rbufln[phase]      = nrSize * (1<<i);
          this->_rbufln[phase+1]    = 0;
          this->_rpwqln[phase]      = nrExtent * (1<<i);
          this->_rpwqln[phase+1]    = 0;
        }
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, rtype);//SSS:     Setting consumer type to rtype since
      this->_sndpwq[phase+1].configure((char *)this->_sbuf[phase+1], this->_spwqln[phase+1], this->_spwqln[phase+1], NULL, rtype);// I already copied into dbuf
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rpwqln[phase], 0, rtype);
      this->_rcvpwq[phase+1].configure((char *)this->_rbuf[phase+1], this->_rpwqln[phase+1], 0, rtype);
    }
  xlpgas::CollExchange<T_NI>::reset();
}
