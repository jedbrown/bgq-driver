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

#include <stdio.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <agents/include/comm/commagent.h>
#include <agents/include/comm/rgetpacing.h>
#include <agents/include/comm/fence.h>
#include <spi/include/kernel/location.h>
#include <spi/include/kernel/memory.h>
#include <spi/include/kernel/MU.h>
#include <spi/include/mu/Descriptor.h>
#include <spi/include/mu/Descriptor_inlines.h>
#include <spi/include/mu/InjFifo.h>
#include <spi/include/mu/RecFifo.h>
#include <spi/include/mu/Addressing.h>
#include <spi/include/mu/Addressing_inlines.h>
#include <assert.h>
#include <spi/include/wu/wait.h>

#ifdef TRACE
#undef TRACE
#endif
#define TRACE(x) //fprintf x

#define NUM_ITER 100

/* Number of buffers dictates how many buffers are in flight at any time.
 * 1 means 1 1024-byte buffer.
 * 2 means 1 1024-byte buffer, and 1 2048-byte buffer.
 * 3 means 1 1024-byte buffer, and 1 2048-byte buffer, and 1 4096 byte buffer.
 * and so on.
 * The default of 11 means there will be 11 buffers ranging from 1024 through 1 MB in size.
 */
#define NUM_BUFS 11
int _iter=0;

static int                         _injFifoSize = 65536;
static MUSPI_InjFifoSubGroup_t     _ififo_subgroup;
static char                       *_ififoPtr;
static int                         _recFifoSize = 16384;
static MUSPI_RecFifoSubGroup_t     _rfifo_subgroup;
MUSPI_RecFifo_t                   *_rfifoShadowPtr;
static char                       *_rfifoPtr;
uint32_t                           _globalRecFifoId;
MUSPI_BaseAddressTableSubGroup_t   _batSubgroup;
uint8_t                            _batId;
volatile uint64_t                  _MUcounter;
volatile uint64_t                  _MUcounterOffset;

int      _requestStatus[NUM_BUFS]; /* Status of each request.  0=complete.  1=active. */
unsigned char *_sBuff  [NUM_BUFS];
uint64_t       _sBuffPA[NUM_BUFS];
unsigned char *_rBuff  [NUM_BUFS];
uint64_t       _rBuffPA[NUM_BUFS];

MUHWI_Descriptor_t _rgetDesc;
MUHWI_Descriptor_t _dataDesc;
MUHWI_Descriptor_t _completionDesc;

MUHWI_Destination_t myCoords;


