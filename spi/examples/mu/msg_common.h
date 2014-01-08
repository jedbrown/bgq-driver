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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

//////////////////////////////////////////
// Basic SPI and HWI includes
//////////////////////////////////////////
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/MU_PacketCommon.h>
#include <firmware/include/personality.h>
#include <spi/include/mu/Descriptor.h>
#include <spi/include/mu/Descriptor_inlines.h>
#include <spi/include/mu/InjFifo.h>
#include <spi/include/mu/RecFifo.h>
#include <spi/include/mu/Addressing.h>
#include <spi/include/mu/Addressing_inlines.h>
#include <spi/include/mu/GIBarrier.h>
#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/process.h>
#include <spi/include/kernel/location.h>
#include <spi/include/kernel/collective.h>

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
                      uint64_t            *chksumPtr );

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
                      char                *textPtr );

/**
 * \brief Poll a Reception Counter Until It Hits Zero
 *
 * This function polls the specified reception counter until it hits
 * zero, with no timeout.
 *
 * \param [in]  receptionCounter  Pointer to the reception counter
 *
 */
void msg_CounterPoll( volatile uint64_t *receptionCounter );


/**
 * \brief Injection Fifo Handle
 *
 * This is a "handle" returned from msg_InjFifoInit() and passed into subsequent
 * calls to msg_InjFifoXXXX() functions.  It is used internally within the
 * msg_InjFifoXXXX() functions to anchor resources that have been allocated.
 */
typedef struct {
  void* pOpaqueObject;
} msg_InjFifoHandle_t;


/**
 * \brief Poll a Reception Counter Until It Hits Zero or Exceeds Timeout
 *
 * This function polls the specified reception counter until it hits
 * zero, or until the specified time limit is exceeded.
 *
 * \param [in]  receptionCounter  Pointer to the reception counter
 * \param [in]  counterPollBase   The base value returned from a previous
 *                                call to msg_CounterPollWithTimeoutInit().
 * \param [in]  seconds           Time limit (in seconds)
 *
 * \retval  0  Successful poll.  The counter hit zero.
 * \retval -1  Unsuccessful poll.  The time limit was exceeded.
 *
 * \see msg_CounterPollWithTimeoutInit()
 */
int msg_CounterPollWithTimeout( volatile uint64_t *receptionCounter,
                                uint64_t           counterPollBase,
                                uint64_t           seconds );

int msg_InjFifoInit ( msg_InjFifoHandle_t *injFifoHandlePtr,
                      uint32_t             startingSubgroupId,
                      uint32_t             startingFifoId,
                      uint32_t             numFifos,
                      size_t               fifoSize,
                      Kernel_InjFifoAttributes_t  *injFifoAttrs );


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
void msg_InjFifoTerm ( msg_InjFifoHandle_t injFifoHandle );


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
                             MUHWI_Descriptor_t *descPtr );


static inline void msg_BuildPt2PtDirectPutInfo 
(MUSPI_Pt2PtDirectPutDescriptorInfo_t   * dinfo,
 uint64_t                                 put_offset,
 uint64_t                                 counter_offset,
 uint64_t                                 buffer,
 uint64_t                                 bytes,
 uint64_t                                 torusInjectionFifoMap,
 int                                      vc,
 uint8_t                                  hintsABCD,
 uint8_t                                  hintsE)
{
  dinfo->Base.Pre_Fetch_Only  = 0;
  dinfo->Base.Payload_Address = buffer;
  dinfo->Base.Message_Length  = bytes;
  dinfo->Base.Torus_FIFO_Map  = torusInjectionFifoMap;
  //  dinfo->Base.Dest            = dest;
  dinfo->Pt2Pt.Hints_ABCD = hintsABCD;

  if ( vc == MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC ){
    dinfo->Pt2Pt.Misc1      = hintsE                                 |
                              MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE   |
                              MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
                              MUHWI_PACKET_DO_NOT_DEPOSIT;
    dinfo->Pt2Pt.Misc2      = MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
  }
  else if (vc == MUHWI_PACKET_VIRTUAL_CHANNEL_DYNAMIC) {
    dinfo->Pt2Pt.Misc1      = hintsE                               |
                              MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE |
                              MUHWI_PACKET_USE_DYNAMIC_ROUTING     |
                              MUHWI_PACKET_DO_NOT_DEPOSIT;
    dinfo->Pt2Pt.Misc2      = MUHWI_PACKET_VIRTUAL_CHANNEL_DYNAMIC;
  }
  dinfo->Pt2Pt.Skip       = 0;
  dinfo->DirectPut.Rec_Payload_Base_Address_Id = 0;
  dinfo->DirectPut.Rec_Payload_Offset          = put_offset;
  dinfo->DirectPut.Rec_Counter_Base_Address_Id = 0;
  dinfo->DirectPut.Rec_Counter_Offset          = counter_offset;
  
  dinfo->DirectPut.Pacing = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;  
}


