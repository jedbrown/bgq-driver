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



///////////////////////////////////////////////////////
////  Alltoall Test via MU Rmote Put operations
///////////////////////////////////////////////////////

/////////////////////////////////////////
/// Test library common code
////////////////////////////////////////
#include "msg_common.h"

#define SEND_BUFFER_ALIGNMENT   128
#define RECV_BUFFER_ALIGNMENT   128
#define MAX_MESSAGE_SIZE       32768

// Sub message size
static int window_size  =  2048;   //  size of submessages
long long messageSizeInBytes = MAX_MESSAGE_SIZE;

// Maximum number of ndoes this test will run on
#define MAX_NUM_NODES          1024

#define NUM_INJ_FIFOS          64
#define INJ_MEMORY_FIFO_SIZE  ((64*1024) -1)

int num_iterations  = 100;
int skip_iterations = 1;
int check_messages  = 1;
int do_dynamic      = 1;

// Allocate static memory for descriptors
char muDescriptorsMemory[ MAX_NUM_NODES * sizeof(MUHWI_Descriptor_t) + 64 ];

// pointer to descriptor array
MUHWI_Descriptor_t *muDescriptors;

// Allocate static memory for send and receive buffers
char sendBufMemory[MAX_NUM_NODES * MAX_MESSAGE_SIZE+ SEND_BUFFER_ALIGNMENT];
char recvBufMemory[MAX_NUM_NODES * MAX_MESSAGE_SIZE+ SEND_BUFFER_ALIGNMENT];


// Random permutation array. The alltoall messages will be sent
// to neighbors in a random permutation
uint32_t randPerm[MAX_NUM_NODES];

// rank2dest cache
struct { 
  MUHWI_Destination_t dest;
  uint8_t             hintsABCD;
  uint8_t             hintsE;
} rank2dest[ MAX_NUM_NODES];

// pointers to send and receive buffers
char * recvBuffers;
char * sendBuffers;

// receive counter
volatile uint64_t recvCounter;

// base addess table slot for receive buffer and counter
uint32_t recvBufBatId = 0, recvCntrBatId = 1;

// physical address of send buffers
uint64_t sendBufPAddr;

// struct to hold torus coordinates or dimensions data
typedef struct { uint8_t a,b,c,d,e; } torus_t;

// number of participating processes (1 proc/node)
unsigned numNodes;

// rank of this process
unsigned myRank;

// Enable different zone routing modes
uint8_t  zoneRoutingMask = 0;
unsigned zoneRoutingId   = 0;

// stay on bubble bits
uint8_t stayOnBubbleMask  = 0;
unsigned stayOnBubbleFlag = 0;

// Call to create the alltoall descriptors
void create_descriptors();

// Set up hints, fifomaps and destination address values
void setup_destinations(Personality_t *p);

// Call to set up the base address table id and memory regions for the
// alltoall
void setup_mregions_bats_counters(void);

// Create a random permutation
void create_random_permutation(void);

MUSPI_GIBarrier_t GIBarrier;

void alltoall_exit (int rc) {
  exit (rc);
}

//Call GI barrier
void global_barrier(void)
{
  int rc = 0;
  uint64_t timeoutCycles = 60UL * 1600000000UL; // about 60 sec at 1.6 ghz
  rc = MUSPI_GIBarrierEnter ( &GIBarrier );
  if (rc)
    {
      printf("MUSPI_GIBarrierEnter failed returned rc = %d\n", rc);
      alltoall_exit(1);
    }
  
  //=======================================================================
  
  // Poll for completion of the barrier.
  rc = MUSPI_GIBarrierPollWithTimeout ( &GIBarrier, timeoutCycles);
  if ( rc )
    {
      printf("MUSPI_GIBarrierPollWithTimeout failed returned rc = %d\n", rc);
      DelayTimeBase (200000000000UL);
      alltoall_exit(1);
    }
}



