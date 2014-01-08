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

#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/sd_rcb.h>

#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>

#include <stdio.h>

Personality_t* PERS = 0;

#define PRIMORDIAL_NODE() ( ( PERS->Network_Config.Acoord == 0 ) && ( PERS->Network_Config.Bcoord == 0 ) && ( PERS->Network_Config.Ccoord == 0 ) && ( PERS->Network_Config.Dcoord == 0 ) && ( PERS->Network_Config.Ecoord == 0 ) )

#define INFO(s) if ( PRIMORDIAL_NODE() ) printf s;
#define ABORT(s) { printf s; Terminate(__LINE__); }
#define DEBUG(s) /*printf s;*/
#define EVENT(s)  printf s; 
#define ERROR(s) printf s;


void barrier( void ) {

    const uint64_t BARRIER_TIMEOUT = 120ull * 1000ull * 1000ull; // 2 minutes

    if ( fwext_getFwInterface()->barrier( BARRIER_TIMEOUT ) != FW_OK ) {
	ABORT(( "(E) Barrier timeout."));
    }
}

// auto-calibration
void hss_calibrate_auto(int link, int dir, int lane)
{
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, 0x0100);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, 0x0102);
    while (rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36) != 0x0106) /*spin*/;
}

int is_torus_location(int a, int b, int c, int d, int e ) {

    return 
	( 
	    ( PERS->Network_Config.Acoord == a ) &&
	    ( PERS->Network_Config.Bcoord == b ) &&
	    ( PERS->Network_Config.Ccoord == c ) &&
	    ( PERS->Network_Config.Dcoord == d ) &&
	    ( PERS->Network_Config.Ecoord == e )
	    ) ? 1 : 0;
}

const char LINKS[] = { 'A', 'B', 'C', 'D', 'E', 'I' };
const char DIRS[] = { 'M', 'P' };

#define DIR(lane) ((lane)<4 ? DIRS[0] : DIRS[1])

typedef struct _PRBS_Data_t {
    uint64_t tx_mask;
    uint64_t reset_mask;
    uint64_t enable_mask;
    uint64_t sync_mask;
    uint64_t error_mask;
    uint32_t control_dcr;
    uint32_t status_dcr;
} PRBS_Data_t;

#define NUM_HSS_DIMENSIONS 6

PRBS_Data_t PRBS[NUM_HSS_DIMENSIONS] = { { 0, }, };

void prbs_initialize_controls( void ) {

    unsigned dimension, direction, link;
    
    unsigned prbs_type = 2; // @todo support different PRBS types (custom bit patterns)

    for ( link = 0, dimension = 0; dimension < NUM_HSS_DIMENSIONS; dimension++ ) { // iterate A,B,C,D,E,IO

	unsigned side = dimension / 3;
	unsigned offset = dimension % 3;

	PRBS[dimension].control_dcr = (side == 0) ? ( SERDES_LEFT_DCR(PRBS_CTL_A)  + offset ) : ( SERDES_RIGHT_DCR(PRBS_CTL_D)  + offset );
	PRBS[dimension].status_dcr  = (side == 0) ? ( SERDES_LEFT_DCR(PRBS_STAT_A) + offset ) : ( SERDES_RIGHT_DCR(PRBS_STAT_D) + offset );

	for ( direction = 0; direction < 2; direction++, link++ ) {

	    //if ( ( is_torus_location(0,1,1,0,0) || is_torus_location(0,1,2,0,0) ) && ( dimension == 2) ) {
	    if (1) {

		if ( PERS->Network_Config.NetFlags2 & (SD_ENABLE_T0>>link) ) {

		    int physicalDirection = direction ; //^ ( TI_GET_TORUS_DIM_REVERSED(dimension,PERS->Network_Config.NetFlags2) ? 1 : 0 );

		    DEBUG(("(D) Link %d is active : %c%c\n", link, LINKS[dimension], DIRS[physicalDirection]));

		    PRBS[dimension].reset_mask |= (physicalDirection == 0) ? SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_M_set(-1) : SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_RST_P_set(-1);
		
		    PRBS[dimension].tx_mask |= (physicalDirection == 0) ?
			( SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_M_set(0xf) | 
			  SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_M_set(prbs_type)
			    ) :
			( SERDES_LEFT_DCR__PRBS_CTL_A__TX_PRBS_EN_P_set(0xf) |
			  SERDES_LEFT_DCR__PRBS_CTL_A__PRBS_SEL_P_set(prbs_type)
			    );
		
		    PRBS[dimension].enable_mask |= (physicalDirection == 0) ? SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_M_set(0xf) : SERDES_LEFT_DCR__PRBS_CTL_A__RX_PRBS_EN_P_set(0xf);

		    PRBS[dimension].sync_mask  |= (physicalDirection == 0) ? SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_set(-1) : SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_set(-1);
		    PRBS[dimension].error_mask |= (physicalDirection == 0) ? SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_set(-1) : SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_P_set(-1);
		}
		else {
		    DEBUG(("(D) Link %d is inactive : %c%c\n", link, LINKS[dimension], DIRS[physicalDirection]));
		}
	    }
	    DCRWritePriv( PRBS[dimension].control_dcr, PRBS[dimension].tx_mask );
	}

	DEBUG(( "(D) PRBS controls for %c : { control=%08X reset=%016lX enable=%016lX } { status=%08X sync=%016lX err=%016lX }\n", 
		LINKS[dimension],
		PRBS[dimension].control_dcr, PRBS[dimension].reset_mask, PRBS[dimension].enable_mask,
		PRBS[dimension].status_dcr, PRBS[dimension].sync_mask, PRBS[dimension].error_mask
		  ));
    }

}

