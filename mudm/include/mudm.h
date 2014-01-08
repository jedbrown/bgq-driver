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

//! \file  mudm.h 
//! \brief Declaration of special programming interfaces.
//! \remarks primarily intended for OFED or CN verbs implementers over torus


#ifndef	_MUDM_H_ /* Prevent multiple inclusion */
#define	_MUDM_H_


#define MUDM_VERSION 4

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/bqc/MU_PacketCommon.h> 



#ifdef __LINUX__
#include <linux/kernel.h>
#include <linux/errno.h>
#else
#include <stdio.h>
#include <errno.h>
#define EXPORT_SYMBOL(symbol)
#endif

#include <spi/include/kernel/trace.h>

/**
Common constants as #Define
*/
#define MUDM_MAX_REMOTE_SGE 58
#define MUDM_MAX_SGE       58
#define MUDM_MAX_IP_SGE     62
#define MUDM_MAX_PAYLOAD_SIZE      512

#define MUDM_MAX_PRIVATE_DATA  256


/**
 Defines a piece of memory 
  Note:  any 0 memlength or null physicalAddr terminates the list
*/


struct mudm_sgl {
uint64_t physicalAddr:36;
uint64_t memlength:28;
}__attribute__((__packed__)) ; 

/* Warning!  'Clever' method for generating max value for a bit field. */
#define MUDM_MAX_SGL_LENGTH ((u64) (((struct mudm_sgl)  { -1LL, -1LL }  ).memlength + 1))


struct mudm_sgl_virt {
void * virtualAddr;
uint32_t memlength;
}__attribute__((__packed__));

#define MUDM_UNKNOWN_QPN -1

/**
 * \brief Connection-oriented header describing the end-point nodes
 * and OFED qpn values (4-valued connection definition).
 *
 */


struct ionet_header {
                   uint16_t  sequence_number;    // sequence number for packets not involving connecting or disconnecting
                   uint32_t  torus_source_node;  // source torus node.  High bit on for IO node 
                   uint32_t  torus_dest_node;    //destination torus node.  High bit on for IO node                    
                   uint16_t  payload_length;     //Length of Payload data  
                   uint16_t  type;               // Identifies the payload 
                   uint16_t source_qpn;          //source QP number 
                   uint16_t dest_qpn;            //destination QP number--if unknown use UNKNOWN_QPN  
}__attribute__((__packed__)) ;



struct ionet_pkt_header {
                   uint16_t  sequence_number;    // sequence number on connection 
                   uint32_t  torus_source_node;  // source torus node.  High bit on for IO node 
                   uint32_t  torus_dest_node;    //destination torus node.  High bit on for IO node
                   uint16_t  payload_length;     //Length of Payload data  
                   uint16_t  type;               // Identifies the payload 
                   uint8_t   options;            // Copy of the options used on sending the message
                   uint8_t   port_info;          // identifies the port for IOnode response (from CN) or IOnode sent on
                   uint16_t  reserved;
}__attribute__((__packed__)) ;

struct broadcast_pkt_header {
                   uint16_t  sequence_number;    // sequence number on connection 
                   uint32_t  torus_source_node;  // source torus node.  High bit on for IO node 
                   uint32_t  reserved1;          // reserved
                   uint16_t  payload_length;     //Length of Payload data  
                   uint16_t  type;               // Identifies the payload 
                   uint32_t  reserved2;          // reserved
}__attribute__((__packed__)) ;


struct ionet_frame {
                   struct ionet_pkt_header ionet_pkt_hdr;
                   uint8_t   payload[0];  /* 1-512 bytes */
}__attribute__((__packed__)) ;


struct ionet_ip_remote {
                      struct ionet_header ionet_hdr;
                      uint16_t NUMSGEs; 
                      struct mudm_sgl SGE[0];/* 1 or more sgl elements within 510 bytes*/
}__attribute__((__packed__)) ;

struct ionet_send_imm {
                      struct ionet_header ionet_hdr;
                      char   payload[0];    /* 1-512 bytes */
}__attribute__((__packed__)) ;

#define MUDM_IMMEDIATE_DATA  1
#define MUDM_SOLICITED       2

/**
 * \brief Structure for sending physical memory addresses and sizes to a target
 * \param[in] flags is the bit-wise indicator of whether immediate data and whether solicit bit 
 * \li \c MUDM_IMMEDIATE_DATA
 * \li \c MUDM_SOLICITED
 * \note immediate data is valid for OFED IBV_WR_SEND_WITH_IMM or IBV_WR_RDMA_WRITE_WTIH_IMM 
 *       which corresponds to MUDM_RDMA_READ and 
 * \note solicited is valid for OFED Send and RDMA Write with immediate
 * 
 */
