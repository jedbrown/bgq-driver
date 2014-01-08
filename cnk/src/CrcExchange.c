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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#include "Kernel.h"
#include "NodeState.h"
#include "util.h"


extern NodeState_t NodeState;


#define CRCX_INJ_FIFO_SUBGROUP  67
#define CRCX_INJ_FIFO_ID        7

#define CRCX_REC_FIFO_SUBGROUP  67
#define CRCX_REC_FIFO_GROUP     (CRC_REC_FIFO_SUBGROUP/BGQ_MU_NUM_FIFO_SUBGROUPS)
#define CRCX_REC_FIFO_ID        1
#define CRCX_REC_FIFO_GLOBAL_ID ( (CRCX_REC_FIFO_SUBGROUP*4) + CRCX_REC_FIFO_ID )

#define CRCX_NOT_APPLICABLE 0
#define CRCX_PENDING        1
#define CRCX_VALIDATED      2
#define CRCX_FAILED         3


//#define CRCX_DEBUG(s) printf s ;
#define CRCX_DEBUG(s)
#define CRCX_ERROR(s) printf s ;

// wait this many seconds while waiting for user traffic to stop
const uint64_t crx_quiese_timout_secs = 30;


int crcx_barrier( MUSPI_GIBarrier_t* barrier, int barrierId ) {

    // @todo : Explore usage of system all-reduce here instead of MU barrier, since
    //         MU barriers affect user CRCs.

    CRCX_DEBUG( ( "(>) %s:%d barrier=%p id=%d\n", __func__, __LINE__, barrier, barrierId) );

    int rc = 0;

    if ( ( rc = MUSPI_GIBarrierEnter( barrier ) ) != 0 ) {
	return - __LINE__;
    }

    if ( ( rc = MUSPI_GIBarrierPollWithTimeout( barrier, -1 /*1600ull * 1000ull * 1000ull * 60ull*/ ) ) != 0 ) { // @todo fix magic 
	return - __LINE__;
    }

    CRCX_DEBUG( ( "(<) %s:%d barrierId=%d\n", __func__, __LINE__, barrierId) );

    return 0;
}



int crcx_initialize_injection_fifo( void ) {

    /* ----------------------------------------------------------------------
     * Initialize the injection FIFO used for CRC exchange.  This entails:
     *
     *   - allocating the FIFO
     *   - creating the associatied memory region
     *   - initializing the FIFO
     *   - activating the FIFO
     * ----------------------------------------------------------------------- */


    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );

    int rc;

    CrcExchange_Data_t* s = &NodeState.crcExchange;

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

    /* -----------------------------------------------------------
     * Initialize the reception FIFO used for CRC exchange.  This
     * entails:
     *
     *   - creating the memory region.
     *   - allocating the reception FIFO.
     *   - initializing the reception FIFO.
     *   - enabling the reception FIFO
     *
     * ------------------------------------------------------------ */

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );    

    int rc = 0;
    CrcExchange_Data_t* s = &NodeState.crcExchange;
    Personality_t*   p = &NodeState.Personality;

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

    uint64_t enables = 1ull << ( 16 - CRCX_REC_FIFO_GLOBAL_ID % 16 - 1 );
    rc = Kernel_RecFifoEnable( CRCX_REC_FIFO_SUBGROUP / BGQ_MU_NUM_FIFO_SUBGROUPS, enables );

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



int crcx_initialize( void ) {

    int rc = 0;

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__) );

    if ( NodeState.crcExchange.initialized == 0 ) {

       	if ( ( rc = crcx_initialize_injection_fifo() ) != 0 ) {
	    CRCX_ERROR(("(E) [%s:%d] bad rc=%d from crcx_initialize_injection_fifo\n", __func__, __LINE__, rc ));
	    return - __LINE__;
	}

	if ( ( rc = crcx_initialize_reception_fifo() ) != 0 ) {
	    CRCX_ERROR(("(E) [%s:%d] bad rc=%d from crcx_initialize_reception_fifo\n", __func__, __LINE__, rc ));
	    return - __LINE__;
	}

	NodeState.crcExchange.initialized = 1;
    }

    CRCX_DEBUG( ( "(<) %s:%d rc=%d\n", __func__, __LINE__, rc) );

    return rc;
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

