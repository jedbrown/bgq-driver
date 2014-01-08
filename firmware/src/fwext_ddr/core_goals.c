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
#include <firmware/include/fwext/sd_init_auto.h>
#include <firmware/include/fwext/sd_helper.h>
uint64_t get_goal_mask ()
{
	return use_io << 10 |
	     use_ep << 9 |
	     use_em << 8 |
	     use_dp << 7 |
	     use_dm << 6 |
	     use_cp << 5 |
	     use_cm << 4 |
	     use_bp << 3 |
	     use_bm << 2 |
	     use_ap << 1 |
	     use_am << 0 ;
}

int use_left ()
{
	return  use_am + use_ap + use_bm + use_bp + use_cm + use_cp;
}

int use_right ()
{
	return use_dm + use_dp + use_em + use_ep + use_io;
}
