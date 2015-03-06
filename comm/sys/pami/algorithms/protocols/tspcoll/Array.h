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
 * \file algorithms/protocols/tspcoll/Array.h
 * \brief ???
 */
#include <assert.h>
#include <string.h>

#ifndef __algorithms_protocols_tspcoll_Array_h__
#define __algorithms_protocols_tspcoll_Array_h__

namespace util
{
  /* ********************************************************************** */
  /*              Expandable Array implementation                           */
  /* ********************************************************************** */

#define MAXOF(a,b) (((a)>(b))?(a):(b))

  template <class T> class Array
    {
    public:
      void * operator new (size_t, void * arg) { return arg; }
      Array () { _table = NULL; _max = _len = 0; }
      T &operator[](int idx)
	{
	  assert (idx>=0);
	  if (idx>=(int)_max)
	    {
	      size_t oldmax = _max;
	      _max = MAXOF((2*_max+1),((size_t)(idx+1)));
	      _table = (T *) realloc (_table, sizeof(T)*_max);
	      if (!_table)
              {
                PAMI_assert(0);
                //xlpgas_fatalerror(-1, "Memory allocation error");
              }
	      memset (_table+oldmax,0,sizeof(T)*(_max-oldmax));
	    }
	  if ((size_t)idx>=_len) _len=idx+1;
	  return _table[idx];
	}
      size_t max () const { return _max; }
      size_t len () const { return _len; }

    private:
      T      * _table;
      size_t   _max;
      size_t   _len;
    };

}

#endif
