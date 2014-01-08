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
#include "Firmware_internals.h"
#include "Firmware_RAS.h"

void fw_ticket_wait(size_t taketicket_lock, size_t nowserving_lock) 
{
    uint64_t ticketnum = BeDRAM_ReadIncWrap(taketicket_lock);
    while(BeDRAM_Read(nowserving_lock) != ticketnum) 
    {
    }
}

int fw_ticket_wait_w_timeout(size_t taketicket_lock, size_t nowserving_lock, uint64_t microseconds) 
{
    uint64_t timeout = GetTimeBase() + microseconds * FW_Personality.Kernel_Config.FreqMHz;
    uint64_t ticketnum = BeDRAM_ReadIncWrap(taketicket_lock);
    while(BeDRAM_Read(nowserving_lock) != ticketnum)
    {
        if(GetTimeBase() > timeout)
        {
            return -1;
        }
    }
    return 0;
}

void fw_ticket_post( size_t nowserving_lock ) 
{
    BeDRAM_WriteAdd(nowserving_lock, 1);
}


void fw_semaphore_init( size_t lock_number, uint32_t number_of_available_resources) {
  //BeDRAM_WriteRaw(lock_number, number_of_available_resources);
  //BeDRAM_ReadIncWrap(lock_number);
  BeDRAM_Write(lock_number, number_of_available_resources);
}

void fw_semaphore_down( size_t lock_number ) {

#if 1
  while ( 1 ) {

    if ( (int64_t)BeDRAM_Read(lock_number) > 0 ) {

      // We might get in ... now decrement atomically and
      // make sure that we did. 

      if ( (int64_t)BeDRAM_ReadDecWrap(lock_number) > 0 ) {
	return;
      }
      else {
	// Nice try !
	BeDRAM_ReadIncWrap(lock_number);
      }
    }
  }
#endif
}

int fw_semaphore_down_w_timeout( size_t lock_number, uint64_t microseconds ) {

#if 1

  uint64_t timeout = 0; 


  // +------------------------------------------------------------------+
  // | NOTE: the implementation is optimistic in that we do not compute |
  // |      a timeout value unless the first attempt to acquire the     |
  // |      lock fails.  The first pass, of course, takes some amount   |
  // |      of time, but the timeout is not exact anyhow.               |
  // +------------------------------------------------------------------+

  do {

    if ( (int64_t)BeDRAM_Read(lock_number) > 0 ) {

      // We might get in ... now decrement atomically and
      // make sure that we did. 

      if ( (int64_t)BeDRAM_ReadDecWrap(lock_number) > 0 ) {
	return 0;
      }
      else {
	// Nice try !
	BeDRAM_ReadIncWrap(lock_number);
      }
    }

    if (timeout == 0) {
      timeout =
	GetTimeBase() +
	  (microseconds * (uint64_t)FW_Personality.Kernel_Config.FreqMHz );
    }

  } while ( GetTimeBase() < timeout );

  return -1;

#endif
}

void fw_semaphore_up( size_t lock_number ) {
  BeDRAM_ReadIncWrap(lock_number);
}

void fw_core_barrier( void )
{
  int core_barrier = BeDRAM_LOCKNUM_COREINIT(ProcessorCoreID()); 

  // This simple barrier implementation assumes that the same 4 threads
  // will participate in each barrier round.

  ppc_msync();
  uint64_t current = BeDRAM_ReadIncSat(core_barrier); 
  uint64_t target = (current & ~3ul) + 4;

  if ((current + 1) < target) {
    ThreadPriority_VeryLow(); // spin at low priority
    while ((BeDRAM_Read(core_barrier) < target) != 0) {
      fw_pdelay(1600);  // should be calibrated or perhaps parameterized
    }
    ThreadPriority_MediumHigh(); // restore priority
  }
  isync();
}


void fw_pdelay( uint64_t pclocks ) {
  uint64_t end = GetTimeBase() + pclocks;
  while ( GetTimeBase() < end )
    /* spin */
    ;
}

void fw_udelay( uint64_t microseconds ) {

  uint64_t end = GetTimeBase() + ( microseconds * FW_Personality.Kernel_Config.FreqMHz );

  while ( GetTimeBase() < end ) 
    /* spin */
    ;

}

size_t strlen(const char* s) {

  int result = 0;

  if (s) {
    while (*(s++))
      result++;
  }
  return result;
}

int puts( const char *str ) {
  return fw_putn(str,strlen(str));
}

extern int fw_vsprintf( char *buf, const char* fmt, va_list args );

