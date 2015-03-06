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
 * \file test/api/coll_validate.h
 * \brief ???
 */

#ifndef __test_api_coll_validate_h__
#define __test_api_coll_validate_h__

/* Define to assert on failures */
/* #define PAMI_TEST_STRICT     */


#include "coll_util.h"

void alltoall_initialize_bufs(char *sbuf, char *rbuf, size_t l, size_t r)
{
  size_t k;
  size_t d = l * r;

  for (k = 0; k < l; k++)
  {
    sbuf[ d + k ] = ((r + k) & 0xff);
    rbuf[ d + k ] = 0xff;
    if(gVerbose > 3)
      fprintf(stderr,"%zu: (I) sbuf[%zu]:%#02x \n",
           r,
           d + k,
           sbuf[ d + k ] );
  }
}


int alltoall_check_rcvbuf(char *rbuf, size_t l, size_t nranks, size_t myrank)
{
  size_t r, k;
  int err = 0; 
  for (r = 0; r < nranks; r++)
  {
    size_t d = l * r;
    for (k = 0; k < l; k++)
    {
      if (rbuf[ d + k ] != (char)((myrank + k) & 0xff))
      {
        if(gVerbose)        
          fprintf(stderr,"%zu: (E) rbuf[%zu]:%#02x instead of %#02x (r:%zu)\n",
               myrank,
               d + k,
               rbuf[ d + k ],
               (char)((myrank + k) & 0xff),
               r );
        err = 1;
      }
    }
  }
  return err;
}

void alltoall_initialize_bufs_dt(char *sbuf, char *rbuf, size_t l, size_t r, int dt)
{
  size_t k;
  size_t d = l * r;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *isbuf = (unsigned int *)  sbuf;
    unsigned int *irbuf = (unsigned int *)  rbuf;

    for (k = 0; k < l; k++)
    {
      isbuf[ d + k ] = ((unsigned int)((r + k) & 0xffffffff));
      irbuf[ d + k ] = 0xffffffff;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dsbuf = (double *)  sbuf;
    double *drbuf = (double *)  rbuf;

    for (k = 0; k < l; k++)
    {
      dsbuf[ d + k ] = ((double)((r + k))) * 1.0;
      drbuf[ d + k ] = 0xffffffffffffffff;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *fsbuf = (float *)  sbuf;
    float *frbuf = (float *)  rbuf;

    for (k = 0; k < l; k++)
    {
      fsbuf[ d + k ] = ((float)((r + k))) * 1.0;
      frbuf[ d + k ] = 0xffffffffffffffff;
    }
  }
  else
  {
    char *csbuf = (char *)  sbuf;
    char *crbuf = (char *)  rbuf;

    for (k = 0; k < l; k++)
    {
      csbuf[ d + k ] = ((r + k) & 0xff);
      crbuf[ d + k ] = 0xff;
    }
  }

}


int alltoall_check_rcvbuf_dt(char * rbuf, size_t l, size_t nranks, size_t myrank, int dt)
{
  size_t r, k;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *irbuf = (unsigned int *)rbuf;
    for (r = 0; r < nranks; r++)
    {
      size_t d = l * r;
      for (k = 0; k < l; k++)
      {
        if ((unsigned)irbuf[ d + k ] != (unsigned)((myrank + k)))
        {
          printf("%zu: (E) rbuf[%zu]:%#02x instead of %#02x (r:%zu)\n",
                 myrank,
                 d + k,
                 (unsigned int)irbuf[ d + k ],
                 (unsigned)((myrank + k)),
                 r );
          return 1;
        }
      }
    }
  }
  else  if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *drbuf = (double *)rbuf;
    for (r = 0; r < nranks; r++)
    {
      size_t d = l * r;
      for (k = 0; k < l; k++)
      {
        if ((double)drbuf[ d + k ] != (double)(((myrank*1.0) + (k*1.0))))
        {
          printf("%zu: (E) rbuf[%zu]:%f instead of %f (r:%zu)\n",
                 myrank,
                 d + k,
                 (double)drbuf[ d + k ],
                 (double)((myrank*1.0) + (k*1.0)),
                 r );
          return 1;
        }
      }
    }
  }
  else  if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *frbuf = (float *)rbuf;
    for (r = 0; r < nranks; r++)
    {
      size_t d = l * r;
      for (k = 0; k < l; k++)
      {
        if ((float)frbuf[ d + k ] != (float)(((myrank*1.0) + (k*1.0))))
        {
          printf("%zu: (E) rbuf[%zu]:%02f instead of %02f (r:%zu)\n",
                 myrank,
                 d + k,
                 (float)frbuf[ d + k ],
                 (float)(((myrank*1.0) + (k*1.0))),
                 r );
          return 1;
        }
      }
    }
  }
  return 0;
}

