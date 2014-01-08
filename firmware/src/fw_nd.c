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

-----------------------------------------------------*/

 
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include "Firmware_internals.h"

#include "Firmware_RAS.h"
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/nd_x2_dcr.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/testint_inlines.h>
#include <hwi/include/bqc/classroute.h>
#include <hwi/include/bqc/MU_Macros.h>
#include <hwi/include/bqc/MU_Pt2PtNetworkHeader.h>
#include <spi/include/mu/GIBarrier.h>

// These defines are no longer produced by the generated DCR headers.

#define ND_500_NUM_FATAL_ERR_DCRs   6
#define ND_500_NUM_RCP_FIFO_P_DCRs  4
#define ND_500_NUM_INJ_FIFO_P_DCRs  2
#define ND_500_NUM_STAT_DCRs        10
#define ND_X2_NUM_STAT_DCRs         2

// recoverable error thresholds when in  diagnostics mode (+Diagnostics in svc host file)

#define _BGQ_FW_ND_CE_THRESH_DIAG             1
#define _BGQ_FW_ND_RETRAN_THRESH_DIAG         5
#define _BGQ_FW_ND_RECEIVER_ERR_THRESH_DIAG   2000


// recoverable error thresholds when NOT in  diagnostics mode
// _BGQ_FW_ND_RECEIVER_ERR_THRESH is the number of network cycles (nominally 500 MHz cycles)
// during which a receiver is in error recovery state,  if the link is idle at the time of
// an error, this will be ~150, but it could be several thousand if the link is busy  
#define _BGQ_FW_ND_CE_THRESH                  50 // About 10 error events (one event = 4-5 CE count). See issue 5098
#define _BGQ_FW_ND_RETRAN_THRESH              166000
#define _BGQ_FW_ND_RECEIVER_ERR_THRESH        200000000

#define _BGQ_ND_NUM_GI_CLASS_DCRS     (16/2)



#define _BGQ_FW_ND_DIM_A 0
#define _BGQ_FW_ND_DIM_B 1
#define _BGQ_FW_ND_DIM_C 2
#define _BGQ_FW_ND_DIM_D 3
#define _BGQ_FW_ND_DIM_E 4
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

#ifndef MAX
#define MAX(a,b)  (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b)  (((a)<(b))?(a):(b))
#endif

// Retran Timeout in bytes
// ( 2.5 max bytes in packet) + 
// 2 ( for round trip) * ( max cable delay  + link chip delay in bytes)
// Cable and link delay in bytes
// ( 100 meters*5ns/meter*2GB/s*2 roundtrip) 
// + 4 linkchips (4*40 ns * 2GB/s * 2 round trip)
// max bytes in packet = 544 
// 4 bytes per network cycle
#define _BGQ_FW_ND_MAX_LINK_DELAY_BYTES   ( ( 100*5*2*2) + (40*4*2*2) )
#define _BGQ_FW_ND_RETRAN_TIMEOUT_BYTES   ( (5*544/2) +  _BGQ_FW_ND_MAX_LINK_DELAY_BYTES)
#define _BGQ_FW_ND_RETRAN_TIMEOUT_CYCLES  ( (_BGQ_FW_ND_RETRAN_TIMEOUT_BYTES)/4)

#define _BGQ_FW_ND_COLL_GRANT_TIMEOUT     (512)
#define _BGQ_FW_ND_NUM_GI_CLASSES 16

#define ZONE_2_IS_SHORTEST_TO_LONGEST 0


int  fw_nd_release_dcr_units(Personality_t *p);                             
int  fw_nd_reset_release_step1(Personality_t *p);
int  fw_nd_reset_release_step2(Personality_t *p);
int  fw_nd_set_det_order_masks(Personality_t *p);
int  fw_nd_set_zone_masks(Personality_t *p);
int  fw_nd_set_gi_class_routes(Personality_t *p);
int  fw_nd_set_collective_class_routes(Personality_t *p);
int  fw_nd_unmask_interrupts(Personality_t *p);
int  fw_nd_set_torus_routing_info(Personality_t *p);
int  fw_nd_setup_gea(Personality_t *p); // setup GEA interrupt mapping for ND
void fw_nd_compute_det_order_and_zone_masks(Personality_t *p);
void fw_nd_compute_order_dimensions(Personality_t *p, int *dim_order, int *effective_len);
void fw_nd_order_dimensions(Personality_t *p, int *dim_order, int *effective_len);
uint64_t fw_nd_get_dcr_cutoffs(Personality_t *p);
uint64_t fw_nd_get_dcr_neighbors(Personality_t *p);
int  fw_nd_is_multinode(Personality_t *p);


extern inline int fw_nd_get_use_port6_for_io(Personality_t *p)
{
  return TI_UsePort6forIO();
}

extern inline int fw_nd_get_use_port7_for_io(Personality_t *p)
{
  return TI_UsePort7forIO();
}


// TODO  in loopback, if one is set, so must the other
extern inline int fw_nd_get_use_port67_for_io(Personality_t *p)
{
  return ( fw_nd_get_use_port6_for_io(p) || fw_nd_get_use_port7_for_io(p) );
}

// TODO - could also come from testint DCR, must be consistent with that
extern inline int fw_nd_get_is_ionode(Personality_t *p)
{
  if ( p->Kernel_Config.NodeConfig & PERS_ENABLE_IsIoNode) return 1;
  return 0; 
}


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



extern inline void fw_nd_get_coords(Personality_t *p,int *c)
{

  // get the coords from the personality and put into a vector
  c[0] = p->Network_Config.Acoord;
  c[1] = p->Network_Config.Bcoord;  
  c[2] = p->Network_Config.Ccoord;
  c[3] = p->Network_Config.Dcoord;  
  c[4] = p->Network_Config.Ecoord;

}

extern inline void fw_nd_get_nodes(Personality_t *p,int *n)
{

  // get the nodes from the personality and put into a vector
  n[0] = p->Network_Config.Anodes;
  n[1] = p->Network_Config.Bnodes;  
  n[2] = p->Network_Config.Cnodes;
  n[3] = p->Network_Config.Dnodes;  
  n[4] = p->Network_Config.Enodes;

}


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

extern inline void fw_nd_get_is_torus(Personality_t *p,int *is_torus)
{
  int i;
  for (i=0;i<5;i++) is_torus[i] = ND_GET_TORUS(i,p->Network_Config.NetFlags);
}

extern inline uint64_t fw_nd_get_is_torus_bitmask(Personality_t *p)
{
  return  ND_GET_ALL_TORUS(p->Network_Config.NetFlags);
  
}  



// TODO: get from Personality once default is set

extern inline void fw_nd_get_loopback(Personality_t *p,int *loopback)
{
  int i;
  for (i=0;i<6;i++)
  {
    loopback[i] = ND_GET_LOOPBACK(i,p->Network_Config.NetFlags);
  }
}

extern inline uint64_t fw_nd_get_loopback_bitmask(Personality_t *p)
{

  int loopback[6];
  fw_nd_get_loopback(p,loopback);
  // set 6 LSB bits  58,59,60,61,62,63
  uint64_t x =0;
  int i;
  for (i=0;i<6;i++) if  (loopback[i]== ND_LOOPBACK_TORUS_INTERNAL)  x |= _BN(58+i);
  return x;
}


extern inline uint64_t fw_nd_get_vcdn_ranhints(Personality_t *p)
{
  // can result in non-minimal paths (except on E when E is of length 2)
  // if base coordinate is 0, so disable except on E
  // if base coordinates are nonzero, this can be revisited
  return ND_GET_ALL_TORUS_RAN_HINTS(p->Network_Config.NetFlags) & _BN(63) ;

  
}


extern inline uint64_t fw_nd_get_se_arb_re_inj_ratio(Personality_t *p)
{
  return ND_GET_SE_ARB_RE_INJ_RATIO(p->Network_Config.NetFlags);
}