int printf( const char *fmt, ... ) {
   va_list args;
   va_start( args, fmt );
   int len = 0;
   char buffer[128];
    

   len = fw_vsprintf( buffer, fmt, args );
   va_end( args );
   fw_putn( buffer, len);

   if (len > sizeof(buffer))
   {
      fw_putn("PRINTF OVERFLOWED\n", 18);
      crash(__LINE__);
   }
    
   return len;
}

void *memset( void *dest, int value, size_t len ) {

    char* c = (char*)dest;
    int i;

    for ( i = 0; i < len; i++, c++ ) {
	*c = value;
    }

    return dest;
}

void FW_Error( const char *fmt, ... ) {

   va_list args;
   va_start( args, fmt );

   int len = 0;
   char buffer[128];
    

   len = fw_vsprintf( buffer, fmt, args );
   va_end( args );
   buffer[len] = 0;

   fw_writeRASString( FW_RAS_ERROR, buffer);

   puts(buffer);

   if (len > sizeof(buffer)) {
       fw_putn("PRINTF OVERFLOWED\n", 18);
       asm volatile( "b 0"); //crash(__LINE__);
   }

}

void FW_Warning( const char *fmt, ... ) {

   va_list args;
   va_start( args, fmt );

   int len = 0;
   char buffer[128];
    

   len = fw_vsprintf( buffer, fmt, args );
   va_end( args );
   buffer[len] = 0;

   fw_writeRASString( FW_RAS_WARNING, buffer);

   if (len > sizeof(buffer)) {
       fw_putn("PRINTF OVERFLOWED\n", 18);
       asm volatile( "b 0"); //crash(__LINE__);
   }

}

void FW_RAS_printf( const uint32_t msg_id, const char* fmt, ... ) {

   va_list args;
   va_start( args, fmt );

   int len = 0;
   char buffer[128];
    

   len = fw_vsprintf( buffer, fmt, args );
   va_end( args );
   buffer[len] = 0;

   extern int _fw_writeRASString(uint32_t , const char*  );
   _fw_writeRASString( msg_id, buffer);

   if (len > sizeof(buffer)) {
       fw_putn("PRINTF OVERFLOWED\n", 18);
       asm volatile( "b 0"); //crash(__LINE__);
   }

}

// Warning: Assumes "force load/store alignment" is off, i.e., misaligned ld/st is ok!
void *memcpy( void *dest, const void *src, size_t n )
{
   uint64_t *s64 = (uint64_t *)src;
   uint64_t *d64 = (uint64_t *)dest;
//   dcache_block_touch( (void *)s64 );
   size_t h32  = (n >> 5);
   size_t t8, t1;
   register uint64_t ra, rb, rc, rd;

   while ( h32-- )
      {
//      dcache_block_touch( (void *)(s64 + 4) );
      ra     = *s64++;
      rb     = *s64++;
      rc     = *s64++;
      rd     = *s64++;
      *d64++ = ra;
      *d64++ = rb;
      *d64++ = rc;
      *d64++ = rd;
      }

   if ( (t8 = ((n & 0x18) >> 3)) )
      {
      while ( t8-- )
         *d64++ = *s64++;
      }

   if ( (t1 = (n & 0x7)) )
      {
      uint8_t *s8 = (uint8_t *)s64;
      uint8_t *d8 = (uint8_t *)d64;

      while ( t1-- )
         *d8++ = *s8++;
      }

   return dest;
}




int fw_putn( const char *str, unsigned len) {
    return fw_mailbox_putn(str, len);
}


typedef struct FW_RasHistoryEntry_t {
  uint32_t id;
  uint32_t age;
  uint32_t count;
  uint32_t _reserved;
  uint64_t timeOfFirstEvent;
  uint64_t timeOfLatestEvent;
} FW_RasHistoryEntry_t;

#define FW_RAS_HISTORY_SIZE 4


FW_RasHistoryEntry_t FW_RasHistory[FW_RAS_HISTORY_SIZE];


//#define _DEBUG_(x) printf x
#define _DEBUG_(x)

int _fw_writeRASEvent(fw_uint32_t message_id, fw_uint16_t number_of_details, fw_uint64_t details[]) { 
    return fw_mailbox_writeRASEvent(message_id, number_of_details, details);
}

int _fw_writeRASString(uint32_t message_id, const char* message ) { 
    return fw_mailbox_writeRASString( message_id, message );
}


void fw_flushRasHistoryEntry( FW_RasHistoryEntry_t* event, uint32_t messageCode ) {

    _DEBUG_(( "(>) %s event={%X,%d} msg=%X\n", __func__, event->id, event->count, messageCode));

  uint64_t details[2];
  details[0] = event->count;
  details[1] = event->id;

  while ( _fw_writeRASEvent( messageCode, 2, (fw_uint64_t *)details ) == FW_EAGAIN );


}

