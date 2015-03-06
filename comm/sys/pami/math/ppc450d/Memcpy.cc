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
 * \file math/ppc450d/Memcpy.cc
 * \brief ???
 */
#include <stdint.h>
#include "Util.h"

static inline void *_ByteCpy( void *dest, const void *src, size_t n )
{
   const uint8_t *sp = (const uint8_t *)src;
   uint8_t *dp = (uint8_t *)dest;
   size_t n4 = n >> 2;
   size_t nt = n & 3;

   while ( n4-- )
      {
      uint8_t a = *(sp+0);
      uint8_t b = *(sp+1);
      uint8_t c = *(sp+2);
      uint8_t d = *(sp+3);
      *(dp+0) = a;
      *(dp+1) = b;
      *(dp+2) = c;
      *(dp+3) = d;
      sp += 4;
      dp += 4;
      }

   while( nt-- )
      *dp++ = *sp++;

   return( dest );
}

static inline void *_WordCpy( void *dest, const void *src , size_t n )
{
   const uint32_t *sp = (const uint32_t *)src;
   uint32_t *dp = (uint32_t *)dest;
   size_t words  = n >> 2;
   size_t dwords = n >> 3;
   size_t new_n  = n&0x3;

   while ( dwords-- )
      {
      uint32_t a = *(sp+0);
      uint32_t b = *(sp+1);
      *(dp+0) = a;
      *(dp+1) = b;
      sp += 2;
      dp += 2;
      }
   if ( words & 1 )
      *dp++ = *sp++;

   if ( new_n )
      _ByteCpy( dp, sp, new_n );

   return( dest );
}

static inline void *_QuadCpy( void *dest, const void *src, size_t n )
{
   register int u = 16;
   const uint8_t *sp = (const uint8_t *)src;
   uint8_t *dp = (uint8_t *)dest;

   int q64 = n >> 6;
   n &= 0x3F;

   sp -= 16;
   dp -= 16;

   while ( q64-- )
      {
      asm volatile( "lfpdux  1,%0,%2;"
                    "lfpdux  2,%0,%2;"
                    "lfpdux  3,%0,%2;"
                    "lfpdux  4,%0,%2;"
                    "stfpdux 1,%1,%2;"
                    "stfpdux 2,%1,%2;"
                    "stfpdux 3,%1,%2;"
                    "stfpdux 4,%1,%2;"
                    : "+b" (sp),
                      "+b" (dp)
                    : "r" (u)
                    : "memory",
                      "fr1", "fr2", "fr3", "fr4" );
      }

   int q16 = (n >> 4);
   n &= 0xF;
   while ( q16-- )
      {
      asm volatile( "lfpdux   1,%0,%2;"
                    "stfpdux  1,%1,%2;"
                    : "+b" (sp),
                      "+b" (dp)
                    : "r" (u)
                    : "memory",
                      "fr1");
      }
   sp+=16;
   dp+=16;
   if ( n )
      _WordCpy( dp, sp, n );

   return( dest );
}

  /* Removed because tmg_memcpy16 uses DCBZ, which
     is not compatible with write-thru mode. */
//extern void tmg_memcpy16 (void *dst, const void *src, unsigned len);


#define OPTIMIZED_Core_memcpy
void * Core_memcpy (void *dest, const void *src, size_t n)
{
  if (((uint32_t)dest | (uint32_t)src) & 0x0f)
  {
    // Unaligned.
    return _WordCpy( dest, src, n );
  }
  else
  {
    // 16 byte aligned.
    return _QuadCpy( dest, src, n );
  }
}

#include "../Memcpy.x.h"
