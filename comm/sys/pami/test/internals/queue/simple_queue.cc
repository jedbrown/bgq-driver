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
 * \file test/internals/queue/simple_queue.cc
 * \brief Test the queue interface features
 */

#include <stdio.h>
#include <pami.h>

#include "Global.h"
#include "util/queue/Queue.h"
#include "util/queue/CircularQueue.h"

#define ELEMENTS 10240
#define QUEUE_SIZE 4

template <class T_Queue>
class TestElement : public T_Queue::Element
{
  public:

    TestElement () :
      T_Queue::Element (),
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

template <class T_Queue>
void dump_queue (T_Queue & queue, const char * type, const char * name = "Dump")
{
  char pad[10240];
  size_t i = 0;
  while (name[i] != 0)
  {
    pad[i++] = ' ';
  }
  pad[i] = 0;

  TestElement<T_Queue> * e = (TestElement<T_Queue> *) queue.peek();

  if (e == NULL)
    fprintf (stdout, "[%s] %s: head -> %p\n", type, name, e);
  else
  {
    fprintf (stdout, "[%s] %s: head -> %p(%zu)\n", type, name, e, e->get());
    while ((e = (TestElement<T_Queue> *) queue.next(e)) != NULL)
    {
      fprintf (stdout, "[%s] %s       -> %p(%zu)\n", type, pad, e, e->get());
    }
  }
}


template <class T_Queue>
bool test_stack_operations (size_t qsize, size_t esize, const char * name = "")
{
  bool failed = false;

  T_Queue q;
  TestElement<T_Queue> element[2][ELEMENTS];
  TestElement<T_Queue> * e = NULL;

  size_t i;
  for (i=0; i<ELEMENTS; i++)
  {
    element[0][i].set (i);
    element[1][i].set (i);
  }

  fprintf (stdout, "[%s]\n", name);
  fprintf (stdout, "[%s] ---------------------------------------------------------------\n", name);
  fprintf (stdout, "[%s]   Test push-pop %zu element(s) to existing queue of size %zu\n", name, esize, qsize);
  fprintf (stdout, "[%s] ---------------------------------------------------------------\n", name);

  for (i=0; i<qsize; i++)
  {
    q.push ((typename T_Queue::Element *) &element[0][i]);
  }

  fprintf (stdout, "[%s] Initial queue state\n", name);

  // dump the state of the queue
  dump_queue(q, name, "       q");
  fprintf (stdout, "[%s]\n", name);

  // Test push operation
  fprintf (stdout, "[%s] Test push operation\n", name);
  fprintf (stdout, "[%s]\n", name);
  for (i=0; i<esize; i++)
  {
    q.push ((typename T_Queue::Element *) &element[1][i]);

    // check q for correctness ..
    if ((e = (TestElement<T_Queue> *) q.peek()) == NULL)
    {
      fprintf (stdout, "[%s] ... ERROR. Expected non-empty queue, but q.peek() == %p\n", name, e);
      failed = true;
    }
    else
    {
      size_t ecount = 0;
      while (e != NULL && (ecount <= i))
      {
        size_t expected = i-ecount;
        if (e->get() != expected)
        {
          fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, expected, e, e->get());
          failed = true;
        }
        ecount++;
        e = (TestElement<T_Queue> *) q.next(e);
      }
      size_t qcount = qsize;
      while (e != NULL && qcount > 0)
      {
        size_t expected = qcount-1;
        if (e->get() != expected)
        {
          fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, expected, e, e->get());
          failed = true;
        }
        qcount--;
        e = (TestElement<T_Queue> *) q.next(e);
      }
    }

    fprintf (stdout, "[%s] After push, (%p)->get() = %zu\n", name, &element[1][i], element[1][i].get());

