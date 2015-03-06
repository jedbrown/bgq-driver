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
 * \file arch/MemoryInterface.h
 * \brief Defines arch memory interface
 */

#ifndef __arch_MemoryInterface_h__
#define __arch_MemoryInterface_h__

#ifndef mem_barrier
#error "mem_barrier() was not defined by the architecture"
#endif

#include "Arch.h"

namespace PAMI
{
  namespace Memory
  {
    typedef enum
    {
      full_sync = 0,

      // The instruction synchronization causes the processor to complete
      // execution of all previous instructions and then to discard instructions
      // (which may have begun execution) following the synchronization. After
      // the instruction synchronization is executed, the following instructions
      // then begin execution.
      //
      // Instruction synchronization is used in locking code to ensure that the
      // loads following entry into the critical section are not performed
      // (because of aggressive out-of-order or speculative execution in the
      // processor) until the lock is granted.
      instruction,

      remote_msync,
      l1p_flush,
    } attribute_t;

    template <unsigned T_Attribute>
    static  bool supports ()
    {
      return false;
    };

    template <unsigned T_Attribute>
    static void sync ()
    {
      mem_barrier();
    };

    static void sync ()
    {
      sync<0> ();
    };

    template <> bool supports <full_sync> () { return true; };

  };
};

#endif // __arch_MemoryInterface_h__


//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
