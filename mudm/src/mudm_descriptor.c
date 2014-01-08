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

#include "common.h"
#include "mudm_descriptor.h"
#include <mudm/include/mudm_utils.h>
#include <hwi/include/bqc/MU_MessageUnitHeader.h>
#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/mu/Descriptor.h>
#include <spi/include/mu/Descriptor_inlines.h>

#include <mudm/include/mudm.h>
#include <mudm/include/mudm_inlines.h>
#include <mudm/include/mudm_utils.h>
#include "mudm_macro.h"
#include "mudm_trace.h"


void Update_memfifo_payload( MUHWI_Descriptor_t  *desc,
                           uint64_t payload_phys_addr,
                           uint64_t payload_length
                                             )
{       
    /* Set pointer to the network header portion of the packet header.  */
   // MUHWI_Pt2PtNetworkHeader_t *pkth = &(desc->PacketHeader.NetworkHeader.pt2pt);
    /* Set pointer to message unit header within the packet header. */
    MUHWI_MessageUnitHeader_t *muh =  &(desc->PacketHeader.messageUnitHeader); 
    struct ionet_header * ionet_hdr = (struct ionet_header * )&muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;
    //SoftwareBytes_t  *swb = (SoftwareBytes_t  *)&muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;
    desc->Pa_Payload = payload_phys_addr; /* Set payload physical address.  */
    desc->Message_Length = payload_length; 
    ionet_hdr = (struct ionet_header * )&muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;
    ionet_hdr->payload_length = payload_length;
    //muh->Packet_Types.Memory_FIFO.Unused1 = 0;
    //muh->Packet_Types.Memory_FIFO.Put_Offset_MSB =0;

}


