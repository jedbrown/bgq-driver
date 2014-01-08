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


#ifndef	_MUDM_COMMON_INLINES_H_ /* Prevent multiple inclusion */
#define	_MUDM_COMMON_INLINES_H_

#include "common.h"
#include "mudm_descriptor.h"
#include <mudm/include/mudm_utils.h>
#include <hwi/include/bqc/MU_MessageUnitHeader.h>
#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>

#include <mudm/include/mudm.h>
#include <mudm/include/mudm_inlines.h>
#include <mudm/include/mudm_utils.h>
#include "mudm_macro.h"
#include "mudm_trace.h"
#include "mudm_ras.h"

/* Need GetTimeBase but rename to handle commonness with Linux & CNK*/
/* Using the definitions from A2_inlines.h and A2_core.h            */
/* in directory hwi/include/bqc                                     */

#define  SPRN_TBRO2                (0x10C)          // Time Base 64-bit                             User Read-only

__INLINE__ uint64_t GetTimeBase2( void )
{
    return mfspr( SPRN_TBRO2 );         // 64-bit timebase
}

__INLINE__  void zero_memregion(struct mudm_memory_region * mmr){
  memset(mmr->base_vaddr,0,mmr->length);
}

__INLINE__ void my_mbar(void)  { do { asm volatile ("mbar"  : : : "memory"); } while(0); }


__INLINE__ uint32_t isIOnode(struct my_context * mcontext){ return mcontext->myTorusAddress.Destination.Reserved2;}

//static uint64_t injectCount = 0;
/**
 * \brief Inject a Descriptor Into the Injection Fifo
 *
 * \param[in] injfifo  Pointer to the injection fifo shadow structure
 * \param[in] desc     Pointer to the descriptor
 *
 * \return Descriptor number of the descriptor that was injected.
 * \retval -1 Injection fifo is full
 */
#define MUDM_GET_NEXT_DESC(counter,controls,pointer) \
    do {\
      counter = MUSPI_InjFifoNextDesc (controls->injfifo,(void **)pointer);\
    }while ( (counter == -1) && (controls->state) );\
    if ( unlikely(0==controls->state) ){\
      unlock(&controls->inj_fifo_lock);\
      return (uint64_t)(-1);\
    };\
  
__INLINE__ 
uint64_t InjFifoInject (MUDM_InjFifo_t   * injfifo_ctls, 
					 void              * desc, 
                                         BG_FlightRecorderRegistry_t *logregistry) 
{   
    MUHWI_Descriptor_t  * descPtr;
    void * descPtrTemp=NULL;
    uint64_t desc_count;

    lock(&injfifo_ctls->inj_fifo_lock);
    MUDM_GET_NEXT_DESC(desc_count,injfifo_ctls,&descPtrTemp);
    descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    //DUMPHEXSTR("inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    LOG_DESCR_HEADER(logregistry,descPtr);
    LOG_INJECT_DESCR(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo_ctls->injfifo);
    unlock(&injfifo_ctls->inj_fifo_lock);
    //$$$
    PRINT("injected %p desc_count=%llu \n",descPtr,(LLUS)desc_count);
    return desc_count;
}


__INLINE__ 
uint64_t InjFifoInjectMemFifo (MUDM_InjFifo_t   * injfifo_ctls,
					 MUHWI_Descriptor_t  *desc,
                                         uint16_t type,
                                         uint64_t payload_phys_addr,
                                         uint64_t payload_length, 
                                         uint64_t * send_count,
                                         BG_FlightRecorderRegistry_t *logregistry) 
                                       