void prbs_start( void ) {

    
    DEBUG(("(I) Starting PRBS ...\n"));

    unsigned phase, dimension;

    for ( phase = 0; phase < 3; phase++ ) {

	for ( dimension = 0; dimension < NUM_HSS_DIMENSIONS; dimension++ ) {

	    if ( PRBS[dimension].enable_mask == 0 )
		continue;

	    switch ( phase ) {

	    case 0 : { // Put PRBS into reset
		DCRWritePriv( PRBS[dimension].control_dcr, PRBS[dimension].tx_mask | PRBS[dimension].reset_mask );
		break;
	    }

	    case 1 : { // Enable PRBS (holding reset)
		DCRWritePriv( PRBS[dimension].control_dcr, PRBS[dimension].tx_mask | PRBS[dimension].reset_mask | PRBS[dimension].enable_mask );
		break;
	    }

	    case 2 : { // Enable PRBS (clearing reset bits)
		DCRWritePriv( PRBS[dimension].control_dcr, PRBS[dimension].tx_mask | PRBS[dimension].enable_mask );
		break;
	    }

	    default : {
		ABORT(("(E) illegal phase (%d) at %s:%d\n", phase, __func__, __LINE__));
	    } 
	    }
	}
    }
}

#define PRBS_NO_SYNC(l) ( 0x1 << (10-(l)))
#define PRBS_ERRORS(l)  ( 0x1 << (21-(l)))

const char* fourBitString( uint64_t value, uint32_t position, char* buffer) {

    uint64_t mask = 0x8000000000000000ull >> (position-4);
    unsigned i;

    for ( i = 0; i < 4; i++ ) {
	buffer[i] = (value & mask) ? '1' : '0';
	mask >>= 1;
    }
    buffer[i] = 0;
    return buffer;
}

