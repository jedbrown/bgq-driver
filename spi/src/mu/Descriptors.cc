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


/*!
 * \file Descriptors.cc
 *
 * \brief Message Unit SPI Descriptor Functions
 *
 */

#include <new>
#include <hwi/include/common/compiler_support.h>
#include <spi/include/mu/DescriptorWrapperXX.h>
#include <spi/include/mu/Descriptor.h>
#include <spi/include/mu/Pt2PtMemoryFIFODescriptorXX.h>
#include <spi/include/mu/Pt2PtDirectPutDescriptorXX.h>
#include <spi/include/mu/Pt2PtRemoteGetDescriptorXX.h>
#include <spi/include/mu/CollectiveMemoryFIFODescriptorXX.h>
#include <spi/include/mu/CollectiveDirectPutDescriptorXX.h>
#include <spi/include/mu/CollectiveRemoteGetDescriptorXX.h>


/*!
 * \brief Extern C
 *
 * Tell the compiler to expose the interfaces in this file as C interfaces,
 * without name mangling.
 */
extern "C" {


/*!
 * \brief Create a Descriptor For a Point-to-Point Memory FIFO Message
 *
 * A memory FIFO message is one that is sent to another node and whose packets
 * are placed into a designated in-memory reception FIFO (as specified in the
 * packet header).  The reception FIFO must be polled by the core(s) to process
 * the packets.
 *
 * This function creates the memory FIFO descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreatePt2PtMemoryFIFODescriptor( MUHWI_Descriptor_t                    *desc,
					   MUSPI_Pt2PtMemoryFIFODescriptorInfo_t *info 
					 )
{
  /* Instantiate a point-to-point memory FIFO descriptor on top of the storage
   * pointed to by "desc".
   */
  new (desc) MUSPI_Pt2PtMemoryFIFODescriptor( info );

  return 0;
}


/*!
 * \brief Create a Descriptor For a Point-to-Point Direct Put Message
 *
 * A direct put message is one that is sent to another node and its data
 * is directly put into memory by the MU on the destination node...it does
 * not go into a reception fifo.
 *
 * This function creates the direct-put descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreatePt2PtDirectPutDescriptor( MUHWI_Descriptor_t                   *desc,
					  MUSPI_Pt2PtDirectPutDescriptorInfo_t *info 
					)
{
  /* Instantiate a point-to-point direct put descriptor on top of the storage
   * pointed to by "desc".
   */
  new (desc) MUSPI_Pt2PtDirectPutDescriptor( info );

  return 0;
}


/*!
 * \brief Create a Descriptor For a Point-to-Point Remote Get Message
 *
 * A remote get message is one that is sent to another node and its data
 * (containing one or more descriptors) is injected into a remote get injection
 * FIFO by the MU.  It is not received by the cores.
 *
 * This function creates the remote get descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreatePt2PtRemoteGetDescriptor( MUHWI_Descriptor_t                   *desc,
					  MUSPI_Pt2PtRemoteGetDescriptorInfo_t *info 
					)
{
  /* Instantiate a point-to-point remote get descriptor on top of the storage
   * pointed to by "desc".
   */
  new (desc) MUSPI_Pt2PtRemoteGetDescriptor( info );

  return 0;
}


/*!
 * \brief Create a Descriptor For a Collective Memory FIFO Message
 *
 * A memory FIFO message is one that is sent to another node and whose packets
 * are placed into a designated in-memory reception FIFO (as specified in the
 * packet header).  The reception FIFO must be polled by the core(s) to process
 * the packets.
 *
 * This function creates the memory FIFO descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreateCollectiveMemoryFIFODescriptor( MUHWI_Descriptor_t                         *desc,
						MUSPI_CollectiveMemoryFIFODescriptorInfo_t *info 
					      )
{
  /* Instantiate a collective memory FIFO descriptor on top of the storage
   * pointed to by "desc".
   */
  new (desc) MUSPI_CollectiveMemoryFIFODescriptor( info );

  return 0;
}


