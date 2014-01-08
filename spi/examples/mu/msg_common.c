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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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


#include <sys/types.h>
#include <stdint.h>
#include <hwi/include/bqc/MU_PacketCommon.h>
#include <spi/include/kernel/MU.h>
#include <spi/include/mu/GIBarrier.h>

#include "msg_common.h"

/**
 * \brief Initialize Buffer with Random Data
 *
 * Initialize a buffer with random data, using the APPSEED env var random number seed,
 * the message's source coordinates, the message's destination coordinates, and a
 * specified unique value.
 *
 * The buffer is initialized backwards so it can be checked backwards.
 *
 * \param [in]  bufPtr     Pointer to the buffer to be initialized.
 * \param [in]  size       Size of the buffer, in bytes.
 * \param [in]  sourcePtr  Pointer to source coordinates (may be NULL)
 * \param [in]  destPtr    Pointer to destination coordinates (may be NULL)
 * \param [in]  uniquePtr  Pointer to unique value (may be NULL)
 * \param [in]  chksumPtr  Pointer to a location where the chksum of the buffer
 *                         is stored (may be NULL).  This result is only
 *                         accurate if size is a multiple of 8.
 *
 * \returns  The buffer is initialized with random data.
 *           The chksum result is returned in the location pointed-to by
 *           chksumPtr.
 */
void msg_InitBuffer ( void                *bufPtr,
                      size_t               size,
                      MUHWI_Destination_t *sourcePtr,
                      MUHWI_Destination_t *destPtr,
                      uint32_t            *uniquePtr,
                      uint64_t            *chksumPtr )
{
  /* memset (bufPtr, size, 0); *//* Simple initialization test */
  size_t i = 0;
  char *buf = (char *) bufPtr;
  for (i = 0; i < size; ++i) {
    buf[i] = i & 0xFF;
  }
}

/**
 * \brief Check Buffer with Random Data
 *
 * Check a buffer with random data, using the APPSEED env var random number seed,
 * the message's source coordinates, the message's destination coordinates,
 * and a specified unique value.
 * 
 * The buffer is checked backwards.  This is because the last bytes of the
 * buffer are the last to arrive, and we want to ensure those bytes are there
 * at the time we begin checking the buffer.
 *
 * \param [in]  bufPtr     Pointer to the buffer to be checked.
 * \param [in]  size       Size of the buffer, in bytes.
 * \param [in]  sourcePtr  Pointer to source coordinates (may be NULL)
 * \param [in]  destPtr    Pointer to destination coordinates (may be NULL)
 * \param [in]  uniquePtr  Pointer to unique value (may be NULL)
 * \param [in]  textPtr    Pointer to text shown in error messages
 *
 * \retval  0  The buffer checks-out successfully.
 * \retval -1  The buffer failed to check correctly.  A message was printed
 *             showing where the check failed.
 *
 * \see msg_InitBuffer()
 */
int msg_CheckBuffer ( void                *bufPtr,
                      size_t               size,
                      MUHWI_Destination_t *sourcePtr,
                      MUHWI_Destination_t *destPtr,
                      uint32_t            *uniquePtr,
                      char                *textPtr )
{
  size_t i = 0;
  char *buf = (char *) bufPtr;
  for (i = 0; i < size; ++i) {
    if (buf[i] != (i & 0xFF))
      return 1;
  }

  return 0;
}

/**
 * \brief Poll a Reception Counter Until It Hits Zero
 *
 * This function polls the specified reception counter until it hits
 * zero, with no timeout.
 *
 * \param [in]  receptionCounter  Pointer to the reception counter
 *
 */
void msg_CounterPoll( volatile uint64_t *receptionCounter )
{
  while (1) 
    {
      if (*receptionCounter == 0)
        {
          break;
        }
    }
}

/**
 * \brief Injection Fifo Info Structure
 *
 * This structure is used within the implementation of the msg_InjFifoXXXXX() 
 * functions.
 */
