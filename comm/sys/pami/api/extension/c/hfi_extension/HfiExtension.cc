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
///
/// \file api/extension/c/hfi_extension/HfiExtension.cc
/// \brief ???
///
#include "HfiExtension.h"
#include "pami.h"

extern internal_rc_t _dbg_hfi_perf_counters(lapi_handle_t hndl, lapi_pkt_counter_t* cnt,
        bool is_dump);
extern int LAPI_Remote_update(lapi_handle_t ghndl, uint count,
        lapi_remote_update_t *info);

pami_result_t
PAMI::HfiExtension::hfi_pkt_counters (pami_context_t context,
        hfi_pkt_counter_t *counters)
{

  internal_rc_t rst =  _dbg_hfi_perf_counters( ((Context*)context)->my_hndl,
          (lapi_pkt_counter_t*)counters, false);

  return PAMI_RC( rst );
}

//TODO replace LAPI function with an internal func

pami_result_t
PAMI::HfiExtension::hfi_remote_update (pami_context_t context,
        uint count, hfi_remote_update_info_t* info)
{
  int rst = LAPI_Remote_update( ((Context*)context)->my_hndl,
          count, (lapi_remote_update_t*)info);

  if (rst == 0)
      return PAMI_SUCCESS;
  else
      return PAMI_ERROR;
}
