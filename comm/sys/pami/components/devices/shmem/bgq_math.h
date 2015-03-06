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
 * \file components/devices/shmem/bgq_math.h
 * \brief ???
 */
#ifndef __components_devices_shmem_bgq_math_h__
#define __components_devices_shmem_bgq_math_h__
//#define "components/devices/shmem/a2qpx_nway_math.h"

void _pami_core_int_min_2way(int* dst, int* src0, int *src1, unsigned num_ints);
void _pami_core_int_max_2way(int* dst, int* src0, int *src1, unsigned num_ints);
void _pami_core_int_sum_2way(int* dst, int* src0, int *src1, unsigned num_ints);
inline unsigned convertToCount(unsigned bytes,pami_dt dt);

inline unsigned int_math_2way(int* dst, int* src0, int *src1, unsigned num_ints, pami_op opcode)
{
  switch (opcode)
  {
    case PAMI_SUM:
   _pami_core_int_sum_2way( dst,  src0,  src1, num_ints );
    break;

    case PAMI_MAX:
  _pami_core_int_max_2way( dst,  src0,  src1, num_ints );
      break;

    case PAMI_MIN:
  _pami_core_int_min_2way( dst,  src0,  src1, num_ints );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      signed int* srcs[2] = {src0,src1};
      coremath func = MATH_OP_FUNCS(PAMI_SIGNED_INT, opcode, 2);
      func(dst, (void**)srcs,  2, num_ints);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_ints;
}

void _pami_core_int_min_4way(int* dst, int* src0, int *src1, int* src2, int* src3, unsigned num_ints);
void _pami_core_int_max_4way(int* dst, int* src0, int *src1, int* src2, int* src3, unsigned num_ints);
void _pami_core_int_sum_4way(int* dst, int* src0, int *src1, int* src2, int* src3, unsigned num_ints);
inline unsigned int_math_4way(int* dst, int* src0, int *src1, int* src2, int* src3, unsigned num_ints, pami_op opcode)
{
  switch (opcode )
  {
    case PAMI_SUM:
   _pami_core_int_sum_4way( dst,  src0,  src1, src2,  src3, num_ints );
      break;

    case PAMI_MAX:
   _pami_core_int_max_4way( dst,  src0,  src1, src2,  src3, num_ints );
      break;

    case PAMI_MIN:
   _pami_core_int_min_4way( dst,  src0,  src1, src2,  src3, num_ints );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      signed int* srcs[4] = {src0,src1,src2,src3};
      coremath func = MATH_OP_FUNCS(PAMI_SIGNED_INT, opcode, 4);
      func(dst, (void**)srcs,  4, num_ints);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_ints;

}

void _pami_core_int_min_8way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, unsigned num_ints);
void _pami_core_int_max_8way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, unsigned num_ints);
void _pami_core_int_sum_8way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, unsigned num_ints);

inline unsigned int_math_8way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, unsigned num_ints, pami_op opcode)
{
  switch (opcode) 
  {
    case PAMI_SUM:
   _pami_core_int_sum_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, num_ints );
      break;

    case PAMI_MAX:
   _pami_core_int_max_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7,  num_ints );
      break;

    case PAMI_MIN:
   _pami_core_int_min_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7,  num_ints );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      signed int* srcs[8] = {src0,src1,src2,src3,src4,src5,src6,src7};
      coremath func = MATH_OP_FUNCS(PAMI_SIGNED_INT, opcode, 8);
      func(dst, (void**)srcs,  8, num_ints);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_ints;

}

void _pami_core_int_min_16way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, int* src8, int *src9, int* src10, int* src11,
    int* src12, int* src13, int* src14, int* src15, unsigned num_ints);
void _pami_core_int_max_16way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, int* src8, int *src9, int* src10, int* src11,
    int* src12, int* src13, int* src14, int* src15, unsigned num_ints);
void _pami_core_int_sum_16way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, int* src8, int *src9, int* src10, int* src11,
    int* src12, int* src13, int* src14, int* src15, unsigned num_ints);

inline unsigned int_math_16way(int* dst, int* src0, int *src1, int* src2, int* src3,
    int* src4, int* src5, int* src6, int* src7, int* src8, int *src9, int* src10, int* src11,
    int* src12, int* src13, int* src14, int* src15, unsigned num_ints, pami_op opcode)
{

  switch (opcode) 
  {
    case PAMI_SUM:
   _pami_core_int_sum_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_ints );
      break;

    case PAMI_MAX:
   _pami_core_int_max_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_ints );
      break;

    case PAMI_MIN:
   _pami_core_int_min_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_ints );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      signed int* srcs[16] = {src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10,src11,src12,src13,src14,src15};
      coremath func = MATH_OP_FUNCS(PAMI_SIGNED_INT, opcode, 16);
      func(dst, (void**)srcs,  16, num_ints);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_ints;

}

