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
 * \file test/api/extensions/hfi/remote_update.c
 * \brief Simple test of HFI remote update in the PAMI HFI extension
 */

#include <stdio.h>
#include <string.h>
#include <pami.h>
#include <pami_ext_hfi.h>

#define DBG_FPRINTF(x) /* fprintf x */

#define RC(statement) \
{ \
    pami_result_t rc = statement; \
    if (rc != PAMI_SUCCESS) { \
        fprintf (stderr, #statement " rc = %d, line %d\n", \
                rc, __LINE__); \
        exit(1); \
    } \
}

#define MAX_TABLE_SZ      1024
unsigned long long        table[MAX_TABLE_SZ];

/* fence callback function */
void fence_done(void* ctxt, void* cdata, pami_result_t err) 
{
    (*(int*)cdata) ++;
}

/* barrier callback function */
void barrier_done(void* ctxt, void* cdata, pami_result_t err) 
{
    (*(int*)cdata) ++;
}

int main(int argc, char ** argv)
{
    pami_client_t         client;
    pami_context_t        context;
    pami_result_t         status = PAMI_ERROR;
    pami_configuration_t  pami_config;
    pami_geometry_t       world_geo;
    size_t                barrier_alg_num[2];
    pami_algorithm_t*     bar_always_works_algo = NULL;
    pami_metadata_t*      bar_always_works_md = NULL;
    pami_algorithm_t*     bar_must_query_algo = NULL;
    pami_metadata_t*      bar_must_query_md   = NULL;
    pami_xfer_t           barrier;
    int                   my_id;
    volatile int          is_fence_done   = 0;
    volatile int          is_barrier_done = 0;

    /* create PAMI client */
    RC( PAMI_Client_create("TEST", &client, NULL, 0) );
    DBG_FPRINTF((stderr,"Client created successfully at 0x%p\n",client));

    /* create PAMI context */
    RC( PAMI_Context_createv(client, NULL, 0, &context, 1) );
    DBG_FPRINTF((stderr,"Context created successfully at 0x%p\n",context));

    /* query my task id */
    bzero(&pami_config, sizeof(pami_configuration_t));
    pami_config.name = PAMI_CLIENT_TASK_ID;
    RC( PAMI_Client_query(client, &pami_config, 1) );
    my_id = pami_config.value.intval;
    DBG_FPRINTF((stderr,"My task id is %d\n", my_id));

    /* get the world geometry */
    RC( PAMI_Geometry_world(client, &world_geo) );
    DBG_FPRINTF((stderr,"World geometry is at 0x%p\n",world_geo));

    /* query number of barrier algorithms */
    RC( PAMI_Geometry_algorithms_num(world_geo, PAMI_XFER_BARRIER, 
                barrier_alg_num) );
    DBG_FPRINTF((stderr,"%d-%d algorithms are available for barrier op\n",
                barrier_alg_num[0], barrier_alg_num[1]));
    if (barrier_alg_num[0] <= 0) {
        fprintf (stderr, "Error. No (%lu) algorithm is available for barrier op\n",
                barrier_alg_num[0]);
        return 1;
    }

    /* query barrier algorithm list */
    bar_always_works_algo =
        (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*barrier_alg_num[0]);
    bar_always_works_md =
        (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*barrier_alg_num[0]);
    bar_must_query_algo =
        (pami_algorithm_t*)malloc(sizeof(pami_algorithm_t)*barrier_alg_num[1]);
    bar_must_query_md =
        (pami_metadata_t*)malloc(sizeof(pami_metadata_t)*barrier_alg_num[1]);

    RC( PAMI_Geometry_algorithms_query(world_geo, PAMI_XFER_BARRIER,
                bar_always_works_algo, bar_always_works_md, barrier_alg_num[0],
                bar_must_query_algo, bar_must_query_md, barrier_alg_num[1]) );
    DBG_FPRINTF((stderr,"Algorithm [%s] at 0x%p will be used for barrier op\n",
                bar_always_works_md[0].name, bar_always_works_algo[0]));

    /* begin PAMI fence */
    RC( PAMI_Fence_begin(context) );
    DBG_FPRINTF((stderr,"PAMI fence begins\n"));

    /* ------------------------------------------------------------------------ */

    pami_extension_t          extension;
    const char                ext_name[] = "EXT_hfi_extension";
    const char                sym_name[] = "hfi_remote_update";
    hfi_remote_update_fn      remote_update = NULL;
    hfi_remote_update_info_t  remote_info;
    pami_memregion_t          mem_region;
    size_t                    mem_region_sz = 0;
    unsigned long long        operand = 1234;
    unsigned long long        orig_val = 0;
    int                       offset = (operand)%MAX_TABLE_SZ;

    /* initialize table for remote update operation */
    int i;
    for (i = 0; i < MAX_TABLE_SZ; i ++) {
        table[i] = (unsigned long long) i;
    }
    orig_val = table[offset];

    /* open PAMI extension */
    RC( PAMI_Extension_open (client, ext_name, &extension) );
    DBG_FPRINTF((stderr,"Open %s successfully.\n", ext_name));

    /* load PAMI extension function */
    remote_update = (hfi_remote_update_fn) 
        PAMI_Extension_symbol (extension, sym_name);
    if (remote_update == (void *)NULL)
    {
        fprintf (stderr, "Error. Failed to load %s function in %s\n",
                 sym_name, ext_name); 
        return 1;
    } else {
        DBG_FPRINTF((stderr,"Loaded function %s in %s successfully.\n", 
                    sym_name, ext_name));
    }

    /* create a memory region for remote update operation */
    RC( PAMI_Memregion_create(context, table, 
                MAX_TABLE_SZ*sizeof(unsigned long long),
                &mem_region_sz, &mem_region) );
    DBG_FPRINTF((stderr,"%d-byte PAMI memory region created successfully.\n",
                mem_region_sz));

    /* perform a PAMI barrier */
    is_barrier_done = 0;
    barrier.cb_done = barrier_done;
    barrier.cookie = (void*)&is_barrier_done;
    barrier.algorithm = bar_always_works_algo[0];
    RC( PAMI_Collective(context, &barrier) );
    DBG_FPRINTF((stderr,"PAMI barrier op invoked successfully.\n"));
    while (is_barrier_done == 0)
        PAMI_Context_advance(context, 1000);
    DBG_FPRINTF((stderr,"PAMI barrier op finished successfully.\n"));

    RC( PAMI_Context_lock(context) );

    /* prepare remote update info */
    remote_info.dest = my_id^1;
    remote_info.op = 0;           /* op_add */
    remote_info.atomic_operand = operand;
    remote_info.dest_buf = (unsigned long long)(&(table[offset]));

    /* invoke remote update PAMI extension function */
    RC( remote_update(context, 1, &remote_info) );
    DBG_FPRINTF((stderr,"Function %s invoked successfully.\n", 
                sym_name));

    RC( PAMI_Context_unlock(context) );

    /* perform a PAMI fence */
    is_fence_done = 0;
    RC( PAMI_Fence_all(context, fence_done, (void*)&is_fence_done) );
    DBG_FPRINTF((stderr,"PAMI_Fence_all invoked successfully.\n")); 
    while (is_fence_done == 0)
        PAMI_Context_advance(context, 1000);
    DBG_FPRINTF((stderr,"PAMI_Fence_all finished successfully.\n")); 

    /* perform a PAMI barrier */
    is_barrier_done = 0;
    barrier.cb_done = barrier_done;
    barrier.cookie = (void*)&is_barrier_done;
    barrier.algorithm = bar_always_works_algo[0];
    RC( PAMI_Collective(context, &barrier) );
    DBG_FPRINTF((stderr,"PAMI barrier op invoked successfully.\n"));
    while (is_barrier_done == 0)
        PAMI_Context_advance(context, 1000);
    DBG_FPRINTF((stderr,"PAMI barrier op finished successfully.\n"));

    /* verify data after remote update operation */
    if (table[offset] != orig_val + operand) {
        printf("Data verification at offset %d with operand %lu failed: "
                "[%lu expected with %lu updated]\n",
                offset, operand, orig_val+operand, table[offset]);
    } else {
        printf("Data verification at offset %d with operand %lu passed: "
                "[%lu expected with %lu updated].\n",
                offset, operand, orig_val+operand, table[offset]);
    }

    /* destroy the memory region after remote update operation */
    RC( PAMI_Memregion_destroy(context, &mem_region) );
    DBG_FPRINTF((stderr,"PAMI memory region removed successfully.\n"));

    /* close PAMI extension */
    RC( PAMI_Extension_close (extension) );
    DBG_FPRINTF((stderr,"Close %s successfully.\n", ext_name));

    /* ------------------------------------------------------------------------ */

    /* end PAMI fence */
    RC( PAMI_Fence_end(context) );
    DBG_FPRINTF((stderr,"PAMI fence ends\n"));

    /* destroy PAMI context */
    RC( PAMI_Context_destroyv(&context, 1) );
    DBG_FPRINTF((stderr, "PAMI context destroyed successfully\n"));

    /* destroy PAMI client */
    RC( PAMI_Client_destroy(&client) );
    DBG_FPRINTF((stderr, "PAMI client destroyed successfully\n"));

    return 0;
}
