/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

//! \file  common.h 
//! \brief Declaration of common interernal MUDM structures
//! \remarks Includes MUDM context and connection context structures


#ifndef	_MUDM_COMMON_H_  /* Prevent multiple inclusion */
#define	_MUDM_COMMON_H_

#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>
#include <mudm/include/mudm_atomic.h>
#include <mudm/include/mudm.h>
#include <mudm/include/mudm_bcast.h>
#include "mudm_lock.h"
#include "mudm_cleanup.h"
#include <spi/include/kernel/trace.h>


int dumphex(unsigned char *dmpPtr, int len);

struct mudm_disconnect_hdr {
                   uint16_t  dest_context_index;    //This is the index to the destination connection context
                   uint32_t  torus_source_node;  // source torus node.  High bit on for IO node 
                   uint32_t  torus_dest_node;    //destination torus node.  High bit on for IO node                    
                   uint16_t  lowbytes_timestamp; //Lower 16 bits of destination connection context timestamp
                   uint16_t  type;               // Identifies the payload 
                   uint16_t source_qpn;          //source QP number 
                   uint16_t dest_qpn;            //destination QP number--if unknown use UNKNOWN_QPN  
}__attribute__((__packed__)) ;

// Init software bytes for the packet header.
typedef union _swb
{
  uint8_t bytes[18];
  struct ionet_header ionet_hdr; 
  struct ionet_pkt_header ionet_pkt;
  struct ionet_pkt_header bcast_pkt;
  struct mudm_disconnect_hdr disc_hdr;
} SoftwareBytes_t;

// define state values for bcast
#define BCAST_INACTIVE   0
#define BCAST_SETUP      1
#define BCAST_RECEIVER   2
#define BCAST_ATOMIC     3
#define BCAST_REDUCEWAIT 3
#define BCAST_SENTRDMA   4
#define BCAST_LASTCHECK  5
#define BCAST_WAIT4CLEAR 6
// define types on packets for MUDM use
#define MUDM_DPUT            0x5544 //type reserved for mudm
#define MUDM_REDUCE_ONE      0xF501 //Reduce to a target, MUDM internal
#define MUDM_REDUCE_ALL_INT  0xF50E //Reduce result to all participants, MUDM internal, number of nodes
#define MUDM_REDUCE_ALL_LNC  0xF51F //Reduce result to all participants, MUDM internal, largest node corner
#define MUDM_REDUCE_BCAST_ORIGIN 0xF5B0 //REDUCE directed to origin of RDMA broadcast, MUDM internal
#define MUDM_REDUCE_BCAST_FINISH 0xF5BF //REDUCE directed to all to complete RDMA broadcast, MUDM internal
#define MUDM_RDY_RDMA_BCAST  0xF588 //MUDM FIFO Broadcast to setup RDMA broadcast on a class route, MUDM internal
#define MUDM_CLR_RDMA_BCAST  0xF544 //MUDM FIFO Broadcast to setup RDMA broadcast on a class route, MUDM internal
#define MUDM_CNK2CNK_FIFO    0x1AA1 //MUDM FIFO CNK to CNK, MUDM internal
#define MUDM_PKT_ERROR       0xE000 //General packet error
#define MUDM_PKT_ENOTCONN    0xE06B // Not connected ENOTCONN
#define MUDM_PKT_EILSEQ      0xE054 // Bad sequence number, EILSEQ illegal sequence of bytes

#define CHECK_CONN_MAGIC(_cc_) \
if ( ((struct mudm_connection *)_cc_)-> magic != CONN_MAGIC){\
       MPRINT("\nConnection context had BAD Magic at %s FUNCTION %s  \n",__FILE__,__FUNCTION__);\
       MPRINT("_cc_=%p \n",(void *)_cc_);\
       MPRINT("BAD_MAGIC=%llx \n", (long long unsigned int)( ((struct mudm_connection *)_cc_)->magic) );\
       MDUMPHEXSTR("_cc_ firstbytes hexdump",(void *)_cc_,sizeof(struct mudm_connection) ); \
       return -EFAULT; \
}