int prbs_monitor( void ) {

    int rc = 0;

    DEBUG(("(I) Monitoring PRBS ...\n"));

    //fwext_udelay( 3ull * 1000ull * 1000ull); // Run for 3 seconds

    // MMMMPPPP
    // 01234567
    // --------
    // 21304756

    unsigned char LANE_SHIFTS[] = { 3, 1, 0, 2, 4, 6, 7, 5 };
    
    unsigned dimension;
    //char bitstring[16];

    for ( dimension = 0; dimension < NUM_HSS_DIMENSIONS; dimension++ ) {
	
	if ( PRBS[dimension].enable_mask == 0 )
	    continue;
	
	uint64_t status = DCRReadPriv( PRBS[dimension].status_dcr );

	DEBUG(("(D) %c status %016lX\n", LINKS[dimension], status ));


	uint64_t sync_bit = SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_set(0x8);
	uint64_t err_bit  = SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_set(0x8);

	unsigned lane;

	for ( lane = 0; lane < 8; lane++ /*, sync_bit>>=1, err_bit>>=1 */ ) {

	    sync_bit = SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_set(0x8) >> LANE_SHIFTS[lane];
	    err_bit  = SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_set(0x8) >> LANE_SHIFTS[lane];

#if 0
	    if ( ( is_torus_location(0,1,1,0,0) || is_torus_location(0,1,2,0,0) ) && ( dimension == 2) ) {
		printf("rcb_read( 9, %d, PCS, 0, %X ) = %d\n", dimension, (lane*14) + 2, rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 ));
	    }
#endif

	    if ( PRBS[dimension].sync_mask & sync_bit ) {
		int err[4];

		//printf("rcb_read( 9, %d, PCS, 0, %X ) = %d\n", dimension, (lane*14) + 2, rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 ));
		
		if ( status & sync_bit )  {
		    if ( status & err_bit ) {
			err[lane%4] = rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 );
			//EVENT(("(!) %c%c L%d errors %d\n", LINKS[dimension], DIR(lane), lane, rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 )));
		    }
		    else {
			err[lane%4] = 0;
			//EVENT(("(!) %c%c L%d clean %d\n", LINKS[dimension], DIR(lane), lane, rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 )));
			if ( rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 ) > 0 ) {
			    uint64_t status2 = DCRReadPriv( PRBS[dimension].status_dcr );
			    if ( (status2 & err_bit) == 0 ) {
				printf("NONSENSE! lane=%d link=%d status=%016lX %016lX errmsk=%016lX %016lX err=%d \n", lane, dimension, status, status2, err_bit, PRBS[dimension].error_mask, rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 ));
				Terminate(__LINE__);
			    }
			}
		    }
		}
		else {
		    err[lane%4] = -1;
		    //EVENT(("(!) %c%c L%d sync error\n", LINKS[dimension], DIR(lane), lane ));
		}
		if ((lane % 4) == 3) {
		    EVENT(("(!) %c%c %d %d %d %d\n", LINKS[dimension], DIR(lane), err[0], err[1], err[2], err[3]));
		}
	    }
	}

#if 0	
	if ( ( status & PRBS[dimension].sync_mask ) == PRBS[dimension].sync_mask ) {

	    if ( ( status & PRBS[dimension].error_mask ) != 0 ) {
		
		if ( ( status & PRBS[dimension].error_mask & SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_set(-1) ) != 0 ) {
		    ERROR(("(E) %c- PRBS had errors : %s\n", LINKS[dimension], fourBitString(status,SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_M_position, bitstring)));
		    rc |= PRBS_ERRORS(dimension*2);
		}

		if ( ( status & PRBS[dimension].error_mask & SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_P_set(-1) ) != 0 ) {
		    ERROR(("(E) %c+ PRBS had errors : %s\n", LINKS[dimension], fourBitString(status,SERDES_LEFT_DCR__PRBS_STAT_A__ERROR_P_position, bitstring)));
		    rc |= PRBS_ERRORS(dimension*2 + 1);
		} 

		unsigned la;

		for ( la = 0; la < 8; la++ ) {
		    ERROR(("(E) error(%d) %08X\n", la, rcb_read(9, dimension, RCB_BT_PCS, 0, (la*14) + 2 )));
		}
	    }
	    else {
		EVENT(("(*) Dimension %c PRBS is clean\n", LINKS[dimension]));
	    }
	}
	else {
	    
	    if ( ( status & PRBS[dimension].sync_mask & SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_set(-1) ) != SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_set(-1) ) {
		ERROR(("(E) %c- PRBS did not sync : %s\n", LINKS[dimension], fourBitString(status,SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_M_position, bitstring)));
		rc |= PRBS_NO_SYNC(dimension*2);
	    }

	    if ( ( status & PRBS[dimension].sync_mask & SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_set(-1) ) != SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_set(-1) ) {
		ERROR(("(E) %c+ PRBS did not sync : %s\n", LINKS[dimension], fourBitString(status,SERDES_LEFT_DCR__PRBS_STAT_A__SYNC_P_position, bitstring)));
		rc |= PRBS_NO_SYNC(dimension*2 + 1);
	    }
	}
#endif

    }

    return rc;
}

void prbs_shutdown( void ) {

    DEBUG(("(I) Shutting down PRBS ...\n"));

    unsigned phase, dimension;

    for ( phase = 0; phase <= 1; phase++ ) {

	for ( dimension = 0; dimension < NUM_HSS_DIMENSIONS; dimension++ ) {

	    if ( PRBS[dimension].enable_mask == 0 )
		continue;

	    switch ( phase ) {

	    case 0 : { // Put PRBS into reset
		DCRWritePriv( PRBS[dimension].control_dcr, PRBS[dimension].tx_mask | PRBS[dimension].reset_mask );
		break;
	    }

	    case 1 : { // clear reset
		DCRWritePriv( PRBS[dimension].control_dcr, PRBS[dimension].tx_mask );
#if 0 
		// DEBUG CODE
		{
		    fwext_udelay(1000); // 1 millis
		    int lane;
		    for (lane = 0; lane < 8; lane++) {
			printf("(!) %c%c L%d counter %d\n", LINKS[dimension], DIR(lane), lane, rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 ));
		    }
		}
#endif
		break;
	    }

	    default : {
		ABORT(("(E) illegal phase (%d) at %s:%d\n", phase, __func__, __LINE__));
	    } 
	    }
	}

	if ( phase == 0 ) {
	    fwext_udelay(10000); // 10 millis
	}
    }
}


