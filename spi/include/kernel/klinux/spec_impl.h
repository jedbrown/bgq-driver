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

#ifndef	_KERNEL_KLINUX_SPEC_IMPL_H_ /* Prevent multiple inclusion */
#define	_KERNEL_KLINUX_SPEC_IMPL_H_

/**
 * \file klinux/spec_impl.h
 *
 * \brief C Header File containing speculation SPI
 *        implementations for the Linux kernel.
 *
 */

__INLINE__
uint32_t Kernel_SetSpecState(size_t specStateSize, SpecState_t* specStateBasePtr)
{
   return ENOSYS;
}

__INLINE__
uint32_t Kernel_EnterSpecJailMode(int isLongRunningSpeculation)
{
    return ENOSYS;
}

__INLINE__
uint32_t Kernel_ExitSpecJailMode(void)
{
    return ENOSYS;
}

__INLINE__
uint32_t Kernel_AllocateSpecDomain(unsigned int* domain)
{
    return ENOSYS;
}


__INLINE__
uint32_t Kernel_SetSpecDomainMode(unsigned int domain, uint64_t domainmode)
{
    return ENOSYS;
}

#endif /* _KERNEL_KLINUX_SPEC_IMPL_H_ */