/*!
 * \brief Create a Descriptor For a Collective Direct Put Message
 *
 * A direct put message is one that is sent to another node and its data
 * is directly put into memory by the MU on the destination node...it does
 * not go into a reception fifo.
 *
 * This function creates the direct-put descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreateCollectiveDirectPutDescriptor( MUHWI_Descriptor_t                        *desc,
					       MUSPI_CollectiveDirectPutDescriptorInfo_t *info 
					     )
{
  /* Instantiate a collective direct put descriptor on top of the storage
   * pointed to by "desc".
   */
  new (desc) MUSPI_CollectiveDirectPutDescriptor( info );

  return 0;
}


/*!
 * \brief Create a Descriptor For a Collective Remote Get Message
 *
 * A remote get message is one that is sent to another node and its data
 * (containing one or more descriptors) is injected into a remote get injection
 * FIFO by the MU.  It is not received by the cores.
 *
 * This function creates the remote get descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreateCollectiveRemoteGetDescriptor( MUHWI_Descriptor_t                        *desc,
					       MUSPI_CollectiveRemoteGetDescriptorInfo_t *info 
					     )
{
  /* Instantiate a collective remote get descriptor on top of the storage
   * pointed to by "desc".
   */
  new (desc) MUSPI_CollectiveRemoteGetDescriptor( info );

  return 0;
}


/*!
 * \brief Dump a Descriptor
 *
 * Dump the contents of a descriptor to stdout.
 *
 * \param[in]  identifier  Pointer to a string to be printed.
 * \param[in]  desc        Pointer to the descriptor.
 */

void MUSPI_DescriptorDumpHex ( char *identifier,
			       MUHWI_Descriptor_t *desc )
{
  uint32_t *desc32 = (uint32_t*)desc;
  printf("%s:  Descriptor at address %lx dump:\n"
	 "0x%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n",
	 identifier,
	 (uint64_t)desc,
	 desc32[0],
	 desc32[1],
	 desc32[2],
	 desc32[3],
	 desc32[4],
	 desc32[5],
	 desc32[6],
	 desc32[7],
	 desc32[8],
	 desc32[9],
	 desc32[10],
	 desc32[11],
	 desc32[12],
	 desc32[13],
	 desc32[14],
	 desc32[15]);
}


#if 0 /* Start of unused C implementations */

/*!
 * \brief Create a Descriptor For a Point-to-Point Memory FIFO Message
 *
 * A memory FIFO message is one that is sent to another node and whose packets
 * are placed into a designated in-memory reception FIFO (as specified in the
 * packet header).  The reception FIFO must be polled by the core(s) to process
 * the packets.
 *
 * This function creates the memory FIFO descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreatePt2PtMemoryFIFODescriptor( MUHWI_Descriptor_t                    *desc,
					   MUSPI_Pt2PtMemoryFIFODescriptorInfo_t *info 
					 )
{
  /* Set pointer to packet header portion of the descriptor. */
  MUHWI_Pt2PtNetworkHeader_t *pkth = &(desc->PacketHeader.NetworkHeader.pt2pt); 

  /* Set pointer to message unit header within the packet header. */
  MUHWI_MessageUnitHeader_t *muh = 
    &(desc->PacketHeader.messageUnitHeader); 
  
  MUSPI_DescriptorZeroOut ( desc ); /* Clear the descriptor                    */

  pkth->Data_Packet_Type = MUHWI_PT2PT_DATA_PACKET_TYPE; /* Set
							    pt2pt
							    type */

  pkth->Hints = info->Pt2Pt.Hints_ABCD; /* Set ABCD hints.       */

  pkth->Byte2.Byte2 = info->Pt2Pt.Misc1;/* Set all byte2 fields at 
					   once.                 */

  pkth->Byte3.Byte3 = info->Pt2Pt.Misc2;/* Set all byte3 fields at 
					   once.                 */

  pkth->Destination = info->Base.Dest; /* Set the destination.*/

  pkth->Byte8.Byte8 = MUHWI_PACKET_TYPE_FIFO; /* Set packet
						 type.        */

  pkth->Byte8.Size  = 
    MUSPI_DescriptorGetNumPacketChunks ( info->Base.Message_Length ); /* Set 
							    number of packet
							    chunks in first
							    packet.          */
  
  pkth->Injection_Info.Skip = info->Pt2Pt.Skip; /* Set bytes to
						   skip for 
						   checksum.    */

  muh->Packet_Types.Memory_FIFO.Rec_FIFO_Id =
    info->MemFIFO.Rec_FIFO_Id;             /* Set reception FIFO Id.          */

  muh->Packet_Types.Memory_FIFO.Put_Offset_MSB =
    ( info->MemFIFO.Rec_Put_Offset >> 32 ) & 0x01f; /* Set most significant 5
						       bits of the 37-bit put
						       offset.                */

  muh->Packet_Types.Memory_FIFO.Put_Offset_LSB =
    info->MemFIFO.Rec_Put_Offset & 0xFFFFFFFF;      /* Set least significant 32
						       bits of the 37-bit put
						       offset.                */

  desc->Half_Word0.Half_Word0 = info->Base.Pre_Fetch_Only;  /* Set 
						               pre-fetch-only 
						               indicator.     */

  desc->Half_Word1.Half_Word1 = info->MemFIFO.Interrupt;    /* Set interrupt 
							       indicator.     */

  desc->Pa_Payload = info->Base.Payload_Address; /* Set payload physical address.  */

  desc->Message_Length = info->Base.Message_Length;/* Set message length.   */

  desc->Torus_FIFO_Map = info->Base.Torus_FIFO_Map;/* Set torus fifo map.   */

  return 0;
}