void _pami_core_uint_min_2way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned num_ints);
void _pami_core_uint_max_2way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned num_ints);
void _pami_core_uint_sum_2way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned num_ints);

inline unsigned uint_math_2way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned num_ints, pami_op opcode)
{
  switch (opcode)
  {
    case PAMI_SUM:
   _pami_core_uint_sum_2way( dst,  src0,  src1, num_ints );
    break;

    case PAMI_MAX:
  _pami_core_uint_max_2way( dst,  src0,  src1, num_ints );
      break;

    case PAMI_MIN:
  _pami_core_uint_min_2way( dst,  src0,  src1, num_ints );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      unsigned int* srcs[2] = {src0,src1};
      coremath func = MATH_OP_FUNCS(PAMI_UNSIGNED_INT, opcode, 2);
      func(dst, (void**)srcs,  2, num_ints);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_ints;
}

void _pami_core_uint_min_4way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3, unsigned num_ints);
void _pami_core_uint_max_4way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3, unsigned num_ints);
void _pami_core_uint_sum_4way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3, unsigned num_ints);
inline unsigned uint_math_4way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3, unsigned num_ints, pami_op opcode)
{
  switch (opcode )
  {
    case PAMI_SUM:
   _pami_core_uint_sum_4way( dst,  src0,  src1, src2,  src3, num_ints );
      break;

    case PAMI_MAX:
   _pami_core_uint_max_4way( dst,  src0,  src1, src2,  src3, num_ints );
      break;

    case PAMI_MIN:
   _pami_core_uint_min_4way( dst,  src0,  src1, src2,  src3, num_ints );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      unsigned int* srcs[4] = {src0,src1,src2,src3};
      coremath func = MATH_OP_FUNCS(PAMI_UNSIGNED_INT, opcode, 4);
      func(dst, (void**)srcs,  4, num_ints);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_ints;

}

void _pami_core_uint_min_8way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned num_ints);
void _pami_core_uint_max_8way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned num_ints);
void _pami_core_uint_sum_8way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned num_ints);

inline unsigned uint_math_8way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned num_ints, pami_op opcode)
{
  switch (opcode) 
  {
    case PAMI_SUM:
   _pami_core_uint_sum_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, num_ints );
      break;

    case PAMI_MAX:
   _pami_core_uint_max_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7,  num_ints );
      break;

    case PAMI_MIN:
   _pami_core_uint_min_8way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7,  num_ints );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      unsigned int* srcs[8] = {src0,src1,src2,src3,src4,src5,src6,src7};
      coremath func = MATH_OP_FUNCS(PAMI_UNSIGNED_INT, opcode, 8);
      func(dst, (void**)srcs,  8, num_ints);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_ints;

}

void _pami_core_uint_min_16way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned* src8, unsigned* src9, unsigned* src10, unsigned* src11, unsigned* src12, unsigned* src13, unsigned* src14, unsigned* src15, unsigned num_ints);
void _pami_core_uint_max_16way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned* src8, unsigned* src9, unsigned* src10, unsigned* src11, unsigned* src12, unsigned* src13, unsigned* src14, unsigned* src15, unsigned num_ints);
void _pami_core_uint_sum_16way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned* src8, unsigned* src9, unsigned* src10, unsigned* src11, unsigned* src12, unsigned* src13, unsigned* src14, unsigned* src15, unsigned num_ints);

inline unsigned uint_math_16way(unsigned* dst, unsigned* src0, unsigned* src1, unsigned* src2, unsigned* src3,
    unsigned* src4, unsigned* src5, unsigned* src6, unsigned* src7, unsigned* src8, unsigned* src9, unsigned* src10, unsigned* src11, unsigned* src12, unsigned* src13, unsigned* src14, unsigned* src15, unsigned num_ints, pami_op opcode)
{

  switch (opcode) 
  {
    case PAMI_SUM:
   _pami_core_uint_sum_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_ints );
      break;

    case PAMI_MAX:
   _pami_core_uint_max_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_ints );
      break;

    case PAMI_MIN:
   _pami_core_uint_min_16way( dst,  src0,  src1, src2,  src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15,  num_ints );
      break;

    case PAMI_COPY:
    case PAMI_NOOP:
    case PAMI_PROD:
    case PAMI_LAND:
    case PAMI_LOR:
    case PAMI_LXOR:
    case PAMI_BAND:
    case PAMI_BOR:
    case PAMI_BXOR:
    {
      unsigned int* srcs[16] = {src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10,src11,src12,src13,src14,src15};
      coremath func = MATH_OP_FUNCS(PAMI_UNSIGNED_INT, opcode, 16);
      func(dst, (void**)srcs,  16, num_ints);
      break;
    }
    case PAMI_MAXLOC:
    case PAMI_MINLOC:
    case PAMI_OP_COUNT:
    {
      PAMI_abortf("not supported\n");
      break;
    }
  } 
  return num_ints;

}