// TODO  should be computed from personality
// same as in P, take into account if torus, and tiebreaks


uint64_t fw_nd_get_dcr_cutoffs(Personality_t *p)
{
  // set the cutoff registers, as on BG/L and BG/P break ties differently if coordinate is even or odd
  int coords[5];
  int nodes[5];
  int is_torus[5];
  int co_plus[5];
  int co_minus[5];  
  int i;
  
  uint64_t dcr_val = 0;
  fw_nd_get_coords(p,coords);
  fw_nd_get_nodes(p,nodes);
  fw_nd_get_is_torus(p,is_torus);
  int a,b;
  int even_odd  = (coords[0] + coords[1] + coords[2] + coords[3] + coords[4] )%2;
  // for better checkerboarding,  break ties in all directions the same way depending
  // on if the coordidnate sum is even or odd
  for(i=0;i<5;i++)
  {
    
    switch(is_torus[i])
    {
    case 0: // mesh
      co_minus[i]=0;
      co_plus[i] =nodes[i]-1;
      break;

    default:
      a = (nodes[i] >> 1);   // a = nodes[i]/2 
      b = MAX( 0, (a - 1) ); // b = nodes[i]/2 -1
      //  if ( coords[i] & 1 )
      if ( even_odd )
      { // odd node in dimension i
	co_plus[i]  = MIN( (nodes[i]-1),(coords[i]+b) );
	co_minus[i] = MAX( 0, (coords[i] - a) );
      }
      else 
      { // even node in dimension i
	co_plus[i]  = MIN( (nodes[i]-1),(coords[i]+a) );
	co_minus[i] = MAX( 0, (coords[i] - b) );
      }
      break;
    }
    // never let them be less than 0, in case nodes[i] = 0 (on for D or E on IO node)
    if (nodes[i] == 0)
    {
      co_plus[i] =co_minus[i] = 0;
    }
    

  }
  // Note that the cutoffs have been programmed for D and E, even if
  // IO Node or port6/7 is used for IO
  // This is ok, since those cutoffs are  not used by the logic in such cases
  ND_500_DCR__CTRL_CUTOFFS__A_PLUS_insert (dcr_val,co_plus[0]);
  ND_500_DCR__CTRL_CUTOFFS__A_MINUS_insert(dcr_val,co_minus[0]);  
  ND_500_DCR__CTRL_CUTOFFS__B_PLUS_insert (dcr_val,co_plus[1]);
  ND_500_DCR__CTRL_CUTOFFS__B_MINUS_insert(dcr_val,co_minus[1]); 
  ND_500_DCR__CTRL_CUTOFFS__C_PLUS_insert (dcr_val,co_plus[2]);
  ND_500_DCR__CTRL_CUTOFFS__C_MINUS_insert(dcr_val,co_minus[2]);  
  ND_500_DCR__CTRL_CUTOFFS__D_PLUS_insert (dcr_val,co_plus[3]);
  ND_500_DCR__CTRL_CUTOFFS__D_MINUS_insert(dcr_val,co_minus[3]); 
  ND_500_DCR__CTRL_CUTOFFS__E_PLUS_insert (dcr_val,co_plus[4]);
  ND_500_DCR__CTRL_CUTOFFS__E_MINUS_insert(dcr_val,co_minus[4]);  

  return dcr_val;
}


//  TODO  check this,  
//  the constant assumes  70 meters cable + 4 link chips
//  this should work,  but might need to be adjusted
  
extern inline  uint64_t fw_nd_get_retran_timeout(Personality_t *p)
{
  // retransmission timeout, in nd, or 500 MHz cycles
  return _BGQ_FW_ND_RETRAN_TIMEOUT_CYCLES;
  
}

extern inline  uint64_t fw_nd_get_coll_grant_timeout(Personality_t *p)
{
  // collective grant timeout, in nd, or 500 MHz cycles
  return _BGQ_FW_ND_COLL_GRANT_TIMEOUT;
  
}

uint64_t fw_nd_getBarrierTimeoutInMicroseconds( Personality_t* p ) {

    uint64_t barrier_timeout =  0;

    if ( ND_TORUS_SIZE(p->Network_Config) <= 512 ) {
	barrier_timeout = 5ull * 60ull * 1000ull * 1000ull;  // 5 minutes
    }

    if ( TRACE_ENABLED(TRACE_ND) && (p->Network_Config.Acoord==0) && (p->Network_Config.Bcoord==0) && (p->Network_Config.Ccoord==0) && (p->Network_Config.Dcoord==0) && (p->Network_Config.Ecoord==0) ) {
	printf("(D) barrier_timeout=%ld torus_size = %d\n", barrier_timeout, ND_TORUS_SIZE(p->Network_Config));
    }
    return barrier_timeout;
}

extern int fw_nd_control_system_barrier( void* arg )
{
  
  int rc = 0;
  Personality_t* p = FW_PERSONALITY_PTR();

  if ( TRACE_ENABLED( TRACE_ND ) )
    printf("(>) %s( %lu )\n", __func__, (uint64_t)arg);

  /*
   * If we have a non-trivial torus, then use the control
   * system barrier.
   */


  if ( ND_TORUS_SIZE( p->Network_Config ) > 1 ) {
      extern int fw_mailbox_barrier( fw_uint64_t timeoutInMicroseconds, fw_uint64_t warningThresholdInMicroseconds ) ;
      rc = fw_mailbox_barrier( (fw_uint64_t)arg, ((fw_uint64_t)arg)/2ull );
  }

  if ( rc != 0 ) {
      FW_Warning( "Control System Barrier failed rc=%d.", rc );
  }

  if ( TRACE_ENABLED( TRACE_ND ) )
    printf("(<) %s( %lu ) rc=%d\n", __func__, (uint64_t)arg, rc);

  return rc;
}





// -------------------- end

 int fw_nd_resetRelease( void )
 {

   TRACE_ENTRY(TRACE_ND);

   Personality_t* p = FW_PERSONALITY_PTR();

   //  this code basically assumes that the serdes and hss are brought
   //  out of reset first, ie, the links are trained
   //  if running in internal loopback, they serdes and hss can be kept in reset
   int rc=0;

   if ( TRACE_ENABLED( TRACE_ND ) ) fw_nd_print_network_personality(p); /*empty if*/ 

   rc = fw_nd_reset_release_step1(p);  // program control DCRs and release receivers
   if (rc) return rc;

   // if there is more than one node, a control system barrier is required here
   // to ensure all senders have been released prior to releasing receivers
   
   uint64_t barrier_timeout =  fw_nd_getBarrierTimeoutInMicroseconds(p);

//   Can remove this barrier since receiver error counts are held at zero
//   rc =  fw_nd_control_system_barrier((void *) barrier_timeout); 
//   if (rc) return __LINE__;  

   rc = fw_nd_reset_release_step2(p); // release the senders , unmask interrupts

   if (rc) return rc;

   // if there is more than one node, a control system barrier is required here
   // to ensure all receivers have been released prior to using the network

   rc =  fw_nd_control_system_barrier( (void *) barrier_timeout);
   if (rc) return __LINE__;

   rc = fw_nd_setup_gea(p); // setup GEA interrupt mapping
   if (rc) return rc;

   TRACE_EXIT(TRACE_ND);
   return rc;
 }