struct ionet_send_remote_payload{
 uint64_t RequestID;    /* Remote request ID to release at completion */
 uint64_t remote_vaddr; /* remote virtual Address                     */
 uint32_t rkey;         /* rkey for remote virtual-to-physical xlate  */
 uint32_t sgl_num;  
 uint32_t data_length;
 uint32_t immediate_data; 
 uint32_t flags; 
 uint32_t reserved;
 uint64_t rdma_object;  /* reserved for mudm use                      */
 struct mudm_sgl SGE[MUDM_MAX_REMOTE_SGE];/* 1 or more sgl elements within remainder of packet*/
}__attribute__((__packed__)) ;

struct ionet_send_remote {
                      struct ionet_header ionet_hdr;
                      struct ionet_send_remote_payload payload;
}__attribute__((__packed__)) ;

struct ionet_connect {/* used by Connection Request and Reply */
                      struct ionet_header ionet_hdr;
                      uint32_t source_port;
                      uint32_t dest_port;
                      uint8_t  source_IP_addr[16]; 
                      
                      uint8_t  dest_IP_addr[16]; 
                      
                      uint64_t source_request_id;  
                      uint32_t status; 
                      uint16_t private_data_length; /*0-256 */
                      char   private_data[0];    /* 0-256 bytes */   
} __attribute__((__packed__));
  


#define MUDM_IP_IMMED        0x1bc0
#define MUDM_PKT_DATA_IMM    0x1bc1
#define MUDM_PKT_DATA_IMM_SOLICITED    0x1dc1
#define MUDM_CONN_REQUEST    0x1bc2
#define MUDM_CONN_REPLY      0x1bc3
#define MUDM_IP_REMOTE       0x88b0
#define MUDM_PKT_DATA_REMOTE 0x88b1
#define MUDM_DISCONNECTED    0xdddd
#define MUDM_RDMA_READ       0x4444
#define MUDM_RDMA_WRITE      0x5555 
#define MUDM_RDMA_READ_IMM   0x444D



/* Memory Region Info with physical, virtual addresses, and lengths */
struct mudm_memory_region
{
    size_t    length;        /*!< Number of bytes in the memory region. */
    void*     base_vaddr;    /*!< Virtual address of the memory region. */
    void*     base_paddr;    /*!< Physical address of the memory region */
} __attribute__((__packed__)) ;
/**
 * \brief Receive a message.
 *
 * Message is the message pointer to the data.header followed by payload (0-512 bytes).  Note this pointer becomes invalid after return.  
 * \retval 0 Success
 * \retval -1 Error
*/
typedef int (*mudm_recv)(char* message,void * callback_context);
/**
 * \brief Receive a nondata message--connecting or disconnecting messages
 * 
 * \param[in] conn_context is the opaque handle for the connection context
 * \param[in] nondata_message is the payload of type pointer to struct ionet_header 
 * \param[in] callback_context is the opaque callback context as provided in the mudm_init interface
 *
 * \retval 0  Success
 * \retval -1 Error
 *
 * \note  conn_context for received disconnect or connect_reply with nonzero status is for reference purposes and is not to be used
 *        for any calls into mudm
*/

typedef int (*mudm_recv_conn)(void * conn_context, struct  ionet_header * nondata_message,void * callback_context);

/**
 * \brief Callback to deliver status on a request completing asynchronously.
 *
 * A call made on a different interface with a request ID parameter returned -EINPROGRESS indicating the 
 * operation is continuing asynchronously and mudm_poll will deliver this callback with the request ID 
 * when the operation is determined to have completed.
 *
 * \param[in] requestID was supplied on a prior call into a MUDM interface
 * \li \c 
 * \param[in] status is the state of the operation
 * \li \c 0 for success
 * \param[in] callback_context is the opaque callback context as provided in the mudm_init interface
 *
 * \retval 0  Success
 * \retval -1 Error
 *
 */
//typedef int (*mudm_status)(void* requestID, uint32_t status,void * callback_context);
typedef int (*mudm_status)(void* requestID[], uint32_t status[],void * callback_context,uint32_t  error_return[],uint32_t num_request_ids);

/**
length is the number of bytes to allocate and replicated into memregion if allocation is successful
if successful, memory region is filled in with physical and virtual addresses at 128 byte alignment

The return value is nonzerof for an error 
*/
typedef int (*mudm_allocate)(struct mudm_memory_region * memregion, size_t length,void * callback_context);

