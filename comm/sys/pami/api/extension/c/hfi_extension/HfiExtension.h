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
 * \file api/extension/c/hfi_extension/HfiExtension.h
 * \brief PAMI extension "HFI" interface
 */
#ifndef __api_extension_c_hfi_extension_HfiExtension_h__
#define __api_extension_c_hfi_extension_HfiExtension_h__

#include <stdio.h>
#include <pami.h>
#include "pami_ext_hfi.h"
#include "lapi.h"
#include "Context.h"

namespace PAMI
{
  class HfiExtension
  {
    public:
      static pami_result_t hfi_pkt_counters (pami_context_t context,
              hfi_pkt_counter_t *counters);

      static pami_result_t hfi_remote_update (pami_context_t context,
              uint count, hfi_remote_update_info_t* info);
  };
};

#endif // __api_extension_c_hfi_extension_HfiExtension_h__