void alltoallv_initialize_bufs(char *sbuf, char *rbuf, size_t *sndlens, size_t *rcvlens,
                               size_t *sdispls, size_t *rdispls, size_t r)
{
  size_t k;

  for (k = 0; k < sndlens[r]; k++)
    {
      sbuf[ sdispls[r] + k ] = ((r + k) & 0xff);
      rbuf[ rdispls[r] + k ] = 0xff;
    }
}

int alltoallv_check_rcvbuf(char *rbuf, size_t *rcvlens, size_t *rdispls, size_t sz, size_t myrank)
{
  size_t r, k;

  for (r = 0; r < sz; r++)
    for (k = 0; k < rcvlens[r]; k++)
      {
        if (rbuf[ rdispls[r] + k ] != (char)((myrank + k) & 0xff))
          {
            fprintf(stderr, "%s:Check(%zu) failed rbuf[%zu+%zu]:%#02x instead of %#02x (rank:%zu)\n",
                    gProtocolName, rcvlens[r],
                    rdispls[r], k,
                    rbuf[ rdispls[r] + k ],
                    (char)((myrank + k) & 0xff),
                    r );
            return 1;
          }
      }

  return 0;
}

void alltoallv_initialize_bufs_dt(void *sbuf, void *rbuf, size_t *sndlens, size_t *rcvlens,
                                  size_t *sdispls, size_t *rdispls, size_t r, int dt)
{
  size_t k;
  size_t _r = r;

  if(rcvlens == NULL && rdispls == NULL)
  {
    rcvlens = sndlens;
    rdispls = sdispls;
    r = 0;
  }

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *isbuf = (unsigned int *)  sbuf;
    unsigned int *irbuf = (unsigned int *)  rbuf;
    for (k = 0; k < sndlens[r]; k++)
    {
      isbuf[ sdispls[r] + k ] = ((_r + k));
      irbuf[ rdispls[r] + k ] = 0xffffffff;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dsbuf = (double *)  sbuf;
    double *drbuf = (double *)  rbuf;

    for (k = 0; k < sndlens[r]; k++)
    {
      dsbuf[ sdispls[r] + k ] = ((double)((_r + k))) * 1.0;
      drbuf[ rdispls[r] + k ] = 0xffffffffffffffff;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *fsbuf = (float *)  sbuf;
    float *frbuf = (float *)  rbuf;

    for (k = 0; k < sndlens[r]; k++)
    {
      fsbuf[ sdispls[r] + k ] = ((float)((_r + k))) * 1.0;
      frbuf[ rdispls[r] + k ] = 0xffffffffffffffff;
    }
  }
  else
  {
    char *csbuf = (char *)  sbuf;
    char *crbuf = (char *)  rbuf;

    for (k = 0; k < sndlens[r]; k++)
    {
      csbuf[ sdispls[r] + k ] = ((_r + k) & 0xff);
      crbuf[ rdispls[r] + k ] = 0xff;
    }
  }
}

int alltoallv_check_rcvbuf_dt(void *rbuf, size_t *rcvlens, size_t *rdispls, size_t sz, size_t myrank, int dt)
{
  size_t r, k;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *irbuf = (unsigned int *)rbuf;
    for (r = 0; r < sz; r++)
      for (k = 0; k < rcvlens[r]; k++)
      {
        if (irbuf[ rdispls[r] + k ] != (unsigned int)((myrank + k)))
        {
          fprintf(stderr, "%s:Check(%zu) failed rbuf[%zu+%zu]:%#02x instead of %#02x (rank:%zu)\n",
                  gProtocolName, rcvlens[r],
                  rdispls[r], k,
                  irbuf[ rdispls[r] + k ],
                  (unsigned int)((myrank + k)),
                  r );
        return 1;
        }
      }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *drbuf = (double *)rbuf;
    for (r = 0; r < sz; r++)
      for (k = 0; k < rcvlens[r]; k++)
      {
        if (drbuf[ rdispls[r] + k ] != (double)(((myrank*1.0) + (k*1.0))))
        {
          fprintf(stderr, "%s:Check(%zu) failed rbuf[%zu+%zu]:%02f instead of %02f (rank:%zu)\n",
                  gProtocolName, rcvlens[r],
                  rdispls[r], k,
                  drbuf[ rdispls[r] + k ],
                  (double)(((myrank*1.0) + (k*1.0))),
                  r );
        return 1;
        }
      }
  }
  if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *frbuf = (float *)rbuf;
    for (r = 0; r < sz; r++)
      for (k = 0; k < rcvlens[r]; k++)
      {
        if (frbuf[ rdispls[r] + k ] != (float)(((myrank*1.0) + (k*1.0))))
        {
          fprintf(stderr, "%s:Check(%zu) failed rbuf[%zu+%zu]:%02f instead of %02f (rank:%zu)\n",
                  gProtocolName, rcvlens[r],
                  rdispls[r], k,
                  frbuf[ rdispls[r] + k ],
                  (float)(((myrank*1.0) + (k*1.0))),
                  r );
        return 1;
        }
      }
  }

  return 0;
}

