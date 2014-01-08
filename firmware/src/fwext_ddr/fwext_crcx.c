/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/fwext/fwext.h>
#include <hwi/include/bqc/A2_inlines.h>
//#include <hwi/include/bqc/testint_inlines.h>
#include <hwi/include/bqc/dcr_support.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
//#include <stdlib.h>
#include <firmware/include/fwext/fwext_mu.h>

#include <spi/include/mu/GIBarrier.h>
#include <spi/include/mu/Descriptor.h>
#include <spi/include/kernel/MU.h>
//#include <spi/include/mu/InjFifo.h>

#define CRCX_INJ_FIFO_SUBGROUP  67
#define CRCX_INJ_FIFO_ID        7

#define CRCX_REC_FIFO_SUBGROUP  67
#define CRCX_REC_FIFO_GROUP     (CRC_REC_FIFO_SUBGROUP/BGQ_MU_NUM_FIFO_SUBGROUPS)
#define CRCX_REC_FIFO_ID        3
#define CRCX_REC_FIFO_GLOBAL_ID ( (CRCX_REC_FIFO_SUBGROUP*4) + CRCX_REC_FIFO_ID )

#define CRCX_NOT_DEFINED 0xDEADBEEFDEADBEEFul
#define CRCX_SENT        0x0123456789ABCDEFul
#define CRCX_VALIDATED   0xC00DF00DC00DF00Dul
#define CRCX_FAILED      0xBADBADBADBADBAD0ul


#define CRCX_DEBUG(s) printf s ;
#define CRCX_TERMINATE() state.nonEssential.fw->terminate(__LINE__);
#define CRCX_ERROR(s) printf s ;

typedef struct CRCX_NonEssentialState_t {
    Personality_t*          personality;
    Firmware_Interface_t*   fw;
    MUSPI_GIBarrier_t       giBarrier;
} CRCX_NonEssentialState_t;


typedef struct CRCX_Data_t {
    uint64_t se_crc;
    uint64_t re_crc;
    uint64_t direction;
    uint64_t state;
} CRCX_Data_t;


typedef struct CRCX_EssentialState_t {

    // Injection FIFO data:

    MUSPI_InjFifoSubGroup_t     injection_fifo_subgroup;
    Kernel_InjFifoAttributes_t  injection_fifo_attributes;
    Kernel_MemoryRegion_t       injection_fifo_memory_region;

    ALIGN_L1_CACHE char         injection_fifo[16*BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES];

    // Reception FIFO data

    int32_t registrationId;
    Kernel_MemoryRegion_t       reception_fifo_memory_region;
    Kernel_RecFifoAttributes_t  reception_fifo_attributes;
    MUSPI_RecFifoSubGroup_t     reception_fifo_subgroup;
    ALIGN_L1_CACHE char         reception_fifo[16*64];

    ALIGN_L1_CACHE CRCX_Data_t    crc_data[10];

    MUHWI_Destination_t         my_location;

    MUHWI_Descriptor_t          mem_fifo_descriptor  __attribute__((__aligned__(64))) ;
} CRCX_EssentialState_t;

typedef struct CRCX_State_t {
    CRCX_NonEssentialState_t nonEssential;
    CRCX_EssentialState_t essential;
} CRCX_State_t;

CRCX_State_t state;





