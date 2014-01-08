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

#include <firmware/include/Firmware.h>
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/sd_rcb.h>

#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Personality_t* PERS = 0;

#define PRIMORDIAL_NODE() ( ( PERS->Network_Config.Acoord == 0 ) && ( PERS->Network_Config.Bcoord == 0 ) && ( PERS->Network_Config.Ccoord == 0 ) && ( PERS->Network_Config.Dcoord == 0 ) && ( PERS->Network_Config.Ecoord == 0 ) )

#define INFO(s) if ( PRIMORDIAL_NODE() ) printf s;
#define ABORT(s) { printf s; Terminate(__LINE__); }
#define DEBUG(s) /*printf s;*/
#define EVENT(s)  printf s; 
#define ERROR(s) printf s;

#define DIAGS_HSS_STATIC_PATTERN_NO_SYNC      0x000202C0
#define DIAGS_HSS_STATIC_PATTERN_BAD_EYE      0x000202C1
#define DIAGS_HSS_STATIC_PATTERN_ERROR_BIT    0x000202C2
#define DIAGS_HSS_STATIC_PATTERN_CSBARRIER    0x000202C3
#define DIAGS_HSS_STATIC_PATTERN_PHASE_START  0x000202C4
#define DIAGS_HSS_STATIC_PATTERN_PHASE_END    0x000202C5

void barrier( void ) {

    const uint64_t BARRIER_TIMEOUT = 120ull * 1000ull * 1000ull; // 2 minutes

    if ( fwext_getFwInterface()->barrier( BARRIER_TIMEOUT ) != FW_OK ) {
      fwext_getFwInterface()->writeRASEvent(DIAGS_HSS_STATIC_PATTERN_CSBARRIER, 0, 0);
      ABORT(( "(E) Barrier timeout."));
    }
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

#define NUM_HSS_DIMENSIONS 4

PRBS_Data_t PRBS[NUM_HSS_DIMENSIONS] = { { 0, }, };

char* lanePhyToLogStr(int link, int lane)
{
  // logical lanes are the ones we see in the Serdes DCR registers
  // physical lanes are the ones we access via RCB (i.e. the per lane registers)
  if (link == 5)
  {
    switch (lane)
    {
      case 0:
        return "M1\0";
      case 1:
        return "M3\0";
      case 2:
        return "M2\0";
      case 3:
        return "M0\0";
      default:
        return "NA\0";
    };
  }
  else if (link >= 0 && link <= 4)
  {
    switch (lane)
    {
      case 0:
        return "M3\0";
      case 1:
        return "M1\0";
      case 2:
        return "M0\0";
      case 3:
        return "M2\0";
      case 4:
        return "P0\0";
      case 5:
        return "P2\0";
      case 6:
        return "P3\0";
      case 7:
        return "P1\0";
      default:
        return "NA\0";
    }
  }
  else
  {
    return "NA\0";
  }
}

int laneToBit(int link, int lane)
{
  if (link == 5)
  {
    switch (lane)
    {
      case 0:
        return 1;
      case 1:
        return 3;
      case 2:
        return 2;
      case 3:
        return 0;
      default:
        return -1;
    };
  }
  else if (link >= 0 && link <= 4)
  {
    switch (lane)
    {
      case 0:
        return 3;
      case 1:
        return 1;
      case 2:
        return 0;
      case 3:
        return 2;
      case 4:
        return 0;
      case 5:
        return 2;
      case 6:
        return 3;
      case 7:
        return 1;
      default:
        return -1;
    }
  }
  else
  {
    return -1;
  }
}

int laneLogToPhyInt(int link, int lane)
{
  // logical lanes are the ones we see in the Serdes DCR registers
  // physical lanes are the ones we access via RCB (i.e. the per lane registers)
  if (link == 5)
  {
    switch (lane)
    {
      case 0:
        return 3;
      case 1:
        return 0;
      case 2:
        return 2;
      case 3:
        return 1;
      default:
        return -1;
    };
  }
  else if (link >= 0 && link <= 4)
  {
    switch (lane)
    {
      case 0:
        return 2;
      case 1:
        return 1;
      case 2:
        return 3;
      case 3:
        return 0;
      case 4:
        return 4;
      case 5:
        return 7;
      case 6:
        return 5;
      case 7:
        return 6;
      default:
        return -1;
    }
  }
  else
  {
    return -1;
  }
}

void prbs_initialize_controls( void ) {

    unsigned dimension, direction, link;

    unsigned prbs_type = 3; // static pattern

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
                fw_uint64_t details[2];
                details[0] = phase;
                details[1] = dimension;
                fwext_getFwInterface()->writeRASEvent(DIAGS_HSS_STATIC_PATTERN_PHASE_START, 2, details);
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

	        int bit = laneToBit(dimension, lane);
		//printf("rcb_read( 9, %d, PCS, 0, %X ) = %d\n", dimension, (lane*14) + 2, rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 ));

		if ( status & sync_bit )  {
		    if ( status & err_bit ) {
			int err;
			err = rcb_read(9, dimension, RCB_BT_PCS, 0, (lane*14) + 2 );
			EVENT(("(!) %c%c L%d errors %d\n", LINKS[dimension], DIR(lane), lane, err));
			rc++;

			fw_uint64_t details[4];
			details[0] = LINKS[dimension];
			details[1] = DIR(lane);
			details[2] = bit;
			details[3] = lane;
			fwext_getFwInterface()->writeRASEvent(DIAGS_HSS_STATIC_PATTERN_ERROR_BIT, 4, details);
		    }
		    else {
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
		    EVENT(("(!) %c%c L%d sync error\n", LINKS[dimension], DIR(lane), lane ));
		    rc++;

		    fw_uint64_t details[4];
		    details[0] = LINKS[dimension];
		    details[1] = DIR(lane);
                    details[2] = bit;
                    details[3] = lane;
		    fwext_getFwInterface()->writeRASEvent(DIAGS_HSS_STATIC_PATTERN_NO_SYNC, 4, details);
		}
	    }
	}
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
	        fw_uint64_t details[2];
                details[0] = phase;
                details[1] = dimension;
                fwext_getFwInterface()->writeRASEvent(DIAGS_HSS_STATIC_PATTERN_PHASE_END, 2, details);
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
    fwext_udelay( 10ull * 1000ull * 1000ull); // Run for 10 seconds
    rc = prbs_monitor();
    prbs_shutdown();


    return rc;
}

