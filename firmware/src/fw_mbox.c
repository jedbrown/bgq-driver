/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include <firmware/include/Firmware.h>
#include <firmware/include/mailbox.h>
#include <hwi/include/bqc/testint_dcr.h>
#include "Firmware_internals.h"
#include "Firmware_RAS.h"

// place holders for linker script

#define FW_INBOX_SIZE   ( 4 * 1024)
#define FW_OUTBOX_SIZE  (16 * 1024)
#define FW_BASE_ADDRESS  0x03fffffc0000UL  // NOTE: This must match the linker script!

uint8_t /* FW_MAILBOX_TO_CORE */  fw_MBox2Core[FW_INBOX_SIZE] ALIGN_L1D_CACHE = { 0, };
uint8_t /* FW_MAILBOX_TO_HOST */  fw_MBox2Host[FW_OUTBOX_SIZE] ALIGN_L1D_CACHE = { 0, };

volatile uint64_t fw_mailbox_pointer = 0;
volatile uint64_t fw_mailbox_full_start_time = 0;
volatile uint64_t fw_mailbox_full_end_time = 0;
volatile unsigned fw_mailbox_full_length = 0;


#define USERCODE(h,l) ( (((uint64_t)(h))<<32) | ((l)&0xFFFFFFFF) )

#define FW_MBOX_ERROR (uint64_t)(-1)

#define INCREMENT_AND_WRAP(ptr,size,origin,mbox_size)	\
  ptr += (size);				\
  if ( (ptr) >= (origin + mbox_size) )	\
    ptr = origin;


// ---------------------------------------------------------------------
// NOTE: for now, we will use the same lock for both inbox and outbox
//       mutexes.  Because of this, we cannot issue printfs from inside
//       the inbox code (deadlock).
// ---------------------------------------------------------------------

#define USESEMAPHORE 0

#if USESEMAPHORE
#define FW_OUTBOX_LOCK BeDRAM_LOCKNUM_MBOX_TICKET
#define FW_INBOX_LOCK  BeDRAM_LOCKNUM_MBOX_TICKET
#endif

#define ROUND_2_QW_BOUNDARY(a) ( ( (a) + 15 ) & ~0xF )

#define TI_DCR TESTINT_DCR_PRIV_PTR

int fw_mailbox_init( unsigned leader ) {

  if ( ProcessorCoreID() == leader ) {

    uint64_t offset;

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //
    // Initialize the USERCODE DCRs.  The format is:
    //
    //     0               31 32              63
    //    +------------------+------------------+
    //    | size             | offset           |
    //    +------------------+------------------+
    //
    // USERCODE0 describes the outbox (node->host mailbox) and
    // USERCODE1 describes the inbox (host->core mailbox)
    //
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    offset = (uint64_t)fw_MBox2Host - FW_BASE_ADDRESS; /*out of range pointer*/
    TI_DCR->usercode0[0] = USERCODE(FW_OUTBOX_SIZE,offset);

    offset = (uint64_t)fw_MBox2Core - FW_BASE_ADDRESS; /*out of range pointer*/ 
    TI_DCR->usercode1[0] = USERCODE(FW_INBOX_SIZE, offset);


    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initialize REG0-3 to zero
    //! @todo  The mailbox spec says that REG1 and REG2 should never 
    //         be written by node software, but I think this really 
    //         refers to post-initialization.  Otherwise, we risk a 
    //         storm of interrupts.  Need to confirm with the control 
    //         system team on this.
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    TI_DCR->mailbox_reg0[0] = 0;
    TI_DCR->mailbox_reg1[0] = 0;
    TI_DCR->mailbox_reg2[0] = 0;
    TI_DCR->mailbox_reg3[0] = 0;
    fw_mailbox_pointer = 0;

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initialize the mailbox pointer mutex:
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if USESEMAPHORE
    fw_semaphore_init( FW_OUTBOX_LOCK, 1 );
    //fw_semaphore_init( FW_INBOX_LOCK, 1 );
#endif

    fw_semaphore_init( BeDRAM_LOCKNUM_RAS_HISTORY, 1 );
    fw_semaphore_init( BeDRAM_LOCKNUM_RAS_FLUSH_LOCK, 1 );
    fw_semaphore_init( BeDRAM_LOCKNUM_CS_BARRIER, 1 );
    fw_semaphore_init( BeDRAM_LOCKNUM_MBOX_FULL, 1);
    fw_semaphore_init( BeDRAM_LOCKNUM_MBOX_OPEN_AGAIN, 1 );

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Plug in mailbox implementations of certain functions:
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    extern Firmware_Interface_t FW_Interface;

    FW_Interface.putn = fw_mailbox_putn;

    if ( TRACE_ENABLED(TRACE_Debug) ) {
	 printf("(I) Mailbox is up from leading core %d!\n", leader);
    }

    BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_MBOX_INIT);
  }

  while ( BeDRAM_Read(BeDRAM_LOCKNUM_MBOX_INIT) == 0 )
    /* spin */
    fw_pdelay(1600)  ;


  return 0;
}

int fw_mailbox_init_on_core_0( void ) {
  return fw_mailbox_init(0);
}



/**
 * @brief  Atomically updates the in-memory mailbox pointer to account
 *         for a message of the specified length.  If there is not
 *         sufficient room in the mailbox buffer, this call will return
 *         FW_MBOX_ERR with an errno of FW_EAGAIN.
 * @param[in] length Describes the length of the entire mailbox message
 *         including the header).  It is assumed to be rounded up to 
 *         the nearest quadword.
 * @return The reserved mailbox location.  The "release" routine uses
 *         this as a sentinel value to compare against REG0, thus 
 *         indicating when the message may be released.  This guarantees
 *         ordering of outbound messages. 
 */