int run_prbs( void ) {

    int rc = 0;

    prbs_start();
    fwext_udelay( 2ull * 1000ull * 1000ull); // Run for 10 seconds
    rc = prbs_monitor();
    prbs_shutdown();
    

    return rc;
}

void setTransmittersAndReceivers( int txamp, int txeq, int rxeq ) {

    unsigned link, dir, lane;

    unsigned tx = 3 | (txamp << 2) | (txeq << 8);
    unsigned rx = rxeq;

    for ( link = 0; link < 6; link++ ) {
	for ( dir = 0; dir < 2; dir++ ) {
	    for ( lane = 0; lane < 4; lane++ ) {
		rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x34, rx );
		rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x54, tx );
	    }
	}
    }
}

int hss_calibrate_reg(int link, int dir, int lane, int reg)
{
    rcb_val_t reg_orig, reg_new;
    int sign, mag, left, right, middle;

    reg_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg);

    for (left = 62; left > 0; left--) {
	sign = (left < 31);
	mag = (left - 31) * (1 - (sign*2));
	reg_new = (reg_orig & ~0x003f) | ((sign << 5) | mag);
	rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, reg, reg_new);
	if ((rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg) & 0x0080) != 0) {
	    break;
	}
    }

    for (right = 0; right < 62; right++) {
	sign = (right < 31);
	mag = (right - 31) * (1 - (sign*2));
	reg_new = (reg_orig & ~0x003f) | ((sign << 5) | mag);
	rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, reg, reg_new);
	if ((rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg) & 0x0080) == 0) {
	    break;
	}
    }

    middle = (left + right) / 2;

    sign = (middle < 31);
    mag = (middle - 31) * (1 - (sign*2));
    reg_new = (reg_orig & ~0x003f) | ((sign << 5) | mag);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, reg, reg_new);
    reg_new = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, reg);

    return (middle - 31);
}

int hss_calibrate_lane(int link, int dir, int lane, int fudge)
{
    rcb_val_t auxinterp_orig, auxinterp_new;
    rcb_val_t escope_orig, escope_new;
    rcb_val_t rxfectrl_orig, rxfectrl_new;
    rcb_val_t rxvosctl_orig, rxvosctl_new;
    rcb_val_t rxvoseq_orig, rxvoseq_new;
    int eqoffset, d0offset_orig, d0offset_fudged;

    //auxinterp.en = 1;
    //auxinterp.escope = 1;
    auxinterp_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x02);
    auxinterp_new = (auxinterp_orig & ~0x0001) | 0x0001;
    auxinterp_new = (auxinterp_new & ~0x0002) | 0x0002;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x02, auxinterp_new);

    //escope.en = 1;
    escope_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x24);
    escope_new = (escope_orig & ~0x0100) | 0x0100;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x24, escope_new);

    //rxfectrl.eq_ovr = 1;
    //rxfectrl.sloop = 0;
    //rxfectrl.lsen = 1;
    //rxfectrl.lsby = 0;
    rxfectrl_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x22);
    rxfectrl_new = (rxfectrl_orig & ~0x0004) | 0x0004;
    rxfectrl_new = (rxfectrl_new  & ~0x0008) | 0x0000;
    rxfectrl_new = (rxfectrl_new  & ~0x0010) | 0x0010;
    rxfectrl_new = (rxfectrl_new  & ~0x0020) | 0x0000;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x22, rxfectrl_new);

    //rxvosctl.ovr = 1;
    //rxvosctl.vosen = 0b100;
    rxvosctl_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36);
    rxvosctl_new = (rxvosctl_orig & ~0x0001) | 0x0001;
    rxvosctl_new = (rxvosctl_new  & ~0x0038) | 0x0020;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, rxvosctl_new);

    //rxvoseq.vos = 0b000000;
    rxvoseq_orig = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x4a);
    rxvoseq_new = (rxvoseq_orig & ~0x003f) | 0x0000;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x4a, rxvoseq_new);

    d0offset_orig = hss_calibrate_reg(link, dir, lane, 0x3e);
    hss_calibrate_reg(link, dir, lane, 0x40);
    hss_calibrate_reg(link, dir, lane, 0x42);
    hss_calibrate_reg(link, dir, lane, 0x44);
    hss_calibrate_reg(link, dir, lane, 0x46);
    hss_calibrate_reg(link, dir, lane, 0x48);

    //rxvosctl.vosen = 0b010; (for DC links (A - D))
    //rxvosctl.vosen = 0b001; (for AC links (E))
    rxvosctl_new = (rxvosctl_new  & ~0x0038) | ((link < 4) ? 0x0010 : 0x0008);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, rxvosctl_new);

    eqoffset = hss_calibrate_reg(link, dir, lane, 0x4a);

    if (fudge && (eqoffset == -31)) {
	d0offset_fudged = hss_calibrate_reg(link, dir, lane, 0x3e);
	hss_calibrate_reg(link, dir, lane, 0x40);
	hss_calibrate_reg(link, dir, lane, 0x42);
	hss_calibrate_reg(link, dir, lane, 0x44);
	hss_calibrate_reg(link, dir, lane, 0x46);
	hss_calibrate_reg(link, dir, lane, 0x48);
    } else {
	d0offset_fudged = d0offset_orig;
    }

    //rxvosctl.vosen = 0b000;
    rxvosctl_new = (rxvosctl_new  & ~0x0038) | 0x0000;
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36, rxvosctl_new);

    // restore rxfectrl, escope, and auxinterp
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x22, rxfectrl_orig);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x24, escope_orig);
    rcb_write(9, link, RCB_BT_LANE, (dir*4)+lane, 0x02, auxinterp_orig);

    return (d0offset_orig - d0offset_fudged);
}

