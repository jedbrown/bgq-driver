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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <firmware/include/Firmware.h>
#include <firmware/include/fwext/fwext.h>

#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>

//#include "../libfwext/fwext_lib.h"

#include <hwi/include/bqc/serdes_left_dcr.h>
#include <hwi/include/bqc/serdes_right_dcr.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>

#include <firmware/include/fwext/serdes.h>
#include <firmware/include/fwext/sd_helper.h>
#include <firmware/include/fwext/fwext_nd.h>

#include <firmware/include/fwext/sd_ei.h>


#define LEFT_MIN  0xFF
#define LEFT_MAX  0x23
#define RIGHT_MIN 0xAA
#define RIGHT_MAX 0x00

uint32_t sd_ei_link_enable[ND_RESE_DCR_num]    = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1};
uint32_t sd_ei_error_thresh[ND_RESE_DCR_num]   = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
uint32_t sd_ei_retrans_thresh[ND_RESE_DCR_num] = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1};

uint32_t sd_ei_left_min  = LEFT_MIN;
uint32_t sd_ei_left_max  = LEFT_MAX;
uint32_t sd_ei_right_min = RIGHT_MIN;
uint32_t sd_ei_right_max = RIGHT_MAX;

uint64_t sd_ei_start_time = 0; 
uint64_t sd_ei_stop_time  = 0;
uint64_t sd_ei_duration   = 0;

uint8_t sd_ei_data = 0;

int map(int sender) {
  if (sender == 10) return sender;
  else return sender ^ 1;
}

void sd_ei_init(const uint32_t link_enable[ND_RESE_DCR_num],
		const uint32_t error_thresh[ND_RESE_DCR_num],
		const uint32_t retrans_thresh[ND_RESE_DCR_num],
		uint32_t rcv_ack) {
  unsigned int i;

  //  printf(" (I) Initialize Serdes Error Injection\n");

  for (i = 0; i < ND_RESE_DCR_num; i++) {
    //    printf(" (D) %3X, %3X, %3X\n", link_enable[i], error_thresh[i], retrans_thresh[i]);
    sd_ei_link_enable[i]    = link_enable[i];
    sd_ei_error_thresh[i]   = error_thresh[i];
    sd_ei_retrans_thresh[i] = retrans_thresh[i];
  }
  // Set IO Link into loopback mode.
  //  DCRWritePriv(SERDES_RIGHT_DCR(TS_CTL_IO), SERDES_RIGHT_DCR__TS_CTL_IO__LOOPBACK_EN_set(1));

  if(rcv_ack == 0) {
    for (i = 0; i < ND_RESE_DCR_num; i++) {
      DCRWritePriv(ND_RESE_DCR(i, CTRL), ND_RESE_DCR__CTRL__SE_LINK_STOP_set(1)); // just to beautify the AET - no functional impact, afaik.
    }
  }
  sd_ei_data = rcv_ack;
}

void sd_ei_set_interval(uint32_t left_min, uint32_t left_max, uint32_t right_min, uint32_t right_max) {
  //  printf(" (I) Setting Serdes Error Injection Intervals\n");
  sd_ei_left_min  = left_min;
  sd_ei_left_max  = left_max;
  sd_ei_right_min = right_min;
  sd_ei_right_max = right_max;
}

void sd_ei_reset() {
  //  printf (" (I) Resetting error/retransmission counters\n");
  uint64_t dcr_clear = 0;
  unsigned int i     = 0;
  dcr_clear |= ND_RESE_DCR__CLEAR__SE_CLR_RETRANS_CNT_set(1);
  dcr_clear |= ND_RESE_DCR__CLEAR__RE_CLR_LINK_ERR_CNT_set(1);
  
  for (i = 0; i < ND_RESE_DCR_num; i++) {
    DCRWritePriv(ND_RESE_DCR(i, CLEAR), dcr_clear);
  }

  dcr_clear = 0;

  for (i = 0; i < ND_RESE_DCR_num; i++) {
    DCRWritePriv(ND_RESE_DCR(i, CLEAR), dcr_clear);
  }
}

