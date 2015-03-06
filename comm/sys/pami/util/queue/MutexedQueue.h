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
///  \file util/queue/MutexedQueue.h
///  \brief Mutexed QueueElem and Queue Classes
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
#ifndef __util_queue_MutexedQueue_h__
#define __util_queue_MutexedQueue_h__

#include <stdio.h>

#include "components/memory/MemoryManager.h"

#include "util/common.h"
#include "util/queue/QueueInterface.h"
#include "util/queue/QueueIteratorInterface.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)
#endif

namespace PAMI {

class MutexedQueueElement {
public:
  inline MutexedQueueElement()
  {
    //TRACE_ERR((stderr, "template specialization\n"));
  }

  inline void set(MutexedQueueElement *previous, MutexedQueueElement *next)
  {
    _prev = previous;
    _next = next;
  }

  inline void setPrev(MutexedQueueElement *element)
  {
    _prev = element;
  }

  inline void setNext(MutexedQueueElement *element)
  {
    _next = element;
  }

  inline MutexedQueueElement *prev()
  {
    return _prev;
  }

  inline MutexedQueueElement *next()
  {
    return _next;
  }

protected:

  MutexedQueueElement *_prev;
  MutexedQueueElement *_next;
}; // class MutexedQueueElement

//
// template specialization of queue element
//

template <class T_Queue, class T_Element>
struct MutexedQueueIterator {
  T_Element *curr;
  T_Element *next;
};

template <class T_Mutex>
class MutexedQueue :  public PAMI::Interface::DequeInterface<
        MutexedQueue<T_Mutex>,
        MutexedQueueElement
        >,
      public PAMI::Interface::QueueInfoInterface<
        MutexedQueue<T_Mutex>,
        MutexedQueueElement
        >,
      public PAMI::Interface::QueueIterator<
        MutexedQueue<T_Mutex>, MutexedQueueElement,
        MutexedQueueIterator<MutexedQueue<T_Mutex>, MutexedQueueElement>
        > {
public:
  const static bool removeAll_can_race = false;
  typedef MutexedQueueElement Element;
  typedef MutexedQueueIterator<MutexedQueue<T_Mutex>, MutexedQueueElement> Iterator;

  inline MutexedQueue() :
  PAMI::Interface::DequeInterface<
      MutexedQueue<T_Mutex>,
      MutexedQueueElement
      >(),
  PAMI::Interface::QueueInfoInterface<
      MutexedQueue<T_Mutex>,
      MutexedQueueElement
      >(),
  PAMI::Interface::QueueIterator<
      MutexedQueue<T_Mutex>, MutexedQueueElement,
      MutexedQueueIterator<MutexedQueue<T_Mutex>, MutexedQueueElement>
      >(),
  _mutex(),
  _head(NULL),
  _tail(NULL),
  _size(0)
  {}

  static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
    if (T_Mutex::indirect) {
	return true;//T_Mutex::checkCtorMm(mm);
    } else {
	return true;
    }
  }

  static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
    if (T_Mutex::indirect) {
	return true; //T_Mutex::checkDataMm(mm);
    } else {
	return true; // no mm will be used in init()...
    }
  }

  inline void init(PAMI::Memory::MemoryManager *mm, const char *key)
  {
    if (T_Mutex::indirect) {
    	_mutex.init(mm, key);
    }
  }

  /// \copydoc PAMI::Interface::QueueInterface::enqueue
  inline void enqueue_impl(Element *element)
  {
    TRACE_ERR((stderr, "enqueue(%p)\n", element));
    _mutex.acquire();

    element->set(_tail, NULL);

    if (!_tail) {
      _head = _tail = element;
    } else {
      _tail->setNext(element);
      _tail = element;
    }

    _size++;
    _mutex.release();
  }

  /// \copydoc PAMI::Interface::QueueInterface::dequeue
  inline Element *dequeue_impl()
  {
    _mutex.acquire();
    Element *element = _head;

    if (!element) { _mutex.release(); return NULL; }

    _head = element->next();

    if (_head == NULL) _tail = NULL;
    else _head->setPrev(NULL);

    // Clear the next/prev pointers in the dequeue'd element
    element->setNext(NULL);

    _size--;
    _mutex.release();

    TRACE_ERR((stderr, "dequeue() => %p\n", element));
    return element;
  }

  /// \copydoc PAMI::Interface::QueueInterface::push
  inline void push_impl(Element *element)
  {
    TRACE_ERR((stderr, "push(%p)\n", element));
    _mutex.acquire();

    element->set(NULL, _head);

    if (!_head) {
      _tail = _head = element;
    } else {
      _head->setPrev(element);
      _head = element;
    }

    _size++;
    _mutex.release();
  }

  /// \copydoc PAMI::Interface::QueueInterface::peek
  inline Element *peek_impl()
  {
    return _head;
  }

  /// \copydoc PAMI::Interface::QueueInterface::isEmpty
  inline bool isEmpty_impl()
  {
    return (_head == NULL);
  }

  /// \copydoc PAMI::Interface::QueueInterface::next
  inline Element *next_impl(Element *reference)
  {
    Element *element = reference->next();
    return element;
  }

  /// \copydoc PAMI::Interface::QueueInterface::removeAll
  inline void removeAll_impl(Element *&head, Element *&tail, size_t &size)
  {
    if (_head) {
      _mutex.acquire();
      head = _head;
      tail = _tail;
      size = _size;
      _head = _tail = NULL;
      _size = 0;
      _mutex.release();
    } else {
      head = tail = NULL;
      size = 0;
    }
  }

  /// \copydoc PAMI::Interface::QueueInterface::appendAll
  inline void appendAll_impl(Element *head, Element *tail, size_t size)
  {
    _mutex.acquire();
    if (_tail) {
      _tail->setNext(head);
    } else {
      _head = head;
    }
    _tail = tail;
    _size += size;
    _mutex.release();
  }

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
  /// \copydoc PAMI::Interface::QueueInterface::popTail
  inline Element *popTail_impl()
  {
    _mutex.acquire();
    Element *element = _tail;

    if (!element) { _mutex.release(); return NULL; }

    _tail = element->prev();

    if (_tail == NULL) _head = NULL;
    else _tail->setNext(NULL);

    element->setPrev(NULL);

    _size--;
    _mutex.release();

    return element;
  }

  /// \copydoc PAMI::Interface::QueueInterface::peekTail
  inline Element *peekTail_impl()
  {
    return _tail;
  }
