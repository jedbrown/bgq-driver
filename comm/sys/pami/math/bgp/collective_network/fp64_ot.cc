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
 * \file math/bgp/collective_network/fp64_ot.cc
 * \brief Optimized math routines for 64 bit floating point operations on
 *        the ppc 450 dual fpu architecture.
 */
#include "util/common.h"
//#include "ppc450d/internal_o.h"
#include "math/FloatUtil.h"
#include "math/bgp/collective_network/DblUtils.h"
#include "math/bgp/collective_network/CNUtil.h"
#include "components/devices/bgp/collective_network/CollectiveNetworkLib.h"
#include "math/bgp/collective_network/pami_optibgmath.h"

void _pami_core_fp64_pre_max_o(double *dst, const double *src, int count) {
  uint64_t *d=(uint64_t*)dst, *s=(uint64_t*)src;
  register int n=0;
  register unsigned long long d0, d1, d2, d3;
  register unsigned long long s0, s1, s2, s3;
  for(; n<count-3; n+=4)
    {
      s0 = s[n+0];
      s1 = s[n+1];
      s2 = s[n+2];
      s3 = s[n+3];

      d0 = MAX_DOUBLE_TOTREE(s0);
      d1 = MAX_DOUBLE_TOTREE(s1);
      d2 = MAX_DOUBLE_TOTREE(s2);
      d3 = MAX_DOUBLE_TOTREE(s3);

      d[n+0] = d0;
      d[n+1] = d1;
      d[n+2] = d2;
      d[n+3] = d3;
    }
  for(; n<count; n++)
    {
      s0   = s[n];
      d0   = MAX_DOUBLE_TOTREE(s0);
      d[n] = d0;
    }
}

void _pami_core_fp64_post_max_o(double *dst, const double *src, int count) {
  uint64_t *d=(uint64_t*)dst, *s=(uint64_t*)src;
  register int n=0;
  register unsigned long long d0, d1, d2, d3;
  register unsigned long long s0, s1, s2, s3;
  for(; n<count-3; n+=4)
    {
      s0 = s[n+0];
      s1 = s[n+1];
      s2 = s[n+2];
      s3 = s[n+3];

      d0 = MAX_DOUBLE_FRTREE(s0);
      d1 = MAX_DOUBLE_FRTREE(s1);
      d2 = MAX_DOUBLE_FRTREE(s2);
      d3 = MAX_DOUBLE_FRTREE(s3);

      d[n+0] = d0;
      d[n+1] = d1;
      d[n+2] = d2;
      d[n+3] = d3;
    }
  for(; n<count; n++)
    {
      s0   = s[n];
      d0   = MAX_DOUBLE_FRTREE(s0);
      d[n] = d0;
    }
}

void _pami_core_fp64_int32_pre_maxloc_o(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
  register int n = 0;
  uint64_int32_t *d=(uint64_int32_t*)dst, *s=(uint64_int32_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MAX_DOUBLE_TOTREE(s[n].a);
      d[n].b = LOC_INT_TOTREE(s[n].b);
      d[n].z = 0;
    }
}

void _pami_core_fp64_int32_post_maxloc_o(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
  register int n = 0;
  uint64_int32_t *d=(uint64_int32_t*)dst, *s=(uint64_int32_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MAX_DOUBLE_FRTREE(s[n].a);
      d[n].b = LOC_INT_FRTREE(s[n].b);
    }
}

void _pami_core_fp64_fp64_pre_maxloc_o(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
  register int n = 0;
  uint64_uint64_t *d=(uint64_uint64_t*)dst, *s=(uint64_uint64_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MAX_DOUBLE_TOTREE(s[n].a);
      d[n].b = MIN_DOUBLE_TOTREE(s[n].b);
    }
}

void _pami_core_fp64_fp64_post_maxloc_o(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
  register int n = 0;
  uint64_uint64_t *d=(uint64_uint64_t*)dst, *s=(uint64_uint64_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MAX_DOUBLE_FRTREE(s[n].a);
      d[n].b = MIN_DOUBLE_FRTREE(s[n].b);
    }
}

void _pami_core_fp64_pre_min_o(double *dst, const double *src, int count) {
  uint64_t *d=(uint64_t*)dst, *s=(uint64_t*)src;
  register int n=0;
  register unsigned long long d0, d1, d2, d3;
  register unsigned long long s0, s1, s2, s3;
  for(; n<count-3; n+=4)
    {
      s0 = s[n+0];
      s1 = s[n+1];
      s2 = s[n+2];
      s3 = s[n+3];

      d0 = MIN_DOUBLE_TOTREE(s0);
      d1 = MIN_DOUBLE_TOTREE(s1);
      d2 = MIN_DOUBLE_TOTREE(s2);
      d3 = MIN_DOUBLE_TOTREE(s3);

      d[n+0] = d0;
      d[n+1] = d1;
      d[n+2] = d2;
      d[n+3] = d3;
    }
  for(; n<count; n++)
    {
      s0   = s[n];
      d0   = MIN_DOUBLE_TOTREE(s0);
      d[n] = d0;
    }
}

void _pami_core_fp64_post_min_o(double *dst, const double *src, int count) {
  uint64_t *d=(uint64_t*)dst, *s=(uint64_t*)src;
  register int n=0;
  register unsigned long long d0, d1, d2, d3;
  register unsigned long long s0, s1, s2, s3;
  for(; n<count-3; n+=4)
    {
      s0 = s[n+0];
      s1 = s[n+1];
      s2 = s[n+2];
      s3 = s[n+3];

      d0 = MIN_DOUBLE_FRTREE(s0);
      d1 = MIN_DOUBLE_FRTREE(s1);
      d2 = MIN_DOUBLE_FRTREE(s2);
      d3 = MIN_DOUBLE_FRTREE(s3);

      d[n+0] = d0;
      d[n+1] = d1;
      d[n+2] = d2;
      d[n+3] = d3;
    }
  for(; n<count; n++)
    {
      s0   = s[n];
      d0   = MIN_DOUBLE_FRTREE(s0);
      d[n] = d0;
    }
}

void _pami_core_fp64_int32_pre_minloc_o(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
  register int n = 0;
  uint64_int32_t *d=(uint64_int32_t*)dst, *s=(uint64_int32_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MIN_DOUBLE_TOTREE(s[n].a);
      d[n].b = LOC_INT_TOTREE(s[n].b);
      d[n].z = 0;
    }
}

void _pami_core_fp64_int32_post_minloc_o(fp64_int32_t *dst, const fp64_int32_t *src, int count) {
  register int n = 0;
  uint64_int32_t *d=(uint64_int32_t*)dst, *s=(uint64_int32_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MIN_DOUBLE_FRTREE(s[n].a);
      d[n].b = LOC_INT_FRTREE(s[n].b);
    }
}

void _pami_core_fp64_fp64_pre_minloc_o(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
  register int n = 0;
  uint64_uint64_t *d=(uint64_uint64_t*)dst, *s=(uint64_uint64_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MIN_DOUBLE_TOTREE(s[n].a);
      d[n].b = MIN_DOUBLE_TOTREE(s[n].b);
    }
}

void _pami_core_fp64_fp64_post_minloc_o(fp64_fp64_t *dst, const fp64_fp64_t *src, int count) {
  register int n = 0;
  uint64_uint64_t *d=(uint64_uint64_t*)dst, *s=(uint64_uint64_t*)src;
  for (n = 0; n < count; n++)
    {
      d[n].a = MIN_DOUBLE_FRTREE(s[n].a);
      d[n].b = MIN_DOUBLE_FRTREE(s[n].b);
    }
}
