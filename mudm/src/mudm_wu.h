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

//! \file  mudm_wu.h 
//! \brief Declaration of MUDM wake_up unit elements
//! \remarks 


#ifndef	_MUDM_WU_H_  /* Prevent multiple inclusion */
#define	_MUDM_WU_H_
#include "rdma_object.h"
#include "common_inlines.h"
#ifdef __LINUX__
__INLINE__ void WU_ArmMU_system(uint64_t bits)
{
MENTER;
MPRINT("NO LINUX SUPPORT\n");
return;
}
__INLINE__ void WU_DisarmMU(uint64_t bits)
{
MENTER;
MPRINT("NO LINUX SUPPORT\n");
return;
}
__INLINE__ void WU_ArmWithAddress(uint64_t physical_address, uint64_t mask)
{
MENTER;
MPRINT("NO LINUX SUPPORT \n");
return;
}

__INLINE__ 
void wakeup_wait(void)
{
MENTER;
MPRINT("NO LINUX SUPPORT \n");
return;
}

__INLINE__ void IPI_DeliverWakeup(int processorID)
{

}
__INLINE__ int  IPI_AcknowledgeWakeup(uint64_t *mask1, uint64_t *mask2)
{
  return 0;
}			

#else 

__INLINE__ 
void wakeup_wait(){ppc_waitimpl();}
#include "spi/include/wu/wait.h"

#include <hwi/include/bqc/BIC_inlines.h>
#include <spi/include/upci/upci_syscall.h>
void IPI_DeliverWakeup(int processorID);
int  IPI_AcknowledgeWakeup(uint64_t *mask1, uint64_t *mask2);
#endif 

__INLINE__
void wakeup(void){
IPI_DeliverWakeup(67);  //CNK uses the last thread 67 for MUDM polling
}

__INLINE__
void wakeup_clear_IPI(void){
  uint64_t mask1,mask2;
  //int rc=
  IPI_AcknowledgeWakeup(&mask1, &mask2);
  /*
  if (rc) {
    MPRINT("mask1=%llx mask2=%llx \n", (LLUS)mask1,(LLUS)mask2);
  }*/
}

#endif  //_MUDM_WU_H_