typedef struct msg_InjFifoInfo
{
  MUSPI_InjFifoSubGroup_t  subgroup[BGQ_MU_NUM_FIFO_SUBGROUPS_PER_NODE];
  uint32_t                 numFifosInSubgroup[BGQ_MU_NUM_FIFO_SUBGROUPS_PER_NODE];
  void                    *fifoMemoryPtr [BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP * 
                                          BGQ_MU_NUM_FIFO_SUBGROUPS_PER_NODE];
  void                    *fifoPtr       [BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP * 
                                          BGQ_MU_NUM_FIFO_SUBGROUPS_PER_NODE];
  uint32_t                 startingSubgroupId;
  uint32_t                 startingFifoId;
  uint32_t                 numFifos;
  uint32_t                 numSubgroups;
} msg_InjFifoInfo_t;


int msg_InjFifoInit ( msg_InjFifoHandle_t *injFifoHandlePtr,
                      uint32_t             startingSubgroupId,
                      uint32_t             startingFifoId,
                      uint32_t             numFifos,
                      size_t               fifoSize,
                      Kernel_InjFifoAttributes_t  *injFifoAttrs )
{  
  void                *buffer = NULL;
  uint32_t endingFifoId; // Relative to a subgroup
  uint32_t numFifosInSubgroup;
  int rc;
  uint32_t subgroupId = startingSubgroupId;
  uint32_t fifoIds[BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP];
  Kernel_InjFifoAttributes_t attrs[BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP];
  Kernel_InjFifoAttributes_t defaultAttrs;
  unsigned int i;
  uint64_t lock_cache;

  memset ( &defaultAttrs, 0x00, sizeof(defaultAttrs) );
  if ( injFifoAttrs == NULL ) injFifoAttrs = &defaultAttrs;

  // Malloc space for the info structure
  msg_InjFifoInfo_t *info;
  info = (msg_InjFifoInfo_t *) memalign(32, sizeof(msg_InjFifoInfo_t));
  if ( !info ) return -1;
  
    // Initialize the info structure
  info->startingSubgroupId = startingSubgroupId;
  info->startingFifoId     = startingFifoId;
  info->numFifos           = numFifos;
  info->numSubgroups       = 0;

  // Malloc space for the injection fifos.  They are 64-byte aligned.
  for (i=0; i<numFifos; i++)
    {
      info->fifoPtr[i] = memalign(64, fifoSize);
      if ( !info->fifoPtr[i] ) return -1;
    }
  
  // Process one subgroup at a time.
  // - Allocate the fifos.
  // - Init the MU MMIO for the fifos.
  // - Activate the fifos.
  while ( numFifos > 0 )
    {
      info->numSubgroups++;

      // startingFifoId is the starting fifo number relative to the
      // subgroup we are working on.
      // Determine endingFifoId, the ending fifo number relative to
      // the subgroup we are working on.
      endingFifoId = startingFifoId + numFifos-1;
      if ( endingFifoId > (BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP-1) )
        endingFifoId = BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP-1;
      numFifosInSubgroup = endingFifoId - startingFifoId + 1;
      info->numFifosInSubgroup[subgroupId] = numFifosInSubgroup;

      // Init structures for allocating the fifos...
      // - fifo Ids
      // - attributes
      for (i=0; i<numFifosInSubgroup; i++)
        {
          fifoIds[i] = startingFifoId + i;
          memcpy(&attrs[i],injFifoAttrs,sizeof(attrs[i]));
/*        printf("Attrs[%u] = 0x%x\n",i,*((uint32_t*)&attrs[i])); */
/*        printf("InjFifoInit: fifoIds[%u]=%u\n",i,fifoIds[i]); */
        }

      // Allocate the fifos
      rc = Kernel_AllocateInjFifos (subgroupId,
                                    &info->subgroup[subgroupId], 
                                    numFifosInSubgroup,
                                    fifoIds,
                                    attrs);
      if ( rc ) {
        printf("msg_InjFifoInit: Kernel_AllocateInjFifos failed with rc=%d\n",rc);
        return rc;
      }

      // Init the MU MMIO for the fifos.
      for (i=0; i<numFifosInSubgroup; i++)
        {
          Kernel_MemoryRegion_t memRegion;
          rc = Kernel_CreateMemoryRegion ( &memRegion,
                                           info->fifoPtr[numFifos-i-1],
                                           fifoSize );
          if ( rc ) {
            printf("msg_InjFifoInit: Kernel_CreateMemoryRegion failed with rc=%d\n",rc);
            return rc;
          }
        
          rc = Kernel_InjFifoInit (&info->subgroup[subgroupId], 
                                   fifoIds[i],
                                   &memRegion,
                                   (uint64_t)info->fifoPtr[numFifos-i-1] -
                                   (uint64_t)memRegion.BaseVa,
                                   fifoSize-1);    
          if ( rc ) {
            printf("msg_InjFifoInit: Kernel_InjFifoInit failed with rc=%d\n",rc);
            return rc;
          }

/*        TRACE(("HW freespace=%lx\n", MUSPI_getHwFreeSpace (MUSPI_IdToInjFifo (fifoIds[i],&info->subgroup[subgroupId]))))
; */
        }

      // Activate the fifos.
      rc = Kernel_InjFifoActivate (&info->subgroup[subgroupId],
                                   numFifosInSubgroup,
                                   fifoIds,
                                   KERNEL_INJ_FIFO_ACTIVATE);
      if ( rc ) {
        printf("msg_InjFifoInit: Kernel_InjFifoActivate failed with rc=%d\n",rc);
        return rc;
      }
      
      startingFifoId = 0; // Next subgroup will start at fifo 0.
      
      subgroupId++;       // Next subgroup.
      numFifos -= numFifosInSubgroup;
    }
  
  injFifoHandlePtr->pOpaqueObject = (void *)info;
  return 0;
}



