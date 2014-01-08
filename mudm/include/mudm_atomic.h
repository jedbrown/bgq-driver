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

//! \file  mudm_atomic.h 
//! \brief Declaration of special programming interfaces for broadcasts and collectives
//! \remarks primarily intended for compute node job launch and controls--broadcasts and barriers


#ifndef	_MUDM_ATOMIC_H_ /* Prevent multiple inclusion */
#define	_MUDM_ATOMIC_H_

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/bqc/MU_PacketCommon.h> 

struct remoteget_atomic_controls {
    uint64_t scratch_area[8]; //used for building the directput descriptor for the remote MU 
    volatile uint64_t local_counter;  //after the remoteget is issued, can poll for this going to 0 for completion
    uint64_t returned_val;   //if a value is fetched on the execution of the atomic on the remote MU, it goes here
    uint64_t remote_paddr;   // remote_paddr is the remote atomic physical address location (8B boundary required)
    uint64_t paddr_here;     //starting physical address of this struct
    uint64_t atomic_op;      //atomic operation values from MU_Addressing.h in hwi/include/bqc/
    MUHWI_Destination_t torus_destination; //remote torus location
    uint32_t reserved;
    uint64_t test1_reserved;
    uint64_t test2_reserved;
} __attribute__ ((aligned (64)));

//Note:  If the remote counter is polled, it must go to zero!
struct directput_atomic_controls {
    uint64_t scratch_area[8]; //used for building the directput descriptor for the local MU 
    uint64_t source_val;      // local source value for atomic operation (located here in this struct)
    uint64_t remote_counter_physAddr;  //Physical Memory Address of the remote counter (8B boundary required)
    uint64_t remote_paddr;    // remote physical address on remote (8B boundary required)
    uint64_t paddr_here;      //starting physical address of this struct
    uint64_t atomic_op;       //atomic operation values from MU_Addressing.h in hwi/include/bqc/ 
    MUHWI_Destination_t torus_destination; //remote torus location
    uint32_t reserved;
} __attribute__ ((aligned (64)));

/**
 * 
 * \brief  Do a remoteget of a load atomic operation operation.
 *
 * \details 
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from mudm_init()
 * \param [in]  atc is the pointer to the atomic controls for the operation
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument.
 * 
 * 
 **/


int mudm_remoteget_load_atomic(void* mudm_context,  struct  remoteget_atomic_controls * atc);


/**
 * 
 * \brief  Do a directput of a store atomic operation operation.
 *
 * \details 
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from mudm_init()
 * \param [in]  atc is the pointer to the atomic controls for the operation
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument.
 * 
 **/

int mudm_directput_store_atomic(void* mudm_context,                           
                          struct  directput_atomic_controls * atc);
                          
#endif /* _MUDM_ATOMIC_H_*/
