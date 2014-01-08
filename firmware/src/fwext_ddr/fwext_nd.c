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
/* ---------------------------------------------------------
This a a library to release the network from reset correctly
and to do basic network termination checks at the end of a run
The initial version supports single node,(with all coordinates 0)
in internal loopback, which is sufficient for early full chip tests
Eventually, a somewhat modified version should evolve into the firmware.


For an example of how to link and use this code see:
bgq/hwverify/src/fullchip/mu_nd/nd_reset_test1

In the Makefile of your test directory, ensure you have

LOCAL_LIBS += $(BGQ_BUILD_DIR)/hwverify/src/fullchip/libfw_nd/libfw_nd.a
FWEXT_INCLUDES += -I$(BGQ_BUILD_DIR)/hwverify/src/fullchip
SUBDIRS=../../libfwext ../../libfw_nd  (or relative path to bgq/hwverify/src/fullchip/libfw_nd


your testcase code should then #include <firmware/include/fwext/fwext_nd.h> 
see nd_reset_test1/test_main.c for sample use

-----------------------------------------------------*/

 
#include <firmware/include/fwext/fwext.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>

#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/fwext_nd.h>
#include "Firmware_internals.h"

int  FW_ND_VERBOSE=0;

// These defines are no longer produced by the generated DCR headers.

#define ND_500_NUM_FATAL_ERR_DCRs   6
#define ND_500_NUM_RCP_FIFO_P_DCRs  4
#define ND_500_NUM_INJ_FIFO_P_DCRs  2
#define ND_500_NUM_STAT_DCRs        10
#define ND_X2_NUM_STAT_DCRs         2


int fw_mailbox_barrier( fw_uint64_t arg ) {
  return fwext_getFwInterface()->barrier(arg);
}



int fw_nd_reset_release(Personality_t *p)
   
{

#if 0

  //  +-------------------------------------------------------------------+
  //  |  NOTE: ND initialization is now in firmware but is gated by the   |
  //  |        enable bit in the personality.  So we test that bit here,  |
  //  |        which will catch any tests that have failed to enable the  |
  //  |        ND in their personality (svchost file).                    |
  //  +-------------------------------------------------------------------+

  if ( ( p->Kernel_Config.NodeConfig & PERS_ENABLE_ND ) == 0 ) {
    printf("(E) ND is not enabled.\n");
    Terminate(-1);
  }
#endif

  int rc = 0;

  if ( ( p->Kernel_Config.NodeConfig & PERS_ENABLE_ND ) == 0 ) {

    //printf(">>>>>>>>>>>>>>>>>>>>>>>FORCING ND INIT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    extern  int fw_nd_resetRelease( void );

    p->Kernel_Config.NodeConfig |= PERS_ENABLE_ND;
    rc = fw_nd_resetRelease();
  }

  return rc;
}


int fw_nd_term_check(Personality_t *p)
  
{

#if 0
  if ( ( p->Kernel_Config.NodeConfig & PERS_ENABLE_ND ) == 0 ) {
    printf("(E) ND is not enabled.\n");
    Terminate(-1);
  }
#endif

  int rc = 0;

  if ( ( p->Kernel_Config.NodeConfig & PERS_ENABLE_ND ) == 0 ) {

    extern  int fw_nd_termCheck( void );

    p->Kernel_Config.NodeConfig |= PERS_ENABLE_ND;
    rc = fw_nd_termCheck();
    p->Kernel_Config.NodeConfig &= ~PERS_ENABLE_ND;
  }
  else {
    rc = fwext_termCheck();
  }

  return rc;
}

extern inline uint64_t fw_nd_get_is_torus_bitmask(Personality_t *p)
{
  return  ND_GET_ALL_TORUS(p->Network_Config.NetFlags);
  
}  
// NOTE:  these come in from the testint, and should be completely
// invisible to software, including setting the network DCRs
// putting this here in case it is ever needed
// these indicate whether the card wiring for each dimension swaps the plus
// and minus directions
extern inline void fw_nd_get_swaps(Personality_t *p,int *swaps)
{
  int i;
  uint64_t swapBits = TI_GetDimensionSwaps();
  for (i=0;i<5;i++) 
    swaps[i] =  (swapBits & (0x10 >> i) ) ? 1 : 0;
}