int hss_lane_active(int link, int dir, int lane)
{
    return ((link < 5) || (dir < 2)) &&  // exclude + direction for I/O link
	   ((rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36) & 0x0004) != 0);
}

void hss_calibration_dump_lane(int link, int dir, int lane)
{
    rcb_val_t v34, v36, v3e, v40, v42, v44, v46, v48, v4a;

    v34 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x34);
    v36 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x36);
    v3e = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x3e);
    v40 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x40);
    v42 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x42);
    v44 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x44);
    v46 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x46);
    v48 = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x48);
    v4a = rcb_read(9, link, RCB_BT_LANE, (dir*4)+lane, 0x4a);
    EVENT(("(D) Link %c%c lane %d: 0x34=0x%04x 0x36=0x%04x 0x3e=0x%04x 0x40=0x%04x "
	   "0x42=0x%04x 0x44=0x%04x 0x4a=0x%04x\n",
	   //"0x42=0x%04x 0x44=0x%04x 0x46=0x%04x 0x48=0x%04x 0x4a=0x%04x\n",
	   "ABCDEI"[link], "-+"[dir], (dir*4)+lane,
	    v34, v36, v3e, v40, v42, v44, /*v46, v48,*/ v4a));
}

void hss_calibrate_all(int fudge)
{
    int link, dir, lane;

    for (link = 0; link < 6; link++) {
	for (dir = 0; dir < 2; dir++) {
	    for (lane = 0; lane < 4; lane++) {
		if (hss_lane_active(link, dir, lane)) {
		    hss_calibrate_lane(link, dir, lane, fudge);
#if 0
		    if ( ( is_torus_location(0,1,1,0,0) || is_torus_location(0,1,2,0,0) ) && ( link == 2) ) {
			hss_calibration_dump_lane(link, dir, lane);
		    }
#endif
		}
	    }
	}
    }
}


int step( int i, int j, int k, int txamp, int txeq, int rxeq ) {
    barrier();
    INFO(("------------------------------------------------------------------------\n"));
    INFO(("(!) Step %d,%d,%d TxAmp=%d TxEq=%d RxEq=%d\n", i, j, k, txamp, txeq, rxeq ));
    setTransmittersAndReceivers( txamp, txeq, rxeq );
    hss_calibrate_all(1); // calibrate with fudging
    barrier();
    return run_prbs();
}