/////////////////////////////////////////////////////////////////////////////
/// Read environment variables to program alltoall
/////////////////////////////////////////////////////////////////////////////
void getEnvVars()
{
  char *envvar;

  envvar = getenv("MSG_ALLTOALL_MESSAGE_SIZE");
  if (envvar)
    {
      messageSizeInBytes = strtoul( envvar, 0, 10 );

      if (  messageSizeInBytes > MAX_MESSAGE_SIZE )  
	messageSizeInBytes = MAX_MESSAGE_SIZE;

      if ( (messageSizeInBytes % SEND_BUFFER_ALIGNMENT) ||
	   (messageSizeInBytes % RECV_BUFFER_ALIGNMENT) )
	{
	  printf("ERROR: MESSAGE_SIZE %lld must be multiple of both SEND %d and RECV %d ALIGNMENT\n",
		 messageSizeInBytes,
		 SEND_BUFFER_ALIGNMENT,
		 RECV_BUFFER_ALIGNMENT);

	  alltoall_exit(1);
	}
    }

  envvar = getenv("MSG_ALLTOALL_CHECK_MESSAGES");
  if (envvar)
    {
      check_messages = strtoul( envvar, 0, 10 );
    }

  envvar = getenv("MSG_ALLTOALL_ZONE_ROUTING_ID");
  if (envvar)
    {
      zoneRoutingId = strtoul( envvar, 0, 10 );
      switch (zoneRoutingId)
	{
	case 0: zoneRoutingMask = MUHWI_PACKET_ZONE_ROUTING_0; break; 
	case 1: zoneRoutingMask = MUHWI_PACKET_ZONE_ROUTING_1; break; 
	case 2: zoneRoutingMask = MUHWI_PACKET_ZONE_ROUTING_2; break; 
	case 3: zoneRoutingMask = MUHWI_PACKET_ZONE_ROUTING_3; break; 
	default:
	  printf("ERROR: MSG_ALLTOALL_ZONE_ROUTING_ID is not in range [0,3]\n");
	}
    }
  
  envvar = getenv("MSG_ALLTOALL_STAY_ON_BUBBLE");
  if (envvar)
    {
      stayOnBubbleFlag = strtoul( envvar, 0, 10 );
      if ( stayOnBubbleFlag ) 
	stayOnBubbleMask = MUHWI_PACKET_STAY_ON_BUBBLE;
    }


  envvar = getenv("MSG_ALLTOALL_DYNAMIC");
  if (envvar)
    {
      do_dynamic = strtoul( envvar, 0, 10 );
    }  
}