inline unsigned bgq_math_16way(char* dst, char* src0, char *src1, char* src2, char* src3,
    char* src4, char* src5, char* src6, char* src7, char* src8, char *src9, char* src10, char* src11,
    char* src12, char* src13, char* src14, char* src15, unsigned bytes, pami_op opcode, pami_dt dt)
{

  if (dt == PAMI_DOUBLE)
  {
    return quad_double_math_16way((double*) dst, (double*) src0, (double*)src1, (double*) src2, (double*) src3,
    (double*) src4, (double*) src5, (double*) src6, (double*) src7, (double*) src8, (double*)src9, (double*) src10, (double*) src11,
    (double*) src12, (double*) src13, (double*) src14, (double*) src15, bytes/sizeof(double), opcode);
  }
  else if (dt == PAMI_SIGNED_INT)
  {
    return int_math_16way((int*) dst, (int*) src0, (int*)src1, (int*) src2, (int*) src3,
    (int*) src4, (int*) src5, (int*) src6, (int*) src7, (int*) src8, (int*)src9, (int*) src10, (int*) src11,
    (int*) src12, (int*) src13, (int*) src14, (int*) src15, bytes/sizeof(int), opcode);
  }
  else if (dt == PAMI_UNSIGNED_INT)
  {
    return uint_math_16way((unsigned*) dst, (unsigned*) src0, (unsigned*)src1, (unsigned*) src2, (unsigned*) src3,
    (unsigned*) src4, (unsigned*) src5, (unsigned*) src6, (unsigned*) src7, (unsigned*) src8, (unsigned*)src9, (unsigned*) src10, 
    (unsigned*) src11, (unsigned*) src12, (unsigned*) src13, (unsigned*) src14, (unsigned*) src15, bytes/sizeof(unsigned), opcode);
  } 
  else // un-optimized dt/op math
  {
    char* srcs[16] = {src0,src1,src2,src3,src4,src5,src6,src7,src8,src9,src10,src11,src12,src13,src14,src15};
    coremath func = MATH_OP_FUNCS(dt, opcode, 16);
    PAMI_assertf(func!=NULL,"Unsupported dt %X, op %X\n", dt, opcode);
    unsigned count = convertToCount(bytes,dt);
    func(dst, (void**)srcs,  16, count);
    return count;
  }
}

inline unsigned bgq_math_8way(char* dst, char* src0, char *src1, char* src2, char* src3,
    char* src4, char* src5, char* src6, char* src7, unsigned bytes, pami_op opcode, pami_dt dt)
{

  if (dt == PAMI_DOUBLE)
  {
    return quad_double_math_8way((double*) dst, (double*) src0, (double*)src1, (double*) src2, (double*) src3,
    (double*) src4, (double*) src5, (double*) src6, (double*) src7, bytes/sizeof(double), opcode);
  }
  else if (dt == PAMI_SIGNED_INT)
  {
    return int_math_8way((int*) dst, (int*) src0, (int*)src1, (int*) src2, (int*) src3,
    (int*) src4, (int*) src5, (int*) src6, (int*) src7, bytes/sizeof(int), opcode);
  }
  else if (dt == PAMI_UNSIGNED_INT)
  {
    return uint_math_8way((unsigned*) dst, (unsigned*) src0, (unsigned*)src1, (unsigned*) src2, (unsigned*) src3,
    (unsigned*) src4, (unsigned*) src5, (unsigned*) src6, (unsigned*) src7, bytes/sizeof(unsigned), opcode);
  }
  else // un-optimized dt/op math
  {
    char* srcs[8] = {src0,src1,src2,src3,src4,src5,src6,src7};
    coremath func = MATH_OP_FUNCS(dt, opcode, 8);
    PAMI_assertf(func!=NULL,"Unsupported dt %X, op %X\n", dt, opcode);
    unsigned count = convertToCount(bytes,dt);
    func(dst, (void**)srcs,  8, count);
    return count;
  }
}