uint64_t fw_mailbox_reserve(unsigned length, int* errno) {

  uint64_t result = 0;
  int done = 0;
  int reportedMailboxFull = 0;
  
  while ( ! done ) {
    
#if USESEMAPHORE
    // Lock the mailbox pointer:
    if ( fw_semaphore_down_w_timeout(FW_OUTBOX_LOCK, 200ul) != 0 ) {
      *errno = FW_EAGAIN;
      return FW_MBOX_ERROR;
    }
#else
      // Lock the mailbox pointer:
      fw_ticket_wait(BeDRAM_LOCKNUM_MBOX_TICKET, BeDRAM_LOCKNUM_MBOX_SERVE);
#endif
    
    // If there is enough room in the mailbox for the message, increment the
    // mailbox pointer (we are done).  Otherwise, we will have to try again.

    if ( ( fw_mailbox_pointer + length - TI_DCR->mailbox_reg1[0] ) <= FW_OUTBOX_SIZE ) {
      result = fw_mailbox_pointer;
      fw_mailbox_pointer += length;
      done = 1;
    }
    else {

	// The mailbox is full.  If we are the first thread to detect this condition (as
	// determined by the MBOX_FULL semaphore), note the current time and set the 
	// flag to indicate that we are in this mode.

	if (reportedMailboxFull == 0 ) {

	    if ( fw_semaphore_down_w_timeout( BeDRAM_LOCKNUM_MBOX_FULL, 10000 ) == 0 ) {
		reportedMailboxFull = 1;
		fw_mailbox_full_start_time = GetTimeBase();
		fw_mailbox_full_length = length;
	    }
	}
    }

#if USESEMAPHORE
    // Release the mailbox pointer lock:
    fw_semaphore_up(FW_OUTBOX_LOCK);
#else
    fw_ticket_post(BeDRAM_LOCKNUM_MBOX_SERVE);
#endif


  }

  // If this thread encountered a full mailbox and is reponsible for reporting it, then note the
  // time and lower the semaphore to indicate that the delay has ended.  Some thread will 
  // detect and handle this situation in the release method.

  if ( reportedMailboxFull != 0 ) {
      fw_mailbox_full_end_time = GetTimeBase();
      ppc_msync();
      fw_semaphore_down(BeDRAM_LOCKNUM_MBOX_OPEN_AGAIN);
  }


  return result;
}


/**
 * @brief Releases the message from the mailbox.  This is the complementary routine to
 *     the fw_mailbox_reserve routine described above.  This routine will block until
 *     the external mailbox pointer (REG0) lines up with the expected beginning of this
 *     message (as indicated by the reserve argument).  Upon return, the message
 *     pointer will have been advanced to indicate that the control system should pick
 *     up the message.  It is assumed that the message header and payload has already
 *     been written before this routine is called.
 * @param[in] reserve The reserved "address" (mailbox pointer) as returned by the
 *     fw_mailbox_reserve routine.
 * @param[in] length The length of the message (including header and quad-word rounding).
 * @return 0
 */

int64_t fw_mailbox_release(uint64_t reserve, unsigned length) {

  uint64_t current;


  while ( (current = TI_DCR->mailbox_reg0[0] ) != reserve )
    fw_pdelay(200) /* spin */
    ;

  ppc_msync();

  TI_DCR->mailbox_reg0[0] = current + (uint64_t)length;

  // If the semaphore indicating a full mailbox condition is lowered, then
  // attempt to raise it.  In the event that this is successful, report the
  // condition via a RAS event.

  if ( BeDRAM_Read( BeDRAM_LOCKNUM_MBOX_OPEN_AGAIN ) == 0 ) {

      if ( BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_MBOX_OPEN_AGAIN) == 0 ) {
	  uint64_t start = fw_mailbox_full_start_time;
	  uint64_t end   = fw_mailbox_full_end_time;
	  uint64_t delayInMillis = (end - start) / 1000 / (uint64_t)FW_Personality.Kernel_Config.FreqMHz;
	  ppc_msync();
	  fw_semaphore_up(BeDRAM_LOCKNUM_MBOX_FULL);
	  // If the mailbox has been full for more than 10 seconds, issue a message.
	  if ( delayInMillis >= ( 10ull * 1000ull ) ) {
	      FW_RAS_printf( FW_RAS_INFO, "The mailbox has been full for %ld milliseconds.  Original request was %d bytes.", delayInMillis, fw_mailbox_full_length  );
	  }
      }
      else {
	  BeDRAM_ReadDecSat(BeDRAM_LOCKNUM_MBOX_OPEN_AGAIN);
      }
      
  }

  return 0;
}


/*__INLINE__*/ void fw_mailbox_set_header( MailBoxHeader_t* header, int command, unsigned length ) {
  header->usCmd        = command;
  header->usPayloadLen = length;
  header->usID         = ProcessorID();
  header->usCrc        = 0; // no CRC for nwo
}

__INLINE__ uint64_t fw_mailbox_mask_interrupts( void ) {
  uint64_t current = mfmsr();
  mtmsr( current & ~(MSR_EE | MSR_CE | MSR_ME) );
  isync();
  return current;
}

__INLINE__ void fw_mailbox_restore_interrupts(uint64_t msr) {
  mtmsr(msr);
  isync();
}


int fw_mailbox_putstring(const char* str, size_t len, int add_new_line) {

  // Immediately ignore null requests:

  if (len == 0) {
    return 0;
  }


  unsigned length = sizeof(MailBoxHeader_t) + len ;

  if ( length  > FW_OUTBOX_SIZE) {
    return FW_TOO_BIG;
  }

  int rc;

  // We must disable external, critical and machine check interrupts when in this 
  // path.  Otherwise we risk deadlock.

  uint64_t original_msr = fw_mailbox_mask_interrupts();
  int      errno = 0;
  uint64_t reserve  = fw_mailbox_reserve(ROUND_2_QW_BOUNDARY(length), &errno);

  if ( reserve != FW_MBOX_ERROR ) {

    uint8_t* mbox_ptr = fw_MBox2Host + (reserve & (FW_OUTBOX_SIZE-1));

    fw_mailbox_set_header( (MailBoxHeader_t*)mbox_ptr, JMB_CMD2HOST_STDOUT, len );

    mbox_ptr += sizeof(MailBoxHeader_t);


    // Can the payload be written in one piece or two?

    uint8_t* tailptr = 0;

    if ( ( ( ( reserve ) & (FW_OUTBOX_SIZE-1) ) + length ) > FW_OUTBOX_SIZE ) {
    
      size_t length1 = FW_OUTBOX_SIZE - (mbox_ptr - fw_MBox2Host);

      memcpy( mbox_ptr, str, length1);
      memcpy( fw_MBox2Host, str + length1, len - length1 );
      tailptr = fw_MBox2Host + len - length1 - 1;
    }
    else {
      memcpy( mbox_ptr, str, len );
      tailptr = mbox_ptr + len - 1;
    }


    if (add_new_line) {

      // It is possible that the tail has wrapped around to the very first mailbox byte
      // (thanks, BEAM!).  So we must be very careful.
    
      if (tailptr == fw_MBox2Host) {
	fw_MBox2Host[FW_OUTBOX_SIZE-1] = '\n';
	fw_MBox2Host[0] = 0;
      }
      else {
	*(tailptr--) = 0;
	*tailptr = '\n';
      }
      len++;
    }

    fw_mailbox_release(reserve, ROUND_2_QW_BOUNDARY(length));

    rc = len;
  }
  else {
    rc = (errno == FW_EAGAIN? FW_EAGAIN : FW_ERROR);
  }
    
  fw_mailbox_restore_interrupts(original_msr);

  return rc;
}