int main(int argc, char **argv)
{
    int rc;
    getEnvVars();

    Personality_t pers;
    Kernel_GetPersonality(&pers, sizeof(pers));

    //////////////////////////////////////////////////////
    ///////////  Set up destinations  ////////////////////
    //////////////////////////////////////////////////////

    setup_destinations(&pers);

    if (myRank == 0)
    {      
      printf("main(): Alltoall Performance Test\n");
    }
    
    ///////////////////////////////////////////////////////
    /////////// Set up Injection Fifos  ///////////////////
    ///////////////////////////////////////////////////////
    msg_InjFifoHandle_t injFifoHandle;
    rc = msg_InjFifoInit ( &injFifoHandle,
			   0,        /* startingSubgroupId */
			   0,        /* startingFifoId     */
			   NUM_INJ_FIFOS,       /* numFifos   */
			   INJ_MEMORY_FIFO_SIZE+1, /* fifoSize */
			   NULL      /* Use default attributes */
			 );
    if (rc != 0)
    {
      printf("msg_InjFifoInit failed with rc=%d\n",rc);
      alltoall_exit(1);
    }

    /////////////////////////////////////////////////////////
    ////////  Set up send and recive buffers   //////////////
    /////////////////////////////////////////////////////////
    recvBuffers = (char *)(((uint64_t)recvBufMemory+RECV_BUFFER_ALIGNMENT)&~(RECV_BUFFER_ALIGNMENT-1));    
    sendBuffers = (char *)(((uint64_t)sendBufMemory+SEND_BUFFER_ALIGNMENT)&~(SEND_BUFFER_ALIGNMENT-1));

    if (check_messages)
     {
       uint64_t offset;
       int i;
       for (i=0, offset = 0; 
	    i < numNodes; 
	    i++, offset += messageSizeInBytes)
	 {
	   if ( i == myRank ) continue; // no self send	
	   msg_InitBuffer ( sendBuffers + offset,
			    messageSizeInBytes,
			    (MUHWI_Destination_t *)&myRank, /* sourcePtr */
			    (MUHWI_Destination_t *)&i,      /* destPtr */
			    NULL ,                          /* uniquePtr */
			    NULL                            /* chksumPtr */ );
	 }
     }

    ////////////////////////////////////////////////////////////////////////
    // Set up base address table for reception counter and buffer
    ////////////////////////////////////////////////////////////////////////
    setup_mregions_bats_counters();
   
    ////////////////////////////////////////////////////////////////////////
    // Create descriptors
    ////////////////////////////////////////////////////////////////////////

    // Injection Direct Put Descriptor, one for each neighbor
    muDescriptors =
      ( MUHWI_Descriptor_t *)(((uint64_t)muDescriptorsMemory+64)&~(64-1));
    
    create_descriptors();
    
    // Initialize the barrier, resetting the hardware.
    rc = MUSPI_GIBarrierInit ( &GIBarrier, 0 /*comm world class route */);

    if (rc)
      {
	printf("MUSPI_GIBarrierInit returned rc = %d\n", rc);
	alltoall_exit(__LINE__);
      }

    ///////////////////////////////////////////////////////////////////////////
    //
    // Start the timer.
    // Loop num_iterations times.
    // - Set the reception counter to the message length * numNodes
    // - Loop through each destination, injecting the descriptor.
    // - Wait for receive and send completion
    // Stop the iteration timer.
    //
    //////////////////////////////////////////////////////////////////////////

    uint64_t totalCycles=0;
    uint64_t startTime=0;
    uint64_t totalBytesPerIter     =  messageSizeInBytes * (numNodes - 1);
    uint64_t totalBytes            =  totalBytesPerIter * num_iterations;
    uint64_t totalNumberOfMessages =  (numNodes - 1) * num_iterations;
    uint64_t bytesPer1000Cycles;

    assert (window_size <= messageSizeInBytes);
    if ( myRank == 0 )
      {
	printf("processes:%d, #iterations = %d, Message Size = %llu, Send Buffer Alignment = %u, sendBuffer@ = %p, Recv Buffer Alignment = %u, recvBuffer@=%p,zoneId:%d,stayOnBubble:%d,submessage%d\nPerforming communications...\n", 
	       numNodes, num_iterations, messageSizeInBytes, SEND_BUFFER_ALIGNMENT,sendBuffers,
	       RECV_BUFFER_ALIGNMENT,recvBuffers,zoneRoutingId,stayOnBubbleFlag, window_size);
      }
    
    // create random order for sends 
    create_random_permutation();

    uint64_t descCount[NUM_INJ_FIFOS];

    int i, j;
    for ( i=0; i<num_iterations+skip_iterations; i++) 
      {      
	// reset the recv counter 
	recvCounter = totalBytesPerIter;

	if (i >= skip_iterations) startTime = GetTimeBase();

	global_barrier(); // make sure everybody is set recv counter

	uint64_t bytes = 0;
	for (bytes = 0; bytes < messageSizeInBytes; bytes += window_size) {
	  uint64_t msize = (bytes <= messageSizeInBytes - window_size) ? window_size : (messageSizeInBytes - bytes);
	  for ( j=0; j<numNodes; j++)
	    {
	      if ( myRank == randPerm[j] ) continue; // no self send
	      
	      muDescriptors[randPerm[j]].Message_Length = msize; 
	      muDescriptors[randPerm[j]].Pa_Payload    =  sendBufPAddr + (messageSizeInBytes * randPerm[j]) + bytes;
	      MUSPI_SetRecPutOffset (&muDescriptors[randPerm[j]], myRank * messageSizeInBytes + bytes);

	      descCount[ j % NUM_INJ_FIFOS ] =
		msg_InjFifoInject ( injFifoHandle,
				    j % NUM_INJ_FIFOS,
				    &muDescriptors[randPerm[j]]);
	    }
	}

	// wait for receive completion
	while ( recvCounter > 0 );
	
	// wait for send completion
	unsigned sendDone;
	unsigned nexp = ((numNodes-1) < NUM_INJ_FIFOS) ? (numNodes-1) : NUM_INJ_FIFOS;
	do
	  {
	    sendDone = 0;
	    for ( j = 0; j < NUM_INJ_FIFOS; j++ )
	      sendDone += msg_InjFifoCheckCompletion( injFifoHandle,
						      j,
						      descCount[j]);
	  }
	while ( sendDone < nexp );
	
	_bgq_msync(); // Ensure data is available to all cores.  

	if (i >= skip_iterations) totalCycles += GetTimeBase() - startTime;
      }
    
    if (myRank == 0)
      printf("Cycles = %llu, Number of Messages = %llu, Cycles per Message = %llu, Cycles per alltoall = %llu\n",
	     (long long unsigned int) totalBytes,
	     (long long unsigned int) totalCycles,
	     (long long unsigned int) totalNumberOfMessages,
	     (long long unsigned int) totalCycles/num_iterations);

    if (check_messages)
      {
	uint64_t offset;
	for (i=0, offset = 0; 
	     i < numNodes; 
	     i++, offset += messageSizeInBytes)
	  {
	    if ( i == myRank ) continue; // no self send

	    rc = msg_CheckBuffer ( recvBuffers + offset,
				   messageSizeInBytes,
				   (MUHWI_Destination_t *)&i, /* sourcePtr */
				   (MUHWI_Destination_t *)&myRank, /* destPtr */
				   NULL,   /* uniquePtr */
				   "Checking Receive Buffer" );
	    if ( rc )
	      alltoall_exit(1);
	  }
	
      }

    msg_InjFifoTerm ( injFifoHandle );

    return 0;	
}
 


