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
 * \file algorithms/protocols/tspcoll/Allreduce.cc
 * \brief ???
 */
#include "algorithms/protocols/tspcoll/Allreduce.h"

/* ************************************************************************* */
/*                allreduce callbacks with inline operators                  */
/* ************************************************************************* */
#if 0    // Use Builtin Math
#define CB_ALLREDUCE(typen,opn) cb_allreduce_##opn##_##typen
#define DEFN_CB_ALLREDUCE_INLINE(typename,type,opname,op) \
static void \
CB_ALLREDUCE (typename,opname)(void * dst, const void * src, unsigned nelems, const user_func_t&) \
{ \
  type * dbuf   = (type *) dst; \
  type * pbuf   = (type *) src; \
  for (unsigned i=0; i<nelems; i++) (dbuf[i]) op (pbuf[i]); \
}

/* ------------ */
/* ADD operator */
/* ------------ */

DEFN_CB_ALLREDUCE_INLINE (byte, unsigned char,      add, +=)
DEFN_CB_ALLREDUCE_INLINE (chr,  char,               add, +=)
DEFN_CB_ALLREDUCE_INLINE (srt,  short,              add, +=)
DEFN_CB_ALLREDUCE_INLINE (hwrd, unsigned short,     add, +=)
DEFN_CB_ALLREDUCE_INLINE (int,  int,                add, +=)
DEFN_CB_ALLREDUCE_INLINE (word, unsigned,           add, +=)
DEFN_CB_ALLREDUCE_INLINE (llg,  long long,          add, +=)
DEFN_CB_ALLREDUCE_INLINE (dwrd, unsigned long long, add, +=)
DEFN_CB_ALLREDUCE_INLINE (dbl,  double,             add, +=)
DEFN_CB_ALLREDUCE_INLINE (ldbl, long double,        add, +=)
DEFN_CB_ALLREDUCE_INLINE (flt,  float,              add, +=)

/* ------------- */
/* MULT operator */
/* ------------- */

DEFN_CB_ALLREDUCE_INLINE (byte, unsigned char,      mul, *=)
DEFN_CB_ALLREDUCE_INLINE (chr,  char,               mul, *=)
DEFN_CB_ALLREDUCE_INLINE (srt,  short,              mul, *=)
DEFN_CB_ALLREDUCE_INLINE (hwrd, unsigned short,     mul, *=)
DEFN_CB_ALLREDUCE_INLINE (int,  int,                mul, *=)
DEFN_CB_ALLREDUCE_INLINE (word, unsigned,           mul, *=)
DEFN_CB_ALLREDUCE_INLINE (llg,  long long,          mul, *=)
DEFN_CB_ALLREDUCE_INLINE (dwrd, unsigned long long, mul, *=)
DEFN_CB_ALLREDUCE_INLINE (dbl,  double,             mul, *=)
DEFN_CB_ALLREDUCE_INLINE (ldbl, long double,        mul, *=)
DEFN_CB_ALLREDUCE_INLINE (flt,  float,              mul, *=)

/* ------------- */
/* DIV operator  */
/* ------------- */

DEFN_CB_ALLREDUCE_INLINE (byte, unsigned char,      div, /=)
DEFN_CB_ALLREDUCE_INLINE (chr,  char,               div, /=)
DEFN_CB_ALLREDUCE_INLINE (srt,  short,              div, /=)
DEFN_CB_ALLREDUCE_INLINE (hwrd, unsigned short,     div, /=)
DEFN_CB_ALLREDUCE_INLINE (int,  int,                div, /=)
DEFN_CB_ALLREDUCE_INLINE (word, unsigned,           div, /=)
DEFN_CB_ALLREDUCE_INLINE (llg,  long long,          div, /=)
DEFN_CB_ALLREDUCE_INLINE (dwrd, unsigned long long, div, /=)
DEFN_CB_ALLREDUCE_INLINE (dbl,  double,             div, /=)
DEFN_CB_ALLREDUCE_INLINE (ldbl,  long double,       div, /=)
DEFN_CB_ALLREDUCE_INLINE (flt,  float,              div, /=)

/* ---------------------- */
/* (bitwise) AND operator */
/* ---------------------- */

