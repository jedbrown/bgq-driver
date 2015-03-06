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
 * \file test/internals/queue/queue.cc
 * \brief Test the queue interface features
 */

#include <stdio.h>
#include <pami.h>

#include "Global.h"
#include "util/queue/Queue.h"
#include "util/queue/MutexedQueue.h"
#ifdef __bgp__
#include "components/atomic/bgp/BgpAtomicCounter.h"
#include "components/atomic/bgp/BgpAtomicMutex.h"
#endif
#include "components/atomic/noop/Noop.h"

#define ELEMENTS 10240

#ifdef __bgp__
typedef PAMI::MutexedQueue<PAMI::Mutex::BGP::Atomic> QueueType;
#else
typedef PAMI::Queue QueueType;
#endif

class TestElement : public QueueType
{
  public:

    TestElement () :
      QueueType (),
      _value (0)
    {};

    void set (size_t value)
    {
      _value = value;
    };

    size_t get ()
    {
      return _value;
    };

  private:

    size_t _value;
};


int main(int argc, char **argv)
{
  QueueType q;
  TestElement element[ELEMENTS];
  TestElement * e = NULL;

  size_t i, tmp;
  for (i=0; i<10; i++)
  {
    element[i].set (10-i);
    tmp = q.size();
    q.push ((QueueType::Element *) &element[i]);
    fprintf (stdout, "Push element (%zu) .. q.size () = %zu -> %zu\n", element[i].get(), tmp, q.size());
  }

  fprintf (stdout, "\n");

  for (i=0; i<10; i++)
  {
    tmp = q.size();
    e = (TestElement *) q.pop ();
    fprintf (stdout, "Pop queue, e->get() = %zu .. q.size () = %zu -> %zu\n", e->get(), tmp, q.size());
  }

  for (i=0; i<ELEMENTS; i++) element[i].set (i);


  PAMI::Queue simpleq;
#ifdef __bgp__
  PAMI::MutexedQueue<PAMI::Mutex::BGP::Atomic> atomicq;
#endif

  unsigned long long t0, t1;

  fprintf (stdout, "\n");
#ifdef __bgp__
  t0 = __global.time.timebase();
  for (i=0; i<ELEMENTS; i++) atomicq.push ((QueueType::Element *) &element[i]);
  for (i=0; i<ELEMENTS; i++) e = (TestElement *) atomicq.pop ();
  t1 = __global.time.timebase();
  fprintf (stdout, "avg. atomic queue push-pop cycles: %lld\n", (t1-t0)/ELEMENTS);
#endif
  t0 = __global.time.timebase();
  for (i=0; i<ELEMENTS; i++) simpleq.push ((PAMI::Queue::Element *) &element[i]);
  for (i=0; i<ELEMENTS; i++) e = (TestElement *) simpleq.pop ();
  t1 = __global.time.timebase();
  fprintf (stdout, "avg. simple queue push-pop cycles: %lld\n", (t1-t0)/ELEMENTS);

  fprintf (stdout, "\n");

  return 0;
};