void InitPt2PtMemoryFIFODescriptor( MUHWI_Descriptor_t                    *desc,
                                               SoftwareBytes_t                       *SoftwareBytes,
                                               uint64_t payload_phys_addr,
                                               uint64_t payload_length,
                                               MUHWI_Destination_t dest,
                                               uint8_t direction, 
                                               uint8_t  io_port,
                                               uint16_t rec_FIFO_Id
                                             )
{
        
    /* Set pointer to the network header portion of the packet header.  */
    MUHWI_Pt2PtNetworkHeader_t *pkth = &(desc->PacketHeader.NetworkHeader.pt2pt);
    /* Set pointer to message unit header within the packet header. */
    MUHWI_MessageUnitHeader_t *muh =  &(desc->PacketHeader.messageUnitHeader); 
    ENTER;
    PRINT("MUHWI_Destination dest = %x \n",*(uint32_t *)&dest);
    MUSPI_DescriptorZeroOut(desc);
    /* set data packet type */
    MUSPI_SetDataPacketType(desc,MUHWI_PT2PT_DATA_PACKET_TYPE);
    /* set MU packet type */ 
    MUSPI_SetMessageUnitPacketType(desc,MUHWI_PACKET_TYPE_FIFO);
    /* set base fields */  
    desc->Half_Word0.Half_Word0 = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO; /* Set pre-fetch-only.  */
    desc->Pa_Payload = payload_phys_addr; /* Set payload physical address.  */
    desc->Message_Length = payload_length; 
    pkth->Byte8.Size  = 16; /* Set number of packet chunks to the max.  The MU will adjust it from there. */
    desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_SYSTEM;  // network inj FIFO #13 (0x0000000000000004ULL)
       
    pkth->Destination = dest; /* Set the destination. */

    pkth->Hints = 0; /*hw calcs hint bits */  

    pkth->Byte3.Byte3 = MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM; /* set top 3 bits for virt channel */                
    pkth->Injection_Info.Skip = 0; /* Set number of 4-byte words to skip for checksum.  */
     
    switch(direction){
     case CN2IONODE:
     {
      PRINT("CN2IONODE \n");
      pkth->Byte2.Byte2      = 
       MUHWI_PACKET_HINT_E_NONE               |
       MUHWI_PACKET_ROUTE_TO_IO_NODE          |
       MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
       MUHWI_PACKET_DO_NOT_DEPOSIT;
       break;
     }
     case IONODE2CN:
     {
      PRINT("IONODE2CN \n");
      pkth->Byte2.Byte2      = 
       MUHWI_PACKET_HINT_E_NONE                       |
       MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE           |
       io_port                                        | 
       MUHWI_PACKET_USE_DETERMINISTIC_ROUTING         |
       MUHWI_PACKET_DO_NOT_DEPOSIT;
       HERE;
       PRINT("io_port = %x \n",io_port);
       switch(io_port){
       case MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6:
        desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM; //network inj FIFO #6       (0x0000000000000200ULL)
        break;
       case MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7:
        desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP;//network inj FIFO #7       (0x0000000000000100ULL)
        break;
       case MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT:
       /* use default for io_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT */
       default:
        desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_SYSTEM;  // network inj FIFO #13 (0x0000000000000004ULL)
       }
       MUSPI_assert( (io_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT)||
                     (io_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6) ||
                     (io_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7)
                   );
      break;
     }
     case NODE_LOOPBACK:
     {
       MPRINT("NODE_LOOPBACK \n");
       MHERE;
       pkth->Byte2.Byte2      = 
       MUHWI_PACKET_DO_NOT_DEPOSIT;
     
       break;
     }
     default:
     {
      PRINT("direction is default\n");
      pkth->Byte2.Byte2      = 
       MUHWI_PACKET_DO_NOT_DEPOSIT;
       break;
     }

    }//end switch(direction)

    pkth->Byte2.Interrupt = 1;  //use packet interrupts

    muh->Packet_Types.Memory_FIFO.Rec_FIFO_Id = rec_FIFO_Id; /* Set reception FIFO Id.         */ 
    muh->Packet_Types.Memory_FIFO.Unused1 = 0;
    muh->Packet_Types.Memory_FIFO.Put_Offset_MSB =0;
    memcpy( &muh->Packet_Types.Memory_FIFO.Put_Offset_LSB, SoftwareBytes,
	  sizeof( muh->Packet_Types.Memory_FIFO.Unused2 ) + 
          sizeof(muh->Packet_Types.Memory_FIFO.Put_Offset_LSB)  ); /* pass software specified bytes */
  /* Set interrupt indicator.    */
  //desc->Half_Word1.Half_Word1 = MUHWI_DESCRIPTOR_DO_NOT_INTERRUPT_ON_PACKET_ARRIVAL; 
  desc->Half_Word1.Half_Word1 = MUHWI_PACKET_INTERRUPT_ON_PACKET_ARRIVAL;
  //DUMPHEXSTR("template descriptor",desc,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
  EXIT;
}


