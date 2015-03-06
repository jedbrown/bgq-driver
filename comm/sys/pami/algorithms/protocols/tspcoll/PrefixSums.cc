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
 * \file algorithms/protocols/tspcoll/PrefixSums.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/PrefixSums.h"
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
/*                       Start PrefixSums                                    */
/* ************************************************************************* */
template <class T_NI>
xlpgas::PrefixSums<T_NI>::
PrefixSums (int ctxt, Team * comm, CollectiveKind kind, int tag, int offset, T_NI* ni) :
  CollExchange<T_NI> (ctxt, comm, kind, tag, offset, ni)
{
  this->_tmpbuf    = NULL;
  this->_tmpredbuf = NULL;
  this->_tmpbuflen = 0;
  this->_dbuf      = NULL;
  this->_nelems    = 0;
  this->_exclusive = 0;
  this->_contig    = 1; //SSS: Default is contiguous
  for (this->_logMaxBF = 0; (1<<(this->_logMaxBF+1)) <= (int)this->_comm->size(); this->_logMaxBF++) ;
  int maxBF  = 1<<this->_logMaxBF;
  if(maxBF < (int) this->_comm->size()) this->_logMaxBF++;
  int phase  = 0;

  /* -------------------------------------------- */
  /* prefix sums                                  */
  /* -------------------------------------------- */

  for (int i=0; i<this->_logMaxBF; i++)
    {
      /* send permission chits to senders */
      int left = this->ordinal() - (1<<i);
      this->_dest    [phase] = this->_comm->index2Endpoint (left);
      this->_sbuf    [phase] = (left >= 0) ? &this->_dummy : NULL;
      int right = this->ordinal() + (1<<i);
      this->_rbuf    [phase] = (right < (int)this->_comm->size()) ? &this->_dummy : NULL;
      this->_postrcv [phase] = NULL;
      this->_sbufln  [phase] = 1;
      this->_rbufln  [phase] = (right < (int)this->_comm->size()) ? 1 : 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      //printf("%d: ### in phase %d will send permission to  %d \n", XLPGAS_MYNODE, phase, _dest[phase].node);
      phase ++;

      /* send data */
      this->_dest    [phase] = ( right < (int)this->_comm->size() ) ? this->_comm->index2Endpoint(right) : this->_comm->index2Endpoint(0);
      this->_sbuf    [phase] = NULL; /* send buffer not available */
      this->_rbuf    [phase] = NULL; /* receive buffer not available */
      this->_postrcv [phase] = (left >= 0) ? cb_prefixsums : NULL;
      this->_sbufln  [phase] = 0;    /* data length not available */
      this->_rbufln  [phase] = 0;    /* data length not available */
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_sbufln[phase], this->_sbufln[phase]);
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rbufln[phase], 0);
      //printf("%d: ### in phase %d will send data to  %d \n", XLPGAS_MYNODE, phase, _dest[phase].node);
      phase ++;
    }

  this->_numphases    = phase;
  this->_phase        = this->_numphases;
  this->_sendcomplete = this->_numphases;
}

template <class T_NI>
void xlpgas::PrefixSums<T_NI>::
cb_prefixsums (CollExchange<T_NI> *coll, unsigned phase)
{
  xlpgas::PrefixSums<T_NI> * ar = (xlpgas::PrefixSums<T_NI> *) coll;
  void * inputs[2];
  if(ar->_exclusive == 0)
    {
      if(ar->_contig)
      {
        inputs[0] = ar->_dbuf;
        inputs[1] = ar->_tmpbuf;
        ar->_cb_prefixsums (ar->_dbuf, inputs, 2, ar->_nelems);
      }
      else
      {
        PAMI_Type_transform_data((void*)ar->_dbuf, ar->_rdt, 0,
                                             ar->_tmpredbuf, PAMI_TYPE_BYTE, 0, ar->_nelems * ar->_sdt->GetDataSize(),
                                             PAMI_DATA_COPY, NULL);

        inputs[0] = ar->_tmpredbuf;
        inputs[1] = ar->_tmpbuf;
        ar->_cb_prefixsums (ar->_tmpredbuf, inputs, 2, ar->_nelems);
        PAMI_Type_transform_data((void*)ar->_tmpredbuf, PAMI_TYPE_BYTE, 0,
                                             ar->_dbuf, ar->_rdt, 0, ar->_nelems * ar->_sdt->GetDataSize(),
                                             PAMI_DATA_COPY, NULL);
      }
    }
  else if(ar->ordinal() > 0)
    {
      TypeCode *sdt = ar->_sdt;
      size_t datawidth  = sdt->GetDataSize();
      inputs[1] = (((char *)ar->_tmpbuf) + ar->_nelems * datawidth);
      // In phase 1 we copy the received data to the destination buffer
      if(phase == 1)
        {
          PAMI_Type_transform_data((void*)inputs[1], PAMI_TYPE_BYTE, 0,
                                   ar->_dbuf, ar->_rdt, 0, ar->_nelems * datawidth,
                                   PAMI_DATA_COPY, NULL);
        }
      else
        {
          if(ar->_contig)
          {
            inputs[0] = ar->_dbuf;
            ar->_cb_prefixsums (ar->_dbuf, inputs, 2, ar->_nelems);
          }
          else
          {
            PAMI_Type_transform_data((void*)ar->_dbuf, ar->_rdt, 0,
                                             ar->_tmpredbuf, PAMI_TYPE_BYTE, 0, ar->_nelems * ar->_sdt->GetDataSize(),
                                             PAMI_DATA_COPY, NULL);
            inputs[0] = ar->_tmpredbuf;
            ar->_cb_prefixsums (ar->_tmpredbuf, inputs, 2, ar->_nelems);
            PAMI_Type_transform_data((void*)ar->_tmpredbuf, PAMI_TYPE_BYTE, 0,
                                             ar->_dbuf, ar->_rdt, 0, ar->_nelems * ar->_sdt->GetDataSize(),
                                             PAMI_DATA_COPY, NULL);
          }
        }
        inputs[0] = ar->_tmpbuf;
        ar->_cb_prefixsums (ar->_tmpbuf, inputs, 2, ar->_nelems);
    }
}


