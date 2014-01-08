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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef	_KERNEL_SENDX_IMPL_H_ /* Prevent multiple inclusion */
#define	_KERNEL_SENDX_IMPL_H_

#include <hwi/include/bqc/A2_inlines.h>
#include "cnk/include/SPI_syscalls.h"
#include "cnk/include/Config.h"
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>



__INLINE__
int Kernel_Sendx(struct MsgInputs * mInputs){

  int rc = (int) CNK_SPI_SYSCALL_1(SENDX,mInputs );
  return rc;
}


#endif /* _KERNEL_SENDX_IMPL_H_ */