typedef int (*mudm_free)(struct mudm_memory_region * memregion,void * callback_context);


//! \todo TODO: remove req_inj_fifos, req_rec_fifos, req_num_pollers
struct mudm_init_info {
  uint32_t callers_version; /* =MUDM_VERSION  */
  uint32_t req_inbound_connections; /* requested maximum torus connections */
  uint32_t req_inj_fifos; /* requested number of injection fifos */
  uint32_t req_rec_fifos; /* requested number of receive fifos */
  uint32_t req_num_pollers; /* requested number of supported polling threads */
  mudm_recv recv; /*See callback section for type definition */
  mudm_recv_conn recv_conn; /*See callback section for type definition */
  mudm_status status;  //See callback section for type definition
  mudm_allocate allocate; //See callback section for type definition
  mudm_free free; //See callback section for type definition
  void * personality; /* personality pointer */
  void * callback_context;
  uint32_t req_number_of_rdma_bcast_channels; /* requested number of braodcast directput channels */
  uint32_t wakeupActive; //0=use mudm hard poll; nonzero use mudm wakeup poll
}; 

/* */



//This is output returned by the data mover into a supplied structure
struct mudm_cfg_info {
  uint32_t mudm_version; //set by #define MUDM_VERSION in include mudm.h
  uint32_t mudm_errno; //set by MUDM
  uint32_t max_connections; /* maximum torus connections */
  uint32_t max_payload; /* maximum packet payload */
  uint32_t num_io_links; /* Number of IO links */
  BG_FlightRecorderRegistry_t mudm_hi_wrap_flight_recorder;
  BG_FlightRecorderRegistry_t mudm_lo_wrap_flight_recorder;
  uint32_t wakeupActive; //0=using mudm hard poll; nonzero using mudm wakeup poll
};



/**
 * \brief  Create a MUDM context
 *
 * Configure and initialize a MU Data Mover context.  Receive advice back on how MUDM configured itself.
 * 
 * \param[in]     init_info is the initialization information for the MUDM which the user is requesting
 * \param[in,out] cfg_info is the MUDM configuration info--including error information-- provided to the user
 * \param[out]    mudm_context is an opaque object for the MUDM context
 *
 * \retval 0 Success
 * \retval -EBADRQC Bad request.
 * \li \c version mismatch between request in init_info and MUDM version of interface
 * \retval -ENOMEM No memory available for building MUDM constructs.
 * \retval -EINVAL Invalid value
 * \li \c Null callback value
 *
 */
int  mudm_init(struct mudm_init_info* init_info, 
               struct mudm_cfg_info* cfg_info, 
               void** mudm_context ); 


/**
 * \brief Poll for actionable items on an io_link 
 *
 *  The mudm_startPoll function polls for status completions and inbound messages and executes callbacks.   
 *  The caller obtains an appropriate thread on which to call mudm_start_poll().  Polling is done an mudm context basis.
 *
 * \param[in] mudm_context is the mudm context pointer from mudm_init()
 * \param[in] io_link is a value 0<=io_link<num_io_links where num_io_links is returned in mudm_cfg on mudm_init()
Function returns
 * \retval 0 Success
 * \retval -EINVAL for invalid argument 
 * \remark Actionable items include received packets, DMA completes, injections complete  
*/
int mudm_poll_iolink(void* mudm_context, uint32_t io_link);

/**
 * \brief Poll for actionable items.
 *
 *  The mudm_startPoll function polls for status completions and inbound messages and executes callbacks.  
 *  Return will happen during execution of mudm_terminate().   
 *  The caller obtains an appropriate thread on which to call mudm_start_poll().  Polling is done an mudm context basis.
 *
 * \param[in] mudm_context is the mudm context pointer from mudm_init()
 * \param[in] options sets the how poll is to run
 * \li \c LOOPFOREVER will loop forever
 * \li \c POLLANDRETURN will poll for actionable items, handle and return
Function returns
 * \retval 0 Success
 * \retval -EINVAL for invalid argument 
 * \remark Actionable items include received packets, DMA completes, injections complete  
*/

int mudm_start_poll(void* mudm_context, uint32_t options);
#define MUDM_LOOPFOREVER 0
#define MUDM_POLLANDRETURN 1
/**
 * \brief Terminate the MUDM instance associated with context mudm_context
 *
 * \param[in]  mudm_context is the mudm context pointer from mudm_init()
 *   
 * \retval  0
 des*/
