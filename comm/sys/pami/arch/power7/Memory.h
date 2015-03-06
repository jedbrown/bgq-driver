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
 * \file arch/power7/Memory.h
 * \brief Defines power7 memory specializations
 */

#ifndef __arch_power7_Memory_h__
#define __arch_power7_Memory_h__

#define mem_barrier() { asm volatile ("lwsync"); }

#include "arch/MemoryInterface.h"

#undef  mem_barrier

namespace PAMI
{
  namespace Memory
  {
    template <> const bool supports <instruction> () { return true; };

    template <> void sync <instruction> () { asm volatile ("isync"); };
  };
};

#endif // __arch_power7_Memory_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
