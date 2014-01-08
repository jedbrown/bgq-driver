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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "TestBuffer.h"

	
TestBuffer::TestBuffer(long isize, int worldRank)
: size(isize), rank(worldRank)
{
	 // make at least 32 bytes or multiple of 32 bytes    	
	if (size%32) size += (32 - size%32);
	if (size == 0) size = 32;
    
  	p = (char*)malloc(size);
    
    if (p == NULL) {
    	die("ERROR: Unable to allocate memory size %ld\n", isize);
    }
    curpos = p;
    Init();
}


TestBuffer::~TestBuffer()
{
	free((void*)p);
    p = NULL;
}

void TestBuffer::Init(time_t iseed)
{
   	seed = iseed;
   	if (iseed == 0) { seed = time(NULL); } 
   	srand48(seed);
   	
    for (long i=0; i<(size/4); ++i) {
        ((unsigned int*)p)[i] = lrand48();
    }
}

void TestBuffer::Check()
{
	srand48(seed);
    for (long i=0; i<size/4; ++i) {
        if (((unsigned int*)p)[i] != (unsigned int)lrand48()) {
            die("ERROR: miscompare in buffer data in word at byte %ld\n", i*sizeof(long));
        }
    }        
}
    
void TestBuffer::Check(long trgOffset, TestBuffer & src, long srcOffset, long len)
{
	char *ptrg = p + trgOffset;
	char *psrc = src.p + srcOffset;
	if (len < 0) len = size;
	if (memcmp(ptrg, psrc, len) != 0) {
		// find failing offset
	    while ((*ptrg == *psrc) && (len > 0)) { ptrg++; psrc++; len--; }
	    if (len <= 0) { 
	    	die("ERROR: (rank %d) memcmp fail, but offset check didn't find an error\n", rank);
	    }
	    long offset = (ptrg - p); 
    	char strbuf[8096];
    	long slen = 0;
    	slen += sprintf(strbuf+slen, "Actual (rank %d):", rank);
    	slen += SPrintBytes(strbuf+slen, trgOffset, offset);
    	slen += sprintf(strbuf+slen, "Expected:");   
    	slen += src.SPrintBytes(strbuf+slen, srcOffset, offset);
        die("ERROR: miscompare in buffer data in at byte 0x%08lx\nbufptr=0x%08lx, bufOffset=0x%08lx, chkptr=0x%08lx, chkOffset=0x%08lx\n%s\n", offset, (ulong)(p), trgOffset, (ulong)(src.p), srcOffset, strbuf);
	}
}

long TestBuffer::SPrintBytes(char *strbuf, long startOffset, long offset, long window)
{
	long imin = ((offset - window) / 16) * 16;
	imin = MAX(imin, 0);
	long imax = ((offset + window + 16) / 16) * 16;
	imax = MIN(imax, size-startOffset);
	long len = 0;
	for (long i=imin; i<imax; ++i) {
		if ((i % 16) == 0)     len += sprintf(strbuf+len, "\n0x%08lx: ", i);  
		else if ((i % 8) == 0) len += sprintf(strbuf+len, "  "); 
		else if ((i % 4) == 0) len += sprintf(strbuf+len, " ");
		
		len += sprintf(strbuf+len, "%02hhx", p[i+startOffset]); 
    }            	
	len += sprintf(strbuf+len, "\n");
	return len;
}

void TestBuffer::Copy(TestBuffer & src) {
	int minsize = MIN( size, src.size );
	memcpy(head(), src.head(), minsize);    
}

void TestBuffer::Copy(long trgOffset, TestBuffer & src, long srcOffset, long len)
{
	memcpy(p+trgOffset, src.p+srcOffset, len);
}