int fw_mailbox_putn(const char* str, unsigned length) {
  return fw_mailbox_putstring(str, length, 0);
}

int fw_mailbox_put(const char* str) {

  if (str == 0) {
    return 0;
  }

  int rc = fw_mailbox_putstring(str, strlen(str) + 1, 0);

  return  (rc > 0) ? ( rc - 1 ) /* allow for null termination */ : rc;
}

int fw_mailbox_puts(const char* str) {
  return fw_mailbox_putstring(str, strlen(str) + 2, 1); // allow for null termination and new line
}

  

int fw_mailbox_writeRASEvent(fw_uint32_t message_id, fw_uint16_t num_details, fw_uint64_t details[]) {
  
  int i;
  
  unsigned length   = sizeof(MailBoxHeader_t) + sizeof(MailBoxPayload_RAS_t) + ( sizeof(uint64_t) * num_details );

  if (length > FW_OUTBOX_SIZE) {
    return FW_TOO_BIG;
  }

  int rc = 0, errno = 0;

  uint64_t current_msr = fw_mailbox_mask_interrupts();
  uint64_t reserve  = fw_mailbox_reserve(ROUND_2_QW_BOUNDARY(length), &errno);

  if (reserve != FW_MBOX_ERROR) {

    uint8_t* mbox_ptr = fw_MBox2Host + (reserve & (FW_OUTBOX_SIZE-1));

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    //  NOTE: This code is sensitive to the actual layout of the RAS
    //        payload structure (see mailbox.h).  It also assumes
    //        that the mailbox pointer starts on a quadword boundary.
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // Format the header:
    fw_mailbox_set_header( (MailBoxHeader_t*)mbox_ptr, JMB_CMD2HOST_RAS, length-sizeof(MailBoxHeader_t));
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(MailBoxHeader_t), fw_MBox2Host, FW_OUTBOX_SIZE);


    *((uint64_t*)mbox_ptr) = FW_Personality.Kernel_Config.UCI;
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(uint64_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    *((uint32_t*)mbox_ptr) = message_id;
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(uint32_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    *((uint16_t*)mbox_ptr) = 0; // RESERVED
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(uint16_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    *((uint16_t*)mbox_ptr) = num_details;
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(uint16_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    /*
     * Now add in the details, one by one:
     */

    for (i = 0; i < num_details; i++) {
      *((uint64_t*)mbox_ptr) = details[i];
      INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t),fw_MBox2Host, FW_OUTBOX_SIZE);
    }

    /*
     * Release the packet:
     */

    fw_mailbox_release(reserve, ROUND_2_QW_BOUNDARY(length));
    rc = 0;
  }
  else {
    rc = (errno == FW_EAGAIN ? FW_EAGAIN : FW_ERROR);
  }

  fw_mailbox_restore_interrupts(current_msr);
  return rc;
}

int fw_mailbox_writeRASString(uint32_t message_id, const char* msg ) {
  
  int i;
  unsigned n = strlen(msg);
  
  unsigned length   = sizeof(MailBoxHeader_t) + sizeof(MailBoxPayload_RAS_ASCII_t) + n ; 

  if (length > FW_OUTBOX_SIZE) {
    return FW_TOO_BIG;
  }

  int rc = 0, errno = 0;

  uint64_t current_msr = fw_mailbox_mask_interrupts();
  uint64_t reserve  = fw_mailbox_reserve(ROUND_2_QW_BOUNDARY(length), &errno);

  if (reserve != FW_MBOX_ERROR) {

    uint8_t* mbox_ptr = fw_MBox2Host + (reserve & (FW_OUTBOX_SIZE-1));

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    //  NOTE: This code is sensitive to the actual layout of the RAS
    //        payload structure (see mailbox.h).  It also assumes
    //        that the mailbox pointer starts on a quadword boundary.
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // Format the header:
    fw_mailbox_set_header( (MailBoxHeader_t*)mbox_ptr, JMB_CMD2HOST_RAS_ASCII, length-sizeof(MailBoxHeader_t));
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(MailBoxHeader_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    *((uint64_t*)mbox_ptr) = FW_Personality.Kernel_Config.UCI;
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(uint64_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    *((uint32_t*)mbox_ptr) = message_id;
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(uint32_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    /*
     * Now add in the details, one by one:
     */

    for (i = 0; i < n; i++) {
      *((char*)mbox_ptr) = msg[i];
      INCREMENT_AND_WRAP(mbox_ptr, sizeof(char),fw_MBox2Host, FW_OUTBOX_SIZE);
    }

    /*
     * Release the packet:
     */

    fw_mailbox_release(reserve, ROUND_2_QW_BOUNDARY(length));
    rc = 0;
  }
  else {
    rc = (errno == FW_EAGAIN ? FW_EAGAIN : FW_ERROR);
  }

  fw_mailbox_restore_interrupts(current_msr);
  return rc;
}

/**
 * @brief Issues a termination message to the host control system.
 * @param[in] status The status (exit) code.  This is passed to the
 *     host control system.
 * @return Zero (0) if the message was successfully sent; non-zero 
 *     otherwise.
 */

int fw_mailbox_terminate(int status) {

  int rc = 0, errno = 0;

  // Only allow a single exit code to be presented to the control system.

  if ( BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_EXIT_STATUS_GATE) != 0 ) {
      return FW_ERROR;
  }

  uint64_t current_msr = fw_mailbox_mask_interrupts();
  unsigned length   = sizeof(MailBoxHeader_t) + sizeof(MailBoxPayload_Terminate_t);
  uint64_t reserve  = fw_mailbox_reserve(ROUND_2_QW_BOUNDARY(length), &errno);

  if (reserve != FW_MBOX_ERROR) {
    uint8_t* mbox_ptr = fw_MBox2Host + (reserve & (FW_OUTBOX_SIZE-1));

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    //  NOTE: This code is sensitive to the actual layout of the 
    //        payload structure (see mailbox.h).  It also assumes
    //        that the mailbox pointer starts on a quadword boundary.
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /*
     * Format the header:
     */

    fw_mailbox_set_header( (MailBoxHeader_t*)mbox_ptr, JMB_CMD2HOST_TERMINATE, length-sizeof(MailBoxHeader_t));
    INCREMENT_AND_WRAP(mbox_ptr,sizeof(MailBoxHeader_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    /*
     * Add the RAS Payload header fields.  Note that this data
     * currently occupies a quadword and we make use of that
     * fact here:
     */

    *((uint64_t*)mbox_ptr) = GetTimeBase(); 
    INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    *((uint32_t*)mbox_ptr) = status; 
    INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Host, FW_OUTBOX_SIZE);

    /*
     * Release the packet:
     */

    fw_mailbox_release(reserve, ROUND_2_QW_BOUNDARY(length));
  }
  else {
    rc = (errno == FW_EAGAIN) ? FW_EAGAIN : FW_ERROR;
  }
 
 fw_mailbox_restore_interrupts(current_msr);
  return rc;
}

int fw_mailbox_sendBlockStatus(fw_uint16_t status, fw_uint16_t numArgs, fw_uint64_t args[] ) {

  int errno = 0;

  switch (status) {
  case JMB_BLOCKSTATE_INITIALIZED :
  case JMB_BLOCKSTATE_HALTED :
  {
      if ( numArgs != 0 ) return FW_ERROR;
      break;
  }

  case JMB_BLOCKSTATE_IO_LINK_CLOSED :
  {
      if ( numArgs != 1 ) return FW_ERROR;
      break;
  }

  default : 
  {
      return FW_ERROR;
  }
  }


  uint64_t current_msr = fw_mailbox_mask_interrupts();
  unsigned length   = sizeof(MailBoxHeader_t) + sizeof(MailBoxPayload_BlockState_t);
  uint64_t reserve;

  do {
      reserve = fw_mailbox_reserve(ROUND_2_QW_BOUNDARY(length), &errno);
  } while ( reserve == FW_MBOX_ERROR );

  uint8_t* mbox_ptr = fw_MBox2Host + (reserve & (FW_OUTBOX_SIZE-1));

  fw_mailbox_set_header( (MailBoxHeader_t*)mbox_ptr, JMB_CMD2HOST_BLOCK_STATE, length-sizeof(MailBoxHeader_t));
  INCREMENT_AND_WRAP(mbox_ptr,sizeof(MailBoxHeader_t), fw_MBox2Host, FW_OUTBOX_SIZE);

  *((uint16_t*)mbox_ptr) = status; // block_state
  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint16_t), fw_MBox2Host, FW_OUTBOX_SIZE);

  *((uint16_t*)mbox_ptr) = 0; // _unused
  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint16_t), fw_MBox2Host, FW_OUTBOX_SIZE);

  if ( status == JMB_BLOCKSTATE_IO_LINK_CLOSED ) {
      *((uint32_t*)mbox_ptr) = args[0]; // block_id
  }
  else {
      *((int32_t*)mbox_ptr) = 0;
  }

  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Host, FW_OUTBOX_SIZE);
  
  *((uint64_t*)mbox_ptr) = GetTimeBase();
  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Host, FW_OUTBOX_SIZE);


  fw_mailbox_release(reserve, ROUND_2_QW_BOUNDARY(length));
  fw_mailbox_restore_interrupts(current_msr);

  return FW_OK;
}