int crcx_initialize( void ) {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__) );

    int rc = 0;

    state.nonEssential.personality = fwext_getPersonality();
    state.nonEssential.fw = fwext_getFwInterface();

    if ( ( rc = MUSPI_GIBarrierInit( &(state.nonEssential.giBarrier), 15 ) ) != 0 ) {
	CRCX_ERROR(("(E) [%s:%d] bad rc=%d from MUSPI_GIBarrierInit\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    Personality_Networks_t* nc = &(state.nonEssential.personality->Network_Config);
    fw_uint64_t nf  = nc->NetFlags;
    fw_uint64_t nf2 = nc->NetFlags2;    

    CRCX_DEBUG(("(*) TORUS COORDS (%d,%d,%d,%d,%d)/(%d,%d,%d,%d,%d)\n", nc->Acoord, nc->Bcoord, nc->Ccoord, nc->Dcoord, nc->Ecoord,
	   nc->Anodes, nc->Bnodes, nc->Cnodes, nc->Dnodes, nc->Enodes));

    CRCX_DEBUG(("(*) A(%d,%d):%lld B(%d,%d):%lld C(%d,%d):%lld D(%d,%d):%lld E(%d,%d):%lld\n",
	   ND_GET_LINK_ENABLE(0,0,nf2)?1:0, ND_GET_LINK_ENABLE(0,1,nf2)?1:0, ND_GET_LOOPBACK(0,nf),
	   ND_GET_LINK_ENABLE(1,0,nf2)?1:0, ND_GET_LINK_ENABLE(1,1,nf2)?1:0, ND_GET_LOOPBACK(1,nf),
	   ND_GET_LINK_ENABLE(2,0,nf2)?1:0, ND_GET_LINK_ENABLE(2,1,nf2)?1:0, ND_GET_LOOPBACK(2,nf),
	   ND_GET_LINK_ENABLE(3,0,nf2)?1:0, ND_GET_LINK_ENABLE(3,1,nf2)?1:0, ND_GET_LOOPBACK(3,nf),
	   ND_GET_LINK_ENABLE(4,0,nf2)?1:0, ND_GET_LINK_ENABLE(4,1,nf2)?1:0, ND_GET_LOOPBACK(4,nf)
	      ));


    CRCX_DEBUG( ( "(<) %s:%d rc=%d\n", __func__, __LINE__, rc) );

    return rc;
}

void crcx_barrier( int lineNo ) {

    CRCX_DEBUG( ( "(>) %s:%d lineNo=%d\n", __func__, __LINE__, lineNo) );

#if 0
    if ( state.nonEssential.fw->barrier( 1600ull*1000ull ) != FW_OK ) {
	printf("(E) barrier failure at line %d.\n", lineNo );
	CRCX_TERMINATE();
    }
#endif

#if 1

    int rc = 0;

    if ( ( rc = MUSPI_GIBarrierEnter( &state.nonEssential.giBarrier ) ) != 0 ) {
	CRCX_TERMINATE();
    }

    if ( ( rc = MUSPI_GIBarrierPollWithTimeout( &state.nonEssential.giBarrier, 1600ull * 1000ull * 1000ull * 30ull ) ) != 0 ) {
	CRCX_TERMINATE();
    }
    
#endif

    CRCX_DEBUG( ( "(<) %s:%d lineNo=%d\n", __func__, __LINE__, lineNo) );
}



int crcx_initialize_injection_fifo( void ) {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );

    int rc;

    CRCX_EssentialState_t* s = &state.essential;

    uint32_t injection_fifo_id = CRCX_INJ_FIFO_ID;

    s->injection_fifo_attributes.RemoteGet = 0;
    s->injection_fifo_attributes.System    = 1;

    rc = Kernel_AllocateInjFifos( CRCX_INJ_FIFO_SUBGROUP, &(s->injection_fifo_subgroup), 1, &injection_fifo_id, &(s->injection_fifo_attributes) );
    if ( rc != 0 )  {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_AllocateInjFifos\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    rc = Kernel_CreateMemoryRegion( &(s->injection_fifo_memory_region), s->injection_fifo, sizeof(s->injection_fifo) );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_CreateMemoryRegion\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    rc = Kernel_InjFifoInit( &(s->injection_fifo_subgroup), injection_fifo_id, &(s->injection_fifo_memory_region), 0, s->injection_fifo_memory_region.Bytes-1 );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_InjFifoInit\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }


    rc = Kernel_InjFifoActivate( &(s->injection_fifo_subgroup), 1, &injection_fifo_id, KERNEL_INJ_FIFO_ACTIVATE );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_InjFifoActivate\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }


    CRCX_DEBUG( ( "(<) %s:%d\n", __func__, __LINE__ ) );
    return 0;
}