void sd_ei_activate() { // rcv_ack) {

  uint32_t link_sel               = 0;
  unsigned int i                  = 0;
  uint64_t error_inject_control   = 0;
  uint64_t error_inject_timer_val = 0;
  uint64_t error_inject_limit     = 0;

  //  printf(" (I) Activate Serdes Error Injection\n");

  for (i = 0; i < ND_RESE_DCR_num; i++) {
    //    printf(" (D) %3X, %3X, %3X\n", sd_ei_link_enable[i], sd_ei_error_thresh[i], sd_ei_retrans_thresh[i]);
    DCRWritePriv(ND_RESE_DCR(i, THRESH_RE_ERR), sd_ei_error_thresh[i]);
    DCRWritePriv(ND_RESE_DCR(i, THRESH_SE_RETRAN), sd_ei_retrans_thresh[i]);
  }
  
  /**** Configure Serdes ****/
  // Set the initial error injection rate
  error_inject_timer_val |= SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__COUNT_set(100);
  error_inject_timer_val |= SERDES_LEFT_DCR__ERROR_INJECT_TIMER_VAL__RND_set(0xFFFF);
  DCRWritePriv(SERDES_LEFT_DCR(ERROR_INJECT_TIMER_VAL), error_inject_timer_val);
  
  error_inject_timer_val = 0;
  error_inject_timer_val |= SERDES_RIGHT_DCR__ERROR_INJECT_TIMER_VAL__COUNT_set(100);
  error_inject_timer_val |= SERDES_RIGHT_DCR__ERROR_INJECT_TIMER_VAL__RND_set(0xFFFF);
  DCRWritePriv(SERDES_RIGHT_DCR(ERROR_INJECT_TIMER_VAL), error_inject_timer_val);
  
  // Set min/max error injection rate
  error_inject_limit |= SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__RND_MASK_set(sd_ei_left_max);
  error_inject_limit |= SERDES_LEFT_DCR__ERROR_INJECT_LIMIT__LOW_LIMIT_set(sd_ei_left_min);
  DCRWritePriv(SERDES_LEFT_DCR(ERROR_INJECT_LIMIT), error_inject_limit);
  
  error_inject_limit = 0;
  error_inject_limit |= SERDES_RIGHT_DCR__ERROR_INJECT_LIMIT__RND_MASK_set(sd_ei_right_max);
  error_inject_limit |= SERDES_RIGHT_DCR__ERROR_INJECT_LIMIT__LOW_LIMIT_set(sd_ei_right_min);
  DCRWritePriv(SERDES_RIGHT_DCR(ERROR_INJECT_LIMIT), error_inject_limit);

  // enable error injection (T0-T5) 
  link_sel |= sd_ei_link_enable[0] << 5;
  link_sel |= sd_ei_link_enable[1] << 4;
  link_sel |= sd_ei_link_enable[2] << 3;
  link_sel |= sd_ei_link_enable[3] << 2;
  link_sel |= sd_ei_link_enable[4] << 1;
  link_sel |= sd_ei_link_enable[5] << 0;
  
  error_inject_control |= SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__ENABLE_set(1);
  error_inject_control |= SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__LINK_SELECT_set(link_sel);
  // The first byte should not be 55, 5A, 66, 99 or CC. Idle Data is AA or 69, so EF is a good mask.
  error_inject_control |= SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__MASK_set(0xEFFFFFFF); // semi-random choice.
  DCRWritePriv(SERDES_LEFT_DCR(ERROR_INJECT_CONTROL), error_inject_control);
  
  link_sel  = 0;
  link_sel |= sd_ei_link_enable[6] << 4;
  link_sel |= sd_ei_link_enable[7] << 3;
  link_sel |= sd_ei_link_enable[8] << 2;
  link_sel |= sd_ei_link_enable[9] << 1;
  link_sel |= sd_ei_link_enable[10] << 0;
  
  // enable error injection (T6-T9, TIO)
  error_inject_control = 0;
  error_inject_control |= SERDES_RIGHT_DCR__ERROR_INJECT_CONTROL__ENABLE_set(1);
  error_inject_control |= SERDES_RIGHT_DCR__ERROR_INJECT_CONTROL__LINK_SELECT_set(link_sel);
  error_inject_control |= SERDES_RIGHT_DCR__ERROR_INJECT_CONTROL__MASK_set(0xEF123456); // random choice
  DCRWritePriv(SERDES_RIGHT_DCR(ERROR_INJECT_CONTROL), error_inject_control);

  sd_ei_start_time = GetTimeBase();
}