DEFN_CB_ALLREDUCE_INLINE (byte, unsigned char,      and, &=)
DEFN_CB_ALLREDUCE_INLINE (chr,  char,               and, &=)
DEFN_CB_ALLREDUCE_INLINE (srt,  short,              and, &=)
DEFN_CB_ALLREDUCE_INLINE (hwrd, unsigned short,     and, &=)
DEFN_CB_ALLREDUCE_INLINE (int,  int,                and, &=)
DEFN_CB_ALLREDUCE_INLINE (word, unsigned,           and, &=)
DEFN_CB_ALLREDUCE_INLINE (llg,  long long,          and, &=)
DEFN_CB_ALLREDUCE_INLINE (dwrd, unsigned long long, and, &=)

/* ---------------------- */
/* (bitwise) OR operator  */
/* ---------------------- */

DEFN_CB_ALLREDUCE_INLINE (byte, unsigned char,      or,  |=)
DEFN_CB_ALLREDUCE_INLINE (chr,  char,               or,  |=)
DEFN_CB_ALLREDUCE_INLINE (srt,  short,              or,  |=)
DEFN_CB_ALLREDUCE_INLINE (hwrd, unsigned short,     or,  |=)
DEFN_CB_ALLREDUCE_INLINE (int,  int,                or,  |=)
DEFN_CB_ALLREDUCE_INLINE (word, unsigned,           or,  |=)
DEFN_CB_ALLREDUCE_INLINE (llg,  long long,          or,  |=)
DEFN_CB_ALLREDUCE_INLINE (dwrd, unsigned long long, or,  |=)

/* ------------- */
/* XOR operator  */
/* ------------- */

DEFN_CB_ALLREDUCE_INLINE (byte, unsigned char,      xor, ^=)
DEFN_CB_ALLREDUCE_INLINE (chr,  char,               xor, ^=)
DEFN_CB_ALLREDUCE_INLINE (srt,  short,              xor, ^=)
DEFN_CB_ALLREDUCE_INLINE (hwrd, unsigned short,     xor, ^=)
DEFN_CB_ALLREDUCE_INLINE (int,  int,                xor, ^=)
DEFN_CB_ALLREDUCE_INLINE (word, unsigned,           xor, ^=)
DEFN_CB_ALLREDUCE_INLINE (llg,  long long,          xor, ^=)
DEFN_CB_ALLREDUCE_INLINE (dwrd, unsigned long long, xor, ^=)

/* ------------------- */
/* MIN, MAX operators  */
/* ------------------- */

#define DEFN_CB_ALLREDUCE_MINMAX(typename,type,opname,op)		\
static void \
CB_ALLREDUCE(typename,opname) (void * dst, const void * src, unsigned nelems, const user_func_t& uf) \
{ \
  type * dbuf   = (type *) dst; \
  type * pbuf   = (type *) src; \
  for (unsigned i=0; i<nelems; i++) \
    if (dbuf[i] op pbuf[i]) dbuf[i] = pbuf[i]; \
}

DEFN_CB_ALLREDUCE_MINMAX (byte, unsigned char,      max, <)
DEFN_CB_ALLREDUCE_MINMAX (chr,  char,               max, <)
DEFN_CB_ALLREDUCE_MINMAX (srt,  short,              max, <)
DEFN_CB_ALLREDUCE_MINMAX (hwrd, unsigned short,     max, <)
DEFN_CB_ALLREDUCE_MINMAX (int,  int,                max, <)
DEFN_CB_ALLREDUCE_MINMAX (word, unsigned,           max, <)
DEFN_CB_ALLREDUCE_MINMAX (llg,  long long,          max, <)
DEFN_CB_ALLREDUCE_MINMAX (dwrd, unsigned long long, max, <)
DEFN_CB_ALLREDUCE_MINMAX (dbl,  double,             max, <)
DEFN_CB_ALLREDUCE_MINMAX (ldbl, long double,        max, <)
DEFN_CB_ALLREDUCE_MINMAX (flt,  float,              max, <)

DEFN_CB_ALLREDUCE_MINMAX (byte, unsigned char,      min, >)
DEFN_CB_ALLREDUCE_MINMAX (chr,  char,               min, >)
DEFN_CB_ALLREDUCE_MINMAX (srt,  short,              min, >)
DEFN_CB_ALLREDUCE_MINMAX (hwrd, unsigned short,     min, >)
DEFN_CB_ALLREDUCE_MINMAX (int,  int,                min, >)
DEFN_CB_ALLREDUCE_MINMAX (word, unsigned,           min, >)
DEFN_CB_ALLREDUCE_MINMAX (llg,  long long,          min, >)
DEFN_CB_ALLREDUCE_MINMAX (dwrd, unsigned long long, min, >)
DEFN_CB_ALLREDUCE_MINMAX (dbl,  double,             min, >)
DEFN_CB_ALLREDUCE_MINMAX (ldbl, long double,        min, >)
DEFN_CB_ALLREDUCE_MINMAX (flt,  float,              min, >)

