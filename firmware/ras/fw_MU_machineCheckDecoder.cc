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

#include <sstream>
#include <iostream>
#include <vector>
#include <ras/include/RasEvent.h>
#include <hwi/include/bqc/mu_dcr.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/ddr.h>

using namespace std;

#include "_mu_dcr_decoders.cc"

extern void decode_ND_500_DCR_FATAL_ERR0( ostringstream& str, uint64_t status );
extern void decode_ND_500_DCR_FATAL_ERR1( ostringstream& str, uint64_t status );
extern void decode_ND_500_DCR_FATAL_ERR2( ostringstream& str, uint64_t status );
extern void decode_ND_500_DCR_FATAL_ERR3( ostringstream& str, uint64_t status );
extern void decode_ND_500_DCR_FATAL_ERR4( ostringstream& str, uint64_t status );
extern void decode_ND_500_DCR_FATAL_ERR5( ostringstream& str, uint64_t status );
extern void decode_ND_X2_DCR_FATAL_ERR0( ostringstream& str, uint64_t status );
extern void decode_ND_X2_DCR_FATAL_ERR1( ostringstream& str, uint64_t status );
extern void decode_ND_RESE_DCR_FATAL_ERR( ostringstream& str, uint64_t status, unsigned unit );

extern "C" {


  typedef struct {
    uint64_t dcrAddress;                                                   // Interrupt Status DCR address
    void (*decoder)( ostringstream&, uint64_t );                           // Decoder for the preceding value
    void (*additionalDetails)( ostringstream&, const vector<uint64_t>& );  // Additional data collector
  } MU_Decoder;


  void getEccCountInfo( ostringstream&, const vector<uint64_t>& );
  void getImeInfo( ostringstream&, const vector<uint64_t>& );
  void getMcsramInfo( ostringstream&, const vector<uint64_t>& );
  void getRmeInterruptsInfo( ostringstream&, const vector<uint64_t>& );
  void getSlavePortInfo( ostringstream&, const vector<uint64_t>& );

  MU_Decoder MU_DECODERS[] = {
    {  MU_DCR(ARLOG_INTERRUPTS__MACHINE_CHECK),           decode_arlog_interrupts,         0                     },
    {  MU_DCR(ECC_COUNT_INTERRUPTS__MACHINE_CHECK),       decode_ecc_count_interrupts,     getEccCountInfo       },
    // skip FIFO_INTERRUPTS -- no machine check data
    {  MU_DCR(ICSRAM_INTERRUPTS__MACHINE_CHECK),          decode_icsram_interrupts,        0                     },
    {  MU_DCR(IME_INTERRUPTS__MACHINE_CHECK),             decode_ime_interrupts,           getImeInfo            },
    {  MU_DCR(IMU_ECC_INTERRUPTS__MACHINE_CHECK),         decode_imu_ecc_interrupts,       0                     },
    {  MU_DCR(MASTER_PORT0_INTERRUPTS__MACHINE_CHECK),    decode_master_port0_interrupts,  0                     },
    {  MU_DCR(MASTER_PORT1_INTERRUPTS__MACHINE_CHECK),    decode_master_port1_interrupts,  0                     },
    {  MU_DCR(MASTER_PORT2_INTERRUPTS__MACHINE_CHECK),    decode_master_port2_interrupts,  0                     },
    {  MU_DCR(MCSRAM_INTERRUPTS__MACHINE_CHECK),          decode_mcsram_interrupts,        getMcsramInfo         },
    {  MU_DCR(MISC_ECC_CORR_INTERRUPTS__MACHINE_CHECK),   decode_misc_ecc_corr_interrupts, 0                     },
    {  MU_DCR(MISC_INTERRUPTS__MACHINE_CHECK),            decode_misc_interrupts,          0                     },
    {  MU_DCR(MMREGS_INTERRUPTS__MACHINE_CHECK),          decode_mmregs_interrupts,        0                     },
    {  MU_DCR(RCSRAM_INTERRUPTS__MACHINE_CHECK),          decode_rcsram_interrupts,        0                     },
    {  MU_DCR(RME_INTERRUPTS0__MACHINE_CHECK),            decode_rme_interrupts0,          getRmeInterruptsInfo  },
    {  MU_DCR(RME_INTERRUPTS1__MACHINE_CHECK),            decode_rme_interrupts1,          getRmeInterruptsInfo  },
    {  MU_DCR(RMU_ECC_CORR_INTERRUPTS__MACHINE_CHECK),    decode_rmu_ecc_corr_interrupts,  0                     },
    {  MU_DCR(RMU_ECC_INTERRUPTS__MACHINE_CHECK),         decode_rmu_ecc_interrupts,       0                     },
    {  MU_DCR(RPUTSRAM_INTERRUPTS__MACHINE_CHECK),        decode_rputsram_interrupts,      0                     },
    {  MU_DCR(SLAVE_PORT_INTERRUPTS__MACHINE_CHECK),      decode_slave_port_interrupts,    getSlavePortInfo      },

    {  MU_DCR(ARLOG_INTERRUPTS__STATE),           decode_arlog_interrupts,         0                     },
    {  MU_DCR(ICSRAM_INTERRUPTS__STATE),          decode_icsram_interrupts,        0                     },
    {  MU_DCR(MCSRAM_INTERRUPTS__STATE),          decode_mcsram_interrupts,        0         },
    {  MU_DCR(RCSRAM_INTERRUPTS__STATE),          decode_rcsram_interrupts,        0                     },
    {  MU_DCR(RPUTSRAM_INTERRUPTS__STATE),        decode_rputsram_interrupts,      0                     },
    {  MU_DCR(MISC_ECC_CORR_INTERRUPTS__STATE),   decode_misc_ecc_corr_interrupts, 0                     },
    {  MU_DCR(RME_INTERRUPTS0__STATE),            decode_rme_interrupts0,          0  },
    {  MU_DCR(RME_INTERRUPTS1__STATE),            decode_rme_interrupts1,          0  },


  };

  /*
   * +-----------------------------------------------------------------------------------+
   * | NOTE: This is a common decoder for all of the MU RAS events.  It not only decodes |
   * |       the various interrupt status registers, but also collects the extraneous    |
   * |       data produced by the machine check handler (e.g. intinfo data or other data |
   * |       produced by custom handler logic.                                           |
   * +-----------------------------------------------------------------------------------+
   */

  void fw_MU_machineCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

    /*
     * The argument list is actually a list of pairs (DCR address, DCR value).
     * Locate any known DCRs and decode them.
     */

    ostringstream details;

    for ( unsigned i = 0; i < mbox.size(); i++ ) {
      
      for ( unsigned j = 0; j < sizeof(MU_DECODERS)/sizeof(MU_DECODERS[0]); j++ ) {

	//TRACE(("[%s:%d] comparing arg=%lX to %lx=decode\n", __func__, __LINE__, mbox[i], MU_DECODERS[j].dcrAddress));

	/* -------------------------------------------------------------------------------
	 * If the argument matches a given interrupt status DCR address, then
	 * invoke the corresponding decoder.  Also, if there is an additional details
	 * function, invoke it.
	 * ------------------------------------------------------------------------------- */

	if ( mbox[i] == MU_DECODERS[j].dcrAddress ) {

	  MU_DECODERS[j].decoder( details, mbox[++i] );

	  if ( MU_DECODERS[j].additionalDetails != NULL ) {
	    MU_DECODERS[j].additionalDetails( details, mbox );
	  }

	  break;
	}
      }
    }

    event.setDetail( "DETAILS", details.str() );
  }

  /**
   * @brief Locates the specified DCRs data within the mailbox vector and
   *   appends it's data to the running commentary (details).
   */

  void scrapeDCR( uint32_t dcrAddress, const std::string& label, ostringstream& details, const vector<uint64_t>& mbox ) {
    for ( unsigned i = 0; i < mbox.size(); i++ ) {
      if ( mbox[i] == dcrAddress ) {
	details
	  << " " << label << "="
	  << hex << mbox[++i]
	  ;
      }
    }
  }

  static uint32_t ECC_COUNTERS[] = {
    MU_DCR(ARLOG_ECC_COUNT),
    MU_DCR(ICSRAM_ECC_COUNT),
    MU_DCR(MCSRAM_ECC_COUNT),
    MU_DCR(RCSRAM_ECC_COUNT),
    MU_DCR(RPUT_ECC_COUNT),
    MU_DCR(MMREGS_ECC_COUNT),
    MU_DCR(XS_ECC_COUNT),
    MU_DCR(RME_ECC_COUNT)
  };

  static std::string ECC_NAMES[] = {
    "ARLOG_ECC_COUNT",
    "ICSRAM_ECC_COUNT",
    "MCSRAM_ECC_COUNT",
    "RCSRAM_ECC_COUNT",
    "RPUT_ECC_COUNT",
    "MMREGS_ECC_COUNT",
    "XS_ECC_COUNT",
    "RME_ECC_COUNT"
  };
    

  /**
   * @brief Searches the mailbox data, scraping up any ECC counters.
   */
  void getEccCountInfo( ostringstream& details, const vector<uint64_t>& mbox ) { 

    scrapeDCR( MU_DCR(ECC_INT_THRESH), "ECC_INT_THRESH", details, mbox );

      for ( unsigned j = 0; j < sizeof(ECC_COUNTERS)/sizeof(ECC_COUNTERS[0]); j++ ) {
	scrapeDCR( ECC_COUNTERS[j], ECC_NAMES[j], details, mbox );
      }
  }

  /**
   * @brief Searches the mailbox data for IME_P_ERR* data.
   */

  void getImeInfo( ostringstream& details, const vector<uint64_t>& mbox ) {
    
    for ( unsigned i = 0; i <= 15; i++ ) {

      ostringstream label;
      label << "IME_P_ERR(" << i << ")";

      scrapeDCR( MU_DCR(IME_P_ERR0) + i, label.str(), details, mbox );
    }
  }

  void getMcsramInfo( ostringstream& details, const vector<uint64_t>& mbox ) {
    scrapeDCR( MU_DCR(MCSRAM_ERR_FIFO_ID), "MCSRAM_ERR_FIFO_ID", details, mbox );
  }

  const uint32_t RME_P_ERR[] = {
    MU_DCR(RME_P_ERR0),
    MU_DCR(RME_P_ERR1),
    MU_DCR(RME_P_ERR2),
    MU_DCR(RME_P_ERR3),
    MU_DCR(RME_P_ERR4),
    MU_DCR(RME_P_ERR5),
    MU_DCR(RME_P_ERR6),
    MU_DCR(RME_P_ERR7),
    MU_DCR(RME_P_ERR8),
    MU_DCR(RME_P_ERR9),
    MU_DCR(RME_P_ERR10),
    MU_DCR(RME_P_ERR11),
    MU_DCR(RME_P_ERR12),
    MU_DCR(RME_P_ERR13),
    MU_DCR(RME_P_ERR14),
    MU_DCR(RME_P_ERR15)
  };

  /**
   * @brief Searches the mailbox for any RME_P_ERR* data.
   */

  void getRmeInterruptsInfo( ostringstream& details, const vector<uint64_t>& mbox ) {

    for ( unsigned i = 0; i < sizeof(RME_P_ERR) / sizeof(RME_P_ERR[0]); i++ ) {
      ostringstream label;
      label << "RME_P_ERR(" << i  << ")";
      scrapeDCR( RME_P_ERR[i], label.str(), details, mbox );
    }
  }
  