void  initPt2PtDirectPutDescriptor( MUHWI_Descriptor_t *desc,
                                               uint64_t payload_phys_addr,
                                               uint64_t payload_length,
                                               MUHWI_Destination_t dest,
                                               uint8_t direction, 
                                               uint8_t  io_port,
                                               uint64_t Rec_Counter_Offset, 
                                               uint64_t Rec_Payload_Offset
                                               )
{
    
    
    /* Set pointer to the network header portion of the packet header.  */
    MUHWI_Pt2PtNetworkHeader_t *pkth = &(desc->PacketHeader.NetworkHeader.pt2pt);
    /* Set pointer to message unit header within the packet header. */
    MUHWI_MessageUnitHeader_t *muh =  &(desc->PacketHeader.messageUnitHeader);

    ENTER;
    PRINT("MUHWI_Destination dest = %x \n",*(uint32_t *)&dest); 

    MUSPI_DescriptorZeroOut(desc);
    /* set data packet type */
    MUSPI_SetDataPacketType(desc,MUHWI_PT2PT_DATA_PACKET_TYPE);
    /* set MU packet type */ 
    MUSPI_SetMessageUnitPacketType(desc,MUHWI_PACKET_TYPE_PUT);
        /* set base fields */  
    desc->Half_Word0.Half_Word0 = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO; /* Set pre-fetch-only.  */
    desc->Pa_Payload = payload_phys_addr; /* Set payload physical address.  */
    desc->Message_Length = payload_length; 
    pkth->Byte8.Size  = 16; /* Set number of packet chunks to the max.  The MU will adjust it from there. */
 
    desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_SYSTEM;  // network inj FIFO #13 (0x0000000000000004ULL)
    
    pkth->Destination = dest; /* Set the destination. */

    pkth->Hints = 0; /*hw calcs hint bits */  

    pkth->Byte3.Byte3 = MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM; /* set top 3 bits for virt channel */                
    pkth->Injection_Info.Skip = 0; /* Set number of 4-byte words to skip for checksum.  */
     
    switch(direction){
     case CN2IONODE:
     {
      PRINT("CN2IONODE \n"); 
      pkth->Byte2.Byte2      = 
       MUHWI_PACKET_HINT_E_NONE               |
       MUHWI_PACKET_ROUTE_TO_IO_NODE          |
       MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
       MUHWI_PACKET_DO_NOT_DEPOSIT;
       break;
     }
     case IONODE2CN:
     {
      PRINT("IONODE2CN \n");
      pkth->Byte2.Byte2      = 
       MUHWI_PACKET_HINT_E_NONE                       |
       MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE           |
       io_port                                        | 
       MUHWI_PACKET_USE_DETERMINISTIC_ROUTING         |
       MUHWI_PACKET_DO_NOT_DEPOSIT;
       PRINT("io_port = %x \n",io_port);
       switch(io_port){
       case MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6:
        desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM; //network inj FIFO #6       (0x0000000000000200ULL)
        break;
       case MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7:
        desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP;//network inj FIFO #7       (0x0000000000000100ULL)
        break;
       case MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT:
       /* use default for io_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT */
       default:
        desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_SYSTEM;  // network inj FIFO #13 (0x0000000000000004ULL)
       }
       MUSPI_assert( (io_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT)||
                     (io_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6) ||
                     (io_port == MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7)
                   );
      break;
     }
     case NODE_LOOPBACK:
     {
       MPRINT("NODE_LOOPBACK \n");
       MHERE;
       pkth->Byte2.Byte2      = 
       MUHWI_PACKET_DO_NOT_DEPOSIT; 
       break;
     } 
     default:
     {
      PRINT("direction is default\n");
      pkth->Byte2.Byte2      = 
       MUHWI_PACKET_DO_NOT_DEPOSIT;
       break;
     }

    }//end switch(direction)
    
 
  /* Reference counter atomic setup in MUSPI_GetAtomicOffsetFromBaseAddress in Addressing_inlines.h */
  /* Assuming remote has a base address w/o atomic op set so that atomic op can be set on the counter */
  MUSPI_assert ( ( Rec_Counter_Offset & 0x7 ) == 0 );


  /* directput specific headers */
  /* Starting address is 0 so that OFFSET = physcial address */
  muh->Packet_Types.Direct_Put.Pacing = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;
  
  muh->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = SYS_BATID;

  muh->Packet_Types.Direct_Put.Put_Offset_MSB =
    ( Rec_Payload_Offset >> 32 ) & 0x01f; /* Set most significant 5 bits
					           of the 37-bit put offset.  */
  
  muh->Packet_Types.Direct_Put.Put_Offset_LSB =
    Rec_Payload_Offset & 0xFFFFFFFF; /* Set least significant 32 bits
					      of the 37-bit put offset.       */
  
  muh->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id = SYS_BATID;
  ///see MU_Addressing.h for the difference between MUHWI_ATOMIC_OPCODE_STORE_ADD and MUHWI_ATOMIC_OPCODE_STORE_ADD_COHERENCE_ON_ZERO 
  //muh->Packet_Types.Direct_Put.Counter_Offset = Rec_Counter_Offset | MUHWI_ATOMIC_ADDRESS_INDICATOR | MUHWI_ATOMIC_OPCODE_STORE_ADD;
  muh->Packet_Types.Direct_Put.Counter_Offset = Rec_Counter_Offset | MUHWI_ATOMIC_ADDRESS_INDICATOR | MUHWI_ATOMIC_OPCODE_STORE_ADD_COHERENCE_ON_ZERO;

  //DUMPHEXSTR("template descriptor",desc,BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES);
  EXIT;
} 