void sd_ei_deactivate() {
  uint64_t error_inject_control = 0;

  uint32_t networkFrequency     = 0;
  uint32_t coreFrequency        = 0;

  sd_get_frequencies(&coreFrequency, &networkFrequency);

  sd_ei_stop_time = GetTimeBase();
  sd_ei_duration = (((sd_ei_stop_time - sd_ei_start_time)*500)/coreFrequency); //-720; // 720 seems to be some delay with DCR operations - I don't really know!

  // disable error injection (T0-T5)
  error_inject_control = 0;
  error_inject_control |= SERDES_LEFT_DCR__ERROR_INJECT_CONTROL__ENABLE_set(0);
  DCRWritePriv(SERDES_LEFT_DCR(ERROR_INJECT_CONTROL), error_inject_control);
  
  // disable error injection (T6-T9, TIO)
  error_inject_control = 0;
  error_inject_control |= SERDES_RIGHT_DCR__ERROR_INJECT_CONTROL__ENABLE_set(0);
  DCRWritePriv(SERDES_RIGHT_DCR(ERROR_INJECT_CONTROL), error_inject_control);
  //  printf(" (I) Deactivated Serdes Error Injection\n");
}

// returns recevier error count for link
uint64_t sd_get_error_count(uint32_t link) {
  if (link < ND_RESE_DCR_num) {
    return ND_RESE_DCR__RE_LINK_ERR_CNT__VALUE_get(DCRReadPriv(ND_RESE_DCR(link,RE_LINK_ERR_CNT)));
  } else {
    return 0xDEADBEEF;
  }
}

// return sender retransmission count for link
uint64_t sd_get_retrans_count(uint32_t link) {
  if (link < ND_RESE_DCR_num) {
    return ND_RESE_DCR__SE_RETRANS_CNT__VALUE_get(DCRReadPriv(ND_RESE_DCR(link,SE_RETRANS_CNT)));
  } else {
    return 0xDEADBEEF;
  }
}