void setup_mregions_bats_counters(void)
{
  const uint64_t buffersSize =  numNodes * messageSizeInBytes;

  // allocate bat entries for the recive buffer and the receive counter
  
  uint32_t batIds[2] = { recvBufBatId, recvCntrBatId };
  MUSPI_BaseAddressTableSubGroup_t batSubGrp;
  
  int rc =  Kernel_AllocateBaseAddressTable( 0/*subgrpId*/,
					     &batSubGrp,
					     2,/*nbatids*/
					     batIds,
					     0 /* "User" use */);
  
  if ( rc != 0 )
    {
      printf("Kernel_AllocateBaseAddressTable failed with rc=%d\n",rc);
      alltoall_exit(1);
    }
  
  // Receive buffer bat is set to the PA addr of the receive buffer
  Kernel_MemoryRegion_t memRegion;
  rc = Kernel_CreateMemoryRegion ( &memRegion,
				   recvBuffers,
				   buffersSize);
  if ( rc != 0)
    {
      printf("Kernel_CreateMemoryRegion failed with rc=%d\n",rc);
      alltoall_exit(1);
    }
  
  uint64_t paAddr = 
    (uint64_t)recvBuffers - 
    (uint64_t)memRegion.BaseVa + 
    (uint64_t)memRegion.BasePa;
  
  rc = MUSPI_SetBaseAddress ( &batSubGrp,
			      recvBufBatId,
			      paAddr );
  
  if (rc != 0)
    {
      printf("MUSPI_SetBaseAddress failed with rc=%d\n",rc);
      alltoall_exit(1);
    }
  
  // Receive counter bat is set to the MU style atomic PA addr of the receive counter
  if ( (uint64_t)(&recvCounter) & 0x7 )
    {
      printf("ERROR: recv counter is not 8 byte aligned\n");
      alltoall_exit(1);
    }
  
  rc = Kernel_CreateMemoryRegion ( &memRegion,
				   (void *)&recvCounter,
				   sizeof(recvCounter));
  if ( rc != 0)
    {
      printf("Kernel_CreateMemoryRegion failed with rc=%d\n",rc);
      alltoall_exit(1);
    }
  
  paAddr = 
    (uint64_t)&recvCounter - 
    (uint64_t)memRegion.BaseVa + 
    (uint64_t)memRegion.BasePa;
  
  uint64_t paAddrAtomic =  MUSPI_GetAtomicAddress(paAddr,MUHWI_ATOMIC_OPCODE_STORE_ADD);
  
  rc = MUSPI_SetBaseAddress ( &batSubGrp,
			      recvCntrBatId,
			      paAddrAtomic );
  
  if (rc != 0)
    {
      printf("MUSPI_SetBaseAddress failed with rc=%d\n",rc);
      alltoall_exit(1);
    }
  
  // Get the send buffers physical address
  
  rc = Kernel_CreateMemoryRegion ( &memRegion,
				   sendBuffers,
				   buffersSize);
  if ( rc != 0)
    {
      printf("Kernel_CreateMemoryRegion failed with rc=%d\n",rc);
      alltoall_exit(1);
    }
  
  sendBufPAddr = 
    (uint64_t)sendBuffers - 
    (uint64_t)memRegion.BaseVa + 
    (uint64_t)memRegion.BasePa;
  
}