int fw_nd_init_global_barrier(void) {
   
  Personality_t* p = FW_PERSONALITY_PTR();

  uint64_t barrierSynchronizationTimeout = 10ull * (uint64_t)p->Kernel_Config.FreqMHz * 1000ull * 1000ull;  /* 10 seconds */
  uint64_t barrierTimeoutInMicroseconds  = fw_nd_getBarrierTimeoutInMicroseconds(p);
  uint64_t barrierTimeoutInCycles        = barrierTimeoutInMicroseconds * (uint64_t)p->Kernel_Config.FreqMHz;

  int rc;

#if 0
  int rc = MUSPI_GIBarrierInitMU( 
				 FW_GI_BARRIER_BOOT_CLASS_ROUTE, 
				 fw_nd_control_system_barrier, 
				 (void*)barrierTimeoutInMicroseconds,
				 barrierSynchronizationTimeout
				 );
#endif

  /* Step 2: Reset the control register to the initial state */
  // Write "100" to local contribution
  rc = MUSPI_GIBarrierInitMU1(FW_GI_BARRIER_BOOT_CLASS_ROUTE);
  if ( rc ) {
      FW_RAS_printf( FW_RAS_BARRIER_INIT_ERROR,  "Barrier initialization error (rc=%d line=%d).", rc, __LINE__);
      return rc;
  }

  // Step 3: Check all receivers in the system barrier are ready

#if 0
  if (FW_GI_BARRIER_BOOT_CLASS_ROUTE != 15) {
    return __LINE__;
  }
#endif

  uint64_t gi_class_route = ND_500_DCR__CTRL_GI_CLASS_14_15__CLASS15_UP_PORT_I_get(DCRReadPriv(ND_500_DCR(CTRL_GI_CLASS_14_15)));
  uint64_t re_gi_val;
  int i;

  // masking off local input
  gi_class_route &= ~_BN(63); 

  uint64_t start_cycle = GetTimeBase();
  while (gi_class_route != 0) {
      for (i=0; i<11; i++) {
	  if (_BN(52+i) & gi_class_route) {
	      re_gi_val = DCRReadPriv(ND_500_DCR(GI_STAT_RE)+i);
	      if ((re_gi_val & 0x7) == MUSPI_GIBARRIER_INITIAL_STATE) {
		  gi_class_route &= ~_BN(52+i);
	      }
	  }
      }

      if ( barrierTimeoutInCycles > 0 ){
	  if (GetTimeBase() > ( start_cycle + barrierTimeoutInCycles ) ) {
	      FW_RAS_printf( FW_RAS_BARRIER_INIT_ERROR,  "Barrier initialization error - receivers not ready (rc=%d, line=%d).",__LINE__ + 1,__LINE__);
	      return __LINE__;
	  }
      }
  }

  /* Step 4: Read the status register until it is in the initial state
   * or until a timeout occurs */

  rc = MUSPI_GIBarrierInitMU2(FW_GI_BARRIER_BOOT_CLASS_ROUTE, barrierSynchronizationTimeout);

  
  /* Step 5: If no timeout, barrier one final time */
  if ( rc == 0 ) {
      rc =  fw_nd_control_system_barrier((void *) barrierTimeoutInMicroseconds);
  }
  else {
      FW_RAS_printf( FW_RAS_BARRIER_INIT_ERROR,  "Barrier initialization error - status not sync'd (rc=%d, line=%d).", rc, __LINE__);
  }
 

  if ( TRACE_ENABLED( TRACE_ND ) ) {
    printf("(<) %s rc=%d\n", __func__, rc );
  }

  return rc;
}



 int fw_nd_reset_release_step1(Personality_t *p)

 {
   // bring the dcr units out of reset, 
   // program the coordinates etc



   uint64_t reset_rese;
   uint64_t i;
   int rc =0;
   // release the DCR units so other DCRs can be written 
   fw_nd_release_dcr_units(p);

   uint64_t x;
   uint64_t control_internal =0;

   x = fw_nd_get_loopback_bitmask(p);  
   control_internal |= ND_500_DCR__CTRL_INTERNAL__LOOPBACK_set(x);

   x = fw_nd_get_is_torus_bitmask(p);
   control_internal |= ND_500_DCR__CTRL_INTERNAL__IS_TORUS_set(x);

   x = fw_nd_get_vcdn_ranhints(p);  // randomize the hint bits for dynamic on ties
   control_internal |=  ND_500_DCR__CTRL_INTERNAL__VCDN_RAN_HINTS_set(x);

   x = fw_nd_get_se_arb_re_inj_ratio(p);
   control_internal |= ND_500_DCR__CTRL_INTERNAL__SE_ARB_RE_INJ_RATIO_set(x);

   // collective arbitration settings, issues 812 and 820, and 1432
   // must be 0 to avoid livelocks with collectives and multiple deposit direections
   ND_500_DCR__CTRL_INTERNAL__COLL_UP_ARB_SE_AVAIL_SEL_insert(control_internal, 0);
   
   //should be 1, drop request only when necessary
   control_internal |= ND_500_DCR__CTRL_INTERNAL__COLL_UP_ARB_RE_DROP_REQ_SEL_set(1);
   

   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(CTRL_INTERNAL),control_internal);

   fw_nd_set_torus_routing_info(p);

   // the retransmission and collective grant timeouts share a DCR
   uint64_t retran_timeout     = fw_nd_get_retran_timeout(p);
   uint64_t coll_grant_timeout = fw_nd_get_coll_grant_timeout(p);
   x = 0;
   x  = ND_500_DCR__CTRL_TIMEOUT__SE_RETRANS_set(retran_timeout);
   x |= ND_500_DCR__CTRL_TIMEOUT__COLL_GRANT_set( coll_grant_timeout);
   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(CTRL_TIMEOUT),x);


   fw_nd_set_gi_class_routes(p);  
   fw_nd_set_collective_class_routes(p); 


   // bring the injection and reception fifos out of reset in the x2 domain
   fw_tracedDcrWrite(TRACE_ND,ND_X2_DCR(RESET),0);

   // bring the injection and reception fifos, gi, collectives, rand  out of reset in the 500MHz domain
   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(RESET), 0);



   // bring the receivers out of reset, keep the senders in reset, note this releases the SE_TOK* units 
   reset_rese = ND_RESE_DCR__RESET__SE_set(1);
    for (i=0;i< ND_RESE_DCR_num;i++)
   {
     if ( fw_nd_get_enable_sender(i,p)) {
         // Hold receiver errors at 0
	 fw_tracedDcrWrite(TRACE_ND,ND_RESE_DCR(i, CLEAR), ND_RESE_DCR__CLEAR__RE_CLR_LINK_ERR_CNT_set(1));
	 fw_tracedDcrWrite(TRACE_ND,ND_RESE_DCR(i, RESET),reset_rese );
     }
   }

   return rc;

 }

 int fw_nd_release_dcr_units(Personality_t *p)

 {
   // release the DCR units so other DCRs can be written   
   uint64_t reset_x2, reset_all;
   uint64_t reset_x500;
   uint64_t reset_rese;
   uint64_t i;

   // make sure the units are in reset already

   reset_all = ~0ULL;
   fw_tracedDcrWrite(TRACE_ND,ND_X2_DCR(RESET),reset_all);   
   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(RESET),reset_all);
   for (i=0;i<  ND_RESE_DCR_num;i++)
   {
     if ( fw_nd_get_enable_sender(i,p)) fw_tracedDcrWrite(TRACE_ND,ND_RESE_DCR(i, RESET),reset_all );
   }


   // lower the DCR register resets 
   reset_x2 = ~ ND_X2_DCR__RESET__DCRS_OUT_set(1);

   // DCRWritePriv(ND_X2_DCR(RESET) ,reset_x2);
   fw_tracedDcrWrite(TRACE_ND,ND_X2_DCR(RESET),reset_x2);

   reset_x500 = ~ND_500_DCR__RESET__DCRS_OUT_set(1)  ;
   // DCRWritePriv(ND_500_DCR(RESET),reset_x500);
   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(RESET),reset_x500);

   reset_rese = ~ND_RESE_DCR__RESET__DCRS_OUT_set(1);  
   for (i=0;i<  ND_RESE_DCR_num;i++)
   {
     if ( fw_nd_get_enable_sender(i,p)) fw_tracedDcrWrite(TRACE_ND,ND_RESE_DCR(i, RESET),reset_rese );
   }

   return 0;


 }





 int  fw_nd_reset_release_step2(Personality_t *p)

 {
   int rc = 0;
   uint64_t i;
   for (i=0;i< ND_RESE_DCR_num;i++)
   {
     if ( fw_nd_get_enable_sender(i,p) ) fw_tracedDcrWrite(TRACE_ND,ND_RESE_DCR(i, RESET),0);
   }

   rc = fw_nd_unmask_interrupts(p);  
   return rc;

 }






 int fw_nd_set_torus_routing_info(Personality_t *p)
 {

   uint64_t coords =0;


   // not conducive to looping, could do so with additional macros
   coords |= ND_500_DCR__CTRL_COORDS__NODE_COORD_A_set(p->Network_Config.Acoord);
   coords |= ND_500_DCR__CTRL_COORDS__NODE_COORD_B_set(p->Network_Config.Bcoord);
   coords |= ND_500_DCR__CTRL_COORDS__NODE_COORD_C_set(p->Network_Config.Ccoord);
   coords |= ND_500_DCR__CTRL_COORDS__NODE_COORD_D_set(p->Network_Config.Dcoord);
   if (fw_nd_get_is_ionode(p)== 0)  coords |= ND_500_DCR__CTRL_COORDS__NODE_COORD_E_set(p->Network_Config.Ecoord);

   if (p->Network_Config.Anodes) coords |= ND_500_DCR__CTRL_COORDS__MAX_COORD_A_set(p->Network_Config.Anodes -1);
   if (p->Network_Config.Bnodes) coords |= ND_500_DCR__CTRL_COORDS__MAX_COORD_B_set(p->Network_Config.Bnodes -1);
   if (p->Network_Config.Cnodes) coords |= ND_500_DCR__CTRL_COORDS__MAX_COORD_C_set(p->Network_Config.Cnodes -1);
   if ((p->Network_Config.Dnodes) && (fw_nd_get_use_port67_for_io(p)==0)) 
   coords |= ND_500_DCR__CTRL_COORDS__MAX_COORD_D_set(p->Network_Config.Dnodes -1);
   if ((p->Network_Config.Enodes) && (fw_nd_get_is_ionode(p)== 0) )
   coords |= ND_500_DCR__CTRL_COORDS__MAX_COORD_E_set(p->Network_Config.Enodes -1);

   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(CTRL_COORDS),coords);

   uint64_t cutoffs = fw_nd_get_dcr_cutoffs(p);
   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(CTRL_CUTOFFS),cutoffs);

   uint64_t neighbors = fw_nd_get_dcr_neighbors(p);
   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(CTRL_NEIGHBOR_COORDS),neighbors);


   fw_nd_compute_det_order_and_zone_masks(p);  // this could be moved to the control system eventually
   fw_nd_set_det_order_masks(p);
   fw_nd_set_zone_masks(p); 
   return 0;

 }





 int  fw_nd_set_det_order_masks(Personality_t *p)

 {


   uint64_t  det_order_masks = 0;
   det_order_masks |= ND_500_DCR__CTRL_DET_ORDER__MASK0_set( ND_GET_DET_ORDER_MASK(0,p->Network_Config.NetFlags)); 
   det_order_masks |= ND_500_DCR__CTRL_DET_ORDER__MASK1_set( ND_GET_DET_ORDER_MASK(1,p->Network_Config.NetFlags)); 
   det_order_masks |= ND_500_DCR__CTRL_DET_ORDER__MASK2_set( ND_GET_DET_ORDER_MASK(2,p->Network_Config.NetFlags)); 
   det_order_masks |= ND_500_DCR__CTRL_DET_ORDER__MASK3_set( ND_GET_DET_ORDER_MASK(3,p->Network_Config.NetFlags)); 
   det_order_masks |= ND_500_DCR__CTRL_DET_ORDER__MASK4_set( ND_GET_DET_ORDER_MASK(4,p->Network_Config.NetFlags)); 
   fw_tracedDcrWrite(TRACE_ND, ND_500_DCR(CTRL_DET_ORDER),det_order_masks);
   return 0;


 }

 int  fw_nd_set_zone_masks(Personality_t *p)

 {

   uint64_t zone_masks = 0;
   // There are four 32 bit fields in the personality for the 4 zone masks
   // Each mask has five 5 bit fields, the first 7 bits are 0 in the personality
   // In the DCR, two sets of zone masks share a single 64 bit DCR, as two 32 bit fields
   // and the first 7 bits are 0, so even though there is not macro in the DCR .h file, 
   // can combine the two 32 bit fields from the personality into a single 64 bit
   // value to be written
   zone_masks =  _B32(31,(uint64_t) p->Network_Config.ZoneRoutingMasks[0]) |
	         _B32(63,(uint64_t) p->Network_Config.ZoneRoutingMasks[1]);
   
   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(CTRL_DYNAMIC_ZONE01),zone_masks);

   zone_masks =  _B32(31,(uint64_t) p->Network_Config.ZoneRoutingMasks[2]) |
                 _B32(63,(uint64_t) p->Network_Config.ZoneRoutingMasks[3]);

   fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(CTRL_DYNAMIC_ZONE23),zone_masks);
   return 0;
 }


 int   fw_nd_set_gi_class_routes(Personality_t *p)
 {

   // We copy the class route for class 15 from the personality verbatim.
   // This sets up the "primordial" class route.  The other class routes will all be 0.
   // There are two class routes per control DCR.
   // Note: the mu init code by default sets all gi classes to be system, 
   uint64_t dcr_val =0;
   dcr_val |=ND_500_DCR__CTRL_GI_CLASS_00_01__CLASS01_UP_PORT_I_set(p->Network_Config.PrimordialClassRoute.GlobIntUpPortInputs) 
           | ND_500_DCR__CTRL_GI_CLASS_00_01__CLASS01_UP_PORT_O_set(p->Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs);
   fw_tracedDcrWrite(TRACE_ND, ND_500_DCR(CTRL_GI_CLASS_14_15),dcr_val);
   return 0;

 }


 int  fw_nd_set_collective_class_routes(Personality_t *p)

 {

   // We copy the class route for class 15 from the personality verbatim.
   // This sets up the "primordial" class route.  The other class routes will all be 0.
   uint64_t dcr_val =0;
   dcr_val |= ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS01_TYPE_set( 
                             ND_CR_GET_COLL_TYPE (p->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs)) 
	   |  ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS01_UP_PORT_I_set( 
                             ND_CR_GET_COLL_INPUT(p->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs))
	   |  ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS01_UP_PORT_O_set(
			     p->Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs);
   
   fw_tracedDcrWrite(TRACE_ND, ND_500_DCR(CTRL_COLL_CLASS_14_15),dcr_val);
   return 0;

 }



 int fw_nd_unmask_interrupts(Personality_t *p)

 {
   int rc=0;
   uint64_t enable = ~0;
   uint64_t check_val;
   uint64_t i;
   // we also include a simple test to see if the dcrs written can be read back
   // and have the expected value,  if not it is an error, but not a complete test

   // unmask the 500 MHz domain
   for (i=0;i<ND_500_NUM_FATAL_ERR_DCRs;i++)
   {
     fw_tracedDcrWrite(TRACE_ND, ND_500_DCR(FATAL_ERR_ENABLE) +i,enable);
     }

   // read one of them back to see if get what was written
   check_val = DCRReadPriv( ND_500_DCR(FATAL_ERR_ENABLE)+0);
   if ( check_val != ND_500_DCR__FATAL_ERR_ENABLE__VALUE_set(enable) ) return 101;


   // unmask the x2 domain
   fw_tracedDcrWrite(TRACE_ND,ND_X2_DCR(FATAL_ERR0_ENABLE),enable);
   fw_tracedDcrWrite(TRACE_ND,ND_X2_DCR(FATAL_ERR1_ENABLE),enable);

   // read one of them back to see if get what was written
   check_val =  DCRReadPriv(ND_X2_DCR(FATAL_ERR1_ENABLE));
   if ( check_val !=  ND_X2_DCR__FATAL_ERR1_ENABLE__VALUE_set(enable)) return 102;


   // lower the recoverable error count thresholds
   uint64_t ce_thresh, retran_thresh,receiver_err_thresh;
   
   ce_thresh                = _BGQ_FW_ND_CE_THRESH ;
   retran_thresh            = _BGQ_FW_ND_RETRAN_THRESH;
   receiver_err_thresh      = _BGQ_FW_ND_RECEIVER_ERR_THRESH;


   if ( p->Kernel_Config.NodeConfig & PERS_ENABLE_DiagnosticsMode )
     {
       ce_thresh            = _BGQ_FW_ND_CE_THRESH_DIAG ;
       retran_thresh        = _BGQ_FW_ND_RETRAN_THRESH_DIAG ;
       receiver_err_thresh  = _BGQ_FW_ND_RECEIVER_ERR_THRESH_DIAG;

     }
	

   
  fw_tracedDcrWrite(TRACE_ND,ND_500_DCR(CE_THRESHOLD),ce_thresh);     

   // unmask the send receive units

   for (i=0;i< ND_RESE_DCR_num;i++)
   {
     if ( fw_nd_get_enable_sender(i,p))
     {
       // lower the recoverable error count thresholds
       fw_tracedDcrWrite(TRACE_ND, ND_RESE_DCR(i,THRESH_ARRAY_CE),ce_thresh);
       fw_tracedDcrWrite(TRACE_ND, ND_RESE_DCR(i,THRESH_SE_RETRAN),retran_thresh);
       fw_tracedDcrWrite(TRACE_ND, ND_RESE_DCR(i,THRESH_RE_ERR), receiver_err_thresh);

       if ( PERS_ENABLE_MaskLinkErrors & p->Kernel_Config.NodeConfig) 
       {
	 ND_RESE_DCR__FATAL_ERR__SE_RETRANS_CNT_OVER_THRESH_insert(enable,0);
	 ND_RESE_DCR__FATAL_ERR__RE_LINK_ERR_OVER_THRESH_insert(enable,0);
	 
       }
       
       fw_tracedDcrWrite(TRACE_ND, ND_RESE_DCR(i,FATAL_ERR_ENABLE),enable);

       // read it back to see if get what was written
       check_val = DCRReadPriv(ND_RESE_DCR(i,FATAL_ERR_ENABLE));
       if ( check_val != ND_RESE_DCR__FATAL_ERR_ENABLE__VALUE_set(enable)) return (103+ (int) i);
     }


   }

   return rc;



 }