/* ************************************************************************* */
/*                      start prefixSums operation                           */
/* ************************************************************************* */
template <class T_NI>
void xlpgas::PrefixSums<T_NI>::reset (const void         * sbuf,
				void               * dbuf,
				pami_data_function   op,
				TypeCode           * sdt,
				TypeCode           * rdt,
				size_t               nelems)
{
  assert (sbuf != NULL);
  assert (dbuf != NULL);
  xlpgas::CollExchange<T_NI>::reset();
  this->_sdt = sdt;
  this->_rdt = rdt;
  if(sbuf == PAMI_IN_PLACE)
  {
    this->_sdt = rdt;
    sbuf       = dbuf;
  }
  this->_dbuf   = dbuf;

  size_t datawidth  = rdt->GetDataSize();
  size_t dataextent = rdt->GetExtent();
  this->_nelems     = (nelems * datawidth) / this->_sdt->GetDataSize();
  size_t tmpbufsize;

  if(dataextent != datawidth || this->_sdt->GetDataSize() != this->_sdt->GetExtent()) this->_contig = 0;

  /* For exclusive scan we need twice the amount of temporary buffer space
     _tmpbuf = {x,y}  , where
      x = partial "inclusive" scan result
      y = buffer received from left neighbour
      size of x = size of y = nelems * datawidth
  */
  if(_exclusive == 0)
    tmpbufsize = nelems * datawidth;
  else
    tmpbufsize = 2 * nelems * datawidth;

  /* need more memory in temp buffer? */
  /* SSS: We allocate double the buffer size instead of allocating twice.
          We use the second half of the allocated buffer to pack _dbuf
          in case of non contig data for reduction. */
  if (this->_tmpbuflen < tmpbufsize * 2)
  {
    if (this->_tmpbuf) {
      __global.heap_mm->free (this->_tmpbuf);
      this->_tmpbuf = NULL;
    }
    assert (nelems * datawidth > 0);

#if TRANSPORT == bgp
      //int alignment = MAXOF(sizeof(void*), datawidth);
      int alignment = sizeof(void*);
      int rc = __global.heap_mm->memalign (&this->_tmpbuf, alignment, tmpbufsize * 2);
      //printf("L%d: AR bgp alment=%d sz=%d %d \n",XLPGAS_MYNODE, alignment,nelems*datawidth,rc)
#else
    this->_tmpbuf = __global.heap_mm->malloc (tmpbufsize * 2);
    int rc = 0;
#endif
    if (rc || !this->_tmpbuf)
	  xlpgas_fatalerror (-1, "PrefixSums: memory allocation error, rc=%d", rc);

    this->_tmpredbuf = (void*)((char *)this->_tmpbuf + tmpbufsize);
  }

 
  this->_tmpbuflen = tmpbufsize * 2;

  if(_exclusive == 0 && sbuf != dbuf)
  {
    PAMI_Type_transform_data((void*)sbuf, this->_sdt, 0,
                             dbuf, rdt, 0, nelems * datawidth,
                             PAMI_DATA_COPY, NULL);
  }
  else
  {
    PAMI_Type_transform_data((void*)sbuf, this->_sdt, 0,
                             this->_tmpbuf, PAMI_TYPE_BYTE, 0, nelems * datawidth,
                             PAMI_DATA_COPY, NULL);
  }

  /* --------------------------------------------------- */
  /*  set source and destination buffers and node ids    */
  /* We deal with non-power-of-2 nodes                   */
  /* --------------------------------------------------- */
  int phase  = 0;
  void *sbuf_phase, *rbuf_phase;
  if(this->_exclusive == 0)
    {
      sbuf_phase = this->_dbuf;
      rbuf_phase = this->_tmpbuf;
    }
  else
    {
      sbuf_phase = this->_tmpbuf;
      rbuf_phase = (((char *)this->_tmpbuf) + nelems * datawidth);
    }

  if(tmpbufsize == 0 && !rbuf_phase)
  {
    rbuf_phase = this->_dbuf;
  }

  for (int i=0; i<this->_logMaxBF; i++)   /* prefix sums pattern */
    {
      phase ++;
      int tgt = this->ordinal() + (1<<i);
      this->_sbuf    [phase] = (tgt < (int)this->_comm->size()) ? sbuf_phase : NULL;
      this->_sbufln  [phase] = (tgt<(int)this->_comm->size()) ? nelems * datawidth  : 0 ;
      this->_spwqln  [phase] = (tgt<(int)this->_comm->size()) ? nelems * (this->_exclusive ? datawidth:dataextent) : 0 ;
      tgt = this->ordinal() - (1<<i);
      this->_rbuf    [phase] = (tgt>=0) ? rbuf_phase : NULL;
      this->_rbufln  [phase] = this->_rpwqln  [phase] = (tgt>=0) ? nelems * datawidth  : 0;
      this->_sndpwq[phase].configure((char *)this->_sbuf[phase], this->_spwqln[phase], this->_spwqln[phase], NULL, (this->_exclusive ? NULL : _rdt));
      this->_rcvpwq[phase].configure((char *)this->_rbuf[phase], this->_rpwqln[phase], 0);
      phase ++;
    }
  assert (phase == this->_numphases);
  this->_cb_prefixsums = xlpgas::Allreduce::getcallback (op, this->_sdt);
}
