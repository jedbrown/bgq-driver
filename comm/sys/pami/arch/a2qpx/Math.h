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
 * \file arch/a2qpx/Math.h
 * \brief ...
 */

#ifndef __arch_a2qpx_Math_h__
#define __arch_a2qpx_Math_h__

#include "arch/MathInterface.h"

namespace PAMI
{
  namespace Math
  {
#if 0
    template <>
    inline void sum<double> (double * dst, double * src, size_t count)
    {
      // platform-optimized double-sum math
      fprintf (stderr, "a2qpx double sum template specialization.. dst = %p, src = %p, count = %zu\n", dst, src, count);

      unsigned i;

      for (i = 0; i < count; i++)
        {
          fprintf (stderr, "a2qpx double sum template specialization .. src[%d] = %e, dst[%d] = %e -> %e\n", i, src[i], i, dst[i], dst[i] + src[i]);
          dst[i] += src[i];
        }
    }

    inline void sum (double (&dst)[64], double (&src)[64])
    {
      // platform-optimized double-sum 64 count math
      fprintf (stderr, "a2qpx sum 64\n");
    }
#endif
  };
};
#endif // __arch_a2qpx_Math_h__
