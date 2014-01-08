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
 * Mappings for Designersim vs Fullchip compatibility
 *
 * Stefan Koch <stk@de.ibm.com>
 *
 *****************************************************************************/
#ifndef   _BGQ_SD_HELPER_H_
#define   _BGQ_SD_HELPER_H_

#include <firmware/include/fwext/fwext_lib.h>
#include <firmware/include/fwext/serdes.h>

#ifndef _BGQ_SD_HELPER_C_
extern int use_a,  use_b,  use_c,  use_d,  use_e,  use_io;
extern int use_am, use_bm, use_cm, use_dm, use_em;
extern int use_ap, use_bp, use_cp, use_dp, use_ep;
#else
int use_a,  use_b,  use_c,  use_d,  use_e,  use_io;
int use_am, use_bm, use_cm, use_dm, use_em;
int use_ap, use_bp, use_cp, use_dp, use_ep;
#endif

void     personality_convert_map(Personality_t *p);
void     sd_dcr_wr(int addr, uint64_t val);
uint64_t sd_dcr_rd(int addr);
void     dcr_write   (int addr, uint64_t val, int, int, int, int);
uint64_t dcr_read(int addr, int, int, int);
void     hss_init();
void     sd_get_frequencies(uint32_t *coreFrequency, uint32_t *networkFrequency);

#define dcr_addr_t             uint32_t
#define dcr_val_t              uint64_t

#define sd_get_std_bit16(bit)       (1     << (bit))
#define sd_std_move_upto(val,bit)   ((val) << (bit))
#define sd_ibm_move64_upto(val,bit) ((val) << (63-(bit)))
#define sd_get_ibm_bit64(bit)       (0x8000000000000000ULL >> bit)

#endif /* _BGQ_SD_HELPER_H_ */