    // dump the state of the queue
    dump_queue(q, name, "       q");
  }

  // Test pop operation
  fprintf (stdout, "[%s]\n", name);
  fprintf (stdout, "[%s] Test pop operation\n", name);
  fprintf (stdout, "[%s]\n", name);
  for (i=esize; i>0; i--)
  {
    e = (TestElement<T_Queue> *) q.pop();

    // check q for correctness ..
    if (e == NULL)
    {
      fprintf (stdout, "[%s] ... ERROR. Expected non-empty queue, but q.peek() == %p\n", name, e);
      failed = true;
    }
    else if (e->get() != (i-1))
    {
      fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, (i-1), e, e->get());
      failed = true;
    }

    fprintf (stdout, "[%s] After pop, (%p)->get() = %zu\n", name, e, e->get());

    // dump the state of the queue
    dump_queue(q, name, "       q");
  }
  for (i=qsize; i>0; i--)
  {
    e = (TestElement<T_Queue> *) q.pop();

    // check q for correctness ..
    if (e == NULL)
    {
      fprintf (stdout, "[%s] ... ERROR. Expected non-empty queue, but q.peek() == %p\n", name, e);
      failed = true;
    }
    else if (e->get() != (i-1))
    {
      fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, (i-1), e, e->get());
      failed = true;
    }

    fprintf (stdout, "[%s] After pop, (%p)->get() = %zu\n", name, e, e->get());

    // dump the state of the queue
    dump_queue(q, name, "       q");
  }

  return failed;
}

template <class T_Queue>
bool test_stack_operations (const char * name = "")
{
  fprintf (stdout, "\n");
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s]   Test push-pop operations\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  bool failed = false;

  size_t i, j;
  for (i=0; i<4; i++)
  {
    for (j=1; j<4; j++)
    {
      failed |= test_stack_operations<T_Queue> (i, j, name);
    }
  }
  return failed;
}

template <class T_Queue>
bool test_queue_operations (size_t qsize, size_t esize, const char * name = "")
{
  bool failed = false;

  T_Queue q;
  TestElement<T_Queue> element[2][ELEMENTS];
  TestElement<T_Queue> * e = NULL;

  size_t qcount, ecount, i;
  for (i=0; i<ELEMENTS; i++)
  {
    element[0][i].set (i);
    element[1][i].set (i);
  }

  fprintf (stdout, "[%s]\n", name);
  fprintf (stdout, "[%s] ---------------------------------------------------------------\n", name);
  fprintf (stdout, "[%s]   Test enqueue-dequeue %zu element(s) to existing queue of size %zu\n", name, esize, qsize);
  fprintf (stdout, "[%s] ---------------------------------------------------------------\n", name);

  for (i=0; i<qsize; i++)
  {
    q.enqueue ((typename T_Queue::Element *) &element[0][i]);
  }

  fprintf (stdout, "[%s] Initial queue state\n", name);

  // dump the state of the queue
  dump_queue(q, name, "       q");
  fprintf (stdout, "[%s]\n", name);

  // Test enqueue operation
  fprintf (stdout, "[%s] Test enqueue operation\n", name);
  fprintf (stdout, "[%s]\n", name);
  for (i=0; i<esize; i++)
  {
    q.enqueue ((typename T_Queue::Element *) &element[1][i]);

    // check q for correctness ..
    if ((e = (TestElement<T_Queue> *) q.peek()) == NULL)
    {
      fprintf (stdout, "[%s] ... ERROR. Expected non-empty queue, but q.peek() == %p\n", name, e);
      failed = true;
    }
    else
    {
      size_t qcount = 0;
      while (e != NULL && qcount < qsize)
      {
        if (e->get() != qcount)
        {
          fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, qcount, e, e->get());
          failed = true;
        }
        qcount++;
        e = (TestElement<T_Queue> *) q.next(e);
      }
      if (qsize > 0 && e == NULL)
      {
        fprintf (stdout, "[%s] ... ERROR. Expected an original element from q\n", name);
        failed = true;
      }
      if (qcount != qsize)
      {
        fprintf (stdout, "[%s] ... ERROR. Lost elements originally in q, size was %zu, but is now %zu\n", name, qsize, qcount);
        failed = true;
      }

      size_t ecount = 0;
      while (e != NULL && ecount < i)
      {
        if (e->get() != ecount)
        {
          fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, ecount, e, e->get());
          failed = true;
        }
        ecount++;
        e = (TestElement<T_Queue> *) q.next(e);
      }
        if (ecount != i)
      {
        fprintf (stdout, "[%s] ... ERROR. Lost elements previously enqueued\n", name);
        failed = true;
      }
    }

    fprintf (stdout, "[%s] After enqueue, (%p)->get() = %zu\n", name, &element[1][i], element[1][i].get());

    // dump the state of the queue
    dump_queue(q, name, "       q");
    fprintf (stdout, "[%s]\n", name);
  }

  // Test dequeue operation
  fprintf (stdout, "[%s] Test dequeue operation\n", name);
  fprintf (stdout, "[%s]\n", name);
  qcount = 0;
  if (qsize > 0)
  {
    while ((qcount<qsize) && (e = (TestElement<T_Queue> *) q.dequeue()) != NULL)
    {
      if (e->get() != qcount)
      {
        fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, qcount, e, e->get());
        failed = true;
      }
      qcount++;

      // dump the state of the queue
      fprintf (stdout, "[%s] After dequeue, (%p)->get() = %zu\n", name, e, e->get());
      dump_queue(q, name, "       q");
      fprintf (stdout, "[%s]\n", name);
    }
  }
  ecount = 0;
  while ((e = (TestElement<T_Queue> *) q.dequeue()) != NULL)// && ecount<esize)
  {
    if (e->get() != ecount)
    {
      fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, ecount, e, e->get());
      failed = true;
    }
    ecount++;

    // dump the state of the queue
    fprintf (stdout, "[%s] After dequeue, (%p)->get() = %zu\n", name, e, e->get());
    dump_queue(q, name, "       q");
    fprintf (stdout, "[%s]\n", name);
  }
  if (e != NULL)
  {
    fprintf (stdout, "[%s] ... ERROR. Extraneous element(s) in queue\n", name);
    failed = true;
  }
  if (e == NULL && ((qcount != qsize) || (ecount != esize)))
  {
    fprintf (stdout, "[%s] ... ERROR. Lost elements previously enqueued\n", name);
    failed = true;
  }

  fprintf (stdout, "[%s]   Test %s\n", name, failed==true?"FAILED":"PASSED");

  return failed;
}

