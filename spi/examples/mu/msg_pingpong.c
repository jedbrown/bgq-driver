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


///////////////////////////////////////////////////// 
////////   Memory Fifo Ping Pong test  //////////////
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
////////  Test library common code  /////////////////
/////////////////////////////////////////////////////
#include "msg_common.h"

// The ping and pong functions
int ping (int a, int b, int c, int d, int e, int t, int bytes, int my_t);
int pong (int a, int b, int c, int d, int e, int t, int bytes, int my_t);

#define NUM_LOOPS   1000

#define MAX_MESSAGE_SIZE              8192            // Multiple of 8
#define REC_MEMORY_FIFO_SIZE          0x00000FFFFULL  // 64K bytes
#define INJ_MEMORY_FIFO_SIZE          0x00000FFFFULL  // 64K bytes

// Injection Memory FIFO Descriptor
MUHWI_Descriptor_t mu_iMemoryFifoDescriptor[2] __attribute__((__aligned__(64))) ;

// Injection Memory FIFO Descriptor Information Structures
MUSPI_Pt2PtMemoryFIFODescriptorInfo_t mu_iMemoryFifoDescriptorInfo[2];

/**
 * \brief Context for book-keeping on the receiver to process incoming
 * packets.
 */
typedef struct MsgContext {
  int    done;
  int    size;
  int    bytes_recvd;
  char  *buf;
  unsigned long long HWEndTime;
} MsgContext_t;

MsgContext_t   my_recv_context[2];  /** Store receive context */

/**
 * \brief Receive packet callback handler. SPIs can poll FIFOs and
 * call a user defined dispatch callback. The packet header carries
 * the dipatch id. 
 */
int recv_packet (void                       * param,
		 MUHWI_PacketHeader_t       * hdr,
		 uint32_t                     bytes) 
{
  MsgContext_t *context   = (MsgContext_t *) param;
  context->HWEndTime      = GetTimeBase();

  SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO);

  char *dst = context->buf + context->bytes_recvd;
  char *src = (char *)hdr + 32;

  context->size = sw_hdr->BytesStruct.message_size_in_bytes;
  bytes -= 32; /* get the payload bytes */

  //MU can pad message with zeros
  if (context->bytes_recvd + bytes >= context->size) {
    bytes = context->size - context->bytes_recvd;
    context->done = 1;
  }
  
  context->bytes_recvd += bytes;  

  if (bytes <= 16) 
    while (bytes --) 
      *(dst++) = *(src++);
  else
    memcpy(dst, src, bytes);
  
  return 0;
}

MUSPI_InjFifoSubGroup_t   ififo_subgroup[2];
MUSPI_RecFifoSubGroup_t   rfifo_subgroup[2];

//The source of the ping pong message
int   ROOT_A = 0;
int   ROOT_B = 0;
int   ROOT_C = 0;
int   ROOT_D = 0;
int   ROOT_E = 0;
int   ROOT_T = 0;

//The address of the neighbor that bounces it back
int   NEIGHBOR_A = 0;
int   NEIGHBOR_B = 0;
int   NEIGHBOR_C = 1;
int   NEIGHBOR_D = 0;
int   NEIGHBOR_E = 0;
int   NEIGHBOR_T = 0;

// allocates area for message send buffer
uint64_t sbuf[MAX_MESSAGE_SIZE/sizeof(uint64_t)];

// allocates area for message recv buffer
uint64_t rbuf[MAX_MESSAGE_SIZE/sizeof(uint64_t)];

#define test_exit  exit

