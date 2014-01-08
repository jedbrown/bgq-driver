/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

//
// BGQ Wakeup Unit MMIO definitions
//
#ifndef WU_MMIO_H
#define WU_MMIO_H

#include <hwi/include/bqc/PhysicalMap.h>

__BEGIN_DECLS

#define WAKEUP_OFFSET_FROM_L1P 0x1c00

#define WAKEUP_BASE ((unsigned long *)(PHYMAP_MINADDR_L1P + PHYMAP_PRIVILEGEDOFFSET + WAKEUP_OFFSET_FROM_L1P))
#define WAKEUP_BASE_USER ((unsigned long *)(PHYMAP_MINADDR_L1P + WAKEUP_OFFSET_FROM_L1P))

#define WAC_BASE(N)		((       ((N) * 0x40)) / sizeof (unsigned long))
#define WAC_ENABLE(N)		((0x20 + ((N) * 0x40)) / sizeof (unsigned long))
#define SET_GUARD		(0x1e8 / sizeof (unsigned long))
#define CLEAR_GUARD		(0x1f0 / sizeof (unsigned long))
#define SET_THREAD(i)		((0x300 + (i)*0x40) / sizeof (unsigned long))
#define CLEAR_THREAD(i)		((0x320 + (i)*0x40) / sizeof (unsigned long))
#define WAC_TTYPES		(0x2d0 / sizeof (unsigned long))
#define WU_USER_ACCESS		(0x2c8 / sizeof (unsigned long))

__END_DECLS

#endif
