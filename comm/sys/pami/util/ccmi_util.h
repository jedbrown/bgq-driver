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
 * \file util/ccmi_util.h
 * \brief ???
 */

#ifndef __util_ccmi_util_h__
#define __util_ccmi_util_h__

//#define throw() // Needed for xlC -noeh processing
//#include <config.h>

#include <new>
#include <stdio.h>
#include <assert.h>
#include <string.h>


#define MEMCPY           memcpy
#define MEMSET           memset

#if ASSERT_LEVEL==0
  #define CCMI_abort()         assert(0)
  #define CCMI_assert(x)
  #define CCMI_assert_debug(x)
#elif ASSERT_LEVEL==1
  #define CCMI_abort()         assert(0)
  #define CCMI_assert(x)       assert(x)
  #define CCMI_assert_debug(x)
#else /* ASSERT_LEVEL==2 */
  #define CCMI_abort()         assert(0)
  #define CCMI_assert(x)       assert(x)
  #define CCMI_assert_debug(x) assert(x)
#endif

#define CCMIQuad_sizeof(x)  ((sizeof(x)+15)>>4)

#if defined(__bgl__) || defined(__bgx__) || defined(__bgxl__)
  #define MAX_NUM_CORES	2
#elif defined(__bgp__)
  #define MAX_NUM_CORES	4
// ...and so on, for BGQ, ...
#endif /* __bgl__ || __bgx__ ... */

// \brief do some debug code
#undef DO_DEBUG
#define DO_DEBUG(x) //x

/// \brief Trace the template name: eg.
/// DO_DEBUG((templateName<T_Mcast>()));
///   ...results in something like this traced:
/// templateName() [with T_Model = PAMI::Device::LocalBcastWQModel]
template <class T>
inline void templateName()
{
  fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
}



// Jenkin's one at a time hash
static inline void init_hash(uint32_t *hash)
{
  (*hash) = 0;
}
static inline void update_hash(uint32_t *hash,
                               char     *key,
                               uint32_t len)
{
  uint32_t   i;
  for (i=0; i<len; ++i)
    {
      (*hash) += key[i];
      (*hash) += ((*hash) << 10);
      (*hash) ^= ((*hash) >> 6);
    }
}
static inline void finalize_hash(uint32_t *hash)
{
  uint32_t mask = 0x1FFFFF;
  (*hash) += ((*hash) << 3);
  (*hash) ^= ((*hash) >> 11);
  (*hash) += ((*hash) << 15);
  // 27 bit value good for CAU group id
  (*hash) = ((*hash) & mask);        
}



#endif