/*!
 * \brief Create a Descriptor For a Point-to-Point Direct Put Message
 *
 * A direct put message is one that is sent to another node and its data
 * is directly put into memory by the MU on the destination node...it does
 * not go into a reception fifo.
 *
 * This function creates the direct-put descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreatePt2PtDirectPutDescriptor( MUHWI_Descriptor_t                   *desc,
					  MUSPI_Pt2PtDirectPutDescriptorInfo_t *info 
					)
{
  /* Set pointer to packet header portion of the descriptor. */
  MUHWI_Pt2PtNetworkHeader_t *pkth = &(desc->PacketHeader.NetworkHeader.pt2pt); 

  /* Set pointer to message unit header within the packet header. */
  MUHWI_MessageUnitHeader_t *muh = 
    &(desc->PacketHeader.messageUnitHeader); 
  
  MUSPI_DescriptorZeroOut ( desc ); /* Clear the descriptor                    */

  pkth->Data_Packet_Type = MUHWI_PT2PT_DATA_PACKET_TYPE; /* Set
							    pt2pt
							    type */

  pkth->Hints = info->Pt2Pt.Hints_ABCD; /* Set ABCD hints.       */

  pkth->Byte2.Byte2 = info->Pt2Pt.Misc1;/* Set all byte2 fields at 
					   once.                 */

  pkth->Byte3.Byte3 = info->Pt2Pt.Misc2; /* Set all byte3 fields at 
					    once.                */

  pkth->Destination = info->Base.Dest; /* Set the destination.*/

  pkth->Byte8.Byte8 = MUHWI_PACKET_TYPE_PUT; /* Set packet 
						type.         */

  pkth->Byte8.Size  = 
    MUSPI_DescriptorGetNumPacketChunks ( info->Base.Message_Length ); /* Set
						        number of packet chunks
							in first packet.    */

  pkth->Injection_Info.Skip = info->Pt2Pt.Skip; /* Set bytes to 
						   skip for 
						   checksum.    */

  muh->Packet_Types.Direct_Put.Pacing = info->DirectPut.Pacing; /*
							       Set pacing 
							       indicator.     */

  muh->Packet_Types.Direct_Put.Rec_Payload_Base_Address_Id =
    info->DirectPut.Rec_Payload_Base_Address_Id; /* Set payload base address 
						    id.                       */
  
  muh->Packet_Types.Direct_Put.Put_Offset_MSB =
    ( info->DirectPut.Rec_Payload_Offset >> 32 ) & 0x01f; /* Set most significant 5 bits
							     of the 37-bit put offset.  */
  
  muh->Packet_Types.Direct_Put.Put_Offset_LSB =
    info->DirectPut.Rec_Payload_Offset & 0xFFFFFFFF; /* Set least significant 32 bits
							of the 37-bit put offset.       */

  muh->Packet_Types.Direct_Put.Rec_Counter_Base_Address_Id =
    info->DirectPut.Rec_Counter_Base_Address_Id; /* Set reception counter base
						    address id.               */

  muh->Packet_Types.Direct_Put.Counter_Offset =
    info->DirectPut.Rec_Counter_Offset;          /* Set reception counter offset. */

  desc->Half_Word0.Half_Word0 = info->Base.Pre_Fetch_Only; /* Set 
							      pre-fetch-only
							      indicator.    */

  desc->Pa_Payload = info->Base.Payload_Address;/* Set payload physical 
						   address.                */

  desc->Message_Length = info->Base.Message_Length; /* Set message length.  */

  desc->Torus_FIFO_Map = info->Base.Torus_FIFO_Map; /* Set torus fifo map.  */

  return 0;
}