void set_prbs_static_pattern( int pattern ) {
    unsigned link, dir, lane;

    for ( link = 0; link < NUM_HSS_DIMENSIONS; link++ ) {
	for ( dir = 0; dir < 2; dir++ ) {
	    for ( lane = 0; lane < 4; lane++ ) {
		rcb_write(9, link, RCB_BT_PCS, 0, (((dir*4)+lane)*14) + 0x08, pattern );
		rcb_write(9, link, RCB_BT_PCS, 0, (((dir*4)+lane)*14) + 0x0a, pattern );
		rcb_write(9, link, RCB_BT_PCS, 0, (((dir*4)+lane)*14) + 0x0c, pattern );
	    }
	}
    }
}

void dumpRegs()
{
  uint64_t prbsCtrlReg0 = DCRReadPriv( 0x140120 );
  uint64_t prbsCtrlReg1 = DCRReadPriv( 0x140121 );
  uint64_t prbsCtrlReg2 = DCRReadPriv( 0x140122 );
  uint64_t prbsCtrlReg3 = DCRReadPriv( 0x144120 );
  uint64_t prbsCtrlReg4 = DCRReadPriv( 0x144121 );
  uint64_t prbsCtrlReg5 = DCRReadPriv( 0x144122 );

  printf("PRBS CTRL regs: 0x140120: %016lX, 0x140121: %016lX, 0x140122: %016lX, 0x144120: %016lX, 0x144121: %016lX, 0x144122: %016lX",
      prbsCtrlReg0, prbsCtrlReg1, prbsCtrlReg2, prbsCtrlReg3, prbsCtrlReg4, prbsCtrlReg5 );

  unsigned link, dir, lane;
  for ( link = 0; link < NUM_HSS_DIMENSIONS; link++ ) {
      for ( dir = 0; dir < 2; dir++ ) {
          for ( lane = 0; lane < 4; lane++ ) {
              printf("PCS 0x08: %04X, PCS 0x0A: %04X, PCS 0x0C: %04X",
                  rcb_read(9, link, RCB_BT_PCS, 0, (((dir*4)+lane)*14) + 0x08 ),
                  rcb_read(9, link, RCB_BT_PCS, 0, (((dir*4)+lane)*14) + 0x0a ),
                  rcb_read(9, link, RCB_BT_PCS, 0, (((dir*4)+lane)*14) + 0x0c ));
          }
      }
  }
}