void fw_mailbox_waitForQuiesced() {
    do
    {
    } while( fw_mailbox_pollOutbox() );
}


static void acquire_mapping(uint64_t vaddr)
{
  // If vaddr is beyond the "usual" first GB of DDR, add a temporary TLB entry
  // to cover the target area.  It will be unmapped in release_mapping(). 
  if ( vaddr >= FW_1GB ) {
    tlbwe(
	  MAS1_V(1) | MAS1_TID(0) | MAS1_TS(0) | MAS1_TSIZE_1GB,
	  MAS2_EPN((vaddr & ~(FW_1GB-1)) >> 12) | MAS2_W(0) | MAS2_I(0) |
	    MAS2_M(1) | MAS2_G(0) | MAS2_E(0),
	  MAS7_3_RPN( (vaddr & ~(FW_1GB-1)) >> 12 ) | MAS3_U0(1) |
	    MAS3_SR(1) | MAS3_SW(1) | MAS3_SX(1) |
	    MAS3_UR(0) | MAS3_UW(0) | MAS3_UX(0) | MAS3_U3(1),
	  MAS8_TGS(0) | MAS8_VF(0) | MAS8_TLPID(0),
	  MMUCR3_X(0) | MMUCR3_R(1) | MMUCR3_C(1) | MMUCR3_ECL(0) |
	    MMUCR3_CLASS(1) | MMUCR3_ThdID(0xF)
    );
  }
}

static void release_mapping(uint64_t vaddr)
{
  // If necessary, unmap the temporary TLB entry.
  if ( vaddr >= FW_1GB ) {
    tlbilx((void*) vaddr, MAS6_SPID(0) | MAS6_ISIZE_1GB | MAS6_SAS(0));
  }
}


/**
 * @brief Processes one inbound message from the JTAG inbox.
 *     Note that it is assumed that such a message exists.
 * @return The message ID that identifies the type of message
 *     that was processed (see the JMB_CMD2CORE_* mnemonics in
 *     mailbox.h).  Or -1 if no legal message was detected.
 */