int mudm_terminate(void* mudm_context); 


/**
 * \brief Sends a connect request from the remote node.
 *
 * \param[in]  mudm_context is the mudm context pointer from mudm_init()
 * \param[out] conn_context is the returned opaque handle for the connection context
 * \param[in]  dest_node is the destination node
 * \param[in]  conn_request is the payload of type pointer to struct ionet_connect  
 * \param[in]  private_data is the address of private data associated with the connect 
 *             request (RDMA CM)
 * \param[in]  private_data_length is the length of the private data (0-256)
 *   
 * \retval  0  Sent to remote
 * \retval -EFAULT Invalid conn_context 
 * \retval -EBUSY No resources, busy using resources.  Try again later.
 * \retval -EINVAL for invalid argument
*/

int  mudm_connect(void* mudm_context,
                 void** conn_context, 
                 uint32_t dest_node,
                 struct ionet_connect * conn_request,
                 char * private_data,
                 uint16_t private_data_length);
#define MUDM_CN2IO_NODE     (uint32_t)(-1)
#define MUDM_NODE_LOOPBACK  0xC0000000

/**
 * \brief Responds to the connect request from the remote node.
 *
 * \param[in]  conn_context is the mudm connection context pointer from mudm_connect or mudm_recv_connect
 * \param[in]  dest_node is the destination node
 * \param[in]  conn_reply is the payload of type pointer to struct ionet_connect  
 * \param[in]  private_data is the address of private data associated with the connect 
 *             request (RDMA CM)
 * \param[in]  private_data_length is the length of the private data (0-256)
 *   
 * \retval  0  Sent to remote
 * \retval -EFAULT Invalid conn_context 
 * \retval -EBUSY No resources, busy using resources.  Try again later.
 * \retval -EINVAL for invalid argument
*/


int mudm_conn_reply(void* conn_context,
                struct ionet_connect * conn_reply,
                char * private_data,
                uint16_t private_data_length);

/**
  * \brief Disconnect from the remote
  *
  * \param[in]  conn_context is the mudm connection context pointer from mudm_connect or mudm_recv_connect
  * \retval  0  Sent to remote and disconnected locally
  * \retval -EFAULT Invalid conn_context
  */
int mudm_disconnect(void * context);


/**
 * 
 * \brief  Send a packet on a connection.
 *
 * Using the standard header, send a memfifo packet on the connection of protocol type.
 * This provides the simplest point-to-point memFIFO transfer for sending a message to a remote connection.  
 * 
 *
 * \param [in]  conn_context is the mudm connection context pointer from  mudm_connect or mudm_accept
 * \param [in]  requestID is for the registered status callback indicating the status of the operation completing (may be NULL).
 * \param [in]  type is the protocol type
 * \param [in]  payload_paddr is the physical address of the data (payload) 
 * \param [in]  payload_length is the length of the data.
 *   
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument.
 * \li \c payload_length > TORUS_MAX_PAYLOAD_SIZE
 * \retval -EINPROGRESS Request is in progress, requestID pending
 * \retval -EBUSY No resources, busy using resources.  Try again later.
 * 
 * 
 **/
int mudm_send_pkt(void* conn_context ,
                     void* requestID, 
                     uint16_t type,
                     void * payload_paddr,
                     uint16_t payload_length); 

/**
 * 
 * \brief  Send data memory descriptors within the size of a memfifo packet on a connection.
 *
 * The mudm_send_message_remote is intended for more complex cases of sending messages with scatter-gather lists 
 * and exceeding the memFIFO size of the MU.  The message will be transported via DMA facilities.
 * 
 *
 * \param [in]  conn_context is the mudm connection context pointer from  mudm_connect or mudm_accept
 * \param [in]  type is the protocol type
 * \li \c RDMA_READ
 * \li \c PKT_DATA_REMOTE
 * \li \c  RDMA_WRITE
 * \param [in]  payload of type pointer to struct ionet_send_remote_payload
 *   
 * \retval  0   Request is complete, requestID not pending
 * \retval -EINVAL Invalid argument 
 * \li \c type is not a supported type
 * \retval -EINPROGRESS Request is in progress, requestID pending 
 * \retval -EBUSY No resources, busy using resources.  Try again later.
 * \retval -EFAULT  Faulting condition
 * \li \c payload->sgl_num=0
 * \retval -E2BIG payload->sgl_num > MUDM_MAX_SGE
 * 
 * 
 **/
