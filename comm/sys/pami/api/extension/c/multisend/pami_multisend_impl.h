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
 * \file api/extension/c/multisend/pami_multisend_impl.h
 * \brief Multisend extension C interface implementation
 */
#ifndef __api_extension_c_multisend_pami_multisend_impl_h__
#define __api_extension_c_multisend_pami_multisend_impl_h__

#include "PipeWorkQueue.h"
#include "Topology.h"
#include "api/c/pami_ext.h"

pami_result_t PAMI_Geometry_create_topology(pami_client_t           client,
                                                       pami_configuration_t    configuration[],
                                                       size_t                  num_configs,
                                                       pami_geometry_t        *geometry,
                                                       pami_geometry_t         parent,
                                                       unsigned                id,
                                                       pami_topology_t        *topology,
                                                       pami_context_t          context,
                                                       pami_event_function     fn,
                                                       void                   *cookie)
{
  PAMI::Client * _client = (PAMI::Client *) client;
  return _client->geometry_create_topology(geometry,
                                           configuration,
                                           num_configs,
                                           parent,
                                           id,
                                           topology,
                                           context,
                                           fn,
                                           cookie);
}


////////////////////////////////////////////////////////////////////////////////
// Functions from pami_pipeworkqueue.h                                         //
////////////////////////////////////////////////////////////////////////////////




///
/// \copydoc PAMI_PipeWorkQueue_config_circ
///
void PAMI_PipeWorkQueue_config_circ(pami_pipeworkqueue_t *wq,
                                   size_t bufsize)
{
    PAMI_abort();
}

///
/// \copydoc PAMI_PipeWorkQueue_config_circ_usr
///
void PAMI_PipeWorkQueue_config_circ_usr(pami_pipeworkqueue_t *wq,
                                       char                *buffer,
                                       size_t               bufsize)
{
    PAMI_abort();
}

///
/// \copydoc PAMI_PipeWorkQueue_config_flat
///
void PAMI_PipeWorkQueue_config_flat(pami_pipeworkqueue_t *wq,
                                   char                *buffer,
                                   size_t               bufsize,
                                   size_t               bufinit)
{
    PAMI_abort();
}

///
/// \copydoc PAMI_PipeWorkQueue_config_noncontig
///
void PAMI_PipeWorkQueue_config_noncontig(pami_pipeworkqueue_t *wq,
                                        char                *buffer,
                                        pami_type_t          *type,
                                        size_t               typecount,
                                        size_t               typeinit)
{
    PAMI_abort();
}

///
/// \copydoc PAMI_PipeWorkQueue_export
///
pami_result_t PAMI_PipeWorkQueue_export(pami_pipeworkqueue_t     *wq,
                                      pami_pipeworkqueue_ext_t *exp)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->exp (exp);
}

///
/// \copydoc PAMI_PipeWorkQueue_import
///
pami_result_t PAMI_PipeWorkQueue_import(pami_pipeworkqueue_ext_t *import,
                                      pami_pipeworkqueue_t     *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->import (import);
}


///
/// \copydoc PAMI_PipeWorkQueue_clone
///
void PAMI_PipeWorkQueue_clone(pami_pipeworkqueue_t *wq,
                             pami_pipeworkqueue_t *obj)
{
}


///
/// \copydoc PAMI_PipeWorkQueue_destroy
///
void PAMI_PipeWorkQueue_destroy(pami_pipeworkqueue_t *wq)
{
}

///
/// \copydoc PAMI_PipeWorkQueue_reset
///
void PAMI_PipeWorkQueue_reset(pami_pipeworkqueue_t *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    pwq->reset();
}


///
/// \copydoc PAMI_PipeWorkQueue_dump
///
void PAMI_PipeWorkQueue_dump(pami_pipeworkqueue_t *wq,
                            const char          *prefix)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    pwq->dump (prefix);
}

///
/// \copydoc PAMI_PipeWorkQueue_setConsumerWakeup
///
void PAMI_PipeWorkQueue_setConsumerWakeup(pami_pipeworkqueue_t *wq,
                                         void                *vec)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    pwq->setConsumerWakeup (vec);
}

///
/// \copydoc PAMI_PipeWorkQueue_setProducerWakeup
///
void PAMI_PipeWorkQueue_setProducerWakeup(pami_pipeworkqueue_t *wq,
                                         void                *vec)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    pwq->setProducerWakeup (vec);
}

///
/// \copydoc PAMI_PipeWorkQueue_setProducerUserInfo
///
void PAMI_PipeWorkQueue_setProducerUserInfo(pami_pipeworkqueue_t *wq,
                                         void *word1, void *word2)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    pwq->setProducerUserInfo (word1, word2);
}

///
/// \copydoc PAMI_PipeWorkQueue_setConsumerUserInfo
///
void PAMI_PipeWorkQueue_setConsumerUserInfo(pami_pipeworkqueue_t *wq,
                                         void *word1, void *word2)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    pwq->setConsumerUserInfo (word1, word2);
}

