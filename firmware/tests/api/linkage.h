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

#ifndef _KERNEL_LINKAGE_H_ // Prevent multiple inclusion.
#define _KERNEL_LINKAGE_H_

//
// Define any special linkage and segmentation for the kernel.
//
// Coordinate segmentation with the linker script (.lds) files.
//

#include <hwi/include/common/compiler_support.h>

#if defined(__KERNEL__)

#if defined(__ASSEMBLY__)

#define _PROLOG_ABS0(fcn) \
         .section ".ktext.abs0","ax" ; \
         .type fcn,@function ; \
         .global fcn;

#define _PROLOG_START(fcn) \
         .section ".ktext.start","ax" ; \
         .type fcn,@function ; \
         .global fcn;

#define _PROLOG(fcn) \
         .section ".text","ax" ; \
         .type fcn,@function ; \
         .global fcn;


#define _EPILOG(fcn) \
         .size fcn,.-fcn ; \
         .previous

#define _DEPILOG(dat) \
         .size dat,.-dat ;

#else // __ASSEMBLY__

#define KTEXT_ABS0    __attribute__((__section__(".ktext.abs0")))
#define KTEXT_START   __attribute__((__section__(".ktext.start")))
#define KTEXT_VECTORS __attribute__((__section__(".ktext.vectors")))
#define FWEXT_DATA  __attribute__((__section__(".data")))

#endif // __ASSEMBLY__

#endif // __KERNEL__

#endif // Add nothing below this line.