void fw_flushAllRasHistory() {

    _DEBUG_(( "(>) %s\n", __func__ ));

  fw_semaphore_down( BeDRAM_LOCKNUM_RAS_HISTORY );

  unsigned i;
  for ( i = 0; i < FW_RAS_HISTORY_SIZE; i++ ) {
    if ( FW_RasHistory[i].count > FW_RAS_EVENT_FILTER_THRESHOLD ) {
      fw_flushRasHistoryEntry( &(FW_RasHistory[i]), FW_RAS_STORM_WARNING );
    }
    FW_RasHistory[i].id    = 0;
    FW_RasHistory[i].count = 0;
    FW_RasHistory[i].timeOfLatestEvent = 0; // make this entry be old!
  }
  
  fw_semaphore_up( BeDRAM_LOCKNUM_RAS_HISTORY );

  fw_ddr_flushCorrectables(1);
  fw_l1p_flushCorrectables(1);
  fw_l2_flushCorrectables(1);
  fw_mu_flushCorrectables();
  fw_nd_flushCorrectables();
}


int fw_monitorRASEvent( uint32_t message_id ) {

  _DEBUG_(( "(>) %s msg=%X\n", __func__, message_id ));

  int rc = 0;
  unsigned i;

  // If we are in "extreme" mode, we do not filter any events.
  if ( ( FW_Personality.Kernel_Config.RASPolicy & PERS_RASPOLICY_VERBOSITY(-1) ) == PERS_RASPOLICY_EXTREME ) {
    return 0;
  }

  // Make a few CIOS messages exempt.
  if ( ( message_id == 0x000B0006 ) || ( message_id == 0x000B0007 ) ) {
      return 0;
  }


  // +----------------------------------------------------------------------+
  // | Update the RAS history.  This entails:                               |
  // |    o  Acquiring a lock for the history table.                        |
  // |    o  Locating the best entry in the history table.                  |
  // |    o  Flushing any cached events from the history table entry.       |
  // |    o  Updating the history table entry.                              |
  // |    o  Checking to see if a RAS storm is occurring.                   |
  // |    o  Releasing the lock for the history table.                      |
  // +----------------------------------------------------------------------+

  if ( fw_semaphore_down_w_timeout( BeDRAM_LOCKNUM_RAS_HISTORY, 200ul ) != 0 ) {
    return FW_EAGAIN;
  }

  FW_RasHistoryEntry_t* entry = NULL;
  FW_RasHistoryEntry_t* oldestEntry = NULL;
  uint64_t timestamp = GetTimeBase();

  /*
   * Select either the entry that matches our RAS event's ID, or the oldest entry in the cache.
   */

  for ( i = 0; i < FW_RAS_HISTORY_SIZE; i++ ) {

    if ( FW_RasHistory[i].id == message_id ) {
      entry = &(FW_RasHistory[i]);
      break;
    }

    if ( oldestEntry == NULL ) {
      oldestEntry = &(FW_RasHistory[i]);
    }
    else if ( oldestEntry->timeOfLatestEvent > FW_RasHistory[i].timeOfLatestEvent ) {
      oldestEntry = &(FW_RasHistory[i]);
    }
  }

  if ( entry == NULL ) {
    entry = oldestEntry;
  }

  /*
   * If the selected entry does not match the current RAS event, it might contain
   * accumulated (filtered) data.  In such an event, flush the data and establish
   * new credentials in the entry.
   */

  if ( entry->id != message_id ) {

    if ( entry->count > FW_RAS_EVENT_FILTER_THRESHOLD ) {
      fw_flushRasHistoryEntry( entry, FW_RAS_STORM_WARNING );
    }
    entry->count = 0;
    entry->id = message_id;
    entry->timeOfFirstEvent = timestamp;
  }

  /*
   * Update the count and timestamp.  If the count takes us over the 
   * error threshold, then issue the storm event.
   */

  entry->count++;
  entry->timeOfLatestEvent = timestamp;

  if ( entry->count == FW_RAS_EVENT_ERROR_THRESHOLD ) {

    uint32_t flushCode;
    uint64_t threshold = (uint64_t)entry->count * FW_RAS_STORM_THRESHOLD_MILLIS * FW_Personality.Kernel_Config.FreqMHz * (uint64_t)1000;

    if ( ( entry->timeOfLatestEvent - entry->timeOfFirstEvent ) < threshold ) {
      flushCode = FW_RAS_STORM_ERROR;
    }
    else {
      flushCode = FW_RAS_STORM_WARNING;
    }

    fw_flushRasHistoryEntry( entry, flushCode );
    entry->id = 0;
    entry->count = 0;
    entry->timeOfLatestEvent = 0;

    rc = 1;

    if ( flushCode == FW_RAS_STORM_ERROR ) {
	fw_semaphore_up( BeDRAM_LOCKNUM_RAS_HISTORY );
	Terminate(__LINE__);
    }
  }
  else if ( entry->count > FW_RAS_EVENT_FILTER_THRESHOLD ) {
    rc = 1;
  }
  else {
    rc = 0;
  }

  fw_semaphore_up( BeDRAM_LOCKNUM_RAS_HISTORY );


  return rc;

}