template <class T_Queue>
bool test_queue_operations (const char * name = "")
{
  fprintf (stdout, "\n");
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s]   Test enqueue-dequeue operations\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  bool failed = false;

  size_t i, j;
  for (i=0; i<4; i++)
  {
    for (j=1; j<4; j++)
    {
      failed |= test_queue_operations<T_Queue> (i, j, name);
    }
  }
  return failed;
}



template <class T_Queue>
bool test_q2q_operations (size_t q0size, size_t q1size, const char * name = "")
{
  bool failed = false;

  T_Queue q[2];
  TestElement<T_Queue> element[2][ELEMENTS];
  TestElement<T_Queue> * e = NULL;

  size_t i;
  for (i=0; i<ELEMENTS; i++)
  {
    element[0][i].set (i);
    element[1][i].set (i);
  }

  fprintf (stdout, "[%s]\n", name);
  fprintf (stdout, "[%s] ----------------------------------------------\n", name);
  fprintf (stdout, "[%s]   Test adding queue (size %zu) to queue (size %zu)\n", name, q1size, q0size);
  fprintf (stdout, "[%s] ----------------------------------------------\n", name);

  for (i=0; i<q0size; i++)
    q[0].enqueue ((typename T_Queue::Element *) &element[0][i]);

  for (i=0; i<q1size; i++)
    q[1].enqueue ((typename T_Queue::Element *) &element[1][i]);

  fprintf (stdout, "[%s] Before q[0].enqueue (&q[1]) ...\n", name);

  // dump the state of the queues
  dump_queue(q[0], name, "q[0]");
  dump_queue(q[1], name, "q[1]");
  fprintf (stdout, "[%s]\n", name);

  q[0].enqueue (&q[1]);

  fprintf (stdout, "[%s] After q[0].enqueue (&q[1])\n", name);

  // check q[1] for correctness .. should be empty
  if ((e = (TestElement<T_Queue> *) q[1].peek()) != NULL)
  {
    fprintf (stdout, "[%s] ... ERROR. Expected empty queue, q[1].peek() == %p\n", name, e);
    failed = true;
  }

  // check q[0] for correctness .. should contain the original q[0] elements
  // in order followed by all of the q[1] elements in order.
  if ((q0size+q1size) == 0)
  {
    if ((e = (TestElement<T_Queue> *) q[0].peek()) != NULL)
    {
    fprintf (stdout, "[%s] ... ERROR. Expected empty queue, q[0].peek() == %p\n", name, e);
    failed = true;
    }
  }
  else if ((e = (TestElement<T_Queue> *) q[0].peek()) == NULL)
  {
    fprintf (stdout, "[%s] ... ERROR. Expected non-empty queue, q[0].peek() == %p\n", name, e);
    failed = true;
  }
  else
  {
    size_t q0count = 0;
    while (e != NULL && q0count < q0size)
    {
      if (e->get() != q0count)
      {
        fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, q0count, e, e->get());
        failed = true;
      }
      q0count++;
      e = (TestElement<T_Queue> *) q[0].next(e);
    }
    if (q1size > 0 && e == NULL)
    {
      fprintf (stdout, "[%s] ... ERROR. Expected an element originally from q[1]\n", name);
      failed = true;
    }
    if (q0count != q0size)
    {
      fprintf (stdout, "[%s] ... ERROR. Lost elements originally in q[0], size was %zu, but is now %zu\n", name, q0size, q0count);
      failed = true;
    }
    size_t q1count = 0;
    while (e != NULL && q1count < q1size)
    {
      if (e->get() != q1count)
      {
        fprintf (stdout, "[%s] ... ERROR. Expected element id %zu, but (%p)->get() == %zu\n", name, q1count, e, e->get());
        failed = true;
      }
      q1count++;
      e = (TestElement<T_Queue> *) q[0].next(e);
    }
    if (q1count != q1size)
    {
      fprintf (stdout, "[%s] ... ERROR. Lost elements originally in q[1], size was %zu, but is now %zu\n", name, q1size, q1count);
      failed = true;
    }
  }

  // dump the state of the queues
  dump_queue(q[0], name, "q[0]");
  dump_queue(q[1], name, "q[1]");
  fprintf (stdout, "[%s]\n", name);

  fprintf (stdout, "[%s]   Test %s\n", name, failed==true?"FAILED":"PASSED");

  return failed;
}