/* Init software bytes for the packet header. */
typedef union _swb
{
  struct {
    uint8_t reserved[6];
    uint8_t bytes[14];
  };
  
  struct {
    uint8_t  reserved[6];
    uint8_t  functionid;
    uint8_t  bytes[5];
    
    uint32_t message_size_in_bytes;
    uint8_t unused[4];
  } BytesStruct;
  
} SoftwareBytes_t;


static inline void msg_BuildPt2PtMemoryFifoInfo 
(MUSPI_Pt2PtMemoryFIFODescriptorInfo_t  * minfo,
 SoftwareBytes_t                          SoftwareBytes,  
 uint                                     rfifoid,
 uint64_t                                 put_offset,
 uint64_t                                 buffer,
 uint64_t                                 bytes,
 uint64_t                                 torusInjectionFifoMap,
 int                                      vc,
 uint8_t                                  hintsABCD,
 uint8_t                                  hintsE)
{
  minfo->Pt2Pt.Hints_ABCD = hintsABCD;
  if ( vc ==  MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC ) {
    minfo->Pt2Pt.Misc1      = hintsE         |
      MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE   |
      MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
      MUHWI_PACKET_DO_NOT_DEPOSIT;
    minfo->Pt2Pt.Misc2 = MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
  }
  else if (vc == MUHWI_PACKET_VIRTUAL_CHANNEL_DYNAMIC) {
    minfo->Pt2Pt.Misc1      = hintsE       |
      MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE |
      MUHWI_PACKET_USE_DYNAMIC_ROUTING     |
      MUHWI_PACKET_DO_NOT_DEPOSIT;
    minfo->Pt2Pt.Misc2      = MUHWI_PACKET_VIRTUAL_CHANNEL_DYNAMIC;
  }
  else if ( vc ==  MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM ) {
    minfo->Pt2Pt.Misc1      = hintsE         |
      MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE   |
      MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |
      MUHWI_PACKET_DO_NOT_DEPOSIT;
    minfo->Pt2Pt.Misc2 = MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM;
  }
  else { MUSPI_assert(0); }
    
  minfo->Pt2Pt.Skip  = 0;
  minfo->MemFIFO.Rec_FIFO_Id    = rfifoid;
  minfo->MemFIFO.Rec_Put_Offset = put_offset;
  minfo->Base.Pre_Fetch_Only= MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
  minfo->MemFIFO.Interrupt  = MUHWI_PACKET_DO_NOT_INTERRUPT_ON_PACKET_ARRIVAL;
  minfo->MemFIFO.SoftwareBit = 0;
  memcpy( minfo->MemFIFO.SoftwareBytes,
          SoftwareBytes.bytes,
          sizeof( minfo->MemFIFO.SoftwareBytes ) );

  //Warning assume V == P
  minfo->Base.Payload_Address = buffer;
  minfo->Base.Message_Length =  bytes;
  minfo->Base.Torus_FIFO_Map = torusInjectionFifoMap;
}