void InitCollectiveMemoryFIFODescriptor( MUHWI_Descriptor_t          *desc,
                                               uint32_t source_torus_addr,
                                               uint16_t rec_FIFO_Id
                                             )
{
    /* Set pointer to the network header portion of the packet header.  */
    MUHWI_CollectiveNetworkHeader_t *pkth = &(desc->PacketHeader.NetworkHeader.collective);
    /* Set pointer to message unit header within the packet header. */
    MUHWI_MessageUnitHeader_t *muh =  &(desc->PacketHeader.messageUnitHeader); 
    // software specified bytes 
    struct broadcast_pkt_header * b = (struct broadcast_pkt_header *) &muh->Packet_Types.Memory_FIFO.Put_Offset_LSB;

    // ZERO out descriptor and leverage the 0s
    MUSPI_DescriptorZeroOut(desc);

    ////////////////// MU_Descriptor.h elements   
    /* set base fields */  
    //desc->Half_Word0.Half_Word0 = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO; //This is ZERO
    desc->Half_Word1.Half_Word1 = MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL; 
    //desc->Pa_Payload = payload_phys_addr; // Set payload physical address.  Not done in template init 
    //desc->Message_Length = payload_length; //Not done in template init
    desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CSYSTEM; // (0x0000000000000001ULL) injection into the Collective System hardware FIFO
      
    /////////////  // MU_Pt2PtNetworkHeader.h elements 
    /* set data packet type to collective*/
    MUSPI_SetDataPacketType(desc,MUHWI_COLLECTIVE_DATA_PACKET_TYPE);// Byte 0.  Packet type MU_CollectiveNetworkHeader.h 

    //uint8_t Byte1 ( OpCode : 4 ; Word_Length : 4; Collective word length. MU_CollectiveNetworkHeader.h
    //Since this is broadcast, let the zeroing of the descriptor mean OpCode=0 "and" and 4 bytes for Word_Length=0
    //Byte2  Class_Route : 4; /**< Collective class route. */  
    
    pkth->Byte2.Interrupt = 1;  //use packet interrupts
    
    
    //Byte3 Virtual_channel : 3  Collective_Type : 2 is 0 (Broadcast)
    pkth->Byte3.Byte3 = MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM_COLLECTIVE; /* set top 3 bits for virt channel */  
    //pkth->Byte3.Collective_Type=MUHWI_COLLECTIVE_TYPE_BROADCAST='00'b 

    //Destination is irrelevant for broadcast
    //pkth->Destination = dest; /* Set the destination.  Bytes 4 - 7.  MU_CollectiveNetworkHeader.h */

    //Byte8  Packet_Type : 2  uint8_t Size        : 5
    //MUSPI_SetMessageUnitPacketType(desc,MUHWI_PACKET_TYPE_FIFO); Packet_Type=FIFO='00'b    
    pkth->Byte8.Size  = 16; /* Set number of packet chunks to the max.  The MU will adjust it from there. */
         
    // Bytes 9 - 11.  Injection Information.         
    //pkth->Injection_Info.Skip = 0; //use zeroing of struct to set this
     
    /////////////  // MU_MessageUnitHeader.h elements 
    muh->Packet_Types.Memory_FIFO.Rec_FIFO_Id = rec_FIFO_Id; /* Set reception FIFO Id.         */ 
    //muh->Packet_Types.Memory_FIFO.Unused1 = 0; //use zeroing of struct to set this
    //muh->Packet_Types.Memory_FIFO.Put_Offset_MSB =0; //use zeroing of struct to set this

    // software specified bytes     
    b->torus_source_node = source_torus_addr;
    
}

