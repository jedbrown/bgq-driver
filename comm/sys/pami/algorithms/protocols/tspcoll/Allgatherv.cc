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
 * \file algorithms/protocols/tspcoll/Allgatherv.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Allgatherv.h"
#include "algorithms/protocols/tspcoll/Team.h"

#undef TRACE
#ifdef DEBUG_COLL
#define TRACE(x)  fprintf x;
#else
#define TRACE(x)
#endif

/* ************************************************************************* */
/*                       start Allgatherv                                    */
/* ************************************************************************* */
template <class T_NI>
xlpgas::Allgatherv<T_NI>::
Allgatherv (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset,T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  this->_tmpbuf = NULL;
  this->_tmpbuflen = 0;
  this->_dbuf = NULL;
  //this->_nbytes = 0;
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
      this->_rcvpwq[i].configure((char *)this->_rbuf[i], this->_rbufln[i], 0);
    }
  this->_numphases   *= 3;
  this->_phase        = this->_numphases;
  this->_sendcomplete = this->_numphases;
}

/* ************************************************************************* */
/*                      start allgatherv operation                         */
/* ************************************************************************* */
template <class T_NI>
void xlpgas::Allgatherv<T_NI>::reset (const void         * sbuf,
				void               * rbuf,
				TypeCode           * stype,
				size_t               stypecount,
				TypeCode           * rtype,
				size_t             * rtypecounts,
				size_t             * rdispls)
{
  size_t allsumbytes  = 0;
  size_t allsumstride = 0;
  for(int i=0;i<(int)this->_comm->size();i++)
  {
    allsumstride +=(rtype->GetExtent()   * rtypecounts[i]);
    allsumbytes  +=(rtype->GetDataSize() * rtypecounts[i]);
  }
  if(sbuf == PAMI_IN_PLACE)
  {
    sbuf       = (void *)((char *)rbuf + rdispls[this->ordinal()]*rtype->GetExtent());
    stype      = rtype;
    stypecount = rtypecounts[this->ordinal()];
  }


  /* --------------------------------------------------- */
  /*    copy source buffer to dest buffer                */
  /* --------------------------------------------------- */
  PAMI_Type_transform_data((void*)sbuf, stype, 0, (char *)rbuf + rdispls[this->ordinal()]*rtype->GetExtent(),
                           rtype, 0, rtype->GetDataSize()*rtypecounts[this->ordinal()], PAMI_DATA_COPY, NULL);
  /* --------------------------------------------------- */
  /* initialize destinations, offsets and buffer lengths */
  /* --------------------------------------------------- */

  for (int i=0, phase=this->_numphases/3; i<this->_numphases/3; i++, phase+=2)
    {
      int destindex = (this->ordinal()+2*this->_comm->size()-(1<<i))%this->_comm->size();
      int origindex = (this->ordinal()+(1<<i))%this->_comm->size();
      this->_dest[phase]   =  this->_comm->index2Endpoint (destindex);
      this->_dest[phase+1]   = this->_dest[phase];
      this->_sbuf[phase]   = (char *)rbuf + rdispls[this->ordinal()] * rtype->GetExtent();
      this->_sbuf[phase+1]   = (char *)rbuf;

      size_t phasesumbytes  = 0;
      size_t phasesumstride = 0;
      for (int n=0; n < (1<<i); n++)
      {
        phasesumstride += (rtype->GetExtent()*rtypecounts[(this->ordinal()+n)%this->_comm->size()]);
        phasesumbytes  += (rtype->GetDataSize()*rtypecounts[(this->ordinal()+n)%this->_comm->size()]);
      }

      this->_rbuf[phase]   = (char *)rbuf + ((rdispls[this->ordinal()]*rtype->GetExtent() + phasesumstride) % allsumstride);
      this->_rbuf[phase+1]   = (char *)rbuf;
      if (rdispls[this->ordinal()]*rtype->GetExtent() + phasesumstride >= allsumstride)
        {
          this->_sbufln[phase]   = allsumbytes - rdispls[this->ordinal()]*rtype->GetDataSize();
          this->_sbufln[phase+1] = rdispls[this->ordinal()]*rtype->GetDataSize() + phasesumbytes - allsumbytes;
          this->_spwqln[phase]   = allsumstride - rdispls[this->ordinal()]*rtype->GetExtent();
          this->_spwqln[phase+1] = rdispls[this->ordinal()]*rtype->GetExtent() + phasesumstride - allsumstride;
        }
      else
        {
          this->_sbufln[phase] = phasesumbytes;
          this->_sbufln[phase+1] = 0;
          this->_spwqln[phase] = phasesumstride;
          this->_spwqln[phase+1] = 0;
        }
      //recalculate for origin to know recv len
      phasesumbytes  = 0;
      phasesumstride = 0;
      for (int n=0; n < (1<<i); n++)
      {
        phasesumstride += (rtype->GetExtent()*rtypecounts[(origindex+n)%this->_comm->size()]);
        phasesumbytes  += (rtype->GetDataSize()*rtypecounts[(origindex+n)%this->_comm->size()]);
      }
      if (rdispls[origindex]*rtype->GetExtent() + phasesumstride >= allsumstride)
        {
          this->_rbufln[phase]   = allsumbytes - rdispls[origindex]*rtype->GetDataSize();
          this->_rbufln[phase+1] = rdispls[origindex]*rtype->GetDataSize() + phasesumbytes - allsumbytes;
          this->_rpwqln[phase]   = allsumstride - rdispls[origindex]*rtype->GetExtent();
          this->_rpwqln[phase+1] = rdispls[origindex]*rtype->GetExtent() + phasesumstride - allsumstride;
        }
      else
        {
          this->_rbufln[phase] = phasesumbytes;
          this->_rbufln[phase+1] = 0;
          this->_rpwqln[phase] = phasesumstride;
          this->_rpwqln[phase+1] = 0;
        }
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, rtype);
      this->_sndpwq[phase+1].configure((char *)this->_sbuf[phase+1], this->_spwqln[phase+1], this->_spwqln[phase+1], NULL, rtype);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rpwqln[phase], 0, rtype);
      this->_rcvpwq[phase+1].configure((char *)this->_rbuf[phase+1], this->_rpwqln[phase+1], 0, rtype);
    }
  /* ----------------------------------- */
  /* ----------------------------------- */
  xlpgas::CollExchange<T_NI>::reset();
}
