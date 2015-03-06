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
///
/// \file common/MultisendInterface.h
/// \brief PAMI multisend implementation.
///

#ifndef __common_MultisendInterface_h__
#define __common_MultisendInterface_h__

#include "common/type/TypeCode.h"
#include "common/type/TypeFunc.h"

typedef struct
{
  pami_event_function   function;
  void                * clientdata;
} pami_callback_t;

typedef struct pami_quad_t
{
  unsigned w0; /**< Word[0] */
  unsigned w1; /**< Word[1] */
  unsigned w2; /**< Word[2] */
  unsigned w3; /**< Word[3] */
}
pami_quad_t __attribute__ ((__aligned__ (16)));

typedef pami_quad_t pami_pipeworkqueue_t[8];
typedef pami_quad_t pami_pipeworkqueue_ext_t[2];

/** \brief The various types a Topology can be */
typedef enum
{
  PAMI_EMPTY_TOPOLOGY = 0, /**< topology represents no (zero) tasks    */
  PAMI_SINGLE_TOPOLOGY,    /**< topology is for one task               */
  PAMI_RANGE_TOPOLOGY,     /**< topology is a simple range of tasks    */
  PAMI_LIST_TOPOLOGY,      /**< topology is an unordered list of tasks */
  PAMI_COORD_TOPOLOGY,     /**< topology is a rectangular segment
                               represented by coordinates               */
  PAMI_AXIAL_TOPOLOGY,     /**< topology is a axial neighborhood --
                               represented by a rectangular seqment, a
                               reference task, and optional torus flags */
  PAMI_EPLIST_TOPOLOGY,    /**< topology is an unordered list of endpoints */
  PAMI_TOPOLOGY_COUNT
} pami_topology_type_t;

typedef pami_quad_t pami_topology_t[16];


/**
 * \brief Network type
 */
typedef enum
{
  PAMI_DEFAULT_NETWORK = 0, /**< Default network type. \b Guaranteed to work. */
  PAMI_N_TORUS_NETWORK,     /**< nD-Torus / nD-SMP network type.
                              * mapping->torusDims() for torus dim,
                              * mapping->globalDims() for all (torus+SMP) dim.
                              */
  PAMI_SOCKET_NETWORK,      /**< Unix socket network type. */
  PAMI_SHMEM_NETWORK,       /**< local shared memory "network" for smp nodes. */
  PAMI_NETWORK_COUNT        /**< Number of network types defined. */
}
pami_network;


/** \todo Remove this platform-specific #define */
#define PAMI_MAX_DIMS 7
/* #define PAMI_MAX_DIMS  4 */

/**
 * \brief A structure to describe a network coordinate
 */
typedef struct
{
  pami_network network; /**< Network type for the coordinates */
  union
  {
    int no_network;
    struct
    {
      size_t coords[PAMI_MAX_DIMS];
    } n_torus;
    struct
    {
      int recv_fd;   /**< Receive file descriptor */
      int send_fd;   /**< Send file descriptor    */
    } socket;   /**< PAMI_SOCKET_NETWORK coordinates */
    struct
    {
      size_t rank;   /**< Global task id of process */
      size_t peer;   /**< Local task id of process */
    } shmem;    /**< PAMI_SHMEM_NETWORK coordinates */
  } u;
} pami_coord_t;


extern int pami_dt_shift[PAMI_DT_COUNT]; /// \todo what is this and is it really an extension?


/**  Deprecated Multicast:  To be deleted soon!!! */
/**********************************************************************/
typedef enum
{
  PAMI_UNDEFINED_CONSISTENCY = -1,
  PAMI_RELAXED_CONSISTENCY,
  PAMI_MATCH_CONSISTENCY,
  PAMI_WEAK_CONSISTENCY,
  PAMI_CONSISTENCY_COUNT
} pami_consistency_t;

/**********************************************************************/


#endif // __common_MultisendInterface_h__
