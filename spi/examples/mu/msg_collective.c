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



////////////////////////////////////////////////////////////
/////// collective network performance test ///////////////
////////////////////////////////////////////////////////////

/////////////////////////////////////////
/// Test library common code
////////////////////////////////////////
#include "msg_common.h"

#define LOOPS 1000
#define WARMUP 10
#define REC_MEMORY_FIFO_SIZE          0x000FFFFFULL  // 512K bytes

// Let get a message that requires more than one Torus Packet
#define MID_MESSAGE_SIZE          (32*1024)

// Let get a message that requires more than one Torus Packet
#define MAX_MESSAGE_SIZE          (1024*1024)

#define INJ_MEMORY_FIFO_SIZE  0x00000FFFFULL  // 64K bytes

char injMemoryFifoBuf[INJ_MEMORY_FIFO_SIZE+1]__attribute__((__aligned__(64)));

// Injection Direct Put Descriptor
__attribute((aligned(64))) MUHWI_Descriptor_t muspi_iDirectPutDescriptor;

// Injection Direct Put Descriptor Information Structure
MUSPI_CollectiveDirectPutDescriptorInfo_t mu_iDirectPutDescriptorInfo;

// Injection Mem Fifo Descriptor
__attribute((aligned(64))) MUHWI_Descriptor_t muspi_iMemFifoDescriptor;

// Injection Mem Fifo Descriptor Information Structure
MUSPI_CollectiveMemoryFIFODescriptorInfo_t mu_iMemFifoDescriptorInfo;

typedef struct _recvArea_t {
  char             recvBuffer[MAX_MESSAGE_SIZE];
  uint64_t         counter[2];
} recvArea_t;

MUSPI_InjFifoSubGroup_t   fifo_subgroup;  

int recv_packet (void                       * param,
                 MUHWI_PacketHeader_t       * hdr,
                 uint32_t                     bytes);

/// Collective via remote put operation
void coll_put(int                                  bytes,
	      char                               * src,
	      Kernel_MemoryRegion_t              * mregionSend,
	      recvArea_t                         * recvAreaPtr,
	      Kernel_MemoryRegion_t              * mregionRecv,
	      MUSPI_BaseAddressTableSubGroup_t   * bat,
	      int                                  batid,
	      int                                  op,
	      int                                  sizeoftype,
	      int                                  fifoid,
	      int                                  vc);

/// Collective via memory fifo operation
void coll_fifo(int                                  bytes,
	       char                               * src,
	       Kernel_MemoryRegion_t              * mregionSend,
	       recvArea_t                         * recvAreaPtr,
	       Kernel_MemoryRegion_t              * mregionRecv,
	       int                                  op,
	       int                                  sizeoftype,
	       int                                  fifoid,
	       int                                  vc);

int init_buffer   (char *buffer, int op, int sizeoftype, int bytes);
int check_results (char *buffer, int op, int sizeoftype, int bytes);


#define test_exit exit

// flag is set if we run in loopback mode
unsigned isLoopback = 0;
unsigned isMaster   = 0;

MUSPI_GIBarrier_t GIBarrier;
void global_barrier(void)
{
  int rc = 0;
  uint64_t timeoutCycles = 1600000000; // about 1 sec at 16 mhz
  // TODO : real multi-node barrier 
  if ( ! isLoopback )
  {
    //printf("Delaying until all nodes reach this point\n");
    // Enter the barrier.
    rc = MUSPI_GIBarrierEnter ( &GIBarrier );
    if (rc)
    {
      printf("MUSPI_GIBarrierEnter failed returned rc = %d\n", rc);
      test_exit(1);
    }
    
    //=======================================================================
    
    // Poll for completion of the barrier.
    rc = MUSPI_GIBarrierPollWithTimeout ( &GIBarrier, timeoutCycles);
    if ( rc )
    {
      printf("MUSPI_GIBarrierPollWithTimeout failed returned rc = %d\n", rc);
      test_exit(1);
    }
    //Delay(10000000);
  }
}

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