#define rcb_bit(n) (1 << n)
#define DIAGS_SURVEY_LINK_ERRORS_RAS_ID 0x00020280

#define usleep(n) Delay_ns(n*1000, 1600);

int gen_data_eye(int link, int llane, int vstep, int pstep, int *eye_passtot, int *eye_samples)
{
  int rc = 0;
  int lane = laneLogToPhyInt(link, llane);

  // step 0: configure lane for data eye gathering

  uint16_t auxinterp = rcb_read(9, link, RCB_BT_LANE, lane, 0x02);
  uint16_t escope = rcb_read(9, link, RCB_BT_LANE, lane, 0x24);

  rcb_write(9, link, RCB_BT_LANE, lane, 0x02, (auxinterp | rcb_bit(0) | // auxinterp.en = 1
      rcb_bit(1))); // auxinterp.escope = 1
  rcb_write(9, link, RCB_BT_LANE, lane, 0x24, (escope | rcb_bit(8))); // escope.en = 1

  uint16_t rxvosd0 = rcb_read(9, link, RCB_BT_LANE, lane, 0x3e);
  uint16_t rxvosd1 = rcb_read(9, link, RCB_BT_LANE, lane, 0x40);
  uint16_t rxvose0 = rcb_read(9, link, RCB_BT_LANE, lane, 0x42);
  uint16_t rxvose1 = rcb_read(9, link, RCB_BT_LANE, lane, 0x44);
  uint16_t rxvoseq = rcb_read(9, link, RCB_BT_LANE, lane, 0x4a);
  uint16_t rxvosctl = rcb_read(9, link, RCB_BT_LANE, lane, 0x36);
  uint16_t rxvosr0   = rcb_read(9, link, RCB_BT_LANE, lane, 0x46);
  uint16_t rxvosr1   = rcb_read(9, link, RCB_BT_LANE, lane, 0x48);

  if (0 == (rxvosctl & 1))
  {
    rcb_write(9, link, RCB_BT_LANE, lane, 0x3e, 0x3f & (rxvosd0 >> 8)); // rxvosd0.vos = rxvosd0.val
    rcb_write(9, link, RCB_BT_LANE, lane, 0x40, 0x3f & (rxvosd1 >> 8)); // rxvosd1.vos = rxvosd1.val
    rcb_write(9, link, RCB_BT_LANE, lane, 0x42, 0x3f & (rxvose0 >> 8)); // rxvose0.vos = rxvose0.val
    rcb_write(9, link, RCB_BT_LANE, lane, 0x44, 0x3f & (rxvose1 >> 8)); // rxvose1.vos = rxvose1.val
    rcb_write(9, link, RCB_BT_LANE, lane, 0x4a, 0x3f & (rxvoseq >> 8)); // rxvoseq.vos = rxvoseq.val
  }

  rcb_write(9, link, RCB_BT_LANE, lane, 0x36, (rxvosctl | rcb_bit(0))); // rxvosctl.ovr = 1

  // step 1: gather data eye

  uint16_t passtot = 0;
  uint16_t samples = 0;
  uint16_t tot = 0;
  uint16_t offset = 0;
  uint16_t div_ratio = 1;
  uint16_t cdroffset = rcb_read(9, link, RCB_BT_LANE, lane, 0x32);

  uint16_t v, p;
  //        uint16_t vstep = 4; // voltage step size
  //        uint16_t pstep = 8; // phase step size
  char eye[64 / vstep][128*div_ratio/pstep];

  for(  v = 0; v < 64; v += vstep)
  {
    if (v < 32)
    {
      offset = ((31 - v) & 0x1f) | 0x20;
    }
    else
    {
      offset = (v & 0x1f);
    }

    rcb_write(9, link, RCB_BT_LANE, lane, 0x46, (rxvosr0 & ~0x3f) | offset); // rxvosr0.vos = offset;
    rcb_write(9, link, RCB_BT_LANE, lane, 0x48, (rxvosr1 & ~0x3f) | offset); // rxvosr1.vos = offset;

    for (p = 0; p < 128*div_ratio; p += pstep)
    {
      //only look at 2 < phase < 55 for eyes
      if ((p > 2) && (p < 0x48))
      {
        eye[(64-v)/vstep][p/pstep] = '.';
        samples++;
        continue;
      }

      rcb_write(9, link, RCB_BT_LANE, lane, 0x32, (cdroffset | ((p & 0x7f) << 8))); // cdroffset.rove

      // clear error counter by reading it
      tot = rcb_read(9, link, RCB_BT_LANE, lane, 0x26);// escount.count

      // wait and read again to get a rough error rate
      usleep(6000);
      tot = rcb_read(9, link, RCB_BT_LANE, lane, 0x26);// escount.count

      samples++;
      eye[(64-v)/vstep][p/pstep] = '.';

      if (tot == 0)
      {
        eye[(64-v)/vstep][p/pstep] = '#';
        passtot++;
      }
    }
  }

  // Check to see if the static pattern data eye is not 50%.
  float eyeSize = ((passtot * 100) / samples);
  if(eyeSize < 50.0)
  {
    char torus = 'A' + link;
    char torusDir = ((lane >= 4) ? '+' : '-');
    int bit = laneToBit(link, lane);
    printf("ERROR! Static data eye size bad for link %d, lane %d (%c%c, bit %d). Eye size is %d.%02d%%.", link, lane,
        torus, torusDir, bit, ((passtot * 100) / samples), ((passtot*10000)/samples)%100);

    // step 3: print data eye
    printf("\nDATA EYE FOR LINK %i, PHY LANE %i LOG LANE %i:\n\n", link, lane, llane);

    for (v = 0; v < 64; v += vstep)
    {
      char line[sizeof(eye[v / vstep]) + 1];
      memcpy(line, eye[v/vstep], sizeof(eye[v/vstep]));
      line[sizeof(eye[v/vstep])] = '\0';
      printf("%s\n", line);
    }

    // fake float - float doesn't print for whatever reason.
    printf("\n");
    printf("eye passes    = %i.%02i%%\n", ((passtot * 100) / samples), ((passtot*10000)/samples)%100);
    printf("total passes  = %i\n", passtot);
    printf("total samples = %i\n", samples);

    // Check to see if the static pattern data eye is not 40%.
    if(eyeSize < 40.0)
    {
      rc = 1;

      fw_uint64_t details[7];
      details[0] = link;
      details[1] = lane;
      details[2] = torus;
      details[3] = torusDir;
      details[4] = bit;
      details[5] = ((passtot * 100) / samples);
      details[6] = ((passtot * 10000) / samples) % 100;
      fwext_getFwInterface()->writeRASEvent(DIAGS_HSS_STATIC_PATTERN_BAD_EYE, 7, details);
    }
  }

  *eye_passtot = passtot;
  *eye_samples = samples;

  // step 4: restore original values.
  rcb_write(9, link, RCB_BT_LANE, lane, 0x3e, rxvosd0);
  rcb_write(9, link, RCB_BT_LANE, lane, 0x40, rxvosd1);
  rcb_write(9, link, RCB_BT_LANE, lane, 0x42, rxvose0);
  rcb_write(9, link, RCB_BT_LANE, lane, 0x44, rxvose1);
  rcb_write(9, link, RCB_BT_LANE, lane, 0x4a, rxvoseq);
  rcb_write(9, link, RCB_BT_LANE, lane, 0x36, rxvosctl);

  return rc;
}