void alltoallv_int_initialize_bufs(char *sbuf, char *rbuf, int *sndlens, int *rcvlens,
                                   int *sdispls, int *rdispls, size_t r)
{
  size_t k;

  for (k = 0; k < sndlens[r]; k++)
    {
      sbuf[ sdispls[r] + k ] = ((r + k) & 0xff);
      rbuf[ rdispls[r] + k ] = 0xff;
    }
}

int alltoallv_int_check_rcvbuf(char *rbuf, int *rcvlens, int *rdispls, size_t sz, size_t myrank)
{
  size_t r, k;

  for (r = 0; r < sz; r++)
    for (k = 0; k < rcvlens[r]; k++)
      {
        if (rbuf[ rdispls[r] + k ] != (char)((myrank + k) & 0xff))
          {
            fprintf(stderr, "%s:Check(%u) failed rbuf[%u+%zu]:%#02x instead of %#02x (rank:%zu)\n",
                    gProtocolName, rcvlens[r],
                    rdispls[r], k,
                    rbuf[ rdispls[r] + k ],
                    (char)((myrank + k) & 0xff),
                    r );
            return 1;
          }
      }

  return 0;
}

void alltoallv_int_initialize_bufs_dt(void *sbuf, void *rbuf, int *sndlens, int *rcvlens,
                                      int *sdispls, int *rdispls, size_t r, int dt)
{
  size_t k;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *isbuf = (unsigned int *)  sbuf;
    unsigned int *irbuf = (unsigned int *)  rbuf;
    for (k = 0; k < sndlens[r]; k++)
    {
      isbuf[ sdispls[r] + k ] = ((r + k));
      irbuf[ rdispls[r] + k ] = 0xffffffff;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dsbuf = (double *)  sbuf;
    double *drbuf = (double *)  rbuf;

    for (k = 0; k < sndlens[r]; k++)
    {
      dsbuf[ sdispls[r] + k ] = ((double)((r + k))) * 1.0;
      drbuf[ rdispls[r] + k ] = 0xffffffffffffffff;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *fsbuf = (float *)  sbuf;
    float *frbuf = (float *)  rbuf;

    for (k = 0; k < sndlens[r]; k++)
    {
      fsbuf[ sdispls[r] + k ] = ((float)((r + k))) * 1.0;
      frbuf[ rdispls[r] + k ] = 0xffffffffffffffff;
    }
  }
  else
  {
    char *csbuf = (char *)  sbuf;
    char *crbuf = (char *)  rbuf;

    for (k = 0; k < sndlens[r]; k++)
    {
      csbuf[ sdispls[r] + k ] = ((r + k) & 0xff);
      crbuf[ rdispls[r] + k ] = 0xff;
    }
  }
}

int alltoallv_int_check_rcvbuf_dt(void *rbuf, int *rcvlens, int *rdispls, size_t sz, size_t myrank, int dt)
{
  size_t r, k;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *irbuf = (unsigned int *)rbuf;
    for (r = 0; r < sz; r++)
      for (k = 0; k < rcvlens[r]; k++)
      {
        if (irbuf[ rdispls[r] + k ] != (unsigned int)((myrank + k)))
        {
          fprintf(stderr, "%s:Check(%u) failed rbuf[%u+%zu]:%#02x instead of %#02zx (rank:%zu)\n",
                  gProtocolName, rcvlens[r],
                  rdispls[r], k,
                  irbuf[ rdispls[r] + k ],
                  ((myrank + k)),
                  r );
        return 1;
        }
      }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *drbuf = (double *)rbuf;
    for (r = 0; r < sz; r++)
      for (k = 0; k < rcvlens[r]; k++)
      {
        if (drbuf[ rdispls[r] + k ] != (double)(((myrank*1.0) + (k*1.0))))
        {
          fprintf(stderr, "%s:Check(%u) failed rbuf[%u+%zu]:%02f instead of %02f (rank:%zu)\n",
                  gProtocolName, rcvlens[r],
                  rdispls[r], k,
                  drbuf[ rdispls[r] + k ],
                  (double)(((myrank*1.0) + (k*1.0))),
                  r );
        return 1;
        }
      }
  }
  if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *frbuf = (float *)rbuf;
    for (r = 0; r < sz; r++)
      for (k = 0; k < rcvlens[r]; k++)
      {
        if (frbuf[ rdispls[r] + k ] != (float)(((myrank*1.0) + (k*1.0))))
        {
          fprintf(stderr, "%s:Check(%u) failed rbuf[%u+%zu]:%02f instead of %02f (rank:%zu)\n",
                  gProtocolName, rcvlens[r],
                  rdispls[r], k,
                  frbuf[ rdispls[r] + k ],
                  (float)(((myrank*1.0) + (k*1.0))),
                  r );
        return 1;
        }
      }
  }

  return 0;
}

