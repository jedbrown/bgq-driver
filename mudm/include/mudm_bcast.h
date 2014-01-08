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

//! \file  mudm_bcast.h 
//! \brief Declaration of special programming interfaces for broadcasts and collectives
//! \remarks primarily intended for compute node job launch and controls--broadcasts and barriers


#ifndef	_MUDM_BCAST_H_ /* Prevent multiple inclusion */
#define	_MUDM_BCAST_H_

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/bqc/MU_PacketCommon.h> 

#define MUDM_REDUCE_ALL      0xF50F //Reduce result to all participants                                                            
/**
 * 
 * \brief  Broadcast a memFIFO packet.
 *
 * \details Send a memfifo packet on the system collective class route.
 * 
 *
 * \param[in]  mudm_context is the mudm context pointer from mudm_init()
 * \param [in]  requestID is for the registered status callback indicating the status of the operation completing (may be NULL).
 * \param [in]  type is the protocol type
 * \param [in]  payload_paddr is the physical address of the data (payload) 
 * \param [in]  payload_length is the length of the data.
 * \param [in]  class_route is the selected MU class route for collective/broadcast operation and was established by the caller
 *   
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument.
 * \li \c payload_length > TORUS_MAX_PAYLOAD_SIZE
 * \retval -EINPROGRESS Request is in progress, requestID pending
 * \retval -EBUSY No resources, busy using resources.  Try again later.
 * 
 * 
 **/
int mudm_bcast_memfifo(void* mudm_context,
                          void* requestID, 
                          uint16_t type, 
                          void * payload_paddr,
                          uint16_t payload_length,
                          uint8_t  class_route); 


/**
 * 
 * \brief  Conduct a reduce operation.
 *
 * \details 
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from mudm_init().
 * \param [in]  opcode is the collective operation--use values defined in MU_CollectiveNetworkHeader.h
 * \param [in]  payload_paddr is the physical address of the data (payload) 
 * \param [in]  payload_length is the length of the data.  Length must be 2^n number of words, 4*2^n bytes<=512 (4,8,16,32,64,128,256,512)
 * \param [in]  torus_destination is the 32-bit MU encoding of the reduce final torus destination  (ignored for reduce all operation)
 * \param [in]  class_route is the selected MU class route for collective/broadcast operation and was established by the caller
 * \param [in]  reduce_choice is MUHWI_COLLECTIVE_TYPE_ALLREDUCE or MUHWI_COLLECTIVE_TYPE_REDUCE from MU_CollectiveNetworkHeader.h
 *   
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument.
 * \li \c payload_length > TORUS_MAX_PAYLOAD_SIZE
 * \retval -EINPROGRESS Request is in progress, requestID pending
 * \retval -EBUSY No resources, busy using resources.  Try again later.
 * 
 * 
 **/

int mudm_bcast_reduce(void* mudm_context,
                          
                          uint16_t type,  
                          uint8_t op_code, 
                          void * payload_paddr,
                          uint16_t payload_length,
                          MUHWI_Destination_t torus_destination,
                          uint8_t  class_route,
                          uint8_t  reduce_choice,
                          void* result_vaddr);

/**
 * 
 * \brief  Structure of rdma broadcast values
 *
 * \details 
 * 
 *
 * \param [in]  requestID is for the registered status callback indicating the status of the operation completing.  If NULL, no callback
 * \param [in]  dest_payload_paddr is the target physical address of the data movement 
 * \param [in]  source_payload_paddr is the source physical address of the data (payload) 
 * \param [in]  payload_length is the length of the data.
 * \param [in]  class_route is the selected MU class route for collective/broadcast operation
 * \param [in]  num_in_class_route is the number of compute nodes in the class route  
 * \param [in]  status_mem is the physical=real address of where to write status_val after the RDMA completes
 * \param [in]  status_val is the value to be written to status_mem
 *   
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument.
 * \li \c payload_length > TORUS_MAX_PAYLOAD_SIZE
 * \retval -EINPROGRESS Request is in progress.  
 * \retval -EBUSY No resources, busy using resources.  Try again later. (requestID pending for an RDMA broadcast)
 * 
 * 
 **/
struct mudm_rdma_bcast {  
   void* requestID; // call status callback request ID       
   void * source_payload_paddr;
   void * dest_payload_paddr;
   uint64_t payload_length;
   uint32_t num_in_class_route;
   uint8_t class_route;
   uint8_t reserved[3];
   uint64_t status_mem; 
   uint64_t status_val;
};  

/**
 * 
 * \brief  Broadcast a direct put RDMA operation.
 *
 * \details 
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from mudm_init()
 * \param [in]  mrb points to a structure containing the parameters for the rdma broadcast
 *   
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument.
 * \retval -EINPROGRESS Request is in progress. 
 * \retval -EBUSY No resources, busy using resources.  Try again later. (requestID pending for an RDMA broadcast)
 * 
 * 
 **/

int mudm_bcast_RDMA_write(void* mudm_context, struct mudm_rdma_bcast * mrb);


                          
#endif /* _MUDM_BCAST_H_*/