int performEyeCheck()
{
  int rc = 0;
  Personality_t *pers = fwext_getPersonality();
  personality_convert_map(pers);

  int linkdir[8];

  linkdir[0] = (SD_ENABLE_T0 & pers->Network_Config.NetFlags2) != 0;
  linkdir[1] = (SD_ENABLE_T1 & pers->Network_Config.NetFlags2) != 0;
  linkdir[2] = (SD_ENABLE_T2 & pers->Network_Config.NetFlags2) != 0;
  linkdir[3] = (SD_ENABLE_T3 & pers->Network_Config.NetFlags2) != 0;
  linkdir[4] = (SD_ENABLE_T4 & pers->Network_Config.NetFlags2) != 0;
  linkdir[5] = (SD_ENABLE_T5 & pers->Network_Config.NetFlags2) != 0;
  linkdir[6] = (SD_ENABLE_T6 & pers->Network_Config.NetFlags2) != 0;
  linkdir[7] = (SD_ENABLE_T7 & pers->Network_Config.NetFlags2) != 0;

  unsigned int i;
  int voltage_step = 4;
  int phase_step = 8;

  if (fwext_getenv("VSTEP") != 0)
    voltage_step = fwext_strtoul(fwext_getenv("VSTEP"), 0, 0);
  if (fwext_getenv("PSTEP") != 0)
    phase_step = fwext_strtoul(fwext_getenv("PSTEP"), 0, 0);

  int eye_passtot[6][8];
  int eye_samples[6][8];

  for (i = 0; i < 8; i++)
  {
    if (linkdir[i] == 1)
    {
      int _eye_passtot, _eye_samples;

      int link = i / 2;
      int lane = 0;
      for (lane = ((i % 2) * 4); lane <= ((i % 2) * 4) + 3; lane++)
      {
        rc += gen_data_eye(link, lane, voltage_step, phase_step, &_eye_passtot, &_eye_samples);

        eye_passtot[link][lane] = _eye_passtot;
        eye_samples[link][lane] = _eye_samples;
      }
    }
  }

  return rc;
}



