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
 * \file arch/ArchInterface.h
 * \brief Defines common and default macros and checks for required macro definitions.
 */

#ifndef __arch_ArchInterface_h__
#define __arch_ArchInterface_h__

#include <stdint.h>   // This may not be platform-independent.
#include <stddef.h>   // This may not be platform-independent.

#ifdef __cplusplus
///
/// \brief Encapuslating class for static methods which allows partial template specialization
///
/// A specific architecture may create a template specialization to optimize
/// the copy for a type.
///
/// \code
/// /* Example template specialization for uint32_t type. */
///
/// template <>
/// template <unsigned N>
/// void Type<uint32_t>::copy (uint32_t * dst, uint32_t * src)
/// {
///   size_t i;
///   for (i=0; i<(N>>2); i++) dst[i] = src[i];
///
///   if (N & 0x03)
///   {
///     uint8_t * const d = (uint8_t * const) dst[N>>2];
///     uint8_t * const s = (uint8_t * const) src[N>>2];
///     for (i=0; i<(N&0x03); i++) d[i] = s[i];
///   }
/// };
/// \endcode
///
template <class T>
class Type
{
  public:
    inline  Type () {};
    inline ~Type () {};

    template <unsigned N>
    static inline void copy (T * dst, T * src)
    {
      uint8_t * const d = (uint8_t * const) dst;
      uint8_t * const s = (uint8_t * const) src;

      size_t i;
      for (i=0; i<N; i++) d[i] = s[i];
    };
};
#endif // __cplusplus

#endif // __arch_ArchInterface_h__
