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


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// get the goodies
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <firmware/include/mailbox.h>

void  fw_mailbox_sendReadyMessage(void);

// [32,126]

#define ISPRINTABLE(c) ( ((c) >= 32) && ( (c) <= 126 ) )

char* toString( char* binary, char* buffer, int length ) {

  char* p = buffer;

  while ( length > 0 ) {
    *p = ISPRINTABLE(*binary) ? *binary : '.';
    binary++;
    p++;
    length--;
  }

  *p = 0;

  return buffer;
}

void dumpMemory( void* addr, int length ) {

  uint64_t current = (uint64_t)addr;
  char buffer[sizeof(uint64_t)*4+1];

  if ( ( current & 7 ) != 0 ) {
    printf("(E) unaligned access (%lX)\n", current);
    return;
  }
  
  while ( length > 0 ) {
    uint64_t* p = (uint64_t*)current;
    printf("%lX  :  %lX %lX %lX %lX /* %s */\n", current, p[0], p[1], p[2], p[3], toString( (char*)p, buffer, sizeof(uint64_t)*4 ) );
    current += sizeof(uint64_t)*4;
    length -= sizeof(uint64_t)*4;
  }
}

const char* cmd2str( uint16_t cmd ) {
  switch (cmd) 
    {
    case JMB_CMD2CORE_NONE : return "None";
    case JMB_CMD2CORE_WRITE : return "Write";
    case JMB_CMD2CORE_FILL : return "Fill";
    case JMB_CMD2CORE_READ : return "Read";
    case JMB_CMD2CORE_LAUNCH : return "Launch";
    case JMB_CMD2CORE_STDIN : return "Stdin";
    case JMB_CMD2CORE_BARRIER_ACK : return "Barrier-Ack";
    case JMB_CMD2CORE_CONFIGURE_DOMAINS : return "Configure-Domains";
    case JMB_CMD2CORE_ELF_SCTN_CRC : return "Elf-Sctn-CRC";
    default : return "???";
    }
}

#define FW_INBOX_SIZE   ( 4 * 1024 ) // This must match what is actually in fw_mbox.c!

#define INCREMENT_AND_WRAP(ptr,size,origin,mbox_size)	\
  ptr += (size);					\
  if ( (ptr) >= (origin + mbox_size) )			\
    ptr = origin;

#define ROUND_2_QW_BOUNDARY(a) ( ( (a) + 15 ) & ~0xF )

int main( int argc, char *argv[], char **envp ) {

  if ( ProcessorID() == 0 ) {

    int done = 0;
    int n = 0;
    uint64_t inbox  = 0x3FFFFFC0000ul | ( DCRReadPriv( TESTINT_DCR(USERCODE1) ) & 0xFFFFFFFFul );

    // Tell the control system that we are ready:
    fw_mailbox_sendReadyMessage();

    // Shut down all other threads:
    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE0), _B1(0,1) );
    DCRWritePriv( TESTINT_DCR(THREAD_ACTIVE1), 0);

    // Poll the inbox:
    for ( n = 1; ! done ; n++ ) {

      uint64_t tail;
      uint64_t head;
      uint64_t mbox;

      printf("Waiting for inbox message number %d...\n", n);

      do {
	tail = DCRReadPriv( TESTINT_DCR(MAILBOX_REG2) );
        head = DCRReadPriv( TESTINT_DCR(MAILBOX_REG3) );
      } while ( tail == head );
      
      // Testint reg 3 contains the total number of bytes written
      // to the inbox.  So construct a pointer to the current
      // message header:

      mbox = inbox + ( head & ( FW_INBOX_SIZE - 1 ) );
      
      MailBoxHeader_t* hdr = (MailBoxHeader_t*)mbox;

      printf("Msg %d : usCmd=%d (%s) len=%d id=%d crc=%d\n", n, hdr->usCmd, cmd2str(hdr->usCmd), hdr->usPayloadLen, hdr->usID, hdr->usCrc );
      
      INCREMENT_AND_WRAP( mbox, sizeof(MailBoxHeader_t), inbox, FW_INBOX_SIZE );

      if ( (mbox + hdr->usPayloadLen) > (inbox + FW_INBOX_SIZE) ) {

	// Dump in two pieces:

	uint16_t len1 = (uint16_t)(inbox + FW_INBOX_SIZE - mbox);

	dumpMemory( (void*)mbox,  len1 );
	dumpMemory( (void*)inbox, (uint16_t)(hdr->usPayloadLen - len1 ) );
      }
      else {
	dumpMemory( (void*)mbox, (int)hdr->usPayloadLen );
      }
      

      // Acknowledge the message by updating the tail pointer:
      head += ROUND_2_QW_BOUNDARY(sizeof(MailBoxHeader_t) + (hdr->usPayloadLen));
      DCRWritePriv( TESTINT_DCR( MAILBOX_REG3 ), head );
    }
  }

  return 0;


#if 0

  uint64_t outbox = 0x3FFFFFC0000ul | ( DCRReadPriv( TESTINT_DCR(USERCODE0) ) & 0xFFFFFFFFul );

  printf("inbox:%lX outbox:%lX\n", inbox, outbox );

  //dumpMemory( (void*)0x3fffffd5700ul, 1024 );
  //dumpMemory( (void*)outbox, 1024 );


  dumpMemory( (void*)0x3fffffc0000ul, 256 * 1024 * 1024 );

#endif

    
}


