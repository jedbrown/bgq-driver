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
 * \file util/BitVector.h
 * \brief ???
 */
#ifndef __util_BitVector__
#define __util_BitVector__

#include <climits>
#include <cstring>
#include "Global.h"

namespace PAMI
{
  class BitVector
  {
#ifdef __64BIT__
    typedef unsigned long long bit_word_t;
#else
    typedef unsigned int bit_word_t;
#endif

    enum { BITWORD_SIZE = (unsigned)sizeof(bit_word_t) * CHAR_BIT };

    bit_word_t  *_bits;
    unsigned   _size;
    unsigned   _capacity;

public:
    // Default ctor - creates an empty bit vector
    BitVector():_size(0),_capacity(0)
    {
      _bits = NULL;
    }

    // Creates a bit vector of 'size' bits. all bits are initialized to 'val'
    explicit BitVector(unsigned size, bool val = false) :_size(size)
    {
      _capacity = numBitWords(_size);
      int rc;
      rc = __global.heap_mm->memalign((void **)&_bits, 0,
                                          sizeof(bit_word_t) * _capacity);
      PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc bit vector _bits");
      std::memset(_bits, val ? 1 : 0,  _capacity * sizeof(bit_word_t));
    }

    // Copy ctor
    BitVector(const BitVector &right) :_size(right.size())
    {
      if (_size == 0) {
        _bits = 0;
        _capacity = 0;
        return;
      }

      _capacity = numBitWords(_size);
      int rc;
      rc = __global.heap_mm->memalign((void **)&_bits, 0,
                                          sizeof(bit_word_t) * _capacity);
      PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc bit vector _bits");
      std::memcpy(_bits, right._bits, sizeof(bit_word_t) * _capacity);
    }

    ~BitVector()
    {
      __global.heap_mm->free(_bits);
    }
    // Set the bit at index 'n' to 1
    void set(unsigned n)
    {
      _bits[n / BITWORD_SIZE] |= ((bit_word_t)1 << (n % BITWORD_SIZE));
    }
    // Set the bit at index 'n' to 0
    void clear(unsigned n)
    {
      _bits[n / BITWORD_SIZE] &= ~((bit_word_t)1 << (n % BITWORD_SIZE));
    }
    // Fetch the bit at index 'n'. Returns as an unsigned integer
    unsigned get(unsigned n)
    {
      bit_word_t bit = _bits[n / BITWORD_SIZE] & ((bit_word_t)1 << (n % BITWORD_SIZE));
      return bit != 0;
    }
    // flip the bit at index 'n'
    void flip(unsigned n)
    {
      _bits[n / BITWORD_SIZE] ^= ((bit_word_t)1 << (n % BITWORD_SIZE));
    }
    // Returns the number of bits in this bit vector
    unsigned size() const
    {
       return _size;
    }

private:
    unsigned numBitWords(unsigned size)
    {
      return (size + BITWORD_SIZE - 1) / BITWORD_SIZE;
    }

  }; // BitVector
};   // PAMI
#endif
