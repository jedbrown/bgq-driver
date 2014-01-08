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
/******************************************************************************
 * Lib to access Rambus Configuration Bus
 *
 * Stefan Koch <stk@de.ibm.com>
 *
 *****************************************************************************/
#ifndef   _BGQ_SD_RCB_H_
#define   _BGQ_SD_RCB_H_

#include "sd_helper.h"

#define RCB_BT_PCS      2
#define RCB_BT_LANE     1
#define RCB_BT_SUPPORT  0

#define rcb_addr_t	uint32_t
#define rcb_val_t       uint16_t

void      rcb_set_excluded_link(int);
void      rcb_write(int fam, int link, int type, int lane, int reg, uint16_t val);
rcb_val_t rcb_read(int fam, int link, int type, int lane, int reg);
void      rcb_reset_shadows();
void      rcb_set_debug_level(int);

#endif /* _BGQ_SD_RCB_H_ */
