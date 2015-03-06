/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file api/extension/c/hfi_extension/pami_ext_hfi.h
 * \brief HFI specified interfaces
 */

#ifndef __api_extension_c_hfi_extension_pami_ext_hfi_h__
#define __api_extension_c_hfi_extension_pami_ext_hfi_h__

#include "pami.h"

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct {
    unsigned long long  total_packets_sent;
    unsigned long long  packets_send_drop;
    unsigned long long  indicate_packet_sent;
    unsigned long long  total_packets_recv;
    unsigned long long  packets_recv_drop;
    unsigned long long  indicate_packet_recv;
    unsigned long long  immediate_packet_sent;
  } hfi_pkt_counter_t;

  typedef struct {
      uint               dest;            /* Destination of operation */
      uint               op;              /* Atomic operation type    */
      unsigned long long dest_buf;        /* buffer on destination    */
      unsigned long long atomic_operand;  /* operand value for        */
      /* atomic  operation        */
  } hfi_remote_update_info_t;

  typedef pami_result_t (*hfi_pkt_counters_fn) (pami_context_t context,
          hfi_pkt_counter_t *counters);
  typedef pami_result_t (*hfi_remote_update_fn) (pami_context_t context,
          uint count, hfi_remote_update_info_t* info);

#ifdef __cplusplus
}
#endif

#endif // __pami_ext_hfi_h__