#if 1


#define SYS_GROUPID      16    /* 17th group */
#define SYS_BATID        528  /* 66 * 8 + 0 since 8 BATs per subgroup */
#define SYS_BATID2       536  /* 67 * 8 + 0 since 8 BATs per subgroup, last subgroup */
#define MCONTEXT_INDEX   (BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP-1)
#define RDMA_COUNTER_INDEX   (MCONTEXT_INDEX-1)
#define MCONTEXT_INDEX_VA    (MCONTEXT_INDEX-2)
#define SYS_BATID_BCAST_CTR       SYS_BATID+RDMA_COUNTER_INDEX // For broadcast directput counter base address
#define SYS_BATID_BCAST_CTR2      SYS_BATID2+RDMA_COUNTER_INDEX // For broadcast directput counter base address
#define SYS_BATID_MCONTEXT        SYS_BATID+MCONTEXT_INDEX 
#define SYS_BATID_MCONTEXT2       SYS_BATID2+MCONTEXT_INDEX


#endif 


#define TORUS_MAX_PAYLOAD_SIZE      512
#define TORUS_PACKET_SIZE           544

#define LLUS long long unsigned int

#ifdef  __LINUX__
#define NUM_IO_LINKS 2
#else
#define NUM_IO_LINKS 1
#endif

#include "mudm_descriptor.h"
/*
struct mudm_rdma_bcast {  
   void* requestID; // call status callback request ID      
   void * source_payload_paddr;
   void * dest_payload_paddr;
   uint64_t payload_length;
   uint32_t num_in_class_route;
   uint8_t class_route;
   uint8_t reserved[3];
}; 
*/

struct rdma_bcast_set_counter {  
   struct mudm_rdma_bcast mrb; 
   uint64_t mu_counter_offset;  //this is the offset from the base table entry value (mcontext physical address) on every compute node
   MUHWI_Destination_t origination_node;
}__attribute__ ((aligned (64)));

struct bcast_rdma_object {
  struct mu_element * mu_counter_bcast_addr;        // counter target for directput descriptors--polled until 0 on all nodes

  uint64_t bcast_rdma_object_pa;    /* physical address of this object */ 
  volatile uint32_t state;    /* see #define values in common.h, BCAST_xxxx  */
  __attribute((aligned(64))) MUHWI_Descriptor_t  mu_iDirectPutDescriptor; //for RDMA write broadcast
  struct rdma_bcast_set_counter bcast_counter_info;  //local copy
  struct directput_atomic_controls dpatc;
  uint64_t local_status_address;
  uint64_t local_status_value;
  uint64_t num_compute_nodes_in_class_route;
  MUHWI_Destination_t origin_bcast_node;
  uint8_t  class_route4bcast;
 };

struct mudm_bcast {/* for bcast_context */
  //__attribute((aligned(64))) Pt2PtMemoryFIFODescriptorInfo_t       mu_iMemoryFifoDescriptorInfo; /* modified the MUSPI version */
  __attribute((aligned(64))) MUHWI_Descriptor_t  mu_iMemoryFifoDescriptor;// for broadcast, reduce, all-reduce collectives 
  struct bcast_rdma_object RDMA_bcast_control;
  MUSPI_InjFifo_t * IdToInjFifo; /* using Injection Fifo Pointer */
  Lock_Atomic_t   * inj_fifo_lock; /* lock on inj fifo being used */
  void * maincontext; /* points to mudm_context */ 
  uint64_t memfifo_sent;
  uint64_t send_count;
};

struct MU_subgroup_memory {
   MUSPI_InjFifoSubGroup_t   ififo_subgroup; /*injection FIFO subgroup */
   MUSPI_RecFifoSubGroup_t   rfifo_subgroup; /*receive   FIFO subgroup */

