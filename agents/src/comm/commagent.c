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


/**
 * \file commagent.c
 *
 * \brief C File containing Comm Agent Implementation
 */


#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <hwi/include/bqc/MU_Addressing.h>
#include <hwi/include/bqc/MU_Fifo.h>
#include <hwi/include/bqc/MU_Macros.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <spi/include/mu/Addressing.h>
#include <spi/include/mu/Addressing_inlines.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/kernel/process.h>
#include <spi/include/l2/atomic.h>
#include <spi/include/mu/Descriptor.h>
#include <spi/include/kernel/MU.h>
#include <spi/include/wu/wait.h>
#include <firmware/include/personality.h>
#include <agents/include/comm/commagent.h>
#include "rgetpacing_internal.h"
#include "fence_internal.h"
#include "commagent_internal.h"


#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x

#ifdef TRACE2
#undef TRACE2
#endif
#define TRACE2(x) //fprintf x


/**
 * \brief Reception Fifo Default Size In Bytes
 */
#define COMM_AGENT_REC_FIFO_SIZE (65536)


/**
 * \brief Comm Agent Reception Fifo State Values
 */
typedef enum
  {
    COMM_AGENT_REC_FIFO_STATE_UNINITIALIZED,
    COMM_AGENT_REC_FIFO_STATE_INITIALIZED
  } CommAgent_RecFifo_State_t;


/**
 * \brief Comm Agent Injection Fifo State Values
 */
typedef enum
  {
    COMM_AGENT_INJ_FIFO_STATE_UNINITIALIZED,
    COMM_AGENT_INJ_FIFO_STATE_INITIALIZED
  } CommAgent_InjFifo_State_t;


/**
 * \brief Injection Fifo Default Size In Bytes
 */
#define COMM_AGENT_INJ_FIFO_SIZE (65536)


/**
 * \brief Global variables
 *
 */
CommAgent_SharedMemoryMap_t *_agentShmPtr; /**< Agent's
					    * shared memory pointer.
					    */
static volatile uint64_t *_producerL2Counter[4]; /**< L2 atomic pointers to the
						  *   producerL2counter, one
						  *   for each thread.  Set up
						  *   for "load" access.
						  */
static volatile uint64_t *_boundL2Counter[4]; /**< L2 atomic pointers to the
					       *   boundL2counter, one
					       *   for each thread.  Set up for
					       *   store-add access.
					       */
static uint64_t _headCounter=0; /**< Agent's counter representing the next item
				 *   in the request queue to be processed.
				 */
/* int32_t's */
static int _agentShmFd; /**< Agent's shared memory file descriptor. */
static int _requestQueueSize = COMM_AGENT_REQUEST_QUEUE_SIZE;

static MUSPI_RecFifoSubGroup_t     _rfifo_subgroup;
static MUSPI_RecFifo_t            *_rfifo;
static char                       *_rfifoPtr;
static int                         _recFifoSize  = COMM_AGENT_REC_FIFO_SIZE;
static CommAgent_RecFifo_State_t   _recFifoState = COMM_AGENT_REC_FIFO_STATE_UNINITIALIZED;
CommAgent_RecFifoDispatchElement_t _agentRecFifoDispatch[COMM_AGENT_MAX_DISPATCH_ID];
uint32_t                           _agentRecFifoDispatchRegistered = 0;

static int                         _injFifoSize = COMM_AGENT_INJ_FIFO_SIZE;
static MUSPI_InjFifoSubGroup_t     _ififo_subgroup;
MUSPI_InjFifo_t                   *_ififo;
static char                       *_ififoPtr;
static uint32_t                    _injFifoId;
static CommAgent_InjFifo_State_t   _injFifoState = COMM_AGENT_INJ_FIFO_STATE_UNINITIALIZED;

MUSPI_BaseAddressTableSubGroup_t _batSubgroup;
uint8_t _batId;
uint16_t _globalBatId;

int _subRemoteGetSize;
int _maxBytesInNetwork;
int _numSubMessageCounters;
int _numCounters;
int _paceRgets;
int _useWakeup;

/* There are entries in the following arrays for various block sizes as follows:
 * [0]:       Racks <  2
 * [1]:  2 <= Racks <  4
 * [2]:  4 <= Racks <  8
 * [3]:  8 <= Racks < 16
 * [4]: 16 <= Racks < 32
 * [5]: 32 <= Racks < 48
 * [6]: 48 <= Racks < 64
 * [7]: 64 <= Racks < 80
 * [8]: 80 <= Racks < 96
 * [9]: 96 <= Racks     
 *
 * NOTE:  If you change the values in these arrays, please update 
 *        agents/doc/comm/README
 */
static int _subRemoteGetSizeDefaults[10]=  { 16384, /* Less than 2 racks */
                                             16384, /* Less than 4 racks */
                                             8192,  /* Less than 8 racks */
                                             8192,  /* Less than 16 racks */
                                             8192,  /* Less than 32 racks */
                                             8192,  /* Less than 48 racks */
                                             8192,  /* Less than 64 racks */
                                             8192,  /* Less than 80 racks */
                                             8192,  /* Less than 96 racks */
                                             8192 };/* 96 racks and above */
static int _maxBytesInNetworkDefaults[10]= { 65536,  /* Less than 2 racks */
                                             65536,  /* Less than 4 racks */
                                             32768,  /* Less than 8 racks */
                                             24576,  /* Less than 16 racks */
                                             24576,  /* Less than 32 racks */
                                             24576,  /* Less than 48 racks */
                                             24576,  /* Less than 64 racks */
                                             24576,  /* Less than 80 racks */
                                             24576,  /* Less than 96 racks */
                                             24576 };/* 96 racks and above */

uint64_t _WACrangePA;
uint64_t _WACrangeMask;

uint64_t           _interruptsToClear = 0;
volatile uint64_t *_interruptStatusPtr;


/**
 * \brief Re-Arm Wakeup Unit
 */
