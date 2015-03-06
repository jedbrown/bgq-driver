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
///  \file util/queue/MutexedMultiQueue.h
///  \brief QueueElem and Queue Classes for multi-queue objects
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
#ifndef __util_queue_MutexedMultiQueue_h__
#define __util_queue_MutexedMultiQueue_h__

#include <stdio.h>
#include "util/common.h"
#include "util/queue/QueueInterface.h"
#include "util/queue/QueueIteratorInterface.h"
#include "util/queue/Queue.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)
#endif

////////////////////////////////////////////////////////////////////////////////
// The following are similar to Queue but support multi-threaded queues in the
// sense that an object may be queued from multiple places at the same time.
// (the object belongs to multiple linked lists at one time)
//
// It is possible that users of the Queue could be replaced by the multiqueue
// classes below.

namespace PAMI
{

  template <size_t T_NumElems>
  class MutexedMultiQueueElement
  {
    protected:

      typedef struct element
      {
        MutexedMultiQueueElement * prev;
        MutexedMultiQueueElement * next;
      } element_t;

    public:
      inline MutexedMultiQueueElement ()
      {
        unsigned n;

        for (n = 0; n < T_NumElems; n++)
          {
            _elem[n].prev = NULL;
            _elem[n].next = NULL;
          }
      };

      inline ~MutexedMultiQueueElement () {};

      inline MutexedMultiQueueElement * prev (size_t n)
      {
        return _elem[n].prev;
      };

      inline MutexedMultiQueueElement * next (size_t n)
      {
        return _elem[n].next;
      };

      inline void setPrev (MutexedMultiQueueElement * element, size_t n)
      {
        _elem[n].prev = element;
      };

      inline void setNext (MutexedMultiQueueElement * element, size_t n)
      {
        _elem[n].next = element;
      };

      inline void set (MutexedMultiQueueElement * prev, MutexedMultiQueueElement * next, size_t n)
      {
        setPrev (prev, n);
        setNext (next, n);
      };

    protected:
      element_t _elem[T_NumElems];
  };

  template <class T_Queue, class T_Element>
  struct MutexedMultiQueueIterator {
	T_Element *curr;
	T_Element *next;
  };

  template <class T_Mutex, size_t T_NumElems, size_t T_ElemNum>
  class MutexedMultiQueue : public PAMI::Interface::DequeInterface< MutexedMultiQueue<T_Mutex,T_NumElems, T_ElemNum>, MutexedMultiQueueElement<T_NumElems> >,
      public PAMI::Interface::QueueInfoInterface< MutexedMultiQueue<T_Mutex,T_NumElems, T_ElemNum>, MutexedMultiQueueElement<T_NumElems> >,
      public PAMI::Interface::QueueIterator<
		MutexedMultiQueue<T_Mutex,T_NumElems,T_ElemNum>,
		MutexedMultiQueueElement<T_NumElems>,
		MutexedMultiQueueIterator<MutexedMultiQueue<T_Mutex,T_NumElems,T_ElemNum>,
				MutexedMultiQueueElement<T_NumElems>
				>
		>
  {
    public:
      typedef MutexedMultiQueueElement<T_NumElems> Element;
      typedef MutexedMultiQueueIterator<MutexedMultiQueue<T_Mutex,T_NumElems,T_ElemNum>,
				MutexedMultiQueueElement<T_NumElems>
				> Iterator;

      inline MutexedMultiQueue () :
          Interface::DequeInterface< MutexedMultiQueue<T_Mutex,T_NumElems, T_ElemNum>, MutexedMultiQueueElement<T_NumElems> > (),
          PAMI::Interface::QueueInfoInterface< MutexedMultiQueue<T_Mutex,T_NumElems, T_ElemNum>, MutexedMultiQueueElement<T_NumElems> >(),
          PAMI::Interface::QueueIterator<
		MutexedMultiQueue<T_Mutex,T_NumElems,T_ElemNum>,
		MutexedMultiQueueElement<T_NumElems>,
		MutexedMultiQueueIterator<MutexedMultiQueue<T_Mutex,T_NumElems,T_ElemNum>,
				MutexedMultiQueueElement<T_NumElems>
				>
		>(),
          _head (NULL),
          _tail (NULL),
          _size (0)
      {};

      static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
           return T_Mutex::checkCtorMm(mm);
      }