static inline void msg_BuildCollectiveDirectPutAllreduceInfo
(MUSPI_CollectiveDirectPutDescriptorInfo_t  * dinfo,
 uint64_t                                bytes,
 uint64_t                                buffer, 
 uint32_t                                op,
 uint32_t                                sizeoftype,
 MUHWI_Destination_t                     dest,
 uint64_t                                put_offset,
 uint64_t                                counter_offset,
 uint32_t                                classRoute,
 int                                     vc) 
{
  uint64_t torusInjectionFifoMap = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CUSER;  // bit 0 ===> A- Torus FIFO
  
  if (vc == MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM_COLLECTIVE)
    torusInjectionFifoMap = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CSYSTEM;

  dinfo->Base.Pre_Fetch_Only  = 0;
  dinfo->Base.Payload_Address = (uint64_t)buffer;
  dinfo->Base.Message_Length  = bytes;
  dinfo->Base.Torus_FIFO_Map  = torusInjectionFifoMap;
  dinfo->Base.Dest            = dest;
  
  dinfo->Collective.Op_Code     = op;
  dinfo->Collective.Word_Length = sizeoftype;
  dinfo->Collective.Class_Route = classRoute;
  dinfo->Collective.Misc        = vc | MUHWI_COLLECTIVE_TYPE_ALLREDUCE;
  dinfo->Collective.Skip        = 0;
  
  dinfo->DirectPut.Rec_Payload_Base_Address_Id = 0;
  dinfo->DirectPut.Rec_Payload_Offset          = put_offset;
  dinfo->DirectPut.Rec_Counter_Base_Address_Id = 0;
  dinfo->DirectPut.Rec_Counter_Offset          = counter_offset;

  dinfo->DirectPut.Pacing = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;  
}


static inline void msg_BuildCollectiveMemoryFifoAllreduceInfo
(MUSPI_CollectiveMemoryFIFODescriptorInfo_t  * minfo,
 SoftwareBytes_t                          SoftwareBytes,  
 uint64_t                                bytes,
 uint64_t                                buffer, 
 uint32_t                                op,
 uint32_t                                sizeoftype,
 MUHWI_Destination_t                     dest,
 uint64_t                                put_offset,
 uint32_t                                rfifoid,
 uint32_t                                classRoute,
 int                                     vc) 
{
  uint64_t torusInjectionFifoMap = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CUSER;  // bit 0 ===> A- Torus FIFO
  
  if (vc == MUHWI_PACKET_VIRTUAL_CHANNEL_SYSTEM_COLLECTIVE)
    torusInjectionFifoMap = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CSYSTEM;

  minfo->Base.Pre_Fetch_Only  = 0;
  minfo->Base.Payload_Address = (uint64_t)buffer;
  minfo->Base.Message_Length  = bytes;
  minfo->Base.Torus_FIFO_Map  = torusInjectionFifoMap;
  minfo->Base.Dest            = dest;
  
  minfo->Collective.Op_Code     = op;
  minfo->Collective.Word_Length = sizeoftype;
  minfo->Collective.Class_Route = classRoute;
  minfo->Collective.Misc        = vc | MUHWI_COLLECTIVE_TYPE_ALLREDUCE;
  minfo->Collective.Skip        = 0;
  
  minfo->MemFIFO.Rec_FIFO_Id    = rfifoid;
  minfo->MemFIFO.Rec_Put_Offset = put_offset;

  minfo->MemFIFO.Interrupt  = MUHWI_PACKET_DO_NOT_INTERRUPT_ON_PACKET_ARRIVAL;
  minfo->MemFIFO.SoftwareBit = 0;
  memcpy( minfo->MemFIFO.SoftwareBytes,
          SoftwareBytes.bytes,
          sizeof( minfo->MemFIFO.SoftwareBytes ) );
}
