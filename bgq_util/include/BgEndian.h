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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef __BG_ENDIAN_H__
#define __BG_ENDIAN_H__

#include <stdint.h>
#include <assert.h>
#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

#if 0
const int BgEndian1 = 1;
#define isBGendian() ( (*(char*)&BgEndian1) == 0 )
#else
__INLINE__ int isBGendian(void) {
  int BgEndian1 = 1;
  return ( (*(char*)&BgEndian1) == 0 );
}
#endif

__INLINE__ void inplaceEndianFlip(void *v, int bytes) {
  assert(bytes%2==0);
  uint8_t tmp;
  uint8_t *u = (uint8_t *)v;
  int i;
  for (i=0; i<bytes/2; i++) {
    tmp = u[i];
    u[i] = u[bytes-1-i];
    u[bytes-1-i] = tmp;
  }
}

__INLINE__ void maybe_inplaceEndianFlip(int c, void *v, int bytes) {
  if (c) {
    assert(bytes%2==0);
    uint8_t tmp;
    uint8_t *u = (uint8_t *)v;
    int i;
    for (i=0; i<bytes/2; i++) {
      tmp = u[i];
      u[i] = u[bytes-1-i];
      u[bytes-1-i] = tmp;
    }
  }
}


__INLINE__ void nonBG_inplaceEndianFlip(int c, void *v, int bytes) {
  maybe_inplaceEndianFlip(!isBGendian(), v, bytes);
}

// ---------

__INLINE__ double double_EndianFlip(double i) {
  double f = i; // Do not flip i !!!!!!!
  inplaceEndianFlip(&f, sizeof(f));
  return f;
}

__INLINE__ double maybe_double_EndianFlip(int c, double i) {
  double f = i; // Do not flip i !!!!!!!
  maybe_inplaceEndianFlip(c,&f, sizeof(f));
  return f;
}

__INLINE__ double nonBG_double_EndianFlip(double i) {
  return maybe_double_EndianFlip(!isBGendian(),i);
}

// ---------

__INLINE__ int64_t int64_EndianFlip(int64_t i) {
  int64_t f = i; // Do not flip i !!!!!!!
  inplaceEndianFlip(&f, sizeof(f));
  return f;
}

__INLINE__ int64_t maybe_int64_EndianFlip(int c, int64_t i) {
  int64_t f = i; // Do not flip i !!!!!!!
  maybe_inplaceEndianFlip(c,&f, sizeof(f));
  return f;
}

__INLINE__ int64_t nonBG_int64_EndianFlip(int64_t i) {
  return maybe_int64_EndianFlip(!isBGendian(),i);
}

// ---------

__INLINE__ uint16_t uint16_EndianFlip(uint16_t i) {
  uint16_t f = i; // Do not flip i !!!!!!!
  inplaceEndianFlip(&f, sizeof(f));
  return f;
}

__INLINE__ uint16_t maybe_uint16_EndianFlip(int c, uint16_t i) {
  uint16_t f = i; // Do not flip i !!!!!!!
  maybe_inplaceEndianFlip(c,&f, sizeof(f));
  return f;
}

__INLINE__ uint16_t nonBG_uint16_EndianFlip(uint16_t i) {
  return maybe_uint16_EndianFlip(!isBGendian(),i);
}

// ---------

__INLINE__ uint32_t uint32_EndianFlip(uint32_t i) {
  uint32_t f = i; // Do not flip i !!!!!!!
  inplaceEndianFlip(&f, sizeof(f));
  return f;
}

__INLINE__ uint32_t maybe_uint32_EndianFlip(int c, uint32_t i) {
  uint32_t f = i; // Do not flip i !!!!!!!
  maybe_inplaceEndianFlip(c,&f, sizeof(f));
  return f;
}

__INLINE__ uint32_t nonBG_uint32_EndianFlip(uint32_t i) {
  return maybe_uint32_EndianFlip(!isBGendian(),i);
}

// ---------

__INLINE__ uint64_t uint64_EndianFlip(uint64_t i) {
  uint64_t f = i; // Do not flip i !!!!!!!
  inplaceEndianFlip(&f, sizeof(f));
  return f;
}

__INLINE__ uint64_t maybe_uint64_EndianFlip(int c, uint64_t i) {
  uint64_t f = i; // Do not flip i !!!!!!!
  maybe_inplaceEndianFlip(c,&f, sizeof(f));
  return f;
}

__INLINE__ uint64_t nonBG_uint64_EndianFlip(uint64_t i) {
  return maybe_uint64_EndianFlip(!isBGendian(),i);
}

__END_DECLS

#endif // __BG_ENDIAN_H__