int crcx_initialize_reception_fifo( void ) {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );    

    int rc = 0;
    CRCX_EssentialState_t* s = &state.essential;
    Personality_t*   p = state.nonEssential.personality;

    //s->registrationId = MUSPI_RegisterReceiveFunction( receive_packet, NULL );
    //if ( s->registrationId < 0 ) CRCX_TERMINATE();

    rc = Kernel_CreateMemoryRegion( &s->reception_fifo_memory_region, s->reception_fifo, sizeof(s->reception_fifo) );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_CreateMemoryRegion\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    
    s->reception_fifo_attributes.System = 1;

    uint32_t reception_fifo_id = CRCX_REC_FIFO_ID;

    rc = Kernel_AllocateRecFifos( CRCX_REC_FIFO_SUBGROUP, &(s->reception_fifo_subgroup), 1, &reception_fifo_id, &s->reception_fifo_attributes );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_AllocateRecFifos\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }


    rc = Kernel_RecFifoInit( &s->reception_fifo_subgroup, reception_fifo_id, &s->reception_fifo_memory_region, 0, sizeof(s->reception_fifo)-1 );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_RecFifoInit\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    uint64_t enables = 1ull << ( 16 - CRCX_REC_FIFO_GLOBAL_ID%16 - 1 );
    rc = Kernel_RecFifoEnable( CRCX_REC_FIFO_SUBGROUP / BGQ_MU_NUM_FIFO_SUBGROUPS, enables );
//1ull /* << ( BGQ_MU_NUM_REC_FIFOS_PER_GROUP - 1 - ( CRCX_REC_FIFO_SUBGROUP % BGQ_MU_NUM_REC_FIFOS_PER_GROUP ) ) */ );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_RecFifoEnable\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }
    

    s->my_location.Destination.A_Destination = p->Network_Config.Acoord;
    s->my_location.Destination.B_Destination = p->Network_Config.Bcoord;
    s->my_location.Destination.C_Destination = p->Network_Config.Ccoord;
    s->my_location.Destination.D_Destination = p->Network_Config.Dcoord;
    s->my_location.Destination.E_Destination = p->Network_Config.Ecoord;

    CRCX_DEBUG( ( "(<) %s:%d\n", __func__, __LINE__ ) );

    return rc;
}

int crcx_teardown_reception_fifo( void ) {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );    

    int rc = 0;
    CRCX_EssentialState_t* s = &state.essential;

    uint64_t disables = 1ull << ( 16 - (CRCX_REC_FIFO_GLOBAL_ID % 16) - 1 );
    rc = Kernel_RecFifoDisable( CRCX_REC_FIFO_SUBGROUP / BGQ_MU_NUM_FIFO_SUBGROUPS, disables );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_RecFifoDisable\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    uint32_t reception_fifo_id = CRCX_REC_FIFO_ID;

    rc = Kernel_DeallocateRecFifos( &(s->reception_fifo_subgroup), 1, &reception_fifo_id );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_DeallocateRecFifos\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    CRCX_DEBUG( ( "(<) %s:%d\n", __func__, __LINE__ ) );

    return rc;
}


int crcx_teardown_injection_fifo( void ) {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );

    int rc;

    CRCX_EssentialState_t* s = &state.essential;

    uint32_t injection_fifo_id = CRCX_INJ_FIFO_ID;

    rc = Kernel_InjFifoActivate( &(s->injection_fifo_subgroup), 1, &injection_fifo_id, KERNEL_INJ_FIFO_DEACTIVATE );
    if ( rc != 0 ) {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_InjFifoActivate\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    rc = Kernel_DeallocateInjFifos( &(s->injection_fifo_subgroup), 1, &injection_fifo_id );
    if ( rc != 0 )  {
	CRCX_ERROR(("[%s:%d] bad rc=%d from Kernel_AllocateInjFifos\n", __func__, __LINE__, rc ));
	return - __LINE__;
    }

    CRCX_DEBUG( ( "(<) %s:%d\n", __func__, __LINE__ ) );
    return 0;
}



