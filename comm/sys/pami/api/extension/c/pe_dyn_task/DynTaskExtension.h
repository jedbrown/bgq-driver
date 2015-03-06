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
 * \file api/extension/c/pe_dyn_task/DynTaskExtension.h
 * \brief PAMI extension "dynamic tasking extension" interface
 */
#ifndef __api_extension_c_pe_dyn_task_DynTaskExtension_h__
#define __api_extension_c_pe_dyn_task_DynTaskExtension_h__

#include <pami.h>

enum {PAMIX_CLIENT_DYNAMIC_TASKING=1032, PAMIX_CLIENT_WORLD_TASKS};

namespace PAMI
{

  class DynTaskExtension
  {

    public:

      DynTaskExtension();

      static pami_result_t client_dyn_task_query(pami_client_t           client,
                                                 pami_configuration_t    config[],
                                                 size_t                  num_configs );


  }; // class DynTaskExtension

}; // namespace PAMI

#endif // __api_extension_c_pe_dyn_task_DynTaskExtension_h__
