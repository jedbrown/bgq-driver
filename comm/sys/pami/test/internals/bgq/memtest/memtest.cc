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
 * \file test/internals/bgq/memtest/memtest.cc
 * \brief ???
 */
#include <stdio.h>
#include <string.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <assert.h>

void* Core_memcpy(void* dst, void* src, size_t bytes);

#define NUM_LOOPS 10000

#define BUFSIZE 65536

#define BOUNDRY 64

char srcbuf[BUFSIZE+128] __attribute__((__aligned__(64))), dstbuf[BUFSIZE+128] __attribute__((__aligned__(64)));

int main() {
  uint64_t start, end;
  int i = 0;

  for (int a = 0; a < 32; a++) {
    char *src = (char *)srcbuf + BOUNDRY + a;
    char *dst = (char *)dstbuf + BOUNDRY + a;
    
    for (int bytes = 1; bytes <= 2048; bytes++) {
      char * overflow = src + bytes;
      for (i = 0; i < BOUNDRY; i++)
	overflow[i] = 0xff;

      char * underflow = src - BOUNDRY;
      for (i = 0; i < BOUNDRY; i++)
	underflow[i] = 0xff;

      for (i = 0; i < bytes; i++)
	src[i] = i & 0xff;      
      
      Core_memcpy(dst, src, bytes);
      
      for (i = 0; i < bytes; i++)
	assert (dst[i] == (i & 0xff));
      
      overflow = dst + bytes;
      for (i = 0; i < BOUNDRY; i++)
	assert (overflow[i] == 0);
      
      underflow = dst - BOUNDRY;
      for (i = 0; i < BOUNDRY; i++)
	assert (underflow[i] == 0);
            
      memset(srcbuf, 0, sizeof(srcbuf));
      memset(dstbuf, 0, sizeof(dstbuf));
    }
  }

  for (i =0; i < BUFSIZE; i++)
    srcbuf[i] = i & 0xff;

  Core_memcpy(dstbuf, srcbuf, BUFSIZE);

  start = GetTimeBase();
  for (i = 0; i < NUM_LOOPS; i++)
    Core_memcpy(dstbuf, srcbuf, BUFSIZE);
  end = GetTimeBase();
  
  for (i =0; i < BUFSIZE; i++)
    assert (dstbuf[i] == (i & 0xff));

  printf ("Copy throughput %5.3g bytes/cycle \n", (double)(BUFSIZE * NUM_LOOPS)/(double)(end - start));
  
  return 0;
}