uint32_t rfifoid;
void *rec_memory_fifo;
MUSPI_RecFifoSubGroup_t   rfifo_subgroup;  
uint32_t dispatchId;
MsgContext_t  context;

int main(int argc, char **argv)
{
  int rc;

  Personality_t pers;
  Kernel_GetPersonality(&pers, sizeof(pers)); 

  if ( pers.Network_Config.Acoord == 0 &&
       pers.Network_Config.Bcoord == 0 &&
       pers.Network_Config.Ccoord == 0 &&
       pers.Network_Config.Dcoord == 0 &&
       pers.Network_Config.Ecoord == 0 )
    isMaster = 1;

  // Allocate space for the reception counter and the receive buffer    
  recvArea_t *recvAreaPtr = (recvArea_t *)malloc ( sizeof(recvArea_t) );    
  //Get a memory region for the recvArea
  Kernel_MemoryRegion_t  recvAreaMemRegion;
  rc = Kernel_CreateMemoryRegion ( &recvAreaMemRegion, recvAreaPtr, sizeof(recvArea_t) );
  
  if ( rc != 0)
  {
    printf("Kernel_CreateMemoryRegion failed for recvAreaMemRegion rc=%d\n",
	   rc);
    test_exit(1);
  }

  char *messageBuf = malloc ( MAX_MESSAGE_SIZE );
  Kernel_MemoryRegion_t  mregionSend;
  rc=Kernel_CreateMemoryRegion (&mregionSend, messageBuf, MAX_MESSAGE_SIZE);
  
  if ( rc != 0)
  {
    printf("Kernel_CreateMemoryRegion failed for messageBuf with rc=%d\n",
	   rc);
    test_exit(1);
  }

  int networkvc = MUHWI_PACKET_VIRTUAL_CHANNEL_USER_SUB_COMM; //MUHWI_PACKET_VIRTUAL_CHANNEL_USER_COMM_WORLD; //

  // Initialize the barrier, resetting the hardware.
  rc = MUSPI_GIBarrierInit ( &GIBarrier, 0 /* classRouteId */ );
  if (rc)
    {
      printf("MUSPI_GIBarrierInit for class route %u returned rc = %d\n", 0, rc);
      test_exit(__LINE__);
    }

  //Allocate 2 fifos, one for user and one for system
  uint32_t fifoid = 0;  
  Kernel_InjFifoAttributes_t injFifoAttrs;
  injFifoAttrs.RemoteGet = 0;
  injFifoAttrs.System    = 0;

  Kernel_AllocateInjFifos (0, &fifo_subgroup, 1, &fifoid, &injFifoAttrs);
  
  Kernel_MemoryRegion_t  mregionInj;
  rc = Kernel_CreateMemoryRegion ( &mregionInj, injMemoryFifoBuf, 
				   sizeof(injMemoryFifoBuf) );

  dispatchId = MUSPI_RegisterReceiveFunction (recv_packet, &context);    

  if ( rc != 0)
  {
    printf("Kernel_CreateMemoryRegion failed for injMemoryFifoBuf rc=%d\n",
	   rc);
    test_exit(1);
  }
  
  Kernel_InjFifoInit (&fifo_subgroup, fifoid, &mregionInj,
		      (uint64_t)injMemoryFifoBuf - (uint64_t)mregionInj.BaseVa,
		      INJ_MEMORY_FIFO_SIZE);    
  
  Kernel_InjFifoActivate (&fifo_subgroup, 1, &fifoid, KERNEL_INJ_FIFO_ACTIVATE); 
  
  // allocates areas for reception and injection fifos
  rec_memory_fifo = malloc (REC_MEMORY_FIFO_SIZE + 1);
  rfifoid = 0;
    
  Kernel_MemoryRegion_t  mregionRec;
  Kernel_CreateMemoryRegion ( &mregionRec, rec_memory_fifo, (REC_MEMORY_FIFO_SIZE + 1) );
  
  Kernel_RecFifoAttributes_t recFifoAttrs;
  recFifoAttrs.System = 0;
  recFifoAttrs.System = 0;  
  uint64_t recFifoEnableBits=0;

  //printf("main(): allocate reception FIFO\n");
  Kernel_AllocateRecFifos (0, &rfifo_subgroup, 1, &rfifoid, &recFifoAttrs);

  //printf("main(): init reception FIFO\n");
  Kernel_RecFifoInit    (& rfifo_subgroup, 0, & mregionRec, 
			 (uint64_t)rec_memory_fifo - (uint64_t)mregionRec.BaseVa,
			 REC_MEMORY_FIFO_SIZE);
  
  recFifoEnableBits |= ( 0x0000000000000001ULL <<
			 ( 15 - ( (0/*sgid*/*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) + 0 )) );
  
  Kernel_RecFifoEnable ( 0, /* Group ID */
			 recFifoEnableBits );  

  uint32_t batids[1] = {0};
  MUSPI_BaseAddressTableSubGroup_t bat;
  rc = Kernel_AllocateBaseAddressTable ( 0, &bat, 1, batids, 0 /* "User" use */ );
  if (rc != 0)
  {
    printf("Kernel_AllocateBaseAddressTable failed with rc=%d\n",rc);
    test_exit(1);
  }

  rc = MUSPI_SetBaseAddress(&bat, 0, (uint64_t)recvAreaMemRegion.BasePa);
  if (rc != 0)
  {
    printf("MUSPI_SetBaseAddress failed with rc=%d\n",rc);
    test_exit(1);
  }
  
  unsigned bytes = 0;

  if (isMaster) {
    printf ("Allreduce Performance\n");
    printf("Type\tOperation\t\tbytes\t\tcycles\t\n"); 
  }

  unsigned fifo = 0;

  coll_put (8, messageBuf, &mregionSend, 
	    recvAreaPtr, &recvAreaMemRegion, &bat, batids[0], 
	    MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD, sizeof(uint64_t), 
	    fifo, networkvc);
  
  coll_put (8, messageBuf, &mregionSend, 
	    recvAreaPtr, &recvAreaMemRegion, &bat, batids[0], 
	    MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD, sizeof(double), 
	    fifo, networkvc);
  
  coll_fifo (8, messageBuf, &mregionSend, 
	     recvAreaPtr, &recvAreaMemRegion, 
	     MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD, sizeof(uint64_t), 
	     fifo, networkvc);
  
  coll_fifo (8, messageBuf, &mregionSend, 
	     recvAreaPtr, &recvAreaMemRegion, 
	     MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD, sizeof(double), 
	     fifo, networkvc);

  for (bytes = MID_MESSAGE_SIZE; bytes <= MAX_MESSAGE_SIZE; bytes*= 2) {
    coll_put (bytes, messageBuf, &mregionSend, 
	      recvAreaPtr, &recvAreaMemRegion, &bat, batids[0], 
	      MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD, sizeof(uint64_t), 
	      fifo, networkvc);
    
    coll_put (bytes, messageBuf, &mregionSend, 
	      recvAreaPtr, &recvAreaMemRegion, &bat, batids[0], 
	      MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD, sizeof(double), 
	      fifo, networkvc);
    
    coll_fifo (bytes, messageBuf, &mregionSend, 
	       recvAreaPtr, &recvAreaMemRegion, 
	       MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD, sizeof(uint64_t), 
	       fifo, networkvc);
    
    coll_fifo (bytes, messageBuf, &mregionSend, 
	       recvAreaPtr, &recvAreaMemRegion, 
	       MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD, sizeof(double), 
	       fifo, networkvc);
  }

  test_exit (0);
  
  return 0;	
}
 