///
/// \copydoc PAMI_PipeWorkQueue_getProducerUserInfo
///
void PAMI_PipeWorkQueue_getProducerUserInfo(pami_pipeworkqueue_t *wq,
                                         void **word1, void **word2)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    pwq->getProducerUserInfo (word1, word2);
}

///
/// \copydoc PAMI_PipeWorkQueue_getConsumerUserInfo
///
void PAMI_PipeWorkQueue_getConsumerUserInfo(pami_pipeworkqueue_t *wq,
                                         void **word1, void **word2)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    pwq->getConsumerUserInfo (word1, word2);
}

///
/// \copydoc PAMI_PipeWorkQueue_bytesAvailableToProduce
///
size_t PAMI_PipeWorkQueue_bytesAvailableToProduce(pami_pipeworkqueue_t *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->bytesAvailableToProduce ();
}

///
/// \copydoc PAMI_PipeWorkQueue_bytesAvailableToConsume
///
size_t PAMI_PipeWorkQueue_bytesAvailableToConsume(pami_pipeworkqueue_t *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->bytesAvailableToConsume ();
}

///
/// \copydoc PAMI_PipeWorkQueue_getBytesProduced
///
size_t PAMI_PipeWorkQueue_getBytesProduced(pami_pipeworkqueue_t *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->getBytesProduced ();
}

///
/// \copydoc PAMI_PipeWorkQueue_getBytesConsumed
///
size_t PAMI_PipeWorkQueue_getBytesConsumed(pami_pipeworkqueue_t *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->getBytesConsumed ();
}

///
/// \copydoc PAMI_PipeWorkQueue_bufferToProduce
///
char *PAMI_PipeWorkQueue_bufferToProduce(pami_pipeworkqueue_t *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->bufferToProduce ();
}

///
/// \copydoc PAMI_PipeWorkQueue_produceBytes
///
void PAMI_PipeWorkQueue_produceBytes(pami_pipeworkqueue_t *wq,
                                    size_t               bytes)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->produceBytes (bytes);
}

///
/// \copydoc PAMI_PipeWorkQueue_bufferToConsume
///
char *PAMI_PipeWorkQueue_bufferToConsume(pami_pipeworkqueue_t *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->bufferToConsume ();
}

///
/// \copydoc PAMI_PipeWorkQueue_consumeBytes
///
void PAMI_PipeWorkQueue_consumeBytes(pami_pipeworkqueue_t *wq,
                                    size_t               bytes)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->consumeBytes (bytes);
}

///
/// \copydoc PAMI_PipeWorkQueue_available
///
int PAMI_PipeWorkQueue_available(pami_pipeworkqueue_t *wq)
{
    PAMI::PipeWorkQueue * pwq = (PAMI::PipeWorkQueue *) wq;
    return pwq->available ();
}



////////////////////////////////////////////////////////////////////////////////
// Functions from pami_topology.h                                              //
////////////////////////////////////////////////////////////////////////////////


///
/// \copydoc PAMI_Topology_create
///
void PAMI_Topology_create(pami_topology_t *topo)
{
    PAMI_abort();
}

///
/// \copydoc PAMI_Topology_create_rect
///
void PAMI_Topology_create_rect(pami_topology_t *topo,
                              pami_coord_t *ll, pami_coord_t *ur, unsigned char *tl)
{
    PAMI_abort();
}

///
/// \copydoc PAMI_Topology_create_axial
///
void PAMI_Topology_create_axial(pami_topology_t *topo,
                               pami_coord_t *ll,
                               pami_coord_t *ur,
                               pami_coord_t *ref,
                               unsigned char *dir,
                               unsigned char *tl)
{
  PAMI_abort();
}

///
/// \copydoc PAMI_Topology_create_task
///
void PAMI_Topology_create_task(pami_topology_t *topo, pami_task_t rank)
{
    PAMI_abort();
}

///
/// \copydoc PAMI_Topology_create_range
///
void PAMI_Topology_create_range(pami_topology_t *topo, pami_task_t rank0, pami_task_t rankn)
{
  new(topo)PAMI::Topology(rank0,rankn);
}

///
/// \copydoc PAMI_Topology_create_list
///
void PAMI_Topology_create_list(pami_topology_t *topo, pami_task_t *ranks, size_t nranks)
{
  new(topo)PAMI::Topology(ranks,nranks);
}

///
/// \copydoc PAMI_Topology_destroy
///
void PAMI_Topology_destroy(pami_topology_t *topo)
{
    PAMI_abort();
}

///
/// \copydoc PAMI_Topology_size_of
///
unsigned PAMI_Topology_size_of(pami_topology_t *topo)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->size_of();
}

///
/// \copydoc PAMI_Topology_size
///
size_t PAMI_Topology_size(pami_topology_t *topo)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->size();
}

///
/// \copydoc PAMI_Topology_type
///
pami_topology_type_t pami_topology_type(pami_topology_t *topo)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->type();
}

