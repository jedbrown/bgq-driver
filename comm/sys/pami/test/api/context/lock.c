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
 * \file test/api/context/lock.c
 * \brief Simple PAMI_Context_lock() test
 */

#include <pthread.h>
#include <pami.h>
#include <stdio.h>

#define MAX_THREAD_NUM 16
// common data
volatile int keys = 0;

void* thread_routine(void* input)
{
    pami_result_t           res = PAMI_ERROR;
    pami_context_t          ctx = (pami_context_t)input;
    int                     my_key;

    // get the lock
    do {
        res = PAMI_Context_trylock (ctx);
    } while (res != PAMI_SUCCESS);
    
    // modify the common data
    my_key = keys;
#if DBG
    fprintf (stderr, 
             "%d: changed common data from %d to %d\n",
             pthread_self(), my_key, my_key+1);
#endif
    my_key ++;
    keys = my_key;

    // release the lock
    res = PAMI_Context_unlock (ctx);
    if (res != PAMI_SUCCESS)
    {
        fprintf (stderr, 
                "Error. Unable to unlock the pami context. result = %d\n", 
                res);
    }

    pthread_exit(NULL);
}

int main (int argc, char ** argv)
{
    pami_client_t           client;
    pami_context_t          context;
    pami_result_t           result = PAMI_ERROR;
    pami_configuration_t*   configuration = NULL;
    char                    cl_string[] = "TEST";
    pthread_t               threads[MAX_THREAD_NUM];
    int                     i, rc;

    result = PAMI_Client_create (cl_string, &client, NULL, 0);
    if (result != PAMI_SUCCESS)
    {
        fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", result);
        return 1;
    }

    result = PAMI_Context_createv(client, configuration, 0, &context, 1);
    if (result != PAMI_SUCCESS)
    {
        fprintf (stderr, "Error. Unable to create the pami context. result = %d\n", result);
        return 1;
    }

    /* Test a context lock */
    result = PAMI_Context_lock (context);
    if (result != PAMI_SUCCESS)
    {
        fprintf (stderr, "Error. Unable to lock the pami context. result = %d\n", result);
        return 1;
    }

    /* Test a context unlock */
    result = PAMI_Context_unlock (context);
    if (result != PAMI_SUCCESS)
    {
        fprintf (stderr, "Error. Unable to unlock the pami context. result = %d\n", result);
        return 1;
    }

    // test context trylock with multiple threads
    for (i = 0; i < MAX_THREAD_NUM; i ++) {
        rc = pthread_create(&threads[i], NULL, thread_routine, (void*)context);
        if (rc) {
            fprintf (stderr, 
                    "Error. Unable to create %d-th thread with rc %d\n",
                    i, rc);
            return 1;
        }
    }

    // join all the created threads
    for (i = 0; i < MAX_THREAD_NUM; i ++) {
        do {
            rc = pthread_join(threads[i], NULL);
            if (rc) {
                fprintf (stderr, 
                         "Error. Unable to join %d-th thread with rc %d\n",
                         i, rc);
            }
        } while (0 != rc);
    }

    // check the common data
    if ( MAX_THREAD_NUM == keys ) {
        fprintf (stderr, 
                 "Trylock with multiple threads passed. %d (%d expected)\n",
                 keys, MAX_THREAD_NUM);
    } else {
        fprintf (stderr, 
                 "Error. Trylock with multiple threads failed. %d (%d expected)\n",
                 keys, MAX_THREAD_NUM);
    }

    /* Destroy the context */
    result = PAMI_Context_destroyv(&context, 1);
    if (result != PAMI_SUCCESS)
    {
        fprintf (stderr, "Error. Unable to destroy the pami context. result = %d\n", result);
        return 1;
    }

    /* Finalize (destroy) the client */
    result = PAMI_Client_destroy(&client);
    if (result != PAMI_SUCCESS)
    {
        fprintf (stderr, "Error. Unable to finalize pami client. result = %d\n", result);
        return 1;
    }

    fprintf (stderr, "Success.\n");

    return 0;
};