void allgather_initialize_sndbuf (void *sbuf, int bytes, int ep_id)
{
  unsigned char c = 0xFF & ep_id;
  memset(sbuf,c,bytes);
}

int allgather_check_rcvbuf (void *rbuf, int bytes, size_t num_ep)
{
  int i,j;
  unsigned char *cbuf = (unsigned char *)  rbuf;

  for (j=0; j<num_ep; j++)
  {
    unsigned char c = 0xFF & j;
    for (i=j*bytes; i<(j+1)*bytes; i++)
      if (cbuf[i] != c)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n",
                gProtocolName, bytes, cbuf, i, cbuf[i], c);
        return 1;
      }
  }
  return 0;
}

void scatter_initialize_sndbuf (void *sbuf, int bytes, size_t ntasks)
{
  size_t i;
  unsigned char *cbuf = (unsigned char *)  sbuf;

  for (i = 0; i < ntasks; i++)
  {
    unsigned char c = 0xFF & i;
    memset(cbuf + (i*bytes), c, bytes);
  }
}

int scatter_check_rcvbuf (void *rbuf, int bytes, pami_task_t task)
{
  int i;
  unsigned char *cbuf = (unsigned char *)  rbuf;

  unsigned char c = 0xFF & task;

  for (i = 0; i < bytes; i++)
    if (cbuf[i] != c)
    {
      fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", gProtocolName, bytes, cbuf, i, cbuf[i], c);
      return 1;
    }

  return 0;
}

void gather_initialize_sndbuf (int ep_id, void *buf, int bytes )
{
  unsigned char *cbuf = (unsigned char *)  buf;
  unsigned char c = 0x00 + ep_id;
  int i = bytes;

  for (; i; i--)
  {
    cbuf[i-1] = c++;
  }
}

int gather_check_rcvbuf (size_t num_ep, void *buf, int bytes)
{

  int j;
  for (j = 0; j < num_ep; j++)
  {
    unsigned char *cbuf = (unsigned char *)  buf + j *bytes;
    unsigned char c = 0x00 + j;
    int i = bytes;
    for (; i; i--)
    {
      if (cbuf[i-1] != c)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p> buf[%d]=%.2u != %.2u \n",
                gProtocolName,bytes,buf, i-1, cbuf[i-1], c);
        return -1;
      }

      c++;
    }
  }
  return 0;
}

