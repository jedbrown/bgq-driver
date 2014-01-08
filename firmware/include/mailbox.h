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

#ifndef _BGQ_MAILBOX_H_ // Prevent multiple inclusion
#define _BGQ_MAILBOX_H_

#include <hwi/include/common/compiler_support.h>

#ifndef __LINUX__
#include <stdint.h>
#endif

#ifdef __xlC__
#pragma options align=packed
#endif

__BEGIN_DECLS

#define JMB_MAX_DOMAINS 2

typedef struct MailBoxHeader_t
{
    volatile uint16_t usCmd;          // Mailbox command field.
    uint16_t usPayloadLen;  // does not include header size or any rounding
    uint16_t usID;               // h/w thread id (0..67) of sender
    uint16_t usCrc;              // 0 means no CRC, calculated 0 is flipped to 0xFFFF.
    //
    // variable length data, of size usPayloadLen immediately follows the header
    //
}
MailBoxHeader_t;

typedef struct MailBoxPayload_Write_t
{
    uint64_t  dest_vaddr;
    uint64_t  count;
    uint8_t   data[0];
}
__PACKED MailBoxPayload_Write_t;


typedef struct MailBoxPayload_Fill_t
{
    uint64_t  dest_vaddr; // should be word-aligned
    uint64_t  count;        // count of bytes to fill
    uint32_t  value;        // value to fill, eg 0, 0xdeadbeef, etc.
}
  __PACKED MailBoxPayload_Fill_t;

typedef struct MailBoxPayload_Read_t
{
    uint64_t  src_vaddr;
    uint64_t  count;
    uint8_t   data[0];
}
__PACKED MailBoxPayload_Read_t;

typedef struct MailBoxPayload_Terminate_t
{
    uint64_t  time_stamp;   // time in pclks
    int32_t   return_code;  // signed return code
}
__PACKED MailBoxPayload_Terminate_t;

typedef struct MailBoxPayload_Launch_t
{
    uint64_t  entry_vaddr[JMB_MAX_DOMAINS];  // virtual address of kernel entry (per domain)
}
__PACKED MailBoxPayload_Launch_t;

typedef struct MailBoxPayload_crcEntry_t
{
    uint64_t  start;  // starting address w/i node's memory to begin CRC32.
    uint64_t  size;   // number of bytes to include in the CRC32.
    uint64_t  crc;    // expected CRC32 value.
}
MailBoxPayload_crcEntry_t;

typedef struct MailBoxPayload_crc_t
{
    uint32_t num_entries;     // Number of actual elf image section that actually follow.
    uint32_t _unused;         // No longer in use
    MailBoxPayload_crcEntry_t elf_sctn_entry[64];
}
MailBoxPayload_crc_t;

typedef struct MailBoxPayload_RAS_t
{
  uint64_t  UCI;             //! Universal component id
  uint32_t  message_id;      //! The RAS event identifier.
  uint16_t  _reserved;       //! Not currently used
  uint16_t  num_details;     //! The number of 64-bit words (details) to follow
  // following here is an array of uint64_t values.  The length of the 
  // array is described by the num_details field.
  uint64_t  details[0];
}
__PACKED MailBoxPayload_RAS_t;

typedef struct MailBoxPayload_RAS_ASCII_t
{
  uint64_t  UCI;             //! Universal component id
  uint32_t  message_id;      //! The RAS event identifier.
  // following here is a null-terminate ASCII string.
  char      message[0];
}
__PACKED MailBoxPayload_RAS_ASCII_t;

typedef struct MailBoxPayload_Stdin_t
{
    uint16_t  count;
    uint8_t   data[0];
}
__PACKED MailBoxPayload_Stdin_t;


typedef struct MailboxPayload_DomainEntry_t {
    uint32_t coreMask;         //! The cores assigned to this domain.  LSB = core 0, and so on.
    uint32_t _reserved;        //! Preserves DW alignment
    uint64_t ddrStart;         //! The start of this domain's DDR region (1GB aligned)
    uint64_t ddrEnd;           //! The end of this domain's DDR region (1GB aligned)
    uint64_t configAddress;    //! The address of this domain's configuration area.
    uint32_t configLength;     //! The length of this domain's configuration area.
    char     options[512];     //! The domain's command line options
} MailboxPayload_DomainEntry_t;

typedef struct MailBoxPayload_ConfigureDomains_t
{
  uint32_t numberOfDomains;    //! The number of valid entries (below)
  uint32_t _reserved;          //! preserves DW alignment.
  MailboxPayload_DomainEntry_t domain[JMB_MAX_DOMAINS];
}
__PACKED MailBoxPayload_ConfigureDomains_t;

