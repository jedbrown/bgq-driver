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
 * \file api/extension/c/pe_dyn_task/DynTaskExtension.cc
 * \brief PAMI extension "dynamic tasking" PE implementation
 */

#include "api/extension/c/pe_dyn_task/DynTaskExtension.h"
#include "../lapi/include/Context.h"


PAMI::DynTaskExtension::DynTaskExtension() {
}

pami_result_t PAMI::DynTaskExtension::client_dyn_task_query(
			             pami_client_t        client,
                                     pami_configuration_t config[],
                                     size_t               num_configs ){
    pami_result_t rc = PAMI_SUCCESS;
    LapiImpl::Client *lapi_client = (LapiImpl::Client *)client;                                                                 
    size_t i;
    for (i=0; i< num_configs; i++) {
        rc = lapi_client->DynTaskQuery(config[i] );
    }

    return rc;
};
