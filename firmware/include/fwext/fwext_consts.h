/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef FWEXT_CONSTS_HEADER
#define FWEXT_CONSTS_HEADER
//
// Data from the memory map not yet available in hwi...
//
// DBS       0x3fdff000000-0x3fdff7fffff: atomic ops for boot eDRAM, 4*8*256KB, non-cached access only, opcode in bits 58-60.
// DBS       0x3fdfffc0000-0x3fdffffffff: boot eDRAM, 256KB
//
//  0x3fe00000000-0x3ffffffffff: privileged mirror of above I/O space
//
// privileged version of bedram base...
//
#define ATOMICOPS_BASE    (0x3fdff000000LL | (1LL<<(63-30)))
#define BEDRAM_BASE       (0x3fdfffc0000LL | (1LL<<(63-30)))

#endif