/********************************************************************************
 * fw_nd_setup_gea : setup GEA lane mapping
 ********************************************************************************/
// mapping : PUEA listens only lane 0 by default (i.e. catches machine check errors)
// 
// lane 1 : ND int 1 : non-critical (non-fatal) interrupts 
// lane 0 : ND int 0 : critical (fatal) interrupts

int fw_nd_setup_gea(Personality_t *p)
{
  uint64_t u;
  int rc = 0;
  unsigned i;

  // Enable counting of receiver errors
  for (i=0;i< ND_RESE_DCR_num;i++) {
    if ( fw_nd_get_enable_sender(i,p)) {
       fw_tracedDcrWrite(TRACE_ND,ND_RESE_DCR(i, CLEAR), 0);
    }
  }

  // MAP1 reg (ND int 1) : non-critical
  u = DCRReadPriv(GEA_DCR(GEA_INTERRUPT_MAP1)); 
  GEA_DCR__GEA_INTERRUPT_MAP1__ND_MAP1_insert(u, 1); // ND interrupt #1 -> lane 1
  fw_tracedDcrWrite(TRACE_ND, GEA_DCR(GEA_INTERRUPT_MAP1), u);

  // MAP1 reg (ND int 0) : critical
  u = DCRReadPriv(GEA_DCR(GEA_INTERRUPT_MAP1));
  GEA_DCR__GEA_INTERRUPT_MAP1__ND_MAP0_insert(u, 0); // ND interrupt #0 -> lane 0
  fw_tracedDcrWrite(TRACE_ND, GEA_DCR(GEA_INTERRUPT_MAP1), u);

#ifndef FW_PREINSTALLED_GEA_HANDLERS

  uint64_t mask[3] = { ND_GEA_MASK_0, ND_GEA_MASK_1, ND_GEA_MASK_2 };
  fw_installGeaHandler( fw_nd_machineCheckHandler, mask );
  
#endif

  uint64_t controlHigh = ND_X2_DCR__X2_INTERRUPTS_CONTROL_LOW__LOCAL_RING_set(1);

  DCRWritePriv( ND_X2_DCR( X2_INTERRUPTS_CONTROL_LOW ), controlHigh );


  DCRWritePriv( ND_X2_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		// [5470] ND_X2_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] ND_X2_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		ND_X2_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		ND_X2_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );

  controlHigh = ND_500_DCR__N500_INTERRUPTS_CONTROL_LOW__LOCAL_RING_set(1);

  DCRWritePriv( ND_500_DCR( N500_INTERRUPTS_CONTROL_LOW ), controlHigh );


  DCRWritePriv( ND_500_DCR(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), 
		// [5470] ND_500_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		// [5470] ND_500_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		ND_500_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		ND_500_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		0 );



  for ( i = 0; i < ND_RESE_DCR_num; i++ ) {

    controlHigh = ND_RESE_DCR__RESE_INTERRUPTS_CONTROL_LOW__LOCAL_RING_set(1);

    DCRWritePriv( ND_RESE_DCR( i,  RESE_INTERRUPTS_CONTROL_LOW ), controlHigh );


    DCRWritePriv( ND_RESE_DCR( i, INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH ), 
		  // [5470] ND_RESE_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | 
		  // [5470] ND_RESE_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | 
		  ND_RESE_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | 
		  ND_RESE_DCR__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | 
		  0 );

  }
  return rc;

}



#include "_fw_nd.h"


typedef struct _ND_CorrectableValue {
  uint64_t statusMask;
  uint32_t counter;    // DCR number of the counter
  uint32_t threshold;  // DCR number of the threshold
  uint64_t clearMask;
  uint32_t rasMsgId;
} ND_CorrectableValue_t;

ND_CorrectableValue_t ND_RESE_CORRECTABLES[] = {
  { 
    ND_RESE_DCR__FATAL_ERR__RE_LINK_ERR_OVER_THRESH_set(1),    
    ND_RESE_DCR__RE_LINK_ERR_CNT_offset,  
    ND_RESE_DCR__THRESH_RE_ERR_offset,    
    ND_RESE_DCR__CLEAR__RE_CLR_LINK_ERR_CNT_set(1),
    FW_RAS_ND_RE_LINK_ERROR

  },
  {
    ND_RESE_DCR__FATAL_ERR__SE_RETRANS_CNT_OVER_THRESH_set(1), 
    ND_RESE_DCR__SE_RETRANS_CNT_offset,   
    ND_RESE_DCR__THRESH_SE_RETRAN_offset,
    ND_RESE_DCR__CLEAR__SE_CLR_RETRANS_CNT_set(1),
    FW_RAS_ND_SE_RETRANS_ERROR
  },
  { 
    ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x80),   
    ND_RESE_DCR__CE_COUNT_offset + 0,     
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(0x80),
    FW_RAS_ND_RE_CE_ERROR
  },  // VC0 receiver
  { 
    ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x40),
    ND_RESE_DCR__CE_COUNT_offset + 1,  
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(0x40),
    FW_RAS_ND_RE_CE_ERROR
  },  // VC1 receiver
  {
    ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x20),
    ND_RESE_DCR__CE_COUNT_offset + 2,  
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(0x20),
    FW_RAS_ND_RE_CE_ERROR
  },  // VC2 receiver
  {
    ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x10),
    ND_RESE_DCR__CE_COUNT_offset + 3,
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(0x10),
    FW_RAS_ND_RE_CE_ERROR
  },  // VC3 receiver
  {
    ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x08), 
    ND_RESE_DCR__CE_COUNT_offset + 4,
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(0x08),
    FW_RAS_ND_RE_CE_ERROR
  },  // VC4 receiver
  {
    ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x04), 
    ND_RESE_DCR__CE_COUNT_offset + 5, 
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(0x04),
    FW_RAS_ND_RE_CE_ERROR
  },  // VC5 receiver
  {
    ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x02), 
    ND_RESE_DCR__CE_COUNT_offset + 6, 
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(0x02),
    FW_RAS_ND_RE_CE_ERROR
  },  // VC0 list
  {
    ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x01),
    ND_RESE_DCR__CE_COUNT_offset + 7,
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(0x01),
    FW_RAS_ND_RE_CE_ERROR
  },  // bypass
  { 
    ND_RESE_DCR__FATAL_ERR__SE_CE_CNT_OVER_THRESH_set(1),
    ND_RESE_DCR__CE_COUNT_offset + 8,
    ND_RESE_DCR__THRESH_ARRAY_CE_offset,
    ND_RESE_DCR__CLEAR__SE_CLR_CE_CNT_set(1),
    FW_RAS_ND_RE_CE_ERROR
  },  // sender retransmission
};


