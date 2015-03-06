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
///  \file util/queue/Queue.h
///  \brief QueueElem and Queue Classes
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
#ifndef __util_queue_Queue_h__
#define __util_queue_Queue_h__

#include <stdio.h>

#include "components/memory/MemoryManager.h"
#include "util/common.h"
#include "util/queue/QueueInterface.h"
#include "util/queue/QueueIteratorInterface.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)
#endif

namespace PAMI {

//
// template specialization of queue element
//

class Queue;
namespace Interface {

template <>
class QueueElement<Queue> {
public:
	inline QueueElement() {
		//TRACE_ERR((stderr, "template specialization\n"));
	}

	inline void set(QueueElement *previous, QueueElement *next) {
		_prev = previous;
		_next = next;
	}

	inline void setPrev(QueueElement *element) {
		_prev = element;
	}

	inline void setNext(QueueElement *element) {
		_next = element;
	}

	inline QueueElement *prev() {
		return _prev;
	}

	inline QueueElement *next() {
		return _next;
	}

protected:
	QueueElement *_prev;
	QueueElement *_next;
}; // class QueueElement<Queue>

}; // namespace Interface

template <class T_Queue, class T_Element>
struct BasicQueueIterator {
	T_Element *parent;
	T_Element *curr;
	T_Element *next;
};

class Queue : public PAMI::Interface::DequeInterface<Queue>,
	public PAMI::Interface::QueueInfoInterface<Queue>,
	public PAMI::Interface::QueueIterator <
	Queue, Interface::QueueElement<Queue> ,
	BasicQueueIterator<Queue, Interface::QueueElement<Queue> >
		> {
public:
	typedef Interface::QueueElement<Queue>  Element;
	typedef BasicQueueIterator<Queue, Interface::QueueElement<Queue> > Iterator;

	inline Queue() :
		PAMI::Interface::DequeInterface<Queue> (),
		PAMI::Interface::QueueInfoInterface<Queue> (),
		PAMI::Interface::QueueIterator <
		Queue, Interface::QueueElement<Queue> ,
		BasicQueueIterator<Queue, Interface::QueueElement<Queue> >
		> (),
		_head(NULL),
		_tail(NULL),
		_size(0) {
	}

	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
                (void)mm;
		return true;
	}

	static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
                (void)mm;
		return true;
	}

	inline void init(PAMI::Memory::MemoryManager *mm, const char *key) {
                (void)mm;(void)key;
		_head = NULL;
		_tail = NULL;
		_size = 0;
	}

	/// \copydoc PAMI::Interface::QueueInterface::enqueue
	inline void enqueue_impl(Queue::Element *element) {
		TRACE_ERR((stderr, "enqueue(%p)\n", element));

		element->set(_tail, NULL);

		if (!_tail) { _head = _tail = element; }
		else {
			_tail->setNext(element);
			_tail = element;
		}

		_size++;
	}

	/// \copydoc PAMI::Interface::QueueInterface::dequeue
	inline Queue::Element *dequeue_impl() {
		Queue::Element *element = _head;

		if (!element) { return NULL; }

		_head = element->next();

		if (_head == NULL) { _tail = NULL; }
		else { _head->setPrev(NULL); }

		// Clear the next/prev pointers in the dequeue'd element
		element->setNext(NULL);

		_size--;

		TRACE_ERR((stderr, "dequeue() => %p\n", element));
		return element;
	}

	/// \copydoc PAMI::Interface::QueueInterface::push
	inline void push_impl(Element *element) {
		TRACE_ERR((stderr, "push(%p)\n", element));

		element->set(NULL, _head);

		if (!_head) { _tail = _head = element; }
		else {
			_head->setPrev(element);
			_head = element;
		}

		_size++;
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
		head = _head;
		tail = _tail;
		size = _size;
		if (head) {
			_head = _tail = NULL;
			_size = 0;
		}
	}

	/// \copydoc PAMI::Interface::QueueInterface::appendAll
	inline void appendAll_impl(Element *head, Element *tail, size_t size) {
		if (_tail) {
			_tail->setNext(head);
		} else {
			_head = head;
		}
		_tail = tail;
		_size += size;
	}

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
	/// \copydoc PAMI::Interface::QueueInterface::popTail
	inline Element *popTail_impl() {
		Element *element = _tail;

		if (!element) { return NULL; }

		_tail = element->prev();

		if (_tail == NULL) { _head = NULL; }
		else { _tail->setNext(NULL); }

		element->setPrev(NULL);

		_size--;

		return element;
	}

	/// \copydoc PAMI::Interface::QueueInterface::peekTail
	inline Element *peekTail_impl() {
		return _tail;
	}