int main(int argc, char **argv)
{
  int rc;
  Personality_t pers;
  //works in cnk ?
  Kernel_GetPersonality(&pers, sizeof(pers));

  int32_t     my_a = 0;
  int32_t     my_b = 0;
  int32_t     my_c = 0;
  int32_t     my_d = 0;
  int32_t     my_e = 0;
  int32_t     my_t = 0;  //Set from kernel sys call
  uint i = 0;
  uint64_t gi_timeout = 1600000000; // about 1 sec at 16 mhz
  gi_timeout *= 30;

  char rec_memory_fifo_buffer[REC_MEMORY_FIFO_SIZE+32];
  void *rec_memory_fifo = (void*)(rec_memory_fifo_buffer);
  rec_memory_fifo = (void*)( ((uint64_t)rec_memory_fifo_buffer + 32)  & ~(31UL) );
  char inj_memory_fifo_buffer[INJ_MEMORY_FIFO_SIZE+64];
  void *inj_memory_fifo = (void*)( ((uint64_t)inj_memory_fifo_buffer + 64)  & ~(63UL) );

#ifdef TIME_DESCRIPTOR_CREATION
  //printf("main(): Memory Fifo Ping Pong Test - Timing Descriptor Creation\n");
#else
  //printf("main(): Memory Fifo Ping Pong Test - Not Timing Descriptor Creation\n");
#endif

  my_a = pers.Network_Config.Acoord;
  my_b = pers.Network_Config.Bcoord;
  my_c = pers.Network_Config.Ccoord;
  my_d = pers.Network_Config.Dcoord;
  my_e = pers.Network_Config.Ecoord;

  //Assume atomst one process per core
  my_t = Kernel_PhysicalProcessorID ();
  
  if (my_t > 1)
    my_t = 1;

  int _root = 0, _neighbor = 0; 

  if (my_a == ROOT_A &&
      my_b == ROOT_B &&
      my_c == ROOT_C &&
      my_d == ROOT_D &&
      my_e == ROOT_E &&
      my_t == ROOT_T)
    {
      printf("We are the root node with coords %d %d %d %d %d %d sending to coords %d %d %d %d %d %d\n",my_a,my_b,my_c,my_d,my_e,my_t,NEIGHBOR_A,NEIGHBOR_B,NEIGHBOR_C,NEIGHBOR_D,NEIGHBOR_E,NEIGHBOR_T);
      _root = 1;
    }
  else if (my_a == NEIGHBOR_A &&
	   my_b == NEIGHBOR_B &&
	   my_c == NEIGHBOR_C &&
	   my_d == NEIGHBOR_D &&
	   my_e == NEIGHBOR_E &&
	   my_t == NEIGHBOR_T)
    {
      printf("We are the neighbor node with coords %d %d %d %d %d %d\n",my_a,my_b,my_c,my_d,my_e,my_t);      
      _neighbor = 1;
    }

  //////// Initializes the send buffer ///////////////////////
  for (i=0;i<MAX_MESSAGE_SIZE/8;i++) 
    sbuf [i] = (uint64_t)(i+4) + 0x0100000000000000ull;
  
  // clears the recv buffer
  for (i=0;i<MAX_MESSAGE_SIZE/8;i++)
  {
    rbuf[i] = 0x00;
  }
  
  //////////////////////////////////////////////////////////
  /////////////// Initialize Injection FIFOs ///////////////
  //////////////////////////////////////////////////////////
  uint32_t fifoid = 0;  
  Kernel_InjFifoAttributes_t injFifoAttrs[1];
  injFifoAttrs[0].RemoteGet = 0;
  injFifoAttrs[0].System    = 0;  
  rc = Kernel_AllocateInjFifos (my_t,
				&ififo_subgroup[my_t], 
				1,
				&fifoid, 
				injFifoAttrs);
  
  /// Map virtual address
  Kernel_MemoryRegion_t  mregion;
  Kernel_CreateMemoryRegion (&mregion,inj_memory_fifo,INJ_MEMORY_FIFO_SIZE+1);
  Kernel_InjFifoInit (&ififo_subgroup[my_t], 
		      fifoid, 
		      &mregion, 
		      (uint64_t)inj_memory_fifo - (uint64_t)mregion.BaseVa, 
		      INJ_MEMORY_FIFO_SIZE);    
  Kernel_InjFifoActivate (&ififo_subgroup[my_t], 1, &fifoid, KERNEL_INJ_FIFO_ACTIVATE);    
  
  /////////////////////////////////////////////////////////
  ////////// Initialize Reception FIFOs ///////////////////
  ////////////////////////////////////////////////////////
  uint32_t rfifoid = 0;
  memset (&my_recv_context[my_t], 0, sizeof(my_recv_context[0]));  
  MUSPI_RegisterRecvFunction (recv_packet, &my_recv_context[my_t], my_t+1);
  my_recv_context[my_t].buf = (char *) rbuf;  
  Kernel_CreateMemoryRegion (&mregion, rec_memory_fifo, REC_MEMORY_FIFO_SIZE);
  Kernel_RecFifoAttributes_t recFifoAttrs[1];
  recFifoAttrs[0].System = 0;

  Kernel_AllocateRecFifos (my_t, 
			   &rfifo_subgroup[my_t], 
			   1,
			   &rfifoid,
			   recFifoAttrs);
  
  Kernel_RecFifoInit    (& rfifo_subgroup[my_t], rfifoid, 
			 &mregion, 
			 (uint64_t)rec_memory_fifo - (uint64_t)mregion.BaseVa, 
			 REC_MEMORY_FIFO_SIZE);

  uint64_t recFifoEnableBits=0;
  
  recFifoEnableBits |= ( 0x0000000000000001ULL << 
			 ( 15 - ( (my_t/*sgid*/*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) + 0/*RecFifoId*/ )) );

  Kernel_RecFifoEnable ( 0, /* Group ID */ 
			 recFifoEnableBits );
  
  /////////////////////////////////////////////////
  ////////  Initialize GI Barrier  ////////////////
  /////////////////////////////////////////////////
  MUSPI_GIBarrier_t GIBarrier;  
  // Initialize the barrier, resetting the hardware.
  rc = MUSPI_GIBarrierInit ( &GIBarrier, 0 /* classRouteId */ );
  if (rc)
    {
      printf("MUSPI_GIBarrierInit for class route %u returned rc = %d\n",0, rc);
      test_exit(__LINE__);
    }  
  if (rc) test_exit(__LINE__);
  
  // Enter the MU barrier
  rc = MUSPI_GIBarrierEnter ( &GIBarrier );
  if (rc)
    {
      printf("MUSPI_GIBarrierEnter failed on iteration = %d, returned rc = %d\n", i, rc);
      test_exit(1);
    }
  
  // Poll for completion of the barrier.
  rc = MUSPI_GIBarrierPollWithTimeout ( &GIBarrier, gi_timeout);
  if ( rc )
    {
      printf("MUSPI_GIBarrierPollWithTimeout failed on iteration = %d, returned rc = %d\n", i, rc);
      test_exit(1);
    }
  
  ///Test ping pong
  if (_root)
    ping (NEIGHBOR_A, NEIGHBOR_B, NEIGHBOR_C, NEIGHBOR_D, NEIGHBOR_E, NEIGHBOR_T, MAX_MESSAGE_SIZE, my_t); 
  else if (_neighbor)
    pong (ROOT_A, ROOT_B, ROOT_C, ROOT_D, ROOT_E, ROOT_T, MAX_MESSAGE_SIZE, my_t);
  else
    test_exit (0);
  
  Delay(1000); // Make sure all processes are done
  test_exit(0);	
  return 0;
}


