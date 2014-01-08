/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2012, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

#ifndef	_KERNEL_SENDX_H_ /* Prevent multiple inclusion */
#define	_KERNEL_SENDX_H_

/*! \file  sendx.h
 * \brief Declaration of special programming interfaces for sending user custom messages to sysiod dynamic link-loaded library
 * \remarks Need to configure bg.properties to load the accompanying sysiod_plugin library for handling the messages
 * 
*/

#include "kernel_impl.h" 
#include "hwi/include/common/compiler_support.h"
#include <ramdisk/include/services/UserMessages.h>

__BEGIN_DECLS



struct CNKuserRDMA {
  void * cnk_address;                             //!< CNK memory address of a memory region within user CNK process space
  uint32_t cnk_bytes;                             //!< Number of byes to CNK memory region in CNK user process space to apply operations
};

#define MSG_DATA_ONLY    0  //!< Option for the message carries data only
#define MSG_DATA_PLUS    1  //!< Option for the message possible has a file descriptor and/or RDMA regions 

struct MsgInputs {
  uint8_t  version;             //!< User settable version
  uint8_t  options;             //!< Options specified by user (e.g., MSG_DATA_ONLY or MSG_DATA_PLUS)
  uint16_t type;                //!< User settable type
  uint32_t recv_length;         //!< Length of user provided response buffer recvMessage (0 means not supplied)
  int      numberOfRdmaRegions; //!< Number of RDMA Regions
  int      cnkFileDescriptor[2];//!< CNK file descriptors (0 value means ignore)
  int      data_length;         //!< Length of data to send in message to IO node sysiod user link-load library (0 means no data)
  char *   dataRegion;          //!< Address to dataRegion to send in message to IO node sysiod user link-load library (NULL means no data)
  char *   recvMessage;         //!< Address region for receiving response message of length recv_length (NULL means no region supplied) 
  struct CNKuserRDMA * cnkUserRDMA;  //!< Address to list of RDMA regions in CNK user space
};

//! \brief Do a sendx which will send a user message to sysiod on the IO node
//! \param mInputs is a struct with the appopriate values set for a simple message or a more involved message involving file descriptors and/or RDMA
__INLINE__
int Kernel_Sendx(struct MsgInputs * mInputs);


#include "sendx_impl.h"

__END_DECLS

#endif /* _KERNEL_SENDX_H_ */