{   
    MUHWI_Descriptor_t  * descPtr;
    void * descPtrTemp=NULL;
    MUHWI_MessageUnitHeader_t *muh;
    struct ionet_header * ionet_hdr;
    uint64_t desc_count;
    lock(&injfifo_ctls->inj_fifo_lock);
    MUDM_GET_NEXT_DESC(desc_count,injfifo_ctls,&descPtrTemp);
    descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    descPtr->Pa_Payload = payload_phys_addr; /* Set payload physical address.  */
    descPtr->Message_Length = payload_length; 
    muh =  &(descPtr->PacketHeader.messageUnitHeader); 
    ionet_hdr = (struct ionet_header * )&muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;
    ionet_hdr->payload_length = payload_length;
    ionet_hdr->type = type;    
    ionet_hdr->sequence_number = *send_count;
    ++(*send_count);
    // $$$     
    //DUMPHEXSTR("memfifo inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    LOG_DESCR_HEADER(logregistry,descPtr);
    INJ_MEMFIFO_CONN(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo_ctls->injfifo);
    unlock(&injfifo_ctls->inj_fifo_lock);
    //$$$
    PRINT("injected %p memfifo desc_count=%llu \n",descPtr,(LLUS)desc_count);
    //MDUMPHEXSTR("POST INJECT:  memfifo inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    return desc_count;
}

__INLINE__ 
uint64_t InjFifoInjectMemFifo_withDest (MUDM_InjFifo_t   * injfifo_ctls,
					 MUHWI_Descriptor_t  *desc,
                                         MUHWI_Destination_t  dest,       /* remote destination    */
                                         uint16_t type,
                                         uint64_t payload_phys_addr,
                                         uint64_t payload_length, 
                                         uint64_t * send_count,
                                         BG_FlightRecorderRegistry_t *logregistry) 
                                       
{   
    MUHWI_Descriptor_t  * descPtr;
    void * descPtrTemp=NULL;
    MUHWI_MessageUnitHeader_t *muh;
    struct ionet_header * ionet_hdr;
    uint64_t desc_count;
    lock(&injfifo_ctls->inj_fifo_lock);
    MUDM_GET_NEXT_DESC(desc_count,injfifo_ctls,&descPtrTemp);
    descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    descPtr->Pa_Payload = payload_phys_addr; /* Set payload physical address.  */
    descPtr->Message_Length = payload_length; 
    muh =  &(descPtr->PacketHeader.messageUnitHeader); 
    ionet_hdr = (struct ionet_header * )&muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;
    ionet_hdr->payload_length = payload_length;
    ionet_hdr->type = type;    
    ionet_hdr->sequence_number = *send_count;
    ++(*send_count);
    descPtr->PacketHeader.NetworkHeader.pt2pt.Destination = dest; /* Set the destination. */
    // $$$     
    //DUMPHEXSTR("memfifo inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    LOG_DESCR_HEADER(logregistry,descPtr);
    INJ_MEMFIFO_CONN(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo_ctls->injfifo);
    unlock(&injfifo_ctls->inj_fifo_lock);
    //$$$
    PRINT("injected %p memfifo desc_count=%llu \n",descPtr,(LLUS)desc_count);
    //MDUMPHEXSTR("POST INJECT:  memfifo inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    return desc_count;
}
////////////////////////////////////////////
__INLINE__   uint64_t conn_memfifo(struct mudm_connection * ccontext,
                                               uint16_t type,
                                               uint64_t payload_phys_addr,
                                               uint64_t payload_length
               ){               
  return InjFifoInjectMemFifo (ccontext->injfifo_ctls, 
                                             &ccontext->mu_iMemoryFifoDescriptor,
                                             type,payload_phys_addr, payload_length, 
                                             &ccontext ->memfifo_sent,ccontext -> flight_recorder); 
                                   
}

__INLINE__   uint64_t dput_memfifo(struct mudm_connection * ccontext,
                                               uint16_t type,
                                               uint64_t payload_phys_addr,
                                               uint64_t payload_length,
                                               uint64_t * send_count 
                                               
               ){               
  return InjFifoInjectMemFifo (ccontext->injfifo_ctls, 
                                             &ccontext->mu_iMemoryFifoDescriptor,
                                             type,payload_phys_addr, payload_length, 
                                             send_count,ccontext -> flight_recorder); 
                                   
}
////////////////////////////////////////////