//#define _MBTRACE(x) printf x
#define _MBTRACE(x)

int fw_mailbox_process_inbound_msg( void ) {

  MailBoxHeader_t* header;
  uint8_t* mbox_ptr;
  uint64_t current_msr = fw_mailbox_mask_interrupts();
  int rc = 0;

#if 0

  // @todo : fix the locking mechanism.  It is broken at the momement
  //         because we have a common lock used for inbox and outbox.


  // NOTE: For now, we use a coarse grained lock on the entire
  //       inbox processing mailbox.
#if USESEMAPHORE
  fw_semaphore_down(FW_INBOX_LOCK);
#else
  fw_ticket_wait(BeDRAM_LOCKNUM_MBOX_TICKET, BeDRAM_LOCKNUM_MBOX_SERVE);
#endif

#endif


  uint64_t bytes_read = TI_DCR->mailbox_reg3[0];

  mbox_ptr = fw_MBox2Core + ( bytes_read & (FW_INBOX_SIZE-1) );
  header   = (MailBoxHeader_t*)mbox_ptr;

  INCREMENT_AND_WRAP(mbox_ptr, sizeof(MailBoxHeader_t), fw_MBox2Core, FW_INBOX_SIZE);

  //! @todo CRC check?

  switch ( header->usCmd ) 
    {
    
    case JMB_CMD2CORE_NONE : 
      {
	break;
      }

    case JMB_CMD2CORE_WRITE : 
      {

	uint64_t dest_vaddr = *((uint64_t*)mbox_ptr); INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	uint64_t count      = *((uint64_t*)mbox_ptr); INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);

	//if ( dest_vaddr >= 0x3C0000000 ) printf("WRITE(addr=%lX len=%ld)\n", dest_vaddr, count);

	acquire_mapping(dest_vaddr);

	if ( count > ( FW_INBOX_SIZE - sizeof(MailBoxHeader_t) - sizeof(MailBoxPayload_Write_t) ) ) {
	  return - __LINE__ ;
	}

	if ( (mbox_ptr + count) >= (fw_MBox2Core + FW_INBOX_SIZE) ) { // wrap around end of the inbox?
	  size_t len1 = FW_INBOX_SIZE - (mbox_ptr - fw_MBox2Core);
	  size_t len2 = count - len1;
	  memcpy( (void*)dest_vaddr, mbox_ptr, len1 );
	  memcpy( (void*)(dest_vaddr+len1), fw_MBox2Core, len2);
	}
	else {
	  memcpy( (void*)dest_vaddr, mbox_ptr, count );
	}

	if ((header->usCrc != 0) &&
	  (Crc16n(0xffff, (uint8_t *) dest_vaddr, count) != header->usCrc)) {
	  rc = 0xffff;
	}

	release_mapping(dest_vaddr);

	_MBTRACE(( "JMB_CMD2CORE_WRITE vaddr=%lX len=%ld\n",dest_vaddr,count));
	break;
      }

    case JMB_CMD2CORE_FILL : 
      {
 
	uint32_t* dest_vaddr = (uint32_t*) *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	uint64_t  count      = *((uint64_t*)mbox_ptr);              INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	uint32_t  value      = *((uint32_t*)mbox_ptr);


	acquire_mapping((uint64_t) dest_vaddr);
	count /= sizeof(uint32_t);
	while (count--) {
	  *(dest_vaddr++) = value;
	}
	release_mapping((uint64_t) dest_vaddr);

	_MBTRACE(( "JMB_CMD2CORE_FILL vaddr=%p len=%ld val=%X\n",dest_vaddr,count,value));
	break;
      }

    case JMB_CMD2CORE_ELF_SCTN_CRC :
      {
	// NOTE: like all mailbox messages, this one might wrap.  And, we must be sensitive to the fact that 
	//       we are running on a small stack.  So pull the individual CRC entries off one at a
	//       time:

	uint32_t num_entries  = (uint32_t)  *((uint32_t*)mbox_ptr);      INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);
	/*uint32_t _unused      = (uint32_t)  *((uint32_t*)mbox_ptr);*/  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);

	_MBTRACE(( "JMB_CMD2CORE_ELF_SCTN_CRC num=%d \n",num_entries));

	while ( ( num_entries > 0 ) && ( rc == 0 ) ) {
	  
	  MailBoxPayload_crcEntry_t crc;

	  crc.start = *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	  crc.size  = *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	  crc.crc   = *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);

	  if ( fw_mailbox_perform_section_crc( &crc ) != 0 ) {

	    // Note: Ralph is requesting that we actually terminate here.  This is because of the
	    // lack of good RAS monitors in various environments.
	    // @todo: how will this behavie in the presence of "no RAS disconnect"?

	    rc = - __LINE__;
	  }

	  num_entries--;
	}

	break;
	
      }

    case JMB_CMD2CORE_CONFIGURE_DOMAINS :
      {

	uint32_t num_domains  = *((uint32_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);
    /* _reserved*/                                   INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);
	int d;


	_MBTRACE(( "JMB_CMD2CORE_CONFIGURE_DOMAINS num_domains=%d\n",num_domains));

	for ( d = 0; d < JMB_MAX_DOMAINS; d++ ) {

	    uint32_t coreMask  = *((uint32_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);
	    /* _reserved */                               INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);
	    uint64_t ddrStart  = *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	    uint64_t ddrEnd    = *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	    uint64_t configPtr = *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	    uint32_t configLen = *((uint32_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);

	    _MBTRACE(( "JMB_CMD2CORE_CONFIGURE_DOMAINS domain=%d coreMask=%X ddrStart=%lX ddrEnd=%lX\n",d, coreMask, ddrStart, ddrEnd));
	    _MBTRACE(( "JMB_CMD2CORE_CONFIGURE_DOMAINS cfgPtr=%lX cfgLen=%d\n",                            configPtr, configLen      ));

	    int i;


	    // There is an extra word of padding in each domain descriptor:

	    /* padding */   INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);

	    if ( d < num_domains ) {

		char* option = FW_InternalState.nodeState.domain[d].options;

		for ( i = 0; i < sizeof(FW_InternalState.nodeState.domain[d].options); i++ ) {
		    option[i] = *((char*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(char), fw_MBox2Core, FW_INBOX_SIZE);
		}

		FW_InternalState.nodeState.domain[d].coreMask      = coreMask & FW_InternalState.nodeState.coreMask;
		FW_InternalState.nodeState.domain[d].ddrOrigin     = ddrStart;
		FW_InternalState.nodeState.domain[d].configAddress = configPtr;
		FW_InternalState.nodeState.domain[d].configLength  = configLen;
		if ( ddrEnd == -1 ) {
		    FW_InternalState.nodeState.domain[d].ddrEnd = (uint64_t)FW_Personality.DDR_Config.DDRSizeMB * 1024ull * 1024ull  -  1;
		}
		else {

		    if ( ddrEnd > ( (uint64_t)FW_Personality.DDR_Config.DDRSizeMB * 1024ull * 1024ull ) ) {
			FW_Error( "End of memory for domain %d (%lX) exceeds configured end of memory (%lX).", d, ddrEnd, (uint64_t)FW_Personality.DDR_Config.DDRSizeMB * 1024ull * 1024ull );
			//rc = - __LINE__;
		    }

		    FW_InternalState.nodeState.domain[d].ddrEnd = ddrEnd;
		}
	    }
	    else {
		for ( i = 0; i < sizeof(FW_InternalState.nodeState.domain[0].options); i++ ) {
		    INCREMENT_AND_WRAP(mbox_ptr, sizeof(char), fw_MBox2Core, FW_INBOX_SIZE);
		}
	    }

	}

	break;
      }

    case JMB_CMD2CORE_READ : 
      {
	//MailBoxPayload_Read_t* read = (MailBoxPayload_Read_t*)payload;
	//! @todo what to do w/ read?
	rc = - __LINE__;
	break;
      }

    case JMB_CMD2CORE_LAUNCH : 
      {
	// NOTE: we assume 2 domains here for simplicity.  We grab both
	//       regardless of how many domains were configured
	FW_InternalState.nodeState.domain[0].entryPoint = *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);
	FW_InternalState.nodeState.domain[1].entryPoint = *((uint64_t*)mbox_ptr);  INCREMENT_AND_WRAP(mbox_ptr, sizeof(uint64_t), fw_MBox2Core, FW_INBOX_SIZE);

	_MBTRACE(( "JMB_CMD2CORE_LAUNCH entry[dom0]=%llX entry[dom1]=%llX\n", FW_InternalState.nodeState.domain[0].entryPoint, FW_InternalState.nodeState.domain[1].entryPoint));

	break;
      }

    case JMB_CMD2CORE_STDIN : 
      {
	//! @todo implement
	rc = - __LINE__;
	break;
      }

    case JMB_CMD2CORE_BARRIER_ACK :
      {
	// There is no payload at this time.
	break;
      }

    default : 
      {
	rc = - __LINE__;
	break;
      }
    }

  if (rc == 0xffff) {
    TI_DCR->usercode2[0] = 1;		// report error to control system
    while (TI_DCR->usercode2[0] != 0);	// wait for acknowledgement
    rc = 0;				// ignore the error
  }
  
  bytes_read += ROUND_2_QW_BOUNDARY(sizeof(MailBoxHeader_t) + (header->usPayloadLen));
  TI_DCR->mailbox_reg3[0] = bytes_read;