void create_random_permutation(void)
{
  unsigned seedPtr = myRank;
  srandom(myRank);
  
  int i;
  randPerm[0] = 0;
  for ( i = 1; i < numNodes; i++ )
    {
      uint32_t r = random(); 
      // scale r into the range [0,i]
      r = r % (i+1); // TODO: deal with modulo bias ? 
      randPerm[i] = randPerm[r];
      randPerm[r] = i;
    }
}

void create_descriptors()
{
  uint64_t anyFifoMap = 
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AM |
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_AP | 
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BM |
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_BP | 
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CM |
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_CP |
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DM |
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_DP |
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EM |
    MUHWI_DESCRIPTOR_TORUS_FIFO_MAP_EP;
  
  uint64_t r_offset = myRank * messageSizeInBytes;
  int i;
  uint64_t s_offset;
  static int  did_print =0;
  
  for ( i = 0, s_offset = 0; 
	i < numNodes; 
	i++, s_offset += messageSizeInBytes )
    {
      if ( i == myRank ) continue;
      
      // Injection Direct Put Descriptor Information Structure
      MUSPI_Pt2PtDirectPutDescriptorInfo_t dinfo;
      
      memset( (void*)&dinfo, 0x00, sizeof(dinfo) );
      
      dinfo.Base.Payload_Address = sendBufPAddr + s_offset;
      dinfo.Base.Message_Length  = messageSizeInBytes;
      
      dinfo.Base.Torus_FIFO_Map  = anyFifoMap;
      
      dinfo.Base.Dest = rank2dest[i].dest;
      
      dinfo.Pt2Pt.Hints_ABCD = rank2dest[i].hintsABCD; 
      if ( do_dynamic)
      {	  
	dinfo.Pt2Pt.Misc1 =
	  rank2dest[i].hintsE |
	  MUHWI_PACKET_USE_DYNAMIC_ROUTING |  
	  MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE;
	
	dinfo.Pt2Pt.Misc2 = 
	  MUHWI_PACKET_VIRTUAL_CHANNEL_DYNAMIC | 
	  zoneRoutingMask | 
	  stayOnBubbleMask;
	if ( (myRank ==0) && (did_print ==0)) printf(" dyanmic routing  zoneRoutingMask=%d stayOnBubbleMask=%d\n",
						     zoneRoutingMask, stayOnBubbleMask);
      }
      else
      {	    	    
	dinfo.Pt2Pt.Misc1 =
	  rank2dest[i].hintsE |
	  MUHWI_PACKET_USE_DETERMINISTIC_ROUTING |  
	  MUHWI_PACKET_DO_NOT_ROUTE_TO_IO_NODE;
	
	dinfo.Pt2Pt.Misc2 = 
	  MUHWI_PACKET_VIRTUAL_CHANNEL_DETERMINISTIC | 
	  zoneRoutingMask | 
	  stayOnBubbleMask;
	if ( (myRank ==0) && (did_print ==0)) printf(" deterministic routing\n");
      }
      did_print++;
      
      
      dinfo.Pt2Pt.Skip  = 8; // for checksumming, skip the header 	      
      dinfo.DirectPut.Rec_Payload_Base_Address_Id = recvBufBatId;
      dinfo.DirectPut.Rec_Payload_Offset          = r_offset;
      dinfo.DirectPut.Rec_Counter_Base_Address_Id = recvCntrBatId;
      dinfo.DirectPut.Rec_Counter_Offset          = 0;
      
      dinfo.DirectPut.Pacing = MUHWI_PACKET_DIRECT_PUT_IS_NOT_PACED;
      
      int rc = MUSPI_CreatePt2PtDirectPutDescriptor( 
						    &muDescriptors[i],
						    &dinfo );
      if (rc != 0)
	{
	  printf("MUSPI_CreatePt2PtDirectPutDescriptor failed with rc=%d\n",rc);
	  alltoall_exit(1);
	}
      
    } // End: Set up descriptors
}
 