static void init()
{
  int rc = 0;
  uint32_t fifoid=0;
  uint32_t subgroup, group;
  int i;

  /* If we are the 1st process, set up the rget inj fifo */
  if ( Kernel_ProcessorID() == 0 )
    {
      /* Set up an rget injection fifo to be used by all processes on this node.
       * It is at a well-known location...subgroup 0, fifo 0.
       * - Allocate storage for an injection fifo
       * - Allocate and initialize that injection fifo.
       * - Activate that injection fifo.
       */
      rc = posix_memalign( (void**)&_ififoPtr, 64, _injFifoSize );
      assert ( rc == 0 );

      /* Set user fifo attribute. */
      Kernel_InjFifoAttributes_t injFifoAttrs[1];
      injFifoAttrs[0].RemoteGet = 1;
      injFifoAttrs[0].System    = 0;
      injFifoAttrs[0].Priority  = 0;
      
      subgroup = 0;

      rc = Kernel_AllocateInjFifos (subgroup,
				    &_ififo_subgroup,
				    1,
				    &fifoid,
				    injFifoAttrs);
      assert ( rc == 0 );
      
      Kernel_MemoryRegion_t  mregion;
      Kernel_CreateMemoryRegion ( &mregion,
				  _ififoPtr,
				  _injFifoSize );
      
      rc = Kernel_InjFifoInit( &_ififo_subgroup,
			       fifoid,
			       &mregion,
			       (uint64_t)_ififoPtr -
			       (uint64_t)mregion.BaseVa,
			       _injFifoSize-1 );
      assert ( rc == 0 );
      
      rc = Kernel_InjFifoActivate ( &_ififo_subgroup,
				    1,
				    &fifoid,
				    KERNEL_INJ_FIFO_ACTIVATE );
      assert ( rc == 0 );

      /* Allocate a Base Address Table Entry for all processes on the node to use,
       * and set its value to zero. 
       */
      uint32_t batId = 0;
      rc = Kernel_AllocateBaseAddressTable( 0, /* subgroup */
					    &_batSubgroup,
					    1,
					    &batId,
					    0 /* "User" access */);
      assert ( rc == 0 );
      
      MUHWI_BaseAddress_t baseAddress;
      baseAddress = 0;
      
      rc = MUSPI_SetBaseAddress ( &_batSubgroup,
				  batId,
				  baseAddress );
      assert ( rc == 0 );
    }  

  /* Set up a reception fifo to receive packets.
   * - Allocate storage for a reception fifo
   * - Use the subgroup equal to our HW thread ID.
   * - Allocate and initialize that reception fifo.
   * - Enable that reception fifo.
   */
  rc = posix_memalign( (void**)&_rfifoPtr, 32, _recFifoSize );
  assert ( rc == 0 );

  Kernel_RecFifoAttributes_t recFifoAttrs[1];
  recFifoAttrs[0].System = 0;

  subgroup = Kernel_ProcessorID();
  group    = Kernel_ProcessorCoreID();

  rc = Kernel_AllocateRecFifos (subgroup,
				&_rfifo_subgroup,
				1,
				&fifoid,
				recFifoAttrs);
  assert ( rc == 0 );

  _rfifoShadowPtr = &_rfifo_subgroup._recfifos[fifoid];

  uint64_t recFifoEnableBits;

  Kernel_MemoryRegion_t  mregion;
  Kernel_CreateMemoryRegion ( &mregion,
			      _rfifoPtr,
			      _recFifoSize );
	      
  rc = Kernel_RecFifoInit( &_rfifo_subgroup, 
			   fifoid,
			   &mregion, 
			   (uint64_t)_rfifoPtr -
			   (uint64_t)mregion.BaseVa,
			   _recFifoSize-1 );
  assert ( rc == 0 );

  recFifoEnableBits = ( 0x0000000000000001ULL << 
			( 15 - 
			  ( ( (Kernel_ProcessorThreadID())*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) + 
			    fifoid ) ) );
		  
  rc = Kernel_RecFifoEnable ( group,
			      recFifoEnableBits );
  assert ( rc == 0 );
  
  _globalRecFifoId = subgroup * BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP;

  /* Allocate NUM_BUFS send and recv buffers */
  for (i=0; i<NUM_BUFS; i++)
    {
      int size = (1<<i)*1024;

      rc = posix_memalign( (void**)&_sBuff[i], 8, size );
      assert ( rc == 0 );

      /* Init the buffer */
      int j;
      unsigned char value=i;
      unsigned char *bufPtr=_sBuff[i];
      for (j=0; j<size; j++)
	{
	  *bufPtr = value++;
	  bufPtr++;
	}

      Kernel_MemoryRegion_t  mregion;
      Kernel_CreateMemoryRegion ( &mregion,
				  _sBuff[i],
				  size );
	      
      _sBuffPA[i] = 
	(uint64_t)_sBuff[i] -
	(uint64_t)mregion.BaseVa +
	(uint64_t)mregion.BasePa;

      rc = posix_memalign( (void**)&_rBuff[i], 8, size );
      assert ( rc == 0 );

      Kernel_CreateMemoryRegion ( &mregion,
				  _rBuff[i],
				  size );
	      
      _rBuffPA[i] = 
	(uint64_t)_rBuff[i] -
	(uint64_t)mregion.BaseVa +
	(uint64_t)mregion.BasePa;
    }

  /* Obtain our node coordinates */
  Personality_t personality;
  Kernel_GetPersonality(&personality, sizeof(personality));
  myCoords.Destination.A_Destination = personality.Network_Config.Acoord;
  myCoords.Destination.B_Destination = personality.Network_Config.Bcoord;
  myCoords.Destination.C_Destination = personality.Network_Config.Ccoord;
  myCoords.Destination.D_Destination = personality.Network_Config.Dcoord;
  myCoords.Destination.E_Destination = personality.Network_Config.Ecoord;
  
  /* Build the remote get descriptor model */
  {
    MUSPI_Pt2PtRemoteGetDescriptorInfo_t i;
    memset(&i, 0x00, sizeof(i));
    i.Base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
    i.Base.Payload_Address = 0; /* To be set by the agent */
    i.Base.Message_Length  = sizeof(MUHWI_Descriptor_t);
    i.Base.Torus_FIFO_Map  = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_PRIORITY;
    i.Base.Dest.Destination.Destination = myCoords.Destination.Destination;
    i.Pt2Pt.Hints_ABCD     = 0;
    i.Pt2Pt.Misc1          = MUHWI_PACKET_USE_DETERMINISTIC_ROUTING;
    i.Pt2Pt.Misc2          = MUHWI_PACKET_VIRTUAL_CHANNEL_HIGH_PRIORITY;
    i.Pt2Pt.Skip           = 0;
    i.RemoteGet.Type             = MUHWI_PACKET_TYPE_GET;
    i.RemoteGet.Rget_Inj_FIFO_Id = 0;
    rc = MUSPI_CreatePt2PtRemoteGetDescriptor( &_rgetDesc,
					       &i
					     );
    assert ( rc == 0 );
  }

  /* Build the data descriptor model */
  {
    MUSPI_Pt2PtDirectPutDescriptorInfo_t i;
    memset(&i, 0x00, sizeof(i));
    i.Base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
    i.Base.Payload_Address = 0; /* To be set at runtime */
    i.Base.Message_Length  = 0; /* To be set at runtime */
    i.Base.Torus_FIFO_Map  = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0;
    i.Base.Dest.Destination.Destination = myCoords.Destination.Destination;
    i.Pt2Pt.Hints_ABCD     = 0;
    i.Pt2Pt.Misc1          = MUHWI_PACKET_USE_DETERMINISTIC_ROUTING;
    i.Pt2Pt.Misc2          = MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
    i.Pt2Pt.Skip           = 0;
    i.DirectPut.Rec_Payload_Base_Address_Id = 0;
    i.DirectPut.Rec_Payload_Offset          = 0; /* To be set at runtime */
    i.DirectPut.Rec_Counter_Base_Address_Id = 0;
    i.DirectPut.Rec_Counter_Offset          = 0; /* Not used...agent uses its own */
    i.DirectPut.Pacing                      = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;
    rc = MUSPI_CreatePt2PtDirectPutDescriptor( &_dataDesc,
					       &i
					     );
    assert ( rc == 0 );
  }

  /* Build the completion descriptor model */
  {
    MUSPI_Pt2PtMemoryFIFODescriptorInfo_t i;  
    memset(&i, 0x00, sizeof(i));
    i.Base.Pre_Fetch_Only  = MUHWI_DESCRIPTOR_PRE_FETCH_ONLY_NO;
    i.Base.Payload_Address = 0;
    i.Base.Message_Length  = 0;
    i.Base.Torus_FIFO_Map  = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_LOCAL0;
    i.Base.Dest.Destination.Destination = myCoords.Destination.Destination;
    i.Pt2Pt.Hints_ABCD     = 0;
    i.Pt2Pt.Misc1          = MUHWI_PACKET_USE_DETERMINISTIC_ROUTING;
    i.Pt2Pt.Misc2          = MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC;
    i.Pt2Pt.Skip           = 0;
    i.MemFIFO.Rec_FIFO_Id  = _globalRecFifoId;
    i.MemFIFO.Rec_Put_Offset = 0; /* Will contain the message number at runtime */
    rc = MUSPI_CreatePt2PtMemoryFIFODescriptor( &_completionDesc,
						&i
					      );
    assert ( rc == 0 );
  }

  /* Initialize request data structures */
  memset(_requestStatus,0x00,sizeof(_requestStatus));

  /* Wait to ensure this fifo has been allocated on process 0 before proceeding. */
  sleep(10);
  
}