template <class T_Queue>
bool test_q2q_operations (const char * name = "")
{
  fprintf (stdout, "\n");
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s]   Test queue-to-queue operations\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  fprintf (stdout, "[%s] ==============================================\n", name);
  bool failed = false;

  size_t i, j;
  for (i=0; i<4; i++)
  {
    for (j=0; j<4; j++)
    {
      failed |= test_q2q_operations<T_Queue> (i, j, name);
    }
  }
  return failed;
}



int main(int argc, char **argv)
{
  bool failed[2];

  failed[0]  = false;
  failed[0] |= test_stack_operations<PAMI::Queue>("PAMI::Queue");
  failed[0] |= test_queue_operations<PAMI::Queue>("PAMI::Queue");
  //failed[0] |= test_q2q_operations<PAMI::Queue>("PAMI::Queue");

  failed[1]  = false;
  failed[1] |= test_stack_operations<PAMI::CircularQueue>("PAMI::CircularQueue");
  failed[1] |= test_queue_operations<PAMI::CircularQueue>("PAMI::CircularQueue");
  failed[1] |= test_q2q_operations<PAMI::CircularQueue>("PAMI::CircularQueue");

  fprintf (stdout, "\n");
  fprintf (stdout, "==========================================\n");
  fprintf (stdout, "  Test results\n");
  fprintf (stdout, "    [PAMI::Queue]:          %s\n", failed[0]==true?"FAILED":"PASSED");
  fprintf (stdout, "    [PAMI::CircularQueue]:  %s\n", failed[1]==true?"FAILED":"PASSED");
  fprintf (stdout, "==========================================\n");
  fprintf (stdout, "\n");

  return failed[0] | failed[1];
};