static
void reArmWU ()
{
  TRACE((stderr,"Arming WAC with PA=0x%lx, mask=0x%lx\n",_WACrangePA,_WACrangeMask));
  WU_ArmWithAddress( _WACrangePA, _WACrangeMask );  
}


/**
 * \brief Wakeup Wait
 */
static
void WUwait ()
{
  TRACE2((stderr,"%s() [%s:%d]: Starting Wakeup Wait...\n",__FUNCTION__,__FILE__,__LINE__));

  if ( _useWakeup ) 
    {
      // fprintf(stderr,"Entering wakeup wait at %lu\n",GetTimeBase());
      ppc_waitimpl();
    }
}


/**
 * \brief Extract Environment Variable Integer
 * 
 * \param[in]     env   Pointer to the environment variable string from getenv().
 *                      May be NULL if this environment variable did not exist.
 * \param[in,out] dval  Pointer to the value of the environment variable.
 *                      On input, this is the default value, in case env is 
 *                      NULL.  On output, this is the value specified by the 
 *                      env string (when it is not NULL), or it remains unchanged
 *                      when env is NULL.
 *
 * \retval  value  The value of the environment variable.
 */
static inline 
int ENV_Int( char * env, int * dval )
{
  int result;
  if(env != NULL)
  {
    /* Convert value to integer */
    result = atoi(env);
  }
  else
    result = *dval;
  return *dval = result;
}


/**
 * \brief Set Default Configuration
 *
 * Set the default values for configuration variables.
 */
static
int setDefaultConfig()
{
  Personality_t p;
  int rc;
  size_t systemSize;
  size_t index;
  
  rc = Kernel_GetPersonality ( &p, sizeof(Personality_t) );
  if ( rc ) 
  {
    fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to get the personality, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,rc);
    return rc;
  }

  systemSize = 
    p.Network_Config.Anodes *
    p.Network_Config.Bnodes *
    p.Network_Config.Cnodes *
    p.Network_Config.Dnodes *
    p.Network_Config.Enodes;
  
  index = 9; /* Set for largest system size */
  if ( systemSize < 96*1024 ) index = 8;
  if ( systemSize < 80*1024 ) index = 7;
  if ( systemSize < 64*1024 ) index = 6;
  if ( systemSize < 48*1024 ) index = 5;
  if ( systemSize < 32*1024 ) index = 4;
  if ( systemSize < 16*1024 ) index = 3;
  if ( systemSize <  8*1024 ) index = 2;
  if ( systemSize <  4*1024 ) index = 1;
  if ( systemSize <  2*1024 ) index = 0;

  /* Use the index to default the sub message size, max, and message size for pacing */
  _subRemoteGetSize  = _subRemoteGetSizeDefaults[index];
  _maxBytesInNetwork = _maxBytesInNetworkDefaults[index];

  TRACE((stderr,"%s() [%s:%d]: SystemSize=%zu, index=%zu, Default _subRemoteGetSize=%d, _maxBytesInNetwork=%d\n",__FUNCTION__,__FILE__,__LINE__, systemSize, index, _subRemoteGetSize, _maxBytesInNetwork));

  _requestQueueSize = COMM_AGENT_REQUEST_QUEUE_SIZE;
  _injFifoSize = COMM_AGENT_INJ_FIFO_SIZE;
  _recFifoSize = COMM_AGENT_REC_FIFO_SIZE;
  _paceRgets = 1;
  _numCounters = COMM_AGENT_REMOTE_GET_PACING_NUM_COUNTERS;
  _useWakeup = 1;

  return 0;
}


/**
 * \brief Get Environment Variables
 *
 * The documentation for the env vars is in rgetpacing.h.
 */
static
void getEnvVars ()
{

  /* Override _requestQueueSize */
  ENV_Int( getenv("COMMAGENT_REQUESTQUEUESIZE"), &_requestQueueSize );
  if ( _requestQueueSize < 1 )
    _requestQueueSize = 1;

  TRACE((stderr,"%s() [%s:%d]: Request queue size is %d entries\n",__FUNCTION__,__FILE__,__LINE__,_requestQueueSize));

  /* Override _injFifoSize */
  ENV_Int( getenv("MUSPI_INJFIFOSIZE"), &_injFifoSize );
  if ( _injFifoSize < BGQ_MU_MIN_INJFIFO_SIZE_IN_BYTES )
    _injFifoSize = BGQ_MU_MIN_INJFIFO_SIZE_IN_BYTES;
  _injFifoSize = (_injFifoSize + 63) & ~(63); /* Round to 64B boundary */

  TRACE((stderr,"%s() [%s:%d]: InjFifo size is %d\n",__FUNCTION__,__FILE__,__LINE__,_injFifoSize));

  /* Override _recFifoSize
   * We use the COMMAGENT_RECFIFOSIZE as opposed to MUSPI_RECFIFOSIZE because
   * the MUSPI_RECFIFOSIZE default is too large for the comm agent, and the
   * comm agent doesn't use the rec fifo threshold interrupt, so the comm agent's
   * rec fifo size can be different from the other users.
   */
  ENV_Int( getenv("COMMAGENT_RECFIFOSIZE"), &_recFifoSize );
  if ( _recFifoSize < BGQ_MU_MIN_RECFIFO_SIZE_IN_BYTES )
    _recFifoSize = BGQ_MU_MIN_RECFIFO_SIZE_IN_BYTES;
  _recFifoSize = (_recFifoSize + 31) & ~(31); /* Round to 32B boundary */

  TRACE((stderr,"%s() [%s:%d]: RecFifo size is %d\n",__FUNCTION__,__FILE__,__LINE__,_recFifoSize));

  /* Override _subRemoteGetSize */
  ENV_Int( getenv("COMMAGENT_RGETPACINGSUBSIZE"), &_subRemoteGetSize );
  if ( _subRemoteGetSize < 1 )
    _subRemoteGetSize = 1;

  TRACE((stderr,"%s() [%s:%d]: Sub-remote-get-size is %d\n",__FUNCTION__,__FILE__,__LINE__,_subRemoteGetSize));

  /* Override _maxBytesInNetwork */
  ENV_Int( getenv("COMMAGENT_RGETPACINGMAX"), &_maxBytesInNetwork );
  if ( _maxBytesInNetwork < 1 )
    _maxBytesInNetwork = 1;
  if ( _maxBytesInNetwork < _subRemoteGetSize ) _maxBytesInNetwork = _subRemoteGetSize;

  TRACE((stderr,"%s() [%s:%d]: Max bytes in network is %d\n",__FUNCTION__,__FILE__,__LINE__,_maxBytesInNetwork));

  /* Override _paceRgets */
  ENV_Int( getenv("COMMAGENT_RGETPACING"), &_paceRgets );

  TRACE((stderr,"%s() [%s:%d]: Pace remote gets is %d\n",__FUNCTION__,__FILE__,__LINE__,_paceRgets));

  /* Calculate the number of sub-message reception counters used for pacing. */
  _numSubMessageCounters = _maxBytesInNetwork / _subRemoteGetSize;

  TRACE((stderr,"%s() [%s:%d]: Num sub-message counters is %d\n",__FUNCTION__,__FILE__,__LINE__,_numSubMessageCounters));

  /* Override _numCounters */
  ENV_Int( getenv("COMMAGENT_NUMRGETS"), &_numCounters );
  if ( _numCounters < _numSubMessageCounters )
    _numCounters = _numSubMessageCounters;

  TRACE((stderr,"%s() [%s:%d]: Number of rgets (counters) is %d\n",__FUNCTION__,__FILE__,__LINE__,_numCounters));

  /* Override _useWakeup */
  ENV_Int( getenv("COMMAGENT_WAKEUP"), &_useWakeup );

  TRACE((stderr,"%s() [%s:%d]: Use Wakeup Unit is %d\n",__FUNCTION__,__FILE__,__LINE__,_useWakeup));
}