void InitBroadcastDirectPut( MUHWI_Descriptor_t  *desc,uint64_t Rec_Counter_Offset,  uint64_t Rec_Payload_Offset)
{
    /* Set pointer to the network header portion of the packet header.  */
    MUHWI_Pt2PtNetworkHeader_t *pkth = &(desc->PacketHeader.NetworkHeader.pt2pt);
    /* Set pointer to message unit header within the packet header. */
    MUHWI_MessageUnitHeader_t *muh =  &(desc->PacketHeader.messageUnitHeader); 

    // ZERO out descriptor and leverage the 0s
    MUSPI_DescriptorZeroOut(desc);

    ////////////////// MU_Descriptor.h elements   
    /* set base fields */  
    //desc->Half_Word0.Half_Word0 = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO; //This is ZERO
    //desc->Half_Word1.Half_Word1 = MUHWI_DESCRIPTOR_DO_NOT_INTERRUPT_ON_PACKET_ARRIVAL; //This is ZERO--NA for Dput
    //desc->Pa_Payload = payload_phys_addr; //  Set source payload physical address.  Not done here in the template   
    //desc->Message_Length = payload_length; //Not done here in the template
    desc->Torus_FIFO_Map = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CSYSTEM; // (0x0000000000000001ULL) injection into the Collective System hardware FIFO
      
    /////////////  //MU_CollectiveNetworkHeader.h elements 
    /* set data packet type to collective*/
    MUSPI_SetDataPacketType(desc,MUHWI_COLLECTIVE_DATA_PACKET_TYPE);// Byte 0.  Packet type MU_CollectiveNetworkHeader.h 
    pkth->Byte8.Size  = 16; /* Set number of packet chunks to the max.  The MU will adjust it from there. */
    
    //Byte3 Virtual_channel : 3  Collective_Type : 2 is 0 (Broadcast)
    pkth->Byte3.Byte3 = MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM_COLLECTIVE; /* set top 3 bits for virt channel */  
    //pkth->Byte3.Collective_Type=MUHWI_COLLECTIVE_TYPE_BROADCAST='00'b 

    //Destination is irrelevant for broadcast
    //pkth->Destination = dest; /* Set the destination.  Bytes 4 - 7.  MU_CollectiveNetworkHeader.h */

    //Byte8  Packet_Type : 2  uint8_t Size        : 5
    MUSPI_SetMessageUnitPacketType(desc,MUHWI_PACKET_TYPE_PUT); //Packet_Type= 01 = Put 
    //pkth->Byte8.Size  = 0; /* Set number of packet chunks to 0 
         
    // Bytes 9 - 11.  Injection Information.         
    //pkth->Injection_Info.Skip = 0; //use zeroing of struct to set this
     
    /////////////  //DirectPut MU_MessageUnitHeader.h elements 
    //uint16_t Rec_Payload_Base_Address_Id : 10;  Reception payload's  base address Id.
      /* directput specific headers */
    /* Starting address is 0 so that OFFSET = physcial address */
    //muh->Packet_Types.Direct_Put.Pacing = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;   0 = Packet is not paced

    
    muh->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id = SYS_BATID;  //use the 0 value basing entry like for mostly everything else
    muh->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id = SYS_BATID_BCAST_CTR;
    /* Reference counter atomic setup in MUSPI_GetAtomicOffsetFromBaseAddress in Addressing_inlines.h */
    /* Assuming remote has a base address w/o atomic op set so that atomic op can be set on the counter */
    MUSPI_assert ( ( Rec_Counter_Offset & 0x7 ) == 0 );
    muh->Packet_Types.Direct_Put.Counter_Offset = Rec_Counter_Offset | MUHWI_ATOMIC_ADDRESS_INDICATOR | MUHWI_ATOMIC_OPCODE_STORE_ADD_COHERENCE_ON_ZERO;
    muh->Packet_Types.Direct_Put.Put_Offset_MSB =
      ( Rec_Payload_Offset >> 32 ) & 0x01f; /* Set most significant 5 bits
					           of the 37-bit put offset.  */
  
    muh->Packet_Types.Direct_Put.Put_Offset_LSB =
        Rec_Payload_Offset & 0xFFFFFFFF; /* Set least significant 32 bits
					      of the 37-bit put offset.       */
}

