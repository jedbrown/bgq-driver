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
///
///  \file util/queue/GccCmpSwapQueue.h
///  \brief GCC compare_and_swap QueueElem and Queue Classes
///
///  These classes implement a base queue element and queues constructed
///  from the queue element.  This can be used to implement
///  - Message Queues
///  - Shared Memory Queues
///  - Circular or linear Queues
///
///  Definitions:
///  - QueueElement:  An item to be inserted into a queue
///  - Queue:         A queue of elements
///
///
#ifndef __util_queue_GccCmpSwapQueue_h__
#define __util_queue_GccCmpSwapQueue_h__

#include <stdio.h>

#include "components/memory/MemoryManager.h"

#include "util/common.h"
#include "util/queue/QueueInterface.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)
#endif

namespace PAMI {

class GccCmpSwapQueueElement {
public:
  inline GccCmpSwapQueueElement()
  {
    //TRACE_ERR((stderr, "template specialization\n"));
  }

  inline void setNext(GccCmpSwapQueueElement *element)
  {
    _next = element;
  }

  inline GccCmpSwapQueueElement *next()
  {
    return _next;
  }

protected:
  GccCmpSwapQueueElement *_next;
}; // class GccCmpSwapQueueElement

//
// template specialization of queue element
//

class GccCmpSwapQueue : public PAMI::Interface::DequeInterface<
        GccCmpSwapQueue,
        GccCmpSwapQueueElement
        >,
      public PAMI::Interface::QueueInfoInterface<
        GccCmpSwapQueue,
        GccCmpSwapQueueElement
        > {
public:
  const static bool removeAll_can_race = true;
  typedef GccCmpSwapQueueElement Element;

  inline GccCmpSwapQueue() :
  PAMI::Interface::DequeInterface<
      GccCmpSwapQueue,
      GccCmpSwapQueueElement
      >(),
  PAMI::Interface::QueueInfoInterface<
      GccCmpSwapQueue,
      GccCmpSwapQueueElement
      >(),
  _head(NULL),
  _tail(NULL),
  _size(0)
  {}

	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
		return true;
	}

	static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
		return true;
	}

  inline void init(PAMI::Memory::MemoryManager *mm, const char *key) {
  }

  /// \copydoc PAMI::Interface::QueueInterface::enqueue
  inline void enqueue_impl(Element *e) {
    Element *t, *u;

    e->setNext(NULL);
    u = _tail;
    do {
      t = u;
    } while ((u = __sync_val_compare_and_swap(&_tail, t, e)) != t);
    if (t) {
      // PAMI_assert(t->next == NULL);
      t->setNext(e); // t->next was NULL...
    } else {
      // q->tail already set to 'e'...
      _head = e; // q->head was NULL...
    }
    //__sync_fetch_and_add(&_size, 1);
  }

  /// \copydoc PAMI::Interface::QueueInterface::dequeue
  inline Element *dequeue_impl() {
    PAMI_abortf("DequeInterface::dequeue not implemented");
    return NULL;
  }

  /// \copydoc PAMI::Interface::QueueInterface::push
  inline void push_impl(Element *element) {
    PAMI_abortf("DequeInterface::push not implemented");
  }

  /// \copydoc PAMI::Interface::QueueInterface::peek
  inline Element *peek_impl() {
    return _head;
  }

  /// \copydoc PAMI::Interface::QueueInterface::isEmpty
  inline bool isEmpty_impl() {
    return (_head == NULL);
  }

  /// \copydoc PAMI::Interface::QueueInterface::next
  inline Element *next_impl(Element *reference) {
    return reference->next();
  }

  /// \copydoc PAMI::Interface::QueueInterface::removeAll
  inline void removeAll_impl(Element *&head, Element *&tail, size_t &size) {
    Element *t;
    //size_t c;

    head = _head;
    if (!head) return;

    _head = NULL;
    do {
      t = _tail;
      //c = _size;
    } while (!__sync_bool_compare_and_swap(&_tail, t, NULL));
    //__sync_fetch_and_add(&new_work->_size, -c);
    tail = t;
    size = 0; // = c;
  }

  /// \copydoc PAMI::Interface::QueueInterface::appendAll
  inline void appendAll_impl(Element *head, Element *tail, size_t size) {
    Element *t, *u;

    tail->setNext(NULL);
    u = _tail;
    do {
      t = u;
    } while ((u = __sync_val_compare_and_swap(&_tail, t, tail)) != t);
    if (t) {
      // PAMI_assert(t->next == NULL);
      t->setNext(head); // t->next was NULL...
    } else {
      // q->tail already set
      _head = head; // q->head was NULL...
    }
    //__sync_fetch_and_add(&_size, size);
  }

  /// \copydoc PAMI::Interface::DequeInterface::tail
  inline Element *tail_impl() {
    return _tail;
  }

  /// \copydoc PAMI::Interface::DequeInterface::before
  inline Element *before_impl(Element *reference) {
    PAMI_abortf("DequeInterface::before not implemented");
    return NULL;
  }

  /// \copydoc PAMI::Interface::DequeInterface::insert
  inline void insert_impl(Element *reference, Element *element) {
    PAMI_abortf("DequeInterface::insert not implemented");
  }

  /// \copydoc PAMI::Interface::DequeInterface::append
  inline void append_impl(Element *reference, Element *element) {
    PAMI_abortf("DequeInterface::append not implemented");
  }

  /// \copydoc PAMI::Interface::DequeInterface::remove
  inline void remove_impl(Element *element) {
    PAMI_abortf("DequeInterface::remove not implemented");
  }

  /// \copydoc PAMI::Interface::QueueInfoInterface::size
  inline size_t size_impl()
  {
    return _size;
  }

  /// \copydoc PAMI::Interface::QueueInfoInterface::dump
  inline void dump_impl(const char *str, int n) {
    PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
  }

#ifdef VALIDATE_ON
  /// \copydoc PAMI::Interface::QueueInfoInterface::validate
  inline void validate_impl() {
    PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
  }
#endif

protected:

  Element *_head;
  Element *_tail;
  size_t _size;

}; // class PAMI::GccCmpSwapQueue
}; // namespace PAMI

#endif // __util_queue_GccCmpSwapQueue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