typedef struct MailBoxPayload_BlockState_t
{
    uint16_t block_state;        //! Current state of the block (constants below).
    uint16_t _unused;
    uint32_t block_id;           //! Defined with block_state == JMB_BLOCKSTATE_IO_LINK_CLOSED
    uint64_t timestamp;          //! Timebase (from the node) at which point the message was issued.
}
__PACKED MailBoxPayload_BlockState_t;


#define JMB_CTRLSYSREQ_SHUTDOWN            (1) //! Shut down the node/block
#define JMB_CTRLSYSREQ_SHUTDOWN_IO_LINK    (2) //! Shut down the I/O link associated with a specified block

typedef struct MailBoxPayload_ControlSystemRequest_t
{
    uint32_t sysreq_id;   //! From the JMB_CTRLSYSREQ_* list above.
    
    union {

	struct {
	    uint32_t block_id;
	} shutdown_io_link;

    } details;

}
__PACKED MailBoxPayload_ControlSystemRequest_t;

// Block state constants
#define JMB_BLOCKSTATE_INITIALIZED        (0x0001) //! Block is initialized
#define JMB_BLOCKSTATE_HALTED             (0x0002) //! Block is halted
#define JMB_BLOCKSTATE_IO_LINK_CLOSED     (0x0003) //! I/O Link is halted

// CRC handling constants
#define JMB_INITIAL_CRC        (0xFFFF) // Initial seed value for CRC16n check
#define JMB_INITIAL_CRC32  (0xFFFFFFFF) // Initial seed value for CRC32n check
#define JMB_NO_CRC             (0x0000) // No CRC was calculated. This CRC should not be checked.

// Result codes
#define JMB_RC_SUCCESS         (0x0000) // Success!  It's all good.
#define JMB_RC_BAD_CMD         (0x0001) // An unknown/invalid command was found.
#define JMB_RC_BAD_CRC         (0x0002) // The CRC check failed.
#define JMB_RC_EINVAL          (0x0003) // An Invalid parameter or argument was found.
#define JMB_RC_ENOSYS          (0x0004) // Receiver does not support this command.


// This bit will be set when the reader (host or core) has processed
//   the message, and indicates that an rc is available, depending
//   on the message in the Result field.
#define JMB_CMD_ACK            (0x8000)

// JTAG MailBox Messages from Cores to Host (note: these are mapped via 1 bit per command).
#define JMB_CMD2HOST_NONE              (0x0000) // No command.
#define JMB_CMD2HOST_READY             (0x0001) // Initial boot done, ready to load kernel, etc.
#define JMB_CMD2HOST_STDOUT            (0x0002) // MailBox payload contains a stdout message.
#define JMB_CMD2HOST_STDERR            (0x0100) // Mailbox payload contains a stderr message.
#define JMB_CMD2HOST_RAS               (0x0004) // MailBox contains a binary formatted RAS message.
#define JMB_CMD2HOST_TERMINATE         (0x0008) // Termination Request, check Result.
#define JMB_CMD2HOST_NETWORK_INIT      (0x0010) // Ready for Network Init (HSSBIST can begin on Host)
#define JMB_CMD2HOST_HSSBIST_SEND_DONE (0x0020) // HSSBIST: Nodes are Sending BIST patterns
#define JMB_CMD2HOST_HSSBIST_RESULT    (0x0040) // Report PASS/FAIL of HSSBIST
#define JMB_CMD2HOST_RAS_ASCII         (0x0080) // MailBox contains an ascii formatted RAS message.
#define JMB_CMD2HOST_BARRIER_REQ       (0x0200) // A software barrier is being requested.  Once all nodes in the “partition” have requested  the barrier, the host should reply to all with a _BGQ_JMB_CMD2CORE_BARRIER_ACK message. 
#define JMB_CMD2HOST_BLOCK_STATE       (0x0400) // Current state of block during boot and shutdown sequences

#define JMB_CMD2CORE_NONE                   (0x0000) // No command.
#define JMB_CMD2CORE_WRITE                  (0x0001) // Write bytes to   memory.
#define JMB_CMD2CORE_FILL                   (0x0002) // Fill  bytes in   memory.
#define JMB_CMD2CORE_READ                   (0x0003) // Read  bytes from memory.
#define JMB_CMD2CORE_LAUNCH                 (0x0004) // Launch the Kernel's Entry Point.
#define JMB_CMD2CORE_STDIN                  (0x0005) // Console Input via stdin.
#define JMB_CMD2CORE_BARRIER_ACK            (0x0006) // The previously requested software barrier has been reached
#define JMB_CMD2CORE_CONFIGURE_DOMAINS      (0x0007) // Configure domains (assign cores and memory, etc.)
#define JMB_CMD2CORE_ELF_SCTN_CRC           (0x0008) // Performa a CRC32 validation of code loaded into DDR
#define JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST (0x0009) // Issue a Control System Request

__END_DECLS

#ifdef __xlC__
#pragma options align=reset
#endif


#endif // Add nothing below this line.