int crcx_send_packets( MUSPI_GIBarrier_t* barrier ) {

    CRCX_DEBUG( ( "(>) %s:%d barrier=%p\n", __func__, __LINE__, barrier ) );    
    
    int i;
    int rc = 0;

    Personality_t* p = &NodeState.Personality;
    CrcExchange_Data_t* s = &NodeState.crcExchange;
    Personality_Networks_t* netcfg = &(p->Network_Config);

    if ( ( rc = crcx_barrier( barrier, __LINE__ ) ) != 0 ) {
	CRCX_ERROR(("(E) CRC barrier error rc=%d\n", rc ));
	return - __LINE__;
    }

#if 1
    {
	uint64_t timeout = GetTimeBase() + p->Kernel_Config.FreqMHz * 1000ull * 10ull;
	while ( GetTimeBase() < timeout );
    }
#endif

    // Capture all CRCs first

    for ( i = 0; i < 10; i++ ) {

	s->crc_data[i].state = CRCX_NOT_APPLICABLE;  // assumption

	if ( ND_GET_LOOPBACK( i/2, netcfg->NetFlags ) == ND_LOOPBACK_NONE ) {

	    if ( ND_GET_LINK_ENABLE( i/2, i%2, netcfg->NetFlags2 ) != 0 ) {
		s->crc_data[i].se_crc = DCRReadPriv( ND_RESE_DCR(i,SE_LINK_CRC_USR) );
		s->crc_data[i].re_crc = DCRReadPriv( ND_RESE_DCR(i,RE_LINK_CRC_USR) );
		s->crc_data[i].state  = CRCX_PENDING;
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
		p2p_descriptor.Base.Message_Length   = sizeof(CrcExchange_Packet_t);
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
    CrcExchange_Data_t* s = &NodeState.crcExchange;
    CrcExchange_Packet_t* crc = (CrcExchange_Packet_t*)((char*)packet + sizeof(MUHWI_PacketHeader_t));

    s->packets_received++;

    CRCX_DEBUG(("(>) %s parm=%p packet=%p / %p / %p len=%d\n", __func__, recv_func_parm, packet, s->reception_fifo, s->reception_fifo + sizeof(s->reception_fifo), packet_length ));
    CRCX_DEBUG(("(*) packet se.crc=%016lX re.crc=%016lX direction=%ld state=%016lX\n", crc->se_crc, crc->re_crc, crc->direction, crc->state));

    if ( ( crc->state != CRCX_PENDING ) || (crc->direction >= 10) ) {
	CRCX_ERROR(("(E) invalid packet: { se_crc=%016lX re_crc=%016lX direction=%ld state=%016lX \n", crc->se_crc, crc->re_crc, crc->direction, crc->state));
	rc--;
    }



    int link = ( (crc->direction / 2) * 2 ) + ( ( crc->direction & 1 ) ? 0 : 1 );
    
    CRCX_DEBUG(("(*) compare link=%d this.se_crc = %016lX vs %016lX that.re_crc link=%ld\n", link, s->crc_data[link].se_crc, crc->re_crc, crc->direction ));
    CRCX_DEBUG(("(*) compare link=%d this.re_crc = %016lX vs %016lX that.se_crc link=%ld\n", link, s->crc_data[link].se_crc, crc->re_crc, crc->direction ));


    if ( s->crc_data[link].state != CRCX_PENDING ) {
	s->crc_data[link].state = CRCX_FAILED;
	CRCX_ERROR(("(E) illegal state link=%d state=%016lX\n", link, s->crc_data[link].state));
	rc--;
    }

#ifdef CRCX_SIMULATE_ERROR

    // This code can be used to simulate a CRC miscompare.  It is activatated by and environment variable
    // and forces the error on the E link of node (0,0,0,0,1).

    uint32_t simulate_error = 0;
    Personality_t*   p = &NodeState.Personality;
    App_GetEnvValue("BG_SIMULATE_CRC_EXCHANGE_ERROR", &simulate_error);

    if ( simulate_error ) {
	if ((p->Network_Config.Acoord==0) && (p->Network_Config.Bcoord==0) && (p->Network_Config.Ccoord==0) && (p->Network_Config.Dcoord==0) && (p->Network_Config.Ecoord==1)) 
	{
	    if ( ( link == 8 ) || ( link == 9 ) ) {
		s->crc_data[link].se_crc++;
	    }
	}
    }

#endif

    if ( s->crc_data[link].se_crc != crc->re_crc ) {
	s->crc_data[link].state = CRCX_FAILED;
	CRCX_ERROR(("(E) miscompare link=%d this.se_crc = %016lX vs %016lX that.re_crc [%d]\n", link, s->crc_data[link].se_crc, crc->re_crc, __LINE__ ));
	RASBEGIN(3);
	RASPUSH(link);
	RASPUSH(s->crc_data[link].se_crc);
	RASPUSH(crc->re_crc);
	RASFINAL(RAS_CRC_EXCHANGE_ERROR);
	rc--;
    }

    if ( s->crc_data[link].re_crc != crc->se_crc ) {
	s->crc_data[link].state = CRCX_FAILED;
	CRCX_ERROR(("(E) miscompare link=%d this.re_crc = %016lX vs %016lX that.se_crc [%d]\n", link, s->crc_data[link].re_crc, crc->se_crc, __LINE__ ));
	RASBEGIN(3);
	RASPUSH(link);
	RASPUSH(s->crc_data[link].re_crc);
	RASPUSH(crc->se_crc);
	RASFINAL(RAS_CRC_EXCHANGE_ERROR);
	rc--;
    }

    if ( rc == 0 ) {
	s->crc_data[link].state = CRCX_VALIDATED;
    }
    
    return rc;
}

int crcx_receive_packets() {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );    

    CrcExchange_Data_t* s = &NodeState.crcExchange;

    int i;
    int done = 0;
    int rc = 0;

    uint64_t timeout = GetTimeBase() + (uint64_t)(NodeState.Personality.Kernel_Config.FreqMHz) * 1000ull * 1000ull * 60ull; //@ todo magic
    MUSPI_RecFifo_t* receptionFIFO = MUSPI_IdToRecFifo( CRCX_REC_FIFO_ID, &(s->reception_fifo_subgroup) );    

    while ( done == 0 ) {

	unsigned wrap = 0;
	unsigned totalBytes = MUSPI_getAvailableBytes( receptionFIFO, &wrap );
	
	CRCX_DEBUG(("(*) Packets are here : bytes=%d  wrap=%d\n", totalBytes, wrap ));

	if ( wrap == 0 ) {
	    while ( totalBytes > 0 ) {
		uint32_t curBytes = 0;
		MUHWI_PacketHeader_t* hdr = MUSPI_getNextPacketOptimized(receptionFIFO, &curBytes);
		CRCX_DEBUG(("(*) Fetched next packet curBytes=%d\n", curBytes));
		rc += crcx_receive_packet( 0, hdr, curBytes );
		totalBytes -= curBytes;
	    }
	}
	else {
	    uint32_t curBytes = 0;
	    MUHWI_PacketHeader_t* hdr = MUSPI_getNextPacketWrap(receptionFIFO, &curBytes);
	    CRCX_DEBUG(("(*) Fetched next packet (wrap) curBytes=%d\n", curBytes));
	    rc += crcx_receive_packet( 0, hdr, curBytes );
	}


	MUSPI_syncRecFifoHwHead( receptionFIFO );

	done = 1; // assumption

	for ( i = 0; i < 10; i++ ) {
	    if ( s->crc_data[i].state == CRCX_PENDING ) {
		done = 0;
	    }
	}

	if ( ( ! done ) && (  GetTimeBase() > timeout ) ) {
	    CRCX_ERROR(("(E) Timed out in CRC exchange.\n"));
	    rc--;
	    done = 1;
	}
    }

    CRCX_DEBUG( ( "(<) %s:%d rc=%d\n", __func__, __LINE__, rc ) );    
    return rc;
}


#ifdef CRCX_TEARDOWN_EVERY_TIME

// NOTE:  this routine is currently not being used

int crcx_teardown_reception_fifo( void ) {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );    

    int rc = 0;
    CrcExchange_Data_t* s = &NodeState.crcExchange;

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

#endif

#ifdef CRCX_TEARDOWN_EVERY_TIME

// NOTE: this route is not currently being used 

int crcx_teardown_injection_fifo( void ) {

    CRCX_DEBUG( ( "(>) %s:%d\n", __func__, __LINE__ ) );

    int rc;

    CrcExchange_Data_t* s = &NodeState.crcExchange;

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

#endif

#if 0

static const char* CRCX_STATES[] = { 
    "N/A      ", 
    "PENDING  ", 
    "VALIDATED", 
    "FAILED   " };

void crcx_dump() {

    Personality_t* p = &NodeState.Personality;
    CrcExchange_Data_t* s = &NodeState.crcExchange;
    Personality_Networks_t* netcfg = &(p->Network_Config);
    int i;

    printf("(P) packets_received = %ld\n", s->packets_received);
    for ( i = 0; i < 10; i++ ) {

	printf(
	    "(P) [%d] %016lX %016lX %s %ld [ %lld : %d ]\n",
	    i,
	    s->crc_data[i].se_crc,
	    s->crc_data[i].re_crc,
	    CRCX_STATES[s->crc_data[i].state],
	    s->crc_data[i].direction,
	    ND_GET_LOOPBACK( i/2, netcfg->NetFlags ),
	    ND_GET_LINK_ENABLE( i/2, i%2, netcfg->NetFlags2 ) ? 1 : 0
	    );

    }
}

#endif

int  crcx_quiesce_user_traffic( ) 
{
  //  wait until all user traffic has completed
  int i;

  Personality_t* p = &NodeState.Personality;
  //  CrcExchange_Data_t* s = &NodeState.crcExchange;
  Personality_Networks_t* netcfg = &(p->Network_Config);
  
  uint64_t t_trace   =  GetTimeBase() + crx_quiese_timout_secs *  p->Kernel_Config.FreqMHz * 1000000ull / 2;
  uint64_t t_timeout =  GetTimeBase() + crx_quiese_timout_secs *  p->Kernel_Config.FreqMHz * 1000000ull ;
  uint64_t expected_tokens[10];
  uint64_t token_mask[10];
  int vc4_user[10];
  
  // for each link, figure out the expected tokens
  for ( i = 0; i < 10; i++ ) { 
     expected_tokens[i] = 0;
     Kernel_GetNDExpectedTokensDCR(i,&expected_tokens[i]); // it's ok if link is in reset
     token_mask[i] = ~0ull;
     // mask out the system pt-pt
     ND_RESE_DCR__SE_TOKENS__VC_SYSTEM_insert( token_mask[i],0); // ignore system pt-pt
     ND_RESE_DCR__SE_TOKENS__VC_SYSTEM_insert( expected_tokens[i],0); // ignore system pt-pt
     // PAMI always uses vc 5, so vc4_user[i] should be 0 or 2  in PAMI runs
     vc4_user[i] =  Kernel_NDWhoUsesVC4(i);  // is 1 if user commworld
     CRCX_DEBUG((" i= %4d vc_user=%4d\n",i,vc4_user[i]));
     
     switch(vc4_user[i])
     {
     case 0: // unused
     case 1: // user commworld 
       break; // do nothing to the sender token mask
 
     case 2:  // system, mask this out
        ND_RESE_DCR__SE_TOKENS__VC_COLL0_insert(token_mask[i],0);
        ND_RESE_DCR__SE_TOKENS__VC_COLL0_insert(expected_tokens[i],0); 
	break;
     default:
       return - __LINE__; // error 
     }
     
  }
  
  
  int done=1;
  int done1,done2,done3,done4,done5,done6;
  uint64_t trace_oneshot = 0;
  done1=done2=done3=done4=done5=done6=1;
  while (  GetTimeBase() < t_timeout)
  {
    done=1;
    uint64_t tokens;
    uint64_t re_vc_state;
    
    done1=done2=done3=done4=done5=done6=1;

#define FLTRACE(id, d1, d2, d3, d4) if(trace_oneshot == 1) { Kernel_WriteFlightLog(FLIGHTLOG, id, d1, d2, d3, d4); }
    if(GetTimeBase() > t_trace)
    {
        trace_oneshot++;
    }
    // check all the links (could be optimized to only check the links that are not yet done )
    for ( i = 0; i < 10; i++ ) {

	if ( ND_GET_LOOPBACK( i/2, netcfg->NetFlags ) == ND_LOOPBACK_NONE ) {

	    if ( ND_GET_LINK_ENABLE( i/2, i%2, netcfg->NetFlags2 ) != 0 ) {
	      
	      //  read the sender tokens and check to expected
	      tokens =  DCRReadPriv( ND_RESE_DCR(i,SE_TOKENS) );
	      tokens &= token_mask[i];
              FLTRACE(FL_CRCEXCHD1, i, tokens, expected_tokens[i],0);
	      if ( tokens != expected_tokens[i]) done1 =0;  // multiple dones for debugging
	      
	      
	      // now check that there are no packets in any of the receiver vc queues
	      re_vc_state = DCRReadPriv( ND_RESE_DCR(i,RE_VC0_PKT_CNT) ); // should be 0
              FLTRACE(FL_CRCEXCHD2, i, re_vc_state,0,0);
	      if (  re_vc_state ) done2 = 0;

	      // the slot valids are non-zero if any packets are in the receiver
	      re_vc_state = DCRReadPriv( ND_RESE_DCR(i,RE_VC1_STATE) );
              FLTRACE(FL_CRCEXCHD3, i, re_vc_state,ND_RESE_DCR__RE_VC1_STATE__CTRL_SLOT_VALID_get(re_vc_state),0);
	      if ( ND_RESE_DCR__RE_VC1_STATE__CTRL_SLOT_VALID_get(re_vc_state)) done3 = 0;

	      re_vc_state = DCRReadPriv( ND_RESE_DCR(i,RE_VC2_STATE) );
              FLTRACE(FL_CRCEXCHD4, i, re_vc_state,ND_RESE_DCR__RE_VC2_STATE__CTRL_SLOT_VALID_get(re_vc_state),0);
	      if ( ND_RESE_DCR__RE_VC2_STATE__CTRL_SLOT_VALID_get(re_vc_state)) done4 = 0;

	      //  if  physical vc 4 is used for user commworld, must wait for this to be empty too
	      if ( vc4_user[i] == 1) 
	      {
		re_vc_state = DCRReadPriv( ND_RESE_DCR(i,RE_VC4_STATE) );
                FLTRACE(FL_CRCEXCHD5, i, re_vc_state,ND_RESE_DCR__RE_VC4_STATE__CTRL_SLOT_VALID_get(re_vc_state),0);
		if ( ND_RESE_DCR__RE_VC4_STATE__CTRL_SLOT_VALID_get(re_vc_state)) done5 = 0;
	      }
  
	      re_vc_state = DCRReadPriv( ND_RESE_DCR(i,RE_VC5_STATE) );
              FLTRACE(FL_CRCEXCHD6, i, re_vc_state, ND_RESE_DCR__RE_VC5_STATE__CTRL_SLOT_VALID_get(re_vc_state), 0);
	      if ( ND_RESE_DCR__RE_VC5_STATE__CTRL_SLOT_VALID_get(re_vc_state)) done6 = 0;

	    }
	}
	
    }
    
    
    if ( done1+done2+done3+done4+done5+done6 < 6) done=0;
    
    if ( done == 1) return 0;  
    
  
  }  // end of the  while (  GetTimeBase() < t_timeout)

  // if we are here, we timed  out
  CRCX_DEBUG(("done1=%d done2=%d done3=%d done4=%d done5=%d done6=%d\n",done1,done2,done3,done4,done5,done6));
  return - __LINE__;
}







int crcx_exchange_crcs( MUSPI_GIBarrier_t* barrier ) {

    CRCX_DEBUG( ( "(>) %s:%d barrier=%p\n", __func__, __LINE__, barrier ) );

    int rc = 0;

    uint64_t msr = mfmsr();
    mtmsr( msr | MSR_FP );

    CRCX_DEBUG(( "Enabling FP %016lX -> %016llX\n", msr, msr | MSR_FP ));


 
    // TODO add a barrier or allreduce, warning,  could be a timeout if there are greatly different
    // times between when nodes exit

    // concerns about premature exits,  eg,  soemone sends 1MB, but receiver posts receive for 1KB, could be
    // packets stuck in network

    if  ( ( rc = crcx_quiesce_user_traffic() ) != 0 ) {
	CRCX_ERROR(("(E) non-zero return code rc=%d from  crcx_quiesce_user_traffic \n", rc ));
	rc =  - __LINE__;
	goto crcx_done;
    }

    // TODO when not using GI barriers for synchronization, disable GI's, then re-enable them at end

   
    if  ( ( rc = crcx_send_packets(barrier) ) != 0 ) {
	CRCX_ERROR(("(E) non-zero return code rc=%d from crcx_send_packets\n", rc ));
	rc =  - __LINE__;
	goto crcx_done;
    }


    if ( ( rc = crcx_receive_packets() ) != 0 ) {
	CRCX_ERROR(("(E) non-zero return code rc=%d from crcx_receive_packets\n", rc ));
	rc = - __LINE__;
	goto crcx_done;
    }


#ifdef CRCX_TEARDOWN_EVERY_TIME

    if ( ( rc = crcx_teardown_reception_fifo() ) != 0 ) {
	CRCX_ERROR(("(E) non-zero return code rc=%d from crcx_teardown_reception_fifo\n", rc ));
	rc = - __LINE__;
	goto crcx_done;
    }

    if ( ( rc = crcx_teardown_injection_fifo() ) != 0 ) {
	CRCX_ERROR(("(E) non-zero return code rc=%d from crcx_teardown_injection_fifo\n", rc ));
	rc = - __LINE__;
	goto crcx_done;
    }
#endif

crcx_done:
    mtmsr(msr);

#if 0
    crcx_dump();
#endif

    CRCX_DEBUG( ( "(<) %s:%d rc=%d\n", __func__, __LINE__, rc) );
    return rc;
}