static void CB_ALLREDUCE(dblint,max) (void * dst, const void * src, unsigned nelems, const user_func_t&) \
{ \
  typedef struct { double val; int loc; }  dblint_t;
  dblint_t * dbuf   = (dblint_t *)dst;
  dblint_t * pbuf   = (dblint_t *)src;
  for (unsigned i=0; i<nelems; i++)
    if (dbuf[i].val < pbuf[i].val) { dbuf[i].val = pbuf[i].val; dbuf[i].loc = pbuf[i].loc; }
    else if (dbuf[i].val == pbuf[i].val && dbuf[i].loc < pbuf[i].loc) dbuf[i].loc = pbuf[i].loc;
}

/**
 * user function
 */
#define DEFN_CB_ALLREDUCE_UF(typename,type,opname)		\
static void \
 CB_ALLREDUCE(typename,opname) (void * dst, const void * src, unsigned nelems, const user_func_t& uf) \
{ \
  type * dbuf   = (type *) dst; \
  type * pbuf   = (type *) src; \
  for (unsigned i=0; i<nelems; i++) \
    dbuf[i] = uf.uf_##typename(dbuf[i], pbuf[i]);	\
}

DEFN_CB_ALLREDUCE_UF (byte, unsigned char,      uf)
DEFN_CB_ALLREDUCE_UF (chr,  char,               uf)
DEFN_CB_ALLREDUCE_UF (srt,  short,              uf)
DEFN_CB_ALLREDUCE_UF (hwrd, unsigned short,     uf)
DEFN_CB_ALLREDUCE_UF (int,  int,                uf)
DEFN_CB_ALLREDUCE_UF (word, unsigned,           uf)
DEFN_CB_ALLREDUCE_UF (llg,  long long,          uf)
DEFN_CB_ALLREDUCE_UF (dwrd, unsigned long long, uf)
DEFN_CB_ALLREDUCE_UF (dbl,  double,             uf)
DEFN_CB_ALLREDUCE_UF (ldbl, long double,        uf)
DEFN_CB_ALLREDUCE_UF (flt,  float,              uf)



/* allreduce callback discriminator  */
xlpgas::Allreduce::cb_Allreduce_t
xlpgas::Allreduce::getcallback (pami_data_function op, TypeCode* dt)
{
  uintptr_t      optype, dtype;
  PAMI::Type::TypeFunc::GetEnums(dt, op, dtype, optype);
  switch (optype)
    {
    case XLPGAS_OP_ADD:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  add);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   add);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   add);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  add);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   add);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  add);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   add);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  add);
	  case XLPGAS_DT_dbl:    return CB_ALLREDUCE(dbl,   add);
	  case XLPGAS_DT_ldbl:   return CB_ALLREDUCE(ldbl,   add);
	  case XLPGAS_DT_flt:    return CB_ALLREDUCE(flt,   add);
	  default:               break;
	  }
      }

    case XLPGAS_OP_MUL:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  mul);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   mul);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   mul);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  mul);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   mul);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  mul);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   mul);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  mul);
	  case XLPGAS_DT_dbl:    return CB_ALLREDUCE(dbl,   mul);
	  case XLPGAS_DT_ldbl:   return CB_ALLREDUCE(ldbl,  mul);
	  case XLPGAS_DT_flt:    return CB_ALLREDUCE(flt,   mul);
          default:               break;
	  }
      }

    case XLPGAS_OP_DIV:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  div);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   div);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   div);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  div);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   div);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  div);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   div);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  div);
	  case XLPGAS_DT_dbl:    return CB_ALLREDUCE(dbl,   div);
	  case XLPGAS_DT_ldbl:   return CB_ALLREDUCE(ldbl,   div);
	  case XLPGAS_DT_flt:    return CB_ALLREDUCE(flt,   div);
          default:               break;
	  }
      }

    case XLPGAS_OP_AND:
    case XLPGAS_OP_LOGAND:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  and);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   and);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   and);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  and);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   and);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  and);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   and);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  and);
          default:               break;
	  }
      }

    case XLPGAS_OP_OR:
    case XLPGAS_OP_LOGOR:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  or);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   or);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   or);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  or);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   or);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  or);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   or);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  or);
          default:               break;
	  }
      }

    case XLPGAS_OP_XOR:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  xor);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   xor);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   xor);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  xor);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   xor);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  xor);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   xor);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  xor);
          default:               break;
	  }
      }

    case XLPGAS_OP_MAX:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  max);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   max);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   max);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  max);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   max);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  max);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   max);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  max);
	  case XLPGAS_DT_dbl:    return CB_ALLREDUCE(dbl,   max);
	  case XLPGAS_DT_ldbl:   return CB_ALLREDUCE(ldbl,   max);
	  case XLPGAS_DT_flt:    return CB_ALLREDUCE(flt,   max);
	  case XLPGAS_DT_dblint: return CB_ALLREDUCE(dblint,max);
          default:               break;
	  }
      }

    case XLPGAS_OP_MIN:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  min);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   min);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   min);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  min);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   min);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  min);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   min);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  min);
	  case XLPGAS_DT_dbl:    return CB_ALLREDUCE(dbl,   min);
	  case XLPGAS_DT_ldbl:   return CB_ALLREDUCE(ldbl,  min);
	  case XLPGAS_DT_flt:    return CB_ALLREDUCE(flt,   min);