void coll_put(int                                  bytes,
	      char                               * src,
	      Kernel_MemoryRegion_t              * mregionSend,
	      recvArea_t                         * recvAreaPtr,
	      Kernel_MemoryRegion_t              * mregionRecv,
	      MUSPI_BaseAddressTableSubGroup_t   * bat,
	      int                                  batid,
	      int                                  op,
	      int                                  sizeoftype,
	      int                                  fifoid,
	      int                                  vc)
{
  MUHWI_Destination_t dest;
  MUSPI_SetUpDestination ( &dest, 0, 0, 0, 0, 0 );
  int rc;
  int i = 0;

  volatile uint64_t *counterAddress[2];
  counterAddress[0]  = (volatile uint64_t*)&(recvAreaPtr->counter[0]);
  counterAddress[1]  = (volatile uint64_t*)&(recvAreaPtr->counter[1]);
  *counterAddress[0] = bytes;
  *counterAddress[1] = bytes;

  unsigned char   *recvBufferAddress = (unsigned char  *)&(recvAreaPtr->recvBuffer[0]);

  //Calculate the offsets of the counter and receive buffer from the
  //base address.
  uint64_t recvAreaBasePA = (uint64_t)mregionRecv->BasePa;
  uint64_t counterOffset[2];
  counterOffset[0]=(uint64_t)counterAddress[0]-(uint64_t)mregionRecv->BaseVa;
  counterOffset[1]=(uint64_t)counterAddress[1]-(uint64_t)mregionRecv->BaseVa;

  uint64_t recvBufferOffset=(uint64_t)recvBufferAddress-(uint64_t)mregionRecv->BaseVa;
  
  // Allocate area for messageBuf[] buffer 
  // Set up the base address table
  
  uint64_t muAtomicCounterOffset[2];
 
  muAtomicCounterOffset[0] = 
    MUSPI_GetAtomicOffsetFromBaseAddress (bat, 
					  batid, 
					  recvAreaBasePA + counterOffset[0],
					  MUHWI_ATOMIC_OPCODE_STORE_ADD); 
  
  muAtomicCounterOffset[1] = 
    MUSPI_GetAtomicOffsetFromBaseAddress (bat, 
					  batid, 
					  recvAreaBasePA + counterOffset[1],
					  MUHWI_ATOMIC_OPCODE_STORE_ADD);   
  
  memset( (void*)&mu_iDirectPutDescriptorInfo, 0x00, sizeof(mu_iDirectPutDescriptorInfo) );
  
  msg_BuildCollectiveDirectPutAllreduceInfo(& mu_iDirectPutDescriptorInfo,
					    bytes,
					    (uint64_t)src - 
					    (uint64_t)mregionSend->BaseVa + 
					    (uint64_t)mregionSend->BasePa,
					    op,
					    sizeoftype,
					    dest,
					    recvBufferOffset,
					    muAtomicCounterOffset[0],
					    0,
					    vc);
  
  rc = MUSPI_CreateCollectiveDirectPutDescriptor
    (&muspi_iDirectPutDescriptor, 
     &mu_iDirectPutDescriptorInfo);

  if (rc != 0)
  {
    printf("MUSPI_CreateCollectiveDirectPutDescriptor failed with rc=%d\n",
	   rc);
    test_exit(1);
  }
  
  //  if (isMaster)
  //MUSPI_DescriptorDumpHex("Direct Put Descriptor",
  //			  &muspi_iDirectPutDescriptor);

  init_buffer (src, op, sizeoftype, bytes);

  global_barrier();

  uint64_t total_time = 0;
  uint64_t min_time = ~0UL;
  uint64_t max_time = 0UL;

  for (i = 0; i < LOOPS+WARMUP; i++) {
    uint64_t prev = GetTimeBase(); 
    *counterAddress[(i+1)&0x1] = bytes;    
    muspi_iDirectPutDescriptor.PacketHeader.messageUnitHeader.Packet_Types.Direct_Put.Counter_Offset = muAtomicCounterOffset[i&0x1];

    MUSPI_InjFifoInject (MUSPI_IdToInjFifo(fifoid, &fifo_subgroup), &muspi_iDirectPutDescriptor);
    
    // wait for the counter to reach ZERO
    while (*counterAddress[i&0x1] != 0);
    _bgq_msync();
    if (i >= WARMUP) {
      uint64_t elapsed = GetTimeBase() - prev;
      if (elapsed < min_time) min_time = elapsed;
      if (elapsed > max_time) max_time = elapsed;
      total_time += elapsed;
    }
  }
  
  rc = check_results (recvAreaPtr->recvBuffer, 
		      op, sizeoftype, bytes);
  
  if (rc != 0){
    printf ("Test failed for op %d sizeoftype %d size %d!!!\n", 
	    op, sizeoftype, bytes);
  }

  char *opname;
  if (op == MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD)
    opname = "FP64  Add";
  else
    opname = "INT64 Add";      

  if (isMaster)
    printf("%s\t%s\t\t%d\t\t%lu\n", 
	   "RPUT",opname,  bytes, 
	   total_time/LOOPS
	   );
}