inline unsigned bgq_math_4way(char* dst, char* src0, char *src1, char* src2, char* src3, unsigned bytes, pami_op opcode, pami_dt dt)
{

  if (dt == PAMI_DOUBLE)
  {
   return  quad_double_math_4way((double*) dst, (double*) src0, (double*)src1, (double*) src2, (double*) src3, bytes/sizeof(double), opcode);
  }
  else if (dt == PAMI_SIGNED_INT)
  {
    return int_math_4way((int*) dst, (int*) src0, (int*)src1, (int*) src2, (int*) src3, bytes/sizeof(int), opcode);
  }
  else if (dt == PAMI_UNSIGNED_INT)
  {
    return uint_math_4way((unsigned*) dst, (unsigned*) src0, (unsigned*)src1, (unsigned*) src2, (unsigned*) src3, bytes/sizeof(unsigned), opcode);
  }
  else // un-optimized dt/op math
  {
    char* srcs[4] = {src0,src1,src2,src3}; 
    coremath func = MATH_OP_FUNCS(dt, opcode, 4);
    PAMI_assertf(func!=NULL,"Unsupported dt %X, op %X\n", dt, opcode);
    unsigned count = convertToCount(bytes,dt);
    func(dst, (void**)srcs,  4, count);
    return count;
  }
}

inline unsigned bgq_math_2way(char* dst, char* src0, char *src1, unsigned bytes, pami_op opcode, pami_dt dt)
{

  if (dt == PAMI_DOUBLE)
  {
   return  quad_double_math_2way((double*) dst, (double*) src0, (double*)src1, bytes/sizeof(double), opcode);
  }
  else if (dt == PAMI_SIGNED_INT)
  {
    return int_math_2way((int*) dst, (int*) src0, (int*)src1, bytes/sizeof(int), opcode);
  }
  else if (dt == PAMI_UNSIGNED_INT)
  {
    return uint_math_2way((unsigned*) dst, (unsigned*) src0, (unsigned*)src1, bytes/sizeof(unsigned), opcode);
  }
  else // un-optimized dt/op math
  {
    char* srcs[2] = {src0,src1}; 
    coremath func = MATH_OP_FUNCS(dt, opcode, 2);
    PAMI_assertf(func!=NULL,"Unsupported dt %X, op %X\n", dt, opcode);
    unsigned count = convertToCount(bytes,dt);
    func(dst, (void**)srcs,  2, count);
    return count;
  }
}

inline unsigned convertToCount(unsigned bytes,pami_dt dt)
{
  unsigned count;
  switch (dt)
  {
   case PAMI_BYTE:
   case PAMI_SIGNED_CHAR:
     count = bytes/sizeof(char);
     break;
   case PAMI_UNSIGNED_CHAR:
     count = bytes/sizeof(unsigned char);
     break;
   case PAMI_SIGNED_SHORT:
     count = bytes/sizeof(short);
     break;
   case PAMI_UNSIGNED_SHORT:
     count = bytes/sizeof(unsigned short);
     break;
   case PAMI_SIGNED_INT:
     count = bytes/sizeof(int);
     break;
   case PAMI_UNSIGNED_INT:
     count = bytes/sizeof(unsigned int);
     break;
   case PAMI_SIGNED_LONG:
     count = bytes/sizeof(signed long);
     break;
   case PAMI_UNSIGNED_LONG:
     count = bytes/sizeof(unsigned long);
     break;
   case PAMI_SIGNED_LONG_LONG:
     count = bytes/sizeof(long long);
     break;
   case PAMI_UNSIGNED_LONG_LONG:
     count = bytes/sizeof(unsigned long long);
     break;
   case PAMI_FLOAT:
     count = bytes/sizeof(float);
     break;
   case PAMI_DOUBLE:
     count = bytes/sizeof(double);
     break;
   case PAMI_LONG_DOUBLE:
     count = bytes/sizeof(long double);
     break;
   case PAMI_LOGICAL1:
     count = bytes;
     break;
   case PAMI_LOGICAL2:
     count = bytes>>1;
     break;
   case PAMI_LOGICAL4:
     count = bytes>>2;
     break;
   case PAMI_LOGICAL8:
     count = bytes>>3;
     break;
   case PAMI_SINGLE_COMPLEX:
     count = bytes/sizeof(fp32_fp32_t);
     break;
   case PAMI_DOUBLE_COMPLEX:
     count = bytes/sizeof(fp64_fp64_t);
     break;
   case PAMI_LOC_2INT:
     count = bytes/sizeof(int32_int32_t);
     break;
   case PAMI_LOC_SHORT_INT:
     count = bytes/sizeof(int16_int32_t);
     break;
   case PAMI_LOC_FLOAT_INT:
     count = bytes/sizeof(fp32_int32_t);
     break;
   case PAMI_LOC_DOUBLE_INT:
     count = bytes/sizeof(fp64_int32_t);
     break;
   case PAMI_LOC_2FLOAT:
     count = bytes/sizeof(fp32_fp32_t);
     break;
   case PAMI_LOC_2DOUBLE:
     count = bytes/sizeof(fp64_fp64_t);
     break;
   default:
     PAMI_abortf("Unsupported dt %X\n", dt);
  }
  return count;
}

#endif
