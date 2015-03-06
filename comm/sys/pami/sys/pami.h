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
/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/* ------------------------------------------------------------------------- */

/**
 * \file sys/pami.h
 * \brief PAMI
 */

#ifndef __pami_h__
#define __pami_h__

#include <stdlib.h>
#include <stdint.h>
#include <sys/uio.h>

#include "pami_sys.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * \brief PAMI_IN_PLACE definition.
   */
  #define PAMI_IN_PLACE ((void *) -1L)

  /**
   * \brief PAMI result status codes.
   */
  typedef enum
  {
    PAMI_SUCCESS =  0, /**< Successful execution        */
    PAMI_NERROR  = -1, /**< Generic error (-1)          */
    PAMI_ERROR   =  1, /**< Generic error (+1)          */
    PAMI_INVAL,        /**< Invalid argument            */
    PAMI_UNIMPL,       /**< Function is not implemented */
    PAMI_EAGAIN,       /**< Not currently available     */
    PAMI_ENOMEM,       /**< Out of memory               */
    PAMI_SHUTDOWN,     /**< Task has shutdown           */
    PAMI_CHECK_ERRNO,  /**< Check the errno val         */
    PAMI_OTHER,        /**< Other undefined error       */
    PAMI_RESULT_EXT = 1000 /**< Begin extension-specific values */
  } pami_result_t;

  typedef void*    pami_client_t;   /**< Client of communication contexts */
  typedef void*    pami_context_t;  /**< \ingroup context Context for data transfers       */
  typedef void*    pami_type_t;     /**< \ingroup datatype Description for data layout  */
  typedef uint32_t pami_task_t;     /**< Identity of a process            */
  typedef uint32_t pami_endpoint_t; /**< Identity of a context            */
  typedef void*    pami_geometry_t; /**< Geometry object handle           */
  typedef size_t   pami_algorithm_t;/**< Algorithm object handle          */

  /* The following are setters for various object handles                           */
  /* These allow PAMI users to set and test values for uninitialized object handles */
  extern pami_client_t    PAMI_CLIENT_NULL;    /**< Uninitialized client */
  extern pami_type_t      PAMI_TYPE_NULL;      /**< Uninitialized type */
  extern pami_context_t   PAMI_CONTEXT_NULL;   /**< Uninitialized context */
  extern pami_geometry_t  PAMI_GEOMETRY_NULL;  /**< Uninitialized geometry */
  extern pami_algorithm_t PAMI_ALGORITHM_NULL; /**< Uninitialized algorithm */
  extern pami_endpoint_t  PAMI_ENDPOINT_NULL;  /**< Uninitialized endpoint */
  extern size_t           PAMI_ALL_CONTEXTS;   /**< Select all endpoints in a task */

  /**
   * \brief Event callback function signature of application functions used to
   *        process message events on a communication context
   *
   * At the time an event function is invoked the application has already
   * protected access to the associated communication context, either by using
   * PAMI_Context_lock() or by some other mechanism. Consequently, application code
   * executed from within an event callback function has dedicated access to the
   * associated communication context and is \b not required to further protect
   * access to the context prior to invoking functions that operate on that
   * context.
   *
   * The event function is invoked as a result of an advance of the internal
   * progress engine of the associated communication context. The application
   * code executed from within an event callback function \b must \b not invoke
   * any progress function, such as PAMI_Context_advance(), to avoid corruption
   * of the internal progress engine state.
   *
   * \note Application code executed from within an event callback function does
   *       \b not have implicit dedicated access to other communication
   *       contexts. Operations on other communication contexts must be
   *       protected from re-entrant access from other execution resources by
   *       the application.
   *
   * \warning The application must take care to avoid deadlocks when protecting
   *          access to a communication context other than the communication
   *          context which invoked the event callback function. It is
   *          recommended that PAMI_Context_trylock() or PAMI_Context_post()
   *          be used instead of PAMI_Context_lock() in this situtation.
   *
   * \param [in] context   Communication context that invoked the callback
   * \param [in] cookie    Event callback application argument
   * \param [in] result    Asynchronous result information
   */
  typedef void (*pami_event_function) ( pami_context_t   context,
                                        void           * cookie,
                                        pami_result_t    result );

  /**
   * \brief Function to produce data at send side or consume data at receive side
   *
   * \ingroup datatype
   *
   * The default operation with a typed transfer is to copy data. A data
   * function, when associated with a type, allows a different way of handling
   * data. For example, one can write a data function to perform data reduction
   * instead of data copy.
   *
   * Unlike memory copy, a data function may not be able to handle byte data.
   * Atom size is defined as the minimum unit size that a data function
   * can accept. For example, a data function for summing up doubles may
   * require the input to be an integral number of doubles, thus the atom
   * size for this data function is \c sizeof(double).
   *
   * When a data function is used with a typed transfer, one must make sure
   * the atom size of the data function divides the atom size of the type.
   *
   * \param [in,out] target  The address of a contiguous target buffer
   * \param [in]     source  The address of a contiguous source buffer
   * \param [in]     bytes   The number of bytes to handle
   * \param [in]     cookie  A user-specified value
   */
  typedef void (*pami_data_function) (void       * target,
                                      void       * source,
                                      size_t       bytes,
                                      void       * cookie);

  /**
   * \brief Work callback function signature of application functions used to
   *        process work posted to a communication context
   *
   * The work callback function, with the associated cookie, will continue to be
   * invoked during communication context advance until the function does not
   * return ::PAMI_EAGAIN.
   *
   * If the work callback function will return ::PAMI_SUCCESS then the
   * application may safely dispose of the pami_work_t object, or the
   * application may re-post the pami_work_t object if arrangements were made to
   * provide the location of the pami_work_t object to the work callback
   * function (e.g. via the cookie).
   *
   * If the work callback function will return ::PAMI_EAGAIN then the
   * application must \b not have altered the pami_work_t object in any way.
   *
   * \see PAMI_Context_post()
   *
   * At the time a work function is invoked the application has already
   * protected access to the associated communication context, either by using
   * PAMI_Context_lock() or by some other mechanism. Consequently, application code
   * executed from within a work callback function has dedicated access to the
   * associated communication context and is \b not required to further protect
   * access to the context prior to invoking functions that operate on that
   * context.
   *
   * The work function is invoked as a result of an advance of the internal
   * progress engine of the associated communication context. The application
   * code executed from within a work callback function \b must \b not invoke
   * any progress function, such as PAMI_Context_advance(), to avoid corruption
   * of the internal progress engine state.
   *
   * \note Application code executed from within a work callback function does
   *       \b not have implicit dedicated access to other communication
   *       contexts. Operations on other communication contexts must be
   *       protected from re-entrant access from other execution resources by
   *       the application.
   *
   * \warning The application must take care to avoid deadlocks when protecting
   *          access to a communication context other than the communication
   *          context which invoked the work callback function. It is
   *          recommended that PAMI_Context_trylock() or PAMI_Context_post()
   *          be used instead of PAMI_Context_lock() in this situtation.
   *
   * \param [in] context   Communication context that invoked this function
   * \param [in] cookie    Work callback application argument
   *
   * \return ::PAMI_SUCCESS  The work function will dequeue and stop running
   *
   * \return ::PAMI_EAGAIN   The work function will remain queued and will be
   *                         invoked on the next advance of the communication
   *                         context.
   */
  typedef pami_result_t (*pami_work_function)(pami_context_t context, void *cookie);

  typedef uintptr_t pami_work_t[PAMI_WORK_SIZE_STATIC/sizeof(uintptr_t)]; /**< Context work opaque object */


  /*****************************************************************************/
  /**
   * \defgroup configuration Configuration
   * \{
   */
  /*****************************************************************************/

  typedef char* pami_user_key_t;   /**< Char string as configuration key */
  typedef char* pami_user_value_t; /**< Char string as configuration value */

  /**
   * \brief User configuration data structure
   */
  typedef struct
  {
    pami_user_key_t    key;   /**< The configuration key   */
    pami_user_value_t  value; /**< The configuration value */
  } pami_user_config_t;

  /**
   * This enum contains ALL possible attributes for all hardware
   */
  typedef enum {
    /* Attribute            Query / Update                                 */
    PAMI_CLIENT_CLOCK_MHZ = 100,    /**< Q : size_t : Frequency of the CORE clock, in units of 10^6/seconds.  This can be used to approximate the performance of the current task. */
    PAMI_CLIENT_CONST_CONTEXTS,     /**< Q : size_t : All processes will return the same ::PAMI_CLIENT_NUM_CONTEXTS */
    PAMI_CLIENT_HWTHREADS_AVAILABLE,/**< Q : size_t : The number of HW threads available to a process without over-subscribing (at least 1) */
    PAMI_CLIENT_MEMREGION_SIZE,     /**< Q : size_t : Size of the pami_memregion_t handle in this implementation, in units of Bytes. */
    PAMI_CLIENT_MEM_SIZE,           /**< Q : size_t : Size of the core main memory, in units of 1024^2 Bytes    */
    PAMI_CLIENT_NUM_TASKS,          /**< Q : size_t : Total number of tasks        */
    PAMI_CLIENT_NUM_LOCAL_TASKS,    /**< Q : size_t : Number of tasks located on the same node as this task */
    PAMI_CLIENT_LOCAL_TASKS,        /**< Q : size_t[] : Array of all tasks located on the same node as this task */
    PAMI_CLIENT_NUM_CONTEXTS,       /**< Q : size_t : The maximum number of contexts allowed on this process */
    PAMI_CLIENT_PROCESSOR_NAME,     /**< Q : char[] : A unique name string for the calling process, and should be suitable for use by
                                              MPI_Get_processor_name(). The storage should *not* be freed by the caller. */
    PAMI_CLIENT_TASK_ID,            /**< Q : size_t : ID of this task (AKA "rank") */
    PAMI_CLIENT_WTIMEBASE_MHZ,      /**< Q : size_t : Frequency of the WTIMEBASE clock, in units of 10^6/seconds.  This can be used to convert from PAMI_Wtimebase to PAMI_Timer manually. */
    PAMI_CLIENT_WTICK,              /**< Q : double : This has the same definition as MPI_Wtick(). */
    PAMI_CLIENT_NONCONTIG,          /**< Q : bool : Include collective algorithms that support non-contiguous data in the world geometry algorithms list (true/default)
                                            *       Non-contiguous data support is not required so algorithms on the "always works" list *may* not support it (false) 
                                            *       This attribute may be ignored on some platforms.  */
    PAMI_CLIENT_MEMORY_OPTIMIZE,    /**< Q : size_t : PAMI Memory Optimize Level 0 - N (reduce memory usage as level increases).  This may result in reduced performance.  
                                            *       This attribute may be ignored on some platforms.  */

    PAMI_CONTEXT_DISPATCH_ID_MAX = 200, /**< Q : size_t : Maximum allowed dispatch id, see PAMI_Dispatch_set() */

    PAMI_DISPATCH_RECV_IMMEDIATE_MAX = 300, /**< Q : size_t : Maximum number of bytes that can be received, and provided to the application, in a dispatch function. */
    PAMI_DISPATCH_SEND_IMMEDIATE_MAX, /**< Q : size_t : Maximum number of bytes that can be transfered with the PAMI_Send_immediate() function. */
    PAMI_DISPATCH_ATOM_SIZE_MAX,    /**< Q : size_t : Maximum atom size that can be used with PAMI_Send_typed() function. */

    PAMI_GEOMETRY_OPTIMIZE = 400,   /**< Q : bool : Populate the geometry algorithms list with hardware optimized algorithms.  If the algorithms list
                                            *       is not optimized, point-to-point routines only will be present, and hardware resources will be released */
    PAMI_GEOMETRY_NONCONTIG,        /**< Q : bool : Include collective algorithms that support non-contiguous data in the geometry algorithms list (true/default)
                                            *       Non-contiguous data support is not required so algorithms on the "always works" list *may* not support it (false) 
                                            *       This attribute may be ignored on some platforms.  */
    PAMI_GEOMETRY_MEMORY_OPTIMIZE,  /**< Q : size_t : PAMI Memory Optimize Level 0 - N (reduce memory usage as level increases).  This may result in reduced performance.  
                                            *       This attribute may be ignored on some platforms.  */

    PAMI_TYPE_DATA_SIZE = 500,      /**< Q : size_t : The data size of a type */
    PAMI_TYPE_DATA_EXTENT,          /**< Q : size_t : The data extent of a type */
    PAMI_TYPE_ATOM_SIZE,            /**< Q : size_t : The atom size of a type */
    PAMI_ATTRIBUTE_NAME_EXT = 1000  /**< Begin extension-specific values */
  } pami_attribute_name_t;

  /**
   * \brief Attribute value data structure
   */
  typedef union
  {
    size_t         intval;          /**< integer value */
    double         doubleval;       /**< double value */
    const char   * chararray;       /**< char array value */
    const size_t * intarray;        /**< integer array value */
  } pami_attribute_value_t;

  /** \} */ /* end of "configuration" group */

  /**
   * \brief General purpose configuration structure.
   */
  typedef struct
  {
    pami_attribute_name_t  name;  /**< Attribute type */
    pami_attribute_value_t value; /**< Attribute value */
  } pami_configuration_t;

  /*****************************************************************************/
  /**
   * \defgroup Collectives Metadata
   * \{
   */
  /*****************************************************************************/

  /**
   *  This enum contains the "reason" codes that indicate why a particular collective
   *  operation could fail.  Upon querying the metadata of a particular collective
   *  the user may have a function pointer populated in the "check_fn" field of the
   *  metadata struct.  The user can pass in the call site parameters (the pami_xfer_t)
   *  of the collective into this function and receive a list of failures.
   */
  typedef union metadata_result_t
  {
    unsigned bitmask;                 /**< Raw bitmask data                    */
    struct
    {
      unsigned unspecified       : 1; /**< Unspecified failure                 */
      unsigned range             : 1; /**< Send/Recv bytes are out of range    */
      unsigned align_send_buffer : 1; /**< Send buffer must be aligned         */
      unsigned align_recv_buffer : 1; /**< Receive buffer must be aligned      */
      unsigned datatype_op       : 1; /**< Datatype/operation not valid        */
      unsigned contiguous_send   : 1; /**< Send data must be contiguous        */
      unsigned contiguous_recv   : 1; /**< Receive data must be contiguous     */
      unsigned continuous_send   : 1; /**< Send data must be continuous        */
      unsigned continuous_recv   : 1; /**< Receive data must be continuous     */
      unsigned nonlocal          : 1; /**< The protocol associated with this
                                           metadata requires "nonlocal"
                                           knowledge, meaning that the result
                                           code from the check_fn must be
                                           allreduced to determine if the
                                           operation will work:
                                           0:no, >0:  requires nonlocal data   */

    } check;                          /**< Metadata 'reason' information       */
  } metadata_result_t;

  /** Forward declaration of pami_xfer_t */
  typedef struct pami_xfer_t *xfer_p;

  /**
   * \brief Function signature for metadata queries
   *
   * \param [in] in A "call-site"   collective query parameters
   * \retval pami_metadata_check  failure code, 0=success, nonzero
   *                               bits are OR'd from pami_metadata_check_t
   */
  typedef metadata_result_t (*pami_metadata_function) (struct pami_xfer_t *in);

  /**
   * \brief A metadata structure to describe a collective protocol
   *
   * \b Correctness \b Parameters
   *
   * If an algorithm is placed on the "must query list", then the user must use the metadata
   * to determine if the protocol can be issued given the call site parameters of the
   * collective operation.
   *
   * This may include calling the check_fn function, which takes as input a pami_xfer_t
   * corresponding to the call site parameters.  The call site is defined as the code location
   * where the user will call the PAMI_Collective function.
   *
   * The semantics for when the user must call the check_fn function are as follows:
   * If check_fn is NULL, and checkrequired is 0, the user cannot and is not required
   * to call the check_fn function.  The user is still required to check the other bits
   * in the bitmask_correct to determine if the collective parameters satisfy the
   * requirements.
   *
   * check_fn will never be NULL when checkrequired is set to 1
   *
   * When check_fn is non-NULL and checkrequired is 1, the user must call the check_fn
   * function for each call to PAMI_Collective to determine if the collective is valid.
   * check_fn will return local validity of the parameters, and no other bits in the struct
   * need to be checked. If the nonlocal bit is set in the return code, a reduction will
   * be required to determine the validity of the protocol across tasks.
   *
   * When check_fn is non-NULL and checkrequired is 0, the user must call the check_fn
   * for each set of parameters.  For a given pami_xfer_t structure with a given set of
   * correctness bits set in the metadata, if the values of the parameters will change
   * and are effected by the associated bits in the metadata, check_fn must be called.
   * For example, if the alldtop bit is set, the user can cache that the protocol will work
   * for all datatypes and operations. The user might still need to test a given message
   * size however.
   */
  typedef struct
  {
    char                 * name;             /**<  A character string describing the protocol   */
    unsigned               version;          /**<  A version number for the protocol            */
    pami_metadata_function check_fn;         /**<  A function pointer to validate parameters for
                                                   the collective operation.  Can be NULL if
                                                   no correctness check is required             */
    size_t                 range_lo;         /**<  This protocol has minimum bytes to send/recv
                                                   requirements                                 */
    size_t                 range_hi;         /**<  This protocol has maximum bytes to send/recv
                                                   requirements                                 */
    unsigned               send_min_align;   /**<  This protocol requires a minimum address
                                                   alignment                                    */
    unsigned               recv_min_align;   /**<  This protocol requires a minimum address
                                                   alignment                                    */
    /**
     * \brief Bitmask to determine if certain checks are required.
     */
    union
    {
      unsigned             bitmask_correct;  /**<  Raw bitmask data                             */
      struct
      {
        unsigned           checkrequired: 1; /**<  A flag indicating whether or not the user MUST
                                                   query the metadata or not.  0:query is not
                                                   necessary.  1: the user must call the check_fn
                                                   to determine if the call site parameters are
                                                   correct.                                     */
        unsigned           nonlocal     : 1; /**<  The protocol associated with this metadata
                                                   requires "nonlocal" knowledge, meaning that
                                                   the result code from the check_fn must be
                                                   allreduced to determine if the operation
                                                   will work 0:no, 1:  requires nonlocal data   */
        unsigned           rangeminmax  : 1; /**<  This protocol only supports a range of bytes
                                                   sent/received. 0: no min/max, 
                                                   1: check range_lo/range_hi                   */
        unsigned           sendminalign : 1; /**<  This protocol requires a minimum address
                                                   alignment of send_min_align bytes.
                                                   0: no min alignment, 1: check send_min_align */
        unsigned           recvminalign : 1; /**<  This protocol requires a minimum address
                                                   alignment of recv_min_align bytes
                                                   0: no min alignment, 1: check recv_min_align */
        unsigned           alldtop      : 1; /**<  This protocol works for all datatypes and
                                                   operations for reduction/scan
                                                   0:not for all dt & op, 1:for all dt & op     */
        unsigned           contigsflags : 1; /**<  This protocol requires contiguous data(send) */
        unsigned           contigrflags : 1; /**<  This protocol requires contiguous data(recv) */
        unsigned           continsflags : 1; /**<  This protocol requires continuous data(send):  
                                                   for vector collectives, the target buffers
                                                   of the vectors must be adjacent in memory    */
        unsigned           continrflags : 1; /**<  This protocol requires continuous data(recv):  
                                                   for vector collectives, the target buffers
                                                   of the vectors must be adjacent in memory    */
        unsigned           blocking     : 1; /**<  This protocol may block. 
                                                   0: not blocking,
                                                   1: may block                                 */
        unsigned           inplace      : 1; /**<  This protocol supports in-place buffering: 
                                                   0: the send and recv buffers my not overlap,
                                                   1: the send and recv buffers may overlap     */
        unsigned           asyncflowctl : 1; /**<  This protocol is asynchronous and may require
                                                   some flow control (barrier) to control 
                                                   excessive unexpected data buffering.         */
        unsigned           oneatatime   : 1; /**<  This protocol only permits one collective   
                                                   to be out-standing at a time                 */ 
        unsigned           global_order : 4; /**<  This collective requires the user to start 
                                                   the collective in a consistent global order, 
                                                   even across geometries                       */ 
      } values;
    } check_correct;

    /**
     * \brief Performance parameters
     */
    union
    {
      unsigned             bitmask_perf;     /**<  Raw bitmask data                             */
      struct
      {
        unsigned           hw_accel     : 1; /**<  This collective is using special purpose
                                                   hardware to accelerate the collective
                                                   0:  no 1:  yes                               */
      } values;
    } check_perf;
    size_t                 range_lo_perf;    /**<  Estimated performance range in bytes(low)    */
    size_t                 range_hi_perf;    /**<  Estimated performance range in bytes(high)   */
    unsigned               min_align_perf;   /**<  Estimated performance minimum address alignment */
  } pami_metadata_t;

  /** \} */ /* end of "PAMI Collectives Metadata" group */


  /*****************************************************************************/
  /**
   * \defgroup activemessage Active Message Point-to-Point
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Options for dispatch and send hints
   *
   * \see pami_dispatch_hint_t
   * \see pami_send_hint_t
   */
  typedef enum
  {
    PAMI_HINT_DEFAULT = 0, /**< This hint leaves the option up to the PAMI implementation to choose. */
    PAMI_HINT_ENABLE  = 1, /**< A hint that the implementation should enable this option.  */
    PAMI_HINT_DISABLE = 2, /**< A hint that the implementation should disable this option. */
    PAMI_HINT_INVALID = 3  /**< An invalid hint value provided for 2 bit completeness.     */
  } pami_hint_t;

  /**
   * \brief "hard" hints for registering a send dispatch
   *
   * These hints are considered 'hard' hints that must be honored by the
   * implementation or the dispatch set must fail and return an error.
   *
   * Alternatively, hints may be specified for each send operation. Hints
   * specified in this way are considered 'soft' hints and may be silently
   * ignored by the implementation during a send operation.
   *
   * Hints are used to improve performance by allowing the send implementation
   * to safely assume that certain use cases will not ever, or will always,
   * be valid.
   *
   * \note Hints should be specified with the pami_hint_t enum values.
   *
   * \see pami_send_hint_t
   * \see PAMI_Dispatch_set()
   *
   */
  typedef struct
  {
    /**
     * \brief Parallelize communication across multiple contexts
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * the send implementation may use other contexts to aid in the
     * communication operation. It is required that all contexts be advanced.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the send implementation will not use other contexts to aid in the
     * communication operation. It is not required that all contexts be
     * advanced, only the contexts with active communication must be advanced.
     *
     * If specified as ::PAMI_HINT_DEFAULT, the effect on the
     * communication is equivalent to the effect of ::PAMI_HINT_ENABLE
     */
    unsigned  multicontext          : 2;

    /**
     * \brief Long (multi-packet) header support
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * the send implementation will enable single- and multi-packet header
     * support.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the send implementation will not attempt to send more than a "single
     * packet" of header information.  This requires that that only up to
     * ::PAMI_DISPATCH_RECV_IMMEDIATE_MAX bytes of
     * header information is sent. Failure to limit the number of header bytes
     * sent will result in undefined behavior.
     *
     * If specified as ::PAMI_HINT_DEFAULT, the effect on the
     * communication is equivalent to the effect of ::PAMI_HINT_ENABLE
     */
    unsigned  long_header           : 2;

    /**
     * \brief All asynchronous receives will be contiguous using
     *        ::PAMI_TYPE_BYTE with a zero offset.
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * it is not required to set pami_recv_t::type nor pami_recv_t::offset for
     * the receive.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the send implementation will enable application-specified typed receive
     * support and it is required to set pami_recv_t::type and pami_recv_t::offset
     * for the receive.
     *
     * If specified as ::PAMI_HINT_DEFAULT, the effect on the
     * communication is equivalent to the effect of ::PAMI_HINT_DISABLE
     */
    unsigned  recv_contiguous       : 2;

    /**
     * \brief All asynchronous receives will use ::PAMI_DATA_COPY and a \c NULL data cookie
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * it is not required to set pami_recv_t::data_fn nor pami_recv_t::data_cookie
     * for the receive.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the send implementation will enable application-specified data function
     * typed receive support and it is required to set pami_recv_t::data_fn and
     * pami_recv_t::data_cookie for the receive.
     *
     * If specified as ::PAMI_HINT_DEFAULT, the effect on the
     * communication is equivalent to the effect of ::PAMI_HINT_DISABLE
     */
    unsigned  recv_copy             : 2;

    /**
     * \brief All sends will result in an 'immediate' receive
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * the dispatch function will always receive as 'immediate', and it is not
     * required to initialize the pami_recv_t output parameters. It is also
     * required that only up to ::PAMI_DISPATCH_RECV_IMMEDIATE_MAX
     * bytes of combined header and data is sent. Failure to limit the number of
     * bytes sent will result in undefined behavior.
     *
     * \note It is not neccessary to set the pami_dispatch_hint_t::recv_contiguous
     *       hint, nor the pami_dispatch_hint_t::recv_copy hint, when the
     *       pami_dispatch_hint_t::recv_immediate hint is set to
     *       ::PAMI_HINT_ENABLE
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the dispatch function will never receive as 'immediate', even for
     * zero-byte messages, and it is required to always initialize all
     * pami_recv_t output parameters for every receive.
     *
     * If specified as ::PAMI_HINT_DEFAULT during PAMI_Dispatch_set(),
     * the implementation may provide the source data as an 'immediate' receive
     * or the implementation may provide the source data as an 'asynchronous'
     * receive. The appropriate pami_recv_t output parameters must be initialized
     * as required by the dispatch function input parameters.
     */
    unsigned  recv_immediate        : 2;

    /**
     * \brief Force match ordering semantics
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * the dispatch functions for all communication between a pair of endpoints
     * will always be invoked in the same order as the send operations were
     * invoked on the context associated with the origin endpoint. The
     * completion callbacks for 'asycnhronous' receives may not be invoked in
     * the same order as the dispatch functions were invoked.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the dispatch functions for all communication between a pair of endpoints
     * may not be invoked in the same order as the send operations were
     * invoked on the context associated with the origin endpoint. The
     * completion callbacks for 'asynchronous' receives may not be invoked in
     * the same order as the dispatch functions were invoked.
     *
     * If specified as ::PAMI_HINT_DEFAULT, the effect on the
     * communication is equivalent to the effect of ::PAMI_HINT_DISABLE
     */
    unsigned  consistency           : 2;

    /**
     * \brief Send and receive buffers are ready for RDMA operations
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * the application asserts that the memory for the send and receive buffers
     * has been previously registered for rdma transfers using the
     * PAMI_Memregion_create() function.
     *
     * \note It is an error to specify ::PAMI_HINT_DISABLE for the
     *       pami_dispatch_hint_t::use_rdma hint when the
     *       pami_dispatch_hint_t::buffer_registered hint is set to
     *       ::PAMI_HINT_ENABLE
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the memory for the send and receive buffers may, or may not, have been
     * previously registered for rdma transfers using the PAMI_Memregion_create()
     * function.
     *
     * If specified as ::PAMI_HINT_DEFAULT, the effect on the
     * communication is equivalent to the effect of ::PAMI_HINT_DISABLE
     **/
    unsigned  buffer_registered     : 2;

    /**
     * \brief Force the destination endpoint to make asynchronous progress
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * an execution resource on the destination endpoint will asynchronously
     * process the receive operation when the context is not advanced.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the destination endpoint will not asynchronously process the receive
     * operation when the context is not advanced.
     *
     * If specified as ::PAMI_HINT_DEFAULT, the effect on the
     * communication is equivalent to the effect of ::PAMI_HINT_DISABLE
     **/
    unsigned  remote_async_progress : 2;

    /**
     * \brief Communication uses rdma operations
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * the implementation must use rdma operations to aid in the communication
     * operation.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the implementation must not use rdma operations to aid in the
     * communication operation.
     *
     * If specified as ::PAMI_HINT_DEFAULT during PAMI_Dispatch_set(),
     * the implementation may, or may not, use rdma operations to aid in the
     * communication operation.
     */
    unsigned  use_rdma              : 2;

    /**
     * \brief Communication uses shared memory optimizations
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * the implementation must use shared memory optimizations to aid in the
     * communication operation.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the implementation must not use shared memory optimizations to aid in the
     * communication operation.
     *
     * If specified as ::PAMI_HINT_DEFAULT during PAMI_Dispatch_set(),
     * the implementation may, or may not, use shared memory optimizations to
     * aid in the communication operation.
     *
     * Certain transfer operations, such as PAMI_Send_immediate() and others,
     * may return ::PAMI_INVAL if shared memory is forced on and the destination
     * endpoint is located on a different node. The number and the list of tasks
     * located on the same node may be queried using PAMI_Client_query() and the
     * ::PAMI_CLIENT_NUM_LOCAL_TASKS and
     * ::PAMI_CLIENT_LOCAL_TASKS configuration attributes.
     *
     */
    unsigned  use_shmem             : 2;

    /**
     * \brief Return an error if PAMI_Send_immediate() resources are not available
     *
     * If specified as ::PAMI_HINT_ENABLE during PAMI_Dispatch_set(),
     * the PAMI_Send_immediate() implementation will internally queue the send
     * operation until network resource become available.
     *
     * If specified as ::PAMI_HINT_DISABLE during PAMI_Dispatch_set(),
     * the PAMI_Send_immediate() implementation will return ::PAMI_EAGAIN when
     * network resources are unavailable.
     *
     * If specified as ::PAMI_HINT_DEFAULT, the effect on the
     * communication is equivalent to the effect of ::PAMI_HINT_ENABLE
     **/
    unsigned  queue_immediate       : 2;

  } pami_dispatch_hint_t;

  /**
   * \brief "Soft" hints for sending a message
   *
   * These hints are considered 'soft' hints that may be silently ignored
   * by the implementation during a send operation.
   *
   * Alternatively, hints may be specified when a send dispatch identifier
   * is registered using PAMI_Dispatch_set().  Hints set in this way are
   * considered 'hard' hints and must be honored by the implementation,
   * or the dispatch set must fail and return an error.
   *
   * \note Hints should be specified with the pami_hint_t enum values.
   *
   * \see pami_dispatch_hint_t
   * \see PAMI_Send()
   * \see PAMI_Send_typed()
   * \see PAMI_Send_immediate()
   */
  typedef struct
  {
    unsigned reserved0             : 12; /**< \brief Reserved for future use. */

    /**
     * \brief Send and receive buffers are ready for RDMA operations
     * \see   pami_dispatch_hint_t::buffer_registered
     **/
    unsigned buffer_registered     : 2;

    /**
     * \brief Force the destination endpoint to make asynchronous progress
     * \see   pami_dispatch_hint_t::remote_async_progress
     **/
    unsigned remote_async_progress : 2;

    /**
     * \brief Communication uses rdma operations
     * \see   pami_dispatch_hint_t::use_rdma
     **/
    unsigned use_rdma              : 2;

    /**
     * \brief Communication uses shared memory optimizations
     * \see   pami_dispatch_hint_t::use_shmem
     **/
    unsigned use_shmem             : 2;

    unsigned reserved1             : 2; /**< \brief Reserved for future use. */

  } pami_send_hint_t;

  /**
   * \brief "Soft" hints for a collective operation
   *
   * These hints are considered 'soft' hints that may be silently ignored
   * by the implementation during a send operation.
   */
  typedef struct
  {
    /**
     * \brief Parallelize communication across multiple contexts
     * \see   pami_dispatch_hint_t::multicontext
     **/
    unsigned multicontext          : 2;

    unsigned reserved0             : 20; /**< \brief Reserved for future use. */

  } pami_collective_hint_t;

  /**
   * \brief Active message send common parameters structure
   */
  typedef struct
  {
    struct iovec       header;   /**< Header buffer address and size in bytes */
    struct iovec       data;     /**< Data buffer address and size in bytes */
    size_t             dispatch; /**< Dispatch identifier */
    pami_send_hint_t   hints;    /**< Hints for sending the message */
    pami_endpoint_t    dest;     /**< Destination endpoint */
  } pami_send_immediate_t;

  /**
   * \brief Structure for event notification
   */
  typedef struct
  {
    void                * cookie;   /**< Argument to \b all event callbacks */
    pami_event_function   local_fn; /**< Local message completion event */
    pami_event_function   remote_fn;/**< Remote message completion event */
  } pami_send_event_t;

  /**
   * \brief Structure for send parameters unique to a simple active message send
   */
  typedef struct
  {
    pami_send_immediate_t send;     /**< Common send parameters */
    pami_send_event_t     events;   /**< Non-blocking event parameters */
  } pami_send_t;

  /**
   * \brief Structure for send parameters of a typed active message send
   */
  typedef struct
  {
    pami_send_immediate_t send;     /**< Common send parameters */
    pami_send_event_t     events;   /**< Non-blocking event parameters */
    struct
    {
      pami_type_t         type;        /**< Datatype */
      size_t              offset;      /**< Starting offset in \c datatype */
      pami_data_function  data_fn;     /**< Function to produce data */
      void               *data_cookie; /**< cookie for produce data function */
    } typed;                           /**< Typed send parameters */
  } pami_send_typed_t;

  /**
   * \brief Non-blocking active message send for contiguous data
   *
   * A low-latency send operation may be enhanced by using a dispatch id which
   * was set with the pami_dispatch_hint_t::recv_immediate hint bit enabled.
   * This hint asserts that all receives with the dispatch id will not exceed
   * a certain limit.
   *
   * The implementation configuration attribute ::PAMI_DISPATCH_RECV_IMMEDIATE_MAX
   * defines the maximum size of data buffers that can be completely received
   * with a single dispatch callback. Typically this limit is associated with
   * a network resource attribute, such as a packet size.
   *
   * It is safe for the application to deallocate, or otherwise alter, the
   * pami_send_t parameter structure after this function returns. However,
   * it is \b not safe for the application to deallocate, or otherwise alter,
   * the memory locations specified by pami_send_immediate_t::header and
   * pami_send_immediate_t::data until the pami_send_event_t::local_fn is
   * invoked.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \pre  The pami_send_immediate_t::dispatch identifier must be registered on
   *       the context with PAMI_Dispatch_set().
   *
   * \note It \e is valid to specify the endpoint associated with the
   *       communication context used to issue the operation as the
   *       destination for the transfer.
   *
   * \see pami_send_hint_t
   * \see PAMI_Dispatch_query
   *
   * \param [in] context     Communication context
   * \param [in] parameters  Send simple parameter structure
   *
   * \retval ::PAMI_SUCCESS  The request has been accepted.
   * \retval ::PAMI_INVAL    The request has been rejected due to invalid parameters.
   */
  pami_result_t PAMI_Send (pami_context_t    context,
                           pami_send_t     * parameters);

  /**
   * \brief Immediate active message send for small contiguous data
   *
   * The blocking send is only valid for small data buffers. The implementation
   * configuration attribute ::PAMI_DISPATCH_SEND_IMMEDIATE_MAX defines the upper
   * bounds for the size of data buffers, including header data, that can be
   * sent with this function. This function will return an error if a data
   * buffer larger than the ::PAMI_DISPATCH_SEND_IMMEDIATE_MAX is attempted.
   *
   * This function provides a low-latency send that can be optimized by the
   * specific pami implementation. If network resources are immediately
   * available the send data will be injected directly into the network. If
   * resources are not available the specific pami implementation may internally
   * buffer the send parameters and data until network resource are available
   * to complete the transfer. In either case the send will immediately return,
   * no done callback is invoked, and is considered complete.
   *
   * The low-latency send operation may be further enhanced by using a dispatch
   * id which was set with the pami_dispatch_hint_t::recv_immediate hint bit
   * enabled. This hint asserts that all receives with the dispatch id will not
   * exceed a certain limit.
   *
   * The implementation configuration attribute ::PAMI_DISPATCH_RECV_IMMEDIATE_MAX
   * defines the maximum size of data buffers that can be completely received
   * with a single dispatch callback. Typically this limit is associated with
   * a network resource attribute, such as a packet size.
   *
   * It is safe for the application to deallocate, or otherwise alter, the
   * pami_send_immediate_t parameter structure and the memory locations specified by
   * pami_send_immediate_t::header and pami_send_immediate_t::data after this
   * function returns.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \pre  The pami_send_immediate_t::dispatch identifier must be registered on
   *       the context with PAMI_Dispatch_set().
   *
   * \note It \e is valid to specify the endpoint associated with the
   *       communication context used to issue the operation as the
   *       destination for the transfer.
   *
   * \see pami_send_hint_t
   * \see PAMI_Dispatch_query
   *
   * \param [in] context    Communication context
   * \param [in] parameters Send parameter structure
   *
   * \retval ::PAMI_SUCCESS  The request has been accepted.
   * \retval ::PAMI_INVAL    The request has been rejected due to invalid parameters.
   * \retval ::PAMI_EAGAIN   The request could not be satisfied due to unavailable
   *                         network resources and the request data could not be
   *                         queued for later processing due to the value of the
   *                         pami_dispatch_hint_t::queue_immediate hint for this
   *                         dispatch identifier.
   */
  pami_result_t PAMI_Send_immediate (pami_context_t          context,
                                     pami_send_immediate_t * parameters);

  /**
   * \brief Non-blocking active message send for non-contiguous typed data
   *
   * Transfers data according to a predefined data memory layout, or type, to
   * the remote task.
   *
   * Conceptually, the data is transfered as a byte stream which may be
   * received by the remote task into a different format, such as a contiguous
   * buffer or the same or different predefined type.
   *
   * It is safe for the application to deallocate, or otherwise alter, the
   * pami_send_typed_t parameter structure after this function returns. However,
   * it is \b not safe for the application to deallocate, or otherwise alter,
   * the memory locations specified by pami_send_immediate_t::header and
   * pami_send_immediate_t::data until the pami_send_event_t::local_fn
   * is invoked.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \pre  The pami_send_immediate_t::dispatch identifier must be registered on
   *       the context with PAMI_Dispatch_set().
   *
   * \note It \e is valid to specify the endpoint associated with the
   *       communication context used to issue the operation as the
   *       destination for the transfer.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Send typed parameter structure
   *
   * \retval ::PAMI_SUCCESS  The request has been accepted.
   * \retval ::PAMI_INVAL    The request has been rejected due to invalid parameters.
   */
  pami_result_t PAMI_Send_typed (pami_context_t      context,
                                 pami_send_typed_t * parameters);

  /**
   * \brief Active message receive parameter structure
   *
   * This structure is initialized by the application and then returned as an
   * output parameter from the active message dispatch callback function to
   * direct the receive of the incoming data stream.
   *
   * When pami_recv_t::type is ::PAMI_TYPE_BYTE, the receive buffer is
   * contiguous and must be large enough to hold the entire message.
   *
   * With a non-contiguous pami_recv_t::type, the receive buffer in general must
   * be large enough for the incoming message as well. However, pami_recv_t::type
   * may be constructed in such a way that unwanted portions of the incoming
   * data stream are disposed into a circular junk buffer.
   *
   * \see pami_dispatch_p2p_function
   */
  typedef struct
  {
    void                  * cookie;      /**< Argument to \b all event callbacks */
    pami_event_function     local_fn;    /**< Local message completion event */
    void                  * addr;        /**< Starting address of the buffer */
    pami_type_t             type;        /**< Datatype */
    size_t                  offset;      /**< Starting offset of the type */
    pami_data_function      data_fn;     /**< Function to consume data */
    void                  * data_cookie; /**< cookie for produce data function */
  } pami_recv_t;

  /**
  * \brief Dispatch callback function signature of application functions used to
   *       receive incoming messages on a communication context
   *
   * Each dispatch callback function supports two kinds of receives:
   * \e immediate and \e asynchronous.
   *
   * An \e immediate receive occurs when the dispatch function is invoked and all
   * of the data sent is immediately available in the buffer. In this case
   * \c pipe_addr will point to a valid memory location - even when the number
   * of bytes sent is zero, and the \c recv output structure will be \c NULL.
   *
   * An \e asynchronous receive occurs when the dispatch function is invoked and
   * all of the data sent is \b not immediately available. In this case the
   * application must provide information to direct the completion of the
   * receive. The \c recv output structure will point to a valid memory
   * location for this purpose, and the \c pipe_addr pointer will be \c NULL.
   * The \c data_size parameter will contain the number of bytes that are being
   * sent from the \c origin endpoint.
   *
   * The memory location, specified by pami_recv_t::addr, for an asynchronous
   * receive must not be deallocated, or otherwise altered, until the
   * pami_recv_t::local_fn is invoked.
   *
   * \note The maximum number of bytes that may be immediately received can be
   *       queried using PAMI_Dispatch_query() with the
   *       ::PAMI_DISPATCH_RECV_IMMEDIATE_MAX configuration attribute.
   *
   * At the time a dispatch function is invoked the application has already
   * protected access to the associated communication context, either by using
   * PAMI_Context_lock() or by some other mechanism. Consequently, application code
   * executed from within a dispatch callback function has dedicated access to the
   * associated communication context and is \b not required to further protect
   * access to the context prior to invoking functions that operate on that
   * context. The application code executed from within an event callback
   * function \b must \b not invoke any progress function, such as
   * PAMI_Context_advance(), to avoid corruption of the internal progress engine
   * state.
   *
   * \note Application code executed from within a dispatch callback function
   *       does \b not have implicit dedicated access to other communication
   *       contexts. Operations on other communication contexts must be
   *       protected from re-entrant access from other execution resources by
   *       the application.
   *
   * \warning The application must take care to avoid deadlocks when protecting
   *          access to a communication context other than the communication
   *          context which invoked the dispatch callback function. It is
   *          recommended that PAMI_Context_trylock() or PAMI_Context_post()
   *          be used instead of PAMI_Context_lock() in this situtation.
   *
   * \param [in] context     Communication context that invoked this function
   * \param [in] cookie      Dispatch callback application argument
   * \param [in] header_addr Application header address
   * \param [in] header_size Application header size in bytes
   * \param [in] pipe_addr   Address of the \e immediate receive buffer - valid
   *                         only if non-NULL
   * \param [in] data_size   Application data size in bytes, valid regardless of
   *                         message type
   * \param [in] origin      Endpoint that originated the transfer
   * \param [in] recv        Receive message structure, only needed if
   *                         \c pipe_addr is non-NULL
   */
  typedef void (*pami_dispatch_p2p_function) (pami_context_t    context,
                                              void            * cookie,
                                              const void      * header_addr,
                                              size_t            header_size,
                                              const void      * pipe_addr,
                                              size_t            data_size,
                                              pami_endpoint_t   origin,
                                              pami_recv_t     * recv);

  /** \} */ /* end of "active message" group */


  /*****************************************************************************/
  /**
   * \defgroup rma Remote Memory Access
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Input parameters common to all rma transfers
   **/
  typedef struct
  {
    pami_endpoint_t       dest;      /**< Destination endpoint */
    pami_send_hint_t      hints;     /**< Hints for sending the message */
    size_t                bytes;     /**< Data transfer size in bytes:  0 bytes disallowed*/
    void                * cookie;    /**< Argument to \b all event callbacks */
    pami_event_function   done_fn;   /**< Local completion event */
  } pami_rma_t;

  /**
   * \brief Input parameters for rma simple transfers
   **/
  typedef struct
  {
    void               * local;     /**< Local transfer virtual address */
    void               * remote;    /**< Remote transfer virtual address */
  } pami_rma_addr_t;

  /**
   * \brief Input parameters for rma typed transfers
   */
  typedef struct
  {
    pami_type_t           local;     /**< Data type of local buffer */
    pami_type_t           remote;    /**< Data type of remote buffer */
  } pami_rma_typed_t;

  /*****************************************************************************/
  /**
   * \defgroup put Put
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Input parameters for rma put transfers
   */
  typedef struct
  {
    pami_event_function   rdone_fn;  /**< Remote completion event - all local
                                         data has been received by remote task */
  } pami_rma_put_t;


  /**
   * \brief Input parameters for simple put transfers
   * \code
   * pami_put_simple_t parameters;
   * parameters.rma.dest     = ;
   * parameters.rma.hints    = ;
   * parameters.rma.bytes    = ;
   * parameters.rma.cookie   = ;
   * parameters.rma.done_fn  = ;
   * parameters.addr.local   = ;
   * parameters.addr.remote  = ;
   * parameters.put.rdone_fn = ;
   * \endcode
   */
  typedef struct
  {
    pami_rma_t            rma;       /**< Common rma parameters */
    pami_rma_addr_t       addr;      /**< Simple rma address parameters */
    pami_rma_put_t        put;       /**< Common put parameters */
  } pami_put_simple_t;

  /**
   * \brief One-sided put operation for simple contiguous data transfer
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Simple put input parameters
   *
   * \retval ::PAMI_SUCCESS  The request has been accepted.
   * \retval ::PAMI_INVAL    The request has been rejected due to invalid parameters.
   */
  pami_result_t PAMI_Put (pami_context_t      context,
                          pami_put_simple_t * parameters);

  /**
   * \brief Input parameters for simple typed put transfers
   * \code
   * pami_put_typed_t parameters;
   * parameters.rma.dest     = ;
   * parameters.rma.hints    = ;
   * parameters.rma.bytes    = ;
   * parameters.rma.cookie   = ;
   * parameters.rma.done_fn  = ;
   * parameters.addr.local   = ;
   * parameters.addr.remote  = ;
   * parameters.type.local   = ;
   * parameters.type.remote  = ;
   * parameters.put.rdone_fn = ;
   * \endcode
   */
  typedef struct
  {
    pami_rma_t            rma;       /**< Common rma parameters */
    pami_rma_addr_t       addr;      /**< Simple rma address parameters */
    pami_rma_typed_t      type;      /**< Simple rma typed parameters */
    pami_rma_put_t        put;       /**< Common put parameters */
  } pami_put_typed_t;

  /**
   * \brief One-sided put operation for typed non-contiguous data transfer
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Typed put input parameters
   *
   * \retval ::PAMI_SUCCESS  The request has been accepted.
   * \retval ::PAMI_INVAL    The request has been rejected due to invalid parameters.
   */
  pami_result_t PAMI_Put_typed (pami_context_t     context,
                                pami_put_typed_t * parameters);


  /** \} */ /* end of "put" group */

  /*****************************************************************************/
  /**
   * \defgroup get Get
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Input parameter structure for simple get transfers
   * \code
   * pami_get_simple_t parameters;
   * parameters.rma.dest    = ;
   * parameters.rma.hints   = ;
   * parameters.rma.bytes   = ;
   * parameters.rma.cookie  = ;
   * parameters.rma.done_fn = ;
   * parameters.addr.local  = ;
   * parameters.addr.remote = ;
   * \endcode
   */
  typedef struct
  {
    pami_rma_t            rma;       /**< Common rma parameters */
    pami_rma_addr_t       addr;      /**< Simple rma address parameters */
  } pami_get_simple_t;

  /**
   * \brief One-sided get operation for simple contiguous data transfer
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Simple get input parameters
   *
   * \retval ::PAMI_SUCCESS  The request has been accepted.
   * \retval ::PAMI_INVAL    The request has been rejected due to invalid parameters.
   */
  pami_result_t PAMI_Get (pami_context_t      context,
                          pami_get_simple_t * parameters);

  /**
   * \brief Input parameter structure for typed get transfers
   * \code
   * pami_get_typed_t parameters;
   * parameters.rma.dest    = ;
   * parameters.rma.hints   = ;
   * parameters.rma.bytes   = ;
   * parameters.rma.cookie  = ;
   * parameters.rma.done_fn = ;
   * parameters.addr.local  = ;
   * parameters.addr.remote = ;
   * parameters.type.local  = ;
   * parameters.type.remote = ;
   * \endcode
   */
  typedef struct
  {
    pami_rma_t            rma;       /**< Common rma parameters */
    pami_rma_addr_t       addr;      /**< Simple rma address parameters */
    pami_rma_typed_t      type;      /**< Simple rma typed parameters */
  } pami_get_typed_t;

  /**
   * \brief One-sided get operation for typed non-contiguous data transfer
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Typed get input parameters
   *
   * \retval ::PAMI_SUCCESS  The request has been accepted.
   * \retval ::PAMI_INVAL    The request has been rejected due to invalid parameters.
   */
  pami_result_t PAMI_Get_typed (pami_context_t     context,
                                pami_get_typed_t * parameters);

  /** \} */ /* end of "get" group */

  /*****************************************************************************/
  /**
   * \defgroup rmw Read-Modify-Write
   * \{
   */
  /*****************************************************************************/

  /** \brief Atomic rmw operation */
  typedef enum
  {
    /* fetch */
    PAMI_ATOMIC_FETCH    = (0x1 << 0), /**< 'fetch' operation; replace 'local' with 'remote' */

    /* compare */
    PAMI_ATOMIC_COMPARE  = (0x1 << 1), /**< 'compare' operation; requires a 'modify' operation */

    /* modify */
    PAMI_ATOMIC_SET      = (0x1 << 2), /**< 'set' operation; replace 'remote' with 'value' */
    PAMI_ATOMIC_ADD      = (0x2 << 2), /**< 'add' operation; add 'value' to 'remote' */
    PAMI_ATOMIC_OR       = (0x3 << 2), /**< 'or'  operation; bitwise-or 'value' to 'remote' */
    PAMI_ATOMIC_AND      = (0x4 << 2), /**< 'and' operation; bitwise-and 'value' to 'remote' */
    PAMI_ATOMIC_XOR      = (0x5 << 2), /**< 'xor' operation; bitwise-xor 'value' to 'remote' */

    /* fetch then modify */
    PAMI_ATOMIC_FETCH_SET = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_SET), /**< 'fetch then set' operation */
    PAMI_ATOMIC_FETCH_ADD = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_ADD), /**< 'fetch then add' operation */
    PAMI_ATOMIC_FETCH_OR  = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_OR),  /**< 'fetch then or'  operation */
    PAMI_ATOMIC_FETCH_AND = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_AND), /**< 'fetch then and' operation */
    PAMI_ATOMIC_FETCH_XOR = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_XOR), /**< 'fetch then xor' operation */

    /* compare and modify */
    PAMI_ATOMIC_COMPARE_SET = (PAMI_ATOMIC_COMPARE | PAMI_ATOMIC_SET), /**< 'set if test equals remote' operation */
    PAMI_ATOMIC_COMPARE_ADD = (PAMI_ATOMIC_COMPARE | PAMI_ATOMIC_ADD), /**< 'add if test equals remote' operation */
    PAMI_ATOMIC_COMPARE_OR  = (PAMI_ATOMIC_COMPARE | PAMI_ATOMIC_OR),  /**< 'or if test equals remote' operation */
    PAMI_ATOMIC_COMPARE_AND = (PAMI_ATOMIC_COMPARE | PAMI_ATOMIC_AND), /**< 'and if test equals remote' operation */
    PAMI_ATOMIC_COMPARE_XOR = (PAMI_ATOMIC_COMPARE | PAMI_ATOMIC_XOR), /**< 'xor if test equals remote' operation */

    /* fetch then compare and modify */
    PAMI_ATOMIC_FETCH_COMPARE_SET = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_COMPARE_SET), /**< 'fetch then set if test equals remote' operation */
    PAMI_ATOMIC_FETCH_COMPARE_ADD = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_COMPARE_ADD), /**< 'fetch then add if test equals remote' operation */
    PAMI_ATOMIC_FETCH_COMPARE_OR  = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_COMPARE_OR),  /**< 'fetch then or if test equals remote' operation */
    PAMI_ATOMIC_FETCH_COMPARE_AND = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_COMPARE_AND), /**< 'fetch then and if test equals remote' operation */
    PAMI_ATOMIC_FETCH_COMPARE_XOR = (PAMI_ATOMIC_FETCH | PAMI_ATOMIC_COMPARE_XOR), /**< 'fetch then xor if test equals remote' operation */

  } pami_atomic_t;

  /** \brief Read-Modify-Write parameter structure */
  typedef struct
  {
    pami_endpoint_t       dest;      /**< Destination endpoint */
    pami_send_hint_t      hints;     /**< Hints for sending the message */
    void                * cookie;    /**< Argument to \b all event callbacks */
    pami_event_function   done_fn;   /**< Atomic operation completion event */
    void                * local;     /**< Local (fetch) transfer virtual address */
    void                * remote;    /**< Remote (source) transfer virtual address */
    void                * value;     /**< Operation input local data value location */
    void                * test;      /**< Operation input local test value location */
    pami_atomic_t         operation; /**< Read-modify-write operation */

    /**
     * \brief Read-modify-write data type
     *
     * Only the following `pami_type_t` types are valid for rmw operations:
     *
     * - PAMI_TYPE_SIGNED_INT
     * - PAMI_TYPE_UNSIGNED_INT
     * - PAMI_TYPE_SIGNED_LONG
     * - PAMI_TYPE_UNSIGNED_LONG
     * - PAMI_TYPE_SIGNED_LONG_LONG
     * - PAMI_TYPE_UNSIGNED_LONG_LONG
     */
    pami_type_t           type;
  } pami_rmw_t;

  /**
   * \brief Atomic read-modify-write operation to a remote memory location
   *
   * The following data types are supported for read-modify-write operations:
   *
   * - PAMI_TYPE_SIGNED_INT
   * - PAMI_TYPE_UNSIGNED_INT
   * - PAMI_TYPE_SIGNED_LONG
   * - PAMI_TYPE_UNSIGNED_LONG
   * - PAMI_TYPE_SIGNED_LONG_LONG (only supported on 64-bit platforms)
   * - PAMI_TYPE_UNSIGNED_LONG_LONG (only supported on 64-bit platforms)
   *
   * \warning All read-modify-write operations are \b unordered relative
   *          to all other data transfer operations - including other
   *          read-modify-write operations.
   *
   * Example read-modify-write operations include:
   *
   * \par \c rmw.type=PAMI_TYPE_SIGNED_INT; \c rmw.operation=PAMI_ATOMIC_FETCH_AND
   *      "32-bit signed integer fetch-then-and operation"
   * \code
   * int *local, *remote, *value, *test;
   * *local = *remote; *remote &= *value;
   * \endcode
   *
   * \par \c rmw.type=PAMI_TYPE_UNSIGNED_LONG; \c rmw.operation=PAMI_ATOMIC_COMPARE_XOR
   *      "native word sized signed integer compare-and-xor operation"
   * \code
   * unsigned long *local, *remote, *value, *test;
   * (*remote == test) ? *remote ^= *value;
   * \endcode
   *
   * \par \c rmw.type=PAMI_TYPE_SIGNED_LONG_LONG; \c rmw.operation=PAMI_ATOMIC_FETCH_COMPARE_OR
   *      "64-bit signed integer fetch-then-compare-and-or operation"
   * \code
   * long long *local, *remote, *value, *test;
   * *local = *remote; (*remote == *test) ? *remote |= *value;
   * \endcode
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters read-modify-write input parameters
   *
   * \retval ::PAMI_SUCCESS  The request has been accepted.
   * \retval ::PAMI_INVAL    The request has been rejected due to invalid parameters.
   */
  pami_result_t PAMI_Rmw (pami_context_t context, pami_rmw_t * parameters);

  /** \} */ /* end of "rmw" group */

  /*****************************************************************************/
  /**
   * \defgroup rdma RDMA
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Memory region opaque object
   *
   * A memory region must be created before it may be used. The opaque
   * object may be directly transfered to a remote endpoint using the
   * PAMI_Send() or PAMI_Send_immediate() interface.
   *
   * \see PAMI_Memregion_create
   * \see PAMI_Memregion_destroy
   */
  typedef uintptr_t pami_memregion_t[PAMI_CLIENT_MEMREGION_SIZE_STATIC/sizeof(uintptr_t)];

  /**
   * \brief Create a local memory region for one sided operations
   *
   * The local memregion may be transfered, via a send message, to a remote task
   * to allow the remote task to perform one-sided operations with this local
   * task.
   *
   * \note Memory regions may overlap. When one of the overlapping regions is
   *       destroyed any remaining overlapping memory regions are still usable.
   *
   * \param [in]  context   Communication context
   * \param [in]  address   Base virtual address of the memory region
   * \param [in]  bytes_in  Number of bytes requested
   * \param [out] bytes_out Number of bytes granted
   * \param [out] memregion Memory region object to initialize
   *
   * \retval PAMI_SUCCESS The entire memory region, or a portion of
   *                      the memory region was pinned. The actual
   *                      number of bytes pinned from the start of the
   *                      buffer is returned in the \c bytes_out
   *                      parameter. The memory region must be freed with
   *                      with PAMI_Memregion_destroy().
   *
   * \retval PAMI_EAGAIN  The memory region was not pinned due to an
   *                      unavailable resource. The memory region does not
   *                      need to be freed with PAMI_Memregion_destroy().
   *
   * \retval PAMI_INVAL   An invalid parameter value was specified. The memory
   *                      region does not need to be freed with
   *                      PAMI_Memregion_destroy().
   *
   * \retval PAMI_ERROR   The memory region was not pinned and does not need to
   *                      be freed with PAMI_Memregion_destroy().
   */
  pami_result_t PAMI_Memregion_create (pami_context_t     context,
                                       void             * address,
                                       size_t             bytes_in,
                                       size_t           * bytes_out,
                                       pami_memregion_t * memregion);

  /**
   * \brief Destroy a local memory region for one sided operations
   *
   * The memregion object will be changed to an invalid value so that
   * it is clearly destroyed.
   *
   * \param [in] context   Communication context
   * \param [in] memregion Memory region object
   *
   * \retval ::PAMI_SUCCESS The memory region was successfully destroyed
   */
  pami_result_t PAMI_Memregion_destroy (pami_context_t     context,
                                        pami_memregion_t * memregion);

  /**
   * \brief RMA memory region parameters
   */
  typedef struct
  {
    pami_memregion_t      * mr;     /**< Memory region */
    size_t                  offset; /**< Offset from beginning of memory region */
  } pami_rma_mr_t;

  /**
   * \brief RMA RDMA parameters
   */
  typedef struct
  {
    pami_rma_mr_t           local;  /**< Local memory region information */
    pami_rma_mr_t           remote; /**< Remote memory region information */
  } pami_rdma_t;

  /*****************************************************************************/
  /**
   * \defgroup rput Rput
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Input parameter structure for simple rdma put transfers
   * \code
   * pami_rput_simple_t parameters;
   * parameters.rma.dest           = ;
   * parameters.rma.hints          = ;
   * parameters.rma.bytes          = ;
   * parameters.rma.cookie         = ;
   * parameters.rma.done_fn        = ;
   * parameters.rdma.local.mr      = ;
   * parameters.rdma.local.offset  = ;
   * parameters.rdma.remote.mr     = ;
   * parameters.rdma.remote.offset = ;
   * parameters.put.rdone_fn       = ;
   * \endcode
   */
  typedef struct
  {
    pami_rma_t            rma;       /**< Common rma parameters */
    pami_rdma_t           rdma;      /**< Common rdma memregion parameters */
    pami_rma_put_t        put;       /**< Common put parameters */
  } pami_rput_simple_t;

  /**
   * \brief Input parameter structure for typed rdma put transfers
   * \code
   * pami_rput_typed_t parameters;
   * parameters.rma.dest           = ;
   * parameters.rma.hints          = ;
   * parameters.rma.bytes          = ;
   * parameters.rma.cookie         = ;
   * parameters.rma.done_fn        = ;
   * parameters.rdma.local.mr      = ;
   * parameters.rdma.local.offset  = ;
   * parameters.rdma.remote.mr     = ;
   * parameters.rdma.remote.offset = ;
   * parameters.type.local         = ;
   * parameters.type.remote        = ;
   * parameters.put.rdone_fn       = ;
   * \endcode
   */
  typedef struct
  {
    pami_rma_t            rma;       /**< Common rma parameters */
    pami_rdma_t           rdma;      /**< Common rdma memregion parameters */
    pami_rma_typed_t      type;      /**< Common rma typed parameters */
    pami_rma_put_t        put;       /**< Common put parameters */
  } pami_rput_typed_t;

  /**
   * \brief Simple put operation for one-sided contiguous data transfer.
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Input parameters structure
   *
   * \retval ::PAMI_SUCCESS The operation was successfully initiated
   */
  pami_result_t PAMI_Rput (pami_context_t context, pami_rput_simple_t * parameters);

  /**
   * \brief Put operation for data type specific one-sided data transfer.
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Input parameters structure
   *
   * \retval ::PAMI_SUCCESS The operation was successfully initiated
   */
  pami_result_t PAMI_Rput_typed (pami_context_t context, pami_rput_typed_t * parameters);

  /** \} */ /* end of "rput" group */

  /*****************************************************************************/
  /**
   * \defgroup rget Rget
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Input parameter structure for simple rdma get transfers
   * \code
   * pami_rget_simple_t parameters;
   * parameters.rma.dest           = ;
   * parameters.rma.hints          = ;
   * parameters.rma.bytes          = ;
   * parameters.rma.cookie         = ;
   * parameters.rma.done_fn        = ;
   * parameters.rdma.local.mr      = ;
   * parameters.rdma.local.offset  = ;
   * parameters.rdma.remote.mr     = ;
   * parameters.rdma.remote.offset = ;
   * \endcode
   */
  typedef struct
  {
    pami_rma_t            rma;       /**< Common rma parameters */
    pami_rdma_t           rdma;      /**< Common rdma memregion parameters */
  } pami_rget_simple_t;

  /**
   * \brief Simple get operation for one-sided contiguous data transfer.
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Input parameters structure
   *
   * \retval ::PAMI_SUCCESS The operation was successfully initiated
   */
  pami_result_t PAMI_Rget (pami_context_t context, pami_rget_simple_t * parameters);

  /**
   * \brief Input parameter structure for typed rdma get transfers
   * \code
   * pami_rget_typed_t parameters;
   * parameters.rma.dest           = ;
   * parameters.rma.hints          = ;
   * parameters.rma.bytes          = ;
   * parameters.rma.cookie         = ;
   * parameters.rma.done_fn        = ;
   * parameters.rdma.local.mr      = ;
   * parameters.rdma.local.offset  = ;
   * parameters.rdma.remote.mr     = ;
   * parameters.rdma.remote.offset = ;
   * parameters.type.local         = ;
   * parameters.type.remote        = ;
   * \endcode
   */
  typedef struct
  {
    pami_rma_t            rma;       /**< Common rma parameters */
    pami_rdma_t           rdma;      /**< Common rdma memregion parameters */
    pami_rma_typed_t      type;      /**< Common rma typed parameters */
  } pami_rget_typed_t;

  /**
   * \brief Get operation for data type specific one-sided data transfer.
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] parameters Input parameters structure
   *
   * \retval ::PAMI_SUCCESS The operation was successfully initiated
   */
  pami_result_t PAMI_Rget_typed (pami_context_t context, pami_rget_typed_t * parameters);

  /** \} */ /* end of "rget" group */
  /** \} */ /* end of "rdma" group */
  /** \} */ /* end of "rma" group */



  /*****************************************************************************/
  /**
   * \defgroup dynamictasks Dynamic Tasks
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Clean up local resources to an endpoint in preparation for
   *        task shutdown or checkpoint
   *
   * It is the user of this API's responsibility to ensure that all
   * communication has been quiesced to and from the destination via a fence
   * call and synchronization.
   *
   * \note It is \b not \b valid to specify the destination endpoint associated
   *       with the communication context used to issue the operation.
   *
   * \param [in] context    Communication context
   * \param [in] dest       Array of destination endpoints to close connections to
   * \param [in] count      Number of endpoints in the array dest
   *
   * \retval ::PAMI_SUCCESS The purge operation was successful
   */

  pami_result_t PAMI_Purge (pami_context_t    context,
                            pami_endpoint_t * dest,
                            size_t            count);

  /**
   * \brief Setup local resources to an endpoint in preparation for
   *        task restart or creation
   *
   * \note It is \b not \b valid to specify the destination endpoint associated
   *       with the communication context used to issue the operation.
   *
   * \param [in] context    Communication context
   * \param [in] dest       Array of destination endpoints to resume connections to
   * \param [in] count      Number of endpoints in the array dest
   *
   * \retval ::PAMI_SUCCESS The resume operation was successful
   */
  pami_result_t PAMI_Resume (pami_context_t    context,
                             pami_endpoint_t * dest,
                             size_t            count);

  /** \} */ /* end of "dynamic tasks" group */


  /*****************************************************************************/
  /**
   * \defgroup sync Memory Synchronization and Data Fence
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Begin a memory synchronization region
   *
   * A fence region is defined as an area of program control on the local task
   * bounded by the PAMI_Fence_begin() and PAMI_Fence_end() functions.
   *
   * \warning It is considered \b illegal to invoke a fence operation outside of
   *          a fence region.
   *
   * \warning It is considered \b illegal to begin a fence region inside an
   *          existing fence region. Fence regions can not be nested.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context Communication context
   *
   * \retval ::PAMI_SUCCESS The fence region was successfully started
   */
  pami_result_t PAMI_Fence_begin (pami_context_t context);

  /**
   * \brief End a memory synchronization region
   *
   * A fence region is defined as an area of program control on the local task
   * bounded by the PAMI_Fence_begin() and PAMI_Fence_end() functions.
   *
   * \warning It is considered \b illegal to invoke a fence operation outside of
   *          a fence region.
   *
   * \warning It is considered \b illegal to end a fence region outside of an
   *          existing fence region.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context Communication context
   *
   * \retval ::PAMI_SUCCESS The fence region was successfully closed
   */
  pami_result_t PAMI_Fence_end (pami_context_t context);


  /**
   * \brief Synchronize all transfers between all endpoints on a context.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] done_fn    Event callback to invoke when the fence is complete
   * \param [in] cookie     Event callback argument
   *
   * \retval ::PAMI_SUCCESS The operation was successfully initiated
   */
  pami_result_t PAMI_Fence_all (pami_context_t        context,
                                pami_event_function   done_fn,
                                void                * cookie);

  /**
   * \brief Synchronize all transfers to an endpoints.
   *
   * \note It is valid to specify the destination endpoint associated with the
   *       communication context used to issue the operation.
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \param [in] context    Communication context
   * \param [in] done_fn    Event callback to invoke when the fence is complete
   * \param [in] cookie     Event callback argument
   * \param [in] target     Endpoint to synchronize
   *
   * \retval ::PAMI_SUCCESS The operation was successfully initiated
   */
  pami_result_t PAMI_Fence_endpoint (pami_context_t        context,
                                     pami_event_function   done_fn,
                                     void                * cookie,
                                     pami_endpoint_t       target);

  /** \} */ /* end of "sync" group */

  /**
   * \brief Collective operation transfer types; used by geometry and xfer routines
   */
  typedef enum
  {
    PAMI_XFER_BROADCAST = 0,
    PAMI_XFER_ALLREDUCE,
    PAMI_XFER_REDUCE,
    PAMI_XFER_ALLGATHER,
    PAMI_XFER_ALLGATHERV,
    PAMI_XFER_ALLGATHERV_INT,
    PAMI_XFER_SCATTER,
    PAMI_XFER_SCATTERV,
    PAMI_XFER_SCATTERV_INT,
    PAMI_XFER_GATHER,
    PAMI_XFER_GATHERV,
    PAMI_XFER_GATHERV_INT,
    PAMI_XFER_BARRIER,
    PAMI_XFER_ALLTOALL,
    PAMI_XFER_ALLTOALLV,
    PAMI_XFER_ALLTOALLV_INT,
    PAMI_XFER_SCAN,
    PAMI_XFER_REDUCE_SCATTER,
    PAMI_XFER_AMBROADCAST,
    PAMI_XFER_AMSCATTER,
    PAMI_XFER_AMGATHER,
    PAMI_XFER_AMREDUCE,
    PAMI_XFER_COUNT,
    PAMI_XFER_TYPE_EXT = 1000 /**< Begin extension-specific values */
  } pami_xfer_type_t;

  /* ************************************************************************* */
  /* **************     Geometry (like groups/communicators)  **************** */
  /* ************************************************************************* */

  /**
   * \brief Task range
   */
  typedef struct
  {
    pami_task_t lo; /**< Range lower bounds task identifier */
    pami_task_t hi; /**< Range lower bounds task identifier */
  } pami_geometry_range_t;


  /**
   * \brief Initialize a geometry with a range of tasks
   *
   * A synchronizing operation will take place during geometry_initialize
   * on the parent geometry.
   *
   * If the output geometry "geometry" pointer is NULL, then no geometry will be
   * created, however, all nodes in the parent must participate in the
   * geometry_initialize operation, even if they do not create a geometry
   *
   * If a geometry is created without a parent geometry (parent is set to
   * PAMI_GEOMETRY_NULL),then an "immediate" geometry will be created.
   * In this case, the new geometry will be created and synchronized,
   * However, the new geometry cannot take advantage of optimized collectives
   * from the parent in the creation of the new geometry.  This kind of
   * geometry create may not be as optimal as when a parent has been provided
   *
   * A unique geometry id is required to give each task described by
   * a geometry a unique communication context/channel to the other tasks in
   * that geometry.  Many higher level api's have code to manage the creation of
   * unique geometry id's.  PAMI defers the unique geometry id assignment to these
   * higher level API's
   *
   * This API takes a context_offset into the array of contexts created
   * at PAMI_Context_createv.  There may be more than one context created per
   * task, so this offset specifies a set of contexts that will be participating
   * in the geometry.  All tasks in the create API must use the same context_offset.
   * This effectively selects a single endpoint (associated with this "primary" context)
   * per task that will be a participant in the collective communication.
   * PAMI_ALL_CONTEXTS can be passed as the context_offset to select all endpoints
   * in the task.  Parent must be PAMI_GEOMETRY_NULL if all contexts are selected
   *
   * Note that this does not effect the advance rules, which state that the user
   * must advance all contexts unless the multicontext hint is disabled for the
   * collective operation.
   *
   * It is an error to post a collective via PAMI_Collective() to a context
   * that is not specified by the client/context_offset pair.
   *
   * \param [in]  client          The PAMI client
   * \param [in]  context_offset  The offset of the context to use for task based collectives
   * \param [in]  configuration   List of configurable attributes and values
   * \param [in]  num_configs     The number of configuration elements
   * \param [out] geometry        Opaque geometry object to initialize
   * \param [in]  parent          Parent geometry containing all the nodes in the task list
   * \param [in]  id              Identifier for this geometry(must be > 0)
   *                              which uniquely represents this geometry(if tasks overlap)
   * \param [in]  task_slices     Array of node slices participating in the geometry
   *                              User must keep the array of slices in memory for the
   *                              duration of the geometry's existence
   * \param [in]  slice_count     Number of task slices participating in the geometry
   * \param [in]  context         context to deliver async callback to
   * \param [in]  fn              event function to call when geometry has been created
   * \param [in]  cookie          user cookie to deliver with the callback
   *
   * \retval ::PAMI_SUCCESS       The task range geometry object initialization was started
   */

  pami_result_t PAMI_Geometry_create_taskrange (pami_client_t           client,
                                                size_t                  context_offset,
                                                pami_configuration_t    configuration[],
                                                size_t                  num_configs,
                                                pami_geometry_t       * geometry,
                                                pami_geometry_t         parent,
                                                unsigned                id,
                                                pami_geometry_range_t * task_slices,
                                                size_t                  slice_count,
                                                pami_context_t          context,
                                                pami_event_function     fn,
                                                void                  * cookie);
  /**
   * \brief Initialize a geometry with a list of tasks
   *
   * A synchronizing operation will take place during geometry_initialize
   * on the parent geometry.
   *
   * If the output geometry "geometry" pointer is NULL, then no geometry will be
   * created, however, all nodes in the parent must participate in the
   * geometry_initialize operation, even if they do not create a geometry
   *
   * If a geometry is created without a parent geometry (parent is set to
   * PAMI_GEOMETRY_NULL),then an "immediate" geometry will be created.
   * In this case, the new geometry will be created and synchronized,
   * However, the new geometry cannot take advantage of optimized collectives
   * from the parent in the creation of the new geometry.  This kind of
   * geometry create may not be as optimal as when a parent has been provided
   *
   * A unique geometry id is required to give each task described by
   * a geometry a unique communication context/channel to the other tasks in
   * that geometry.  Many higher level api's have code to manage the creation of
   * unique geometry id's.  PAMI defers the unique geometry id assignment to these
   * higher level API's
   *
   * This API takes a context_offset into the array of contexts created
   * at PAMI_Context_createv.  There may be more than one context created per
   * task, so this offset specifies a set of contexts that will be participating
   * in the geometry.  All tasks in the create API must use the same context_offset.
   * This effectively selects a single endpoint (associated with this "primary" context)
   * per task that will be a participant in the collective communication.
   * PAMI_ALL_CONTEXTS can be passed as the context_offset to select all endpoints
   * in the task.  Parent must be PAMI_GEOMETRY_NULL if all contexts are selected
   *
   * Note that this does not effect the advance rules, which state that the user
   * must advance all contexts unless the multicontext hint is disabled for the
   * collective operation.
   *
   * It is an error to post a collective via PAMI_Collective() to a context
   * that is not specified by the client/context_offset pair.
   *
   * \param [in]  client          The PAMI client
   * \param [in]  context_offset  The offset of the context to use for task based collectives
   * \param [in]  configuration   List of configurable attributes and values
   * \param [in]  num_configs     The number of configuration elements
   * \param [out] geometry        Opaque geometry object to initialize
   * \param [in]  parent          Parent geometry containing all the nodes in the task list
   * \param [in]  id              Identifier for this geometry(must be > 0)
   *                              which uniquely represents this geometry(if tasks overlap)
   * \param [in]  tasks           Array of tasks to build the geometry list
   *                              User must keep the task list in memory
   *                              duration of the geometry's existence
   * \param [in]  task_count      Number of tasks participating in the geometry
   * \param [in]  context         context to deliver async callback to
   * \param [in]  fn              event function to call when geometry has been created
   * \param [in]  cookie          user cookie to deliver with the callback
   *
   * \retval ::PAMI_SUCCESS       The task list geometry object initialization was started
   */

  pami_result_t PAMI_Geometry_create_tasklist (pami_client_t               client,
                                               size_t                      context_offset,
                                               pami_configuration_t        configuration[],
                                               size_t                      num_configs,
                                               pami_geometry_t           * geometry,
                                               pami_geometry_t             parent,
                                               unsigned                    id,
                                               pami_task_t               * tasks,
                                               size_t                      task_count,
                                               pami_context_t              context,
                                               pami_event_function         fn,
                                               void                      * cookie);

  /**
   * \brief Initialize a geometry with a list of endpoints
   *
   * This call creates and initializes an "endpoint based" geometry.
   * An endopint based geometry is scoped to a pami client, and is created
   * with a list of endpoints that represent the contexts that will participate
   * in the collective operation.  This allows the PAMI library to identify
   * multiple participants per task in a collective operation.  Note that this
   * is a generalization of PAMI_Geometry_create_tasklist and
   * PAMI_Geometry_create_taskrange, both of which only allow one endpoint per task
   * to be specified, if PAMI_ALL_CONTEXTS is not specified as the context offset.
   *
   * A unique geometry id is required to give each task described by
   * a geometry a unique communication context/channel to the other tasks in
   * that geometry.  Many higher level api's have code to manage the creation of
   * unique geometry id's.  PAMI defers the unique geometry id assignment to these
   * higher level API's
   *
   * This operation is nonblocking, and thus must be posted to a context.  The call
   * to create the geometry is made only by a single call per task.  This means that
   * multiple calls per task are not allowed, although multiple endpoints per task
   * may be specified in the endpoint list.
   *
   * Note that the context this operation is posted to is not necessarily contained
   * in the endpoint list.  Completion will be delivered to the context specified
   * in the create call.
   *
   * All contexts associated with the endpoints in the geometry list and the context
   * the work is posted to must be advanced until the geometry completion callback(fn)
   * has been called.  For simplicity, advancing all contexts is sufficient to
   * accomplish this.
   *
   * The same endpoint cannot be specified more than once in the endpoint list
   *
   * \param [in]  client          The PAMI client
   * \param [in]  configuration   List of configurable attributes and values
   * \param [in]  num_configs     The number of configuration elements
   * \param [out] geometry        Opaque geometry object to initialize
   * \param [in]  id              Identifier for this geometry (must be > 0)
   *                              which uniquely represents this lgeometry (if tasks overlap)
   * \param [in]  endpoints       Array of endpoints to build the geometry list
   *                              User must keep the endpoint list in memory for the
   *                              duration of the geometry's existence
   * \param [in]  endpoint_count  Number of endpoints participating in the geometry
   * \param [in]  context         context to deliver async callback to
   * \param [in]  fn              event function to call when geometry has been created
   * \param [in]  cookie          user cookie to deliver with the callback
   *
   * \retval ::PAMI_SUCCESS       The endpoint list geometry object initialization was started
   */

  pami_result_t PAMI_Geometry_create_endpointlist (pami_client_t               client,
                                                   pami_configuration_t        configuration[],
                                                   size_t                      num_configs,
                                                   pami_geometry_t           * geometry,
                                                   unsigned                    id,
                                                   pami_endpoint_t           * endpoints,
                                                   size_t                      endpoint_count,
                                                   pami_context_t              context,
                                                   pami_event_function         fn,
                                                   void                      * cookie);


  /**
   * \brief Return a pointer to the world geometry
   *
   * This routine returns a pointer to the "world geometry"
   * The world geometry is created at client create time as a convenience
   * to the PAMI user.  It contains a representation of all the tasks associated with the
   * client.
   *
   * The world geometry cannot be destroyed by the user, and is cleaned up at
   * PAMI_Client_destroy time.
   *
   * The world geometry is scoped to the client object, meaning each client geometry
   * will have a world geometry associated with it.
   *
   * The world_geometry has geometry_id 0, which means that the PAMI user should not
   * create any new geometries using id 0.
   * This call returns a pointer to the world geometry
   * This geometry represents context offset 0 on all tasks
   * in the current job.
   *
   * \param [in]  client          The PAMI client
   * \param [out] world_geometry  world geometry object
   *
   * \retval ::PAMI_SUCCESS       The world geometry object was initialized.
   */
  pami_result_t PAMI_Geometry_world (pami_client_t                client,
                                     pami_geometry_t            * world_geometry);

  /**
   * \brief Determines the number of algorithms available for a given collective operation.
   *
   * The algorithms are returned in two different lists; an "always works" list,
   * and an "under-certain conditions" list.
   *
   * \param [in]     geometry      Geometry to be analyzed.
   * \param [in]     coll_type     Type of collective operation
   * \param [in,out] lists_lengths Array of 2 elements representing all valid
   *                               algorithms and optimized algorithms.
   *
   * \retval ::PAMI_SUCCESS  number of algorithms is determined.
   * \retval ::PAMI_INVAL    There is an error with input parameters
   */
  pami_result_t PAMI_Geometry_algorithms_num (pami_geometry_t    geometry,
                                              pami_xfer_type_t   coll_type,
                                              size_t           * lists_lengths);

  /**
   * \brief Query the protocols and attributes available for a set of algorithms.
   *
   * The first lists are used to populate collectives that work under
   * any condition.  The second lists are used to populate
   * collectives that the metadata must be checked before use.
   *
   * \param [in]     geometry  Geometry to be analyzed.
   * \param [in]     colltype  Type of collective operation to query
   * \param [in,out] algs0     Array of algorithms to query
   * \param [in,out] mdata0    Metadata array to be filled in if algorithms
   *                           are applicable; can be NULL.
   * \param [in]     num0      Number of algorithms to query
   * \param [in,out] algs1     Array of algorithms to query
   * \param [in,out] mdata1    Metadata array to be filled in if algorithms
   *                           are applicable; can be NULL.
   * \param [in]     num1      number of algorithms to fill in.
   *
   * \retval ::PAMI_SUCCESS    The algorithm is applicable to geometry.
   * \retval ::PAMI_INVAL      Error in input arguments or not applicable.
   */
  pami_result_t PAMI_Geometry_algorithms_query (pami_geometry_t    geometry,
                                                pami_xfer_type_t   colltype,
                                                pami_algorithm_t * algs0,
                                                pami_metadata_t  * mdata0,
                                                size_t             num0,
                                                pami_algorithm_t * algs1,
                                                pami_metadata_t  * mdata1,
                                                size_t             num1);

  /**
   * \brief Free any memory allocated inside of a geometry.
   *
   * PAMI_Geometry_destroy will free any internal resources allocated
   * during a geometry create routine.  This may involve syncronization
   * with other tasks in the geometry to free the resource, and thus
   * requires a callback to be run before completion.  Synchronization with other
   * tasks can not be assumed, however, as the operation to free the
   * geometry may be local only, and dependant on the hardware and networks
   * used for communication.
   *
   * PAMI_Geometry_destroy is nonblocking.  After the callback function
   * has been called, the user is free to reuse the geometry id
   * of the geometry being freed, subject to the geometry id creation
   * rules.  It is valid to assume the geometry has been destroyed in
   * the callback function.
   *
   * It is invalid to destroy geometry 0 (the world geometry)
   *
   * A valid client, geometry pointer, and context should provided
   * to this API.  The fn can be NULL, but the user cannot re-use
   * the geometry id(provided to the geometry create routine)
   * for the lifecycle of the client.  If the event function is NULL
   * there is no way for the user to determine when the resources
   * can be reused.
   *
   * The geometry handle will be changed to an invalid value so that
   * it is clearly destroyed.
   *
   * \param [in]  client    The PAMI client
   * \param [in]  geometry  The geometry object to free
   * \param [in]  context   context to deliver async callback to
   * \param [in]  fn        event function to call when geometry has been destroyed
   * \param [in]  cookie    user cookie to deliver with the callback
   *
   * \retval ::PAMI_SUCCESS Memory free didn't fail
   */
  pami_result_t PAMI_Geometry_destroy(pami_client_t          client,
                                      pami_geometry_t      * geometry,
                                      pami_context_t         context,
                                      pami_event_function    fn,
                                      void                 * cookie);

  /**
   * \brief Non-blocking alltoall vector operation parameters.
   */
  typedef struct
  {
    char        * sndbuf;      /**< The base address of the buffers containing data to be sent */
    pami_type_t   stype;       /**< Send buffer type; ignored if sndbuf == PAMI_IN_PLACE */
    size_t      * stypecounts; /**< Array of type replication counts; size of geometry length;
                                    ignored if sndbuf == PAMI_IN_PLACE */
    size_t      * sdispls;     /**< Array of stype counts into the sndbuf; size of geometry length;
                                    ignored if sndbuf == PAMI_IN_PLACE */
    char        * rcvbuf;      /**< The base address of the buffer for data reception */
    pami_type_t   rtype;       /**< Receive buffer type */
    size_t      * rtypecounts; /**< Array of type replication counts; size of geometry length */
    size_t      * rdispls;     /**< Array of rtype counts into the rcvbuf; size of geometry length */
  } pami_alltoallv_t;

  /**
   * \brief Non-blocking alltoall vector operation parameters.
   */
  typedef struct
  {
    char        * sndbuf;      /**< The base address of the buffers containing data to be sent */
    pami_type_t   stype;       /**< Send buffer type; ignored if sndbuf == PAMI_IN_PLACE */
    int         * stypecounts; /**< Array of type replication counts; size of geometry length;
                                    ignored if sndbuf == PAMI_IN_PLACE */
    int         * sdispls;     /**< Array of stype counts into the sndbuf; size of geometry length;
                                    ignored if sndbuf == PAMI_IN_PLACE */
    char        * rcvbuf;      /**< The base address of the buffer for data reception */
    pami_type_t   rtype;       /**< Receive buffer type */
    int         * rtypecounts; /**< Array of type replication counts; size of geometry length */
    int         * rdispls;     /**< Array of rtype counts into the rcvbuf; size of geometry length */
  } pami_alltoallv_int_t;

  /**
   * \brief Non-blocking alltoall operation parameters.
   */
  typedef struct
  {
    char        * sndbuf;     /**< The base address of the buffers containing data to be sent */
    pami_type_t   stype;      /**< Send buffer type; ignored if sndbuf == PAMI_IN_PLACE */
    size_t        stypecount; /**< Send buffer type count; ignored if sndbuf == PAMI_IN_PLACE */
    char        * rcvbuf;     /**< The base address of the buffer for data reception */
    pami_type_t   rtype;      /**< Receive buffer type */
    size_t        rtypecount; /**< Receive buffer type count */
  } pami_alltoall_t;

  /**
   * \brief Non-blocking reduce operation parameters
   */
  typedef struct
  {
    pami_endpoint_t      root;        /**< The endpoint of the reduction root */
    char               * sndbuf;      /**< Send buffer */
    pami_type_t          stype;       /**< Send buffer type; ignored if sndbuf == PAMI_IN_PLACE */
    size_t               stypecount;  /**< Send buffer type count; ignored if sndbuf == PAMI_IN_PLACE */
    char               * rcvbuf;      /**< Receive buffer */
    pami_type_t          rtype;       /**< Receive buffer type */
    size_t               rtypecount;  /**< Receive buffer type count */
    pami_data_function   op;          /**< Reduce operation */
    void               * data_cookie; /**< Cookie to provide to each individual reduce operation */
    int                  commutative; /**< Inputs to operation are commutative */
  } pami_reduce_t;

  /**
   * \brief Non-blocking reduce_scatter operation parameters
   */
  typedef struct
  {
    char               * sndbuf;      /**< Send buffer */
    pami_type_t          stype;       /**< Send buffer type */
    size_t               stypecount;  /**< Send buffer type count */
    char               * rcvbuf;      /**< Receive buffer */
    pami_type_t          rtype;       /**< Receive buffer type */
    size_t             * rcounts;     /**< Number of elements to receive from the destination,
                                           common on all nodes; index of my task is the count
                                           my task will receive */
    pami_data_function   op;          /**< Reduce operation */
    void               * data_cookie; /**< Cookie to provide to each individual reduce operation */
    int                  commutative; /**< Inputs to operation are commutative */
  } pami_reduce_scatter_t;

  /**
   * \brief Non-blocking broadcast operation parameters
   */
  typedef struct
  {
    pami_endpoint_t   root;      /**< The endpoint performing the broadcast */
    char            * buf;       /**< Source buffer to broadcast on root, destination buffer on non-root */
    pami_type_t       type;      /**< Data type layout; may be different on root/destinations */
    size_t            typecount; /**< Data type of the source buffer */
  } pami_broadcast_t;


  /**
   * \brief Non-blocking allgather parameters
   */
  typedef struct
  {
    char        * sndbuf;     /**< Send buffer */
    pami_type_t   stype;      /**< Data type of send buffer; ignored if sndbuf == PAMI_IN_PLACE */
    size_t        stypecount; /**< Send type replication count; ignored if sndbuf == PAMI_IN_PLACE */
    char        * rcvbuf;     /**< Receive buffer */
    pami_type_t   rtype;      /**< Data type of receive buffer */
    size_t        rtypecount; /**< Receive type replication count */
  } pami_allgather_t;


  /**
   * \brief Non-blocking gather parameters
   */
  typedef struct
  {
    pami_endpoint_t   root;       /**< The gather root endpoint */
    char            * sndbuf;     /**< Send buffer */
    pami_type_t       stype;      /**< Data type of send buffer; ignored if sndbuf == PAMI_IN_PLACE */
    size_t            stypecount; /**< Send type replication count; ignored if sndbuf == PAMI_IN_PLACE */
    char            * rcvbuf;     /**< Receive buffer */
    pami_type_t       rtype;      /**< Data type of receive buffer */
    size_t            rtypecount; /**< Receive type replication count */
  } pami_gather_t;

  /**
   * \brief Non-blocking gatherv with size_t-type arrays parameters
   */
  typedef struct
  {
    pami_endpoint_t   root;        /**< The gatherv root endpoint */
    char            * sndbuf;      /**< Base address of the buffers containing data to be sent */
    pami_type_t       stype;       /**< Type of all source buffers; ignored if sndbuf == PAMI_IN_PLACE */
    size_t            stypecount;  /**< Source type replication count; ignored if sndbuf == PAMI_IN_PLACE */
    char            * rcvbuf;      /**< Receive buffer */
    pami_type_t       rtype;       /**< Receive type */
    size_t          * rtypecounts; /**< Array of type replication counts; size of geometry length */
    size_t          * rdispls;     /**< Array of rtype counts into the rcvbuf; size of geometry length */
  } pami_gatherv_t;

  /**
   * \brief Non-blocking gatherv with integer-type arrays parameters
   */
  typedef struct
  {
    pami_endpoint_t   root;        /**< The gatherv_int root node endpoint */
    char            * sndbuf;      /**< Base address of the buffers containing data to be sent */
    pami_type_t       stype;       /**< Type of all source buffers; ignored if sndbuf == PAMI_IN_PLACE */
    int               stypecount;  /**< Source type replication count; ignored if sndbuf == PAMI_IN_PLACE */
    char            * rcvbuf;      /**< Receive buffer */
    pami_type_t       rtype;       /**< Receive type */
    int             * rtypecounts; /**< Array of type replication counts; size of geometry length */
    int             * rdispls;     /**< Array of rtype counts into the rcvbuf; size of geometry length */
  } pami_gatherv_int_t;

  /**
   * \brief Non-blocking allgatherv with size_t-type arrays parameters
   */
  typedef struct
  {
    char        * sndbuf;      /**< Base address of the buffers containing data to be sent */
    pami_type_t   stype;       /**< Type of all source buffers; ignored if sndbuf == PAMI_IN_PLACE */
    size_t        stypecount;  /**< Source type replication count; ignored if sndbuf == PAMI_IN_PLACE */
    char        * rcvbuf;      /**< Receive buffer */
    pami_type_t   rtype;       /**< Receive type */
    size_t      * rtypecounts; /**< Array of type replication counts; size of geometry length */
    size_t      * rdispls;     /**< Array of rtype counts into the rcvbuf; size of geometry length */
  } pami_allgatherv_t;

  /**
   * \brief Non-blocking allgatherv with integer-type arrays parameters.
   */
  typedef struct
  {
    char        * sndbuf;      /**< Base address of the buffers containing data to be sent */
    pami_type_t   stype;       /**< Type of all source buffers; ignored if sndbuf == PAMI_IN_PLACE */
    int           stypecount;  /**< Source type replication count; ignored if sndbuf == PAMI_IN_PLACE */
    char        * rcvbuf;      /**< Receive buffer */
    pami_type_t   rtype;       /**< Receive type */
    int         * rtypecounts; /**< Array of type replication counts; size of geometry length */
    int         * rdispls;     /**< Array of rtype counts into the rcvbuf; size of geometry length */
  } pami_allgatherv_int_t;


  /**
   * \brief Non-blocking scatter operation parameters.
   */
  typedef struct
  {
    pami_endpoint_t   root;       /**< The scatter root node endpoint */
    char            * sndbuf;     /**< Source buffer */
    pami_type_t       stype;      /**< Source buffer type */
    size_t            stypecount; /**< Source buffer type count */
    char            * rcvbuf;     /**< Receive buffer */
    pami_type_t       rtype;      /**< Receive buffer type; ignored if rcvbuf == PAMI_IN_PLACE */
    size_t            rtypecount; /**< Receive buffer type count; ignored if rcvbuf == PAMI_IN_PLACE */
  } pami_scatter_t;

  /**
   * \brief Non-blocking scatterv operation parameters.
   */
  typedef struct
  {
    pami_endpoint_t   root;        /**< The scatterv root node endpoint */
    char            * sndbuf;      /**< Base address of the buffers containing data to be sent */
    pami_type_t       stype;       /**< Source type for all buffers */
    size_t          * stypecounts; /**< Array of type replication counts; size of geometry length */
    size_t          * sdispls;     /**< Array of stype counts into the sndbuf; size of geometry length */
    char            * rcvbuf;      /**< Receive buffer */
    pami_type_t       rtype;       /**< Receive buffer type; ignored if rcvbuf == PAMI_IN_PLACE */
    size_t            rtypecount;  /**< Receive buffer type count; ignored if rcvbuf == PAMI_IN_PLACE */
  } pami_scatterv_t;

  /**
   * \brief Non-blocking scatterv_int operation parameters.
   */
  typedef struct
  {
    pami_endpoint_t   root;        /**< The scatterv_int root node endpoint */
    char            * sndbuf;      /**< Base address of the buffers containing data to be sent */
    pami_type_t       stype;       /**< Source type for all buffers */
    int             * stypecounts; /**< Array of type replication counts; size of geometry length */
    int             * sdispls;     /**< Array of stype counts into the sndbuf; size of geometry length */
    char            * rcvbuf;      /**< Receive buffer */
    pami_type_t       rtype;       /**< Receive buffer type; ignored if rcvbuf == PAMI_IN_PLACE */
    int               rtypecount;  /**< Receive buffer type count; ignored if rcvbuf == PAMI_IN_PLACE */
  } pami_scatterv_int_t;

  /**
   * \brief Non-blocking allreduce operation parameters.
   */
  typedef struct
  {
    char               * sndbuf;      /**< Source buffer */
    pami_type_t          stype;       /**< Source buffer type; ignored if sndbuf == PAMI_IN_PLACE */
    size_t               stypecount;  /**< Source buffer type count; ignored if sndbuf == PAMI_IN_PLACE */
    char               * rcvbuf;      /**< Receive buffer */
    pami_type_t          rtype;       /**< Receive buffer type */
    size_t               rtypecount;  /**< Receive buffer type count */
    pami_data_function   op;          /**< Reduce operation */
    void               * data_cookie; /**< Cookie to provide to each individual reduce operation */
    int                  commutative; /**< Inputs to operation are commutative */
  } pami_allreduce_t;

  /**
   * \brief Non-blocking scan operation parameters.
   */
  typedef struct
  {
    char               * sndbuf;      /**< Source buffer */
    pami_type_t          stype;       /**< Source buffer type; ignored if sndbuf == PAMI_IN_PLACE */
    size_t               stypecount;  /**< Source buffer type count; ignored if sndbuf == PAMI_IN_PLACE */
    char               * rcvbuf;      /**< Receive buffer */
    pami_type_t          rtype;       /**< Receive buffer type */
    size_t               rtypecount;  /**< Receive buffer type count */
    pami_data_function   op;          /**< Reduce operation */
    void               * data_cookie; /**< Cookie to provide to each individual reduce operation */
    int                  exclusive;   /**< Scan operation is exclusive of current node */
  } pami_scan_t;

  /**
   * \brief Non-blocking barrier operation parameters.
   *
   * \note The barrier collective operation does not require any additional
   *       parameters.  This structure is defined for completeness purposes.
   */
  typedef struct
  {
  } pami_barrier_t;

  /**
   * \brief Non-blocking active message broadcast operation parameters.
   *
   * This differs from AMSend in only one particular: it takes geometry/team
   * as an argument. The semantics are as follows: the included header and data
   * are broadcast to every place in the team. The completion handler is invoked
   * on the sender side as soon as send buffers can be reused. On the receive
   * side the usual two-phase reception protocol is executed: a header handler
   * determines the address to which to deposit the data and sets the address
   * of a receive completion hander to be invoked once the data has arrived.
   */
  typedef struct
  {
    size_t        dispatch;    /**< Registered dispatch id to use */
    void        * user_header; /**< Single metadata to send to destination in the header */
    size_t        headerlen;   /**< Number of bytes of the metadata; can be 0 */
    void        * sndbuf;      /**< Base source buffer to broadcast */
    pami_type_t   stype;       /**< Datatype of the send buffer */
    size_t        stypecount;  /**< Replication count of the send buffer data type */
  } pami_ambroadcast_t;

  /**
   * \brief The active message callback function for active message broadcast
   */
  typedef void (*pami_dispatch_ambroadcast_function) (pami_context_t         context,      /**< IN:  communication context which invoked the dispatch function */
                                                      void                 * cookie,       /**< IN:  dispatch cookie */
                                                      const void           * header_addr,  /**< IN:  header address  */
                                                      size_t                 header_size,  /**< IN:  header size     */
                                                      const void           * pipe_addr,    /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                                                      size_t                 data_size,    /**< IN:  number of bytes of message data */
                                                      pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                                                      pami_geometry_t        geometry,     /**< IN:  Geometry */
                                                      pami_recv_t          * recv);        /**< OUT: receive message structure, only needed if addr is non-NULL */


  /**
   * \brief Non-blocking active message scatter operation parameters.
   *
   * This is slightly more complicated than an AMBroadcast, because it allows
   * different headers and data buffers to be sent to everyone in the team.
   */
  typedef struct
  {
    size_t        dispatch;   /**< Registered dispatch id to use */
    void        * headers;    /**< Array of metadata to send to destination */
    size_t        headerlen;  /**< Number of bytes of every header in headers array */
    void        * sndbuf;     /**< Base source buffer to scatter (size of geometry) */
    pami_type_t   stype;      /**< Single datatype of the send buffer */
    size_t        stypecount; /**< Replication count of the send buffer data type */
  } pami_amscatter_t;

  /**
   * \brief The active message callback function for active message scatter
   */
  typedef void (*pami_dispatch_amscatter_function) (pami_context_t         context,      /**< IN:  communication context which invoked the dispatch function */
                                                    void                 * cookie,       /**< IN:  dispatch cookie */
                                                    const void           * header_addr,  /**< IN:  header address  */
                                                    size_t                 header_size,  /**< IN:  header size     */
                                                    const void           * pipe_addr,    /**< IN:  address of PAMI pipe  buffer, valid only if non-NULL        */
                                                    size_t                 data_size,    /**< IN:  number of bytes of message data */
                                                    pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                                                    pami_geometry_t        geometry,     /**< IN:  Geometry */
                                                    pami_recv_t          * recv);        /**< OUT: receive message structure, only needed if addr is non-NULL */

  /**
   * \brief Non-blocking active message gather operation parameters.
   *
   * This is the reverse of amscatter. It works as follows. The header only,
   * no data, is broadcast to the team. Each place in the team executes the
   * header handler and points to a data buffer in local space. A reverse transfer
   * then takes place (the buffer is sent from the receiver back to the sender,
   * and deposited in one of the buffers provided as part of the original call
   * (the "data" parameter).
   */
  typedef struct
  {
    size_t        dispatch;   /**< Registered dispatch id to use */
    void        * headers;    /**< Array of metadata to send to destination */
    size_t        headerlen;  /**< Number of bytes of every header in headers array */
    void        * rcvbuf;     /**< Target buffer of the gather operation (size of geometry) */
    pami_type_t   rtype;      /**< Data layout of the incoming gather */
    size_t        rtypecount; /**< Replication count of the target buffer data type */
  } pami_amgather_t;

  /**
   * \brief The active message callback function for active message gather
   */
  typedef void (*pami_dispatch_amgather_function) (pami_context_t         context,      /**< IN:  communication context which invoked the dispatch function */
                                                   void                 * cookie,       /**< IN:  dispatch cookie */
                                                   const void           * header_addr,  /**< IN:  header address  */
                                                   size_t                 header_size,  /**< IN:  header size     */
                                                   size_t                 data_size,    /**< IN:  number of bytes of message data */
                                                   pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                                                   pami_geometry_t        geometry,     /**< IN:  Geometry */
                                                   pami_recv_t          * send);        /**< OUT: send message structure */

  /**
   * \brief Non-blocking active message reduce operation parameters.
   *
   * This is fairly straightforward given how amgather works. Instead of
   * collecting the data without processing, all buffers are reduced using the
   * operation and data type provided by the sender. The final reduced data is
   * deposited in the original buffer provided by the initiator. On the receive
   * side the algorithm has the right to change the buffers provided by the header
   * handler (this may avoid having the implementor allocate more memory for
   * internal buffering).
   */
  typedef struct
  {
    size_t        dispatch;    /**< Registered dispatch id to use */
    void        * user_header; /**< Metadata to send to destinations in the header */
    size_t        headerlen;   /**< Number of bytes of metadata to send */
    void        * rcvbuf;      /**< Target buffer of the reduce operation (size of geometry) */
    pami_type_t   rtype;       /**< Data layout of the incoming reduce */
    size_t        rtypecount;  /**< Replication count of the incoming reduce */
    void        * data_cookie; /**<  */
    int           commutative; /**< Inputs to operation are commutative */
  } pami_amreduce_t;

  /**
   * \brief The active message callback function for active message reduce
   */
  typedef void (*pami_dispatch_amreduce_function) (pami_context_t         context,      /**< IN:  communication context which invoked the dispatch function */
                                                   void                 * cookie,       /**< IN:  dispatch cookie */
                                                   const void           * header_addr,  /**< IN:  header address  */
                                                   size_t                 header_size,  /**< IN:  header size     */
                                                   size_t                 data_size,    /**< IN:  number of bytes of message data */
                                                   pami_endpoint_t        origin,       /**< IN:  root initiating endpoint */
                                                   pami_geometry_t        geometry,     /**< IN:  Geometry */
                                                   pami_data_function   * op,           /**< OUT: PAMI math operation to perform on the datatype */
                                                   pami_recv_t          * send);        /**< OUT: send message structure, only needed if addr is non-NULL */

  /**
   * \brief Collective operation parameters
   */
  typedef union
  {
    pami_allreduce_t        xfer_allreduce;      /**< Allreduce collective parameters */
    pami_broadcast_t        xfer_broadcast;      /**< Broadcast collective parameters */
    pami_reduce_t           xfer_reduce;         /**< Reduce collective parameters */
    pami_allgather_t        xfer_allgather;      /**< Allgather collective parameters */
    pami_allgatherv_t       xfer_allgatherv;     /**< Allgatherv collective parameters */
    pami_allgatherv_int_t   xfer_allgatherv_int; /**< Allgatherv_int collective parameters */
    pami_scatter_t          xfer_scatter;        /**< Scatter collective parameters */
    pami_scatterv_t         xfer_scatterv;       /**< Scatterv collective parameters */
    pami_scatterv_int_t     xfer_scatterv_int;   /**< Scatterv_int collective parameters */
    pami_gather_t           xfer_gather;         /**< Gather collective parameters */
    pami_gatherv_t          xfer_gatherv;        /**< Gatherv collective parameters */
    pami_gatherv_int_t      xfer_gatherv_int;    /**< Gatherv_int collective parameters */
    pami_alltoall_t         xfer_alltoall;       /**< Alltoall collective parameters */
    pami_alltoallv_t        xfer_alltoallv;      /**< Alltoallv collective parameters */
    pami_alltoallv_int_t    xfer_alltoallv_int;  /**< Alltoallv_int collective parameters */
    pami_ambroadcast_t      xfer_ambroadcast;    /**< Active message broadcast collective parameters */
    pami_amscatter_t        xfer_amscatter;      /**< Active message scatter collective parameters */
    pami_amgather_t         xfer_amgather;       /**< Active message gather collective parameters */
    pami_amreduce_t         xfer_amreduce;       /**< Active message reduce collective parameters */
    pami_scan_t             xfer_scan;           /**< Scan collective parameters */
    pami_barrier_t          xfer_barrier;        /**< Barrier collective parameters */
    pami_reduce_scatter_t   xfer_reduce_scatter; /**< Reduce-Scatter collective parameters */
  } pami_collective_t;

  /**
   * \brief Collective operation parameter structure
   */
  typedef struct pami_xfer_t
  {
    pami_event_function      cb_done;   /**< Callback to invoke on collective completion */
    void                   * cookie;    /**< Cookie to deliver to cb_done function */
    pami_algorithm_t         algorithm; /**< Algorithm object from PAMI_Geometry_algorithms_query() */
    pami_collective_hint_t   options;   /**< Collective hints and options */
    pami_collective_t        cmd;       /**< Collective specific arguments */
  } pami_xfer_t;


  /**
   * \brief Start a collective on a context
   *
   * For each endpoint specified in the geometry, a call to PAMI_Collective must
   * be made to the corresponding context.
   *
   * It is illegal to post a collective via PAMI_Collective() to a context
   * that is not accociated with an endpoint in the geometry
   *
   * \param [in] context     context to deliver async callback
   * \param [in] cmd         Collective operation arguments
   *
   * \retval ::PAMI_SUCCESS  The collective operation has started successfully.
   */
  pami_result_t PAMI_Collective (pami_context_t context, pami_xfer_t * cmd);

  /*****************************************************************************/
  /**
   * \defgroup datatype Type System
   *
   * A type is a set of contiguous buffers with a signature
   *    { ( bytes_i, disp_i ) | i = 1..n }
   * where bytes_i is the size and disp_i is the displacement of the i-th buffer.
   * The size of a buffer is greater or equal to 0 and the displacement of a
   * buffer can be negative, 0 or positive.
   *
   * The data size of a type is the sum of all sizes of its buffers, i.e.
   *    bytes_1 + bytes_2 + ... + bytes_n.
   *
   * The extent of a type is the difference between the cursor position after
   * the last buffer is added and the cursor position (0) before the first 
   * buffer is added.
   *
   * An atom size can be associated with a type as long as the atom size is a
   * common divisor of all the buffer sizes. Refer to ::pami_data_function for
   * the relation between the atom size of a data function and the atom size of
   * a type.
   *
   * When a type is applied to an address, the type is repeated infinitely with
   * a stride that's equal to the extent of the type, in a way similar to
   * declaring a pointer in C.
   *
   * Once a type is created with PAMI_Type_create(), buffers can be added into
   * it in two ways. PAMI_Type_add_simple() adds contiguous buffers of the same
   * size repeatedly with a stride. PAMI_Type_add_typed() adds buffers of a
   * defined type repeatedly with a stride. After a type is completed by
   * PAMI_Type_complete(), it can be shared among concurrent transfers but
   * cannot be modified.
   *
   * \{
   */
  /*****************************************************************************/

  /*****************************************************************************/
  /**
   * \defgroup datatype_and_datacopy_predefines Predefined data types and copy functions
   *
   * \ref datatype_predefines can be used in operations where a ::pami_type_t
   * is provided to specify a contiguous data layout of a known type.
   *
   * \ref datacopy_predefines, for specific combinations of predefined data copy
   * functions and predefined data types, as well as all user-defined data copy
   * functions, can be used with the predefined data types.
   *
   * The predefined data types and copy functions can be used for both
   * collective and point-to-point communication.
   * \{
   */
  /*****************************************************************************/

  /*****************************************************************************/
  /**
   * \defgroup datatype_predefines Predefined data types
   *
   * \{
   */
  /*****************************************************************************/

  /*****************************************************************************/
  /**
   * \defgroup datatype_predefines_integer Integer
   *           Only valid for \ref datacopy_predefines_copy,
   *           \ref datacopy_predefines_minmax,
   *           \ref datacopy_predefines_sumprod, \ref datacopy_predefines_logical,
   *           and \ref datacopy_predefines_bitwise predefined copy functions
   *           and user-defined copy functions.
   *
   * \defgroup datatype_predefines_floatingpoint Floating point
   *           Only valid for \ref datacopy_predefines_copy,
   *           \ref datacopy_predefines_minmax and
   *           \ref datacopy_predefines_sumprod predefined copy functions
   *           and user-defined copy functions.
   *
   * \defgroup datatype_predefines_logical Logical
   *           Only valid for \ref datacopy_predefines_copy and
   *           \ref datacopy_predefines_logical predefined
   *           copy functions and user-defined copy functions.
   *
   * \defgroup datatype_predefines_complex Complex number
   *           Only valid for \ref datacopy_predefines_copy and
   *           \ref datacopy_predefines_sumprod predefined
   *           copy functions and user-defined copy functions.
   *
   * \defgroup datatype_predefines_maxloc_minloc Maxloc and minloc
   *           Only valid for \ref datacopy_predefines_copy and
   *           \ref datacopy_predefines_maxloc_minloc predefined
   *           copy functions and user-defined copy functions.
   */
  /*****************************************************************************/

  /**
   * \var PAMI_TYPE_BYTE
   * \brief Predefined data type for a contiguous data layout of bytes.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_BYTE;

  /**
   * \var PAMI_TYPE_SIGNED_CHAR
   * \brief Predefined data type for a contiguous data layout of signed char types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_SIGNED_CHAR;

  /**
   * \var PAMI_TYPE_UNSIGNED_CHAR
   * \brief Predefined data type for a contiguous data layout of unsigned char types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_UNSIGNED_CHAR;

  /**
   * \var PAMI_TYPE_SIGNED_SHORT
   * \brief Predefined data type for a contiguous data layout of signed short types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_SIGNED_SHORT;

  /**
   * \var PAMI_TYPE_UNSIGNED_SHORT
   * \brief Predefined data type for a contiguous data layout of unsigned short types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_UNSIGNED_SHORT;

  /**
   * \var PAMI_TYPE_SIGNED_INT
   * \brief Predefined data type for a contiguous data layout of signed int types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_SIGNED_INT;

  /**
   * \var PAMI_TYPE_UNSIGNED_INT
   * \brief Predefined data type for a contiguous data layout of unsigned int types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_UNSIGNED_INT;

  /**
   * \var PAMI_TYPE_SIGNED_LONG
   * \brief Predefined data type for a contiguous data layout of signed long types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_SIGNED_LONG;

  /**
   * \var PAMI_TYPE_UNSIGNED_LONG
   * \brief Predefined data type for a contiguous data layout of unsigned long types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_UNSIGNED_LONG;

  /**
   * \var PAMI_TYPE_SIGNED_LONG_LONG
   * \brief Predefined data type for a contiguous data layout of signed long long types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_SIGNED_LONG_LONG;

  /**
   * \var PAMI_TYPE_UNSIGNED_LONG_LONG
   * \brief Predefined data type for a contiguous data layout of unsigned long long types.
   * \ingroup datatype_predefines_integer
   */
  extern pami_type_t PAMI_TYPE_UNSIGNED_LONG_LONG;

  /**
   * \var PAMI_TYPE_FLOAT
   * \brief Predefined data type for a contiguous data layout of float types.
   * \ingroup datatype_predefines_floatingpoint
   */
  extern pami_type_t PAMI_TYPE_FLOAT;

  /**
   * \var PAMI_TYPE_DOUBLE
   * \brief Predefined data type for a contiguous data layout of double types.
   * \ingroup datatype_predefines_floatingpoint
   */
  extern pami_type_t PAMI_TYPE_DOUBLE;

  /**
   * \var PAMI_TYPE_LONG_DOUBLE
   * \brief Predefined data type for a contiguous data layout of long double types.
   * \ingroup datatype_predefines_floatingpoint
   */
  extern pami_type_t PAMI_TYPE_LONG_DOUBLE;

  /**
   * \var PAMI_TYPE_LOGICAL1
   * \brief Predefined data type for a contiguous data layout of 1-byte logical types.
   * \ingroup datatype_predefines_logical
   */
  extern pami_type_t PAMI_TYPE_LOGICAL1;

  /**
   * \var PAMI_TYPE_LOGICAL2
   * \brief Predefined data type for a contiguous data layout of 2-byte logical types.
   * \ingroup datatype_predefines_logical
   */
  extern pami_type_t PAMI_TYPE_LOGICAL2;

  /**
   * \var PAMI_TYPE_LOGICAL4
   * \brief Predefined data type for a contiguous data layout of 4-byte logical types.
   * \ingroup datatype_predefines_logical
   */
  extern pami_type_t PAMI_TYPE_LOGICAL4;

  /**
   * \var PAMI_TYPE_LOGICAL8
   * \brief Predefined data type for a contiguous data layout of 8-byte logical types.
   * \ingroup datatype_predefines_logical
   */
  extern pami_type_t PAMI_TYPE_LOGICAL8;

  /**
   * \var PAMI_TYPE_SINGLE_COMPLEX
   * \brief Predefined data type for a contiguous data layout of single-precision complex types.
   * \ingroup datatype_predefines_complex
   */
  extern pami_type_t PAMI_TYPE_SINGLE_COMPLEX;

  /**
   * \var PAMI_TYPE_DOUBLE_COMPLEX
   * \brief Predefined data type for a contiguous data layout of double-precision complex types.
   * \ingroup datatype_predefines_complex
   */
  extern pami_type_t PAMI_TYPE_DOUBLE_COMPLEX;

  /**
   * \var PAMI_TYPE_LOC_2INT
   * \brief Predefined data type for a contiguous data layout of 2 int types.
   * \ingroup datatype_predefines_maxloc_minloc
   */
  extern pami_type_t PAMI_TYPE_LOC_2INT;

  /**
   * \var PAMI_TYPE_LOC_2FLOAT
   * \brief Predefined data type for a contiguous data layout of 2 float types.
   * \ingroup datatype_predefines_maxloc_minloc
   */
  extern pami_type_t PAMI_TYPE_LOC_2FLOAT;

  /**
   * \var PAMI_TYPE_LOC_2DOUBLE
   * \brief Predefined data type for a contiguous data layout of 2 double types.
   * \ingroup datatype_predefines_maxloc_minloc
   */
  extern pami_type_t PAMI_TYPE_LOC_2DOUBLE;

  /**
   * \var PAMI_TYPE_LOC_SHORT_INT
   * \brief Predefined data type for a contiguous data layout of (short,int) types.
   * \ingroup datatype_predefines_maxloc_minloc
   */
  extern pami_type_t PAMI_TYPE_LOC_SHORT_INT;

  /**
   * \var PAMI_TYPE_LOC_FLOAT_INT
   * \brief Predefined data type for a contiguous data layout of (float,int) types.
   * \ingroup datatype_predefines_maxloc_minloc
   */
  extern pami_type_t PAMI_TYPE_LOC_FLOAT_INT;

  /**
   * \var PAMI_TYPE_LOC_DOUBLE_INT
   * \brief Predefined data type for a contiguous data layout of (double,int) types.
   * \ingroup datatype_predefines_maxloc_minloc
   */
  extern pami_type_t PAMI_TYPE_LOC_DOUBLE_INT;

  /**
   * \var PAMI_TYPE_LOC_LONG_INT
   * \brief Predefined data type for a contiguous data layout of (long,int) types.
   * \ingroup datatype_predefines_maxloc_minloc
   */
  extern pami_type_t PAMI_TYPE_LOC_LONG_INT;

  /**
   * \var PAMI_TYPE_LOC_LONGDOUBLE_INT
   * \brief Predefined data type for a contiguous data layout of (long double,int) types.
   * \ingroup datatype_predefines_maxloc_minloc
   */
  extern pami_type_t PAMI_TYPE_LOC_LONGDOUBLE_INT;

  /** \} */ /* end of "datatype_predefines" group */


  /*****************************************************************************/
  /**
   * \defgroup datacopy_predefines Predefined data copy functions
   *
   * The predefined copy functions provide data type operations optimized for
   * the runtime implementation and can \b only be used with \ref datatype_predefines.
   * Certain predefined copy functions are only valid to be used with specific
   * predefined data types.
   *
   * The predefined data types and copy functions can be used for both
   * collective and point-to-point communication.
   *
   * \{
   */
  /*****************************************************************************/

  /*****************************************************************************/
  /**
   * \defgroup datacopy_predefines_minmax Minimum and maximum
   *           Only valid for predefined \ref datatype_predefines_integer and
   *           \ref datatype_predefines_floatingpoint data types.
   *
   * \defgroup datacopy_predefines_sumprod Sum and product
   *           Only valid for predefined \ref datatype_predefines_integer,
   *           \ref datatype_predefines_floatingpoint, and \ref datatype_predefines_complex
   *           data types.
   *
   * \defgroup datacopy_predefines_logical Logical
   *           Only valid for predefined \ref datatype_predefines_integer and
   *           \ref datatype_predefines_logical data types.
   *
   * \defgroup datacopy_predefines_bitwise Bitwise
   *           Only valid for predefined \ref datatype_predefines_integer
   *           data types.
   *
   * \defgroup datacopy_predefines_copy Copy
   *           Valid for all \ref datatype_predefines.
   *
   * \defgroup datacopy_predefines_maxloc_minloc Maxloc and minloc
   *           Only valid for predefined \ref datatype_predefines_maxloc_minloc
   *           data types.
   *
   */
  /*****************************************************************************/

  /**
   * \var PAMI_DATA_COPY
   * \brief Set each destination element to the corresponding source element.
   * \ingroup datacopy_predefines_copy
   */
  extern pami_data_function PAMI_DATA_COPY;

  /**
   * \var PAMI_DATA_NOOP
   * \brief Do nothing.
   */
  extern pami_data_function PAMI_DATA_NOOP;

  /**
   * \var PAMI_DATA_MAX
   * \brief Set the destination element to the maximum of the source and destination elements.
   * \ingroup datacopy_predefines_minmax
   */
  extern pami_data_function PAMI_DATA_MAX;

  /**
   * \var PAMI_DATA_MIN
   * \brief Set the destination element to the minimum of the source and destination elements.
   * \ingroup datacopy_predefines_minmax
   */
  extern pami_data_function PAMI_DATA_MIN;

  /**
   * \var PAMI_DATA_SUM
   * \brief Set the destination element to the sum of the source and destination elements.
   * \ingroup datacopy_predefines_sumprod
   */
  extern pami_data_function PAMI_DATA_SUM;

  /**
   * \var PAMI_DATA_PROD
   * \brief Set the destination element to the product of the source and destination elements.
   * \ingroup datacopy_predefines_sumprod
   */
  extern pami_data_function PAMI_DATA_PROD;

  /**
   * \var PAMI_DATA_LAND
   * \brief Set the destination element to the "logical and" of the source and destination elements.
   * \ingroup datacopy_predefines_logical
   */
  extern pami_data_function PAMI_DATA_LAND;

  /**
   * \var PAMI_DATA_LOR
   * \brief Set the destination element to the "logical or" of the source and destination elements.
   * \ingroup datacopy_predefines_logical
   */
  extern pami_data_function PAMI_DATA_LOR;

  /**
   * \var PAMI_DATA_LXOR
   * \brief Set the destination element to the "logical exclusive-or" of the source and destination elements.
   * \ingroup datacopy_predefines_logical
   */
  extern pami_data_function PAMI_DATA_LXOR;

  /**
   * \var PAMI_DATA_BAND
   * \brief Set the destination element to the "bitwise and" of the source and destination elements.
   * \ingroup datacopy_predefines_bitwise
   */
  extern pami_data_function PAMI_DATA_BAND;

  /**
   * \var PAMI_DATA_BOR
   * \brief Set the destination element to the "bitwise or" of the source and destination elements.
   * \ingroup datacopy_predefines_bitwise
   */
  extern pami_data_function PAMI_DATA_BOR;

  /**
   * \var PAMI_DATA_BXOR
   * \brief Set the destination element to the "bitwise exclusive-or" of the source and destination elements.
   * \ingroup datacopy_predefines_bitwise
   */
  extern pami_data_function PAMI_DATA_BXOR;

  /**
   * \var PAMI_DATA_MAXLOC
   * \brief Set the destination element to the maximum of the source and destination elements.
   * \ingroup datacopy_predefines_maxloc_minloc
   */
  extern pami_data_function PAMI_DATA_MAXLOC;

  /**
   * \var PAMI_DATA_MINLOC
   * \brief Set the destination element to the minimum of the source and destination elements.
   * \ingroup datacopy_predefines_maxloc_minloc
   */
  extern pami_data_function PAMI_DATA_MINLOC;

  /** \} */ /* end of "datacopy_predefines" group */
  /** \} */ /* end of "datatype_and_datacopy_predefines" group */

  /**
   * \brief Create a new type for noncontiguous transfers
   *
   * \param [out] type       Type identifier to be created
   *
   * \retval ::PAMI_SUCCESS  The type is created.
   * \retval ::PAMI_ENOMEM   Out of memory.
   */
  pami_result_t PAMI_Type_create (pami_type_t * type);

  /**
   * \brief Append simple contiguous buffers to an existing type identifier
   *
   * A cursor, starting from 0, tracks the placement of buffers in a type.
   * Simple buffers are placed by this pseudo-code.
   *
   * \code
   * cursor += offset;
   * while (count--) {
   *   Put a contiguous buffer of bytes at the cursor;
   *   cursor += stride;
   * }
   * \endcode
   *
   * If \c count is 0, this function simply moves the cursor. It is valid to
   * move the cursor forward or backward. It is also valid to place overlapping
   * buffers in a type but the overlapping buffers hold undefined data when
   * such a type is used in data manipulation.
   *
   * The cursor is of size_t. To move the cursor backward, one can cast a
   * negative number to size_t, such as (size_t)-1, and use it as an offset
   * or a stride.
   *
   * \param [in,out] type   Type identifier to be modified
   * \param [in]     bytes  Number of bytes of each contiguous buffer
   * \param [in]     offset Offset from the cursor to place the buffers
   * \param [in]     count  Number of buffers
   * \param [in]     stride Stride between buffers
   *
   * \retval ::PAMI_SUCCESS The buffers are added to the type.
   * \retval ::PAMI_INVAL   A completed type cannot be modified.
   * \retval ::PAMI_ENOMEM  Out of memory.
   */
  pami_result_t PAMI_Type_add_simple (pami_type_t type,
                                      size_t      bytes,
                                      size_t      offset,
                                      size_t      count,
                                      size_t      stride);

  /**
   * \brief Append typed buffers to an existing type identifier
   *
   * A cursor, starting from 0, tracks the placement of buffers in a type.
   * Typed buffers are placed by this pseudo-code.
   *
   * \code
   * cursor += offset;
   * while (count--) {
   *   Put a typed buffer of subtype at the cursor;
   *   cursor += stride;
   * }
   * \endcode
   *
   * The cursor movement in \c subtype has no impact to the cursor of \c type.
   *
   * If \c count is 0, this function simply moves the cursor. It is valid to
   * move the cursor forward or backward. It is also valid to place overlapping
   * buffers in a type but the overlapping buffers hold undefined data when
   * such a type is used in data manipulation.
   *
   * The cursor is of size_t. To move the cursor backward, one can cast a
   * negative number to size_t, such as (size_t)-1, and use it as an offset
   * or a stride.
   *
   * \warning It is considered \b illegal to append an incomplete type to
   *          another type.
   *
   * \param [in,out] type    Type identifier to be modified
   * \param [in]     subtype Type of each typed buffer
   * \param [in]     offset  Offset from the cursor to place the buffers
   * \param [in]     count   Number of buffers
   * \param [in]     stride  Stride between buffers
   *
   * \retval ::PAMI_SUCCESS  The buffers are added to the type.
   * \retval ::PAMI_INVAL    A completed type cannot be modified or an incomplete
   *                         subtype cannot be added.
   * \retval ::PAMI_ENOMEM   Out of memory.
   */
  pami_result_t PAMI_Type_add_typed (pami_type_t type,
                                     pami_type_t subtype,
                                     size_t      offset,
                                     size_t      count,
                                     size_t      stride);

  /**
   * \brief Complete the type identifier
   *
   * The atom size of a type must divide the size of any contiguous buffer
   * that's described by the type. An atom size of one is valid for any type.
   *
   * \warning It is considered \b illegal to modify a type layout after it
   *          has been completed.
   *
   * \param [in] type        Type identifier to be completed
   * \param [in] atom_size   Atom size of the type
   *
   * \retval ::PAMI_SUCCESS  The type is complete.
   * \retval ::PAMI_INVAL    The atom size in invalid.
   */
  pami_result_t PAMI_Type_complete (pami_type_t type,
                                    size_t      atom_size);

  /**
   * \brief Destroy the type
   *
   * The type handle will be changed to an invalid value so that it is
   * clearly destroyed.
   *
   * \param [in] type       Type identifier to be destroyed
   *
   * \retval ::PAMI_SUCCESS The type has been destroyed and is no longer valid
   */
  pami_result_t PAMI_Type_destroy (pami_type_t * type);

  /**
   * \brief Serialize a type
   *
   * Serialize a type and retrieve the address and the size of a serialized
   * type object, which can be copied or transferred like normal data.
   * A serialized type object can be reconstructed into a type with
   * \c PAMI_Type_deserialize.
   *
   * The serialization is internal to PAMI and not into user-allocated
   * memory. Serializing an already-serialized type retrieves the address
   * and the size of the serialized type object.
   *
   * \note A PAMI implementation can choose to keep the internal representation
   * of a type always serialized. Otherwise, it needs to handle serialization
   * while a type is in use.
   *
   * \param [in]  type       Type identifier to be serialized
   * \param [out] address    Address of the serialized type object
   * \param [out] size       Size of the serialized type object
   *
   * \retval ::PAMI_SUCCESS  The serialization is successful.
   * \retval ::PAMI_INVAL    The type is invalid.
   * \retval ::PAMI_ENOMEM   Out of memory.
   */
  pami_result_t PAMI_Type_serialize (pami_type_t   type,
                                     void       ** address,
                                     size_t      * size);

  /**
   * \brief Reconstruct a new type from a serialized type object
   *
   * Successful reconstruction completes the new type and the new type does
   * not depend on the memory of the serialized type object.
   *
   * A reconstructed type can be destroyed by \c PAMI_Type_destroy.
   *
   * \param [out] type      Type identifier to be created
   * \param [in]  address   Address of the serialized type object
   * \param [in]  size      Size of the serialized type object
   *
   * \retval ::PAMI_SUCCESS The reconstruction is successful.
   * \retval ::PAMI_INVAL   The serialized type object is corrupted.
   * \retval ::PAMI_ENOMEM  Out of memory.
   */
  pami_result_t PAMI_Type_deserialize (pami_type_t * type,
                                       void        * address,
                                       size_t        size);

  /**
   * \brief Query the attributes of a type
   *
   * The type being queried must have completed.
   *
   * \param [in] type           The type to query
   * \param [in] configuration  The configuration attributes to query
   * \param [in] num_configs    The number of configuration elements
   *
   * \retval ::PAMI_SUCCESS     The update has completed successfully.
   * \retval ::PAMI_INVAL       The update has failed due to invalid parameters.
   */
  pami_result_t PAMI_Type_query (pami_type_t           type,
                                 pami_configuration_t  configuration[],
                                 size_t                num_configs);

  /**
   * \brief Transform typed data between buffers in the same address space
   *
   * \param [in] src_addr    Source buffer address
   * \param [in] src_type    Source data type
   * \param [in] src_offset  Starting offset of source data type
   * \param [in] dst_addr    Destination buffer address
   * \param [in] dst_type    Destination data type
   * \param [in] dst_offset  Starting offset of destination data type
   * \param [in] size        Amount of data to transform
   * \param [in] data_fn     Function to transform the data
   * \param [in] cookie      Argument to data function
   *
   * \retval ::PAMI_SUCCESS  The operation has completed successfully.
   * \retval ::PAMI_INVAL    The operation has failed due to invalid parameters,
   *                         e.g. incomplete types.
   */
  pami_result_t PAMI_Type_transform_data (void               * src_addr,
                                          pami_type_t          src_type,
                                          size_t               src_offset,
                                          void               * dst_addr,
                                          pami_type_t          dst_type,
                                          size_t               dst_offset,
                                          size_t               size,
                                          pami_data_function   data_fn,
                                          void               * cookie);

  /** \} */ /* end of "datatype" group */

  /**
   * \brief Active message dispatch callbacks
   */
  typedef union
  {
    pami_dispatch_p2p_function         p2p;         /**< point-to-point dispatch function */
    pami_dispatch_ambroadcast_function ambroadcast; /**< active message broadcast dispatch function */
    pami_dispatch_amscatter_function   amscatter;   /**< active message scatter dispatch function */
    pami_dispatch_amgather_function    amgather;    /**< active message gather dispatch function */
    pami_dispatch_amreduce_function    amreduce;    /**< active message reduce dispatch function */
  } pami_dispatch_callback_function;

  /**
   * \brief Initialize point-to-point operations for a dispatch identifier.
   *
   * Point-to-point operations on a communication context for a specific
   * dispatch identifier must be initialized on the origin task before the
   * operations may be used. The target task must initialize before any
   * point-to-point messages may be received.
   *
   * The initialization is a local, non-collective operation. There is no
   * communication between tasks.
   *
   * It is \b illegal for the application to specify different hint assertions
   * for the same client, context offset, and dispatch identifier on different
   * tasks. However, there is no specific error check that will prevent
   * specifying different hint assertions. The result of a communication
   * operation using mismatched hint assertions is \e undefined.
   *
   * \note The maximum allowed dispatch identifier attribute,
   *       ::PAMI_CONTEXT_DISPATCH_ID_MAX, can be queried with
   *       PAMI_Context_query().
   *
   * \ingroup activemessage
   *
   * \param [in] context    Communication context
   * \param [in] dispatch   Dispatch identifier to initialize
   * \param [in] fn         Dispatch receive function
   * \param [in] cookie     Dispatch function cookie
   * \param [in] options    Dispatch registration assertions
   *
   * \retval PAMI_SUCCESS   The dispatch has been set successfully.
   */
  pami_result_t PAMI_Dispatch_set (pami_context_t                    context,
                                   size_t                            dispatch,
                                   pami_dispatch_callback_function   fn,
                                   void                            * cookie,
                                   pami_dispatch_hint_t              options);

  /**
   * \brief Initialize collective active message operations for a dispatch identifier.
   *
   * Collective active message operations on a communication context for a
   * specific dispatch identifier must be initialized on the origin task before
   * the operations may be used. The target task must initialize before any
   * collective active messages may be received.
   *
   * The initialization is a local, non-collective operation. There is no
   * communication between tasks.
   *
   * It is \b illegal for the application to specify different hint assertions
   * for the same client, context offset, and dispatch identifier on different
   * tasks. However, there is no specific error check that will prevent
   * the application from specifying different hint assertions. The result of a
   * communication operation using  mismatched hint assertions is \e undefined.
   *
   * \note The maximum allowed dispatch identifier attribute,
   *       ::PAMI_CONTEXT_DISPATCH_ID_MAX, can be queried with
   *       PAMI_Context_query().
   *
   * \param [in] context    Communication context
   * \param [in] algorithm  Active message collective to set the dispatch
   * \param [in] dispatch   Dispatch identifier to initialize
   * \param [in] fn         Dispatch receive function
   * \param [in] cookie     Dispatch function cookie
   * \param [in] options    Dispatch registration assertions
   *
   * \retval PAMI_SUCCESS   The dispatch has been set successfully.
   */
  pami_result_t PAMI_AMCollective_dispatch_set(pami_context_t                    context,
                                               pami_algorithm_t                  algorithm,
                                               size_t                            dispatch,
                                               pami_dispatch_callback_function   fn,
                                               void                            * cookie,
                                               pami_collective_hint_t            options);

  /**
   * \brief Query the value of an attribute
   *
   * \ingroup configuration
   *
   * \param [in]     context        Communication context
   * \param [in]     dispatch       The PAMI dispatch
   * \param [in,out] configuration  The configuration attribute of interest
   * \param [in]     num_configs    The number of configuration elements
   *
   * \retval PAMI_SUCCESS  The query has completed successfully.
   * \retval PAMI_INVAL    The query has failed due to invalid parameters.
   */
  pami_result_t PAMI_Dispatch_query (pami_context_t        context,
                                     size_t                dispatch,
                                     pami_configuration_t  configuration[],
                                     size_t                num_configs);

  /**
   * \brief Update the value of an attribute
   *
   * \ingroup configuration
   *
   * \param [in] context        Communication context
   * \param [in] dispatch       The PAMI dispatch
   * \param [in] configuration  The configuration attribute to update
   * \param [in] num_configs    The number of configuration elements
   *
   * \retval PAMI_SUCCESS  The update has completed successfully.
   * \retval PAMI_INVAL    The update has failed due to invalid parameters.
   *                       For example, trying to update a read-only attribute.
   */
  pami_result_t PAMI_Dispatch_update (pami_context_t        context,
                                      size_t                dispatch,
                                      pami_configuration_t  configuration[],
                                      size_t                num_configs);

  /**
   * \brief Query the value of an attribute
   *
   * \ingroup configuration
   *
   * \param [in]     client         The PAMI client
   * \param [in,out] configuration  The configuration attribute of interest
   * \param [in]     num_configs    The number of configuration elements
   *
   * \retval ::PAMI_SUCCESS  The query has completed successfully.
   * \retval ::PAMI_INVAL    The query has failed due to invalid parameters.
   */
  pami_result_t PAMI_Client_query (pami_client_t         client,
                                   pami_configuration_t  configuration[],
                                   size_t                num_configs);

  /**
   * \brief Update the value of an attribute
   *
   * \ingroup configuration
   *
   * \param [in] client         The PAMI client
   * \param [in] configuration  The configuration attribute to update
   * \param [in] num_configs    The number of configuration elements
   *
   * \retval ::PAMI_SUCCESS  The update has completed successfully.
   * \retval ::PAMI_INVAL    The update has failed due to invalid parameters.
   *                         For example, trying to update a read-only attribute.
   */
  pami_result_t PAMI_Client_update (pami_client_t         client,
                                    pami_configuration_t  configuration[],
                                    size_t                num_configs);


  /**
  * \brief Query the value of an attribute
  *
   * \ingroup configuration
   *
  * \param [in]     context        The PAMI context
  * \param [in,out] configuration  The configuration attribute of interest
  * \param [in]     num_configs    The number of configuration elements
  *
  * \retval ::PAMI_SUCCESS  The query has completed successfully.
  * \retval ::PAMI_INVAL    The query has failed due to invalid parameters.
  */
  pami_result_t PAMI_Context_query (pami_context_t        context,
                                    pami_configuration_t  configuration[],
                                    size_t                num_configs);

  /**
   * \brief Update the value of an attribute
   *
   * \thread    This operation on the communication context is \b not
   *            \b thread-safe. It is the responsibility of the application to
   *            ensure that only one execution resource operates on a
   *            communication context at any time.
   *
   * \ingroup configuration
   *
   * \param [in] context        Communication context
   * \param [in] configuration  The configuration attribute to update
   * \param [in] num_configs    The number of configuration elements
   *
   * \retval ::PAMI_SUCCESS  The update has completed successfully.
   * \retval ::PAMI_INVAL    The update has failed due to invalid parameters.
   *                         For example, trying to update a read-only attribute.
   */
  pami_result_t PAMI_Context_update (pami_context_t        context,
                                     pami_configuration_t  configuration[],
                                     size_t                num_configs);



  /**
   * \brief Query the value of an attribute
   *
   * \ingroup configuration
   *
   * \param [in]     geometry       The PAMI geometry
   * \param [in,out] configuration  The configuration attribute of interest
   * \param [in]     num_configs    The number of configuration elements
   *
   * \retval ::PAMI_SUCCESS  The query has completed successfully.
   * \retval ::PAMI_INVAL    The query has failed due to invalid parameters.
   */
  pami_result_t PAMI_Geometry_query (pami_geometry_t       geometry,
                                     pami_configuration_t  configuration[],
                                     size_t                num_configs);

  /**
   * \brief Update the value of an attribute
   *
   * \warning Changing a Geometry configuration attribute may fundamentally
   *          alter the Geometry. Any saved knowledge (for example, algorithm lists)
   *          must be discarded and re-queried after a call to PAMI_Geometry_update().
   *
   * \note This is a collective call, and the configuration variable
   *       must be set collectively
   *
   * \ingroup configuration
   *
   * \param [in] geometry      The PAMI geometry
   * \param [in] configuration The configuration attribute to update
   * \param [in] num_configs   The number of configuration elements
   * \param [in] context       context to deliver async callback to
   * \param [in] fn            event function to call when geometry has been created
   * \param [in] cookie        user cookie to deliver with the callback
   *
   * \retval ::PAMI_SUCCESS  The update has completed successfully.
   * \retval ::PAMI_INVAL    The update has failed due to invalid parameters.
   *                         For example, trying to update a read-only attribute.
   */
  pami_result_t PAMI_Geometry_update (pami_geometry_t       geometry,
                                      pami_configuration_t  configuration[],
                                      size_t                num_configs,
                                      pami_context_t        context,
                                      pami_event_function   fn,
                                      void                 *cookie);


  /**
   * \brief Provides the detailed description of the most recent pami result.
   *
   * The "most recent pami result" is specific to each thread.
   *
   * \note  PAMI implementations may provide translated (i18n) text.
   *
   * \param [in] string Character array to write the descriptive text
   * \param [in] length Length of the character array
   *
   * \return Number of characters written into the array
   */
  size_t PAMI_Error_text (char * string, size_t length);


  /*****************************************************************************/
  /**
   * \defgroup time Time functions
   *
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief  Returns an elapsed time on the calling processor.
   * \note   This has the same definition as MPI_Wtime
   *
   * \param [in] client Client handle.
   *
   * \return Time in seconds since an arbitrary time in the past.
   */
  double PAMI_Wtime (pami_client_t client);

  /**
   * \brief  Returns the number of "cycles" elapsed on the calling processor.
   *
   * "Cycles" could be any quickly and continuously increasing counter
   * if true cycles are unavailable.
   *
   * \param [in] client Client handle.
   *
   * \return Number of "cycles" since an arbitrary time in the past.
   */
  unsigned long long PAMI_Wtimebase (pami_client_t client);

  /** \} */ /* end of "Time" group */


  /*****************************************************************************/
  /**
   * \defgroup contexts_and_endpoints Multi-context messaging
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Initialize the runtime for a client program
   *
   * A client \e program is any software that invokes a PAMI function.
   * This includes applications, libraries, and other middleware. Example
   * client names include:
   * - "MPI"
   * - "UPC"
   * - "ARMCI"
   *
   * The opaque client object represents a collection of resources to enable
   * network communications. Resources are allocated and assigned when the
   * client is created.
   *
   * A client must be initialized with an identical name and configuration on
   * each process to enable communication between clients. Clients are unable to
   * communicate with any client that has been initialized with a different
   * configuration. This feature enables the independent development of
   * middleware software and allows middleware to be used concurrently by an
   * application.
   *
   * A communication context must be created before any data transfer functions
   * may be invoked.
   *
   * \note Client creation may be a synchronizing event, but is not required
   *       to be implemented as a synchronizing event. Application code must
   *       not make any assumption about synchronization during client
   *       creation, and therefore must create clients in the same order in
   *       all processes of the job.
   *
   * \see PAMI_Context_createv
   *
   * \param [in]  name           Client unique name
   * \param [out] client         Opaque client object
   * \param [in]  configuration  Array of configurable attributes and values
   * \param [in]  num_configs    Number of configurations, may be zero
   *
   * \retval ::PAMI_SUCCESS  The client has been successfully created.
   * \retval ::PAMI_INVAL    The client name has been rejected by the runtime.
   *                         It happens when a job scheduler requires the client
   *                         name to match what's in the job description.
   */
  pami_result_t PAMI_Client_create (const char           * name,
                                    pami_client_t        * client,
                                    pami_configuration_t   configuration[],
                                    size_t                 num_configs);

  /**
   * \brief Finalize the runtime for a client program
   *
   * The application must destroy any communication contexts associated with
   * the client before the client is destroyed.
   *
   * This function is \b not \b thread-safe and it is the responsibility of the
   * application to ensure that one, and only one, application thread destroys
   * the client. After the client is destroyed the client handle will be changed
   * to an invalid value so that it is are clearly destroyed. It is \b illegal
   * to invoke any functions using a destroyed client.
   *
   * \note The PAMI_Context_lock(), PAMI_Context_trylock(), and
   *       PAMI_Context_unlock(), functions are not available to be used to
   *       ensure thread-safe access to the client destroy function as the
   *       locks, and the contexts associated with the locks, are required to
   *       have been previously destroyed by the application.
   *
   * \param [in] client Opaque client object
   *
   * \retval ::PAMI_SUCCESS  The client has been successfully destroyed.
   * \retval ::PAMI_INVAL    The client is invalid, e.g. already destroyed.
   */
  pami_result_t PAMI_Client_destroy (pami_client_t * client);

  /****************************************************************************/
  /**
   * \defgroup endpoint Endpoints
   *
   * Endpoints are opaque objects that are used to address a destination
   * in a client and are constructed from a client, task, and context offset.
   * The application must not directly read or write the value of the object.
   *
   * - The client is required to disambiguate the task and context offset
   *   identifiers, as these identifiers may be the same for multiple clients
   *
   * - The task is required to construct an endpoint to address the specific
   *   process that contains the destination context
   *
   * - The context offset is required to identify the specific context on the
   *   destination task and corresponds to the location in the array of contexts
   *   created by PAMI_Context_createv()
   *
   * Point-to-point communication operations, such as send, put, and get, will
   * address a destination with the opaque endpoint object. Collective
   * communication operations are addressed by an opaque geometry object which
   * may be constucted from a set of endpoints
   *
   * \par Alternative endpoint identification
   *
   * Applications may prefer to use a unique integer, instead of the endpoint
   * opaque type, to identify all communication destinations for a client. This
   * can be accomplished by creating an array of endpoint opaque objects and
   * using the index into the array as the destination identifier.
   *
   * See to the \link endpoint_table.c endpoint table example \endlink for more
   * information.
   *
   * An array of all endpoints in a client can be large. One strategy to reduce
   * the endpoint array memory requirements is to create the endpoint table in
   * shared memory in an environment where multiple processes have access to the
   * same shared memory area. It is the responsibility of the application to
   * allocate this shared memory area and coordinate the initialization and
   * access of any shared data structures. This includes any endpoint opaque
   * objects which may be created by one process and read by another process.
   *
   * \note The internal implementation of the endpoint opaque object will not
   *       contain any pointers to the local address space of a particular
   *       process, as doing so will prevent the application from placing the
   *       array of endpoints in a shared memory area to be used, read-only, by
   *       all tasks with access to the shared memory area.
   *
   * \{
   */
  /****************************************************************************/

  /**
   * \brief Create an individual endpoint identifier to address a destination
   *        for communication operations
   *
   * \param [in]  client     Opaque destination client object
   * \param [in]  task       Opaque destination task object
   * \param [in]  offset     Destination context offset
   * \param [out] endpoint   Opaque endpoint object
   *
   * \retval ::PAMI_SUCCESS  Valid endpoint from client, task, and offset
   */
  pami_result_t PAMI_Endpoint_create (pami_client_t     client,
                                      pami_task_t       task,
                                      size_t            offset,
                                      pami_endpoint_t * endpoint);

  /**
   * \brief Retrieve the task and context offset associated with an endpoint
   *
   * The endpoint must have been previously initialized.
   *
   * \see PAMI_Endpoint_create()
   *
   * \param [in]  endpoint   Opaque endpoint object
   * \param [out] task       Opaque destination task object
   * \param [out] offset     Destination context offset
   *
   * \retval ::PAMI_SUCCESS  Valid endpoint task and offset
   */
  pami_result_t PAMI_Endpoint_query (pami_endpoint_t   endpoint,
                                     pami_task_t     * task,
                                     size_t          * offset);

  /** \} */ /* end of "endpoint" group */

  /*****************************************************************************/
  /**
   * \defgroup context Communication contexts
   *
   * Communication contexts are a partition of the local resources assigned to
   * the client object for each task. Every context within a client has
   * equivalent functionality and semantics. The application must not directly
   * read or write the value of the ::pami_context_t opaque object.
   *
   * It is the responsibility of the application to ensure that all
   * communication contexts created for a client are advanced by a thread or
   * execution resource to prevent deadlocks. This is the "all advance" rule.
   *
   * All point-to-point and collective operations are posted to a local
   * communication context and delivered to a communication context on a remote
   * task. The runtime could implement \e horizontal parallelism by injecting
   * data or processing events across multiple communication contexts associated
   * with the client. Consequently, data can be received across multiple
   * communication contexts. To guarantee progress of a single operation, and
   * avoid deadlocks, every context must be advanced.
   *
   * The "all advance" rule may be relaxed for an application with expert
   * knowledge about the communication patterns being used. To do this the
   * application must specify special hints to disable "horizontal", or
   * cross-context, parallelism. Refer to the pami_dispatch_hint_t.multicontext
   * and pami_collective_hint_t.multicontext option. These options must be set
   * to pami_hint_t::PAMI_HINT_DISABLE to disable multi-context parallelization
   * and relax the "all advance" progress rule.
   *
   * \thread  The progress engine for each communication context is independent
   *          and may be advanced concurrently by using multiple threads to
   *          invoke the progress functions. However, operations on contexts are
   *          critical sections and \b not \b thread-safe. The application must
   *          ensure that critical sections are protected from re-entrant use.
   *          All callback functions associated with a communication operation
   *          on a communication context will be invoked by the thread or
   *          execution resource that advances the context.
   *
   * \warning The progress functions, such as PAMI_Context_advance(), are \b not
   *          \b re-entrant for a given communication context. Application code
   *          that is executed within any callback function that is invoked as a
   *          result of an advance of the internal progress engine of a
   *          communication context must not invoke any progress function,
   *          such as PAMI_Context_advance(), on the associated communication
   *          context. This restriction is required to avoid corruption of the
   *          internal progress engine state.
   *
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Create and initialize one or more independent communication contexts
   *
   * The maximum number of contexts that may be created for a client can be
   * determined with the PAMI_Client_query() function and the
   * ::PAMI_CLIENT_NUM_CONTEXTS attribute.
   *
   * This function is \b not \b thread-safe
   * and the application must ensure that one, and only one, thread creates the
   * communication contexts for a client.
   *
   * \note The PAMI_Context_lock(), PAMI_Context_trylock(), and
   *       PAMI_Context_unlock(), functions must not be used to ensure
   *       thread-safe access to the context create function as the lock
   *       associated with each context has not yet been created.
   *
   * Context creation is a local operation and does not involve communication or
   * synchronization with other tasks.
   *
   * \param [in]  client        Client handle
   * \param [in]  configuration Array of configurable attributes and values
   * \param [in]  num_configs   Number of configurations, may be zero
   * \param [out] context       Array of communication contexts to initialize
   * \param [in]  ncontexts     Number of contexts to be created
   *
   * \retval ::PAMI_SUCCESS  Contexts have been created.
   * \retval ::PAMI_INVAL    Configuration could not be satisfied or there were
   *                         errors in other parameters.
   */
  pami_result_t PAMI_Context_createv (pami_client_t        client,
                                      pami_configuration_t configuration[],
                                      size_t               num_configs,
                                      pami_context_t       context[],
                                      size_t               ncontexts);


  /**
   * \brief Destroy communication contexts for a client
   *
   * This function is \b not \b thread-safe and it is the responsibility of the
   * application to ensure that one, and only one, application thread destroys
   * the communication context(s) for a client.
   *
   * After the context(s) are
   * destroyed the context handles will be changed to an invalid value so that
   * they are clearly destroyed. It is \b illegal to invoke any functions using
   * a destroyed communication context.
   *
   * \note The PAMI_Context_lock(), PAMI_Context_trylock(), and
   *       PAMI_Context_unlock(), functions must not be used to ensure
   *       thread-safe access to the context destroy function as the lock
   *       associated with each context will be destroyed.
   *
   * \param [in,out] contexts  PAMI communication context list
   * \param [in]     ncontexts The number of contexts in the list.
   *
   * \retval ::PAMI_SUCCESS  The contexts have been destroyed.
   * \retval ::PAMI_INVAL    Some context is invalid, e.g. already destroyed.
   */
  pami_result_t PAMI_Context_destroyv (pami_context_t * contexts,
                                       size_t           ncontexts);

  /**
   * \brief Post work to a context, thread-safe
   *
   * Functions that operate on a communication context are typically not
   * thread-safe and it is the responsibility of the application to ensure that
   * these critical sections are protected from re-entrant access by multiple
   * execution resources. The PAMI_Context_post() function is unique because it
   * does \b not require that the communcation context is locked, or otherwise
   * protected from re-entrant access by multiple execution resource to ensure
   * thread safety. The runtime will internally perform any necessary atomic
   * operations in order to safely post the work to the context.
   *
   * The work callback function, with the associated cookie, will continue to be
   * invoked during communication context advance until the function does not
   * return ::PAMI_EAGAIN.
   *
   * There is no explicit completion notification provided to the \e posting
   * thread when a thread advancing the target context returns ::PAMI_SUCCESS
   * from the work callback function.  If the posting thread desires
   * a completion notification it must explicitly program such notifications.
   *
   * \param [in] context Communication context
   * \param [in] work    Opaque storage for the work, used internally
   * \param [in] fn      Event work function to invoke on the context
   * \param [in] cookie  Opaque data pointer to pass to the work function
   *
   * \return ::PAMI_SUCCESS  The work has been posted.
   * \return ::PAMI_INVAL    The post operation was rejected due to invalid parameters.
   */
  pami_result_t PAMI_Context_post (pami_context_t       context,
                                   pami_work_t        * work,
                                   pami_work_function   fn,
                                   void               * cookie);

  /**
   * \brief Advance the progress engine for a single communication context
   *
   * May complete zero, one, or more outbound transfers. May invoke event
   * callback functions to notify the application of completed events. May
   * invoke dispatch callback functions for incoming transfers. May invoke work
   * callback functions previously posted to the communication context.
   *
   * This polling advance function will return after the first poll iteration
   * that results in a processed event or, if no events are processed, after
   * polling for the maximum number of iterations.
   *
   * The advance operation is considered a critical section for the
   * communication context and this function does \b not provide thread-safe
   * access to this critical section. It is the responsibility of the
   * application to ensure that critical sections are protected from concurrent
   * access by multiple execution resources.
   *
   * \warning This function is \b not \b re-entrant for any given communication
   *          context. Application code that is executed within any callback
   *          function that is invoked as a result of an advance of the internal
   *          progress engine of a communication context must not invoke
   *          any progress function, such as PAMI_Context_advance(), on the
   *          associated communication context. This restriction is required to
   *          avoid corruption of the internal progress engine state.
   *
   * \see PAMI_Context_lock
   * \see PAMI_Context_trylock
   *
   * \param [in] context Communication context
   * \param [in] maximum Maximum number of internal poll iterations
   *
   * \retval ::PAMI_SUCCESS  An event has occurred and been processed
   * \retval ::PAMI_EAGAIN   No event has occurred
   */
  pami_result_t PAMI_Context_advance (pami_context_t context, size_t maximum);

  /**
   * \brief Advance the progress engine for multiple communication contexts
   *
   * May complete zero, one, or more outbound transfers. May invoke event
   * callback functions to notify the application of completed events. May
   * invoke dispatch callback functions for incoming transfers. May invoke work
   * callback functions previously posted to the communication context.
   *
   * This polling advance function will return after the first poll iteration
   * that results in a processed event on any context, or, if no events are
   * processed, after polling for the maximum number of iterations.
   *
   * The advance operation is considered a critical section for each individual
   * communication context and this function does \b not provide thread-safe
   * access to these critical sections. It is the responsibility of the
   * application to ensure that critical sections are protected from concurrent
   * access by multiple execution resources.
   *
   * \note It is possible to define a set of communication contexts that are
   *       always advanced together by any application thread.  It is the
   *       responsibility of the application to atomically lock the context set,
   *       perhaps by using the PAMI_Context_lock() function on a designated
   *       \e leader context, and to manage the application threads to ensure
   *       that only one thread ever advances the set of contexts.
   *
   * \warning This function is not re-entrant for an associated communication
   *          context. Application code that is executed within any callback
   *          function that is invoked as a result of an advance of the internal
   *          progress engine of a communication context \b must \b not invoke
   *          any progress function, such as PAMI_Context_advancev(), on the
   *          associated communication context. This restriction is required to
   *          avoid corruption of the internal progress engine state.
   *
   * \see PAMI_Context_lock
   * \see PAMI_Context_trylock
   *
   * \param [in] context Array of communication contexts
   * \param [in] count   Number of communication contexts
   * \param [in] maximum Maximum number of internal poll iterations on each context
   *
   * \retval ::PAMI_SUCCESS  An event has occurred and been processed.
   * \retval ::PAMI_EAGAIN   No event has occurred.
   */
  pami_result_t PAMI_Context_advancev (pami_context_t context[],
                                       size_t         count,
                                       size_t         maximum);

  /** \} */ /* end of "context" group */

  /*****************************************************************************/
  /**
   * \defgroup locks Context lock functions
   * \{
   */
  /*****************************************************************************/

  /**
   * \brief Advance the progress engine for multiple communication contexts, thread-safe
   *
   * Similar to PAMI_Context_advancev(), this function will advance the progress
   * engine for the communication contexts, however before each individual
   * advance a context lock will be attempted to protected each context from
   * re-entrant access by multiple execution resources.
   *
   * May complete zero, one, or more outbound transfers. May invoke dispatch
   * handlers for incoming transfers. May invoke work event callbacks previously
   * posted to a communication context.
   *
   * This polling advance function will return after the first poll iteration
   * that results in a processed event on any context, or if, no events are
   * processed, after polling for the maximum number of iterations. Applications
   * must not assume that events processed by other threads, or execution
   * resources, will cause this thread to return before the \c maximum number
   * of loop iterations.
   *
   * \warning This function uses context locks for mutual exclusion and will
   *          \b not be thread-safe if the other application threads or
   *          execution resources are using a different system to protect the
   *          communication context critical sections.
   *
   * \see PAMI_Context_lock
   * \see PAMI_Context_trylock
   *
   * \param [in] context Array of communication contexts
   * \param [in] count   Number of communication contexts
   * \param [in] maximum Maximum number of internal poll iterations
   *
   * \retval ::PAMI_SUCCESS  An event has occurred and been processed.
   * \retval ::PAMI_EAGAIN   No event has occurred.
   */
  pami_result_t PAMI_Context_trylock_advancev (pami_context_t context[],
                                               size_t         count,
                                               size_t         maximum);

  /**
   * \brief Acquire an atomic lock on a communication context
   *
   * This function will block until the lock is acquired.
   *
   * \note The lock cannot be assumed to be recursive or non-recursive
   *
   * \param [in] context Communication context associate with the lock
   *
   * \return ::PAMI_SUCCESS  The lock has been acquired.
   * \return ::PAMI_INVAL    The context is invalid.
   */
  pami_result_t PAMI_Context_lock (pami_context_t context);

  /**
   * \brief Attempt to acquire an atomic lock on a communication context
   *
   * \note The lock cannot be assumed to be recursive or non-recursive
   *
   * \param [in] context Communication context associate with the lock
   *
   * \return ::PAMI_SUCCESS  The lock has been acquired.
   * \return ::PAMI_EAGAIN   The lock has not been acquired. Try again later.
   * \return ::PAMI_INVAL    The context is invalid.
   */
  pami_result_t PAMI_Context_trylock (pami_context_t context);

  /**
   * \brief Release an atomic lock on a communication context
   *
   * \note The lock cannot be assumed to be recursive or non-recursive
   *
   * \param [in] context Communication context associate with the lock
   *
   * \return ::PAMI_SUCCESS  The lock has been released.
   * \return ::PAMI_INVAL    The context is invalid.
   */
  pami_result_t PAMI_Context_unlock (pami_context_t context);

  /** \} */ /* end of "lock" group */
  /** \} */ /* end of "context" group */

  /*****************************************************************************/
  /**
   * \defgroup extensions Extensions
   *
   * A PAMI extension may contain one or more extended functions or variables,
   * using the PAMI_Extension_symbol() interface.
   *
   * For example, extension-specific functions may provide a mechanism to query
   * and set configuration attributes, similar to PAMI_Context_query() and
   * PAMI_Context_update(). The extension would define new extension-specific
   * configuration functions, structs, and enums to accomplish this.
   *
   * Refer to the extension-specific documentation for more information on a
   * particular extension.
   *
   * Refer to the \ref pami_extension_framework for more information on how to
   * define, implement, and register a new extension.
   * \{
   */
  /*****************************************************************************/

  /** \brief Opaque extension handle */
  typedef void * pami_extension_t;

  /**
   * \brief Open an extension for use by a client
   *
   * The extension may also be queried during the compile pre-processing using
   * an \c ##ifdef of the form \c __pami_extension_{name}__
   *
   * \code
   * #ifdef __pami_extension_1234__
   *
   * // Use the "1234" extension
   *
   * #endif
   * \endcode
   *
   * \thread This operation on the client is \b not \b thread-safe. It is the
   *         responsibility of the application to ensure that only one
   *         execution resource opens the extension for a client at any time.
   *
   * \param [in]  client    Client handle
   * \param [in]  name      Unique extension name
   * \param [out] extension Extension handle
   *
   * \retval ::PAMI_SUCCESS The named extension is available and implemented by the PAMI runtime.
   * \retval ::PAMI_UNIMPL  The named extension is not implemented by the PAMI runtime.
   * \retval ::PAMI_ERROR   The named extension was not initialized by the PAMI runtime.
   */
  pami_result_t PAMI_Extension_open (pami_client_t      client,
                                     const char       * name,
                                     pami_extension_t * extension);

  /**
   * \brief Close an extension
   *
   * \thread This operation on the client is \b not \b thread-safe. It is the
   *         responsibility of the application to ensure that only one
   *         execution resource closes the extension for a client at any time.
   *
   * \param [in] extension Extension handle
   *
   * \retval ::PAMI_SUCCESS The extension is no longer available for use.
   */
  pami_result_t PAMI_Extension_close (pami_extension_t extension);

  /**
   * \brief Query an extension symbol
   *
   * If the named extension is available and implemented by the PAMI runtime
   * a pointer to the extension symbol is returned. This may be a function
   * pointer which can be used to invoke an extension function, or it may be
   * a pointer to an extension variable.
   *
   * \code
   * typedef void (*pami_extension_1234_foo_fn) (pami_context_t context, size_t foo);
   * typedef void (*pami_extension_1234_bar_fn) (pami_context_t context, struct iovec ** iov);
   * typedef void pami_extension_1234_var_t;
   *
   * pami_extension_1234_foo_fn pami_1234_foo =
   *   (pami_extension_1234_foo_fn) PAMI_Extension_symbol ("pami_extension_1234", "foo");
   * pami_extension_1234_bar_fn pami_1234_bar =
   *   (pami_extension_1234_bar_fn) PAMI_Extension_symbol ("pami_extension_1234", "bar");
   * pami_extension_1234_var_t * pami_1234_var =
   *   (pami_extension_1234_var_t *) PAMI_Extension_symbol ("pami_extension_1234", "var");
   *
   * pami_context_t context = ...;
   * pami_extension_1234_foo (context, 0);
   *
   * struct iovec iov[1024];
   * pami_extension_1234_bar (context, &iov);
   *
   * *var = 1234;
   * \endcode
   *
   * \param [in] extension Extension handle
   * \param [in] name      Extension symbol name
   *
   * \retval NULL Request PAMI extension is not available
   * \return PAMI extension symbol pointer
   */
  void * PAMI_Extension_symbol (pami_extension_t extension, const char * name);

  /** \} */ /* end of "extensions" group */

  /*****************************************************************************/
  /**
   * \defgroup release_notes Release Notes
   *
   * This section documents any changes from previous versions of the interface.
   *
   * The \b PAMI_TYPE_LOGICAL datatype is deprecated and removed from the interface.
   * It is replaced with new logical datatypes that specify an exact byte-width.
   * The previous \b PAMI_TYPE_LOGICAL is an alias to ::PAMI_TYPE_LOGICAL4.
   * \see PAMI_TYPE_LOGICAL1
   * \see PAMI_TYPE_LOGICAL2
   * \see PAMI_TYPE_LOGICAL4
   * \see PAMI_TYPE_LOGICAL8
   * \{
   */
  /*****************************************************************************/

  /** \} */ /* end of "release_notes" group */



#ifdef __cplusplus
};
#endif

#include "pami_sys.h"

#endif /* __pami_h__ */
