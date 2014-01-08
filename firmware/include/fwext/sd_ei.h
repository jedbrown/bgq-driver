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
/* Serdes error injection */

#ifndef SD_EI_H
#define SD_EI_H

#define ERROR_ERR_LOWER_LIMIT_UNDERCUT   0x001 // less errors detected as expected
#define ERROR_ERR_UPPER_LIMIT_EXCEEDED   0x002 // more errors detected as expected
#define ERROR_ERR_MISSING_IRQ            0x004 // missing error count over thresh signal
#define ERROR_ERR_UNEXPECTED_IRQ         0x008 // unexpected error count over thresh signal
#define ERROR_ERR_CLEAR                  0x010 // error count clearing didn't work

#define ERROR_RET_LOWER_LIMIT_UNDERCUT   0x020 // less retransmissions detected as expected
#define ERROR_RET_UPPER_LIMIT_EXCEEDED   0x040 // more retransmissions detected as expected
#define ERROR_RET_MISSING_IRQ            0x080 // missing retransmission count over thresh signal
#define ERROR_RET_UNEXPECTED_IRQ         0x100 // unexpected retransmission count over thresh signal
#define ERROR_RET_CLEAR                  0x200 // retransmision count clearing didn't work

#define ND_DISABLE_RCV_ACK               0x0
#define ND_ENABLE_RCV_ACK                0x1

void     sd_ei_init(const uint32_t link_enable[], const uint32_t error_thresh[], const uint32_t retrans_thresh[], uint32_t rcv_ack);
void     sd_ei_set_interval(uint32_t left_min, uint32_t left_max, uint32_t right_min, uint32_t right_max);
void     sd_ei_activate();//bool rcv_ack);
void     sd_ei_deactivate();
void     sd_ei_reset();
uint32_t sd_ei_check();

uint64_t sd_get_error_count(uint32_t link);
uint64_t sd_get_retrans_count(uint32_t link);

#endif