void crcx_set_destination( MUHWI_Destination_t* dest, MUHWI_Destination_t* src, int link, Personality_Networks_t* net  ) {

    *dest = *src; // copy

    switch ( link ) {

		case 0 : /* A- */ dest->Destination.A_Destination = ( net->Acoord + net->Anodes - 1 ) % net->Anodes; break;
		case 1 : /* A+ */ dest->Destination.A_Destination = ( net->Acoord + 1               ) % net->Anodes; break;

		case 2 : /* B- */ dest->Destination.B_Destination = ( net->Bcoord + net->Bnodes - 1 ) % net->Bnodes; break;
		case 3 : /* B+ */ dest->Destination.B_Destination = ( net->Bcoord + 1               ) % net->Bnodes; break;

		case 4 : /* C- */ dest->Destination.C_Destination = ( net->Ccoord + net->Cnodes - 1 ) % net->Cnodes; break;
		case 5 : /* C+ */ dest->Destination.C_Destination = ( net->Ccoord + 1               ) % net->Cnodes; break;

		case 6 : /* D- */ dest->Destination.D_Destination = ( net->Dcoord + net->Dnodes - 1 ) % net->Dnodes; break;
		case 7 : /* D+ */ dest->Destination.D_Destination = ( net->Dcoord + 1               ) % net->Dnodes; break;

		case 8 : /* E- */ dest->Destination.E_Destination = ( net->Ecoord + net->Enodes - 1 ) % net->Enodes; break;
		case 9 : /* E+ */ dest->Destination.E_Destination = ( net->Ecoord + 1               ) % net->Enodes; break;

		}
}

int crcx_send_packets( void ) {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );    
    
    int i;
    int rc = 0;

    Personality_t* p = state.nonEssential.personality;
    CRCX_EssentialState_t* s = &state.essential;    
    Personality_Networks_t* netcfg = &(p->Network_Config);

#if 0
    if ( (netcfg->Acoord > 0) || (netcfg->Bcoord > 0) || (netcfg->Ccoord > 0) || (netcfg->Dcoord > 0) || (netcfg->Ecoord > 0) ) {
	printf("(W) Skipping for now!!!!!!!!!!!!!!!!!!!!!\n");
	return 0;
    }
#endif

    // Capture all CRCs first
    for ( i = 0; i < 10; i++ ) {

	s->crc_data[i].state = CRCX_NOT_DEFINED;  // assumption

	if ( ND_GET_LOOPBACK( i/2, netcfg->NetFlags ) == ND_LOOPBACK_NONE ) {

	    if ( ND_GET_LINK_ENABLE( i/2, i%2, netcfg->NetFlags2 ) != 0 ) {
		s->crc_data[i].se_crc = DCRReadPriv( ND_RESE_DCR(i,SE_LINK_CRC_USR) );
		s->crc_data[i].re_crc = DCRReadPriv( ND_RESE_DCR(i,RE_LINK_CRC_USR) );
		s->crc_data[i].state  = CRCX_SENT;
		s->crc_data[i].direction = i;
	    }
	}
    }

    for ( i = 0; i < 10; i++ ) {

	if ( ND_GET_LOOPBACK( i/2, netcfg->NetFlags ) == ND_LOOPBACK_NONE ) {

	    if ( ND_GET_LINK_ENABLE( i/2, i%2, netcfg->NetFlags2 ) != 0 ) {

		CRCX_DEBUG( ( "(*) Sending packet in direction %d\n", i ) );

		MUSPI_Pt2PtMemoryFIFODescriptorInfo_t p2p_descriptor;
		memset(&p2p_descriptor, 0x00, sizeof(p2p_descriptor));
      
		p2p_descriptor.Base.Payload_Address  = (uint64_t)&(s->crc_data[i]);
		p2p_descriptor.Base.Message_Length   = sizeof(CRCX_Data_t);
		p2p_descriptor.Base.Torus_FIFO_Map   = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_SYSTEM;

		crcx_set_destination( &p2p_descriptor.Base.Dest, &s->my_location, i, netcfg );


		p2p_descriptor.Pt2Pt.Misc1 = 
		    MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE |
		    MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
		    MUHWI_PACKET_DO_NOT_DEPOSIT
		    ;
		
		p2p_descriptor.Pt2Pt.Misc2 =
		    MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM
		    ;

		p2p_descriptor.MemFIFO.Rec_FIFO_Id =  CRCX_REC_FIFO_GLOBAL_ID;
		p2p_descriptor.MemFIFO.Interrupt = MUHWI_DESCRIPTOR_DO_NOT_INTERRUPT_ON_PACKET_ARRIVAL;
		//p2p_descriptor.MemFIFO.SoftwareBytes[0] = s->registrationId;
		
				    

		rc = MUSPI_CreatePt2PtMemoryFIFODescriptor( &s->mem_fifo_descriptor,  &p2p_descriptor );
		if ( rc != 0 ) {
		    CRCX_ERROR(("[%s:%d] bad rc=%d from MUSPI_CreatePt2PtMemoryFIFODescriptor\n", __func__, __LINE__, rc ));
		    return - __LINE__;
		}
		

		rc = MUSPI_InjFifoInject( MUSPI_IdToInjFifo( CRCX_INJ_FIFO_ID, &(s->injection_fifo_subgroup) ), &(s->mem_fifo_descriptor) );
		if ( rc <= 0 ) {
		    CRCX_ERROR(("[%s:%d] bad rc=%d from MUSPI_InjFifoInject\n", __func__, __LINE__, rc ));
		    return - __LINE__;
		}
	    }
	}
    }

    CRCX_DEBUG( ( "(<) %s:%d\n", __func__, __LINE__ ) );    
    
    return 0;
}

