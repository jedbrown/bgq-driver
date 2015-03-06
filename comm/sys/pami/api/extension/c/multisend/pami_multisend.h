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
 * \file api/extension/c/multisend/pami_multisend.h
 * \brief platform-specific messaging interface
 */

#include "pami.h"

#ifndef __api_extension_c_multisend_pami_multisend_h__
#define __api_extension_c_multisend_pami_multisend_h__

#ifndef PAMI_EXT_ATTR
#define PAMI_EXT_ATTR 1000 /**< starting value for extended attributes */
#endif

typedef struct {
  int x; /**< X value */
  int y; /**< Y value */
  int z; /**< Z value */
} pami_coordinates_t;

typedef enum {
  /* Attribute                    Init / Query / Update    */
  PAMI_COORDINATES = PAMI_EXT_ATTR, /** Q : pami_coordinates_t: coordinates of this task */
} pami_attribute_ext_t;




  /**
   * \brief Map a task to a network address expressed as coordinates.
   *
   * \param[in] task  Id of the rank or task.
   * \param[out] ntw  coordinate object to be filled out.
   */
  pami_result_t PAMI_Task2Network(pami_task_t task,
                                  pami_coord_t *ntw);

  /**
   * \brief Map a network address expressed as coordinates into a task.
   *
   * \param[in] ntw coordinate object to map to a task.
   * \param[out] task Id of the rank or task to be assigned.
   */
  pami_result_t PAMI_Network2Task(pami_coord_t ntw,
                                  pami_task_t *task);

  /**
   * \brief Configure for Shared Circular Buffer variety.
   *
   * Only one consumer and producer are allowed.
   * Creates a circular buffer of specified size in shared memory.
   * Buffer size must be power-of-two.
   *
   * \param[out] wq Opaque memory for PipeWorkQueue
   * \param[in] bufsize Size of buffer to allocate
   */
  void PAMI_PipeWorkQueue_config_circ(pami_pipeworkqueue_t *wq, size_t bufsize);

  /**
   * \brief Configure for User-supplied Circular Buffer variety.
   *
   * Only one consumer and producer are allowed.
   * Uses the provided buffer as a circular buffer of specified size.
   * Buffer size must be power-of-two.
   * Assumes the caller has placed buffer and (this) in appropriate memory
   * for desired use - i.e. all in shared memory if to be used beyond this process.
   *
   * NOTE: details need to be worked out. The buffer actually needs to include the
   * WQ header, so the caller must somehow know how much to allocate memory -
   * and how to ensure desired alignment.
   *
   * \param[out] wq   Opaque memory for PipeWorkQueue
   * \param[in] buffer    Buffer to use
   * \param[in] bufsize Size of buffer
   */
  void PAMI_PipeWorkQueue_config_circ_usr(pami_pipeworkqueue_t *wq, char *buffer, size_t bufsize);

  /**
   * \brief Configure for Memory (flat buffer) variety.
   *
   * Only one consumer and producer are allowed. Still supports pipelining.
   * Sets up a flat buffer of specified maximum size with an arbitrary "initial fill".
   * Assumes the caller has placed buffer and (this) in appropriate memory
   * for desired use - i.e. all in shared memory if to be used beyond this process.
   *
   * \param[out] wq   Opaque memory for PipeWorkQueue
   * \param[in] buffer    Buffer to use
   * \param[in] bufsize Size of buffer
   * \param[in] bufinit Amount of data initially in buffer
   */
  void PAMI_PipeWorkQueue_config_flat(pami_pipeworkqueue_t *wq, char *buffer, size_t bufsize, size_t bufinit);

  /**
   * \brief PROPOSAL: Configure for Non-Contig Memory (flat buffer) variety.
   *
   * Only one consumer and producer are allowed. Still supports pipelining.
   * Sets up a flat buffer of specified maximum size with an arbitrary "initial fill".
   * Assumes the caller has placed buffer and (this) in appropriate memory
   * for desired use - i.e. all in shared memory if to be used beyond this process.
   *
   * This is typically only used for the application buffer, either input or output,
   * and so would not normally have both producer and consumer (only one or the other).
   * The interface is the same as for contiguous data except that "bytesAvailable" will
   * only return the number of *contiguous* bytes available. The user must consume those
   * bytes before it can see the next contiguous chunk.
   *
   * \param[out] wq            Opaque memory for PipeWorkQueue
   * \param[in] buffer         Buffer to use
   * \param[in] type           Memory layout of a buffer unit
   * \param[in] typecount      Number of repetitions of buffer units
   * \param[in] typeinit       Number of units initially in buffer
   */
  void PAMI_PipeWorkQueue_config_noncontig(pami_pipeworkqueue_t *wq, char *buffer, pami_type_t *type, size_t typecount, size_t typeinit);

  /**
   * \brief Export
   *
   * Produces information about the PipeWorkQueue into the opaque buffer "export".
   * This info is suitable for sharing with other processes such that those processes
   * can then construct a PipeWorkQueue which accesses the same data stream.
   *
   * This only has value in the case of a flat buffer PipeWorkQueue and a platform
   * that supports direct mapping of memory from other processes. Circular buffer
   * PipeWorkQueues use shared memory and are inherently inter-process in nature.
   *
   * The exporting process is the only one that can produce to the PipeWorkQueue.
   * All importing processes are restricted to consuming. There is no feedback
   * from the consumers to the producer - no flow control.
   *
   * \param[in] wq             Opaque memory for PipeWorkQueue
   * \param[out] export        Opaque memory to export into
   * \return  success of the export operation
   */
  pami_result_t PAMI_PipeWorkQueue_export(pami_pipeworkqueue_t *wq, pami_pipeworkqueue_ext_t *t_exp);

  /**
   * \brief Import
   *
   * Takes the results of an export of a PipeWorkQueue on a different process and
   * constructs a new PipeWorkQueue which the local process may use to access the
   * data stream.
   *
   * The resulting PipeWorkQueue may consume data, but that is a local-only operation.
   * The producer has no knowledge of data consumed. There can be only one producer.
   * There may be multiple consumers, but the producer does not know about them.
   * An importing processes cannot be the producer.
   *
   * TODO: can this work for circular buffers? does it need to, since those are
   * normally shared memory and thus already permit inter-process communication.
   *
   * \param[in] import        Opaque memory into which an export was done.
   * \param[out] wq           Opaque memory for new PipeWorkQueue
   * \return  success of the import operation
   */
  pami_result_t PAMI_PipeWorkQueue_import(pami_pipeworkqueue_ext_t *import, pami_pipeworkqueue_t *wq);

  /**
   * \brief Clone constructor.
   *
   * Used to create a second local memory wrapper object of the same
   * shared memory resource.
   *
   * \see WorkQueue(WorkQueue &)
   *
   * \param[out] wq Opaque memory for new PipeWorkQueue
   * \param[in] obj old object, to be cloned
   */
  void PAMI_PipeWorkQueue_clone(pami_pipeworkqueue_t *wq, pami_pipeworkqueue_t *obj);

  /**
   * \brief Destructor
   *
   * \param[out] wq Opaque memory for PipeWorkQueue
   */
  void PAMI_PipeWorkQueue_destroy(pami_pipeworkqueue_t *wq);

  /**
   * \brief Reset this pipe work queue.
   *
   * All PipeWorkQueues must be reset() at least once after configure and before using.
   * (TODO: should this simply be part of configure?)
   *
   * Sets the number of bytes produced and the number of bytes
   * consumed to zero (or to "bufinit" as appropriate).
   *
   * This is typically required by circular PipeWorkQueues that are re-used.
   * Flat PipeWorkQueues are usually configured new for each instance
   * and thus do not require resetting. Circular PipeWorkQueues should be
   * reset by only one entity, and at a point when it is known that no other
   * entity is still using it (it must be idle). For example, in a multisend pipeline
   * consisting of:
   *
   *        [barrier] ; local-reduce -(A)-&gt; global-allreduce -(B)-&gt; local-broadcast
   *
   * the PipeWorkQueue "B" would be reset by the root of the reduce when starting the
   * local-reduce operation (when it is known that any prior instances have completed).
   *
   * One reason that a reset may be needed is to preserve buffer alignment. Another is
   * to prevent problems when, say, a consumer requires a full packet of data. In this
   * case, a circular PipeWorkQueue may have been left in a state from the previous
   * operation where the final chunk of data has left less than a packet length before
   * the wrap point. This would create a hang because the consumer would never see a full
   * packet until it consumes those bytes and passes the wrap point.
   *
   * Since resets are performed by the protocol code, it understands the context and
   * whether the PipeWorkQueue represents a flat (application) buffer or an intermediate
   * (circular) one.
   *
   * \param[out] wq Opaque memory for PipeWorkQueue
   */
  void PAMI_PipeWorkQueue_reset(pami_pipeworkqueue_t *wq);

  /**
   * \brief Dump shared memory work queue statistics to stderr.
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \param[in] prefix Optional character string to prefix.
   */
  void PAMI_PipeWorkQueue_dump(pami_pipeworkqueue_t *wq, const char *prefix);

  /**
   * \brief register a wakeup for the consumer side of the PipeWorkQueue
   *
   * The 'vec' parameter is typically obtained from some platform authority,
   * which is the same used by the PipeWorkQueue to perform the wakeup.
   * A consumer wishing to be awoken when data is available would call the
   * system to get their 'vec' value, and pass it to this method to register
   * for wakeups. When the produceBytes method is called, it will use this
   * consumer wakeup 'vec' to tell the system to wake up the consumer process or thread.
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \param[in] vec Opaque wakeup vector parameter
   */
  void PAMI_PipeWorkQueue_setConsumerWakeup(pami_pipeworkqueue_t *wq, void *vec);

  /**
   * \brief register a wakeup for the producer side of the PipeWorkQueue
   *
   * See setConsumerWakeup() for details.
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \param[in] vec Opaque wakeup vector parameter
   */
  void PAMI_PipeWorkQueue_setProducerWakeup(pami_pipeworkqueue_t *wq, void *vec);

  /**
   * \brief register user-defined info for producer
   *
   * Cooperating consumer/producers may share information using
   * these interfaces. The information is static, i.e. after a
   * call to setConsumerUserInfo() with values A and B, all subsequent
   * calls to getConsumerUserInfo() will return values A and B
   * (until another call to setConsumerUserInfo() is made).
   *
   * "Producer" and "Consumer" here are just convenience references,
   * either side (or potentially third-parties) may use these
   * methods as desired to set/get the info.
   *
   * \param[in] word1    First piece of info
   * \param[in] word2    Second piece of info
   */
  void PAMI_PipeWorkQueue_setProducerUserInfo(pami_pipeworkqueue_t *wq, void *word1, void *word2);

  /**
   * \brief register user-defined info for consumer
   * \param[in] word1    First piece of info
   * \param[in] word2    Second piece of info
   */
  void PAMI_PipeWorkQueue_setConsumerUserInfo(pami_pipeworkqueue_t *wq, void *word1, void *word2);

  /**
   * \brief get user-defined info for producer
   * \param[in] word1    First piece of info
   * \param[in] word2    Second piece of info
   */
  void PAMI_PipeWorkQueue_getProducerUserInfo(pami_pipeworkqueue_t *wq, void **word1, void **word2);

  /**
   * \brief get user-defined info for consumer
   * \param[in] word1    First piece of info
   * \param[in] word2    Second piece of info
   */
  void PAMI_PipeWorkQueue_getConsumerUserInfo(pami_pipeworkqueue_t *wq, void **word1, void **word2);

  /**
   * \brief Return the number of contiguous bytes that can be produced into this work queue.
   *
   * Bytes must be produced into the memory location returned by bufferToProduce() and then
   * this work queue \b must be updated with produceBytes().
   *
   * \see bufferToProduce
   * \see produceBytes
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return Number of bytes that may be produced.
   */
  size_t PAMI_PipeWorkQueue_bytesAvailableToProduce(pami_pipeworkqueue_t *wq);

  /**
   * \brief Return the number of contiguous bytes that can be consumed from this work queue.
   *
   * Bytes must be consumed into the memory location returned by bufferToConsume() and then
   * this work queue \b must be updated with consumeBytes().
   *
   * \see bufferToConsume
   * \see consumeBytes
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \param[in] consumer Consumer id for work queues with multiple consumers
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return Number of bytes that may be consumed.
   */
  size_t PAMI_PipeWorkQueue_bytesAvailableToConsume(pami_pipeworkqueue_t *wq);

  /**
   * \brief raw accessor for total number of bytes produced since reset()
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return  number of bytes produced
   */
  size_t PAMI_PipeWorkQueue_getBytesProduced(pami_pipeworkqueue_t *wq);

  /**
   * \brief raw accessor for total number of bytes consumed since reset()
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return  number of bytes consumed
   */
  size_t PAMI_PipeWorkQueue_getBytesConsumed(pami_pipeworkqueue_t *wq);

  /**
   * \brief current position for producing into buffer
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return  location in buffer to produce into
   */
  char *PAMI_PipeWorkQueue_bufferToProduce(pami_pipeworkqueue_t *wq);

  /**
   * \brief notify workqueue that bytes have been produced
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return  number of bytes that were produced
   */
  void PAMI_PipeWorkQueue_produceBytes(pami_pipeworkqueue_t *wq, size_t bytes);

  /**
   * \brief current position for consuming from buffer
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return  location in buffer to consume from
   */
  char *PAMI_PipeWorkQueue_bufferToConsume(pami_pipeworkqueue_t *wq);

  /**
   * \brief notify workqueue that bytes have been consumed
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return  number of bytes that were consumed
   */
  void PAMI_PipeWorkQueue_consumeBytes(pami_pipeworkqueue_t *wq, size_t bytes);

  /**
   * \brief is workqueue ready for action
   *
   * \param[in] wq  Opaque memory for PipeWorkQueue
   * \return  boolean indicate workqueue readiness
   */
  int PAMI_PipeWorkQueue_available(pami_pipeworkqueue_t *wq);



  /**
   * \brief default constructor (PAMI_EMPTY_TOPOLOGY)
   *
   * \param[out] topo Opaque memory for topology
   */
  void PAMI_Topology_create(pami_topology_t *topo);

  /**
   * \brief rectangular segment with torus (PAMI_COORD_TOPOLOGY)
   *
   * Assumes no torus links if no 'tl' param.
   *
   * \param[out] topo Opaque memory for topology
   * \param[in] ll  lower-left coordinate
   * \param[in] ur  upper-right coordinate
   * \param[in] tl  optional, torus links flags
   */
  void PAMI_Topology_create_rect(pami_topology_t *topo,
                                pami_coord_t *ll, pami_coord_t *ur, unsigned char *tl);

  /**
   * \brief Axial topology (PAMI_AXIAL_TOPOLOGY)
   *
   * Assumes no torus links if 'tl' param = NULL.
   *
   * \param[out] topo Opaque memory for topology
   * \param[in] ll  lower-left coordinate
   * \param[in] ur  upper-right coordinate
   * \param[in] ref coordinates of the reference task where axes cross.
   * \param[in] tl  optional, torus links flags
   */
  void PAMI_Topology_create_axial(pami_topology_t *topo,
                                 pami_coord_t *ll,
                                 pami_coord_t *ur,
                                 pami_coord_t *ref,
                                 unsigned char *tl);

  /**
   * \brief single task constructor (PAMI_SINGLE_TOPOLOGY)
   *
   * \param[out] topo Opaque memory for topology
   * \param[in] task  The task
   */
  void PAMI_Topology_create_task(pami_topology_t *topo, pami_task_t task);

  /**
   * \brief task range constructor (PAMI_RANGE_TOPOLOGY)
   *
   * \param[out] topo Opaque memory for topology
   * \param[in] task0 first task in range
   * \param[in] taskn last task in range
   */
  void PAMI_Topology_create_range(pami_topology_t *topo, pami_task_t task0, pami_task_t taskn);

  /**
   * \brief task list constructor (PAMI_LIST_TOPOLOGY)
   *
   * caller must not free tasks[]!
   *
   * \param[out] topo Opaque memory for topology
   * \param[in] tasks array of tasks
   * \param[in] ntasks  size of array
   *
   * \todo create destructor to free list, or establish rules
   */
  void PAMI_Topology_create_list(pami_topology_t *topo, pami_task_t *tasks, size_t ntasks);

  /**
   * \brief destructor
   *
   * For PAMI_LIST_TOPOLOGY, would free the tasks list array... ?
   *
   * \param[out] topo Opaque memory for topology
   */
  void PAMI_Topology_destroy(pami_topology_t *topo);

  /**
   * \brief accessor for size of a Topology object
   *
   * \param[in] topo  Opaque memory for topology
   * \return  size of PAMI::Topology
   */
  unsigned PAMI_Topology_size_of(pami_topology_t *topo);

  /**
   * \brief number of tasks in topology
   * \param[in] topo  Opaque memory for topology
   * \return  number of tasks
   */
  size_t PAMI_Topology_size(pami_topology_t *topo);

  /**
   * \brief type of topology
   * \param[out] topo Opaque memory for topology
   * \return  topology type
   */
  pami_topology_type_t pami_topology_type(pami_topology_t *topo);

  /**
   * \brief Nth task in topology
   *
   * \param[in] topo  Opaque memory for topology
   * \param[in] ix  Which task to select
   * \return  Nth task or (pami_task_t)-1 if does not exist
   */
  pami_task_t PAMI_Topology_index2TaskID(pami_topology_t *topo, size_t ix);

  /**
   * \brief determine index of task in topology
   *
   * This is the inverse function to task(ix) above.
   *
   * \param[in] topo  Opaque memory for topology
   * \param[in] task  Which task to get index for
   * \return  index of task (task(ix) == task) or (size_t)-1
   */
  size_t PAMI_Topology_taskID2Index(pami_topology_t *topo, pami_task_t task);

  /**
   * \brief return range
   *
   * \param[in] topo  Opaque memory for topology
   * \param[out] first  Where to put first task in range
   * \param[out] last Where to put last task in range
   * \return  PAMI_SUCCESS, or PAMI_UNIMPL if not a range topology
   */
  pami_result_t PAMI_Topology_taskRange(pami_topology_t *topo, pami_task_t *first, pami_task_t *last);

  /**
   * \brief return task list
   *
   * \param[in] topo  Opaque memory for topology
   * \param[out] list pointer to list stored here
   * \return  PAMI_SUCCESS, or PAMI_UNIMPL if not a list topology
   */
  pami_result_t PAMI_Topology_taskList(pami_topology_t *topo, pami_task_t **list);

  /**
   * \brief return rectangular segment coordinates
   *
   * This method copies data to callers buffers. It is safer
   * as the caller cannot directly modify the topology.
   *
   * \param[in] topo  Opaque memory for topology
   * \param[out] ll lower-left coord pointer storage
   * \param[out] ur upper-right coord pointer storage
   * \param[out] tl optional, torus links flags
   * \return  PAMI_SUCCESS, or PAMI_UNIMPL if not a coord topology
   */
  pami_result_t PAMI_Topology_rectSeg(pami_topology_t *topo,
                                    pami_coord_t *ll, pami_coord_t *ur,
                                    unsigned char *tl);

  /**
   * \brief does topology consist entirely of tasks local to self
   *
   * \param[in] topo  Opaque memory for topology
   * \return boolean indicating locality of tasks
   */
  int PAMI_Topology_isLocalToMe(pami_topology_t *topo);

  /**
   * \brief is topology a rectangular segment
   * \param[in] topo  Opaque memory for topology
   * \return  boolean indicating rect seg topo
   */
  int PAMI_Topology_isRectSeg(pami_topology_t *topo);

  /**
   * \brief extract Nth dimensions from coord topology
   *
   * \param[in] topo  Opaque memory for topology
   * \param[in] n Which dim to extract
   * \param[out] c0 lower value for dim range
   * \param[out] cn upper value for dim range
   * \param[out] tl optional, torus link flag
   */
  void PAMI_Topology_getNthDims(pami_topology_t *topo, unsigned n,
                               unsigned *c0, unsigned *cn, unsigned char *tl);

  /**
   * \brief is task in topology
   *
   * \param[in] topo  Opaque memory for topology
   * \param[in] task  Task to test
   * \return  boolean indicating task is in topology
   */
  int PAMI_Topology_isTaskMember(pami_topology_t *topo, pami_task_t task);

  /**
   * \brief is coordinate in topology
   *
   * \param[in] topo  Opaque memory for topology
   * \param[in] c0  Coord to test
   * \return  boolean indicating coord is a member of topology
   */
  int PAMI_Topology_isCoordMember(pami_topology_t *topo, pami_coord_t *c0);

  /**
   * \brief create topology of tasks local to self
   *
   * \param[out] _new Where to build topology
   * \param[in] topo  Opaque memory for topology
   */
  void PAMI_Topology_sub_LocalToMe(pami_topology_t *_new, pami_topology_t *topo);

  /**
   * \brief create topology from all Nth tasks globally
   *
   * \param[out] _new Where to build topology
   * \param[in] topo  Opaque memory for topology
   * \param[in] n Which local task to select on each node
   */
  void PAMI_Topology_sub_NthGlobal(pami_topology_t *_new, pami_topology_t *topo, int n);

  /**
   * \brief reduce dimensions of topology (cube -> plane, etc)
   *
   * The 'fmt' param is a pattern indicating which dimensions
   * to squash, and what coord to squash into. A dim in 'fmt'
   * having "-1" will be preserved, while all others will be squashed
   * into a dimension of size 1 having the value specified.
   *
   * \param[out] _new where to build new topology
   * \param[in] topo  Opaque memory for topology
   * \param[in] fmt how to reduce dimensions
   */
  void PAMI_Topology_sub_ReduceDims(pami_topology_t *_new, pami_topology_t *topo, pami_coord_t *fmt);

  /**
   * \brief Return list of tasks representing contents of topology
   *
   * This always returns a list regardless of topology type.
   * Caller must allocate space for list, and determine an
   * appropriate size for that space. Note, there might be a
   * number larger than 'max' returned in 'ntasks', but there
   * are never more than 'max' tasks put into the array.
   * If the caller sees that 'ntasks' exceeds 'max' then it
   * should assume it did not get the whole list, and could
   * allocate a larger array and try again.
   *
   * \param[in] topo  Opaque memory for topology
   * \param[in] max size of caller-allocated array
   * \param[out] tasks  array where task list is placed
   * \param[out] ntasks actual number of tasks put into array
   */
  void PAMI_Topology_getTaskList(pami_topology_t *topo, size_t max, pami_task_t *tasks, size_t *ntasks);

  /**
   * \brief check if task range or list can be converted to rectangle
   *
   * Since a rectangular segment is consider the optimal state, no
   * other analysis is done. A PAMI_SINGLE_TOPOLOGY cannot be optimized,
   * either. Optimization levels:
   *
   *  PAMI_SINGLE_TOPOLOGY (most)
   *  PAMI_COORD_TOPOLOGY
   *  PAMI_RANGE_TOPOLOGY
   *  PAMI_LIST_TOPOLOGY (least)
   *
   * \param[in,out] topo  Opaque memory for topology
   * \return  'true' if topology was changed
   */
  int PAMI_Topology_analyze(pami_topology_t *topo);

  /**
   * \brief check if topology can be converted to type
   *
   * Does not differentiate between invalid conversions and
   * 'null' conversions (same type).
   *
   * \param[in,out] topo  Opaque memory for topology
   * \param[in] new_type  Topology type to try and convert into
   * \return  'true' if topology was changed
   */
  int PAMI_Topology_convert(pami_topology_t *topo, pami_topology_type_t new_type);

  /**
   * \brief produce the intersection of two topologies
   *
   * produces: _new = this ./\. other
   *
   * \param[out] _new New topology created there
   * \param[in] topo  Opaque memory for topology
   * \param[in] other The other topology
   */
  void PAMI_Topology_intersect(pami_topology_t *_new, pami_topology_t *topo, pami_topology_t *other);

  /**
   * \brief produce the difference of two topologies
   *
   * produces: _new = this .-. other
   *
   * \param[out] _new New topology created there
   * \param[in] topo  Opaque memory for topology
   * \param[in] other The other topology
   */
  void PAMI_Topology_subtract(pami_topology_t *_new, pami_topology_t *topo, pami_topology_t *other);


