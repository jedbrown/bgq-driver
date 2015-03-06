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
 * \file arch/a2qpx/Memory.h
 * \brief Defines a2qpx memory specializations
 */

#ifndef __arch_a2qpx_Memory_h__
#define __arch_a2qpx_Memory_h__

#include <hwi/include/bqc/A2_inlines.h>

#define mem_barrier() { do { asm volatile ("mbar"  : : : "memory"); } while(0); }
//{ do { asm volatile ("msync" : : : "memory"); } while(0); }

#include "arch/MemoryInterface.h"

#undef  mem_barrier

#include <hwi/include/bqc/l2_central_inlines.h>
#include <hwi/include/bqc/MU_Macros.h>

namespace PAMI
{
  namespace Memory
  {
    template <> bool supports <instruction>  () { return true; };
    template <> bool supports <remote_msync> () { return true; };
    template <> bool supports <l1p_flush>    () { return true; };

    template <> void sync <instruction>  ()
    {
      do { asm volatile ("isync" : : : "memory"); } while(0);
    };
    
    template <> void sync <remote_msync> ()
    {
      do { asm volatile ("mbar"  : : : "memory"); } while(0);
    };

    template <> void sync <l1p_flush> ()
    {
      volatile uint64_t *dummy_mu_reg =
        (uint64_t *) (BGQ_MU_STATUS_CONTROL_REGS_START_OFFSET(0, 0) +
                      0x030 - PHYMAP_PRIVILEGEDOFFSET);
      *dummy_mu_reg = 0;
    };
  };
};

#endif // __arch_a2qpx_Memory_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
