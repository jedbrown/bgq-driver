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
 * \file common/type/TypeFunc.h
 * \brief ???
 */
#ifndef __common_type_TypeFunc_h__
#define __common_type_TypeFunc_h__

#include "TypeCode.h"
#include "Math.h"

namespace PAMI
{
  namespace Type
  {
    namespace Func
    {
      static void error (void *to, void *from, size_t size, void *cookie)
      {
        // Avoid unused warnings
        (void)to;(void)from;(void)size;(void)cookie;
        // Bad!
        abort();
      };

      template <typename T>
      static void copy (void *to, void *from, size_t bytes, void *cookie) {
        (void)cookie;
        // just use memcpy
        memcpy(to, from, bytes);
      }

      static void noop (void *to, void *from, size_t bytes, void *cookie)
      {
        (void)to;(void)from;(void)bytes;(void)cookie;
      };

      template <typename T>
      static void max (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::max<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void min (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::min<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void sum (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::sum<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void prod (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::prod<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void land (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::land<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void lor (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::lor<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void lxor (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::lxor<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void band (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::band<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void bor (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::bor<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void bxor (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::bxor<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void maxloc (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::maxloc<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <typename T>
      static void minloc (void *to, void *from, size_t bytes, void *cookie) { (void)cookie;Math::minloc<T>((T*)to, (T*)from, bytes/sizeof(T)); }

      template <>
      inline void sum< TypeCode::primitive_complex_t<float> > (void   * dst,
                                                               void   * src,
                                                               size_t   bytes,
                                                               void   * cookie)
      {
        (void) cookie; // cookie is unused
        TypeCode::primitive_complex_t<float> * d =
          (TypeCode::primitive_complex_t<float> *) dst;
        TypeCode::primitive_complex_t<float> * s =
          (TypeCode::primitive_complex_t<float> *) src;

        unsigned i, count = bytes / sizeof(TypeCode::primitive_complex_t<float>);

        for (i = 0; i < count; i++)
          {
            d[i].real      += s[i].real;
            d[i].imaginary += s[i].imaginary;
          }
      }

      template <>
      inline void prod< TypeCode::primitive_complex_t<float> > (void   * dst,
                                                                void   * src,
                                                                size_t   bytes,
                                                                void   * cookie)
      {
        (void) cookie; // cookie is unused
        TypeCode::primitive_complex_t<float> * d =
          (TypeCode::primitive_complex_t<float> *) dst;
        TypeCode::primitive_complex_t<float> * s =
          (TypeCode::primitive_complex_t<float> *) src;

        unsigned n, count = bytes / sizeof(TypeCode::primitive_complex_t<float>);

        for (n = 0; n < count; n++)
          {
            float r = (d[n].real * s[n].real) - (d[n].imaginary * s[n].imaginary);
            float i = (d[n].imaginary * s[n].real) + (d[n].real * s[n].imaginary);

            d[n].real      = r;
            d[n].imaginary = i;
          }
      }

      template <>
      inline void sum< TypeCode::primitive_complex_t<double> > (void   * dst,
                                                                void   * src,
                                                                size_t   bytes,
                                                                void   * cookie)
      {
        (void) cookie; // cookie is unused
        TypeCode::primitive_complex_t<double> * d =
          (TypeCode::primitive_complex_t<double> *) dst;
        TypeCode::primitive_complex_t<double> * s =
          (TypeCode::primitive_complex_t<double> *) src;

        unsigned i, count = bytes / sizeof(TypeCode::primitive_complex_t<double>);

        for (i = 0; i < count; i++)
          {
            d[i].real      += s[i].real;
            d[i].imaginary += s[i].imaginary;
          }
      }

      template <>
      inline void prod< TypeCode::primitive_complex_t<double> > (void   * dst,
                                                                 void   * src,
                                                                 size_t   bytes,
                                                                 void   * cookie)
      {
        (void) cookie; // cookie is unused
        TypeCode::primitive_complex_t<double> * d =
          (TypeCode::primitive_complex_t<double> *) dst;
        TypeCode::primitive_complex_t<double> * s =
          (TypeCode::primitive_complex_t<double> *) src;

        unsigned n, count = bytes / sizeof(TypeCode::primitive_complex_t<double>);

        for (n = 0; n < count; n++)
          {
            double r = (d[n].real * s[n].real) - (d[n].imaginary * s[n].imaginary);
            double i = (d[n].imaginary * s[n].real) + (d[n].real * s[n].imaginary);

            d[n].real      = r;
            d[n].imaginary = i;
          }
      }

    };

    class TypeFunc
    {
      public:

        typedef void (*CopyFunction)(void *to, void *from, size_t size, void *cookie);



        typedef enum
        {
          PRIMITIVE_FUNC_COPY = 0,
          PRIMITIVE_FUNC_NOOP,
          PRIMITIVE_FUNC_MAX,
          PRIMITIVE_FUNC_MIN,
          PRIMITIVE_FUNC_SUM,
          PRIMITIVE_FUNC_PROD,
          PRIMITIVE_FUNC_LAND,
          PRIMITIVE_FUNC_LOR,
          PRIMITIVE_FUNC_LXOR,
          PRIMITIVE_FUNC_BAND,
          PRIMITIVE_FUNC_BOR,
          PRIMITIVE_FUNC_BXOR,
          PRIMITIVE_FUNC_MAXLOC,
          PRIMITIVE_FUNC_MINLOC,

          PRIMITIVE_FUNC_COUNT

        } primitive_func_t;

        static void GetEnums(pami_type_t  dt,     pami_data_function op,
                             uintptr_t   &out_dt, uintptr_t          &out_op)
        {
          TypeCode *stype = (TypeCode *)dt;
          out_dt          = stype->GetPrimitive();
          out_op          = (uintptr_t)op;
        }

        static CopyFunction GetCopyFunction (TypeCode::primitive_type_t type,
                                             TypeFunc::primitive_func_t func)
        {

          CopyFunction array[TypeCode::PRIMITIVE_TYPE_COUNT][TypeFunc::PRIMITIVE_FUNC_COUNT] =
          {
            // --------------------------------- TypeCode::PRIMITIVE_TYPE_BYTE
            {
              Type::Func::copy<uint8_t>,            // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<uint8_t>,             // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<uint8_t>,             // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<uint8_t>,             // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<uint8_t>,            // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<uint8_t>,            // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<uint8_t>,             // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<uint8_t>,            // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<uint8_t>,            // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<uint8_t>,             // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<uint8_t>,            // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_SIGNED_CHAR
            {
              Type::Func::copy<signed char>,        // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<signed char>,         // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<signed char>,         // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<signed char>,         // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<signed char>,        // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<signed char>,        // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<signed char>,         // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<signed char>,        // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<signed char>,        // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<signed char>,         // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<signed char>,        // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_SIGNED_SHORT
            {
              Type::Func::copy<signed short>,       // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<signed short>,        // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<signed short>,        // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<signed short>,        // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<signed short>,       // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<signed short>,       // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<signed short>,        // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<signed short>,       // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<signed short>,       // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<signed short>,        // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<signed short>,       // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_SIGNED_INT
            {
              Type::Func::copy<signed int>,         // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<signed int>,          // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<signed int>,          // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<signed int>,          // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<signed int>,         // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<signed int>,         // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<signed int>,          // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<signed int>,         // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<signed int>,         // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<signed int>,          // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<signed int>,         // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_SIGNED_LONG
            {
              Type::Func::copy<signed long>,        // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<signed long>,         // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<signed long>,         // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<signed long>,         // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<signed long>,        // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<signed long>,        // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<signed long>,         // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<signed long>,        // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<signed long>,        // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<signed long>,         // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<signed long>,        // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_SIGNED_LONG_LONG
            {
              Type::Func::copy<signed long long>,   // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<signed long long>,    // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<signed long long>,    // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<signed long long>,    // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<signed long long>,   // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<signed long long>,   // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<signed long long>,    // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<signed long long>,   // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<signed long long>,   // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<signed long long>,    // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<signed long long>,   // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_UNSIGNED_CHAR
            {
              Type::Func::copy<unsigned char>,      // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<unsigned char>,       // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<unsigned char>,       // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<unsigned char>,       // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<unsigned char>,      // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<unsigned char>,      // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<unsigned char>,       // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<unsigned char>,      // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<unsigned char>,      // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<unsigned char>,       // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<unsigned char>,      // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_UNSIGNED_SHORT
            {
              Type::Func::copy<unsigned short>,     // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<unsigned short>,      // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<unsigned short>,      // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<unsigned short>,      // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<unsigned short>,     // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<unsigned short>,     // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<unsigned short>,      // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<unsigned short>,     // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<unsigned short>,     // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<unsigned short>,      // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<unsigned short>,     // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_UNSIGNED_INT
            {
              Type::Func::copy<unsigned int>,       // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<unsigned int>,        // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<unsigned int>,        // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<unsigned int>,        // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<unsigned int>,       // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<unsigned int>,       // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<unsigned int>,        // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<unsigned int>,       // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<unsigned int>,       // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<unsigned int>,        // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<unsigned int>,       // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_UNSIGNED_LONG
            {
              Type::Func::copy<unsigned long>,      // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<unsigned long>,       // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<unsigned long>,       // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<unsigned long>,       // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<unsigned long>,      // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<unsigned long>,      // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<unsigned long>,       // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<unsigned long>,      // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<unsigned long>,      // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<unsigned long>,       // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<unsigned long>,      // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_UNSIGNED_LONG_LONG
            {
              Type::Func::copy<unsigned long long>, // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<unsigned long long>,  // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<unsigned long long>,  // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<unsigned long long>,  // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<unsigned long long>, // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<unsigned long long>, // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<unsigned long long>,  // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<unsigned long long>, // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::band<unsigned long long>, // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::bor<unsigned long long>,  // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::bxor<unsigned long long>, // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_FLOAT
            {
              Type::Func::copy<float>,              // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<float>,               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<float>,               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<float>,               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<float>,              // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_DOUBLE
            {
              Type::Func::copy<double>,             // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<double>,              // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<double>,              // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<double>,              // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<double>,             // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LONG_DOUBLE
            {
              Type::Func::copy<long double>,        // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                 // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::max<long double>,         // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::min<long double>,         // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<long double>,         // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<long double>,        // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                 // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOGICAL1
            {
              Type::Func::copy<TypeCode::primitive_logical1_t>,// TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<TypeCode::primitive_logical1_t>,// TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<TypeCode::primitive_logical1_t>, // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<TypeCode::primitive_logical1_t>,// TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                                // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOGICAL2
            {
              Type::Func::copy<TypeCode::primitive_logical2_t>,// TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<TypeCode::primitive_logical2_t>,// TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<TypeCode::primitive_logical2_t>, // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<TypeCode::primitive_logical2_t>,// TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                                // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOGICAL4
            {
              Type::Func::copy<TypeCode::primitive_logical4_t>,// TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<TypeCode::primitive_logical4_t>,// TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<TypeCode::primitive_logical4_t>, // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<TypeCode::primitive_logical4_t>,// TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                                // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOGICAL8
            {
              Type::Func::copy<TypeCode::primitive_logical8_t>,// TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::land<TypeCode::primitive_logical8_t>,// TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::lor<TypeCode::primitive_logical8_t>, // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::lxor<TypeCode::primitive_logical8_t>,// TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                                // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_SINGLE_COMPLEX
            {
              Type::Func::copy<TypeCode::primitive_complex_t<float> >,    // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                             // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<TypeCode::primitive_complex_t<float> >,     // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<TypeCode::primitive_complex_t<float> >,    // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                             // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_DOUBLE_COMPLEX
            {
              Type::Func::copy<TypeCode::primitive_complex_t<double> >,   // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                             // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::sum<TypeCode::primitive_complex_t<double> >,    // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::prod<TypeCode::primitive_complex_t<double> >,   // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::error,                            // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::error                             // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },


            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOC_2INT
            {
              Type::Func::copy<TypeCode::primitive_loc_t<int, int> >,        // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::maxloc<TypeCode::primitive_loc_t<int, int> >,      // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::minloc<TypeCode::primitive_loc_t<int, int> >       // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOC_2FLOAT
            {
              Type::Func::copy<TypeCode::primitive_loc_t<float, float> >,    // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::maxloc<TypeCode::primitive_loc_t<float, float> >,  // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::minloc<TypeCode::primitive_loc_t<float, float> >   // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOC_2DOUBLE
            {
              Type::Func::copy<TypeCode::primitive_loc_t<double, double> >,  // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::maxloc<TypeCode::primitive_loc_t<double, double> >, // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::minloc<TypeCode::primitive_loc_t<double, double> > // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOC_SHORT_INT
            {
              Type::Func::copy<TypeCode::primitive_loc_t<short, int> >,      // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::maxloc<TypeCode::primitive_loc_t<short, int, 2> >,    // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::minloc<TypeCode::primitive_loc_t<short, int, 2> >     // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOC_FLOAT_INT
            {
              Type::Func::copy<TypeCode::primitive_loc_t<float, int> >,      // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::maxloc<TypeCode::primitive_loc_t<float, int> >,    // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::minloc<TypeCode::primitive_loc_t<float, int> >     // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOC_DOUBLE_INT
            {
              Type::Func::copy<TypeCode::primitive_loc_t<double, int, 0, 4> >,     // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::maxloc<TypeCode::primitive_loc_t<double, int, 0, 4> >,   // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::minloc<TypeCode::primitive_loc_t<double, int, 0, 4> >    // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOC_LONG_INT
            {
              Type::Func::copy<TypeCode::primitive_loc_t<long, int, 0, sizeof(long)-sizeof(int)> >,     // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::maxloc<TypeCode::primitive_loc_t<long, int, 0, sizeof(long)-sizeof(int)> >,   // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::minloc<TypeCode::primitive_loc_t<long, int, 0, sizeof(long)-sizeof(int)> >    // TypeFunc::PRIMITIVE_FUNC_MINLOC
            },

            // --------------------------------- TypeCode::PRIMITIVE_TYPE_LOC_LONGDOUBLE_INT
            {
              Type::Func::copy<TypeCode::primitive_loc_t<long double, int, 0, sizeof(long double)-sizeof(int)> >,     // TypeFunc::PRIMITIVE_FUNC_COPY
              Type::Func::noop,                                // TypeFunc::PRIMITIVE_FUNC_NOOP
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MAX
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_MIN
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_SUM
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_PROD
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_LXOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BAND
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BOR
              Type::Func::error,                               // TypeFunc::PRIMITIVE_FUNC_BXOR
              Type::Func::maxloc<TypeCode::primitive_loc_t<long double, int, 0, sizeof(long double)-sizeof(int)> >,   // TypeFunc::PRIMITIVE_FUNC_MAXLOC
              Type::Func::minloc<TypeCode::primitive_loc_t<long double, int, 0, sizeof(long double)-sizeof(int)> >    // TypeFunc::PRIMITIVE_FUNC_MINLOC
            }
          };

          return array[type][func];
        };
    };
  };
};


typedef enum
{
  PAMI_COPY   = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_COPY,
  PAMI_NOOP   = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_NOOP,
  PAMI_MAX    = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_MAX,
  PAMI_MIN    = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_MIN,
  PAMI_SUM    = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_SUM,
  PAMI_PROD   = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_PROD,
  PAMI_LAND   = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_LAND,
  PAMI_LOR    = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_LOR,
  PAMI_LXOR   = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_LXOR,
  PAMI_BAND   = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_BAND,
  PAMI_BOR    = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_BOR,
  PAMI_BXOR   = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_BXOR,
  PAMI_MAXLOC = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_MAXLOC,
  PAMI_MINLOC = PAMI::Type::TypeFunc::PRIMITIVE_FUNC_MINLOC,

  PAMI_OP_COUNT=PAMI::Type::TypeFunc::PRIMITIVE_FUNC_COUNT
} pami_op;


#endif // __common_type_TypeFunc_h__
