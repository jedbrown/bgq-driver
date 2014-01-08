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

#include <hwi/include/bqc/mu_dcr.h>

#define CAPTURE_IME_INTERRUPTS  -1
#define CAPTURE_RME_INTERRUPTS0 -2
#define CAPTURE_RME_INTERRUPTS1 -3


#include <stdio.h>

void test_master_port0_interrupts(void);
void test_master_port1_interrupts(void);
void test_arlog_interrupts(void);
void test_master_port2_interrupts(void);
void test_ecc_count_interrupts(void);
void test_imu_ecc_interrupts(void);
void test_misc_ecc_corr_interrupts(void);
void test_rcsram_interrupts(void);
void test_mmregs_interrupts(void);
void test_rmu_ecc_interrupts(void);
void test_mcsram_interrupts(void);
void test_ime_interrupts(void);
void test_rme_interrupts1(void);
void test_rme_interrupts0(void);
void test_icsram_interrupts(void);
void test_rputsram_interrupts(void);
void test_slave_port_interrupts(void);
void test_rmu_ecc_corr_interrupts(void);
void test_misc_interrupts(void);
void test_fifo_interrupts(void);

void test_DCR_address_assumptions(void);

int test_main( void ) {


  if ( ProcessorID() == 0 ) {

    test_DCR_address_assumptions();

    test_master_port0_interrupts();
    test_master_port1_interrupts();
    test_arlog_interrupts();
    test_master_port2_interrupts();
    test_ecc_count_interrupts();
    test_imu_ecc_interrupts();
    test_misc_ecc_corr_interrupts();
    test_rcsram_interrupts();
    test_mmregs_interrupts();
    test_rmu_ecc_interrupts();
    test_mcsram_interrupts();
    test_ime_interrupts();
    test_rme_interrupts1();
    test_rme_interrupts0();
    test_icsram_interrupts();
    test_rputsram_interrupts();
    test_slave_port_interrupts();
    test_rmu_ecc_corr_interrupts();
    test_misc_interrupts();
    test_fifo_interrupts();

  }
  exit(0);
}