void gather_initialize_sndbuf_dt (void *sbuf, int count, int taskid, int dt)
{
  int i = count;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  sbuf;
    unsigned int u = taskid;
    for (; i; i--)
    {
      ibuf[i-1] = (u++);
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_SIGNED_INT)
  {
    int *ibuf = (int *)  sbuf;
    int u = taskid;
    for (; i; i--)
    {
      ibuf[i-1] = (u++);
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dbuf = (double *)  sbuf;
    double d = taskid * 1.0;
    for (; i; i--)
    {
      dbuf[i-1] = d;
      d = d + 1.0;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *fbuf = (float *)  sbuf;
    float f = taskid * 1.0;
    for (; i; i--)
    {
      fbuf[i-1] = f;
      f = f + 1.0;
    }
  }
  else
  {
    char *cbuf = (char *)  sbuf;
    char c = taskid;
    for (; i; i--)
    {
      cbuf[i-1] = (c++);
    }
  }
}

int gather_check_rcvbuf_dt (size_t num_tasks, void *buf, int counts, int dt)
{
  int j;
  for (j = 0; j < num_tasks; j++)
  {
    if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
    {
      unsigned int *ibuf = (unsigned int *)  buf + j * counts;
      unsigned int u = j;
      int i = counts;
      for (; i; i--)
      {
        if (ibuf[i-1] != u)
        {
          fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", gProtocolName, counts, buf, i - 1, ibuf[i-1], u);
          return -1;
        }

        u++;
      }
    }
    else if (dt_array[dt] == PAMI_TYPE_SIGNED_INT)
    {
      int *ibuf = (int *)  buf + j * counts;
      int u = j;
      int i = counts;
      for (; i; i--)
      {
        if (ibuf[i-1] != u)
        {
          fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", gProtocolName, counts, buf, i - 1, ibuf[i-1], u);
          return -1;
        }

        u++;
      }
    }
        else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
    {
      double *dbuf = (double *)  buf + j * counts;
      double d = j * 1.0;
      int i = counts;
      for (; i; i--)
      {
        if (dbuf[i-1] != d)
        {
          fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2f != %.2f \n", gProtocolName, counts, buf, i - 1, dbuf[i-1], d);
          return -1;
        }

        d = d + 1.0;
      }
    }
    else   if (dt_array[dt] == PAMI_TYPE_FLOAT)
    {
      float *fbuf = (float *)  buf + j * counts;
      float f = j * 1.0;
      int i = counts;
      for (; i; i--)
      {
        if (fbuf[i-1] != f)
        {
          fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2f != %.2f \n", gProtocolName, counts, buf, i - 1, fbuf[i-1], f);
          return -1;
        }

        f = f + 1.0;
      }
    }
    else if (dt_array[dt] == PAMI_TYPE_SIGNED_CHAR || dt_array[dt] == PAMI_TYPE_UNSIGNED_CHAR)
    {
      char *cbuf = (char *)  buf + j *counts;
      char c = 0x00 + j;
      int i = counts;
      for (; i; i--)
      {
        if (cbuf[i-1] != c)
        {
          fprintf(stderr, "Check(%d) failed <%p> rank=%.2u, buf[%d]=%.2u != %.2u \n",counts,buf, i, i-1, cbuf[i-1], c);
          return -1;
        }

        c++;
      }
    }
  }
  return 0;
}

void reduce_initialize_sndbuf(void *buf, int count, int op, int dt, int task_id, int num_tasks)
{

  int i;

  if (op_array[op] == PAMI_DATA_BAND && (dt_array[dt] == PAMI_TYPE_SIGNED_INT || dt_array[dt] == PAMI_TYPE_UNSIGNED_INT))
  {
    unsigned *ibuf = (unsigned *)  buf;

    for (i = 0; i < count; i++)
    {
      if(task_id == 0)
        ibuf[i] = -1U;
      else ibuf[i] = ((i & 1)? i : 0);
    }
  }
  else
  if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_INT)
  {
    int *ibuf = ( int *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = -i;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = i;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_LONG)
  {
    long *ibuf = ( long *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = (long)-i;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_LONG)
  {
    unsigned long *ibuf = (unsigned long *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = (unsigned long)i;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_LONG_LONG)
  {
    long long *ibuf = (long long *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = (long long)-i;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_LONG_LONG)
  {
    unsigned long long *ibuf = (unsigned long long *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = (unsigned long long)i;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dbuf = (double *)  buf;

    for (i = 0; i < count; i++)
    {
      dbuf[i] = 1.0 * i;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *dbuf = (float *)  buf;

    for (i = 0; i < count; i++)
    {
      dbuf[i] = 1.0 * i;
    }
  }
  else if (op_array[op] == PAMI_DATA_MAX && dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    memset(buf,  0x0,  count * sizeof(double));
    double *dbuf = (double *)  buf;

    for (i = task_id; i < count; i += num_tasks)
    {
      dbuf[i] = 1.0 * task_id;
    }
    if(task_id == 0) dbuf[0] = -0.01; /* max - more interesting data than 0 */
    else dbuf[0] = -1.01 * task_id; /* more interesting data than 0 */
  }
  else if (op_array[op] == PAMI_DATA_MIN && dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dbuf = (double *)  buf;

    for (i = 0; i < count; i++)
      if ((i % num_tasks) == task_id)
        dbuf[i] = 1.01 * task_id; /* min == i on task_id */
      else 
        dbuf[i] = 1.01 * num_tasks; /* not min */

      if(task_id == 0) dbuf[0] = -0.01; /* more interesting data than 0 */
  }
  else
  {
    size_t sz=get_type_size(dt_array[dt]);
    memset(buf,  task_id,  count * sz);
  }
}

int reduce_check_rcvbuf(void *buf, int count, int op, int dt, int task_id, int num_tasks)
{

  int i;

  int err = 0;

  if (op_array[op] == PAMI_DATA_BAND && (dt_array[dt] == PAMI_TYPE_SIGNED_INT || dt_array[dt] == PAMI_TYPE_UNSIGNED_INT))
  {
    unsigned *rcvbuf = (unsigned *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != ((i & 1)? i : 0))
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %d != %d\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], ((i & 1)? i : 0));
        err = -1;
        return err;
      }
    }
  }
  else
  if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_INT)
  {
    int *rcvbuf = (int *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (int) -i * num_tasks)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %d != %d\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], -i*num_tasks);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *rcvbuf = (unsigned int *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (unsigned) i * num_tasks)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %u != %u\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], i*num_tasks);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_LONG_LONG)
  {
    long long *rcvbuf = (long long *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (long long) -i * num_tasks)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %lld != %lld\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (long long)-i*num_tasks);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_LONG)
  {
    unsigned long *rcvbuf = (unsigned long *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (unsigned long) i * num_tasks)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %lu != %lu\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (unsigned long)i*num_tasks);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_LONG)
  {
    long *rcvbuf = (long *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (long) -i * num_tasks)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %ld != %ld\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (long)-i*num_tasks);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_LONG_LONG)
  {
    unsigned long long *rcvbuf = (unsigned long long *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (unsigned long long) i * num_tasks)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %llu != %llu\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (unsigned long long)i*num_tasks);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *rcvbuf = (double *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != 1.0 * i * num_tasks)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (double)1.0*i*num_tasks);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *rcvbuf = (float *)  buf;

    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != 1.0 * i * num_tasks)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (float)1.0*i*num_tasks);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_MIN && dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *rcvbuf = (double *)  buf;

    if (rcvbuf[0] != -0.01)
    {
      fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, 0, rcvbuf[0], -0.01);
      err = -1;
      return err;
    }
    for (i = 1; i < count; i++)
    {
      if (rcvbuf[i] != 1.01 * (i % num_tasks))
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], 1.01 * (i % num_tasks));
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_MAX && dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *rcvbuf = (double *)  buf;

    if (rcvbuf[0] != -0.01)
    {
      fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, 0, rcvbuf[0], -0.01);
      err = -1;
      return err;
    }
    for (i = 1; i < count; i++)
    {
      if (rcvbuf[i] != 1.0 * (i % num_tasks))
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (double)1.0*(i % num_tasks));
        err = -1;
        return err;
      }
    }
  }

  return err;
}

