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
 * Serdes Lib Header File
 * Reused some code from fw_nd header file (Phil et.al.)
 *
 * Stefan Koch <stk@de.ibm.com>
 *****************************************************************************/
#ifndef   _BGQ_SERDES_H_
#define   _BGQ_SERDES_H_

#include <firmware/include/fwext/fwext_lib.h>



#ifdef _BGQ_SERDES_INIT_C_
int libfw_sd_debug=0;
int sd_en_halfrate=0;
#else
extern int libfw_sd_debug;
extern int sd_en_halfrate;
#endif

void serdes_init(Personality_t *p);
void serdes_pcie_init(Personality_t *p);
void sd_set_halfrate(int enable);
void hss_pcie_init(Personality_t *p);
void hss_init();
void hss_setup(int); //0-all torus, 1-torus + pcie
void hss_set_debug_level(int);
void hss_set_sim_mode(int);
void hss_enable_all_links(void);

uint32_t nd_get_loopback_mode();
uint32_t sd_get_loopback_mode();

#endif /* SERDES_H */