      static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
           return T_Mutex::checkDataMm(mm);
      }

      inline void init(PAMI::Memory::MemoryManager *mm, const char *key)
      {
	_mutex.init(mm, key);
      }

      inline void enqueue_impl (Element * element)
      {
	_mutex.acquire();
        element->set (_tail, NULL, T_ElemNum);

        if (!_tail) _head = _tail = element;
        else
          {
            _tail->setNext (element, T_ElemNum);
            _tail = element;
          }

        _size++;
	_mutex.release();
      };

      inline Element * dequeue_impl ()
      {
	_mutex.acquire();
        Element * element = _head;

        if (!element) return NULL;

        _head = element->next (T_ElemNum);

        if (_head == NULL) _tail = NULL;
        else _head->setPrev (NULL, T_ElemNum);

        // Clear the next/prev pointers in the dequeue'd element
        element->setNext (NULL, T_ElemNum);

        _size--;
	_mutex.release();

        return element;
      };

      inline void push_impl (Element * element)
      {
	_mutex.acquire();
        element->set (NULL, _head, T_ElemNum);

        if (!_head) _tail = _head = element;
        else
          {
            _head->setPrev (element, T_ElemNum);
            _head = element;
          }

        _size++;
	_mutex.release();
      };

      inline Element * peek_impl ()
      {
        return _head;
      };

      inline bool isEmpty_impl ()
      {
        return (_head == NULL);
      };

      inline Element * next_impl (Element * reference)
      {
        return reference->next (T_ElemNum);
      };

      inline Element * tail_impl ()
      {
        return _tail;
      };

      inline Element * before_impl (Element * reference)
      {
        return reference->prev (T_ElemNum);
      };

      inline void insert_impl (Element * reference,
                               Element * element)
      {
	_mutex.acquire();
        Element * rprev = reference->prev (T_ElemNum);
        element->set (rprev, reference, T_ElemNum);
        rprev->setNext (element, T_ElemNum);
        reference->setPrev (element, T_ElemNum);
        _size++;
	_mutex.release();
      };

      inline void append_impl (Element * reference,
                               Element * element)
      {
	_mutex.acquire();
        Element * rnext = reference->next (T_ElemNum);
        element->set (reference, rnext, T_ElemNum);
        reference->setNext (element, T_ElemNum);
        rnext->setPrev (element, T_ElemNum);
        _size++;
	_mutex.release();
      };

      inline void remove_impl (Element * element)
      {
	_mutex.acquire();
        Element * prev = element->prev (T_ElemNum);
        Element * next = element->next (T_ElemNum);

        if (prev != NULL)
          prev->setNext (next, T_ElemNum);
        else
          _head = next;

        if (next != NULL)
          next->setPrev (prev, T_ElemNum);
        else
          _tail = prev;

        _size--;
	_mutex.release();

        return;
      };

      inline size_t size_impl ()
      {
        return _size;
      };

      inline void dump_impl (const char * str, int n)
      {
        PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
      };

#ifdef VALIDATE_ON
      inline void validate_impl ()
      {
        PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
      };
#endif

    // Iterator implementation

    inline void iter_init_impl(Iterator *iter) {
	iter->curr = iter->next = NULL;
    }

    inline bool iter_begin_impl(Iterator *iter) {
	iter->curr = this->head();
	return false;
    }

    inline bool iter_check_impl(Iterator *iter) {
	if (iter->curr == NULL) {
		return false;
	}
	iter->next = nextElem(iter->curr);
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
	iter->curr = NULL;
	return PAMI_SUCCESS;
    }

		inline void iter_dump_impl(const char *str, Iterator *iter) {
			PAMI_abortf("iter_dump not implemented");
		}

    protected:
      Element * _head;
      Element * _tail;
      size_t    _size;
      T_Mutex _mutex;
  };
};

#endif // __util_queue_MutexedMultiQueue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