int ping   ( int      a,
	     int      b,
	     int      c, 
	     int      d, 
	     int      e, 
	     int      t,
	     int      bytes,
	     int      my_t)
{
  int rc =0;
  int i=0;
  unsigned long long StartTime=0,EndTime,MeanTime;
  unsigned long long HWStartTime=0, HWTotalTime=0, HWMeanTime;
  
  rc =0;
 
  SoftwareBytes_t  SoftwareBytes;
  memset( &SoftwareBytes, 0x00, sizeof(SoftwareBytes_t) );

  Delay(500000); // Make sure receiver is ready
  
  for ( i= 0;i<=NUM_LOOPS;i++)
  {     
    if (i == 1)
      StartTime =  GetTimeBase();
    
#ifdef TIME_DESCRIPTOR_CREATION
    if ( i >=0)  // Creates descriptor each time
#else
    if ( i ==0)  // Create  descriptor once
#endif
    {	 
      // bit 0 ===> A- Torus FIFO
      uint64_t torusInjectionFifoMap = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP; 

      // Set up the memory fifo information structure for message
      // 1.  It is used as input to build a memory fifo descriptor.
      memset( &mu_iMemoryFifoDescriptorInfo[my_t], 0x00, 
	      sizeof(mu_iMemoryFifoDescriptorInfo[0]) );
      
      MUSPI_SetUpDestination ( &mu_iMemoryFifoDescriptorInfo[my_t].Base.Dest,
			       a, b, c, d, e );
      SoftwareBytes.BytesStruct.functionid = t+1;
      SoftwareBytes.BytesStruct.message_size_in_bytes = bytes;
      
      msg_BuildPt2PtMemoryFifoInfo ( &mu_iMemoryFifoDescriptorInfo[my_t],
				     SoftwareBytes,
				     t*4,   //use the t to isolate the recfifo
				     0, 
				     (uint64_t)sbuf,
				     bytes,
				     torusInjectionFifoMap,
				     MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC,
				     0,
				     MUHWI_PACKET_HINT_E_NONE);
      
      rc = MUSPI_CreatePt2PtMemoryFIFODescriptor 
	( &(mu_iMemoryFifoDescriptor[my_t]),
	  &(mu_iMemoryFifoDescriptorInfo[my_t]) );
      
      assert(rc ==0);
      
      /* MUSPI_DescriptorDumpHex */
      /* ( (char*)"Actual Memory FIFO Descriptor for message 1", */
      /* 	&(mu_iMemoryFifoDescriptor[my_t]) ); */
    }
   
    // -----------------------------------------
    //    Inject this message into the fifo
    //    - For a hardware timing, start the timer after the injection.
    //      The end time is recorded in the context when the packet is received.
    // ----------------------------------------
    rc = MUSPI_InjFifoInject (MUSPI_IdToInjFifo(0, &ififo_subgroup[my_t]), 
			      &mu_iMemoryFifoDescriptor[my_t]);

    HWStartTime =  GetTimeBase();

    // ---------------------------------------------
    //    Reception Side
    // ---------------------------------------------
    while (!my_recv_context[my_t].done) 
      MUSPI_RecFifoPoll (MUSPI_IdToRecFifo (0, &rfifo_subgroup[my_t]), 
			 1000);
    my_recv_context[my_t].done = 0;
    my_recv_context[my_t].bytes_recvd = 0;    
    
    if (i>0) 
      {
	HWTotalTime += (my_recv_context[my_t].HWEndTime - HWStartTime);
      }
    
  } // i loop
  
  EndTime    = GetTimeBase();
  MeanTime   = (EndTime -StartTime)/(2*NUM_LOOPS);  
  HWMeanTime = HWTotalTime / (2*NUM_LOOPS);
  int hops = a + b + c + d + e;
  printf("ping:  MessageSize=%d, Iterations=%d, a=%3d, b=%3d, c=%3d, d=%3d, e=%3d, hops=%3d, HWMeanTime=%10Ld, SWMeanTime=%10Ld \n",
         bytes, NUM_LOOPS, a, b, c, d, e, hops, HWMeanTime, MeanTime);
  
  //assert(packets_received <= NUM_LOOPS+1);
  return rc;
}