#if 0

#if USESEMAPHORE
  fw_semaphore_up(FW_INBOX_LOCK);
#else
  fw_ticket_post(BeDRAM_LOCKNUM_MBOX_SERVE);
#endif

#endif

  fw_mailbox_restore_interrupts(current_msr);
  
  //! @todo Issue RAS event when a bad packet has been detected?

  return (rc == 0)  ? header->usCmd : rc;
}


/**
 * @brief Polls the inbox for a code load sequence.  Such a sequence is simply zero
 *    or more inbox commands terminated with a "launch" command.  
 */

int fw_mailbox_poll_inbox_load( void ) {

  uint64_t reg2, reg3;
  // NOTE: we need a very long timeout, at least for the initial message, to give
  //       the conrol system time to process any printf's we've generated.
  uint64_t timeout_latency = 3ull * 60ull * 1000ull * 1000ull * (uint64_t)FW_Personality.Kernel_Config.FreqMHz; // three minutes
  uint64_t timeout = GetTimeBase() + timeout_latency;

  for ( ; ; ) {

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * Read the inbox DCRs.  A message has arrived if they are not
     * equal.
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

    reg2 = TI_DCR->mailbox_reg2[0];
    reg3 = TI_DCR->mailbox_reg3[0];

    if ( reg2 != reg3 ) {

      int rc = fw_mailbox_process_inbound_msg();

      if ( rc == JMB_CMD2CORE_LAUNCH ) {
	return 0; // Launch command detected ... we're done
      }
      else if (rc < 0) {
	const char* msg = "(E) Bad Inbound Mailbox Message";
	fw_putn(msg,strlen(msg));
	return rc;
      }
      else {
	timeout = GetTimeBase() + timeout_latency; // compute a new timeout value
      }
    }
    else if ( GetTimeBase() > timeout ) {
      const char* msg = "(E) Mailbox load timeout!";
      fw_putn(msg, strlen(msg));
      return -1; // timed out waiting
    }
  }
}

/**
 * @brief  Executes the JTAG code loading sequence.  It is assumed that this
 *     function is called on all hardware threads.  It will provide the
 *     appropriate threading model for the load.
 */

