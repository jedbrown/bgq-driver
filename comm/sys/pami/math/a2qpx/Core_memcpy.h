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
 * \file math/a2qpx/Core_memcpy.h
 * \brief ???
 */
#ifndef __math_a2qpx_Core_memcpy_h__
#define __math_a2qpx_Core_memcpy_h__

#define OPTIMIZED_Core_memcpy

#include <stdint.h>
#include <stdlib.h>
#include "util/common.h"
#include "math/a2qpx/a2qpx_asm.h"

static inline void* _int8Cpy( void *dest, const void *src, size_t n )
{
  const uint8_t *sp = (const uint8_t *)src;
  uint8_t *dp = (uint8_t *)dest;

  while( n-- )
    *dp++ = *sp++;

  return( dest );
}

//Copy n bytes. Assume 8 byte alignment
static inline void* _int64Cpy( char *dest, char *src , size_t n)
{  
  uint64_t *sp8 = (uint64_t *)src;
  uint64_t *dp8 = (uint64_t *)dest;

  //PPC64 supports unaligned 8 byte int loads/stores in hw
  //if (alignment) {    
  size_t qwords   =  n >> 4;
  size_t new_n    =  n & 0xf;

  uint64_t a,b;  
  while ( qwords-- )
  {
    a = *sp8++;
    b = *sp8++;
    
    *dp8++ = a;
    *dp8++ = b;
  }    

  if (new_n == 0)
    return dest;
  
  uint8_t *sp = (uint8_t*) sp8;
  uint8_t *dp = (uint8_t*) dp8;
  size_t offset = 0;
  
  size_t n_3 = new_n >> 3;
  size_t n_2 = (new_n >> 2) & 0x1;
  size_t n_1 = (new_n >> 1) & 0x1;
  size_t n_0 = new_n & 0x1;  
  
  if (n_3) {
    *(uint64_t*)dp = *(uint64_t*)sp;
    offset += 8;
  }
  
  if (n_2) {
    *(uint32_t*)(dp + offset) = *(uint32_t*) (sp + offset);
    offset += 4;
  }
  
  if (n_1) {
    *(uint16_t*)(dp + offset) = *(uint16_t*)(sp + offset);
    offset += 2;
  }
  
  if (n_0) 
    *(dp + offset) = *(sp + offset);
  
  return( dest );
}


#include "math/a2qpx/qpx_copy.h"

//Copying 512 bytes. All possible alignments
inline void* Core_memcpy_512(void* dst, void* src) {  
  uint64_t alignment = (uint64_t)dst | (uint64_t)src;
  uint64_t align32 = alignment & 0x1f;

  if ( align32 != 0 ) 
  {
    _int64Cpy( (char *)dst, (char *)src, 512);
  }
  else 
    quad_copy_512((char *)dst, (char *)src);
  return dst;
}

void* Core_memcpy(void* dst, void* src, size_t bytes);

//Copying n bytes. All possible alignments
inline void* Core_memcpy(void* dst, void* src, size_t bytes)
{
  char *sp = (char *) src;
  char *dp = (char *) dst;
  size_t nb = 0;

  uint64_t alignment = (uint64_t)dp | (uint64_t)sp;
  uint64_t align32 = alignment & 0x1f;
  
  if ( (bytes >= 128) && (align32 == 0) ) {
    if (bytes == 512) {
      quad_copy_512 (dp, sp);
      return dst;
    }
    
    nb = quad_copy_1024n(dp, sp, bytes);
    bytes -=  nb;
    sp += nb;
    dp += nb;
  }
  
  _int64Cpy( dp, sp, bytes);  
  return dst;
}


#endif
