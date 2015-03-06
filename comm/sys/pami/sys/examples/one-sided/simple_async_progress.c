
/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#include <assert.h>
#include <stdio.h>

#include "simple_async_progress.h"


/* ***************************************************************************
 * The following typedefs are defined by the pami async_progress extension, but
 * must be declared by the application code using the extension. 
 * ***************************************************************************/

/*
 * Async progress can be enabled for various event types. In practice it is not
 * necessary to differentiate between async progress event types and the only
 * value that should be used is ASYNC_PROGRESS_EVENT_ALL.
 */
typedef enum
{
  ASYNC_PROGRESS_EVENT_ALL            =    0,
  ASYNC_PROGRESS_EVENT_RECV_INTERRUPT =    1,
  ASYNC_PROGRESS_EVENT_TIMER          =    2,
  ASYNC_PROGRESS_EVENT_EXT            = 1000
} async_progress_event_t;

/*
 * The 'progress function' call signature...
 */
typedef void (*async_progress_function) (pami_context_t context, void * cookie);

/*
 * The 'progress register' function associates function pointers with the
 * progress, suspend, and resume events for a particular context.
 */
typedef pami_result_t (*async_progress_register_function) (pami_context_t            context,
                                                           async_progress_function   progress_fn,
                                                           async_progress_function   suspend_fn,
                                                           async_progress_function   resume_fn,
                                                           void                    * cookie);

/*
 * The 'progress enable' function starts async progress for a particular
 * context.
 */
typedef pami_result_t (*async_progress_enable_function) (pami_context_t         context,
                                                         async_progress_event_t event);

/*
 * The 'progress disable' function stops async progress for a particular
 * context.
 */
typedef pami_result_t (*async_progress_disable_function) (pami_context_t         context,
                                                          async_progress_event_t event);


/* ************************************************************************** */
/* ************************************************************************** */


/**
 * \brief Async progress 'suspend' event function
 *
 * This function is invoked by the async progress extension when a context
 * will no longer be advance asynchronously.  This can be due to an initial
 * condition that disallows async progress, a runtime change that makes async
 * progress temporarily unavailable, or an active disable of async progress
 * for a context.
 *
 * \param [in] context The communcation context that is no longer under async progress
 * \param [in] cookie  Registered cookie
 */
void async_progress_suspend_function (pami_context_t context, void * cookie)
{
  fprintf (stdout, "(%03d) async progress is now SUSPENDED for the context. For Blue Gene/Q, starting in V1R2M0, the environment variable 'PAMI_COMMTHREAD_SLEEP=100' can be used to enable async progress when hardware threads are over-subscribed.\n", __LINE__);
  
  exit (1);
}


/**
 * \brief Async progress 'resume' event function
 *
 * This function is invoked by the async progress extension when a context
 * will now be advance asynchronously.  This can be due to an initial async
 * progress enable or a runtime change that resolves a temporary unavailablity
 * of async progresse.
 *
 * \param [in] context The communcation context that is now under async progress
 * \param [in] cookie  Registered cookie
 */
void async_progress_resume_function (pami_context_t context, void * cookie)
{
  fprintf (stdout, "(%03d) async progress has RESUMED for context\n", __LINE__);
}


/* ************************************************************************** */
/* ************************************************************************** */


/**
 * \brief Open the 'async progress' extension
 *
 * \param [in] client The PAMI client to open the extension against
 *
 * \return async progress extension handle
 */
pami_extension_t simple_async_progress_open (pami_client_t client)
{
  pami_extension_t extension;
  pami_result_t result = PAMI_ERROR;
  result = PAMI_Extension_open (client, "EXT_async_progress", &extension);
  assert (result == PAMI_SUCCESS);

  return extension;
}


/**
 * \brief Close the 'async progress' extension
 *
 * \param [in] extension The async progress extension handle
 */
void simple_async_progress_close (pami_extension_t extension)
{
  pami_result_t result = PAMI_ERROR;
  result = PAMI_Extension_close (extension);
  assert (result == PAMI_SUCCESS);

  return;
}