void reduce_initialize_sndbuf_iter(void *buf, int count, int op, int dt, int iter)
{

  int i;

  if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_INT)
  {
    int *ibuf = ( int *)  buf;
    
    for (i = 0; i < count; i++)
    {
      ibuf[i] = iter;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = iter;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_LONG)
  {
    long *ibuf = ( long *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = (long)iter;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_LONG)
  {
    unsigned long *ibuf = (unsigned long *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = (unsigned long)iter;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_LONG_LONG)
  {
    long long *ibuf = (long long *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = (long long)iter;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_LONG_LONG)
  {
    unsigned long long *ibuf = (unsigned long long *)  buf;

    for (i = 0; i < count; i++)
    {
      ibuf[i] = (unsigned long long)iter;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dbuf = (double *)  buf;

    for (i = 0; i < count; i++)
    {
      dbuf[i] = 1.0 * iter;
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *dbuf = (float *)  buf;

    for (i = 0; i < count; i++)
    {
      dbuf[i] = 1.0 * iter;
    }
  }
  else
  {
    size_t sz=get_type_size(dt_array[dt]);
    memset(buf,  iter,  count * sz);
  }
}

int reduce_check_rcvbuf_iter(void *buf, int count, int op, int dt, int iter, int num_tasks)
{

  int i;

  int err = 0;

  if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_INT)
  {
    int *rcvbuf = (int *)  buf;
    int result = iter * num_tasks;
    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (int) result)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %d != %d\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], result);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *rcvbuf = (unsigned int *)  buf;
    unsigned int result = iter * num_tasks;
    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (unsigned) result)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %u != %u\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], result);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_LONG_LONG)
  {
    long long *rcvbuf = (long long *)  buf;
    long long result = iter * num_tasks;
    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (long long) result)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %lld != %lld\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (long long)result);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_LONG)
  {
    unsigned long *rcvbuf = (unsigned long *)  buf;
    unsigned long result = iter * num_tasks;
    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (unsigned long) result)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %lu != %lu\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (unsigned long)result);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_SIGNED_LONG)
  {
    long *rcvbuf = (long *)  buf;
    long result = iter * num_tasks;
    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (long) result)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %ld != %ld\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (long)result);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_LONG_LONG)
  {
    unsigned long long *rcvbuf = (unsigned long long *)  buf;
    unsigned long long result = iter * num_tasks;
    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != (unsigned long long) result)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %llu != %llu\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (unsigned long long)result);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *rcvbuf = (double *)  buf;
    double result = iter * num_tasks;
    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != result)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (double)result);
        err = -1;
        return err;
      }
    }
  }
  else if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *rcvbuf = (float *)  buf;
    float result = iter * num_tasks;
    for (i = 0; i < count; i++)
    {
      if (rcvbuf[i] != result)
      {
        fprintf(stderr, "%s:Check %s/%s(%d) failed rcvbuf[%d] %f != %f\n", gProtocolName, dt_array_str[dt], op_array_str[op], count, i, rcvbuf[i], (float)result);
        err = -1;
        return err;
      }
    }
  }

  return err;
}