#define PAMI_DISPATCH_EXTEND


#define PAMI_DISPATCH_TYPE_EXTEND

#define PAMI_HINT_EXTEND

  /**
   * \brief Initialize the geometry using a topology
   *
   * Same semantics as the other PAMI_Geometry_create_* routines.
   *
   * \param[in]  client          pami client
   * \param[in]  configuration   List of configurable attributes and values
   * \param[in]  num_configs     The number of configuration elements
   * \param[out] geometry        Opaque geometry object to initialize
   * \param[in]  parent          Parent geometry containing all the nodes in the task list
   * \param[in]  id              Identifier for this geometry
   *                             which uniquely represents this geometry(if tasks overlap)
   * \param[in]  topology        The topology that describes the members
   * \param[in]  context         context to deliver async callback to
   * \param[in]  fn              event function to call when geometry has been created
   * \param[in]  cookie          user cookie to deliver with the callback
   *
   * \see PAMI_Geometry_create_taskrange
   */
  pami_result_t PAMI_Geometry_create_topology(pami_client_t           client,
                                              pami_configuration_t    configuration[],
                                              size_t                  num_configs,
                                              pami_geometry_t        *geometry,
                                              pami_geometry_t         parent,
                                              unsigned                id,
                                              pami_topology_t        *topology,
                                              pami_context_t          context,
                                              pami_event_function     fn,
                                              void                   *cookie);


#endif // __api_c_pami_ext_h__