__INLINE__ void set_dma_directput(MUHWI_Descriptor_t *desc, uint64_t bytes,
                                   uint64_t  source_physaddr, uint64_t  dest_physaddr)
{ 
  /* Set pointer to message unit header within the packet header. */
  MUHWI_MessageUnitHeader_t *muh =  &(desc->PacketHeader.messageUnitHeader);
  desc->Pa_Payload = source_physaddr;
  desc->Message_Length  = bytes;
  /* Set most significant 5 bits of the 37-bit put offset.  */
  muh->Packet_Types.Direct_Put.Put_Offset_MSB =     ( dest_physaddr >> 32 ) & 0x01f; 
  /* Set least significant 32 bits of the 37-bit put offset.       */
  muh->Packet_Types.Direct_Put.Put_Offset_LSB =  dest_physaddr & 0xFFFFFFFF; 

}

__INLINE__ void set_counter_directput(MUHWI_Descriptor_t *desc, uint64_t  Rec_Counter_Offset){
  /* Reference counter atomic setup in MUSPI_GetAtomicOffsetFromBaseAddress in Addressing_inlines.h */
  /* Assuming remote has a base address w/o atomic op set so that atomic op can be set on the counter */
  /* Set pointer to message unit header within the packet header. */
  MUHWI_MessageUnitHeader_t *muh =  &(desc->PacketHeader.messageUnitHeader);
  MUSPI_assert ( ( Rec_Counter_Offset & 0x7 ) == 0 );
  //see MU_Addressing.h for the difference between MUHWI_ATOMIC_OPCODE_STORE_ADD and MUHWI_ATOMIC_OPCODE_STORE_ADD_COHERENCE_ON_ZERO
  //muh->Packet_Types.Direct_Put.Counter_Offset = Rec_Counter_Offset | MUHWI_ATOMIC_ADDRESS_INDICATOR | MUHWI_ATOMIC_OPCODE_STORE_ADD; 
  muh->Packet_Types.Direct_Put.Counter_Offset = Rec_Counter_Offset | MUHWI_ATOMIC_ADDRESS_INDICATOR | MUHWI_ATOMIC_OPCODE_STORE_ADD_COHERENCE_ON_ZERO; 
}
   
/*
  typedef struct MUDM_InjFifo
{
  MUSPI_InjFifo_t * injfifo; 
  Lock_Atomic_t   * inj_fifo_lock;
  volatile uint32_t state;         //0=inactive; 1=active
} MUDM_InjFifo_t;
*/

__INLINE__ uint64_t InjFifoInjectDirectPut (MUDM_InjFifo_t   * injfifo_ctls, 
					    MUHWI_Descriptor_t * desc, 
                                            uint64_t            bytes,
                                            uint64_t  source_physaddr, 
                                            uint64_t  dest_physaddr,
                                            uint64_t  Rec_Counter_Offset,
                                            BG_FlightRecorderRegistry_t *logregistry) 

{   
    MUHWI_Descriptor_t * descPtr;
    void * descPtrTemp=NULL;
    uint64_t desc_count;
    lock(&injfifo_ctls->inj_fifo_lock);
    MUDM_GET_NEXT_DESC(desc_count,injfifo_ctls,&descPtrTemp);
    descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    set_dma_directput(descPtr, bytes,source_physaddr, dest_physaddr);
    set_counter_directput(descPtr, Rec_Counter_Offset);
    //$$$
    //DUMPHEXSTR("directput inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    LOG_DESCR_HEADER(logregistry,descPtr);
    INJECT_DIRECTPUT(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo_ctls->injfifo);
    unlock(&injfifo_ctls->inj_fifo_lock);
    //$$$
    PRINT("directput injected %p \n",descPtr);
    return desc_count;
}