//#define SIMULATE_ERR 1

int fw_nd_handleReseCorrectables( uint64_t* details, unsigned* n, ND_MachineCheckData_t* mcd, unsigned unit, uint64_t dcr, uint64_t data ) {

  int i;
  int diagsMode = PERS_ENABLED( PERS_ENABLE_DiagnosticsMode );
  Personality_t* personality = FW_PERSONALITY_PTR();

  if ( diagsMode ) {
      details[(*n)++] = dcr;
      details[(*n)++] = data;
  }


  // Determine the base address for this unit's DCRs.
  // NOTE: RESET is at offset 0

  unsigned dcrBase = ND_RESE_DCR(unit,RESET);    

  uint64_t fatalErr = DCRReadPriv( ND_RESE_DCR(unit,FATAL_ERR) );
  uint64_t clearOperation = 0;

#ifdef SIMULATE_ERR
  //fatalErr = ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x20);
#endif

  for ( i = 0; i < sizeof(ND_RESE_CORRECTABLES) / sizeof(ND_RESE_CORRECTABLES[0]); i++ ) {

    if ( ( fatalErr & ND_RESE_CORRECTABLES[i].statusMask ) != 0 ) {

	if ( diagsMode ) {
	    details[ (*n)++ ] = dcrBase + ND_RESE_CORRECTABLES[i].counter;
	    details[ (*n)++ ] = DCRReadPriv( dcrBase + ND_RESE_CORRECTABLES[i].counter );
	}
	else {
	    fw_uint64_t ceDetails[5];
	    int dimension = unit/2;
	    ceDetails[0] = unit;
	    ceDetails[1] = DCRReadPriv( dcrBase + ND_RESE_CORRECTABLES[i].counter );
	    ceDetails[2] = (dimension < 5) ? TI_GET_TORUS_DIM_REVERSED(dimension,personality->Network_Config.NetFlags2) : 0;
	    ceDetails[3] = ND_RESE_DCR(unit,FATAL_ERR);
	    ceDetails[4] = fatalErr & ND_RESE_CORRECTABLES[i].statusMask;
	    fw_machineCheckRas( ND_RESE_CORRECTABLES[i].rasMsgId, ceDetails, sizeof(ceDetails)/sizeof(ceDetails[0]), __FILE__, __LINE__ );
	}
      
      clearOperation |= ND_RESE_CORRECTABLES[i].clearMask;
      //DCRWritePriv( dcrBase + ND_RESE_CORRECTABLES[i].counter, 0 );
    }
  }

  // To clear errors, we must pulse a 1 and then a 0 on the affected
  // bits (the clear bits are sticky)

  uint64_t clear = DCRReadPriv( ND_RESE_DCR(unit,CLEAR) );
  DCRWritePriv( ND_RESE_DCR(unit,CLEAR), clear | clearOperation );
  ppc_msync();
  DCRWritePriv( ND_RESE_DCR(unit,CLEAR), clear );

  return 0;
}