#define _BGQ_FW_ND_DIR_A_MINUS  0
#define _BGQ_FW_ND_DIR_A_PLUS   1
#define _BGQ_FW_ND_DIR_B_MINUS  2
#define _BGQ_FW_ND_DIR_B_PLUS   3
#define _BGQ_FW_ND_DIR_C_MINUS  4
#define _BGQ_FW_ND_DIR_C_PLUS   5
#define _BGQ_FW_ND_DIR_D_MINUS  6
#define _BGQ_FW_ND_DIR_D_PLUS   7
#define _BGQ_FW_ND_DIR_E_MINUS  8
#define _BGQ_FW_ND_DIR_E_PLUS   9
#define _BGQ_FW_ND_DIR_IO       10

extern inline int fw_nd_get_enable_sender(uint64_t dir, Personality_t *p)
{
  int rc = 0;
  switch(dir)
  {
  case _BGQ_FW_ND_DIR_A_MINUS: if ( ND_ENABLE_LINK_A_MINUS & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_A_PLUS:  if ( ND_ENABLE_LINK_A_PLUS  & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_B_MINUS: if ( ND_ENABLE_LINK_B_MINUS & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_B_PLUS:  if ( ND_ENABLE_LINK_B_PLUS  & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_C_MINUS: if ( ND_ENABLE_LINK_C_MINUS & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_C_PLUS:  if ( ND_ENABLE_LINK_C_PLUS  & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_D_MINUS: if ( ND_ENABLE_LINK_D_MINUS & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_D_PLUS:  if ( ND_ENABLE_LINK_D_PLUS  & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_E_MINUS: if ( ND_ENABLE_LINK_E_MINUS & p->Network_Config.NetFlags2 ) rc = 1; break;
  case _BGQ_FW_ND_DIR_E_PLUS:  if ( ND_ENABLE_LINK_E_PLUS  & p->Network_Config.NetFlags2 ) rc = 1; break;
  default:    
    // case _BGQ_FW_ND_DIR_IO:  this bit is in NetFlags, not NetFlags2                   
    if (  ND_ENABLE_ION_LINK  & p->Network_Config.NetFlags ) rc = 1; break;
  }

  return rc;
  
}



extern inline void fw_nd_get_loopback(Personality_t *p,int *loopback)
{
  int i;
  for (i=0;i<6;i++)
  {
    loopback[i] = ND_GET_LOOPBACK(i,p->Network_Config.NetFlags);
  }
}


 // for debugging
 int  fw_nd_print_network_personality(Personality_t *p)
 {

    printf(" nodes  = %3d %3d %3d %3d %3d \n", 
	   p->Network_Config.Anodes,
	   p->Network_Config.Bnodes,
	   p->Network_Config.Cnodes,
	   p->Network_Config.Dnodes,
	   p->Network_Config.Enodes);

    printf(" coords = %3d %3d %3d %3d %3d \n", 
	   p->Network_Config.Acoord,
	   p->Network_Config.Bcoord,
	   p->Network_Config.Ccoord,
	   p->Network_Config.Dcoord,
	   p->Network_Config.Ecoord);

    printf(" PSetSize = %d \n", ND_TORUS_SIZE(p->Network_Config) );

    // is it a torus, link enables, loopbacks
    printf(" is_torus = 0x%016lx\n", fw_nd_get_is_torus_bitmask(p));
    int x[6];
    fw_nd_get_loopback(p,x);
    printf(" loopback = %2d %2d %2d %2d %2d %2d\n",
           x[0],x[1],x[2],x[3],x[4],x[5]);
    fw_nd_get_swaps(p,x);
    printf(" swaps = %2d %2d %2d %2d %2d\n",
	   x[0],x[1],x[2],x[3],x[4]);
    printf(" sender enables = %1d %1d %1d %1d %1d %1d %1d %1d %1d %1d %1d\n",
           fw_nd_get_enable_sender(0,p),fw_nd_get_enable_sender(1,p),fw_nd_get_enable_sender(2,p), 
           fw_nd_get_enable_sender(3,p),fw_nd_get_enable_sender(4,p),fw_nd_get_enable_sender(5,p),    
           fw_nd_get_enable_sender(6,p),fw_nd_get_enable_sender(7,p),fw_nd_get_enable_sender(8,p),   
           fw_nd_get_enable_sender(9,p),fw_nd_get_enable_sender(10,p));

 
    printf (" TESTINT_DCR__CONFIG1 = 0x%016lx\n",DCRReadPriv( TESTINT_DCR(CONFIG1)));

    return 0;

 }

extern int  fw_nd_is_multinode(Personality_t *p);
extern int  fw_nd_check_fatal_errors(Personality_t *p);
extern int  fw_nd_loopback_termcheck(Personality_t *p);

// used by fw_nd_loopback_termcheck for logic verification 
// should not be used in general systems, would eventually want to cut RAS events
extern inline int fw_nd_compare_values(uint64_t a, uint64_t b, uint64_t addr, int line_num)
{
  if ( a == b) return 0;
  FW_Warning(" Error: a != b addr=0x%016lx line=%d   a=0x%016lx b=0x%016lx",addr,line_num,a,b);
  return line_num;
  
}

extern inline uint64_t fw_nd_get_recv_dir(uint64_t send_dir)
{
  switch(send_dir)
  {
  case _BGQ_FW_ND_DIR_A_MINUS: return  _BGQ_FW_ND_DIR_A_PLUS;  break;
  case _BGQ_FW_ND_DIR_A_PLUS:  return  _BGQ_FW_ND_DIR_A_MINUS; break;
  case _BGQ_FW_ND_DIR_B_MINUS: return  _BGQ_FW_ND_DIR_B_PLUS;  break;
  case _BGQ_FW_ND_DIR_B_PLUS:  return  _BGQ_FW_ND_DIR_B_MINUS; break;
  case _BGQ_FW_ND_DIR_C_MINUS: return  _BGQ_FW_ND_DIR_C_PLUS;  break;
  case _BGQ_FW_ND_DIR_C_PLUS:  return  _BGQ_FW_ND_DIR_C_MINUS; break;
  case _BGQ_FW_ND_DIR_D_MINUS: return  _BGQ_FW_ND_DIR_D_PLUS;  break;
  case _BGQ_FW_ND_DIR_D_PLUS:  return  _BGQ_FW_ND_DIR_D_MINUS; break;
  case _BGQ_FW_ND_DIR_E_MINUS: return  _BGQ_FW_ND_DIR_E_PLUS;  break;
  case _BGQ_FW_ND_DIR_E_PLUS:  return  _BGQ_FW_ND_DIR_E_MINUS; break; 
  default:                     return  _BGQ_FW_ND_DIR_IO;      break; 
  }
  
  return 0;
}

#ifndef MAX
#define MAX(a,b)  (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b)  (((a)<(b))?(a):(b))
#endif

 int fw_nd_termCheck( void )

 {
   Personality_t *p = FW_PERSONALITY_PTR();
  int rc=0;

  // Comment out below, since for logic verification and firmware extension tests
  // the term check may be called directly, even if personality is not set
#if 0
   if ( ! PERS_ENABLED( PERS_ENABLE_ND ) ) {
    return 0;
  }
#endif

  if ( p->Kernel_Config.NodeConfig & PERS_ENABLE_Mambo) return 0; // allow mambo to succeed 
  if ( fw_nd_is_multinode(p) ) return 0;   // TODO  
  rc += fw_nd_check_fatal_errors(p);
  rc += fw_nd_loopback_termcheck(p); // Only valid in loopback
  
  // TODO - check fifo pointers, tokens, crcs, other status values

  if ( TRACE_ENABLED( TRACE_ND ) )printf(" fw_nd_term_check rc= %d\n",rc);
  
  return rc;
  


}


int fw_nd_check_fatal_errors(Personality_t *p)
  
{
  int rc=0;
  uint64_t i, dcr_val, dcr_mask; 
  // check the 500 MHz domain
  for (i=0;i<  ND_500_NUM_FATAL_ERR_DCRs;i++)
  {
    dcr_val =  DCRReadPriv( ND_500_DCR(FATAL_ERR0)+i);
    if ( dcr_val) rc+= 900+i;  // TODO: RAS event ? etc

    
  }
  
  // check the _x2 domain

  dcr_val =  DCRReadPriv(ND_X2_DCR(FATAL_ERR0));
  if ( dcr_val !=0) rc+= 910; // TODO: RAS event etc

  dcr_val =  DCRReadPriv(ND_X2_DCR(FATAL_ERR1));
  if ( dcr_val !=0) rc+= 910; // TODO: RAS event etc

  // check the send receive units
  // TODO  dcr_mask = ~ ND_RESE_DCR__FATAL_ERR__SE_DROPPED_PKT_set(1);
  dcr_mask = ~ _BN(63);  // TEMPORARY Until DCR Generator is fixed

  // the last bit in this dcr is a nonfatal error, indicating
  //  a dropped packet so mask that off
  
  
  for (i=0;i< ND_RESE_DCR_num;i++)
  {
    if ( fw_nd_get_enable_sender(i,p) )
    {
      dcr_val = DCRReadPriv( ND_RESE_DCR(i,FATAL_ERR));


      if ( (dcr_val &dcr_mask)  !=0) rc+= 920+i;
    }
    
  }
  
  if ( TRACE_ENABLED( TRACE_ND ) ) printf(" fw_nd_check_fatal_errors rc= %d\n",rc);
  return rc;
  
  
}



int fw_nd_loopback_termcheck(Personality_t *p)
  
{
  // Termination check should probably be a kernel function, since it involves
  // waiting until network is empty and sending crc exchange messages
  // For logic verification, we need to provide a firmware extension version
  // Many of the checks performed here should be ported over to a multi-node kernel function
  // in this version, all miscompares result in a printf and a non-zero return code
  // this version will probably eventually be empty, or moved elswhere

  int rc=0;
  int rc1;
  int recv;
  
  if ( ND_TORUS_SIZE(p->Network_Config) != 1)  return 0; 
  if ( p->Kernel_Config.NodeConfig & PERS_ENABLE_Mambo) return 0; // Mambo knows nothing about these DCRs



  // the network is assumed to be completely quiesced at this point
  uint64_t a,b,addr_a,addr_b;
  

  // check reception fifo head == tail
  uint32_t i;
  for (i=0;i<ND_500_NUM_RCP_FIFO_P_DCRs;i++)
  {
    addr_a =  ND_500_DCR(RCP_FIFO_HEAD_P0) +i;
    addr_b =  ND_500_DCR(RCP_FIFO_TAIL_P0) +i;
    a      =  DCRReadPriv(addr_a);
    b      =  DCRReadPriv(addr_b);
    rc1    = fw_nd_compare_values(a,b,addr_a,__LINE__);
    rc     = MAX(rc,rc1);
  }
 
  // check injection fifo head == tail
  for (i=0; i < ND_500_NUM_INJ_FIFO_P_DCRs; i++)
  {
    addr_a =  ND_500_DCR(INJ_FIFO_HEAD_P0) +i;
    addr_b =  ND_500_DCR(INJ_FIFO_TAIL_P0) +i;
    a      =  DCRReadPriv(addr_a);
    b      =  DCRReadPriv(addr_b);
    rc1    = fw_nd_compare_values(a,b,addr_a,__LINE__);
    rc     = MAX(rc,rc1);
  }
 
  // clk_500: status dcrs 0 to 3 are state machine values
  // 4 - 7 are header check errors, should be 0 in a normal run, but need not be
  // there are spi tests that cause header errors and look at stat 4-7, so don't read these
  // 8 and 9 always tied to 0
  // stat0_value: 0x1111111111110000
  // stat1_value: 0x2492492492490000
  // stat2_value: 0x0000000000000000
  // stat3_value: 0x0000000000000000

  uint64_t dcr_stat_500[ND_500_NUM_STAT_DCRs] = 
  {
    0x1111111111110000ULL, 0x2492492492490000ULL, 0, 0, 0, 0, 0, 0, 0, 0
  };
  
  
  for (i=0;i< ND_500_NUM_STAT_DCRs;i++)  
  {
    if ( ( i<4) || (i>7))
    {
      addr_a =  ND_500_DCR(STAT0) +i; 
      a      =  DCRReadPriv(addr_a);
      rc1    = fw_nd_compare_values(a,dcr_stat_500[i],addr_a,__LINE__);
      rc     = MAX(rc,rc1);
      
    }
  }

  // check x2 state machines, should be 0
  for (i=0;i< ND_X2_NUM_STAT_DCRs;i++) 
  {
    addr_a =  ND_X2_DCR(STAT0) +i;    
    a      =  DCRReadPriv(addr_a);
    rc1    =  fw_nd_compare_values(a,0,addr_a,__LINE__);
    rc     =  MAX(rc,rc1);
  }
  
  // check the rese units
  uint64_t mask =0;
  uint64_t head, tail,one_or_two;


  for (i=0;i< ND_RESE_DCR_num;i++)
  {
    if ( fw_nd_get_enable_sender(i,p) )
    {

      // ND_RESE_DCR__RE_INTERNAL_STATE__VALUE_width 
      // 5 state machines  cannot check first 8 bits then 00 (01 in dd2) 0x1  0x1 0x1 0x1 rest are 0
      //  vc head != tail pointers
      addr_a =  ND_RESE_DCR(i,RE_INTERNAL_STATE);
      mask   =  0x00FFFFFFFFFFFFFFULL;
      a      =  mask & DCRReadPriv(addr_a);
      b      =  0x0011110000000000ULL;
      rc1    =  fw_nd_compare_values(a,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);      
      
      // ND_RESE_DCR__RE_VC0_PKT_CNT_offset should be 0
      addr_a =  ND_RESE_DCR(i,RE_VC0_PKT_CNT);
      a      =  DCRReadPriv(addr_a);
      b      =  0;
      rc1    =  fw_nd_compare_values(a,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);         

      // ND_RESE_DCR__RE_VC0_STATE__VALUE_width  9 nibbles of 4 bit 1's then 0's, on io link all 0
      addr_a =  ND_RESE_DCR(i,RE_VC0_STATE);
      a      =  DCRReadPriv(addr_a);
      b      =  0x1111111110000000ULL;
      if ( i == _BGQ_FW_ND_DIR_IO) b = 0;
      rc1    =  fw_nd_compare_values(a,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);         

      // ND_RESE_DCR__RE_VC1_STATE__VALUE_width  8 bits of 0, 7 4bit 1's, then0's  0 on io link
      addr_a =  ND_RESE_DCR(i,RE_VC1_STATE);
      a      =  DCRReadPriv(addr_a);
      b      =  0x0011111110000000ULL;
      if ( i == _BGQ_FW_ND_DIR_IO) b = 0;
      rc1    =  fw_nd_compare_values(a,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);         

      // ND_RESE_DCR__RE_VC2_STATE__VALUE_width  8 bits of 0, 7 4bit 1's, then0's  0 on io link
      addr_a =  ND_RESE_DCR(i,RE_VC2_STATE);
      a      =  DCRReadPriv(addr_a);
      b      =  0x0011111110000000ULL;
      if ( i == _BGQ_FW_ND_DIR_IO) b = 0;
      rc1    =  fw_nd_compare_values(a,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1); 

      // ND_RESE_DCR__RE_VC3_STATE__VALUE_width  8 bits of 0, 7 4bit 1's, then0's
      addr_a =  ND_RESE_DCR(i,RE_VC3_STATE);
      a      =  DCRReadPriv(addr_a);
      b      =  0x0011111110000000ULL;
      rc1    =  fw_nd_compare_values(a,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1); 


      // ND_RESE_DCR__RE_VC4_STATE__VALUE_width  0-7 is valid 0, 8 - 11 head, 12-15 tail (head = tail)
      //   3 4 bit 1s, 9 4 bit 0's
      addr_a =  ND_RESE_DCR(i,RE_VC4_STATE); 
      a      =  DCRReadPriv(addr_a);
      mask   =  0xFF00FFFFFFFFFFFFULL; // mask out two nibbles, head tail
      b      =  0x0000111000000000ULL;
      rc1    =  fw_nd_compare_values(a & mask ,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1); 
      // note  _G4(x,b) gets 4 bits of x starting at LSB b     
      head   = _G4(a,11);
      tail   = _G4(a,15);
      rc1    =  fw_nd_compare_values(head,tail,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);

      // ND_RESE_DCR__RE_VC5_STATE__VALUE_width  0-7 is valid 0, 8 - 11 head, 12-15 tail (head = tail)
      //   3 4 bit 1s, 9 4 bit 0's,  0 on io link
      addr_a =  ND_RESE_DCR(i,RE_VC5_STATE); 
      a      =  DCRReadPriv(addr_a);
      mask   =  0xFF00FFFFFFFFFFFFULL; // mask out two nibbles, head tail
      b      =  0x0000111000000000ULL;
      if ( i == _BGQ_FW_ND_DIR_IO) b = 0;      
      rc1    =  fw_nd_compare_values(a & mask ,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1); 
      // note  _G4(x,b) gets 4 bits of x starting at LSB b     
      head   = _G4(a,11);
      tail   = _G4(a,15);
      rc1    =  fw_nd_compare_values(head,tail,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);

      // ND_RESE_DCR__SE_INTERNAL_STATE__VALUE_width   4 bits 1 or 2, 
      // 8 bits,  could be 1, 2, e, f, or 0x10, for the 2 idle and 3 ack_only states.
      // 4 bit 1, 4 bit 1, 
      // 12 bits begin pointer , 12 bits end pointer (dd1 bug, so 0, should be = in and dd1 and dd2)
      // rest all 0's
      addr_a =  ND_RESE_DCR(i,SE_INTERNAL_STATE);
      a      =  DCRReadPriv(addr_a);
      mask   =  0x000FF000000FFFFFULL; 
      b      =  0x0001100000000000ULL;
      rc1    =  fw_nd_compare_values(a & mask ,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);
      // bits 0 - 3 are 1 or 2
      one_or_two   = _G4(a,3);
      if     ( !( ( one_or_two == 1) || ( one_or_two  ==2) ) )
      {
	printf(" Error: bits 0-3 of SE_INTERNAL_STATE != 1 or 2 addr=0x%016lx line=%d a=0x%016lx  one_or_two=0x%016lx \n",
	       addr_a,__LINE__,a,one_or_two );
	rc     =  MAX(rc,__LINE__);  
      }
      // bits 4 - 11 are 1 or 2, 0xe, 0xf, or 0x10
       one_or_two   = _G8(a,11);
       if     ( !( ( one_or_two == 1)  || ( one_or_two  ==2)  ||       // two idle states
		   (one_or_two  == 0xe)|| (one_or_two  == 0xf) || (one_or_two  == 0x10  ) ) )  // three ack-only states
      {
	printf(" Error:  bits 4-11 of SE_INTERNAL_STATE != idle or ack_only states addr=0x%016lx line=%d a=0x%016lx  one_or_two=0x%016lx \n",
	       addr_a,__LINE__,a,one_or_two );
	rc     =  MAX(rc,__LINE__);  
      }
      
     


      head   = _G12(a,31);
      tail   = _G12(a,43);
      rc1    =  fw_nd_compare_values(head,tail,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);      

      // check tokens, issue 1089, tokens depends on control register bit
      if ( DCRReadPriv(  ND_RESE_DCR(i,CTRL) ) & _BN(0) ) b  =  0x7677777ULL; // 118 dynamic
      else b = 0x7E77777ULL;
      addr_a =  ND_RESE_DCR(i,SE_TOKENS);      
      a      =  DCRReadPriv(addr_a);
      rc1    =  fw_nd_compare_values(a,b,addr_a,__LINE__);
      rc     =  MAX(rc,rc1);  

      // now check the send and recv link level crcs
      // normally in loopback, match the plus with the minus, but if 
      // in hss loopback, match the plus with plus and minus with minus
      // for direction i, dimension is i/2

      int  loopback =  ND_GET_LOOPBACK(i/2,p->Network_Config.NetFlags);
      if ( loopback == ND_LOOPBACK_HSS) recv = i;
      else recv  = (int) fw_nd_get_recv_dir( (uint64_t) i);
  
      
      addr_a = ND_RESE_DCR(i,SE_LINK_CRC_USR);
      addr_b = ND_RESE_DCR(recv,RE_LINK_CRC_USR);
      a      = DCRReadPriv(addr_a);
      b      = DCRReadPriv(addr_b);
      rc1    = fw_nd_compare_values(a,b,addr_a,__LINE__); 
      rc     = MAX(rc,rc1);  

      addr_a = ND_RESE_DCR(i,SE_LINK_CRC_SYS);
      addr_b = ND_RESE_DCR(recv,RE_LINK_CRC_SYS);
      a      = DCRReadPriv(addr_a);
      b      = DCRReadPriv(addr_b);
      rc1    = fw_nd_compare_values(a,b,addr_a,__LINE__); 
      rc     = MAX(rc,rc1);  
      
    } // end of   if ( fw_nd_get_enable_sender(i,p) )
    
    
  } // end of i loop
  
  return rc;
  
}

void fw_semaphore_down( size_t lock_number ) {

#if 1
  while ( 1 ) {

    if ( (int64_t)BeDRAM_Read(lock_number) > 0 ) {

      // We might get in ... now decrement atomically and
      // make sure that we did. 

      if ( (int64_t)BeDRAM_ReadDecWrap(lock_number) > 0 ) {
	return;
      }
      else {
	// Nice try !
	BeDRAM_ReadIncWrap(lock_number);
      }
    }
  }
#endif
}


void fw_semaphore_up( size_t lock_number ) {
  BeDRAM_ReadIncWrap(lock_number);
}