static int BQC_TXAMPs[] = { 31, 30, 29, 28, 23, 15, 27, 22, 14, 26, 21, 13, 25, 20, 12, 7 };
static int BQC_TXEQs[32][16] = {
    { 0, }, // 0
    { 0, }, // 1
    { 0, }, // 2
    { 0, }, // 3
    { 0, }, // 4
    { 0, }, // 5
    { 0, }, // 6
    { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 7
    { 0, }, // 8
    { 0, }, // 9
    { 0, }, // 10
    { 0, }, // 11
    { 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 12
    { 0, 20, 25,  1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 13
    { 0, 20, 27,  3, 23, 26,  2,  9, -1, -1, -1, -1, -1, -1, -1, -1 }, // 14
    { 0, 20, 25,  1, 21, 26,  2, 22, 27,  3,  8, -1, -1, -1, -1, -1 }, // 15
    { 0, }, // 16
    { 0, }, // 17
    { 0, }, // 18
    { 0, }, // 19
    { 0, 24, 19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 20
    { 0, 24, 12,  1, 25, 18, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 21
    { 0, 24, 12,  3, 27, 15,  2, 26, 17, -1, -1, -1, -1, -1, -1, -1 }, // 22
    { 0, 24, 12,  1, 25, 13,  2, 26, 14,  3, 27, 16, -1, -1, -1, -1 }, // 23
    { 0, }, // 24
    { 0, 13, 21, 30, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 25
    { 0, 15, 23,  3, 14, 22, 29, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 26
    { 0, 13, 21,  1, 14, 22,  2, 15, 23, 28, -1, -1, -1, -1, -1, -1 }, // 27
    { 0, 11, 19,  7, 10, 18,  6,  9, 17,  5,  8, 16, 27, -1, -1, -1 }, // 28
    { 0,  1, 10, 18,  6, 11, 19,  7,  8, 16,  4,  9, 17, 26, -1, -1 }, // 29
    { 0,  3,  2,  9, 17,  5,  8, 16,  4, 11, 19,  7, 10, 18, 25, -1 }, // 30
    { 0,  1,  2,  3,  8, 16,  4,  9, 17,  5, 10, 18,  6, 11, 19, 24 }  // 31
};

static int BQC_RXEQs[] = { 7, 6, 5, 4, 3, 2, 1, 0 };



int hss_tune( void ) {

    INFO(("(I) Begin tuning ...\n"));

    prbs_initialize_controls();

    unsigned i, j, k;

    for ( i = 0; i < sizeof(BQC_TXAMPs)/sizeof(BQC_TXAMPs[0]); i++ ) {
	for ( j = 0; j < 16; j++ ) {
	    if ( BQC_TXEQs[BQC_TXAMPs[i]][j] >= 0 ) {
		for ( k = 0; k < sizeof(BQC_RXEQs)/sizeof(BQC_RXEQs[0]); k++ ) {
		    step( i, j, k, BQC_TXAMPs[i], BQC_TXEQs[BQC_TXAMPs[i]][j], BQC_RXEQs[k] );
		    //{barrier();return 0;} // QUICK EXIT AFTER ONE ITERATION!!!!!!!!!!!!!!!!!!
		}
	    }
	}
    }

    return 0;
}


int test_main() {

    if (PhysicalThreadIndex() > 0) {  // run single-threaded
	exit(0);
    }

/*
    if ( fwext_getenv("HSS_EXPLORE_ALL") != 0 ) {
	// To generate calibrations for all rxeq settings:
	hss_explore_all(1);
    }
*/


    int rc;

    PERS = fwext_getPersonality();

    uint64_t nf2 = PERS->Network_Config.NetFlags2;

    printf("(!) Torus (%d,%d,%d,%d,%d) / (%d,%d,%d,%d,%d) Swaps (%lld,%lld,%lld,%lld,%lld)\n",
	   PERS->Network_Config.Acoord, PERS->Network_Config.Bcoord, PERS->Network_Config.Ccoord, PERS->Network_Config.Dcoord, PERS->Network_Config.Ecoord,
	   PERS->Network_Config.Anodes, PERS->Network_Config.Bnodes, PERS->Network_Config.Cnodes, PERS->Network_Config.Dnodes, PERS->Network_Config.Enodes,
	   TI_GET_TORUS_DIM_REVERSED(0,nf2), TI_GET_TORUS_DIM_REVERSED(1,nf2), TI_GET_TORUS_DIM_REVERSED(2,nf2), TI_GET_TORUS_DIM_REVERSED(3,nf2), TI_GET_TORUS_DIM_REVERSED(4,nf2)
	);

    rc = hss_tune();
    
    //rc = run_prbs();

    exit(rc);
}
