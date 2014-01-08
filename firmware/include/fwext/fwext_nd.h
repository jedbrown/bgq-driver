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
#ifndef   _BGQ_FW_ND_H_ // Prevent multiple inclusion
#define   _BGQ_FW_ND_H_

#include <firmware/include/fwext/fwext_nd_dcr.h>
#include <firmware/include/fwext/fwext_lib.h>



extern int FW_ND_VERBOSE;

extern int  fw_nd_reset_release(Personality_t *p);
extern int  fw_nd_term_check(Personality_t *p);

extern inline void fw_nd_set_verbose(int v)
{
  FW_ND_VERBOSE =v;
}




// below will go away, or FW_ND_VERBOSE will be 0, or will be a personality bit


extern inline void fw_nd_verbose_DCRWritePriv(uint64_t addr, uint64_t val)
{
  DCRWritePriv( (addr),(val));
  if (FW_ND_VERBOSE)  printf(" dcr write address= 0x%016lx dcr val = 0x%016lx \n",
		       (addr),(val));
}


extern int inline fw_nd_test_dcr(uint64_t dcr_addr, uint64_t dcr_val, uint64_t  dcr_val_exp )               // used!
  
{
  // write dcr_val to dcr_addr, read it back and compare it to
  // its expected value dcr_val_exp (often the same as dcr_val)
  // return 0 if success, else 1
  int rc=0;
  fw_nd_verbose_DCRWritePriv(dcr_addr,dcr_val);
  uint64_t dcr_val_out = DCRReadPriv(dcr_addr);
  if ( dcr_val_exp != dcr_val_out)
  {
    rc=1;
    printf(" Error: dcr_addr= 0x%016lx dcr_val= 0x%016lx dcr_val_out= 0x%016lx \n",
	   dcr_addr,dcr_val,dcr_val_out);
    
  }

  return rc;
  
}


#endif