__INLINE__ uint64_t InjFifoInjectDirectPut_withDest (MUDM_InjFifo_t   * injfifo_ctls,                                             
					    MUHWI_Descriptor_t * desc, 
                                            MUHWI_Destination_t  dest,       /* remote destination    */
                                            uint64_t            bytes,
                                            uint64_t  source_physaddr, 
                                            uint64_t  dest_physaddr,
                                            uint64_t  Rec_Counter_Offset,
                                            BG_FlightRecorderRegistry_t *logregistry) 

{   
    MUHWI_Descriptor_t * descPtr;
    void * descPtrTemp=NULL;

    uint64_t desc_count;
    lock(&injfifo_ctls->inj_fifo_lock);
    MUDM_GET_NEXT_DESC(desc_count,injfifo_ctls,&descPtrTemp);
    descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    set_dma_directput(descPtr, bytes,source_physaddr, dest_physaddr);
    set_counter_directput(descPtr, Rec_Counter_Offset);

    descPtr->PacketHeader.NetworkHeader.pt2pt.Destination = dest; /* Set the destination. */
    MHERE;
    MDUMPHEXSTR("directput inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    LOG_DESCR_HEADER(logregistry,descPtr);
    INJECT_DIRECTPUT(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo_ctls->injfifo);
    unlock(&injfifo_ctls->inj_fifo_lock);
    //$$$
    PRINT("directput injected %p \n",descPtr);
    return desc_count;
}


__INLINE__ SoftwareBytes_t  * addressSoftwareBytes( MUHWI_Descriptor_t *desc)
{
    /* Set pointer to message unit header within the packet header. */
    MUHWI_MessageUnitHeader_t *muh =  &(desc->PacketHeader.messageUnitHeader); 
    return (SoftwareBytes_t  *)&muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;
}


////////////////////////////////////////////

////////////////////////////////////////////
__INLINE__   uint64_t direct_put(struct mudm_connection * ccontext,
               uint64_t source_phys_addr,
               uint64_t target_phys_addr,
               uint64_t put_length,
               uint64_t counter){               
  return  InjFifoInjectDirectPut (ccontext->injfifo_ctls, &ccontext->mu_iDirectPutDescriptor, 
                                              put_length, source_phys_addr,  target_phys_addr,
                                              counter,  ccontext->flight_recorder); 
                                   
}//end direct_put
////////////////////////////////////////////

/**
 *
 * \brief Inject Collective
 *
 * \param[in]  desc             Pointer to descriptor.
 *
 * \ingroup COLLECTIVE_DESC
 */
__INLINE__ 
uint64_t InjFifoInjectMemFifoBcast (MUDM_InjFifo_t   * injfifo_ctls,
					 MUHWI_Descriptor_t  *desc,
                                         uint16_t type,
                                         uint64_t payload_phys_addr,
                                         uint64_t payload_length, 
                                         uint8_t  class_route,                                         
                                         uint64_t * send_count,
                                         BG_FlightRecorderRegistry_t *logregistry) 
{   
    MUHWI_Descriptor_t  * descPtr;
    void * descPtrTemp=NULL;
    MUHWI_MessageUnitHeader_t *muh =  NULL; 
    struct broadcast_pkt_header * b = NULL;
    /* Set pointer to the network header portion of the packet header.  */
    uint64_t desc_count;
    ENTER;
    lock(&injfifo_ctls->inj_fifo_lock);
    MUDM_GET_NEXT_DESC(desc_count,injfifo_ctls,&descPtrTemp);
    //MDUMPHEXSTR("template descriptor",desc,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    muh =  &(descPtr->PacketHeader.messageUnitHeader); 
    b = (struct broadcast_pkt_header *) &muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;
    descPtr->PacketHeader.NetworkHeader.collective.Byte2.Class_Route = class_route; //set the (system) class route
    b-> payload_length = payload_length;
    b-> type = type;
    b->sequence_number = (uint16_t)*send_count; 
    descPtr->Pa_Payload = payload_phys_addr; /* Set payload physical address.  */
    descPtr->Message_Length = payload_length; 
    ++(*send_count);
    // $$$     
    PRINT("payload_phys_addr=%p payload_length=%llu class_route=%d \n",(void*)payload_phys_addr,(LLUS)payload_length, class_route);
    LOG_DESCR_HEADER(logregistry,descPtr);
    INJ_MEMFIFO_BCAST(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo_ctls->injfifo);
    unlock(&injfifo_ctls->inj_fifo_lock);
    //$$$
    PRINT("injected %p memfifo desc_count=%llu \n",descPtr,(LLUS)desc_count);
    //MDUMPHEXSTR("POST INJECT:  memfifo inj descriptor",descPtr,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    EXIT;
    return desc_count;
}
////////////////////////////////////////////
/**
 *
 * \brief Inject Collective
 *
 * \param[in]  desc             Pointer to descriptor.
 */
__INLINE__ 
uint64_t InjFifoInjectReduce (MUSPI_InjFifo_t   * injfifo, 
					 MUHWI_Descriptor_t  *desc,
                                         uint16_t type,
                                         uint64_t payload_phys_addr,
                                         uint64_t payload_length,
                                         uint8_t opCode,
                                         MUHWI_Destination_t dest,
                                         Lock_Atomic_t   * inj_fifo_lock,
                                         uint8_t reduce_value,
                                         uint8_t class_route,
                                         uint64_t * send_count,
                                         BG_FlightRecorderRegistry_t *logregistry) 
{   

    
    uint64_t bytesLength         = payload_length - 1;
    uint64_t desc_count;
    unsigned int numNonZeroBits = popcnt64 (bytesLength);
    MUHWI_Descriptor_t  * descPtr;
    void * descPtrTemp=NULL;
    MUHWI_MessageUnitHeader_t *muh =  NULL; 
    struct broadcast_pkt_header * b = NULL;
    MUHWI_CollectiveNetworkHeader_t *pkth = NULL;
    
    ENTER;
    lock(inj_fifo_lock);
    do {
       desc_count = MUSPI_InjFifoNextDesc (injfifo,(void **)&descPtrTemp);
    }while (desc_count == -1);
    //MHERE;
    descPtr = (MUHWI_Descriptor_t  * )descPtrTemp;
    memcpy (descPtr, desc, BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
    desc=NULL;  /* lose reference to template descriptor */
    muh =  &(descPtr->PacketHeader.messageUnitHeader); 
    b = (struct broadcast_pkt_header *) &muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;
    /* Set pointer to the network header portion of the packet header.  */
    pkth = &(descPtr->PacketHeader.NetworkHeader.collective);
    pkth->Destination = dest; /* Set the destination.  Bytes 4 - 7.  MU_CollectiveNetworkHeader.h */
    //pkth->Byte3.Collective_Type 
    pkth->Byte3.Byte3 = ( pkth->Byte3.Byte3 & 0xE7 ) | reduce_value;
    pkth->Byte1.Byte1 = (( numNonZeroBits - 2 ) & 0x7) | opCode;
    b-> payload_length = payload_length;
    b-> type = type;
    b->sequence_number = (uint16_t)*send_count; 
    descPtr->Pa_Payload = payload_phys_addr; /* Set payload physical address.  */
    descPtr->Message_Length = payload_length; 
    ++(*send_count);
    descPtr->PacketHeader.NetworkHeader.collective.Byte2.Class_Route = class_route; //set the (system) class route;
    LOG_DESCR_HEADER(logregistry,descPtr);
    INJ_REDUCE_BCAST(logregistry,&descPtr->PacketHeader);
    my_mbar();//no need for mbar once special TLB bit U1=1
    MUSPI_InjFifoAdvanceDesc (injfifo);
    unlock(inj_fifo_lock);
    return desc_count;
    EXIT;
}
////////////////////////////////////////////



#endif /* _MUDM_COMMON_INLINES_H_ */