int mudm_send_message_remote(void* conn_context,
                        uint16_t type, 
                        struct ionet_send_remote_payload * payload);

/**
 * 
 * \brief  Send data within the size of a memfifo packet as connectionless transport.
 *
 * This function sends a memfifo packet to the remote location using a scatter-gather list of 
 * virtual memory pieces which will be copied into a contiguous memory location for the send.
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from  mudm_init 
 * \param [in]  requestID is for the registered status callback indicating the status of the operation completing (may be NULL).
 * \param [in]  header is a pointer to the bytes for the header of type struct ionet_pkt_header
 * \param [in]  sgl_num is the number of elements in scatter-gather list sgl
 * \param [in]  data_length is the length of the data.
 * \param [in] sgl is the scatter-gather list
 *   
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument 
 * \retval -EBUSY No resources, busy using resources.  Try again later.
 * 
 * \remark The data_length 
 **/

int mudm_pkt_imm(void* mudm_context,
                      void* requestID,       
                      struct ionet_pkt_header * header,           
                      uint16_t sgl_num,  
                      uint16_t data_length,
                      struct   mudm_sgl_virt*  sgl);

//MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_6  (0x08)
//MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_TORUS_PORT_7  (0x10)
//MUHWI_PACKET_RETURN_FROM_IO_NODE_USING_IO_PORT  (0x18)
#define MUDM_NO_IOLINK 0
#define MUDM_CNB2IONODE 1
#define MUDM_USE_DEFAULT_IO_LINK 2
#define MUDM_IOLINK_PORT6  0x08
#define MUDM_IOLINK_PORT7  0x10
#define MUDM_IOLINK_PORT10 0x18
#define MUDM_IOLINK_ALL_PORTS 255
/**
 * 
 * \brief  Send a connectionless memfifo packet given a physical address to the data
 *
 * This function sends a memfifo packet to the remote location.
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from  mudm_init 
 * \param [in]  requestID is for the registered status callback indicating the status of the operation completing (may be NULL).
 * \param [in]  destination_torus_address is the torus address of the destination
 * \param [in]  type is the type field to fill into the ionet_pkt_msg
 * \param [in]  options instructs whether on the same torus or going over an io link or links.  The value will be copied into the ionet_pkt_msg
 * \param [in]  payload_paddr is the physical address of the data (payload) 
 * \param [in]  payload_length is the length of the data.
 *   
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument 
 * 
 * \remark The payload_length cannot be more than the size of the maximum size of the data in a memfifo packet.
 * 
 **/

int    mudm_send_msg(void* mudm_context , 
                     void* requestID, 
                     uint32_t destination_torus_address,  
                     uint16_t  type,  
                     uint8_t  options,
                     void * payload_paddr, /* physical address of payload */
                     uint16_t payload_length);


/**
 * 
 * \brief Perform an RDMA write.
 *
 * This function executes an RDMA transfer to the remote location from the local location, an RDMA write.
 *
 * \param [in]  conn_context is the mudm connection context pointer from  mudm_connect or mudm_accept. 
 * \param [in]  remote_requestID is the remote requestID.
 * \param [in]  rdma_object is the physical address of the mudm rdma object for handling
 *              completion of the rdma operation.
 * \param [in]  bytes_xfer is the number of bytes to transfer.
 * \param [in]  local_sgl is the local list of of physical addresses (target of DMA).
 * \param [in]  remote_sgl is the remote list of memory locations (source of DMA).
 *   
 * \retval  0   Request is complete 
 * \retval -EINVAL Invalid argument 
 * 
 * \remark Exhausting bytes_xfer will complete the RDMA operation.  A null or 0 encountered in a memory list element 
 *         will end the RDMA operation.
 * 
 **/

int mudm_rdma_write(void* conn_context,
                    void* remoteRequestID,
                    uint64_t rdma_object, /* from header with type=RDMA_WRITE */
                    uint64_t bytes_xfer, 
                    struct mudm_sgl*  local_sgl, 
                    struct mudm_sgl* remote_sgl);