///
/// \copydoc PAMI_Topology_index2TaskID
///
pami_task_t PAMI_Topology_index2TaskID(pami_topology_t *topo, size_t ix)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->index2Rank(ix);
}

///
/// \copydoc PAMI_Topology_taskID2Index
///
size_t PAMI_Topology_taskID2Index(pami_topology_t *topo, pami_task_t rank)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->rank2Index(rank);
}

///
/// \copydoc PAMI_Topology_taskRange
///
pami_result_t PAMI_Topology_taskRange(pami_topology_t *topo, pami_task_t *first, pami_task_t *last)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->rankRange(first,last);
}

///
/// \copydoc PAMI_Topology_taskList
///
pami_result_t PAMI_Topology_taskList(pami_topology_t *topo, pami_task_t **list)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->rankList(list);
}

///
/// \copydoc PAMI_Topology_rectSeg
///
pami_result_t PAMI_Topology_rectSeg(pami_topology_t *topo,
                                  pami_coord_t *ll, pami_coord_t *ur,
                                  unsigned char *tl)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->rectSeg(ll,ur,tl);
}

///
/// \copydoc PAMI_Topology_isLocalToMe
///
int PAMI_Topology_isLocalToMe(pami_topology_t *topo)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->isLocalToMe();
}

///
/// \copydoc PAMI_Topology_isRectSeg
///
int PAMI_Topology_isRectSeg(pami_topology_t *topo)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->isRectSeg();
}

///
/// \copydoc PAMI_Topology_getNthDims
///
void PAMI_Topology_getNthDims(pami_topology_t *topo, unsigned n,
                             unsigned *c0, unsigned *cn, unsigned char *tl)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    t->getNthDims(n,c0,cn,tl);
    return;
}

///
/// \copydoc PAMI_Topology_isTaskMember
///
int PAMI_Topology_isTaskMember(pami_topology_t *topo, pami_task_t rank)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->isRankMember(rank);
}

///
/// \copydoc PAMI_Topology_isCoordMember
///
int PAMI_Topology_isCoordMember(pami_topology_t *topo, pami_coord_t *c0)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->isCoordMember(c0);
}

///
/// \copydoc PAMI_Topology_sub_LocalToMe
///
void PAMI_Topology_sub_LocalToMe(pami_topology_t *_new, pami_topology_t *topo)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    t->subTopologyLocalToMe((PAMI::Topology *)_new);
}

///
/// \copydoc PAMI_Topology_sub_NthGlobal
///
void PAMI_Topology_sub_NthGlobal(pami_topology_t *_new, pami_topology_t *topo, int n)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    t->subTopologyNthGlobal((PAMI::Topology *)_new, n);
}

///
/// \copydoc PAMI_Topology_sub_ReduceDims
///
void PAMI_Topology_sub_ReduceDims(pami_topology_t *_new, pami_topology_t *topo, pami_coord_t *fmt)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    t->subTopologyReduceDims((PAMI::Topology *)_new, fmt);
}

///
/// \copydoc PAMI_Topology_getTaskList
///
void PAMI_Topology_getTaskList(pami_topology_t *topo, size_t max, pami_task_t *ranks, size_t *nranks)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    t->getRankList(max,ranks,nranks);
}

///
/// \copydoc PAMI_Topology_analyze
///
int  PAMI_Topology_analyze(pami_topology_t *topo)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->analyzeTopology();
}

///
/// \copydoc PAMI_Topology_convert
///
int  PAMI_Topology_convert(pami_topology_t *topo, pami_topology_type_t new_type)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->convertTopology(new_type);
}

///
/// \copydoc PAMI_Topology_intersect
///
void PAMI_Topology_intersect(pami_topology_t *_new, pami_topology_t *topo, pami_topology_t *other)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->intersectTopology((PAMI::Topology *)_new, (PAMI::Topology *)other);
}

///
/// \copydoc PAMI_Topology_subtract
///
void PAMI_Topology_subtract(pami_topology_t *_new, pami_topology_t *topo, pami_topology_t *other)
{
    PAMI::Topology * t = (PAMI::Topology *) topo;
    return t->subtractTopology((PAMI::Topology *)_new, (PAMI::Topology *)other);
}




///
/// \copydoc PAMI_Task2Network
///
pami_result_t PAMI_Task2Network(pami_task_t task,
                                           pami_coord_t *ntw)
{
#if defined(__pami_target_bgq__) || defined(__pami_target_bgp__)
  return __global.mapping.task2network(task, ntw, PAMI_N_TORUS_NETWORK);
#else
  PAMI_abort();
  return PAMI_ERROR;
#endif
}


///
/// \copydoc PAMI_Network2Task
///
pami_result_t PAMI_Network2Task(pami_coord_t ntw,
                                           pami_task_t *task)
{
  pami_network type;
  return __global.mapping.network2task(&ntw, task, &type);
}



#endif // __api_extension_c_multisend_pami_multisend_impl_h__