   MUSPI_BaseAddressTableSubGroup_t bat; /* base address table subgroup allocation */
   uint32_t batids[BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP];
   MUHWI_BaseAddress_t batvalues[BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP];
   uint32_t rfifoids;  //receive fifo IDs 
   uint32_t ififoid; 
   uint32_t subgroupID;
   uint32_t WU_ArmMU_subgroup_thread_mask;
   uint64_t clearInterrupts_mask; //for clearing recfifo and injfifo interrupts
   //Lock_Atomic_t    io_inj_memory_fifo_lock;  
   struct mudm_memory_region injRegion;
   struct mudm_memory_region recRegion;
   struct mudm_memory_region injrgetRegion;//system rget emulation directput injection
   struct mudm_memory_region recrgetRegion;//system receive emulation rget memfifo packets
   uint32_t wakeupActive; //nonzero if wakeup is to be enabled
} __attribute__ ((aligned (64)));


struct mudm_compare_address_block{
  uint64_t pa_start;
  uint64_t mask;
};

struct mu_element {
  volatile uint64_t mu_counter;        /* counter target for directput descriptors (do not move)*/
  volatile uint64_t error;             /* error if nonzero */
};

struct mudm_connect_payload{
  uint16_t source_conn_index; 
  uint16_t dest_conn_index; 
  uint8_t version;  
  uint8_t using_IO_port;
  uint16_t rec_FIFO_Id;

  uint32_t source_BlockID; 
  MUHWI_Destination_t myBridgeTorusAddress;

  uint64_t sourceUCI;

  uint64_t timestamp; // unique connection identifer correlator
};


#define IONODE2CN 0
#define CN2IONODE 1
#define CN2CN     2
#define ION2ION   3
#define NODE_LOOPBACK 4
#define NODE2NODE 5
#define UNKNOWN   (uint8_t)-1

#define CONN_MAGIC 0xC0C0CCCC01010101ull
#define CONN_FREE 0
#define CONN_PENDING 1
#define CONN_ACTIVE  2
#define CONN_DISCONNECTING -1
#define CONN_ABORTED -2
#define CONN_ORPHANED -3
struct mudm_connection {/* for conn_context */
  //__attribute((aligned(64))) Pt2PtMemoryFIFODescriptorInfo_t       mu_iMemoryFifoDescriptorInfo; /* modified the MUSPI version */
  __attribute((aligned(64))) MUHWI_Descriptor_t  mu_iMemoryFifoDescriptor;
  __attribute((aligned(64))) MUHWI_Descriptor_t  mu_iDirectPutDescriptor;
  __attribute((aligned(64))) MUHWI_Descriptor_t  mu_iDirectPutDescriptor4rget;
  int64_t magic;

  // Keep these together for flight recorder
  volatile int16_t state;
  uint16_t my_index;
  uint16_t remote_rec_FIFO_Id;
  uint16_t local_rec_FIFO_Id;
 
  uint32_t remote_BlockID;
  MUHWI_Destination_t destination;         /* remote destination    */

  MUHWI_Destination_t reverse_destination; /* reverse destination for directput on remoteget  */
  MUHWI_Destination_t remote_bridgingCNnode; /* for remote CN, remote CN uses a bridgingCNnode */

  uint16_t dest_conn_index;
  uint8_t  io_port;                        /* if IO node, use an IO port to CN */ 
  uint8_t  reverse_io_port;                /* if IO node, use an IO port to CN */ 
  uint8_t  direction;                      /* IONODE2CN, CN2ION, CN2CN, ION2ION */
  uint8_t  reverse_direction;              /* IONODE2CN, CN2ION, CN2CN, ION2ION */
  uint16_t lowbit_timestamp;               /* for key matching on disconnect    */
  //End of "Keep these together for flight recorder"

  uint64_t my_physaddr;

  
  MUDM_InjFifo_t  * injfifo_ctls;
  void * maincontext; /* points to mudm_context */
  struct mudm_connection * nextconn;    /* next in list */
  struct mudm_connection * prevconn;    /* previous in list */
  struct mudm_connection ** conn_activelist; // the connection uses this active list on mudm main context
  struct mudm_connection ** conn_pendlist; // the connection uses this pending list on mudm main context
  struct mudm_connection ** conn_abortlist; // the connection uses this abort list on mudm main context
  