#endif

  /// \copydoc PAMI::Interface::DequeInterface::tail
  inline Element *tail_impl()
  {
    return _tail;
  }

  /// \copydoc PAMI::Interface::DequeInterface::before
  inline Element *before_impl(Element *reference)
  {
    Element *element = reference->prev();
    return element;
  }

  /// \copydoc PAMI::Interface::DequeInterface::insert
  inline void insert_impl(Element *reference,
    Element *element)
  {
    _mutex.acquire();
    Element *rprev = reference->prev();
    element->set(rprev, reference);
    rprev->setNext(element);
    reference->setPrev(element);
    _size++;
    _mutex.release();
  }

  /// \copydoc PAMI::Interface::DequeInterface::append
  inline void append_impl(Element *reference,
    Element *element)
  {
    _mutex.acquire();
    Element *rnext = reference->next();
    element->set(reference, rnext);
    reference->setNext(element);
    rnext->setPrev(element);
    _size++;
    _mutex.release();
  }

  /// \copydoc PAMI::Interface::DequeInterface::remove
  inline void remove_impl(Element *element)
  {
    _mutex.acquire();
    Element *prev = element->prev();
    Element *next = element->next();

    if (prev != NULL) {
      prev->setNext(next);
    } else {
      _head = next;
    }
    if (next != NULL) {
      next->setPrev(prev);
    } else {
      _tail = prev;
    }
    _size--;
    _mutex.release();

    return;
  }

  /// \copydoc PAMI::Interface::QueueInfoInterface::size
  inline size_t size_impl()
  {
    return _size;
  }

  /// \copydoc PAMI::Interface::QueueInfoInterface::dump
  inline void dump_impl(const char *str, int n)
  {
    fprintf(stderr, "%s: size=%zd head=%p tail=%p (locked:%d)\n", str,
                              _size, _head, _tail, _mutex.isLocked());
  }

#ifdef VALIDATE_ON
  /// \copydoc PAMI::Interface::QueueInfoInterface::validate
  inline void validate_impl()
  {
    PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
  }
#endif

// Iterator implementation:
// This all works because there is only one thread removing (the iterator),
// all others only append new work.

  inline void iter_init_impl(Iterator *iter) {
    iter->curr = iter->next = NULL;
  }

  inline bool iter_begin_impl(Iterator *iter) {
    iter->curr = this->peek();
    return false; // did not alter queue
  }

  inline bool iter_check_impl(Iterator *iter) {
    if (iter->curr == NULL) {
      // done with this pass...
      return false;
    }
    iter->next = iter->curr->next();
    return true;
  }

  inline void iter_end_impl(Iterator *iter) {
    iter->curr = iter->next;
  }

  inline Element *iter_current_impl(Iterator *iter) {
    return iter->curr;
  }

  inline pami_result_t iter_remove_impl(Iterator *iter) {
    this->remove(iter->curr);
    return PAMI_SUCCESS;
  }

		inline void iter_dump_impl(const char *str, Iterator *iter) {
			PAMI_abortf("iter_dump not implemented");
		}

protected:

  T_Mutex _mutex;
  Element *_head;
  Element *_tail;
  size_t _size;

}; // class PAMI::MutexedQueue
}; // namespace PAMI

#endif // __util_queue_queue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