int hss_wrap_test( int pattern )
{
  int rc = 0;

  barrier();
  set_prbs_static_pattern(pattern & 0xFFFF);

  barrier();
  rc += run_prbs();

  barrier();
  rc += performEyeCheck();

  return rc;
}


int test_main()
{
  int rc = 0;
//  int i = 0;
  PERS = fwext_getPersonality();
  uint64_t nf2 = PERS->Network_Config.NetFlags2;

  // Run single-threaded.
  if (PhysicalThreadIndex() > 0)
  {
    exit(0);
  }

  printf("(!) Torus (%d,%d,%d,%d,%d) / (%d,%d,%d,%d,%d) Swaps (%lld,%lld,%lld,%lld,%lld)\n",
      PERS->Network_Config.Acoord, PERS->Network_Config.Bcoord, PERS->Network_Config.Ccoord, PERS->Network_Config.Dcoord, PERS->Network_Config.Ecoord,
      PERS->Network_Config.Anodes, PERS->Network_Config.Bnodes, PERS->Network_Config.Cnodes, PERS->Network_Config.Dnodes, PERS->Network_Config.Enodes,
      TI_GET_TORUS_DIM_REVERSED(0,nf2), TI_GET_TORUS_DIM_REVERSED(1,nf2), TI_GET_TORUS_DIM_REVERSED(2,nf2), TI_GET_TORUS_DIM_REVERSED(3,nf2), TI_GET_TORUS_DIM_REVERSED(4,nf2)
  );

  barrier();
  prbs_initialize_controls();

  barrier();
  INFO(("(I) Testing static 0's ...\n"));
  rc += hss_wrap_test(0x0000);

  barrier();
  INFO(("(I) Testing static 1's ...\n"));
  rc += hss_wrap_test(0xFFFF);

  barrier();

  exit(rc);
}