/**
 * \brief Enable 'async progress' for a communication context
 *
 * This is a blocking operation. This function will not return until
 * asynchronous progress has been enabled for the communication context.
 *
 * Asynchronous progress is enabled without the use of an explicit 'progress'
 * function. This means that the context will be locked, via PAMI_Context_lock(),
 * and the lock will not be released until asynchronous progress is disabled.
 * Consequently, all new communication must be initiated using the
 * thread-safe PAMI_Context_post() function to post 'work' to the context.
 *
 * \note This simple example does not illustrate the use of an explicit
 *       'progress function'.
 *
 * \param [in] extension The async progress extension handle
 * \param [in] context   The communication context to be advanced asynchronously
 */
void simple_async_progress_enable (pami_extension_t extension,
                                   pami_context_t   context)
{
  pami_result_t result;

  /*
   * Query the async progress register function.
   */
  async_progress_register_function register_fn = NULL;
  register_fn = (async_progress_register_function)
    PAMI_Extension_symbol (extension, "register");
  assert (register_fn != NULL);


  /*
   * Register the async progress event handlers for this context.
   */
  result = PAMI_ERROR;
  result = register_fn (context,
                        NULL,    /* progress function */
                        async_progress_suspend_function,
                        async_progress_resume_function,
                        NULL);   /* cookie */
  assert (result == PAMI_SUCCESS);


  /*
   * Query the async progress enable function.
   */
  async_progress_enable_function enable_fn = NULL;
  enable_fn = (async_progress_enable_function)
    PAMI_Extension_symbol (extension, "enable");
  assert (enable_fn != NULL);


  /*
   * Enable async progress for this context.
   */
  result = PAMI_ERROR;
  result = enable_fn (context, ASYNC_PROGRESS_EVENT_ALL);
  assert (result == PAMI_SUCCESS);


  /*
   * Async progress is enabled when the async progress extension acquires the
   * context lock.
   */
  fprintf (stdout, "(%03d) Waiting for the async progress extension to acquire the context lock.\n", __LINE__);
  do
  {
    result = PAMI_ERROR;
    result = PAMI_Context_trylock (context);
    assert (result != PAMI_ERROR);

    if (result == PAMI_SUCCESS)
      PAMI_Context_unlock (context);
  }
  while (result == PAMI_SUCCESS);
  fprintf (stdout, "(%03d) Async progress enabled for the context.\n", __LINE__);

  return;
}


/**
 * \brief Disable 'async progress' for a communication context
 *
 * This is a blocking operation. This function will not return until
 * asynchronous progress has been disabled for the communication context.
 *
 * \param [in] extension The async progress extension handle
 * \param [in] context   The communication context to be advanced asynchronously
 */
void simple_async_progress_disable (pami_extension_t extension,
                                    pami_context_t   context)
{
  pami_result_t result;

  /*
   * Sanity check that the context lock is held by the async progress extension.
   */
  result = PAMI_ERROR;
  result = PAMI_Context_trylock (context);
  assert (result == PAMI_EAGAIN);


  /*
   * Query the async progress disable function.
   */
  async_progress_disable_function disable_fn = NULL;
  disable_fn = (async_progress_disable_function)
    PAMI_Extension_symbol (extension, "disable");
  assert (disable_fn != NULL);


  /*
   * Disable async progress for this context.
   */
  result = PAMI_ERROR;
  result = disable_fn (context, ASYNC_PROGRESS_EVENT_ALL);
  assert (result == PAMI_SUCCESS);


  /*
   * Block until the context is no longer under async progress. This occurs
   * when the context lock is released ?
   */
  fprintf (stdout, "(%03d) Waiting to acquire the context lock.\n", __LINE__);
  PAMI_Context_lock (context);
  fprintf (stdout, "(%03d) Acquired the context lock - async progress is now disabled.\n", __LINE__);

  return;
};