void bcast_initialize_sndbuf (void *sbuf, int bytes, int root)
{

  unsigned char c = root;
  int i = bytes;
  unsigned char *cbuf = (unsigned char *)  sbuf;

  for (; i; i--)
  {
    cbuf[i-1] = (c++);
  }
}

int bcast_check_rcvbuf (void *rbuf, int bytes, int root)
{
  unsigned char c = root;
  int i = bytes;
  unsigned char *cbuf = (unsigned char *)  rbuf;

  for (; i; i--)
  {
    if (cbuf[i-1] != c)
    {
      fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", gProtocolName, bytes, rbuf, i - 1, cbuf[i-1], c);
      return 1;
    }

    c++;
  }

  return 0;
}

void bcast_initialize_sndbuf_dt (void *sbuf, int count, int root, int dt)
{
  int i = count;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  sbuf;
    unsigned int u = root;
    for (; i; i--)
    {
      ibuf[i-1] = (u++);
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dbuf = (double *)  sbuf;
    double d = root * 1.0;
    for (; i; i--)
    {
      dbuf[i-1] = d;
      d = d + 1.0;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *fbuf = (float *)  sbuf;
    float f = root * 1.0;
    for (; i; i--)
    {
      fbuf[i-1] = f;
      f = f + 1.0;
    }
  }
  else
  {
    char *cbuf = (char *)  sbuf;
    char c = root;
    for (; i; i--)
    {
      cbuf[i-1] = (c++);
    }
  }
}

int bcast_check_rcvbuf_dt (void *rbuf, int count, int root, int dt)
{
  int i = count;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  rbuf;
    unsigned int u = root;
    for (; i; i--)
    {
      if (ibuf[i-1] != u)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", gProtocolName, count, rbuf, i - 1, ibuf[i-1], u);
        return 1;
      }

      u++;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dbuf = (double *)  rbuf;
    double d = root * 1.0;
    for (; i; i--)
    {
      if (dbuf[i-1] != d)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2f != %.2f \n", gProtocolName, count, rbuf, i - 1, dbuf[i-1], d);
        return 1;
      }

      d = d + 1.0;
    }
  }
  else   if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *fbuf = (float *)  rbuf;
    float f = root * 1.0;
    for (; i; i--)
    {
      if (fbuf[i-1] != f)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2f != %.2f \n", gProtocolName, count, rbuf, i - 1, fbuf[i-1], f);
        return 1;
      }

      f = f + 1.0;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_SIGNED_CHAR || dt_array[dt] == PAMI_TYPE_UNSIGNED_CHAR)
  {
    char c = root;
    char *cbuf = (char *)  rbuf;

    for (; i; i--)
    {
      if (cbuf[i-1] != c)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", gProtocolName, count, rbuf, i - 1, cbuf[i-1], c);
        return 1;
      }

      c++;
    }
  }

  return 0;
}