int init_buffer   (char *buffer, int op, int sizeoftype, int bytes) {
  int i = 0;
  double   *d;
  uint32_t *u32;
  uint64_t *u64;

  if (op == MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD) {
    d = (double *)buffer;
    for (i = 0; i < (bytes/sizeoftype); i++)
      d[i] = (double)i;
  }
  else if ( op == MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD && 
	    sizeoftype == sizeof(uint64_t) ) {
    u64 = (uint64_t *)buffer;
    for (i = 0; i < (bytes/sizeoftype); i++)
      u64[i] = (uint64_t)i;
  }
  else if ( op == MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD && 
	    sizeoftype == sizeof(uint32_t) ) {
    u32 = (uint32_t *)buffer;
    for (i = 0; i < (bytes/sizeoftype); i++)
      u32[i] = (uint32_t)i;
  }

  return 0;
}

int check_results (char *buffer, int op, int sizeoftype, int bytes) {
  Personality_t *pers, spers;
  Kernel_GetPersonality(&spers, sizeof(spers)); 
  pers = &spers;

  int i = 0;
  double *d;
  uint32_t *u32;
  uint64_t *u64;
  
  int nodes = pers->Network_Config.Anodes * 
    pers->Network_Config.Bnodes * 
    pers->Network_Config.Cnodes * 
    pers->Network_Config.Dnodes * 
    pers->Network_Config.Enodes;    

  if (op == MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD) {
    d = (double *)buffer;
    for (i = 0; i < (bytes/sizeoftype); i++) {
      if (d[i] != (double)(i * nodes))
	return -1;
    }
  }
  else if ( op == MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD && 
	    sizeoftype == sizeof(uint64_t) ) {
    u64 = (uint64_t *)buffer;
    for (i = 0; i < (bytes/sizeoftype); i++)
      if (u64[i] != (uint64_t)(i * nodes)){
	printf ("Allreduce error expect %lu, got %lu\n", 
		(uint64_t)(i*nodes), u64[i]);
	return -1;
      }
  }
  else if ( op == MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD && 
	    sizeoftype == sizeof(uint32_t) ) {
    u32 = (uint32_t *)buffer;
    for (i = 0; i < (bytes/sizeoftype); i++)
      if (u32[i] != (i * nodes))
	return -1;
  }
  
  return 0;
}
		   