int fw_nd_handle500Err5Correctables( uint64_t* details, unsigned* n, ND_MachineCheckData_t* mcd, unsigned unit /* not used */ , uint64_t dcr, uint64_t data) {

  unsigned i;
  uint64_t fatalErr5 = DCRReadPriv(ND_500_DCR(FATAL_ERR5));
  uint64_t clearOperation = 0;

  details[(*n)++] = dcr;
  details[(*n)++] = data;

#ifdef SIMULATE_ERR
  fatalErr5  =   ND_500_DCR__FATAL_ERR5__INJ_FIFO_D_CE_OVER_THRESH_set(1);
#endif

  // The "correctable over threshold" bits occupy the first 9 bits of the
  // register and correspond, in order, to the CE_CNT arrray.

  for ( i = 0; i <= 8; i++ ) {

    if ( i == 7 ) continue;     // Bit 7 is reserved:

    if (  ( fatalErr5 & _BN(i) ) != 0 ) {

      details[(*n)++] = ND_500_DCR(CE_CNT) + i;
      details[(*n)++] = DCRReadPriv(ND_500_DCR(CE_CNT) + i);

      // ###################################################################
      // Can't do this ... counters are read-only
      // DCRWritePriv( ND_500_DCR(CE_CNT) + i, 0 );
      // ###################################################################

      clearOperation |= _BN(i);
    }
  }

  // Clear counters and status bits bit pulsing the CTRL_CLEAR1 register.
  
  uint64_t clear = DCRReadPriv( ND_500_DCR(CTRL_CLEAR1) );
  DCRWritePriv( ND_500_DCR(CTRL_CLEAR1), clear | clearOperation );
  ppc_msync();
  DCRWritePriv( ND_500_DCR(CTRL_CLEAR1), clear );

  // Mask the errors so that they are not reported again:
  DCRWritePriv( ND_500_DCR(FATAL_ERR_ENABLE)+5, DCRReadPriv(ND_500_DCR(FATAL_ERR_ENABLE)+5) & ~clearOperation );

  return 0;
}




#define IS_BEDRAM(x)         ( ( (uint64_t)(x) & 0xFFFFFFFFFFFC0000ul ) == 0x000003FFFFFC0000ul )
//#define IS_MU_DCR_ADDRESS(x) ( ( (x) >= MU_DCR_base ) && ( (x) < (MU_DCR_base + MU_DCR_size) ) )

typedef int (*ND_CustomHandler_t)(fw_uint64_t* details, unsigned* n, ND_MachineCheckData_t* mcd, unsigned uint, uint64_t dcr, uint64_t data);


int fw_nd_testStatus( uint64_t dcr, uint64_t data, ND_MachineCheckData_t* mcd, unsigned pass, unsigned unit, fw_uint64_t* details, unsigned* n ) {

  int rc = 0;

  //printf("(*) %s dcr=%lX data=%lX pass=%d unit=%d addtl=%lX\n", __func__, dcr, data, pass, unit, mcd->level[pass].additionalInfo);
  if ( IS_BEDRAM(mcd->level[pass].additionalInfo) ) {
    ND_CustomHandler_t func = (ND_CustomHandler_t)mcd->level[pass].additionalInfo;
    rc |= func( details, n, mcd, unit, dcr, data );
  }
  else {
      details[(*n)++] = dcr;
      details[(*n)++] = data;
  }

  return rc;
}

