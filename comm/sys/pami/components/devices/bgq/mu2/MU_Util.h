/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file components/devices/bgq/mu2/MU_Util.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_MU_Util_h__
#define __components_devices_bgq_mu2_MU_Util_h__

#include "spi/include/mu/DescriptorBaseXX.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {      
      static const size_t NumTorusDims = BGQ_TDIMS;
      static const uint64_t UNDEFINED_SEQ_NO = 0xffffffffffffffffUL;	
      static const uint64_t seqno_table[2] = {0, UNDEFINED_SEQ_NO};

      static const uint64_t    _mu_fifomaps[NumTorusDims * 2]  =  
	{
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM,
	  MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP 
	};
      
      static const uint64_t AnyFifoMap = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM | 
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP |
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM |
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP |
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM |
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP |
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM |
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP |
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM |
	MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP; 
      
      static const uint8_t     _mu_hintsABCD[NumTorusDims * 2] = 
	{MUHWI_PACKET_HINT_AM,
	 MUHWI_PACKET_HINT_AP,
	 MUHWI_PACKET_HINT_BM,
	 MUHWI_PACKET_HINT_BP,
	 MUHWI_PACKET_HINT_CM,
	 MUHWI_PACKET_HINT_CP,
	 MUHWI_PACKET_HINT_DM,
	 MUHWI_PACKET_HINT_DP,
	 MUHWI_PACKET_HINT_A_NONE | 
	 MUHWI_PACKET_HINT_B_NONE | 
	 MUHWI_PACKET_HINT_C_NONE | 
	 MUHWI_PACKET_HINT_D_NONE,
	 MUHWI_PACKET_HINT_A_NONE | 
	 MUHWI_PACKET_HINT_B_NONE | 
	 MUHWI_PACKET_HINT_C_NONE | 
	 MUHWI_PACKET_HINT_D_NONE};
      
      static const uint8_t     _mu_pt2ptmisc1 [NumTorusDims * 2] = 
	{
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_EM | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING,
	  MUHWI_PACKET_HINT_EP | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING
	};

      
      static const uint8_t     _mu_pt2ptmisc1_deposit [NumTorusDims * 2] = 
	{
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_E_NONE | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_EM | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT,
	  MUHWI_PACKET_HINT_EP | MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE | MUHWI_PACKET_USE_DETERMINISTIC_ROUTING | MUHWI_PACKET_DEPOSIT
	};      


      static inline void buildP2pDputModelDescriptor (MUSPI_DescriptorBase  & desc,
						      uint16_t                pbatid = 0,
						      uint16_t                cbatid = 0) 
      {
	// Zero-out the descriptor models before initialization
	memset((void *)&desc, 0, sizeof(MUSPI_DescriptorBase));
	
	// --------------------------------------------------------------------
	// Set the common base descriptor fields
	// 
	// For the remote get packet, send it using the high priority torus
	// fifo map.  Everything else uses non-priority torus fifos, pinned
	// later based on destination.  This is necessary to avoid deadlock
	// when the remote get fifo fills.  Note that this is in conjunction
	// with using the high priority virtual channel (set elsewhere).
	// --------------------------------------------------------------------
	MUSPI_BaseDescriptorInfoFields_t base;
	memset((void *)&base, 0, sizeof(base));
	
	base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
	desc.setBaseFields (&base);
	
	// --------------------------------------------------------------------
	// Set the common point-to-point descriptor fields
	//
	// For the remote get packet, send it on the high priority virtual
	// channel.  Everything else is on the deterministic virtual channel.
	// This is necessary to avoid deadlock when the remote get fifo fills.
	// Note that this is in conjunction with setting the high priority
	// torus fifo map (set elsewhere).
	// --------------------------------------------------------------------
	MUSPI_Pt2PtDescriptorInfoFields_t pt2pt;
	memset((void *)&pt2pt, 0, sizeof(pt2pt));
	
	pt2pt.Misc1 =
	  MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
	  MUHWI_PACKET_DO_NOT_DEPOSIT |
	  MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE;
	pt2pt.Misc2 = MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
	
	desc.setDataPacketType (MUHWI_PT2PT_DATA_PACKET_TYPE);
	desc.PacketHeader.NetworkHeader.pt2pt.Byte8.Size = 16;
	desc.setPt2PtFields (&pt2pt);
	
	// --------------------------------------------------------------------                                                      
	// Set the direct put descriptor fields                                                                                      
	// --------------------------------------------------------------------                                                      
	MUSPI_DirectPutDescriptorInfoFields dput;
	memset((void *)&dput, 0, sizeof(dput));
	
	dput.Rec_Payload_Base_Address_Id = pbatid;
	dput.Rec_Payload_Offset          = 0;
	dput.Rec_Counter_Base_Address_Id = cbatid;
	dput.Rec_Counter_Offset          = 0;
	dput.Pacing                      = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;
	
	desc.setDirectPutFields (&dput);
	desc.setMessageUnitPacketType (MUHWI_PACKET_TYPE_PUT);  

	//Skip link checksumming the packet header
	desc.setPt2PtSkip(8);	
      }


    }; //-- MU
  }; //-- Device
}; //-- PAMI

#endif