int pong   ( int      a,
	     int      b,
	     int      c, 
	     int      d, 
	     int      e, 
	     int      t,
	     int      bytes,
	     int      my_t)
{
  int rc =0;
  int i=0;
  unsigned long long StartTime,EndTime,MeanTime;

  SoftwareBytes_t  SoftwareBytes;
  memset( &SoftwareBytes, 0x00, sizeof(SoftwareBytes_t) );
  
  rc =0;  
  int rfifoid = 0;

  StartTime =  GetTimeBase();
  for ( i= 0;i<=NUM_LOOPS;i++)
  {
    if (i == 1)
      StartTime =  GetTimeBase();
    
    // ---------------------------------------------
    //    Reception Side
    // ---------------------------------------------
    while (!my_recv_context[my_t].done) 
      MUSPI_RecFifoPoll (MUSPI_IdToRecFifo (rfifoid, &rfifo_subgroup[my_t]), 
			 1000);
    my_recv_context[my_t].done = 0;
    my_recv_context[my_t].bytes_recvd = 0;    

    
#ifdef TIME_DESCRIPTOR_CREATION
    if ( i >=0)  // Creates descriptor each time
#else
    if ( i ==0)  // Create  descriptor once
#endif
    {	 
      // bit 0 ===> A- Torus FIFO
      uint64_t torusInjectionFifoMap = MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM; 
      
      // Set up the memory fifo information structure for message
      // 1.  It is used as input to build a memory fifo descriptor.
      memset( &mu_iMemoryFifoDescriptorInfo[my_t], 0x00, 
	      sizeof(mu_iMemoryFifoDescriptorInfo[0]) );
      
      MUSPI_SetUpDestination ( &mu_iMemoryFifoDescriptorInfo[my_t].Base.Dest,
			       a, b, c, d, e );
      SoftwareBytes.BytesStruct.functionid = t+1;
      SoftwareBytes.BytesStruct.message_size_in_bytes = bytes;
      
      msg_BuildPt2PtMemoryFifoInfo ( &mu_iMemoryFifoDescriptorInfo[my_t],
				     SoftwareBytes,
				     t*4,
				     0, 
				     (uint64_t)sbuf,
				     bytes,
				     torusInjectionFifoMap,
				     MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC,
				     0,
				     MUHWI_PACKET_HINT_E_NONE);
      
      rc = MUSPI_CreatePt2PtMemoryFIFODescriptor 
	( &(mu_iMemoryFifoDescriptor[my_t]),
	  &(mu_iMemoryFifoDescriptorInfo[my_t]) );
      
      assert(rc ==0);
      
      //MUSPI_DescriptorDumpHex 
      //( (char*)"Actual Memory FIFO Descriptor for message 1", 
      //&(mu_iMemoryFifoDescriptor[my_t]) );	 
    }
    
    // -----------------------------------------
    //    Inject this message into the fifo
    // ----------------------------------------
    rc = MUSPI_InjFifoInject (MUSPI_IdToInjFifo(0, &ififo_subgroup[my_t]), 
			      &mu_iMemoryFifoDescriptor[my_t]);
  } // i loop
  
  EndTime =  GetTimeBase();
  MeanTime = (EndTime -StartTime)/(2*NUM_LOOPS);  
  int hops = a+b+c+d+e;
  printf("pong:  MessageSize=%d, Iterations=%d, a=%3d, b=%3d, c=%3d, d=%3d, e=%3d, hops=%3d, MeanTime = %10Ld \n",
         bytes, NUM_LOOPS, a, b, c, d, e,hops, MeanTime);
  //  assert(packets_received <= NUM_LOOPS+1);
  return rc;
}