int recv_packet (void                       * param,
                 MUHWI_PacketHeader_t       * hdr,
                 uint32_t                     bytes) 
{
  SoftwareBytes_t *sw_hdr = (SoftwareBytes_t *)(&hdr->messageUnitHeader.Packet_Types.Memory_FIFO);
  MsgContext_t *context   = (MsgContext_t *) param;
  context->HWEndTime      = GetTimeBase();
  context->size = sw_hdr->BytesStruct.message_size_in_bytes;
  bytes -= 32; /* get the payload bytes */

  //MU can pad message with zeros
  if (context->bytes_recvd + bytes > context->size) 
    bytes = context->size - context->bytes_recvd;
  memcpy(context->buf + context->bytes_recvd, (char *)hdr + 32, bytes);
  context->bytes_recvd += bytes;
  
  if ( context->bytes_recvd >= context->size )
    context->done = 1;

  return 0;
}

void coll_fifo(int                                  bytes,
	       char                               * src,
	       Kernel_MemoryRegion_t              * mregionSend,
	       recvArea_t                         * recvAreaPtr,
	       Kernel_MemoryRegion_t              * mregionRecv,
	       int                                  op,
	       int                                  sizeoftype,
	       int                                  fifoid,
	       int                                  vc) 

{  
  context.buf  = recvAreaPtr->recvBuffer;
  context.size = bytes;
  context.done = 0;
  context.bytes_recvd = 0;

  SoftwareBytes_t SoftwareBytes;  
  SoftwareBytes.BytesStruct.functionid = dispatchId;
  SoftwareBytes.BytesStruct.message_size_in_bytes = bytes;

  MUHWI_Destination_t dest;
  MUSPI_SetUpDestination ( &dest, 0, 0, 0, 0, 0 );

  msg_BuildCollectiveMemoryFifoAllreduceInfo(& mu_iMemFifoDescriptorInfo,
					     SoftwareBytes,
					     bytes,
					     (uint64_t)src - 
					     (uint64_t)mregionSend->BaseVa + 
					     (uint64_t)mregionSend->BasePa,
					     op,
					     sizeoftype,
					     dest,
					     0, //recvBufferOffset,
					     rfifoid,
					     0,
					     vc);
  
  int rc = MUSPI_CreateCollectiveMemoryFIFODescriptor
    (&muspi_iMemFifoDescriptor, 
     &mu_iMemFifoDescriptorInfo);

  if (rc != 0)
  {
    printf("MUSPI_CreateCollectiveMemoryFIFODescriptor failed with rc=%d\n",
	   rc);
    test_exit(1);
  }
  
  //  if (isMaster)
  //MUSPI_DescriptorDumpHex("Mem Fifo Descriptor",
  //			    &muspi_iMemFifoDescriptor);
  
  init_buffer (src, op, sizeoftype, bytes);

  //Delay (1000);
  global_barrier();
  //printf("After Global Barrier\n");

  //control system barrier call
  //Timed run  
  uint64_t HWStartTime, HWTotalTime = 0;
  uint64_t start_time = GetTimeBase();  

  int i = 0;
  for (i = 0; i < LOOPS + WARMUP; i++) {
    if (i == WARMUP)
      start_time = GetTimeBase();  
    
    MUSPI_InjFifoInject (MUSPI_IdToInjFifo(fifoid, &fifo_subgroup), &muspi_iMemFifoDescriptor);
    
    HWStartTime = GetTimeBase();

    while (!context.done)
      MUSPI_RecFifoPoll (MUSPI_IdToRecFifo (rfifoid, &rfifo_subgroup), 
			 1000);
    
    HWTotalTime += (context.HWEndTime - HWStartTime);
    context.done = 0;
    context.bytes_recvd = 0;
  }
  
  uint64_t end_time = GetTimeBase();   
  
  rc = check_results (recvAreaPtr->recvBuffer, 
		      op, sizeoftype, bytes);
  
  if (rc != 0){
    printf ("Test failed for op %d sizeoftype %d size %d!!!\n", 
	    op, sizeoftype, bytes);
  }

  char *opname;
  if (op == MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD)
    opname = "FP64  Add";
  else
    opname = "INT64 Add";      

  if (isMaster)
    printf("%s\t%s\t\t%d\t\t%ld\n", 
	   "FIFO", opname, bytes, (end_time - start_time)/LOOPS);
}