void scatter_initialize_sndbuf_dt (void *sbuf, int counts, size_t ntasks, int dt)
{
  size_t i;
  size_t j;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  sbuf;
    for (i = 0; i < ntasks; i++)
    {
      unsigned int u = 0xFFFFFFFF & (unsigned)i;
      for(j = 0; j < counts; j++)
      {  
        ibuf[i*counts+j] = u;
      }
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_SIGNED_INT)
  {
    int *ibuf = (int *)  sbuf;
    for (i = 0; i < ntasks; i++)
    {
      int u = 0xFFFFFFFF & (int)i;
      for(j = 0; j < counts; j++)
        ibuf[i*counts+j] = u;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dbuf = (double *)  sbuf;
    for (i = 0; i < ntasks; i++)
    {
      double d = (double)i * 1.0;
      for(j = 0; j < counts; j++)
        dbuf[i*counts+j] = d;
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *fbuf = (float *)  sbuf;
    for (i = 0; i < ntasks; i++)
    {
      float f = (float)i * 1.0;
      for(j = 0; j < counts; j++)
        fbuf[i*counts+j] = f;
    }
  }
  else
  {
    unsigned char *cbuf = (unsigned char *)  sbuf;
    for (i = 0; i < ntasks; i++)
    {
      unsigned char c = 0xFF & i;
      memset(cbuf + (i*counts), c, counts);
    }
  }
}

int scatter_check_rcvbuf_dt (void *rbuf, int counts, pami_task_t task, int dt)
{
  int i;

  if (dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  rbuf;
    unsigned int u = 0xFFFFFFFF & (unsigned)task;
    for (i = 0; i < counts; i++)
    {
      if (ibuf[i] != u)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%u != %u \n", gProtocolName, counts, ibuf, i, ibuf[i], u);
        return 1;
      }
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_SIGNED_INT)
  {
    int *ibuf = (int *)  rbuf;
    int u = 0xFFFFFFFF & (int)task;
    for (i = 0; i < counts; i++)
    {
      if (ibuf[i] != u)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", gProtocolName, counts, ibuf, i, ibuf[i], u);
        return 1;
      }
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_DOUBLE)
  {
    double *dbuf = (double *)  rbuf;
    double d = (double)task * 1.0;
    for (i = 0; i < counts; i++)
    {
      if (dbuf[i] != d)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2f != %.2f \n", gProtocolName, counts, dbuf, i, dbuf[i], d);
        return 1;
      }
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_FLOAT)
  {
    float *fbuf = (float *)  rbuf;
    float f = (float)task * 1.0;
    for (i = 0; i < counts; i++)
    {
      if (fbuf[i] != f)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2f != %.2f \n", gProtocolName, counts, fbuf, i, fbuf[i], f);
        return 1;
      }
    }
  }
  else if (dt_array[dt] == PAMI_TYPE_UNSIGNED_CHAR || dt_array[dt] == PAMI_TYPE_SIGNED_CHAR)
  {
    unsigned char *cbuf = (unsigned char *)  rbuf;
    unsigned char c = 0xFF & task;
    for (i = 0; i < counts; i++)
    {
      if (cbuf[i] != c)
      {
        fprintf(stderr, "%s:Check(%d) failed <%p>rbuf[%d]=%.2u != %.2u \n", gProtocolName, counts, cbuf, i, cbuf[i], c);
        return 1;
      }
    }
  }

  return 0;
}

void scan_initialize_sndbuf (void *buf, int count, int op, int dt, int task_id)
{

  int i;
  /* if (op == PAMI_SUM && dt == PAMI_UNSIGNED_INT) { */
  if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  buf;
    for (i = 0; i < count; i++)
    {
      ibuf[i] = i;
    }
  }
  else
  {
    size_t sz=get_type_size(dt_array[dt]);
    memset(buf,  task_id,  count * sz);
  }
}

int scan_check_rcvbuf (void *buf, int count, int op, int dt, int num_tasks, int task_id, int exclusive)
{

  int i, err = 0;
  /*  if (op == PAMI_SUM && dt == PAMI_UNSIGNED_INT) { */
  if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *rbuf = (unsigned int *)  buf;
    unsigned int x;

    if(exclusive == 1)
      {
        /* Receive buffer is not valid on task 0 for exclusive scan */
        if(task_id == 0)
          return 0;
        else
          x = task_id;
      }
    else
      {
        x = task_id +1;
      }
    for (i = 0; i < count; i++)
    {
      if (rbuf[i] != i * x)
      {
        fprintf(stderr,"Check(%d) failed rbuf[%d] %u != %u\n",count,i,rbuf[i],i*x);
        err = -1;
        return err;    
      }
    }
  }

  return err;
}

void reduce_scatter_initialize_sndbuf (void *buf, int count, int op, int dt, int task_id)
{

  int i;
  /* if (op == PAMI_SUM && dt == PAMI_UNSIGNED_INT) { */
  if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *ibuf = (unsigned int *)  buf;
    for (i = 0; i < count; i++)
    {
      ibuf[i] = i;
    }
  }
  else
  {
    size_t sz=get_type_size(dt_array[dt]);
    memset(buf,  task_id,  count * sz);
  }
}

int reduce_scatter_check_rcvbuf (void *buf, int count, int op, int dt, int num_tasks, int task_id)
{

  int i, err = 0;
  /*  if (op == PAMI_SUM && dt == PAMI_UNSIGNED_INT) { */
  if (op_array[op] == PAMI_DATA_SUM && dt_array[dt] == PAMI_TYPE_UNSIGNED_INT)
  {
    unsigned int *rbuf = (unsigned int *)  buf;
    for (i = 0; i < count / num_tasks; i++)
    {
      if (rbuf[i] != (i + task_id * (count / num_tasks))* num_tasks)
      {
        fprintf(stderr,"Check(%d) failed rbuf[%d] %u != %u\n",count,i,rbuf[i],(i+task_id * (count/num_tasks))*num_tasks);
        err = -1;
        return err;    
      }
    }
  }

  return err;
}

#endif