  uint64_t remote_timestamp;
  uint64_t my_timestamp;
  
  uint64_t memfifo_sent;
  uint64_t rget_memfifo_sent;
  

  BG_FlightRecorderRegistry_t * flight_recorder;
  struct pkt_controls * packetcontrols;
  struct pkt_controls * smallpa_obj_ctls;
  struct rdma_obj_controls * rdma_obj_ctls;

};


#define MUDM_ACTIVE       1
#define MUDM_TERMINATING  0
#define MUDM_MAGIC 0xDDDDDDDD20202020ull
struct my_context {
  uint64_t magic;
  uint64_t num_compute_nodes;
  int64_t state;

  struct mudm_memory_region mmregion;
  uint64_t mudm_context_phys_addr;
  uint64_t rdma_EINVAL_source;
  uint64_t rdma_GOOD_source;

  BG_FlightRecorderRegistry_t mudm_hi_wrap_flight_recorder __attribute__ ((aligned (64)));
  BG_FlightRecorderRegistry_t mudm_no_wrap_flight_recorder __attribute__ ((aligned (64)));


  uint8_t my_using_IO_port;  /* if CN, IONODE will use this IO_port in IONODE to CN direction */
  
  mudm_recv recv; //See callback section for type definition
  mudm_recv_conn recv_conn; //See callback section for type definition
  mudm_status status;  //See callback section for type definition
  mudm_allocate allocate; //See callback section for type definition
  mudm_free free; //See callback section for type definition
  void * callback_context; /* context for called on callback */


  struct mudm_memory_region rdma_mregion[NUM_IO_LINKS];
  struct rdma_sections * RDMA_sections[NUM_IO_LINKS];
  uint64_t * compare_area[NUM_IO_LINKS];
  struct rdma_obj_controls * rdma_obj_ctls[NUM_IO_LINKS]; 


  void * personality;
  MUHWI_Destination_t myTorusAddress;
  MUHWI_Destination_t mybridgingCNnode;

  struct MU_subgroup_memory * MU_subgroup_mem[NUM_IO_LINKS];
  struct mudm_memory_region MU_subgroup_memregion[NUM_IO_LINKS];

  struct mudm_memory_region conn_list_mregion;
  
  struct mudm_bcast system_bcast;  
  __attribute((aligned(64))) MUHWI_Descriptor_t  mu_iMemoryFifoDescriptor;
  uint32_t myBlockID;
  uint32_t max_conn;
//UCI section
  uint64_t myUCI;
  uint64_t myBridgeUCI;
//Time trackers
  uint64_t start_clock;

  struct remoteget_atomic_controls rgatc;

  uint64_t scratch_area[8] __attribute__ ((aligned (64)));  //scratch area for testing--e.g., atomic and bcast operations
  uint64_t scratch_area2[8] __attribute__ ((aligned (64)));  //scratch area for testing--e.g., atomic and bcast operations
  volatile uint64_t dummy_counter;  //Dummy counter for mudm_rdma_write_on_torus
  
  uint64_t largest_node_corner; //in least significant 32 bits
  //! \todo TODO set base table for counter to physical address of my_context and then use offsets into the struct of test_counter and for bcast_rdma
  volatile uint64_t test_counter;    
    
  volatile uint64_t bcast_reduceall_complete;
  char*    bcast_reduceall_data;
  size_t   bcast_reduceall_datasize;
    
  uint64_t num_nodes_this_node_bridges;   
    
  uint64_t max_connections;
  uint64_t poll_active;
  Lock_Atomic_t conn_list_lock;/* lock conn_list */
  struct mudm_connection * conn_freelist; /* connection context free list */
  
  struct mudm_connection * conn_pendlist[NUM_IO_LINKS];  /* connection started and waiting for acceptance */
  struct mudm_connection * conn_activelist[NUM_IO_LINKS];  /* connection active */
  struct mudm_connection * conn_abortlist[NUM_IO_LINKS];  /* connection pending but aborting  */
  