int fw_mailbox_load_via_jtag() {

  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Implementation notes:
   *
   *   1) Code is loaded only on the primordial thread (thread 0
   *      of core 0).  All other threads try to "play nice"
   *      while this is happening.
   *
   *   2) If anything goes wrong, we crash.
   *
   *
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
 
  if (ProcessorID() == 0) {

    int rc;

    // Suspend all other threads.  We must squirrel away the current state
    // for the resume code (below).
   
    uint64_t threadActive0 = DCRReadPriv(TESTINT_DCR(THREAD_ACTIVE0)); 
    uint64_t threadActive1 = DCRReadPriv(TESTINT_DCR(THREAD_ACTIVE1));

    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), _B1(0,1) );
    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), 0);

    // Send a message across JTAG to indicate that we are ready to load:
    fw_mailbox_sendReadyMessage();

    if ( (rc = fw_mailbox_poll_inbox_load() ) == 0) {
      BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_JTAG_LOAD_COMPLETE);
    }
    else {
      fw_mailbox_terminate(rc);
    }

    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), threadActive0 );
    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), threadActive1 );
  }
  else {
    
    ThreadPriority_Low();
    
    while ( ! BeDRAM_Read(BeDRAM_LOCKNUM_JTAG_LOAD_COMPLETE) )
      /* spin */ 
      fw_pdelay(1600)
      ;
    
    ThreadPriority_Medium();    
  }

  int fw_mmu_remap(void);

  fw_mmu_remap();

  return 0;
}

void fw_mailbox_sendReadyMessage( void ) {

  /*
   * Send the message indicating that we are ready:
   */

  int errno = 0;
  uint64_t reserve = fw_mailbox_reserve( ROUND_2_QW_BOUNDARY( sizeof(MailBoxHeader_t) ), &errno );

  if (reserve != FW_MBOX_ERROR) { 
    uint8_t* mbox_ptr = fw_MBox2Host + (reserve & (FW_OUTBOX_SIZE-1));
    fw_mailbox_set_header( (MailBoxHeader_t*)mbox_ptr, JMB_CMD2HOST_READY, 0 );
    fw_mailbox_release( reserve, ROUND_2_QW_BOUNDARY( sizeof(MailBoxHeader_t) ) );
  }
  else {
    crash( - __LINE__ );
  }
}

int fw_mailbox_perform_section_crc( MailBoxPayload_crcEntry_t* crc ) {

  if ( crc->crc == JMB_NO_CRC ) {
    return 0;
  }

  acquire_mapping(crc->start);
  unsigned long computed = Crc32n( 0xFFFFFFFF, (unsigned char*)crc->start, crc->size );
  release_mapping(crc->start);

  if ( computed != crc->crc ) {

    fw_uint64_t details[4];

    details[0] = crc->start;
    details[1] = crc->size;
    details[2] = crc->crc;
    details[3] = computed;

    fw_mailbox_writeRASEvent( FW_RAS_CRC_ERROR, 4, details );
    
    return -1;
  }

  return 0;
}

int fw_mailbox_barrier( fw_uint64_t timeoutInMicroseconds, fw_uint64_t warningThresholdInMicroseconds ) {

  int rc = FW_ERROR;
  uint64_t timeout = -1;
  uint64_t warningTimeout = 0;

  /*
   * Compute the timeout as a timebase value:
   */

  if ( timeoutInMicroseconds != 0 ) {
    timeout = GetTimeBase() + (timeoutInMicroseconds * FW_Personality.Kernel_Config.FreqMHz );
  }

  if ( warningThresholdInMicroseconds != 0 ) {
      warningTimeout = GetTimeBase() + (warningThresholdInMicroseconds * FW_Personality.Kernel_Config.FreqMHz);
  }

  /*
   * We only allow a single outstanding barrier request at a time,
   * so first acquire a lock:
   */

  if ( ( rc = fw_semaphore_down_w_timeout( BeDRAM_LOCKNUM_CS_BARRIER, timeoutInMicroseconds ) ) != 0 ) {
    rc = FW_EAGAIN;
    goto done;
  }


  /*
   * Send an outbound barrier request message:
   */

  int      errno = 0;
  uint64_t reserve  = fw_mailbox_reserve(ROUND_2_QW_BOUNDARY(sizeof(MailBoxHeader_t)), &errno);

  if ( reserve == FW_MBOX_ERROR ) {
    rc = FW_ERROR;
    goto unlock;
  }

  uint8_t* mbox_ptr = fw_MBox2Host + (reserve & (FW_OUTBOX_SIZE-1));
  
  fw_mailbox_set_header( (MailBoxHeader_t*)mbox_ptr, JMB_CMD2HOST_BARRIER_REQ, 0 );
  fw_mailbox_release( reserve, ROUND_2_QW_BOUNDARY( sizeof(MailBoxHeader_t) ) );


  /*
   * Wait for the response.  This is achieved by polling the head and 
   * tail "pointers" of the inbox.  At this time, there is no backoff
   * (e.g. spin/delay loop).  Any inbox messages that arrive are
   * discarded.
   */

  uint64_t head, tail;

  for ( ; GetTimeBase() < timeout ; ) {

    head = TI_DCR->mailbox_reg2[0];
    tail = TI_DCR->mailbox_reg3[0];

    if ( head != tail ) {

      int rc = fw_mailbox_process_inbound_msg();

      if ( rc == JMB_CMD2CORE_BARRIER_ACK ) {
	rc = FW_OK; 
	goto unlock;
      }
      else {
	//! @todo we might need a message queue here.
	printf("(W) Dropping message (type = %d) while waiting for barrier.\n", rc);
      }
    }
    else {
	if ( ( warningTimeout != 0 ) && ( GetTimeBase() > warningTimeout ) ) {
	    fw_mailbox_writeRASEvent( FW_CS_BARRIER_WARNING, 1, &warningThresholdInMicroseconds );
	    warningTimeout = 0;
	}
    }
  }

  rc = FW_TIMEOUT; // If we got here, we timed out

 unlock:
  fw_semaphore_up( BeDRAM_LOCKNUM_CS_BARRIER );

 done:
  return rc;
}


