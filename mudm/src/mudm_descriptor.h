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


#ifndef	_MUDM_DESCRIPTOR_H_ /* Prevent multiple inclusion */
#define	_MUDM_DESCRIPTOR_H_

void Update_memfifo_payload( MUHWI_Descriptor_t  *desc,
                           uint64_t payload_phys_addr,
                           uint64_t payload_length
                                             );

/*
typedef struct MUSPI_InjFifo
{ 
  MUSPI_Fifo_t     _fifo;       //< Fifo shadow structure 
  uint64_t         descCount;   //< Shadow of the number of  descriptors in this fifo that have been  completed by the MU 
  uint64_t         freeSpace;   //< Shadow of the freespace.  This is in units of number of 64-byte descriptors. 
  uint64_t         numInjected; //< Software counter of the number of
				     descriptors injected.  After the
				     application has injected a descriptor,
				     this counter is updated.  Completion of 
                                     the message can be observed by comparing
				     this software counter with the hardware
				     descCount in SRAM (or the descCount shadow
				     in this structure). 
  MUHWI_InjFifo_t *hw_injfifo;  //< Pointer to additional injfifo in SRAM 
} MUSPI_InjFifo_t;
*/
typedef struct MUDM_InjFifo
{
  MUSPI_InjFifo_t * injfifo; 
  Lock_Atomic_t inj_fifo_lock;
  volatile uint32_t state;         //0=inactive; 1=active
} MUDM_InjFifo_t;

/**
 * \brief Memory FIFO Descriptor Information Fields
 *
 * Used to pass information common to memory FIFO descriptors to
 * functions that build descriptors.
 *
 */
typedef struct MemoryFIFODescriptorInfoFields
{
  uint16_t         Rec_FIFO_Id; /**< Reception FIFO Id to receive the packet
				     (0 to 271). 
                                */

  uint32_t         Interrupt;      /**< Specifies whether an interrupt should
				        occur when the last packet of this
					message arrives in the destination's
					reception FIFO.  Specify one of:
					- MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL
					- MUHWI_DESCRIPTOR_DO_NOT_INTERRUPT_ON_PACKET_ARRIVAL
				   */

  uint8_t          SoftwareBit;/**< A bit available to software that will be
				    passed unchanged to the destination in
				    the packet header.  This corresponds to 
				    "Unused1" in the Memory_FIFO portion of
				    the Message Unit Header of the packet.
				    Refer to PacketInlines.h for a function
				    to extract this field from the packet
				    header.  Set this to 0 or 1.
			       */

  uint8_t          SoftwareBytes[18];/**< Bytes available to software that will
				          be passed unchanged to the destination
					  in the packet header for messages contained
                                          within a single packet
			             */
} MemoryFIFODescriptorInfoFields_t;


/**
 * \brief Point-to-Point Memory FIFO Descriptor Information Structure
 *
 * Used to pass parameters specific to Point-to-Point Memory FIFO descriptors to
 * functions that build memory FIFO descriptors.
 * 
 */
typedef struct Pt2PtMemoryFIFODescriptorInfo
{
  MUSPI_BaseDescriptorInfoFields_t Base; /**< Base (common) descriptor info   */

  MUSPI_Pt2PtDescriptorInfoFields_t Pt2Pt; /**< Point-to-point descriptor info*/

  MemoryFIFODescriptorInfoFields_t MemFIFO; /**< Memory FIFO
						       descriptor info   */

} Pt2PtMemoryFIFODescriptorInfo_t;

void InitPt2PtMemoryFIFODescriptor( MUHWI_Descriptor_t                    *desc,
                                               SoftwareBytes_t                       *SoftwareBytes,
                                               uint64_t payload_phys_addr,
                                               uint64_t payload_length,
                                               MUHWI_Destination_t dest,
                                               uint8_t direction, 
                                               uint8_t  io_port,
                                               uint16_t rec_FIFO_Id
                                             );
void  initPt2PtDirectPutDescriptor( MUHWI_Descriptor_t *desc,
                                               uint64_t payload_phys_addr,
                                               uint64_t payload_length,
                                               MUHWI_Destination_t dest,
                                               uint8_t direction, 
                                               uint8_t  io_port,
                                               uint64_t Rec_Counter_Offset, 
                                               uint64_t Rec_Payload_Offset
                                               );
#endif //_MUDM_DESCRIPTOR_H_