/**
 * \brief Setup Injection Fifos
 */
static
int setupInjFifos ()
{
  int rc = 0;

  /* Set up an injection fifo to receive packets.
   * - Allocate storage for an injection fifo
   * - Search subgroups 64 and 65 for a free inj fifo ID
   * - Allocate and initialize that injection fifo.
   * - Set the injection fifo threshold to be 95% of the injection fifo size.
   *   This will cause an interrupt to fire whenever the fifo head or tail is
   *   changed and the free space exceeds that threshold.  We want that so
   *   when the injection fifo fills, and we queue descriptors, we will be
   *   awakened when there is sufficient space in the fifo so we can
   *   inject those descriptors.
   * - Turn on injection fifo threshold interrupt for that injection fifo.
   * - Activate that injection fifo.
   */
  rc = posix_memalign( (void**)&_ififoPtr, 64, _injFifoSize );
  if ( rc == 0 )
    {
      uint32_t subgroup, numFreeFifos, freeFifoIds[BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP];
      for ( subgroup=64; subgroup<=65; subgroup++ )
	{
	  numFreeFifos = 0;
	  rc = Kernel_QueryInjFifos( subgroup,
				     &numFreeFifos,
				     freeFifoIds );
	  if ( numFreeFifos > 0 ) break;
	}
      if ( numFreeFifos > 0 )
	{
	  /* Set user fifo attribute. */
	  Kernel_InjFifoAttributes_t injFifoAttrs[1];
	  injFifoAttrs[0].RemoteGet = 0;
	  injFifoAttrs[0].System    = 0;
	  injFifoAttrs[0].Priority  = 0;

	  rc = Kernel_AllocateInjFifos (subgroup,
					&_ififo_subgroup,
					1,
					freeFifoIds,
					injFifoAttrs);
	  if ( rc == 0 )
	    {
	      Kernel_InjFifoInterrupts_t injFifoInterrupts[1];
	      injFifoInterrupts[0].Threshold_Crossing = 1;
	      
	      Kernel_MemoryRegion_t  mregion;
	      Kernel_CreateMemoryRegion ( &mregion,
					  _ififoPtr,
					  _injFifoSize );
	      
	      rc = Kernel_InjFifoInit( &_ififo_subgroup, 
				       freeFifoIds[0], 
				       &mregion, 
				       (uint64_t)_ififoPtr -
				       (uint64_t)mregion.BaseVa,
				       _injFifoSize-1 );
	      if ( rc == 0 )
		{
		  rc = Kernel_ConfigureInjFifoInterrupts (&_ififo_subgroup,
							  1,
							  freeFifoIds,
							  injFifoInterrupts);
		  if ( rc == 0 )
		    {
		      uint64_t normalThreshold;
		      rc = Kernel_GetInjFifoThresholds( &normalThreshold,
							(uint64_t *)NULL );
		      if ( rc == 0 )
			{
			  if ( normalThreshold == 0 ) // Not set yet?
			    {
			      normalThreshold = _injFifoSize * 0.95 / sizeof(MUHWI_Descriptor_t);
			      rc = Kernel_ConfigureInjFifoThresholds( &normalThreshold,
								      (uint64_t *)NULL );
			      TRACE((stderr,"%s() [%s:%d]: InjFifo Threshold was not set.  Setting it to %lu\n",__FUNCTION__,__FILE__,__LINE__,normalThreshold));
			    }
			  else
			    TRACE((stderr,"%s() [%s:%d]: InjFifo Threshold was previously set to %lu\n",__FUNCTION__,__FILE__,__LINE__,normalThreshold));

			  if ( rc == 0 )
			    {
			      rc = Kernel_InjFifoActivate ( &_ififo_subgroup,
							    1,
							    freeFifoIds,
							    KERNEL_INJ_FIFO_ACTIVATE );
			      if ( rc == 0 )
				{
				  /* Save this interrupt in the clear mask */
				  _interruptsToClear |= 
				    _BN(((subgroup-64)*BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP) + 
					freeFifoIds[0]);			
				    /* Clear inj fifo interrupt status. */
				  _ififo_subgroup.groupStatus->clearInterruptStatus = 
				    _BN(((subgroup-64)*BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP) + 
					freeFifoIds[0]);
				  
				  /* Save the inj fifo ID and pointer to inj fifo.
				   * Indicate that the "inj fifo" functionality is initialized.
				   */
				  _injFifoId = freeFifoIds[0];
				  
				  _ififo = &_ififo_subgroup._injfifos[freeFifoIds[0]];
				  _injFifoState = COMM_AGENT_INJ_FIFO_STATE_INITIALIZED;
				  TRACE((stderr,"%s() [%s:%d]: InjFifo subgroup=%u, fifo ID=%u, InterruptsToClear=0x%lx\n",__FUNCTION__,__FILE__,__LINE__,subgroup,_injFifoId,_interruptsToClear));
				}
			      else
				{
				  fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to activate injection fifo, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
				}
			    }
			  else
			    {
			      fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to configure injection fifo threshold, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
			    }
			}
		      else
			{
			  fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to get injection fifo threshold, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
			}
		    }
		  else
		    {
			  fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to configure injection fifo interrupts, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
		    }
		}
	      else
		{
		  fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to initialize injection fifo, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
		}
	    }
	  else
	    {
	      fprintf(stderr,"%s() [%s:%d]: CommAgent: Injection fifo allocation failure, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
	    }
        }
      else
	{
	  rc = ENOMEM;
	  fprintf(stderr,"%s() [%s:%d]: CommAgent: Not enough available injection fifos.\n",__FUNCTION__,__FILE__,__LINE__);
	}
    }
  else
    {
      rc = ENOMEM;
      fprintf(stderr,"%s() [%s:%d]: CommAgent: Not enough memory to allocate injection fifo.\n",__FUNCTION__,__FILE__,__LINE__);
    }
  return rc;
}


/**
 * \brief Setup Reception Fifos
 */
static
int setupRecFifos ()
{
  int rc = 0;

  /* Set up a reception fifo to receive packets.
   * - Allocate storage for a reception fifo
   * - Search subgroups 64 and 65 for a free rec fifo ID
   * - Allocate and initialize that reception fifo.
   * - Turn on interrupt on packet arrival for that reception fifo.
   * - Enable that reception fifo.
   */
  rc = posix_memalign( (void**)&_rfifoPtr, 32, _recFifoSize );
  if ( rc == 0 )
    {
      uint32_t subgroup, numFreeFifos, freeFifoIds[BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP];
      for ( subgroup=64; subgroup<=65; subgroup++ )
	{
	  numFreeFifos = 0;
	  rc = Kernel_QueryRecFifos( subgroup,
				     &numFreeFifos,
				     freeFifoIds );
	  if ( numFreeFifos > 0 ) break;
	}
      if ( numFreeFifos > 0 )
	{
	  /* Set user fifo attribute. */
	  Kernel_RecFifoAttributes_t recFifoAttrs[1];
	  recFifoAttrs[0].System = 0;

	  rc = Kernel_AllocateRecFifos (subgroup,
					&_rfifo_subgroup,
					1,
					freeFifoIds,
					recFifoAttrs);
	  if ( rc == 0 )
	    {
	      uint64_t recFifoEnableBits;

	      Kernel_RecFifoInterrupts_t recFifoInterrupts[1];
	      recFifoInterrupts[0].Threshold_Crossing = 0;
	      recFifoInterrupts[0].Packet_Arrival     = 1;
	      
	      Kernel_MemoryRegion_t  mregion;
	      Kernel_CreateMemoryRegion ( &mregion,
					  _rfifoPtr,
					  _recFifoSize );
	      
	      rc = Kernel_RecFifoInit( &_rfifo_subgroup, 
				       freeFifoIds[0], 
				       &mregion, 
				       (uint64_t)_rfifoPtr -
				       (uint64_t)mregion.BaseVa,
				       _recFifoSize-1 );
	      if ( rc == 0 )
		{
		  recFifoEnableBits = ( 0x0000000000000001ULL << 
					( 15 - 
					  ( ( (subgroup-64)*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) + 
					    freeFifoIds[0] ) ) );
		  
		  rc = Kernel_ConfigureRecFifoInterrupts (&_rfifo_subgroup,
							  1,
							  freeFifoIds,
							  recFifoInterrupts);
		  if ( rc == 0 )
		    {
		      rc = Kernel_RecFifoEnable ( 16, /* Group ID */ 
						  recFifoEnableBits );
		      if ( rc == 0 )
			{
			  /* Save this interrupt in the clear mask */
			  _interruptsToClear |=  _BN(48 + 
				((subgroup-64)*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) + 
				freeFifoIds[0]);

			  /* Clear rec fifo interrupt status. */
			  _rfifo_subgroup.groupStatus->clearInterruptStatus = 
			    _BN(48 + 
				((subgroup-64)*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) + 
				freeFifoIds[0]);
			      
			  /* Save the rec fifo ID and pointer to rec fifo.
			   * Indicate that the "rec fifo" functionality is initialized.
			   */
			  _agentShmPtr->globalRecFifoId = 
			    (subgroup * BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) + 
			    freeFifoIds[0];
			  
			  _rfifo = &_rfifo_subgroup._recfifos[freeFifoIds[0]];
			  
			  _recFifoState = COMM_AGENT_REC_FIFO_STATE_INITIALIZED;
			  TRACE((stderr,"%s() [%s:%d]: RecFifo subgroup=%u, fifo ID=%u, InterruptsToClear=0x%lx\n",__FUNCTION__,__FILE__,__LINE__,subgroup,_agentShmPtr->globalRecFifoId,_interruptsToClear));
			}
		      else
			{
			  fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to configure reception fifo interrupts, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
			}
		    }
		  else
		    {
		      fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to enable reception fifo, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
		    }
		}
	      else
		{
		  fprintf(stderr,"%s() [%s:%d]: CommAgent: Failure to initialize reception fifo, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
		}
	    }
	  else
	    {
	      fprintf(stderr,"%s() [%s:%d]: CommAgent: Reception fifo allocation failure, subgroup=%u, fifo=%u, rc=%d.\n",__FUNCTION__,__FILE__,__LINE__,subgroup,freeFifoIds[0],rc);
	    }
        }
      else
	{
	  rc = ENOMEM;
	  fprintf(stderr,"%s() [%s:%d]: CommAgent: Not enough available reception fifos.\n",__FUNCTION__,__FILE__,__LINE__);
	}
    }
  else
    {
      rc = ENOMEM;
      fprintf(stderr,"%s() [%s:%d]: CommAgent: Not enough memory to allocate reception fifo.\n",__FUNCTION__,__FILE__,__LINE__);
    }
  return rc;
}


/**
 * \brief Setup Base Address Table
 *
 * Allocate a base address table slot and initialize it to zero for use by
 * the comm agent.
 *
 * \return  _batSubgroup  Initialized subgroup.
 * \return  _batId        BAT slot Id.
 */
static
int setupBaseAddressTable ()
{
  uint32_t subgroup;
  int rc;
  uint32_t numFree=0;
  uint32_t freeIds[BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP];

  /* Set up a base address table slot set to zeros that can be used by
   * the comm agent.
   * - Search subgroups 64 and 65 for a free base address table ID
   * - Allocate and initialize that base address table ID.
   */
  for ( subgroup=64; subgroup<=65; subgroup++ )
    {
      /* Determine the free BAT Ids in this subgroup. */
      rc = Kernel_QueryBaseAddressTable( subgroup,
					 &numFree,
					 freeIds );
      if ( numFree > 0 )
	{
	  break; /* Nothing free in this subgroup?  Go to next. */
	}
    }
  
  if ( numFree > 0 )
    {
      /* Allocate the BAT Ids */
      rc = Kernel_AllocateBaseAddressTable( subgroup,
					    &_batSubgroup,
					    1,
					    &freeIds[0],
					    0 /* "User" access */);
      if ( rc == 0 )
	{
	  _batId = freeIds[0]; /* Remember this ID. */
	  _globalBatId = ( subgroup * BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP ) + _batId;
	  
	  MUHWI_BaseAddress_t baseAddress;
	  baseAddress = 0;
	  
	  rc = MUSPI_SetBaseAddress ( &_batSubgroup,
				      _batId,
				      baseAddress );
	  if ( rc == 0 )
	    {
	      TRACE((stderr,"%s() [%s:%d]: Bat subgroup=%u, Bat ID=%u, Global Bat ID=%u\n",__FUNCTION__,__FILE__,__LINE__,subgroup,_batId, _globalBatId));
	    }
	  else
	    {
	      fprintf(stderr,"%s() [%s:%d]: CommAgent: Cannot run.  Setting base address table entry failed with rc=%d\n",__FUNCTION__,__FILE__,__LINE__,rc);
	    }
	}
      else
	{
	  fprintf(stderr,"%s() [%s:%d]: CommAgent: Cannot run.  Allocating base address table failed with rc=%d\n",__FUNCTION__,__FILE__,__LINE__,rc);
	}	  
    }
  else
    {
      if ( rc == 0 )
	{
	  fprintf(stderr,"%s() [%s:%d]: CommAgent: Cannot run.  No free Base Address Table Ids.\n",__FUNCTION__,__FILE__,__LINE__);
	  rc = ENOENT;
	}
      else
	{
	  fprintf(stderr,"%s() [%s:%d]: CommAgent: Cannot run.  Query Base Address Table failed with rc=%d\n",__FUNCTION__,__FILE__,__LINE__,rc);
	}
    }
  return rc;
}
/**
 * \brief Setup the Agent's Shared Memory
 */
static
int setupAgentSharedMemory ()
{
  int rc;

  /* Create/Open the shared memory file and map it. */
  rc = CommAgent_SetupAgentSharedMemory( &_agentShmPtr,
					 &_agentShmFd );
  if (rc)
    {
      fprintf(stderr,"%s() [%s:%d]: SetupAgentSharedMemory returned errno %d\n",__FUNCTION__,__FILE__,__LINE__,rc);
      return rc;
    }

  /* Set pointer to the request queue.
   *
   * The queue is immediately after the CommAgent_SharedMemoryMap_t
   * structure, starting on a 32B boundary.
   */
  uint64_t queueAddress = (uint64_t)(_agentShmPtr + 1);
  queueAddress = (queueAddress + 31) & ~(31); // Round it
  _agentShmPtr->queue = (CommAgent_WorkRequest_t *)queueAddress;

  /*
   * In shared memory, after the queue, is the WAC range,
   * consisting of the queue header and reception counters.
   * The WAC range must start on a power of 2 boundary and
   * must be that same power of 2 size.  It also must be 64B or larger.
   */
  /* 1. WAC size is the size of the queue header, rounded to an 8-byte boundary,
   *    plus the reception counters size.
   */
  uint64_t sizeOfQueueHeaderRounded = ( sizeof(CommAgent_QueueHeader_t) + 7 ) & ~(7);
  uint64_t actualWACsize = sizeOfQueueHeaderRounded + 
    ( _numCounters * sizeof(uint64_t) );

  /* 2. Make the WACsize a power of 2, as required by the wakeup unit */
  uint64_t tmp = 1;  
  while ( tmp < actualWACsize ) {    tmp <<= 1;  }
  uint64_t wacSize = tmp;

  /* 3. Calculate the queue header address.  The queue header is the first
  *     thing in the WAC range.  So, it must be on the same power of 2 
  *     boundary as the WAC range size. 
  */
  _agentShmPtr->queueHeader = 
    (CommAgent_QueueHeader_t *)
    ( ( (uint64_t)queueAddress +
	( _requestQueueSize * sizeof(CommAgent_WorkRequest_t) ) +
	(wacSize-1) )
      & ~(wacSize-1) );
  
  /* 4. Obtain the physical address of the WAC range, and remember the bits that the
   *    WU uses as a mask against the addresses being stored-to so it knows when
   *    to wakeup.
   */
  Kernel_MemoryRegion_t  mregion;
  Kernel_CreateMemoryRegion ( &mregion,
			      _agentShmPtr->queueHeader,
			      wacSize );
  _WACrangePA = 
    (uint64_t)_agentShmPtr->queueHeader -
    (uint64_t)mregion.BaseVa +
    (uint64_t)mregion.BasePa;

  _WACrangeMask = ~(wacSize-1);

  /* 5. Calculate the rec counter address to be after the queue header,
   *    rounded to an 8 byte boundary.
   */
  _agentShmPtr->receptionCounters =
    (volatile uint64_t *)
    ( (uint64_t)_agentShmPtr->queueHeader +
      sizeOfQueueHeaderRounded );

  TRACE((stderr,"%s() [%s:%d]: sizeof(QueueHeader)=%lu, sizeOfQueueHeaderRounded=%lu, actualWACsize=%lu, wacSize=%lu, _WACrangePA=%lx, _WACrangeMask=%lx, _agentShmPtr=%p, queue=%p, queueHeader=%p, recCounterPtr=%p\n",__FUNCTION__,__FILE__,__LINE__,sizeof(CommAgent_QueueHeader_t),sizeOfQueueHeaderRounded,actualWACsize,wacSize,_WACrangePA,_WACrangeMask,_agentShmPtr,_agentShmPtr->queue,_agentShmPtr->queueHeader,_agentShmPtr->receptionCounters));

  /* Set up pointers to the request queue's L2 atomics and initialize them.
   * - Only the bound counter needs to be initialized...everything else
   *   in the queue is zero.
   * - The producer counter needs to be simply read (loaded) by the agent.
   * - The bound counter needs to be incremented by 1 (store-add) by the agent.
   */
  _agentShmPtr->queueHeader->boundL2counter = _requestQueueSize;

  CommAgent_BuildL2AtomicAddresses ( &_agentShmPtr->queueHeader->producerL2counter,
				     MUHWI_ATOMIC_OPCODE_LOAD,
				     &_producerL2Counter[0] );

  CommAgent_BuildL2AtomicAddresses ( &_agentShmPtr->queueHeader->boundL2counter,
				     MUHWI_ATOMIC_OPCODE_STORE_ADD,
				     &_boundL2Counter[0] );

  TRACE((stderr,"%s() [%s:%d]: agentShmPtr=%p, producerL2counters=%p,%p,%p,%p, boundL2counters=%p,%p,%p,%p\n",__FUNCTION__,__FILE__,__LINE__,_agentShmPtr,_producerL2Counter[0],_producerL2Counter[1],_producerL2Counter[2],_producerL2Counter[3],_boundL2Counter[0],_boundL2Counter[1],_boundL2Counter[2],_boundL2Counter[3]));

  return 0;
}


#if 0
/**
 * \brief Setup the Agent's Shared Memory
 */
static
int setupAgentSharedMemory ()
{
  int rc;

  /* Create/Open the shared memory file and map it. */
  rc = CommAgent_SetupAgentSharedMemory( &_agentShmPtr,
					 &_agentShmFd );
  if (rc)
    {
      fprintf(stderr,"%s() [%s:%d]: SetupAgentSharedMemory returned errno %d\n",__FUNCTION__,__FILE__,__LINE__,rc);
      return rc;
    }

  /*
   * In shared memory, after the shared memory map structure and the queue, is the WAC range,
   * consisting of the queue header, queue, and reception counters.
   * The WAC range must start on a power of 2 boundary and
   * must be that same power of 2 size.
   */
  /* 1. WAC size is the size of the queue header rounded to a 32-byte boundary,
   *    plus the size of the queue rounded to an 8-byte boundary,
   *    plus the reception counters size.
   */
  uint64_t sizeOfQueueHeaderRounded = ( sizeof(CommAgent_QueueHeader_t) + 31 ) & ~(31);
  uint64_t sizeOfQueueRounded       = ( ( _requestQueueSize * sizeof(CommAgent_WorkRequest_t) ) + 7 ) & ~(7);
  uint64_t actualWACsize = sizeOfQueueHeaderRounded + sizeOfQueueRounded + 
    ( _numCounters * sizeof(uint64_t) );

  /* 2. Make the WACsize a power of 2, as required by the wakeup unit */
  uint64_t tmp = 1;  
  while ( tmp < actualWACsize ) {    tmp <<= 1;  }
  uint64_t wacSize = tmp;

  /* 3. Calculate the queue header address to follow the shared memory map structure in
   *    shared memory.  The queue header is the first thing in the WAC range.  So, it
   *    must be on the same power of 2 boundary as the WAC range size. 
  */
  _agentShmPtr->queueHeader = 
    (CommAgent_QueueHeader_t *)
    ( ( ((uint64_t)(_agentShmPtr + 1)) + (wacSize-1) ) & ~(wacSize-1) );
  
  /* 4. Obtain the physical address of the WAC range, and remember the bits that the
   *    WU uses as a mask against the addresses being stored-to so it knows when
   *    to wakeup.
   */
  Kernel_MemoryRegion_t  mregion;
  Kernel_CreateMemoryRegion ( &mregion,
			      _agentShmPtr->queueHeader,
			      wacSize );
  _WACrangePA = 
    (uint64_t)_agentShmPtr->queueHeader -
    (uint64_t)mregion.BaseVa +
    (uint64_t)mregion.BasePa;

  /* Compute the WAC mask to be ANDed with the PA by the WU to determine if the PA is in the wakeup range.
   * Keep only the relevant bits.  The bottom 6 bits are ignored because the WAC range must be larger than
   * 64 bytes.  The top 22 bits are not part of any address. */
   fprintf(stderr,"wacSize-1 = 0x%lx, ~(wacSize-1) = 0x%lx, (~(wacSize-1)) & 0x000003ffffffffc0UL = 0x%lx\n",wacSize-1,(~(wacSize-1)),(~(wacSize-1)) & 0x000003ffffffffc0UL);
  _WACrangeMask = (~(wacSize-1)) & 0x000003ffffffffc0UL;

  /* 5. Calculate the queue address to be after the queue header, rounded to a 32B boundary. */
  _agentShmPtr->queue =
    (CommAgent_WorkRequest_t *)
    ( (uint64_t)_agentShmPtr->queueHeader +
      sizeOfQueueHeaderRounded );

  /* 6. Calculate the rec counter address to be after the queue,
   *    rounded to an 8 byte boundary.
   */
  _agentShmPtr->receptionCounters =
    (volatile uint64_t *)
    ( (uint64_t)_agentShmPtr->queue +
      sizeOfQueueRounded );

  fprintf(stderr,"%s() [%s:%d]: sizeof(QueueHeader)=%lu, sizeOfQueueHeaderRounded=%lu, sizeofQueueRounded=%lu, actualWACsize=%lu, wacSize=%lu, wacRangePA=%lx, wacRangeMask=%lx, _agentShmPtr=%p, queue=%p, queueHeader=%p, recCounterPtr=%p\n",__FUNCTION__,__FILE__,__LINE__,sizeof(CommAgent_QueueHeader_t),sizeOfQueueHeaderRounded,sizeOfQueueRounded,actualWACsize,wacSize,_WACrangePA,_WACrangeMask,_agentShmPtr,_agentShmPtr->queue,_agentShmPtr->queueHeader,_agentShmPtr->receptionCounters);

  /* Set up pointers to the request queue's L2 atomics and initialize them.
   * - Only the bound counter needs to be initialized...everything else
   *   in the queue is zero.
   * - The producer counter needs to be simply read (loaded) by the agent.
   * - The bound counter needs to be incremented by 1 (store-add) by the agent.
   */
  _agentShmPtr->queueHeader->boundL2counter = _requestQueueSize;

  CommAgent_BuildL2AtomicAddresses ( &_agentShmPtr->queueHeader->producerL2counter,
				     MUHWI_ATOMIC_OPCODE_LOAD,
				     &_producerL2Counter[0] );

  CommAgent_BuildL2AtomicAddresses ( &_agentShmPtr->queueHeader->boundL2counter,
				     MUHWI_ATOMIC_OPCODE_STORE_ADD,
				     &_boundL2Counter[0] );

  TRACE((stderr,"%s() [%s:%d]: agentShmPtr=%p, producerL2counters=%p,%p,%p,%p, boundL2counters=%p,%p,%p,%p\n",__FUNCTION__,__FILE__,__LINE__,_agentShmPtr,_producerL2Counter[0],_producerL2Counter[1],_producerL2Counter[2],_producerL2Counter[3],_boundL2Counter[0],_boundL2Counter[1],_boundL2Counter[2],_boundL2Counter[3]));

  return 0;
}
#endif


/**
 * \brief Setup the Wakeup Unit
 */
static
int setupWU()
{
  /* Program the WU to wakeup on MU interrupts
   * The low-order 4 bits specify which of the 4 subgroups on this core
   * to wake up for.  0x0c is 0b1100, which is anything in subgroups 0 or 1
   * (global 64 or 65), which are the subgroups where the commagent could
   * have allocated its MU resources.
   */  
  WU_ArmMU(0x0cUL);

  /* Program the WU to wakeup on stores to the WAC range. */
  reArmWU ();

  return 0;
}


/**
 * \brief Initialize
 *
 * - Get environment variables.
 * - Set up the comm agent's shared memory.
 * - Set up the comm agent's reception fifo.
 */
static
int init ()
{
  int rc;

  rc = setDefaultConfig();

  if ( rc == 0 ) 
  {
    getEnvVars ();

    rc = setupAgentSharedMemory();

    if ( rc == 0 )
    {
      rc = setupInjFifos();

      if ( rc == 0 )
	{
	  rc = setupRecFifos();

	  if ( rc == 0 )
	    {
	      rc = setupBaseAddressTable();
	      if ( rc == 0 )
		{
		  rc = setupWU();

		  if ( rc == 0 )
		    {
		      /* Change the state of the agent's shared memory to "initialized".
		       * At this point, requests can be added to the queue.
		       */

		      _agentShmPtr->agentState = COMM_AGENT_STATE_INITIALIZED_VERSION_2;
		      
		      /* Sync so all processes on the node see these updates. */
		      ppc_msync();
		      
		      TRACE((stderr,"%s() [%s:%d]: AgentState set to INITIALIZED at %lu\n",__FUNCTION__,__FILE__,__LINE__,GetTimeBase()));
		    }
		}
	    }
	}
    }
  }

  return rc;
}

/**
 * \brief Process a Work Request
 *
 * A new work request is ready for processing.  Call the appropriate function
 * to handle the work request, based on the "type" field in the work request.
 */
static
void processWorkRequest( CommAgent_InternalWorkRequest_t *workRequestPtr )
{
  /* This case statement is ordered by most performance critical to least. */
  switch ( workRequestPtr->type )
  {
    case COMM_AGENT_WORK_REQUEST_TYPE_RGET_REQUEST:
      processRgetpacingWorkRequest( 
	(CommAgent_RemoteGetPacing_InternalWorkRequest_t*)workRequestPtr );
      break;
      
    case COMM_AGENT_WORK_REQUEST_TYPE_RGET_INIT:
      processRgetpacingInitWorkRequest( 
        (CommAgent_RemoteGetPacingInit_WorkRequest_t*)workRequestPtr );
      break;
      
    case COMM_AGENT_WORK_REQUEST_TYPE_FENCE_INIT:
      processFenceInitWorkRequest( 
        (CommAgent_FenceInit_WorkRequest_t*)workRequestPtr );
      break;
  }
}


/**
 * \brief Check For Work
 */
static
void checkForNewWork ()
{
  CommAgent_InternalWorkRequest_t *workRequestPtr;
  uint64_t qIndex;
  uint32_t tid  = Kernel_ProcessorThreadID();

  TRACE2((stderr,"%s() [%s:%d]: &producerL2Counter=%p, value=%lu\n",__FUNCTION__,__FILE__,__LINE__,&_agentShmPtr->queueHeader->producerL2counter,_agentShmPtr->queueHeader->producerL2counter));
  TRACE2((stderr,"%s() [%s:%d]: Fetching tail, tid=%u, producerL2Counter[0]=%p\n",__FUNCTION__,__FILE__,__LINE__,tid,_producerL2Counter[tid]));

  /* Fetch the tail.  This could be an atomic load, using the L2 atomic address like this...
   *   uint64_t tail = *_producerL2Counter[tid];
   * But, that load does not prime the L1, so it is not registered with the L2, so
   * when another core writes to that counter, it will not send an invalidate, so
   * we won't wake up.  Thus, just do a normal fetch so the L1 gets primed.  This
   * is atomic too, so no need for the L2 atomic fetch.
   */
  uint64_t tail = _agentShmPtr->queueHeader->producerL2counter;

  TRACE2((stderr,"%s() [%s:%d]: Got tail, tail=%lu\n",__FUNCTION__,__FILE__,__LINE__,tail));

  uint64_t head = _headCounter;
  
  TRACE2((stderr,"%s() [%s:%d]: head=%lu, tail=%lu\n",__FUNCTION__,__FILE__,__LINE__,head,tail));

  /* Check the producer(s)-consumer request queue, calling the appropriate
   * function to handle the work request */
  while ( head < tail )
    { /* There is something in the queue.  Point to it. */      
      qIndex         = head & (COMM_AGENT_REQUEST_QUEUE_SIZE-1);
      workRequestPtr = (CommAgent_InternalWorkRequest_t *)
	                 &_agentShmPtr->queue[qIndex];
      head++;

      /* Wait for the work request in the queue to become READY */
      while ( workRequestPtr->type == COMM_AGENT_WORK_REQUEST_TYPE_NONE );

      /* Sync so we see all of the work request */
      ppc_msync();

      /* \todo Create a work item and queue it */
      processWorkRequest( workRequestPtr );

      TRACE((stderr,"%s() [%s:%d]: Processed request %lu\n",__FUNCTION__,__FILE__,__LINE__,head-1));

      /* Mark the work request as NOT_READY for its next use. */
      workRequestPtr->type = COMM_AGENT_WORK_REQUEST_TYPE_NONE;

      /* Increment the request queue bound to permit another enqueue. */
      *_boundL2Counter[tid] = 1;
    }
  _headCounter = head;
}


/**
 * \brief Poll Reception Fifo
 *
 * Poll the comm agent's reception fifo, processing packets.
 */
static
void pollRecFifo()
{
  uint32_t wrap = 0;
  uint32_t cur_bytes = 0;
  uint32_t total_bytes = 0;
  uint32_t cumulative_bytes = 0;
  MUHWI_PacketHeader_t *hdr = 0;
  uint8_t  dispatchId;

  TRACE2((stderr,"%s() [%s:%d]: >> pollRecFifo\n",__FUNCTION__,__FILE__,__LINE__));

  while ((total_bytes = MUSPI_getAvailableBytes (_rfifo, &wrap)) != 0) 
  {
    TRACE2((stderr,"%s() [%s:%d]: pollRecFifo: total_bytes=%u\n",__FUNCTION__,__FILE__,__LINE__,total_bytes));
    if (wrap == 0) 
    {
      /* No fifo wrap.  Process each packet. */
      cumulative_bytes = 0;
      while (cumulative_bytes < total_bytes ) 
      {
	hdr = MUSPI_getNextPacketOptimized (_rfifo, &cur_bytes);
	dispatchId = hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Unused2[0];
	(*_agentRecFifoDispatch[dispatchId].fn)(
		      _agentRecFifoDispatch[dispatchId].cookie, 
		      hdr, 
		      cur_bytes); /* Call packet handler. */
	cumulative_bytes += cur_bytes;
	/* Touch head for next packet. */
      }
    }
    else 
    {
      /* Packets wrap around to the top of the fifo.  Handle the one packet
       * that wraps.
       */
      hdr = MUSPI_getNextPacketWrap (_rfifo, &cur_bytes);
      dispatchId = hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Unused2[0];
      (*_agentRecFifoDispatch[dispatchId].fn)(
		      _agentRecFifoDispatch[dispatchId].cookie, 
		      hdr, 
		      cur_bytes); /* Call packet handler. */
    }

    /* Store the updated fifo head. */
    MUSPI_syncRecFifoHwHead (_rfifo);
  }
}


/**
 * \brief Do Work
 *
 * Perform each type of work once.
 */
static
void doWork ()
{
  doRemoteGetPacingWork();

  if ( _agentRecFifoDispatchRegistered )
    pollRecFifo();
}


/**
 * \brief Clear Interrupts
 *
 * Clear interrupts that may have fired.
 * We will process anything that caused them to fire, and then go back to
 * wakeup wait.
 */
void clearInterrupts()
{
  MUSPI_ClearInterrupts ( &_rfifo_subgroup,
			  _interruptsToClear );
}


int main(int argc, char **argv)
{
  int rc;

  setbuf(stdout,NULL); setbuf(stderr,NULL);

  TRACE((stderr,"%s() [%s:%d]: Hello from comm agent!\n",__FUNCTION__,__FILE__,__LINE__));

  rc = init ();

  if ( rc == 0 )
    {
      checkForNewWork ();
      doWork ();
      for (;;)
	{
	  clearInterrupts();
	  reArmWU ();
	  checkForNewWork ();
	  doWork ();
	  WUwait ();
	}
      return 0;
    }
  fprintf(stderr,"Comm Agent exiting with error = %d\n",rc);

  /* Note: return 0 (instead of 1) so the app continues running without the comm agent */
  return 0;
}

#undef TRACE
#undef TRACE2