/* Handle the packet by 
 * - verifying the data.
 * - marking the associated request complete
 * - if this is the last request (largest), increment iteration count
 */
void handlePacket( MUHWI_PacketHeader_t *hdr )
{
  uint32_t request = hdr->messageUnitHeader.Packet_Types.Memory_FIFO.Put_Offset_LSB;

  /* Verify the data */
  int rc = memcmp ( _sBuff[request], _rBuff[request], (1<<request)*1024 );
  if ( rc != 0 )
    {
      printf("Data compare error on request %u\n",request);
    }

  _requestStatus[request] = 0;
  if ( request == (NUM_BUFS-1) ) 
    {
      fprintf(stderr,"Iteration %d of %d complete\n",_iter,NUM_ITER);
      _iter++;
    }
  TRACE((stderr,"%s() [%s:%d]: Request %u is complete\n",__FUNCTION__,__FILE__,__LINE__,request));
}


/**
 * \brief Reception Fifo Inline Poll Function
 *
 * An example that demonstrates the use of the Reception fifo
 * inlines to poll packets. This poll routine only returns when all
 * packets have been processed from the reception fifo.
 * An alternative to using these inlines is to call the MUSPI_RecFifoPoll
 * function after registering receive functions.
 *
 * \param[in]  rfifo  Reception fifo structure.
 * \param[in]  fn     Function pointer to the function to be called to
 *                    handle each packet.
 * \param[in]  clientdata  Pointer to client data to be passed to the
 *                         receive function fn.
 */