int fw_writeRASEvent(fw_uint32_t message_id, fw_uint16_t number_of_details, fw_uint64_t details[]) { 

  if ( fw_monitorRASEvent( message_id ) == 0 ) {
    return _fw_writeRASEvent( message_id, number_of_details, details );
  }
  else {
    return FW_OK;
  }
}


int fw_writeRASString(uint32_t message_id, const char* message ) { 

  if ( fw_monitorRASEvent( message_id ) == 0 ) {
      return _fw_writeRASString( message_id, message );
  }
  else {
    return FW_OK;
  }
}

//
// Barrier until all threads call exit. Last one in shuts down the run.
//

int fw_exit_status = 0;

void __NORETURN exit( int status ) {

  fw_exit_status |= status;

  uint64_t done = BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_EXIT) + 1; // post increment!

  if ( TRACE_ENABLED(TRACE_Exit_Sum) ) {
      printf("(I) Exit[%d:%d]: %ld of %ld threads : status = %x\n", ProcessorCoreID(), ProcessorThreadID(), done, BeDRAM_Read(BeDRAM_LOCKNUM_FW_THREADS_PRESENT), status );
  }

  if ( done == BeDRAM_Read(BeDRAM_LOCKNUM_FW_THREADS_PRESENT) ) {
      fw_pcie_shutdown();
      fw_flushAllRasHistory();
      fw_mailbox_terminate(fw_exit_status);
  }

  fw_sleep_forever();
}


#define ADD_SPR_TO_DETAILS(sprn) details[n++] = sprn; details[n++] = mfspr(sprn);

void __NORETURN unexpectedInterrupt( int status ) {

    fw_uint64_t details[2*10 + 1];
    unsigned n = 0;

    //printf("(E) %s status=%x\n", __func__, status );
    details[n++] = status;
    ADD_SPR_TO_DETAILS(SPRN_SRR0);
    ADD_SPR_TO_DETAILS(SPRN_SRR1);
    ADD_SPR_TO_DETAILS(SPRN_CSRR0);
    ADD_SPR_TO_DETAILS(SPRN_CSRR1);
    ADD_SPR_TO_DETAILS(SPRN_MCSRR0);
    ADD_SPR_TO_DETAILS(SPRN_MCSRR1);
    ADD_SPR_TO_DETAILS(SPRN_ESR);
    ADD_SPR_TO_DETAILS(SPRN_DEAR);
    ADD_SPR_TO_DETAILS(SPRN_LR);

    fw_mailbox_writeRASEvent( FW_RAS_UNEXPECTED_INTERRUPT, n, details );
    fw_flushAllRasHistory();
    fw_mailbox_terminate(status);
    fw_sleep_forever();
}

//
// Terminate the run immediately.
//
void __NORETURN crash( int status ) {

  int exitNum = BeDRAM_ReadIncSat(BeDRAM_LOCKNUM_EXIT);

  if ( TRACE_ENABLED(TRACE_Exit_Sum) ) {

      printf("%s[%d:%d]: status = %d.\n", 
	   ( (status < 0) ? "(E) Crash" : "(I) Terminate"),
	   ProcessorCoreID(), 
	   ProcessorThreadID(), 
	   status 
	   );
  }

  if ( exitNum == 0 ) {
      fw_pcie_shutdown();
  }

  fw_flushAllRasHistory();
  fw_mailbox_terminate(status);

  fw_sleep_forever();
}

//
// Terminate the run immediately.
//
void __NORETURN Terminate( int status )
{

    if ( TRACE_ENABLED(TRACE_Exit_Sum) && ( status != 0 ) ) {
	fw_uint64_t details[8];
	unsigned n = 0;
	details[n++] = status;
	details[n++] = mfspr(SPRN_LR);
	details[n++] = mfspr(SPRN_SRR0);
	details[n++] = mfspr(SPRN_SRR1);
	details[n++] = mfspr(SPRN_ESR);
	details[n++] = mfspr(SPRN_DEAR);
	fw_mailbox_writeRASEvent( FW_RAS_TERMINATION, n, details );
    }
    crash(status);
}
