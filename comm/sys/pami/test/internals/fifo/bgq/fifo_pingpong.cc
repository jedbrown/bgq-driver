/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file test/internals/fifo/bgq/fifo_pingpong.cc
 * \brief ???
 */

#include "../test.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "components/memory/heap/HeapMemoryManager.h"
#include "components/memory/shmem/SharedMemoryManager.h"

#include "Global.h"

#include "components/atomic/native/NativeCounter.h"
#include "components/atomic/indirect/IndirectCounter.h"

//#include "components/devices/shmem/ShmemPacket.h"

#include "components/fifo/FifoPacket.h"
#include "components/fifo/wrap/WrapFifo.h"
#include "components/fifo/linear/LinearFifo.h"
#include "components/atomic/bgq/L2CounterBounded.h"

//#include "components/devices/shmem/wakeup/WakeupBGQ.h"

typedef PAMI::Fifo::FifoPacket <32, 128> ShmemPacket;

//typedef PAMI::Fifo::WrapFifo<ShmemPacket, PAMI::BoundedCounter::BGQ::IndirectL2Bounded, 128, PAMI::Wakeup::BGQ> Wrap;
typedef PAMI::Fifo::WrapFifo<ShmemPacket, PAMI::BoundedCounter::BGQ::IndirectL2Bounded, 128> Wrap;

//typedef Fifo::LinearFifo<ShmemPacket, PAMI::Counter::BGQ::IndirectL2, 128, Wakeup::BGQ> ShmemFifo;
typedef PAMI::Fifo::LinearFifo<ShmemPacket, PAMI::Counter::BGQ::IndirectL2, 128> LinearL2;
typedef PAMI::Fifo::LinearFifo<ShmemPacket, PAMI::Counter::Indirect<PAMI::Counter::Native>, 128> LinearNative;



int main (int argc, char ** argv)
{
  size_t task = __global.mapping.task();
  size_t size = __global.mapping.size();

  PAMI::Memory::GenMemoryManager mm;
  char shmemfile[PAMI::Memory::MMKEYSIZE];
  snprintf (shmemfile, sizeof(shmemfile) - 1, "/foo");
  mm.init(__global.shared_mm, 8*1024*1024, 1, 1, 0, shmemfile);

  Test<Wrap> wrap;
  wrap.init (&mm, task, size, "wrap");
  //wrap.functional("wrap fifo");

  Test<LinearNative> linear0;
  linear0.init (&mm, task, size, "linear");

  Test<LinearL2> linear1;
  linear1.init (&mm, task, size, "linearl2");

  sleep(1);
  
  if (task == 0)
  {
    fprintf (stdout, "Memory::supports<l1p_flush>()    = %d\n", PAMI::Memory::supports<PAMI::Memory::l1p_flush>());
    fprintf (stdout, "Memory::supports<remote_msync>() = %d\n", PAMI::Memory::supports<PAMI::Memory::remote_msync>());
    fprintf (stdout, "Memory::supports<999>()          = %d\n", PAMI::Memory::supports<999>());
    fprintf (stdout, "\n");
    
    //fprintf (stdout, "bytes linear linearl2 wrap\n");
    fprintf (stdout, "bytes %10s %10s %10s\n", "linear", "linearl2", "wrap");
    //fprintf (stdout, "bytes wrap\n");
  }
  

  size_t sndlen = 0;

  for (; sndlen < ShmemPacket::payload_size; sndlen = sndlen * 3 / 2 + 1)
  {
  
  unsigned long long elapsed0 = linear0.pingpong(sndlen, 100, "linear fifo native atomics");
  unsigned long long elapsed1 = linear1.pingpong(sndlen, 100, "linear fifo L2 atomics");
  unsigned long long elapsed2 = wrap.pingpong(sndlen, 100, "wrap fifo");
  if (task == 0)
  {
    fprintf (stdout, "%5zu %10lld %10lld %10lld\n", sndlen, elapsed0 / 100 / 2, elapsed1 / 100 / 2, elapsed2 / 100 / 2);
    //fprintf (stdout, "%5zu %10lld %10lld\n", sndlen, elapsed1 / 100 / 2, elapsed2 / 100 / 2);
    //fprintf (stdout, "%4zu %4lld\n", sndlen, elapsed2 / 10 / 2);
  }
}
  

  return 0;
}