int crcx_receive_packet( void* recv_func_parm, MUHWI_PacketHeader_t* packet, uint32_t packet_length ) {

    int rc = 0;

    CRCX_DEBUG(("(*) Hello from %s parm=%p packet=%p len=%d sizeof(hdr)=%ld \n", __func__, recv_func_parm, packet, packet_length, sizeof(MUHWI_PacketHeader_t)));
    CRCX_DEBUG(("(*) hdr = { pkt-type=%02X ... MU.HDR.Rec_FIFO=%d offset.lsb=%d}\n", 
	   packet->NetworkHeader.pt2pt.Data_Packet_Type,
	   packet->messageUnitHeader.Packet_Types.Memory_FIFO.Rec_FIFO_Id,
	   packet->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB
	      ));


    CRCX_Data_t* crc = (CRCX_Data_t*)((char*)packet + sizeof(MUHWI_PacketHeader_t));
    CRCX_DEBUG(("(*) packet se.crc=%016lX re.crc=%016lX direction=%ld state=%016lX\n", crc->se_crc, crc->re_crc, crc->direction, crc->state));

    CRCX_EssentialState_t* s = &state.essential;
    int link = ( (crc->direction / 2) * 2 ) + ( ( crc->direction & 1 ) ? 0 : 1 );

    
    CRCX_DEBUG(("(*) compare link=%d this.se_crc = %016lX vs %016lX that.re_crc link=%ld\n", link, s->crc_data[link].se_crc, crc->re_crc, crc->direction ));

    if ( s->crc_data[link].state != CRCX_SENT ) {
	s->crc_data[link].state = CRCX_FAILED;
	CRCX_ERROR(("(E) illegal state link=%d state=%016lX\n", link, s->crc_data[link].state));
	rc--;
    }

    if ( s->crc_data[link].se_crc != crc->re_crc ) {
	s->crc_data[link].state = CRCX_FAILED;
	CRCX_ERROR(("(E) miscompare link=%d this.se_crc = %016lX vs %016lX that.re_crc\n", link, s->crc_data[link].se_crc, crc->re_crc ));
	rc--;
    }

    CRCX_DEBUG(("(*) compare link=%d this.re_crc = %016lX vs %016lX that.se_crc link=%ld\n", link, s->crc_data[link].se_crc, crc->re_crc, crc->direction ));

    if ( s->crc_data[link].re_crc != crc->se_crc ) {
	s->crc_data[link].state = CRCX_FAILED;
	CRCX_ERROR(("(E) miscompare link=%d this.re_crc = %016lX vs %016lX that.se_crc\n", link, s->crc_data[link].se_crc, crc->re_crc ));
	rc--;
    }

    s->crc_data[link].state = CRCX_VALIDATED;
    
    

#if 0
    uint64_t* rfifod = (uint64_t*)state.essential.reception_fifo;

    int i;

    for ( i = 0; i < 64; i+= 4 ) {
	CRCX_DEBUG(("(*) RFIFO[%2d] : %p : %016lX %016lX %016lX %016lX\n", i, rfifod + i, rfifod[i+0],  rfifod[i+1],   rfifod[i+2],  rfifod[i+3]));
    }
#endif

    return rc;
}

