/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

#ifndef	_KERNEL_DEBUG_H_ /* Prevent multiple inclusion */
#define	_KERNEL_DEBUG_H_

/*!
 * \file debug.h
 *
 * \brief C Header File containing SPI Addressing Inline Functions
 *
 */


/*#include <stdint.h>
 *#include <sys/types.h>
 */
#include "kernel_impl.h"

#include "hwi/include/common/compiler_support.h"
#include "firmware/include/personality.h"


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


int Kernel_InjectRAWRAS(uint32_t message_id, size_t raslength, const uint64_t* rasdata);

__INLINE__
int Kernel_InjectASCIIRAS(uint32_t message_id, const uint8_t* text);

/*!
  \brief Set or get the value of an A2 debug register
 *
 *  THESE INTERFACES ARE EXPERIMENTAL AND UNSUPPORTED.
 *  THEY CANNOT BE USED IN CONJUNCTION WITH ANY DEBUGGER/TOOL.
 *  THEY CANNOT BE USED IN A PROCESS THAT RUNS ON LESS THAN A FULL CORE.
 *
 *  The CNK syscalls underlying Kernel_{Get,Set}DebugReg() are disabled by
 *  default.  Before they can be used, BG_DEBUGREGSYSCALLSENABLED must be set
 *  to a non-zero value in the calling program's initial environment.
 *
 * \param[in] debug_reg - selector for register to set or get
 * \param[in] value_ptr (for GetDebugReg) - location for value to be returned
 * \param[in] value (for SetDebugReg) - value to be set in register
 *
 *
 * \return Error indication
 * \retval  0 success
 * \retval  ENOSYS if not supported or disabled
 * \retval  EPERM if a CDTI-based debugger tool is in use or if the calling
 *                    process is running on less than a full core
 * \retval  EFAULT if value_ptr is invalid
 * \retval  EINVAL if debug_reg is invalid
 *
 */

enum Kernel_Debug_Register {
    Kernel_Debug_DBCR0,
    Kernel_Debug_DBCR1,
    Kernel_Debug_DBCR2,
    Kernel_Debug_DBCR3,
    Kernel_Debug_DAC1,
    Kernel_Debug_DAC2,
    Kernel_Debug_DAC3,
    Kernel_Debug_DAC4,
    Kernel_Debug_IAC1,
    Kernel_Debug_IAC2,
    Kernel_Debug_IAC3,
    Kernel_Debug_IAC4,
    Kernel_Debug_DVC1,
    Kernel_Debug_DVC2,
    Kernel_Debug_MSRDE,
    Kernel_Debug_DBSR
};

__INLINE__
int32_t Kernel_GetDebugReg(enum Kernel_Debug_Register debug_reg, uint64_t *value_ptr);

__INLINE__
int32_t Kernel_SetDebugReg(enum Kernel_Debug_Register debug_reg, uint64_t value);


/**
 * \brief Include implementations of the above functions.
 */
#include "debug_impl.h"

__END_DECLS

#endif /* _KERNEL_DEBUG_H_ */
