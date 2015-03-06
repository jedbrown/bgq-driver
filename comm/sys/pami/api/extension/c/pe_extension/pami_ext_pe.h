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
 * \file api/extension/c/pe_extension/pami_ext_pe.h
 * \brief platform-specific messaging interface
 * \todo  put this into percs subdir and rename to pami_ext.h
 */
#ifndef __api_extension_c_pe_extension_pami_ext_pe_h__
#define __api_extension_c_pe_extension_pami_ext_pe_h__

#include "pami.h"
#ifndef PAMI_EXT_ATTR
#define PAMI_EXT_ATTR 1000 /**< starting value for extended attributes */
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /*****************************************************************************/
  /**
   * \defgroup ext_percs PERCS specific extensions to core PAMI
   *
   * This extension defines features that are available only on PERCS and
   * InfiniBand platforms.
   * \{
   */
  /*****************************************************************************/

  /**
   * \defgroup ext_attr Extended attributes for configuration
   *
   * Explicit cast from \ref pami_attribute_name_ext_t into
   * \ref pami_attribute_name_t may be needed.
   *
   * \{
   */

  typedef enum {
    /* Attribute                     usage : type : default : description   */

    PAMI_ACTIVE_CLIENT             /**<  Q : pami_active_client_t : N/A : retrieve all active clients */
        = PAMI_EXT_ATTR,

    PAMI_CLIENT_RESERVED1          /**<  Q : N/A : N/A : Reserved */
        = PAMI_EXT_ATTR + 100,
    PAMI_CLIENT_THREAD_SAFE,       /**< CQ : bool : true : run in thread-safe mode */
    PAMI_CLIENT_PROTOCOL_NAME,     /**<  Q : string : N/A : retrieve protocol name of the client*/
    PAMI_CLIENT_ACTIVE_CONTEXT,    /**<  Q : pami_active_context_t : N/A : retrieve all active contexts in the client */

    PAMI_CONTEXT_CHECK_PARAM       /**< CQU: bool : true : check function parameters */
        = PAMI_EXT_ATTR + 200,
    PAMI_CONTEXT_ERROR_HANDLER,    /**< CQ : pami_error_handler_t : NULL : asynchronous error handler */
    PAMI_CONTEXT_STATISTICS,       /**<  Q : pami_statistics_t : N/A : retrieve communication statistics */
    PAMI_CONTEXT_TRIGGER,          /**<   U: pami_trigger_t : N/A : add or remove a trigger */
    PAMI_CONTEXT_NETWORK,          /**<  Q : string : N/A : retrieve network string */
    PAMI_CONTEXT_MAX_PKT_SZ,       /**<  Q : size_t : N/A : payload size of 1 packet */
    PAMI_CONTEXT_RFIFO_SZ,         /**<  Q : size_t : N/A : receiving fifo size */
    PAMI_CONTEXT_BULK_MIN_MSG_SZ,  /**<  Q : size_t : N/A : size of smallest RDMA message*/
    PAMI_CONTEXT_BULK_XFER,        /**<  Q : bool   : N/A : run in RDMA mode */
    PAMI_CONTEXT_SHM_ENABLED,      /**<  Q : bool   : N/A : run in SHM mode*/
    PAMI_CONTEXT_ACK_THRESH,       /**<  Q : size_t : N/A : threshold of ACKs sent back to src */
    PAMI_CONTEXT_REXMIT_BUF_CNT,   /**<  Q : size_t : N/A : number of retransmission buffers */
    PAMI_CONTEXT_REXMIT_BUF_SZ,    /**<  Q : size_t : N/A : retransmission buffer  size */
    PAMI_CONTEXT_RC_MAX_QP,        /**<  Q : size_t : N/A : max number of RC QPs */
    PAMI_CONTEXT_RC_USE_LMC,       /**<  Q : bool   : N/A : run with one RC QP per path */
    PAMI_CONTEXT_COPY_ROUTINE,     /**<  Q : pami_copy_routine_t : N/A : function addresses of 3 copy routines */
  } pami_attribute_name_ext_t;

  /** \} */ /* end of "ext_attr" group */

  /**
   * \defgroup error_handler Asynchronous error handler
   * \{
   */

  /**
   * \brief PAMI asynchronous error handler
   *
   * In general, an asynchronous error handler should terminate the process when
   * invoked. If the handler returns, the process will not function normally.
   *
   * \param [in]  context        The PAMI context
   * \param [in]  result         The result (error code)
   *
   */
  typedef void (pami_error_handler_t)( pami_context_t  context,
                                       pami_result_t   result);

  /** \} */ /* end of "error_handler" group */

  /**
   * \defgroup comm_stat Communication statistics
   * \{
   *
   * \ref PAMI_Context_query with \c PAMI_CONTEXT_STATISTICS returns in
   * \ref pami_attribute_value_t.chararray a pointer to \ref pami_statistics_t
   * whose memory is managed by PAMI internally.
   *
   * \c counters field in \ref pami_statistics_t is a variable-length array.
   */

  #define PAMI_COUNTER_NAME_LEN  40   /**< Maximum counter name length */

  /**
   * \brief Counter for statistics
   */
  typedef struct
  {
    char               name[PAMI_COUNTER_NAME_LEN]; /**< Counter name  */
    unsigned long long value;                       /**< Counter value */
  } pami_counter_t;

  /**
   * \brief Array of counters for statistics
   */
  typedef struct
  {
    int                count;       /**< Number of counters */
    pami_counter_t     counters[1]; /**< Array of counters  */
  } pami_statistics_t;

  /** \} */ /* end of "comm_stat" group */

  /**
   * \defgroup trigger User-defined triggers
   * \{
   *
   * \ref PAMI_Context_update with \c PAMI_CONTEXT_TRIGGER passes in
   * \ref pami_attribute_value_t.chararray as a pointer to \ref pami_trigger_t
   * to add or remove a user-defined trigger.
   *
   * \c NULL as trigger_func in \ref pami_statistics_t indicates removal of the
   * trigger with name specified in trigger_name.
   */

  #define PAMI_VOID_CONTEXT  NULL   /**< PAMI context for indirect PAMI user */

  /**
   * \brief Signature of trigger functions
   *
   * \retval 0 If trigger function finishes successfully.
   * \retval Non-0 Otherwise.
   */
  typedef int (*pami_trigger_func_t) (
    pami_context_t     context,
    int                argc,
    char               *argv[]);

  /**
   * \brief Triggers
   */
  typedef struct
  {
    pami_trigger_func_t trigger_func;
    char                *trigger_name;
  } pami_trigger_t;

  /** \} */ /* end of "trigger" group */

  /**
   * \defgroup act_clients Active PAMI clients
   * \{
   *
   * \ref PAMI_Context_query with \c PAMI_ACTIVE_CLIENT returns in
   * \ref pami_attribute_value_t.chararray a pointer to 
   * \ref pami_active_client_t whose memory is managed by PAMI internally.
   *
   */
  

  #define PAMI_VOID_CLIENT      NULL   /**< PAMI client used to open PE Extension before any client is created */

  /**
   * \brief Struct user get after query with PAMI_ACTIVE_CLIENT
   */
  typedef struct
  {
    size_t        client_num;
    pami_client_t clients[1];
  } pami_active_client_t; 
  
  /** \} */ /* end of "act_clients" group */

  /**
   * \defgroup act_context Active PAMI Context in a Client
   * \{
   *
   * \ref PAMI_Context_query with \c PAMI_ACTIVE_CONTEXT returns in
   * \ref pami_attribute_value_t.chararray a pointer to 
   * \ref pami_active_context_t whose memory is managed by PAMI internally.
   *
   */

  /**
   * \brief Struct user get after query with PAMI_ACTIVE_CONTEXT
   */
  typedef struct
  {
    size_t         context_num;
    pami_context_t contexts[1];
  } pami_active_context_t; 
  
  /** \} */ /* end of "act_clients" group */


  /**
   * \defgroup copy_routine
   * \{
   *
   * \ref PAMI_Context_query with \c PAMI_CONTEXT_COPY_ROUTINE returns in
   * \ref pami_attribute_value_t.chararray a pointer to \ref pami_copy_routine_t
   * whose memory is managed by PAMI internally.
   *
   */

  typedef void (copyRtn) (void *,void*,size_t);

  /**
   * \brief Addresses for the copy routines
   */
  typedef struct
  {
      copyRtn *normal_copy;
      copyRtn *copy_to_shm;
      copyRtn *copy_from_shm;
  } pami_copy_routine_t;

  /** \} */ /* end of "copy_routine" group */


  /*****************************************************************************/
  /**
   * \defgroup mutex_cond Context mutex and condition
   *
   * This extension defines more functions that are similar to
   * pthread_mutex_* and pthread_cond_* functions.
   * \{
   */
  /*****************************************************************************/

  typedef void * pami_cond_t;  /**< Condition */

  /**
   * \brief Get the owner of the context lock
   *
   * \param[in]  context PAMI communication context
   * \param[out] owner   Owner of the context lock, compatible to pthread_t
   *
   * \retval PAMI_SUCCESS  The mutex owner has been retrieved.
   * \retval PAMI_INVAL    The \c context or the \c owner pointer is invalid.
   */
  pami_result_t PAMI_Context_mutex_getowner (pami_context_t context, unsigned long *owner);

  /*
   * The following functions are modelled after pthread_cond_* functions.
   */
  /**
   * \brief Create a condition
   *
   * \param[in]  context PAMI communication context
   * \param[out] cond    The condition created
   *
   * \retval PAMI_SUCCESS  The condition has been created.
   * \retval PAMI_INVAL    The \c context or the \c cond pointer is invalid.
   */
  pami_result_t PAMI_Context_cond_create (pami_context_t context, pami_cond_t *cond);

  /**
   * \brief Wait on a condition
   * \note  The caller must have the context lock. Upon return, the caller
   *        still has the lock.
   *
   * \param[in]  context PAMI communication context
   * \param[in]  cond    The condition to wait
   *
   * \retval PAMI_SUCCESS  The condition has been met.
   * \retval PAMI_INVAL    The \c context or the \c cond is invalid.
   */
  pami_result_t PAMI_Context_cond_wait (pami_context_t context, pami_cond_t cond);

  /**
   * \brief Wait on a condition with timeout
   * \note  The caller must have the context lock. Upon return, the caller
   *        still has the lock.
   *
   * \param[in]  context PAMI communication context
   * \param[in]  cond    The condition to wait
   * \param[in]  time    The time to wait
   *
   * \retval PAMI_SUCCESS  The condition has been met.
   * \retval PAMI_EAGAIN   The wait time has expired.
   * \retval PAMI_INVAL    The \c context, the \c cond or the \c time pointer
   *                       is invalid.
   */
  pami_result_t PAMI_Context_cond_timedwait (pami_context_t context, pami_cond_t cond,
          struct timespec *time);

  /**
   * \brief Signal to wake up a waiter on a condition
   *
   * \param[in]  context PAMI communication context
   * \param[in]  cond    The condition to signal
   *
   * \retval PAMI_SUCCESS  The signal has been delivered.
   * \retval PAMI_INVAL    The \c context or the \c cond is invalid.
   */
  pami_result_t PAMI_Context_cond_signal (pami_context_t context, pami_cond_t cond);

  /**
   * \brief Signal to wake up all waiters on a condition
   *
   * \param[in]  context PAMI communication context
   * \param[in]  cond    The condition to sigal
   *
   * \retval PAMI_SUCCESS  The signal has been delivered.
   * \retval PAMI_INVAL    The \c context or the \c cond is invalid.
   */
  pami_result_t PAMI_Context_cond_broadcast (pami_context_t context, pami_cond_t cond);

  /**
   * \brief Destroy a condition
   *
   * \param[in]  context PAMI communication context
   * \param[in]  cond    The condition to destroy
   *
   * \retval PAMI_SUCCESS  The condition has been destroyed.
   * \retval PAMI_INVAL    The \c context or the \c cond is invalid.
   */
  pami_result_t PAMI_Context_cond_destroy (pami_context_t context, pami_cond_t cond);

  /**
   * \brief Function pointers fot the above member functions
   */
  typedef pami_result_t (*global_query_fn) (pami_configuration_t configuration[], size_t num_configs);
  typedef pami_result_t (*mutex_getowner_fn) (pami_context_t context, unsigned long *owner);
  typedef pami_result_t (*cond_create_fn) (pami_context_t context, pami_cond_t *cond);
  typedef pami_result_t (*cond_wait_fn) (pami_context_t context, pami_cond_t *cond);
  typedef pami_result_t (*cond_timedwait_fn) (pami_context_t context, pami_cond_t *cond);
  typedef pami_result_t (*cond_signal_fn) (pami_context_t context, pami_cond_t *cond);
  typedef pami_result_t (*cond_broadcast_fn) (pami_context_t context, pami_cond_t *cond);
  typedef pami_result_t (*cond_destroy_fn) (pami_context_t context, pami_cond_t *cond);

  /** \} */ /* end of "mutex_cond" group */
  /** \} */ /* end of "ext_percs" group */

#ifdef __cplusplus
}
#endif

#endif