static void poll()
{    
  uint32_t wrap = 0;
  uint32_t cur_bytes = 0;
  uint32_t total_bytes = 0;
  uint32_t cumulative_bytes = 0;
  MUHWI_PacketHeader_t *hdr = 0;

  while ((total_bytes = MUSPI_getAvailableBytes (_rfifoShadowPtr, &wrap)) != 0) 
  {
    if (wrap == 0) 
    {
      /* No fifo wrap.  Process each packet. */
      cumulative_bytes = 0;
      while (cumulative_bytes < total_bytes ) 
      {
	hdr = MUSPI_getNextPacketOptimized (_rfifoShadowPtr, &cur_bytes);

	/* Handle the packet by 
	 * - marking the associated request complete
	 * - if this is the last request (largest), increment iteration count
	 */
	handlePacket(hdr);
	
	cumulative_bytes += cur_bytes;
	/* Touch head for next packet. */
      }
    }
    else 
    {
      /* Packets wrap around to the top of the fifo.  Handle the one packet
       * that wraps.
       */
      hdr = MUSPI_getNextPacketWrap (_rfifoShadowPtr, &cur_bytes);
	/* Handle the packet by 
	 * - marking the associated request complete
	 * - if this is the last request (largest), increment iteration count
	 */
      handlePacket(hdr);
    }

    /* Store the updated fifo head. */
    MUSPI_syncRecFifoHwHead (_rfifoShadowPtr);
  }
}