void setup_destinations(Personality_t *pers)
{
  torus_t tcoords = 
    { 
      pers->Network_Config.Acoord,
      pers->Network_Config.Bcoord,
      pers->Network_Config.Ccoord,
      pers->Network_Config.Dcoord,
      pers->Network_Config.Ecoord 
     };
  
  torus_t tdims = 
    { 
      pers->Network_Config.Anodes,
      pers->Network_Config.Bnodes,
      pers->Network_Config.Cnodes,
      pers->Network_Config.Dnodes,
      pers->Network_Config.Enodes 
    };
  
  numNodes = tdims.a * tdims.b * tdims.c * tdims.d * tdims.e;   
  
  // check if numNodes is reasonable  
  if ( numNodes == 1 || numNodes > MAX_NUM_NODES )
    {
      printf("ERROR: # of nodes:%d is either 1 or exceeds MAX_NUM_NODES:%d\n",
	     numNodes,MAX_NUM_NODES);
      alltoall_exit(1);
    }

  if (Kernel_ProcessCount() > 1) {
    printf ("ERROR: this test only works with ppn==1\n");
    alltoall_exit(1);
  }
      
  // set up destinations and hints
  unsigned a,b,c,d,e;
  unsigned a_mult = tdims.b * tdims.c * tdims.d * tdims.e;
  unsigned b_mult = tdims.c * tdims.d * tdims.e;
  unsigned c_mult = tdims.d * tdims.e;
  unsigned d_mult = tdims.e;
  
  for ( a = 0; a < tdims.a; a++ )
    for ( b = 0; b < tdims.b; b++ )
      for ( c = 0; c < tdims.c; c++ )
	for ( d = 0; d < tdims.d; d++ )
	  for ( e = 0; e < tdims.e; e++ )
	    {
	       unsigned rank = 
		 a * a_mult + b * b_mult + c * c_mult + d * d_mult + e;
	       
	       if ( a == tcoords.a && 
		    b == tcoords.b && 
		    c == tcoords.c && 
		    d == tcoords.d && 
		    e == tcoords.e )
		 myRank = rank;
	       
	       MUSPI_SetUpDestination( &rank2dest[rank].dest, a, b, c, d, e );	       	       
	    }
}