int fw_mailbox_get( const char* s, unsigned bufferLength ) {

  /*
   * Exit immeidately if there is no message:
   */

  if ( TI_DCR->mailbox_reg2[0] == TI_DCR->mailbox_reg3[0] ) {
    return FW_EAGAIN;
  }

  //!< @todo: need a lock here?

  uint64_t          bytesRead  = TI_DCR->mailbox_reg3[0];
  uint8_t*          inboxPtr   = fw_MBox2Core + ( bytesRead & (FW_INBOX_SIZE-1) );
  MailBoxHeader_t*  header     = (MailBoxHeader_t*)inboxPtr;
  uint16_t          msgLength  = header->usPayloadLen;
  MailBoxPayload_Stdin_t* stdinPtr = (MailBoxPayload_Stdin_t*) ((char*) header + sizeof(*header));

  /*
   * If the next message is not a stdin message, then give up:
   */

  if ( header->usCmd != JMB_CMD2CORE_STDIN ) {
    return FW_EAGAIN;
  }

  /*
   * If the message won't fit in the buffer, then give up:
   */

  if ( stdinPtr->count > bufferLength ) {
    return FW_TOO_BIG;
  }

  INCREMENT_AND_WRAP(inboxPtr, sizeof(MailBoxHeader_t), fw_MBox2Core, FW_INBOX_SIZE);

#if 0
  /*
   * The message may wrap around the end of the inbox:
   */

  if ( (inboxPtr + msgLength) >= (fw_MBox2Core + FW_INBOX_SIZE) ) {
    size_t len1 = FW_INBOX_SIZE - (inboxPtr - fw_MBox2Core);  // remainder of the inbox
    size_t len2 = msgLength - len1;                           // remainder of the message
    memcpy( (void*)s, inboxPtr, len1 );                       // copy part 1
    memcpy( (void*)(s+len1), fw_MBox2Core, len2);             // copy part 2
  }
  else {
    memcpy( (void*)s, inboxPtr, msgLength );
  }
#endif
  memcpy( (void*)s, stdinPtr->data, stdinPtr->count);

  /*
   * Advance the "tail" pointer (mailbox DCR)
   */

  bytesRead += ROUND_2_QW_BOUNDARY(sizeof(MailBoxHeader_t) + (msgLength));
  TI_DCR->mailbox_reg3[0] = bytesRead;

  return stdinPtr->count;
}

static int _fw_illegal_messages_expiry = 5;

int fw_mailbox_pollInbox( void* message, fw_uint32_t* messageType, unsigned bufferLength ) {

    
    /*
     * Exit immeidately if there is no message:
     */

    *messageType = JMB_CMD2CORE_NONE;

    if ( TI_DCR->mailbox_reg2[0] == TI_DCR->mailbox_reg3[0] ) {
	return FW_EAGAIN;
    }

    //!< @todo: need a lock here?

    uint64_t          bytesRead  = TI_DCR->mailbox_reg3[0];
    uint8_t*          inboxPtr   = fw_MBox2Core + ( bytesRead & (FW_INBOX_SIZE-1) );
    MailBoxHeader_t*  header     = (MailBoxHeader_t*)inboxPtr;
    uint16_t          msgLength  = header->usPayloadLen; // stash this 
    int               rc         = 0;

    INCREMENT_AND_WRAP(inboxPtr, sizeof(MailBoxHeader_t), fw_MBox2Core, FW_INBOX_SIZE);

    *messageType = header->usCmd;

    switch ( header->usCmd ) {

    case JMB_CMD2CORE_STDIN : {

	MailBoxPayload_Stdin_t* stdinPtr = (MailBoxPayload_Stdin_t*) inboxPtr;
	MailBoxPayload_Stdin_t* buffer   = (MailBoxPayload_Stdin_t*) message; 

	/*
	 * If the message won't fit in the buffer, then give up:
	 */

	if ( stdinPtr->count > (bufferLength-sizeof(stdinPtr->count)) ) {
	    return FW_TOO_BIG;
	}

#if 0
	/*
	 * The message may wrap around the end of the inbox:
	 */

	if ( (inboxPtr + msgLength) >= (fw_MBox2Core + FW_INBOX_SIZE) ) {
	    size_t len1 = FW_INBOX_SIZE - (inboxPtr - fw_MBox2Core);  // remainder of the inbox
	    size_t len2 = msgLength - len1;                           // remainder of the message
	    memcpy( (void*)message, inboxPtr, len1 );                       // copy part 1
	    memcpy( (void*)(message+len1), fw_MBox2Core, len2);             // copy part 2
	}
	else {
	    memcpy( (void*)message, inboxPtr, msgLength );
	}
#endif
  
	buffer->count = stdinPtr->count;

	memcpy( buffer->data, stdinPtr->data, stdinPtr->count);

	rc = stdinPtr->count;

	break;
    }

    case JMB_CMD2CORE_CONTROL_SYSTEM_REQUEST : {

	if ( bufferLength < sizeof(MailBoxPayload_ControlSystemRequest_t) ) {
	    return FW_TOO_BIG;
	}
      
	MailBoxPayload_ControlSystemRequest_t* sysreq = (MailBoxPayload_ControlSystemRequest_t*)message;

	sysreq->sysreq_id = *((uint32_t*)inboxPtr);  
	INCREMENT_AND_WRAP(inboxPtr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);
       
      
	switch ( sysreq->sysreq_id ) {

	case JMB_CTRLSYSREQ_SHUTDOWN_IO_LINK : {
	    sysreq->details.shutdown_io_link.block_id = *((uint32_t*)inboxPtr);  INCREMENT_AND_WRAP(inboxPtr, sizeof(uint32_t), fw_MBox2Core, FW_INBOX_SIZE);
	    break;
	}

	case JMB_CTRLSYSREQ_SHUTDOWN : {
	    // nothing more to do
	    break;
	}

	default : {
	    const char* msg = "(E) Illegal sysreq.";
	    fw_putn( msg, strlen(msg) );
	}

	}

	rc = sizeof(MailBoxPayload_ControlSystemRequest_t);

	break;
    }

    default : {

	_fw_illegal_messages_expiry--;

	if ( _fw_illegal_messages_expiry <= 0 ) {
	    FW_Error( "Illegal Inbox header detected :  %X : %X", (uint64_t)header, *((uint64_t*)header) );
	    return FW_ERROR;
	}
      
	*messageType = JMB_CMD2CORE_NONE;
	return FW_EAGAIN;
    }
    }

    /*
     * Advance the "tail" pointer (mailbox DCR)
     */

    bytesRead += ROUND_2_QW_BOUNDARY(sizeof(MailBoxHeader_t) + (msgLength));
    TI_DCR->mailbox_reg3[0] = bytesRead;
    _fw_illegal_messages_expiry = 5; // reset the bad message down counter

    return rc;
}

int fw_mailbox_pollOutbox() {
    return TI_DCR->mailbox_reg0[0] != TI_DCR->mailbox_reg1[0];
}