/*!
 * \brief Create a Descriptor For a Point-to-Point Remote Get Message
 *
 * A remote get message is one that is sent to another node and its data
 * (containing one or more descriptors) is injected into a remote get injection
 * FIFO by the MU.  It is not received by the cores.
 *
 * This function creates the remote get descriptor that can be injected into
 * a memory injection FIFO.
 *
 * \param[in,out]  desc             Pointer to the storage where the descriptor
 *                                  will be created.
 * \param[in]      info             Pointer to a structure containing parameters
 *                                  needed to build the descriptor.
 *
 * \retval  0  Success
 */

int MUSPI_CreatePt2PtRemoteGetDescriptor( MUHWI_Descriptor_t                   *desc,
					  MUSPI_Pt2PtRemoteGetDescriptorInfo_t *info 
					)
{
  /* Set pointer to packet header portion of the descriptor. */
  MUHWI_Pt2PtNetworkHeader_t *pkth = &(desc->PacketHeader.NetworkHeader.pt2pt); 

  /* Set pointer to message unit header within the packet header. */
  MUHWI_MessageUnitHeader_t *muh = 
    &(desc->PacketHeader.messageUnitHeader); 
  
  MUSPI_DescriptorZeroOut ( desc ); /* Clear the descriptor                    */

  pkth->Data_Packet_Type = MUHWI_PT2PT_DATA_PACKET_TYPE; /* Set
							    pt2pt
							    type */

  pkth->Hints = info->Pt2Pt.Hints_ABCD; /* Set ABCD hints.       */

  pkth->Byte2.Byte2 = info->Pt2Pt.Misc1; /* Set all byte2 fields at
					    once.                */

  pkth->Byte3.Byte3 = info->Pt2Pt.Misc2; /* Set all byte3 fields at 
					    once.                */

  pkth->Destination = info->Base.Dest; /* Set the destination.*/

  pkth->Byte8.Byte8 = info->RemoteGet.Type; /* Set packet type   */

  pkth->Byte8.Size  = 
    MUSPI_DescriptorGetNumPacketChunks ( info->Base.Message_Length ); /* Set 
						        number of packet chunks
							in first packet.    */

  pkth->Injection_Info.Skip = info->Pt2Pt.Skip; /* Set bytes to 
						   skip for
						   checksum.  */

  muh->Packet_Types.Remote_Get.Rget_Inj_FIFO_Id =
    info->RemoteGet.Rget_Inj_FIFO_Id;    /* Set remote get injection FIFO Id*/

  desc->Half_Word0.Half_Word0 = info->Base.Pre_Fetch_Only;  /* Set 
							       pre-fetch-only 
							       indicator.   */

  desc->Pa_Payload = info->Base.Payload_Address; /* Set payload physical
						    address.                */

  desc->Message_Length = info->Base.Message_Length; /* Set message length.  */

  desc->Torus_FIFO_Map = info->Base.Torus_FIFO_Map; /* Set torus fifo map.  */

  return 0;
}

#endif /* End of unused C implementations */

} /* End of extern "C" */