#if 0
	  case XLPGAS_DT_dblint: return CB_ALLREDUCE(dblint,min);
#endif
          default:               break;
	  }
      }

    case XLPGAS_OP_UFUNC:
      {
	switch (dtype)
	  {
	  case XLPGAS_DT_byte:   return CB_ALLREDUCE(byte,  uf);
	  case XLPGAS_DT_chr:    return CB_ALLREDUCE(chr,   uf);
	  case XLPGAS_DT_srt:    return CB_ALLREDUCE(srt,   uf);
	  case XLPGAS_DT_hwrd:   return CB_ALLREDUCE(hwrd,  uf);
	  case XLPGAS_DT_int:    return CB_ALLREDUCE(int,   uf);
	  case XLPGAS_DT_word:   return CB_ALLREDUCE(word,  uf);
	  case XLPGAS_DT_llg:    return CB_ALLREDUCE(llg,   uf);
	  case XLPGAS_DT_dwrd:   return CB_ALLREDUCE(dwrd,  uf);
	  case XLPGAS_DT_dbl:    return CB_ALLREDUCE(dbl,   uf);
	  case XLPGAS_DT_ldbl:   return CB_ALLREDUCE(ldbl,  uf);
	  case XLPGAS_DT_flt:    return CB_ALLREDUCE(flt,   uf);
          default:               break;
	  }
      }


    default: break;
    } /* switch (op) */
  xlpgas_fatalerror(-1, "Invalid op/type [%d/%d]", optype, dtype);
  return NULL;
}

size_t xlpgas::Allreduce::datawidthof (xlpgas_dtypes_t dtype)
{
  switch(dtype)
    {
    case XLPGAS_DT_byte:   return sizeof(unsigned char);
    case XLPGAS_DT_chr:    return sizeof(char);
    case XLPGAS_DT_srt:    return sizeof(short);
    case XLPGAS_DT_hwrd:   return sizeof(unsigned short);
    case XLPGAS_DT_int:    return sizeof(int);
    case XLPGAS_DT_word:   return sizeof(unsigned);
    case XLPGAS_DT_llg:    return sizeof(long long);
    case XLPGAS_DT_dwrd:   return sizeof(unsigned long long);
    case XLPGAS_DT_dbl:    return sizeof(double);
    case XLPGAS_DT_ldbl:   return sizeof(long double);
    case XLPGAS_DT_flt:    return sizeof(float);
    case XLPGAS_DT_dblint: return sizeof(double) + sizeof(int);
    }
  return 0;
}
#else // Use PAMI Math


inline xlpgas::Allreduce::cb_Allreduce_t
xlpgas::Allreduce::getcallback (pami_data_function optype, TypeCode* dtype)
{
  cb_Allreduce_t cb_allreduce;
  unsigned       datawidth;
  uintptr_t      op, dt;
  PAMI::Type::TypeFunc::GetEnums(dtype, optype, dt, op);
  CCMI::Adaptor::Allreduce::getReduceFunction((pami_dt)dt, (pami_op)op, datawidth, cb_allreduce);
  return cb_allreduce;
}


extern int pami_dt_sizes[];
inline size_t xlpgas::Allreduce::datawidthof (xlpgas_dtypes_t dtype)
{
  return pami_dt_sizes[dtype];
}





#endif
