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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

#ifndef	_KERNEL_THREAD_H_ /* Prevent multiple inclusion */
#define	_KERNEL_THREAD_H_

/*!
 * \file thread.h
 *
 * \brief C Header File containing SPI Addressing Inline Functions
 *
 */


#include <stdint.h>
#include <sys/types.h>

#include "hwi/include/common/compiler_support.h"
#include "firmware/include/personality.h"

typedef void (*Kernel_CommThreadHandler)(uint64_t, uint64_t, uint64_t, uint64_t);
#define KERNEL_COMM_INT0      0  // Comm Intrpt Group 0 
#define KERNEL_COMM_INT1      1  // Comm Intrpt Group 1 
#define KERNEL_COMM_INT2      2  // Comm Intrpt Group 2  
#define KERNEL_COMM_INT3      3  // Comm Intrpt Group 3 
#define KERNEL_COMM_INT_GEA   4  // Comm Intrpts from Global Event Aggregator
#define KERNEL_COMM_NUM_INTS  5  // Number of messaging interrupt types supported



__BEGIN_DECLS

/*!
 * \brief __INLINE__ definition
 * 
 * Option 1:
 * Make all functions be "static inline":
 * - They are inlined if the compiler can do it
 * - If the compiler does not inline it, a single copy of the function is
 *   placed in the translation unit (eg. xxx.c)for use within that unit.
 *   The function is not externalized for use by another unit...we want this
 *   so we don't end up with multiple units exporting the same function,
 *   which would result in linker errors.
 *
 * Option 2:
 * A GNU C model: Use "extern inline" in a common header (this one) and provide
 * a definition in a .c file somewhere, perhaps using macros to ensure that the
 * same code is used in each case. For instance, in the header file:
 *
 * \verbatim
   #ifndef INLINE
   # define INLINE extern inline
   #endif
   INLINE int max(int a, int b) {
     return a > b ? a : b;
   }
   \endverbatim
 *
 * ...and in exactly one source file (in runtime/SPI), that is included in a
 * library...
 *
 * \verbatim
   #define INLINE
   #include "header.h"
   \endverbatim
 * 
 * This allows inlining, where possible, but when not possible, only one 
 * instance of the function is in storage (in the library).
 */
#ifndef __INLINE__
#define __INLINE__ extern inline
#endif

/*! \brief Returns the number of software threads that currently exist on this hardware thread.
 *  
 *  \return Number of software threads existing on this hardware thread (0-15)
 */

__INLINE__ uint32_t Kernel_SnoopNumThreads( void );

/*! \brief Returns the number of runnable software threads on this hardware thread.
 *  
 *  \return Number of runnable software threads existing on this hardware thread (0-15)
 */

__INLINE__ uint32_t Kernel_SnoopRunnable( void );


/*!
  \brief Sets kernel data structures needed to dispatch a communications thread
 *       when a messaging unit interrupt occurs
 * 
 * \param[in] MU Interrupt ID  Identifies a unique messaging unit interrupt line.  
 * \param[in] Function pointer to the function to be called when the interrupt occurs.  
 * \param[in] Param 1 to be passed to the interrupt handler when called. 
 * \param[in] Param 2 to be passed to the interrupt handler when called.  
 * \param[in] Param 3 to be passed to the interrupt handler when called.  
 * \param[in] Param 4 to be passed to the interrupt handler when called.  
 * 
 * 
 * \return Error indication
 * \retval  0 success
 * \retval  EINVAL if not called on a Communication thread or parameters invalid
 * \retval  ENOSYS if not supported
 *
 */

__INLINE__
int32_t Kernel_CommThreadInstallHandler(int mu_int, Kernel_CommThreadHandler func, uint64_t parm1, uint64_t parm2, uint64_t parm3, uint64_t parm4 );

/*!
  \brief Cause the communcation thread to be removed from the thread scheduling queue
 * 
 * \return Error indication
 * \retval  success does not return
 * \retval  EINVAL if not called while on a Communication thread
 * \retval  ENOSYS if not supported
 *
 */

__INLINE__
int32_t Kernel_CommThreadPoof(void);

/*!
  \brief Disables Messaging Unit interrupts for the installed MU interrupt handlers
 * 
 * 
 * \return Error indication
 * \retval  0 success
 * \retval  EINVAL if not called on a Communication thread
 * \retval  ENOSYS if not supported
 *
 */

__INLINE__
int32_t Kernel_CommThreadDisableInterrupts(void);

/*!
  \brief Enables Messaging Unit interrupts for the installed MU interrupt handlers
 * 
 * 
 * \return Error indication
 * \retval  0 success
 * \retval  EINVAL if not called on a Communication thread
 * \retval  ENOSYS if not supported
 *
 */

__INLINE__
int32_t Kernel_CommThreadEnableInterrupts(void);


/**
 * \brief Include implementations of the above functions.
 */
#include "thread_impl.h"

__END_DECLS

#endif /* _KERNEL_THREAD_H_ */