uint32_t sd_ei_check() {
  uint32_t error                  = 0;

  uint64_t error_count            = 0;
  uint64_t retrans_count          = 0;
  uint64_t fatal_error            = 0;
  
  uint64_t min_error_count        = 0;
  uint64_t max_error_count        = 0;
  uint64_t min_retrans_count      = sd_ei_data;
  uint64_t max_retrans_count      = 0xFFFFFFFFFFFFFFFF;
  unsigned int i                  = 0;

  
  //  printf(" (I) Checking Serdes Error Injection\n");

  for(i = 0; i < ND_RESE_DCR_num; i++) {
    //    printf(" (I) Checking link %i...\n", i);
    if (i <=5 ) {
      min_error_count = ((sd_ei_duration-720)/(sd_ei_left_min+sd_ei_left_max))*146*sd_ei_link_enable[map(i)];
      max_error_count = (sd_ei_duration/sd_ei_left_min)*146*sd_ei_link_enable[map(i)];
    } else {
      min_error_count = ((sd_ei_duration-720)/(sd_ei_right_min+sd_ei_right_max))*146*sd_ei_link_enable[map(i)];
      max_error_count = (sd_ei_duration/sd_ei_right_min)*146*sd_ei_link_enable[map(i)];
    }

    // adapt min/max values for uncertanties
    min_error_count -= min_error_count < 0xA ? 0 : 0xA;
    max_error_count += max_error_count > 0 ? 0xA : 0;
    
    error_count   = ND_RESE_DCR__RE_LINK_ERR_CNT__VALUE_get(DCRReadPriv(ND_RESE_DCR(i,RE_LINK_ERR_CNT)));
    retrans_count = ND_RESE_DCR__SE_RETRANS_CNT__VALUE_get(DCRReadPriv(ND_RESE_DCR(i,SE_RETRANS_CNT)));
    fatal_error   = DCRReadPriv(ND_RESE_DCR(i,FATAL_ERR));

    if (sd_ei_data == 0) { // I don't know any reliablye min/max values while sending data
      if (error_count < min_error_count) { error |= ERROR_ERR_LOWER_LIMIT_UNDERCUT; }
      if (error_count > max_error_count) { error |= ERROR_ERR_UPPER_LIMIT_EXCEEDED; }
    } else {
      min_retrans_count = 0;
      max_retrans_count = 0;
    }

    if (min_error_count > error_count || error_count > max_error_count) {
      printf(" (E) Error count (link %X) : 0x%llX <= 0x%llX <= 0x%llX \n", i,
	     (long long int)min_error_count,
	     (long long int)error_count,
	     (long long int)max_error_count);
    }
    
    if (retrans_count < min_retrans_count || retrans_count > max_retrans_count) {
      printf(" (E) Retransmission count (link %X) : 0x%llX <= 0x%llX \n", i,
	     (long long int)min_retrans_count,
	     (long long int)retrans_count);
      error |= ERROR_RET_LOWER_LIMIT_UNDERCUT;
    } else if (sd_ei_data == 0) {
      //      printf(" (I) Retransmission count (link %X) : 0x%llX <= 0x%llX \n", i,
      //	     (long long int)min_retrans_count,
      //	     (long long int)retrans_count);
    }
    
    if (sd_ei_link_enable[i] == 1 && retrans_count >= sd_ei_retrans_thresh[i]) {
      // expect retrans interrupt
      if(ND_RESE_DCR__FATAL_ERR__SE_RETRANS_CNT_OVER_THRESH_get(fatal_error) == 0) {
	error |= ERROR_RET_MISSING_IRQ;
	printf(" (E) Missing retrans-over-thresh interrupt! (link %X) %i\n", i, sd_ei_retrans_thresh[i]);
      }
    } else {
      // expect NO retrans interrupt
      if(ND_RESE_DCR__FATAL_ERR__SE_RETRANS_CNT_OVER_THRESH_get(fatal_error) != 0) {
	error |= ERROR_RET_UNEXPECTED_IRQ;
	printf(" (E) Unexpected retrans-over-thresh interrupt! (link %X) %i\n", i, sd_ei_retrans_thresh[i]);
      }
    }

    // mapping required because errors are injected in another link as where they are counted.
    if (error_count >= sd_ei_error_thresh[i]) {
      // expect error interrupt
      if(ND_RESE_DCR__FATAL_ERR__RE_LINK_ERR_OVER_THRESH_get(fatal_error) == 0) {
	error |= ERROR_ERR_MISSING_IRQ;
	printf(" (E) Missing error-over-thresh interrupt! (link %X) %i\n", i, sd_ei_error_thresh[i]);
      }
    } else {
      // expect NO error interrupt
      if(ND_RESE_DCR__FATAL_ERR__RE_LINK_ERR_OVER_THRESH_get(fatal_error) != 0) {
	error |= ERROR_ERR_UNEXPECTED_IRQ;
	printf(" (E) Unexpected error-over-thresh interrupt! (link %X) %i\n", i, sd_ei_error_thresh[i]);
      }
    }
  }

  /* Clear DCR registers and check that they stay clear, i.e. that no more errors are counted */
  uint64_t dcr_clear = 0;
  dcr_clear |= ND_RESE_DCR__CLEAR__SE_CLR_RETRANS_CNT_set(1);
  dcr_clear |= ND_RESE_DCR__CLEAR__RE_CLR_LINK_ERR_CNT_set(1);
  
  for (i = 0; i < ND_RESE_DCR_num; i++) {
    DCRWritePriv(ND_RESE_DCR(i, CLEAR), dcr_clear);
  }
  
  for (i = 0; i < ND_RESE_DCR_num; i++) {
    error_count   = ND_RESE_DCR__RE_LINK_ERR_CNT__VALUE_get(DCRReadPriv(ND_RESE_DCR(i,RE_LINK_ERR_CNT)));
    retrans_count = ND_RESE_DCR__SE_RETRANS_CNT__VALUE_get(DCRReadPriv(ND_RESE_DCR(i,SE_RETRANS_CNT)));
    fatal_error   = DCRReadPriv(ND_RESE_DCR(i,FATAL_ERR));
    
    if (error_count != 0) {
      printf(" (E) Clear on error_count did not work (link %X)\n", i);
      error |= ERROR_ERR_CLEAR;
    }
    if (retrans_count != 0) {
      printf(" (E) Clear on retrans_count did not work (link %X)\n", i);
      error |= ERROR_RET_CLEAR;
    }
    if (ND_RESE_DCR__FATAL_ERR__SE_RETRANS_CNT_OVER_THRESH_get(fatal_error) != 0) {
      printf(" (E) Clear on SE_RETRANS_CNT_OVER_THRESH did not work (link %X)\n", i);
      error |= ERROR_RET_CLEAR;
    }
    if (ND_RESE_DCR__FATAL_ERR__RE_LINK_ERR_OVER_THRESH_get(fatal_error) != 0) {
      printf(" (E) Clear on RE_LINK_ERR_OVER_THRESH did not work (link %X)\n", i);
      error |= ERROR_ERR_CLEAR;
    }
  }
  return error;
}

