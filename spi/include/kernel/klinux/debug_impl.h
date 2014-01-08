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

#ifndef	_KERNEL_KLINUX_DEBUG_IMPL_H_ /* Prevent multiple inclusion */
#define	_KERNEL_KLINUX_DEBUG_IMPL_H_

#include <asm/bluegene.h>

__INLINE__
int32_t Kernel_GetDebugReg(enum Kernel_Debug_Register debug_register, uint64_t *value_ptr)
{
   return(ENOSYS);
}

__INLINE__
int32_t Kernel_SetDebugReg(enum Kernel_Debug_Register debug_register, uint64_t value)
{
   return ENOSYS;
}

__INLINE__
int Kernel_InjectRAWRAS(uint32_t message_id, size_t raslength, const uint64_t* rasdata)
{
    return bluegene_writeRAS(message_id, 1, (unsigned short)raslength, (void*)rasdata);
}

__INLINE__
int Kernel_InjectASCIIRAS(uint32_t message_id, const uint8_t* text)
{
    return bluegene_writeRAS(message_id, 0, strlen((const char*)text), (void*)text);
}

#endif /* _KERNEL_KLINUX_DEBUG_IMPL_H_ */
