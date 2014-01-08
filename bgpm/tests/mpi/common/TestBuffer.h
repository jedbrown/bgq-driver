/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#ifndef TestBuffer_h
#define TestBuffer_h

#include <string.h>
#include <time.h>

#ifndef die
#define die(...) {fprintf(stderr, __VA_ARGS__); exit(-1);}
#endif

#ifndef MIN
#define MAX(a,b) (a < b ? b : a)
#define MIN(a,b) (a < b ? a : b)
#endif




class TestBuffer
{
  public:
    TestBuffer(long isize, int worldRank = -1); 
    ~TestBuffer(); 
    void Init(time_t iseed=0); 
    void Check(); 
    void Check(long trgOffset, TestBuffer & src, long srcOffset = 0, long len = -1); 
    long SPrintBytes(char *strbuf, long startOffset, long offset, long window=32);
    void Copy(TestBuffer & src);
    void Copy(long trgOffset, TestBuffer & src, long srcOffset, long len);
    
    TestBuffer & reset() { curpos = p; return *this; }  // reset before run in case current op size may exceed curpos to end of buff
    TestBuffer & advance(long numBytes) {  // advance curpos by given amount - wrap if given amount isn't available.
        curpos += numBytes;
        if (curpos > (p + size - numBytes)) curpos = p;
        return *this;
    }       
    void *head()     { return (void*)p; }      // return head of buffer.
    operator void*() { return (void*)curpos; }
    operator char*() { return curpos; }
    char* p;
    char* curpos;                       // next write/read position into buffer
    long size;
    time_t seed;
    int rank;
};

#endif