/**
 * \brief Terminate Injection Fifos
 * 
 * Terminate the usage of injection fifos.  This deactivates the fifos and
 * frees all of the storage associated with them (previously allocated during
 * msg_InjFifoInit()).
 * 
 * \param [in]  injFifoHandle  The handle returned from msg_InjFifoInit().
 *                             It must be passed into this function untouched
 *                             from when it was returned from msg_InjFifoInit().
 *
 * \note After this function returns, no more InjFifo functions should be called
 *       with this injFifoHandle.
 */
void msg_InjFifoTerm ( msg_InjFifoHandle_t injFifoHandle )
{
  return; /*Simple library do nothing! */
}


/**
 * \brief Inject Descriptor into Injection Fifo
 * 
 * Inject the specified descriptor into the specified injection fifo.
 * 
 * \param [in]  injFifoHandle  The handle returned from msg_InjFifoInit().
 *                             It must be passed into this function untouched
 *                             from when it was returned from msg_InjFifoInit().
 * \param [in]  relativeFifoId  The fifo number, relative to the start of
 *                              the fifos managed by this opaque object.
 *                              For example, if msg_InjFifoInit() was called
 *                              to init fifos in subgroup 2, starting with
 *                              fifo Id 3, the relativeFifoNumber of the
 *                              first fifo is 0, not 3.
 * \param [in]  descPtr         Pointer to the descriptor to be injected.
 *
 * \retval  positiveNumber  The descriptor was successfully injected.  The
 *                          returned value is the sequence number of this 
 *                          descriptor.
 * \retval  -1              The descriptor was not injected, most likely because
 *                          there is no room in the fifo.
 */
uint64_t msg_InjFifoInject ( msg_InjFifoHandle_t injFifoHandle,
                             uint32_t            relativeFifoId,
                             MUHWI_Descriptor_t *descPtr )
{
  msg_InjFifoInfo_t *info = (msg_InjFifoInfo_t*)injFifoHandle.pOpaqueObject;
  
  uint32_t globalFifoId = (info->startingSubgroupId * BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP) +
    info->startingFifoId + relativeFifoId;
  
  uint32_t subgroupId   = globalFifoId / BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP;  
  uint64_t rc = MUSPI_InjFifoInject (MUSPI_IdToInjFifo( globalFifoId % BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP,
							&info->subgroup[subgroupId] ),
				     descPtr);
  return rc;  
}


unsigned msg_InjFifoCheckCompletion(msg_InjFifoHandle_t injFifoHandle,
                                    uint32_t            relativeFifoId,
                                    uint64_t            desc_count)
{
  msg_InjFifoInfo_t *info = (msg_InjFifoInfo_t*)injFifoHandle.pOpaqueObject;
  
  uint32_t globalFifoId = (info->startingSubgroupId * BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP) +
    info->startingFifoId + relativeFifoId;
  
  uint32_t subgroupId   = globalFifoId / BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP;

  return MUSPI_CheckDescComplete(MUSPI_IdToInjFifo( globalFifoId % BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP,
                                                    &info->subgroup[subgroupId] ),
                                 desc_count);
}
