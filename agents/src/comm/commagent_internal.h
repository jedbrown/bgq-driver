/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef  __COMM_AGENT_INTERNAL__
#define  __COMM_AGENT_INTERNAL__


/**
 * \file commagent_internal.h
 *
 * \brief C File containing Comm Agent Internal Declarations
 */


#include <spi/include/mu/RecFifo.h>
#include <spi/include/mu/InjFifo.h>
#include <spi/include/mu/Addressing.h>

__BEGIN_DECLS


/**
 * \brief Pointer to the Comm Agent's shared memory region.
 */
extern CommAgent_SharedMemoryMap_t *_agentShmPtr;


/**
 * \brief Reception Fifo Dispatch Function Array Element
 */
typedef struct CommAgent_RecFifoDispatchElement
{
  MUSPI_RecvFunction_t  fn;
  void                 *cookie;
} CommAgent_RecFifoDispatchElement_t;


/**
 * \brief Reception Fifo Dispatch Function Array
 *
 * This is an array of reception fifo dispatch function pointers and their
 * corresponding cookie.  The internal comm agent functions can register
 * their function pointers via CommAgent_RegisterDispatch().
 */
extern CommAgent_RecFifoDispatchElement_t _agentRecFifoDispatch[COMM_AGENT_MAX_DISPATCH_ID];


/**
 * \brief Reception Fifo Dispatch Function Registered Indicator
 *
 * Indicates whether any reception fifo dispatch functions have been
 * registered.
 */
extern uint32_t _agentRecFifoDispatchRegistered;


/**
 * \brief Register Dispatch Function
 */
__INLINE__
void commAgent_RegisterDispatch( CommAgent_RecFifoDispatchIds_t id,
				 MUSPI_RecvFunction_t fn,
				 void *cookie )
{
  _agentRecFifoDispatch[id].fn     = fn;
  _agentRecFifoDispatch[id].cookie = cookie;
  _agentRecFifoDispatchRegistered  = 1; /* Indicate that a registration has
					 * occurred.
					 */
}


/**
 * \brief Injection Fifo Pointer
 */
extern MUSPI_InjFifo_t *_ififo;

/**
 * \brief Base Address Table Subgroup
 *
 * This is the base address table subgroup for use by the comm agent.
 */
extern MUSPI_BaseAddressTableSubGroup_t _batSubgroup;


/**
 * \brief Base Address Table ID
 *
 * This is the base address table ID containing a zero base address for use
 * by the comm agent.  This is relative to the subgroup.
 */
extern uint8_t _batId;


/**
 * \brief Global Base Address Table ID
 *
 * This is the base address table ID containing a zero base address for use
 * by the comm agent.  This is a global BAT ID.
 */
extern uint16_t _globalBatId;


/**
 * \brief Sub Remote Get Size
 *
 * The number of bytes in a remote get pacing sub remote get.
 */
extern int _subRemoteGetSize;


/**
 * \brief Sub Remote Get Size Base
 *
 * The base value for the number of bytes in a remote get pacing
 * sub remote get.  When the percent of subregions that have active
 * messages is within or above a threshold range, this value is multiplied
 * by a factor to determine the actual subRemoteGetSize to use.
 */
extern int _subRemoteGetSizeBase;


/**
 * \brief The Threshold Range For Changing the Sub Remote Get Size.
 *
 * When the percent of subregions that have active messages is within or above
 * this percent range, set the _subRemoteGetSize to a multiplying factor
 * of the _subRemoteGetSizeBase.  Otherwise, use the _subRemoteGetSizeBase.
 * Threshold values are 0-101.
 */
extern int _subRemoteGetSizeThreshold1, _subRemoteGetSizeThreshold2, _subRemoteGetSizeThresholdRangeSize;


/**
 * \brief The Multiplying Factor Range For Changing the Sub Remote Get Size.
 *
 * When the percent of subregions that have active messages is within or above
 * a percent range, set the _subRemoteGetSize to a multiplying factor
 * of the _subRemoteGetSizeBase.  Otherwise, use the _subRemoteGetSizeBase.
 */
extern int _subRemoteGetSizeMultiplyingFactor1, _subRemoteGetSizeMultiplyingFactor2, _subRemoteGetSizeMultiplyingFactorRangeSize;


/**
 * \brief Max Bytes In Network
 *
 * The maximum number of remote get pacing bytes that can be in the network
 * from this node.  It should be a multiple of the _subRemoteGetSize.
 */
extern int _maxBytesInNetwork;


/**
 * \brief Max Bytes In Network Base
 *
 * The base value for the maximum number of bytes that can be in the network
 * from this node.  When the percent of subregions that have active
 * messages is within or above a threshold range, this value is multiplied
 * by a factor to determine the actual maxBytesInNetwork to use.
 */
extern int _maxBytesInNetworkBase;


/**
 * \brief The Threshold Range For Changing the Max Bytes In Network.
 *
 * When the percent of subregions that have active messages is within or above
 * this percent range, set the _maxBytesInNetwork to a multiplying factor
 * of the _maxBytesInNetworkBase.  Otherwise, use the _maxBytesInNetworkBase.
 * Threshold values are 0-101.
 */
extern int _maxBytesInNetworkThreshold1, _maxBytesInNetworkThreshold2, _maxBytesInNetworkThresholdRangeSize;


/**
 * \brief The Multiplying Factor Range For Changing the Max Bytes In Network.
 *
 * When the percent of subregions that have active messages is within or above
 * a percent range, set the _maxBytesInNetwork to a multiplying factor
 * of the _maxBytesInNetworkBase.  Otherwise, use the _maxBytesInNetworkBase.
 */
extern int _maxBytesInNetworkMultiplyingFactor1, _maxBytesInNetworkMultiplyingFactor2, _maxBytesInNetworkMultiplyingFactorRangeSize;


/**
 * \brief Number of Remote Get Sub Message Counters
 *
 * The number of remote get pacing sub-message reception counters.
 */
extern int _numSubMessageCounters;


/**
 * \brief Number of Remote Get Counters
 *
 * The number of remote get counters, including sub-message counters used for
 * pacing and other counters used for non-paced remote gets.
 */
extern int _numCounters;


/**
 * \brief Whether to Pace Remote Gets
 */
extern int _paceRgets;


/**
 * \brief The Zone to use for Random Traffic
 *
 * Dynamic 0, 1, 2, 3.  Default is 0.
 */
extern int _randomZone;


/**
 * \brief The Threshold For When To Use The Random Zone.
 *
 * When equal to or more than this percent of subregions have active messages,
 * use the random zone.  Otherwise, use the zone provided by the caller.
 * 0-101.  101 means never use the random zone.  Default 101.
 */
extern int _randomThreshold;


/**
 * \brief Indicator of Whether or Not to do SubRegion Calculations
 *
 * SubRegion calculations track where messages are in the network.  This is used
 * to dynamically control routing zones, sub-message size, and max bytes in the
 * network.  These controls only occur when thresholds are exceeded.
 *
 * This flag indicates whether the percentages are set such that subRegion
 * calculations are needed or not.
 */
extern int _doSubRegionCalculations;


__END_DECLS


#endif