  struct mudm_memory_region packet_controls_region[NUM_IO_LINKS];
  struct pkt_controls  * packetcontrols[NUM_IO_LINKS];  /* Use for mudm constructed packets w/ virtual and physical addressability */ 
  struct mudm_memory_region  smallpa_obj_ctls_mregion[NUM_IO_LINKS];
  struct pkt_controls * smallpa_obj_ctls[NUM_IO_LINKS]; /* use packet controls for small memory pieces */
  struct mudm_compare_address_block memblock[NUM_IO_LINKS]; 
 
  /* using Injection Fifo Pointer--second IO link (port 6 or 7) */
  //MUSPI_InjFifo_t * IdToInjFifo[NUM_IO_LINKS];  /* using Injection Fifo Pointer on primary IO link            */ 
  MUDM_InjFifo_t  injfifo_ctls[NUM_IO_LINKS];

  uint32_t sys_rec_fifo_id[NUM_IO_LINKS];
  uint32_t remote_BlockID[NUM_IO_LINKS];  //last reported block id on the link
  uint64_t link4reset[NUM_IO_LINKS];
  uint64_t rget_memfifo_sent;
  uint32_t wakeupActive;
  uint32_t StuckState;
  uint64_t RAS_pacing_timestamp;
  uint64_t RAS_pacing_count;


 

} __attribute__ ((aligned (64)));

uint64_t getUCI(void * personality);
uint32_t get_BlockID(void * personality);
int pers_enable_loopback(void * personality);
uint32_t get_my_bridging_torus_addr(void * personality);
uint32_t get_my_node_addr(void * personality);
uint32_t get_my_torus_addr(void * personality);

uint64_t microsec2cycles(void * personality,uint64_t microseconds);

int conn_list_init( struct my_context * mcontext, uint32_t max_connections );
int set_direction_request(uint32_t src_torus,uint32_t dest_torus);
int process_cleanup_table(struct my_context * mcontext);
uint8_t getCNportOnIOnode(void * personality);
int poll_bcast_rdma_counters(struct my_context * mcontext);
void InitBroadcastDirectPut( MUHWI_Descriptor_t  *desc,uint64_t Rec_Counter_Offset,  uint64_t Rec_Payload_Offset);
uint32_t get_init_sys_rec_fifo_id(int i);

void count_CN_in_block(struct my_context * mcontext);
int rdma_bcast_poll_reduce(struct my_context * mcontext);
int rdma_bcast_poll_atomic(struct my_context * mcontext);
int mudm_recv_bcast (struct my_context * mcontext, MUHWI_PacketHeader_t * hdr, uint32_t bytes);

void fillin_FlightLogegistyEntry_hiwrap(BG_FlightRecorderRegistry_t * bgfreg);
void fillin_FlightLogegistyEntry_nowrap(BG_FlightRecorderRegistry_t * bgfreg);
void fillin_FlightLogegistyEntry_debug(BG_FlightRecorderRegistry_t * bgfreg);
void complete_init_mudm_flightlog_fmt(void * anchor);

//routines from mudm_init.c and used by mudm_cleanup.c
int32_t activate_recfifos(struct MU_subgroup_memory * musm);
void activate_base_address_table(MUSPI_BaseAddressTableSubGroup_t * bat,uint32_t numbats,uint32_t * batids,uint32_t subgrpid,MUHWI_BaseAddress_t * batval);
int activate_injfifos(struct MU_subgroup_memory * musm);
uint64_t deactivate_recfifo(struct my_context * mcontext, struct MU_subgroup_memory * subgroup_mem);


void dump_ccontext_info(struct mudm_connection * ccontext);
//! \todo TODO remove run_atomic_tests
void run_atomic_tests(struct my_context * mcontext);
int mudm_recv_bcast_test(struct my_context * mcontext);
void largest_node_corner_in_block(struct my_context * mcontext);

uint64_t InjFifoInject_rget_emulation (MUDM_InjFifo_t   * injfifo_ctls, 
					 void              * desc, 
                                         BG_FlightRecorderRegistry_t *logregistry);

#endif /* _MUDM_COMMON_H_ */