/**
 * 
 * \brief Perform an RDMA read on a connection
 *
 * This function executes an RDMA transfer from the remote location to the local location, an RDMA read.
 * 
 *
 * \param [in]  conn_context is the mudm connection context pointer from  mudm_connect or mudm_accept. 
 * \param [in]  requestID is for the registered status callback indicating the status of the operation completing (may be NULL).
 * \param [in]  remote_requestID is the remote requestID.
 * \param [in]  rdma_object is the physical address of the mudm rdma object for handling
 *              completion of the rdma operation.
 * \param [in]  bytes_xfer is the number of bytes to transfer.
 * \param [in]  local_sgl is the local list of of physical addresses (target of DMA).
 * \param [in]  remote_sgl is the remote list of memory locations (source of DMA).
 *   
 * \retval  0   Request is complete 
 * \retval EINVAL Invalid argument 
 * 
 * \remark Exhausting bytes_xfer will complete the RDMA operation.  A null or 0 encountered in a memory list element 
 *         will end the RDMA operation.
 * 
 **/
int mudm_rdma_read(void* conn_context ,
                                   void* requestID, 
                                   void* remote_requestID, 
                                   uint64_t rdma_object, /* from header with type=RDMA_READ */
                                   uint64_t bytes_xfer,                                  
                                   struct mudm_sgl*  local_sgl, 
                                   struct mudm_sgl* remote_sgl);


/**
 * 
 * \brief Clear an IO link based on correlating a block ID to that link on an IO node
 *
 * This will clear the IO link on an IO node based 
 * 
 *
 * \param [in]  BlockID is the 32-bit value of the blockid associated with the IO link
 *   
 * \retval  0   Request is complete 
 * \retval EINVAL Invalid argument 
 * 
 * \remark BlockID is assumed to be nonzero.  The BlockId on the compute nodes on an IO link is assumed to be the same.
 *  The block ID on an IO link is the block ID of the last connection made
 * \note If the BlockID is the same on the IO links, both links will be cleared.
 * 
 **/

int mudm_resetIOlink(void* mudm_context , uint32_t blockID);


/**
 * \brief Perform an RDMA read between two nodes on the same torus
 *
 * This function executes an RDMA transfer from the remote location to the local location, an RDMA read.
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from  mudm_init . 
 * \param [in]  torus_destination is the remote node on the same torus for reading the data
 * \param [in]  bytes_xfer is the number of bytes to transfer.
 * \param [in]  counter_paddr for use as the MU counter which will be zero when the RDMA completes
 * \param [in]  local_paddr is the local physcial address target for the read 
 * \param [in]  remote_paddr is the remote physical address source of the data for the read
 *   
 * \retval  0   Request is complete (
 * \retval -EINPROGRESS Request is in progress, check user provided counter at address counter_paddr for RDMA completion
 * \retval -EINVAL Invalid argument 
 * \retval -EBUSY  No resources
 * 
 * \note  If counter_paddr is zero, then the call blocks until the RDMA completes.  If nonzero, then the call returns and the caller
 *        polls whether the counter at location counter_paddr has become zero.
 * 
 **/

int mudm_rdma_read_on_torus(       void* mudm_context ,
                                   MUHWI_Destination_t torus_destination,//remote torus location
                                   uint64_t bytes_xfer, 
                                   void * counter_paddr,                                 
                                   void * local_paddr, 
                                   void * remote_paddr);

/**
 * \brief Perform an RDMA write from one node to another on the same torus
 *
 * This function executes an RDMA transfer to the remote location from the local location, an RDMA write.
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from  mudm_init . 
 * \param [in]  torus_destination is the remote node on the same torus for reading the data
 * \param [in]  bytes_xfer is the number of bytes to transfer.
 * \param [in]  counter_paddr for use as the MU counter which will be zero when the RDMA completes
 * \param [in]  local_paddr is the local physcial address target for the read 
 * \param [in]  remote_paddr is the remote physical address source of the data for the read
 *   
 * \retval  0   Request is complete (
 * \retval -EINVAL Invalid argument 
 * \retval -EBUSY  No resources
 * 
 * 
 * \note  If counter_paddr is  nonzero, the caller is responsible for ensuring that the remote physical address is valid and contains the size of the RDMA transfer.  If counter_paddr is zero, the RDMA transfer will be completed using a dummy counter on the remote.
 **/

int mudm_rdma_write_on_torus(       void* mudm_context ,
                                   MUHWI_Destination_t torus_destination,//remote torus location
                                   uint64_t bytes_xfer, 
                                   void * counter_paddr,                                 
                                   void * local_paddr, 
                                   void * remote_paddr);
/**
 * \brief Wakeup mudm poll loops using wakeup wait
 *
 * This will ensure the mudm poll loop runs if it is sleeping.
 * 
 *
 * \param [in]  mudm_context is the mudm context pointer from  mudm_init .
 **/
void mudm_wakeup( void* mudm_context);


#endif /* _MUDM_H_*/
