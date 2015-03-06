
/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */

#ifndef __simple_async_progress_h__
#define __simple_async_progress_h__


#include <pami.h>


/**
 * \brief Open the 'async progress' extension
 *
 * \param [in] client The PAMI client to open the extension against
 *
 * \return async progress extension handle
 */
pami_extension_t simple_async_progress_open (pami_client_t client);


/**
 * \brief Close the 'async progress' extension
 *
 * \param [in] extension The async progress extension handle
 */
void simple_async_progress_close (pami_extension_t extension);


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
                                   pami_context_t   context);


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
                                    pami_context_t   context);


#endif /* __simple_async_progress_h__ */