void getSlavePortInfo( ostringstream& details, const vector<uint64_t>& mbox ) {
    scrapeDCR( MU_DCR(XS_ERR_INFO), "XS_ERR_INFO", details, mbox );
  }


    void decode_ND_RESE_FATAL_ERR0( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 0 );
    }
    void decode_ND_RESE_FATAL_ERR1( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 1 );
    }
    void decode_ND_RESE_FATAL_ERR2( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 2 );
    }
    void decode_ND_RESE_FATAL_ERR3( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 3 );
    }
    void decode_ND_RESE_FATAL_ERR4( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 4 );
    }
    void decode_ND_RESE_FATAL_ERR5( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 5 );
    }
    void decode_ND_RESE_FATAL_ERR6( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 6 );
    }
    void decode_ND_RESE_FATAL_ERR7( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 7 );
    }
    void decode_ND_RESE_FATAL_ERR8( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 8 );
    }
    void decode_ND_RESE_FATAL_ERR9( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 9 );
    }
    void decode_ND_RESE_FATAL_ERR10( ostringstream& str, uint64_t status ) {
	decode_ND_RESE_DCR_FATAL_ERR( str, status, 10 );
    }

MU_Decoder MU_TERMCHECKS[] = {
    {  MU_DCR(MASTER_PORT0_INTERRUPTS__STATE),    decode_master_port0_interrupts,  0 },
    {  MU_DCR(MASTER_PORT1_INTERRUPTS__STATE),    decode_master_port1_interrupts,  0 },
    {  MU_DCR(MASTER_PORT2_INTERRUPTS__STATE),    decode_master_port2_interrupts,  0 },
    {  MU_DCR(SLAVE_PORT_INTERRUPTS__STATE),      decode_slave_port_interrupts,    0 },
    {  MU_DCR(MMREGS_INTERRUPTS__STATE),          decode_mmregs_interrupts,        0 },
    {  MU_DCR(MCSRAM_INTERRUPTS__STATE),          decode_mcsram_interrupts,        0 },
    {  MU_DCR(RCSRAM_INTERRUPTS__STATE),          decode_rcsram_interrupts,        0 },
    {  MU_DCR(RPUTSRAM_INTERRUPTS__STATE),        decode_rputsram_interrupts,      0 },
    {  MU_DCR(RME_INTERRUPTS0__STATE),            decode_rme_interrupts0,          0 },
    {  MU_DCR(RME_INTERRUPTS1__STATE),            decode_rme_interrupts1,          0 },
    {  MU_DCR(ICSRAM_INTERRUPTS__STATE),          decode_icsram_interrupts,        0 },
    {  MU_DCR(MISC_INTERRUPTS__STATE),            decode_misc_interrupts,          0 },
    {  ND_500_DCR(FATAL_ERR0),                    decode_ND_500_DCR_FATAL_ERR0,    0 },
    {  ND_500_DCR(FATAL_ERR1),                    decode_ND_500_DCR_FATAL_ERR1,    0 },
    {  ND_500_DCR(FATAL_ERR2),                    decode_ND_500_DCR_FATAL_ERR2,    0 },
    {  ND_500_DCR(FATAL_ERR3),                    decode_ND_500_DCR_FATAL_ERR3,    0 },
    {  ND_500_DCR(FATAL_ERR4),                    decode_ND_500_DCR_FATAL_ERR4,    0 },
    {  ND_500_DCR(FATAL_ERR5),                    decode_ND_500_DCR_FATAL_ERR5,    0 },
    {  ND_X2_DCR(FATAL_ERR0),                     decode_ND_X2_DCR_FATAL_ERR0,     0 },
    {  ND_X2_DCR(FATAL_ERR1),                     decode_ND_X2_DCR_FATAL_ERR1,     0 },
    {  ND_RESE_DCR(0,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR0,       0 },
    {  ND_RESE_DCR(1,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR1,       0 },
    {  ND_RESE_DCR(2,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR2,       0 },
    {  ND_RESE_DCR(3,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR3,       0 },
    {  ND_RESE_DCR(4,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR4,       0 },
    {  ND_RESE_DCR(5,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR5,       0 },
    {  ND_RESE_DCR(6,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR6,       0 },
    {  ND_RESE_DCR(7,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR7,       0 },
    {  ND_RESE_DCR(8,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR8,       0 },
    {  ND_RESE_DCR(9,FATAL_ERR),                  decode_ND_RESE_FATAL_ERR9,       0 },
    {  ND_RESE_DCR(10,FATAL_ERR),                 decode_ND_RESE_FATAL_ERR10,      0 },

  };

  void fw_MU_termCheckDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

      if ( mbox.size() < 4 ) {
	  event.setDetail( "DETAILS", "INTERNAL ERROR: missing details" );
	  return;
      }

      uint64_t dcrAddress = mbox[0];
      uint64_t actual = mbox[1];
      uint64_t expected = mbox[2];
      uint64_t mask = mbox[3];
      bool found = false;

      ostringstream details;

      for ( unsigned i = 0; i < sizeof(MU_TERMCHECKS)/sizeof(MU_TERMCHECKS[0]); i++ ) {
	  if ( dcrAddress == MU_TERMCHECKS[i].dcrAddress ) {
	      MU_TERMCHECKS[i].decoder( details, actual & mask );
	      found = true;
	  }
      }

      if ( ! found ) {
	  details <<
	      " DCR=" << hex << dcrAddress <<
	      " Actual=" << hex << actual <<
	      " Expected=" << hex << expected <<
	      " Mask=" << hex << mask
	      ;
      }

      event.setDetail( "DETAILS", details.str() );
  }

  void fw_MU_eccSummaryDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

    /*
     * The argument list is actually a list of pairs (DCR address, DCR value).
     * Locate any known DCRs and decode them.
     */

    ostringstream details;

    int count = 0;

    for ( unsigned i = 0; i < mbox.size(); i++ ) {
	for ( unsigned j = 0; j < sizeof(ECC_COUNTERS)/sizeof(ECC_COUNTERS[0]); j++ ) {
	    if ( mbox[i] == ECC_COUNTERS[j] ) {
		count += mbox[++i];
	    }
	}
    }

    details << "count=" << count << " ";

    for ( unsigned i = 0; i < mbox.size(); i++ ) {
      
      for ( unsigned j = 0; j < sizeof(MU_DECODERS)/sizeof(MU_DECODERS[0]); j++ ) {

	//TRACE(("[%s:%d] comparing arg=%lX to %lx=decode\n", __func__, __LINE__, mbox[i], MU_DECODERS[j].dcrAddress));

	/* -------------------------------------------------------------------------------
	 * If the argument matches a given interrupt status DCR address, then
	 * invoke the corresponding decoder.  Also, if there is an additional details
	 * function, invoke it.
	 * ------------------------------------------------------------------------------- */

	if ( mbox[i] == MU_DECODERS[j].dcrAddress ) {

	  MU_DECODERS[j].decoder( details, mbox[++i] );

	  if ( MU_DECODERS[j].additionalDetails != NULL ) {
	    MU_DECODERS[j].additionalDetails( details, mbox );
	  }

	  break;
	}
      }
    }

    event.setDetail( "DETAILS", details.str() );

    ostringstream countStr;
    countStr << count;
    event.setDetail( "BG_COUNT", countStr.str() );

  }

}