int main()
{
  volatile int rc;
  int i;
  uint64_t uniqueID=0;
  CommAgent_Control_t control;
  memset(&control, 0x00, sizeof(control));
  CommAgent_WorkRequest_t * workPtr=NULL;;

  setbuf(stdout,NULL); setbuf(stderr,NULL);

  uint32_t tid = Kernel_ProcessorID(); // 0-63.

  if (tid == 0) fprintf(stderr,"%s() [%s:%d]: Hello from app test!\n",__FUNCTION__,__FILE__,__LINE__);

  /* Initialize one at a time to ensure integrity of MU registers that span subgroups */

  sleep(tid);

  init();

  sleep(64-tid);

  TRACE(("Done Initializing tid %u\n",tid));

  rc = CommAgent_Init ( &control );

  TRACE((stderr,"%s() [%s:%d]: CommAgent_Init() returned errno %d\n",__FUNCTION__,__FILE__,__LINE__,rc));  

  assert( rc == 0 );
  
  rc = CommAgent_RemoteGetPacing_Init ( control, 
					(CommAgent_RemoteGetPacing_SharedMemoryInfo_t *)NULL );
  
  TRACE((stderr,"%s() [%s:%d]: RemoteGetPacing_Init() returned errno %d\n",__FUNCTION__,__FILE__,__LINE__,rc));  

  assert( rc == 0 );
  
  rc = CommAgent_Fence_Init ( control );
  
  TRACE((stderr,"%s() [%s:%d]: Fence_Init() returned errno %d\n",__FUNCTION__,__FILE__,__LINE__,rc));  

  assert( rc == 0 );

  /* Loop through the set of remote get requests, sending each request to the agent.
   * Each request contains a direct put data descriptor and a memfifo completion descriptor.
   * Each remote get request has a corresponding completion indicator.
   * When a particular remote get request is submitted to the agent, the completion indicator
   * is set to 1, indicating that the request is active.
   * Each pass through the loop, we poll the reception fifo for completion packets.
   * The dispatch routine clears the completion indicator, so on the next pass,
   * the particular remote get request is submitted again to the agent.
   * This continues until the rget request that is for the largest size has been
   * processed NUM_ITER times.
   */

  /* Mode 0 is when the completion descriptor travels to the remote node and back.
   * Mode 1 is when the completion descriptor is a local transfer.
   */
  int mode;
  for ( mode=0; mode<=1; mode++ )
    {
      fprintf(stderr,"Starting test for mode %d\n",mode);

      _iter = 0;
      while ( _iter < NUM_ITER )
	{
	  for ( i=0; i<NUM_BUFS; i++ )
	    {
	      int size = (1<<i)*1024;
	      /* 1. Find the next request to submit */
	      if ( _requestStatus[i] == 0 )
		{
		  /* Init the recv buffer to all FFs */
		  memset(_rBuff[i],0xFF,size);
		  
		  /*    - mark the request as "active" */
		  _requestStatus[i] = 1;

		  /* 2. Allocate a slot in the agent's queue for the DPut */
		  do
		    {
		      rc = CommAgent_AllocateWorkRequest( control,
							  &workPtr,
							  &uniqueID );

		      TRACE((stderr,"%s() [%s:%d]: AllocateWorkRequest for request %d returned errno %d, workPtr=%p, uniqueID=%lu\n",__FUNCTION__,__FILE__,__LINE__,i,rc,workPtr,uniqueID));
		    } while (rc==EAGAIN);
		  assert(rc==0);
		  
		  /* Remember the unique ID */
		  uint64_t dputUniqueID = uniqueID;

		  /* 3. Set up the DPut request in the agent's queue */
		  memcpy ( &workPtr->request.rget.rgetDescriptor,
			   &_rgetDesc,
			   sizeof(_rgetDesc)
			   );
		  memcpy ( &workPtr->request.rget.payloadDescriptor,
			   &_dataDesc,
			   sizeof(_dataDesc)
			   );
		  MUSPI_SetPayload ( & workPtr->request.rget.payloadDescriptor,
				     _sBuffPA[i],
				     size );
		  MUSPI_SetRecPutOffset ( &workPtr->request.rget.payloadDescriptor, 
					  _rBuffPA[i] );
		  workPtr->request.rget.globalInjFifo = i; /* Make this unique for each DPut request */

		  /* Set the peer ID to match that of the memfifo request so these requests
		   * are done sequentially */
		  if ( mode == 1 ) workPtr->request.rget.peerID = dputUniqueID;
		  else workPtr->request.rget.peerID = 0;
		  
		  /* 4. Submit the DPut request to the agent */
		  rc = CommAgent_RemoteGetPacing_SubmitWorkRequest( control,
								    0, /* handle */
								    &workPtr->request.rget );
		  
		  /* 5. Allocate a slot in the agent's queue for the Memfifo completion */
		  do
		    {
		      rc = CommAgent_AllocateWorkRequest( control,
							  &workPtr,
							  &uniqueID );
		      TRACE((stderr,"%s() [%s:%d]: AllocateWorkRequest for request %d returned errno %d, workPtr=%p, uniqueID=%lu\n",__FUNCTION__,__FILE__,__LINE__,i,rc,workPtr,uniqueID));
		    } while (rc==EAGAIN);
		  assert(rc==0);
		  
		  /* 6. Set up the Memfifo request in the agent's queue */
		  memcpy ( &workPtr->request.rget.rgetDescriptor,
			   &_rgetDesc,
			   sizeof(_rgetDesc)
			   );
		  workPtr->request.rget.globalInjFifo = i; /* Make this match the DPut request so they are ordered */
		  
		  memcpy ( &workPtr->request.rget.payloadDescriptor,
			   &_completionDesc,
			   sizeof(_completionDesc)
			   );
		  /* Store the message number in the packet header.  The
		   * poll function will use it to mark that message complete.
		   */
		  MUSPI_SetRecPutOffset ( & workPtr->request.rget.payloadDescriptor, 
					  i );

		  /* Set the peer ID to match that of the rget request so these requests
		   * are done sequentially */
		  if ( mode == 1 ) workPtr->request.rget.peerID = dputUniqueID;
		  else workPtr->request.rget.peerID = 0;
		  
		  /* 7. Submit the request to the agent */
		  rc = CommAgent_RemoteGetPacing_SubmitWorkRequest( control,
								    0, /* handle */
								    &workPtr->request.rget );
		}
	    }
	  /* Poll the reception fifo, marking completed requests "complete".
	   * When the largest request has completed, increment "iter"
	   */
	  poll();
	}
      
      /* Wait for everything to complete */
      for (i=0; i<NUM_BUFS; i++)
	{
	  while ( _requestStatus[i] ) poll();
	}
    }

  printf("No errors\n");

  /***************************************************************************
   * The following was initial test code that is no longer used
   ***************************************************************************/

#if 0

  for (i=0; i<65; i++)
    {
      rc = CommAgent_AllocateWorkRequest( control,
					  (CommAgent_WorkRequest_t**)&workPtr[i] );
      fprintf(stderr,"%s() [%s:%d]: AllocateWorkRequest returned errno %d, workPtr[%d]=%p\n",__FUNCTION__,__FILE__,__LINE__,rc,i,workPtr[i]);
      if ( rc == EAGAIN ) break;
    }

  int limit = i;
  fprintf(stderr,"%s() [%s:%d]: Limit = %d\n",__FUNCTION__,__FILE__,__LINE__,limit);

  for (i=0; i<limit; i++)
    {
      //      memset(workPtr[i],0x00, sizeof(*workPtr[0]));

      rc = CommAgent_RemoteGetPacing_SubmitWorkRequest( control,
							0, /* handle */
							workPtr[i] );
      fprintf(stderr,"%s() [%s:%d]: SubmitWorkRequest %d returned errno %d\n",__FUNCTION__,__FILE__,__LINE__,i,rc);
    }
  for (i=0; i<65; i++) workPtr[i]=(CommAgent_RemoteGetPacing_WorkRequest_t *)((uint64_t)-1);

  for (i=0; i<256; i++)
    {
      int count=0, idx=i%64;

/*       This was the original code.  It exposes a compiler optimization bug */
/*       where the returned does not appear to be set. */
      
/*             while ( (rc = CommAgent_AllocateWorkRequest(  */
/*                                       control, */
/*       			         (CommAgent_WorkRequest_t**)&workPtr[idx] ) )  */
/*       	      == EAGAIN ) count++;  */

      CommAgent_WorkRequest_t *workRequestPtr;
      while ( (rc = CommAgent_AllocateWorkRequest( control,
						   &workRequestPtr ) )
	      == EAGAIN ) count++;
      assert ( rc == 0 );

      workPtr[idx] = (CommAgent_RemoteGetPacing_WorkRequest_t *)workRequestPtr;

/*       if ( ((uint64_t)(workPtr[idx])) == ((uint64_t)-1) )  */
/* 	{ */
/* 	  fprintf(stderr,"workPtr[%d] at %p = %p\n",idx,&workPtr[idx],workPtr[idx]); */
/* 	  Delay(1000000000); */
/* 	  fprintf(stderr,"workPtr[%d] at %p = %p\n",idx,&workPtr[idx],workPtr[idx]); */
/* 	  fprintf(stderr,"Count=%u, i=%d\n",count,i); */
/* 	  int j; */
/* 	  for (j=0; j<65; j++) fprintf(stderr,"workPtr[%d]=%p\n",j,workPtr[j]); */
/* 	  assert(0); */
/* 	} */

      //  memset(workPtr[idx],0x00, sizeof(CommAgent_RemoteGetPacing_WorkRequest_t));

      rc = CommAgent_RemoteGetPacing_SubmitWorkRequest( control,
							0, /* handle */
							workPtr[idx] );
      assert ( rc == 0 );

      fprintf(stderr,"%s() [%s:%d]: Allocated/Submitted work request %d, count=%d\n",__FUNCTION__,__FILE__,__LINE__,i,count);
      
    }

  // Wait for the agent to process everything.
  Sleep(10);

  fprintf(stderr,"Done with Delay\n");

#endif

  return 0;
}

#undef TRACE