int fw_nd_machineCheckHandler( uint64_t status[] ) {

  fw_uint64_t details[32];
  unsigned i, j, k, n = 0;
  int rc = 0;

  // +------------------------------------------------------------------------+
  // | IMPLEMENTATION NOTE: We execute a two-pass walk of the machine check   |
  // |     data table.  The first pass handles situations that are not fatal; |
  // |     the second pass handles fatal situations.  This allows non-fatal   |
  // |     RAS events to be written to the mailbox before a fatal event is    |
  // |     written and potentially brings down the node.                      |
  // +------------------------------------------------------------------------+

  for ( i = 0 ; i < 2; i++ ) {

    n = 0; // Clear out the RAS details array for this pass.

    for ( j = 0; j < sizeof(ND_500_x2_MachineCheck_Data)/sizeof(ND_500_x2_MachineCheck_Data[0]); j++ ) {

      ND_MachineCheckData_t* mcd = &(ND_500_x2_MachineCheck_Data[j]);

      // Skip over this entry if it doesn't have any interesting interrupt
      // bits for this pass:

      if ( mcd->level[i].mask != 0 ) {

	// Read the specified interrupt status DCR:
	uint64_t data = DCRReadPriv( mcd->dcrInterrupt );
	uint64_t enabled = -1;

	if ( mcd->dcrEnable != 0 ) {
	    enabled = DCRReadPriv( mcd->dcrEnable );
	}


#ifdef SIMULATE_ERR
	if ( (i==1) && ( mcd->dcrInterrupt == ND_500_DCR(FATAL_ERR5) ) ) {
	    data =   ND_500_DCR__FATAL_ERR5__RCP_FIFO_C1_CE_OVER_THRESH_set(1);
        }
#endif

	if ( ( data & enabled & mcd->level[i].mask ) != 0 ) {
	  rc |= fw_nd_testStatus( mcd->dcrInterrupt, data & enabled, mcd, i, 0, details, &n );
	}
      }
    }

    for ( j = 0; j < sizeof(ND_RESE_MachineCheck_Data)/sizeof(ND_RESE_MachineCheck_Data[0]); j++ ) {
      
      ND_MachineCheckData_t* mcd = &(ND_RESE_MachineCheck_Data[j]);

      // Skip over this entry if it doesn't have any interesting interrupt
      // bits for this pass:

      //printf("(*) %s:%d i=%d j=%d mask=%lX\n", __func__, __LINE__, i, j, mcd->level[i].mask);

      if ( mcd->level[i].mask != 0 ) {

	for ( k = 0; k < ND_RESE_DCR_num; k++ ) {
	  
	  uint64_t dcr = ND_RESE_DCR(k,RESET) + mcd->dcrInterrupt;

	  // Read the specified interrupt status DCR:
	  uint64_t data = DCRReadPriv( dcr );
	  uint64_t enabled = -1;

	  if ( mcd->dcrEnable != 0 ) {
	      enabled = DCRReadPriv( ND_RESE_DCR(k,RESET) + mcd->dcrEnable );
	  }


#ifdef SIMULATE_ERR
	  //if ( (i==1) && (j==0) && (k==4) ) {
	  //    printf("(*) Simulating error data=%lX\n",data);
	  //  data = ND_RESE_DCR__FATAL_ERR__RE_CE_CNT_OVER_THRESH_set(0x20);
	  //}
#endif

	  if ( ( data & enabled & mcd->level[i].mask ) != 0 ) {
	    rc |= fw_nd_testStatus( dcr, data & enabled, mcd, i, k,  details, &n );
	  }
	}
      }
    }

    // If there are RAS details on this pass, issue the event:

    if ( n > 0 ) {

      fw_uint32_t rasEvent = FW_RAS_ND_ERROR;

      if ( i == 0 ) {
	  rasEvent =  FW_RAS_ND_WARNING;
      }
      else {
	  rc |= -1;
      }

      fw_machineCheckRas( rasEvent, details, n, __FILE__, __LINE__ );
    }
  }

  return rc;
}


void fw_nd_order_dimensions(Personality_t *p, int *dim_order, int *effective_len)
  
{

  // takes personality as inputs, returns dim_order and effective_len
  // dim_order[0] is the id of the longest  dimension,  eg A (= 0)
  // ...
  // dim_order[4] is the id of the shortest dimension,  eg E (= 4)

  // effective_len[0] is the effective length of the longest dimension,
  // if it is n nodes in a dimension, the effective_len is n if a mesh, n/2 if a torus 
  // ...
  // effective_len[4] is the effective length of the shortest  dimension,  

  int i,j,tmp_id,tmp_len;
  int is_torus[5],nodes[5];
  fw_nd_get_is_torus(p,is_torus);
  fw_nd_get_nodes(p,nodes);

  // initialize dim_order and effective_len
  for ( i=0;i<5;i++) 
  {
    dim_order[i] = i;
    effective_len[i] = nodes[i]/(1+is_torus[i]);
  }
  
  // sort the dimensions, via simple bubble sort, break ties in favor of mesh
  int do_swap;
  for ( i=0;i<5;i++) 
  {
    for ( j=i+1;j<5;j++)
    {
      do_swap = 0;
      if (  effective_len[j] > effective_len[i] ) do_swap = 1;
      if (  ( effective_len[j] == effective_len[i] ) && ( is_torus[j] == 0)) do_swap =1;
      if (do_swap)
      {
	tmp_id = dim_order[j];
	tmp_len = effective_len[j];
	dim_order[j] = dim_order[i];
	effective_len[j] = effective_len[i];
	dim_order[i] = tmp_id;
	effective_len[i] = tmp_len;
      }
    }
    
  }
  
  
}
  

void  fw_nd_compute_det_order_and_zone_masks(Personality_t *p)
  
{

  // compute the deterministic routing order, and the zone routing masks 
  // and store them into the personality
  int dim_order[5];
  int effective_len[5];
  fw_nd_order_dimensions(p,dim_order,effective_len);  // get the dimension order and effective lengths
  // the masks are 5 bits,  eg 10000 means A
#define set_zone_mask_bit(d) _BN(59+d)

  
  // longest to shortest order for deterministic routing

  // store these into the appropriate personality bits
  uint64_t net_flags = p->Network_Config.NetFlags; // save the netflags
  // make sure the det order bits are all 0 to start with
  uint64_t  det_order_masks = 0;
  int i;
  for (i=0;i<5;i++) det_order_masks |= ND_DET_ORDER_MASK(i,-1);
  net_flags = net_flags & ~det_order_masks;
  // or in the masks, eg, if dim_order[0] = 1, then set_zone_mask_bit(dim_order[0]) = 01000;
  for (i=0;i<5;i++) net_flags = net_flags |  ND_DET_ORDER_MASK(i,set_zone_mask_bit(dim_order[i])); 
  p->Network_Config.NetFlags = net_flags;  // store it back into the personality
  
  // now compute the zone masks, and store them in the personality
  // From personality.h
  // Getter and Setter.  z=zone (0..3), m=mask (0..4), v=value, nc=pointer to Network_Config structure
  // ND_SET_DYNAMIC_ZONE_MASK(z,m,v,nc)

  // Zero them out first
  for (i=0;i<4;i++) p->Network_Config.ZoneRoutingMasks[i] = 0;

  int mask_num = 0;
  int curr_len = effective_len[0];
  uint32_t zone_mask[5] = {0,0,0,0,0};
  

  // zone id  0 is longest to shortest
  // TODO:  experiment if should bump mask_num if changing from torus to mesh, or vice versa
  for (i=0;i<5;i++) 
  {
    if ( effective_len[i] == curr_len) 
    {
      // or this into the current mask
      zone_mask[mask_num] |= set_zone_mask_bit(dim_order[i]);
    }
    else
    {
      //  bump up the mask_num and save this one into it
      mask_num++;
      zone_mask[mask_num] |= set_zone_mask_bit(dim_order[i]);
      curr_len =  effective_len[i];
      
    }

  }

  for (i=0;i<5;i++)
  {
    ND_SET_DYNAMIC_ZONE_MASK( MUHWI_PACKET_ZONE_ROUTING_0,i,zone_mask[i],&(p->Network_Config)); // save into personality
    zone_mask[i] = 0;  // zero it out for the next set of masks
  }
  
 
  // zone id 1 is unconstrained dynamic routing, only have to set mask 0
  ND_SET_DYNAMIC_ZONE_MASK(1,0,0xFF,&(p->Network_Config)); // save into personality

  // zone id 2 is shortest to longest
  curr_len = effective_len[4];
  mask_num = 0;
  
  for (i=4;i>=0;i--) 
  {
    if (effective_len[i] == curr_len) 
    {
      // or this into the current mask
      zone_mask[mask_num] |= set_zone_mask_bit(dim_order[i]);
    }
    else
    {
      //  bump up the mask_num and save this one into it
      mask_num++;
      zone_mask[mask_num] |= set_zone_mask_bit(dim_order[i]);
      curr_len = effective_len[i];
      
    }
  }

#if ZONE_2_IS_SHORTEST_TO_LONGEST

  for (i=0;i<5;i++)
  {
    ND_SET_DYNAMIC_ZONE_MASK(2,i,zone_mask[i],&(p->Network_Config)); // save into personality
  }

  // zone id 3 is same as determinstic routing
  for (i=0;i<5;i++)
  {
    ND_SET_DYNAMIC_ZONE_MASK(3,i,set_zone_mask_bit(dim_order[i]),&(p->Network_Config)); 
  }

#else

  // Replace zone 2 with the old zone 3 - deterministic ordering
  for (i=0;i<5;i++)
  {
    ND_SET_DYNAMIC_ZONE_MASK(2,i,set_zone_mask_bit(dim_order[i]),&(p->Network_Config)); 
  }

  // New zone 3 mask includes E dimension in all zones (effectively it only adds it to the first zone)
  for (i=0;i<5;i++)
  {
    ND_SET_DYNAMIC_ZONE_MASK(3,i,set_zone_mask_bit(dim_order[i]) | set_zone_mask_bit(4),&(p->Network_Config));
  }

#endif
  

}
 