int crcx_receive_packets() {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );    

    CRCX_EssentialState_t* s = &state.essential;

    int i;
    int done = 0;
    int rc = 0;

    uint64_t timeout = GetTimeBase() + (uint64_t)(state.nonEssential.personality->Kernel_Config.FreqMHz) * 1000ull * 1000ull * 60ull;
    

    while ( done == 0 ) {

	//printf("Polling %p\n", MUSPI_IdToRecFifo( CRCX_REC_FIFO_ID, &(s->reception_fifo_subgroup) ) );

	//int rc = MUSPI_RecFifoPoll( MUSPI_IdToRecFifo( CRCX_REC_FIFO_ID, &(s->reception_fifo_subgroup) ), 100 );

	//printf("Poll: rc=%d\n", rc);

	unsigned wrap = 0;
	MUSPI_RecFifo_t* rfifo = MUSPI_IdToRecFifo( CRCX_REC_FIFO_ID, &(s->reception_fifo_subgroup) );


	unsigned total_bytes = MUSPI_getAvailableBytes( rfifo, &wrap );

	
	CRCX_DEBUG(("(*) Packets are here : bytes=%d  wrap=%d\n", total_bytes, wrap ));

	while ( total_bytes > 0 ) {
	    uint32_t cur_bytes = 0;
	    MUHWI_PacketHeader_t* hdr = MUSPI_getNextPacketOptimized(rfifo, &cur_bytes);
	    printf("(*) Fetched next packet cur_bytes=%d\n", cur_bytes);
	    rc += crcx_receive_packet( 0, hdr, cur_bytes );
	    total_bytes -= cur_bytes;
	}

	done = 1; // assumption

	int packetsAccountedFor = 0;

	for ( i = 0; i < 10; i++ ) {
	    switch ( s->crc_data[i].state ) {
	    case CRCX_VALIDATED :
	    case CRCX_NOT_DEFINED :
	    case CRCX_FAILED :
		packetsAccountedFor++;
		break;
	    }
	}

	if ( packetsAccountedFor == 10 ) {
	    done = 1;
	}
	else if ( GetTimeBase() > timeout ) {
	    CRCX_DEBUG(("(*) Timed out in CRC exchange.\n"));
	    rc--;
	    done = 1;
	}
    }

    CRCX_DEBUG( ( "(<) %s:%d rc=%d\n", __func__, __LINE__, rc ) );    
    return rc;
}


int fwext_crc_exchange() {

#if 0
	extern void msg_ran_enable_int_handler();
	//extern void   fw_mu_set_sys_range( int,  uint64_t, uint64_t);

	msg_ran_enable_int_handler();
	fw_mu_set_sys_range(0, 0, ~0);

#endif

	int rc = 0;

	if ( ( rc = crcx_initialize() ) != 0 ) {
	    return rc;
	}

	crcx_barrier(__LINE__);
	
	if ( ( rc = crcx_initialize_injection_fifo() ) != 0 ) {
	    return rc;
	}

	if ( ( rc = crcx_initialize_reception_fifo() ) != 0 ) {
	    return rc;
	}

	crcx_barrier(__LINE__);

	if ( ( rc = crcx_send_packets() ) != 0 ) {
	    return rc;
	}

	if ( ( rc = crcx_receive_packets() ) != 0 ) {
	    return rc;
	}


	if ( ( rc = crcx_teardown_reception_fifo() ) != 0 ) {
	    return rc;
	}

	if ( ( rc = crcx_teardown_injection_fifo() ) != 0 ) {
	    return rc;
	}

	return 0;
}