#endif

	/// \copydoc PAMI::Interface::DequeInterface::tail
	inline Queue::Element *tail_impl() {
		return _tail;
	}

	/// \copydoc PAMI::Interface::DequeInterface::before
	inline Queue::Element *before_impl(Queue::Element *reference) {
		return reference->prev();
	}

	/// \copydoc PAMI::Interface::DequeInterface::insert
	inline void insert_impl(Queue::Element *reference,
				Queue::Element *element) {
		Queue::Element *rprev = reference->prev();
		element->set(rprev, reference);
		rprev->setNext(element);
		reference->setPrev(element);
		_size++;
	}

	/// \copydoc PAMI::Interface::DequeInterface::append
	inline void append_impl(Queue::Element *reference,
				Queue::Element *element) {
		Queue::Element *rnext = reference->next();
		element->set(reference, rnext);
		reference->setNext(element);
		rnext->setPrev(element);
		_size++;
	}

	/// \copydoc PAMI::Interface::DequeInterface::remove
	inline void remove_impl(Queue::Element *element) {
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
	}

	inline void safe_remove(Queue::Element *parent, Queue::Element *next) {
		if (!parent) {
			_head = next;
		} else {
			parent->setNext(next);
		}
		if (!next) {
			_tail = parent;
		} else {
			next->setPrev(parent);
		}
		_size--;
	}

	/// \copydoc PAMI::Interface::QueueInfoInterface::size
	inline size_t size_impl() {
		return _size;
	}

	/// \copydoc PAMI::Interface::QueueInfoInterface::dump
	inline void dump_impl(const char *str, int n) {
                (void)n;
		fprintf(stderr, "%s: PAMI::Queue %p %p %zd\n", str, _head, _tail, _size);
	}

#ifdef VALIDATE_ON
	/// \copydoc PAMI::Interface::QueueInfoInterface::validate
	inline void validate_impl() {
		PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
	}
#endif

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
	/// \copydoc PAMI::Interface::QueueInfoInterface::insertElem
	inline void insertElem_impl(Queue::Element *element, size_t position) {
		if (position == 0) {
			push(element);
			_size++;
			return;
		}

		size_t i;
		Queue::Element *insert = _head;

		for (i = 1; i < position; i++) {
			insert = insert->next();
		}

		element->set(insert, insert->next());
		insert->setNext(element);
		_size++;

		return;
	}
#endif

	// Iterator implementation:
	// This all works because there is only one thread removing (the iterator),
	// all others only append new work.

	inline void iter_init_impl(Iterator *iter) {
		iter->parent = iter->curr = iter->next = NULL;
	}

	inline bool iter_begin_impl(Iterator *iter) {
		iter->parent = NULL;
		iter->curr = peek();
		return false; // did not alter queue
	}

	inline bool iter_check_impl(Iterator *iter) {
		if (iter->curr == NULL) {
			// done with this pass...
			return false;
		}
		iter->next = nextElem(iter->curr);
		return true;
	}

	inline void iter_end_impl(Iterator *iter) {
		iter->parent = iter->curr;
		iter->curr = iter->next;
	}

	inline Element *iter_current_impl(Iterator *iter) {
		return iter->curr;
	}

	inline pami_result_t iter_remove_impl(Iterator *iter) {
		iter->curr = iter->parent; // back-step
		safe_remove(iter->parent, iter->next);
		return PAMI_SUCCESS;
	}

	inline void iter_dump_impl(const char *str, Iterator *iter) {
		fprintf(stderr, "%s: PAMI::Queue::Iterator %p %p [%p %p %zd]\n", str,
				iter->curr, iter->next, _head, _tail, _size);
	}

private:

	Queue::Element *_head;
	Queue::Element *_tail;
	size_t _size;

}; // class PAMI::Queue
}; // namespace PAMI

#endif // __util_queue_queue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments --max-instatement-indent=79
// astyle options --min-conditional-indent=2 --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements --align-pointer=name --lineend=linux
//