uint64_t fw_nd_get_dcr_neighbors(Personality_t *p)
{
  // Set the coordinates of the "plus" and" minus"  neighbors:  
  int coords[5];
  int nodes[5];
  fw_nd_get_coords(p,coords);
  fw_nd_get_nodes(p,nodes); 
  int n_plus,n_minus,i;
  uint64_t dcr_val =0;

  for(i=0;i<5;i++)
  {
    n_plus  = 0;
    n_minus = 0;
    
    if (nodes[i])
    {
      n_plus  = (coords[i]+1) % nodes[i];            // ok even if mesh
      n_minus = (coords[i]+nodes[i]-1) % nodes[i];
      switch(i)
      {
      case 0:
	ND_500_DCR__CTRL_NEIGHBOR_COORDS__A_PLUS_insert (dcr_val, n_plus);
	ND_500_DCR__CTRL_NEIGHBOR_COORDS__A_MINUS_insert(dcr_val, n_minus);
	break;
      case 1:
	ND_500_DCR__CTRL_NEIGHBOR_COORDS__B_PLUS_insert (dcr_val, n_plus);
	ND_500_DCR__CTRL_NEIGHBOR_COORDS__B_MINUS_insert(dcr_val, n_minus);
	break;
      case 2:
	ND_500_DCR__CTRL_NEIGHBOR_COORDS__C_PLUS_insert (dcr_val, n_plus);
	ND_500_DCR__CTRL_NEIGHBOR_COORDS__C_MINUS_insert(dcr_val, n_minus);
	break;
      case 3:
	if ( (fw_nd_get_is_ionode(p) == 0) ||
	     ( (fw_nd_get_is_ionode(p) ) && (fw_nd_get_use_port67_for_io(p) == 0 ) ) )
	{
	  // not an io node, or io node but some D link not attached to compute nodes
	  ND_500_DCR__CTRL_NEIGHBOR_COORDS__D_PLUS_insert (dcr_val, n_plus);
	  ND_500_DCR__CTRL_NEIGHBOR_COORDS__D_MINUS_insert(dcr_val, n_minus);
	}
	break;
      default:
	if  (fw_nd_get_is_ionode(p) == 0) 
	{
	  ND_500_DCR__CTRL_NEIGHBOR_COORDS__E_PLUS_insert (dcr_val, n_plus);
	  ND_500_DCR__CTRL_NEIGHBOR_COORDS__E_MINUS_insert(dcr_val, n_minus);	  
	}
      } 
      
    }
  }
  return dcr_val;
}



  
int  fw_nd_is_multinode(Personality_t *p)
  
{

  // return 1 multiple nodes involved, else 0
  
  if ( ND_TORUS_SIZE( p->Network_Config ) > 1) return 1; // multiple node torus
  int loopback[6];
  fw_nd_get_loopback(p,loopback);
  // for some testing configurations, could come up with a 1 node torus
  // but the IO link is not in loopback, so something is on other end of the link
  if ( loopback[5] == 0) return 1;  
  return 0;
  
}


//#define SIMULATE_ERR

void fw_nd_flushCorrectables( void ) {

    extern void fw_semaphore_down( size_t lock_number );
    extern void fw_semaphore_up( size_t lock_number );

    fw_semaphore_down( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK );

    int i, j;
    fw_uint64_t details[8];
    Personality_t* personality = FW_PERSONALITY_PTR();

    for ( i = 0; i < ND_RESE_DCR_num; i++ ) {

	details[1] = DCRReadPriv( ND_RESE_DCR(i, RE_LINK_ERR_CNT) );

	if ( details[1] > 0 ) {
	    details[0] = i;
	    details[2] = ( (i/2) < 5 ) ? TI_GET_TORUS_DIM_REVERSED( (i/2),personality->Network_Config.NetFlags2) : 0;
	    fw_writeRASEvent( FW_RAS_ND_RE_LINK_ERROR, 3, details );
	}
	
	details[1] = DCRReadPriv( ND_RESE_DCR(i, SE_RETRANS_CNT) );

	if ( details[1] > 0 ) {
	    details[0] = i;
	    details[2] = ( (i/2) < 5 ) ? TI_GET_TORUS_DIM_REVERSED( (i/2),personality->Network_Config.NetFlags2) : 0;
	    fw_writeRASEvent( FW_RAS_ND_SE_RETRANS_ERROR, 3, details );
	}

	for ( j = 0; j < ND_RESE_DCR__CE_COUNT_range; j++ ) {

	    details[1] = DCRReadPriv( ND_RESE_DCR(i, CE_COUNT) + j );

	    if ( details[1] > 0 ) {
		details[0] = i;
		details[2] = ( (i/2) < 5 ) ? TI_GET_TORUS_DIM_REVERSED( (i/2),personality->Network_Config.NetFlags2) : 0;
		details[3] = ND_RESE_DCR(i, CE_COUNT) + j;
		fw_writeRASEvent( FW_RAS_ND_RE_CE_ERROR, 4, details );
	    }
	}

	
	// To clear errors, we must pulse a 1 and then a 0 on the affected bits (the clear bits are sticky)

	uint64_t clear = DCRReadPriv( ND_RESE_DCR(i,CLEAR) );
	uint64_t mask = 
	    ND_RESE_DCR__CLEAR__RE_CLR_LINK_ERR_CNT_set(1) | 
	    ND_RESE_DCR__CLEAR__SE_CLR_RETRANS_CNT_set(1) |
	    ND_RESE_DCR__CLEAR__RE_CLR_CE_CNT_set(-1) |
	    ND_RESE_DCR__CLEAR__SE_CLR_CE_CNT_set(1);

	DCRWritePriv( ND_RESE_DCR(i,CLEAR), clear | mask );
	ppc_msync();
	DCRWritePriv( ND_RESE_DCR(i,CLEAR), clear );
    }

    fw_semaphore_up( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK );
}
