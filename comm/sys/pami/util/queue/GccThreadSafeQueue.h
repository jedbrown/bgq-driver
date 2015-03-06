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
///  \file util/queue/GccThreadSafeQueue.h
///  \brief QueueElem and Queue Classes for multi-queue objects
///
#ifndef __util_queue_GccThreadSafeQueue_h__
#define __util_queue_GccThreadSafeQueue_h__

#include <stdio.h>
#include "util/common.h"
#include "Compiler.h"
#include "util/queue/QueueInterface.h"
#include "util/queue/QueueIteratorInterface.h"
#include "components/memory/MemoryManager.h"
#include "util/queue/Queue.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)
#endif

////////////////////////////////////////////////////////////////////////////////
// The following are similar to Queue but support multi-threaded queues in the
// sense that an object may be queued from multiple places at the same time.
//(the object belongs to multiple linked lists at one time)
//
// It is possible that users of the Queue could be replaced by the multiqueue
// classes below.

namespace PAMI {

	template <class T_Element>
	class GccThreadSafePrivQueue :	public PAMI::Interface::QueueInterface<
						GccThreadSafePrivQueue<T_Element>,
						T_Element
						>,
					public PAMI::Interface::QueueInfoInterface<
						GccThreadSafePrivQueue<T_Element>,
						T_Element
						>
	{
	public:
		const static bool removeAll_can_race = false;
		typedef T_Element Element;

		inline GccThreadSafePrivQueue() :
		PAMI::Interface::QueueInterface<
			GccThreadSafePrivQueue<T_Element>,
			T_Element
			>(),
		PAMI::Interface::QueueInfoInterface<
			GccThreadSafePrivQueue<T_Element>,
			T_Element
			>(),
		_head(NULL),
		_tail(NULL),
		_size(0)
		{}

		inline void init() {
		}
		inline void enqueue_impl(Element *e) {
			e->setNext(NULL);
			if (_tail) {
				_tail->setNext(e);
			} else {
				_head = e;
			}
			_tail = e;
			++_size;
		}

		inline Element *dequeue_impl() {
			Element *e = _head;
			if (e) {
				_head = e->next();
				if (_head == NULL) _tail = NULL;
				--_size;
				e->setNext(NULL);
			}
			return e;
		}

		inline void push_impl(Element *e) {
			e->setNext(_head);
			if (_head == NULL) _tail = e;
			_head = e;
			++_size;
		}

		inline Element *peek_impl() {
			return _head;
		}

		inline bool isEmpty_impl() {
			return (_head == NULL);
		}

		inline Element *next_impl(Element *reference) {
			return reference->next();
		}

		inline pami_result_t removeIfSafe(Element *parent, bool canRace) {
			Element *n, *e;

			if (parent) e = parent->next();
			else e = _head;
			n = e->next();
			if (n == NULL) {
				if (canRace && _tail != e) {
					// can't remove, an old appender is still appending...
					return PAMI_EAGAIN;
				}
				_tail = parent;
			}
			if (parent) parent->setNext(n);
			else _head = n;
			e->setNext(NULL);
			--_size;
			return PAMI_SUCCESS;
		}

		inline void removeAll_impl(T_Element *&head, T_Element *&tail, size_t &size) {
			head = _head;
			tail = _tail;
			size = _size;
			if (head) {
				_head = _tail = NULL;
				_size = 0;
			}
		}

		inline void appendAll_impl(T_Element *head, T_Element *tail, size_t size) {
			if (_tail) {
				_tail->setNext(head);
			} else {
				_head = head;
			}
			_tail = tail;
			_size += size;
		}

		inline size_t size_impl() {
			return _size;
		}

#ifdef VALIDATE_ON
		inline void validate_impl() {
			_pub_queue.validate();
		}
#endif

	private:
		T_Element *_head;
		T_Element *_tail;
		size_t _size;
	}; // class GccThreadSafePrivQueue


	/// \brief structure used to iterate over queue
	///
	/// '_queue' is a private queue where all active elements are kept.
	/// 'this' is the queue where new work is appended, and that is moved
	/// over to '_queue' periodically. For SE queues the 'parent' (et al.)
	/// fields help allow removal from the middle. Since there is never
	/// any contention for '_queue', removal is easier.
	///
	/// This is only used by the dequeuer thread, so only applies to the
	/// _priv_queue.
	///
	template <class T_Queue, class T_Element>
	struct GccThreadSafeQueueIterator {
		T_Element *parent;
		T_Element *next_par;
		T_Element *curr;
		T_Element *next;
	};

	template <class T_PubQueue>
	class GccThreadSafeQueue :
		public PAMI::Interface::DequeInterface<
			GccThreadSafeQueue<T_PubQueue>,
			typename T_PubQueue::Element
			>,
		public PAMI::Interface::QueueInfoInterface<
			GccThreadSafeQueue<T_PubQueue>,
			typename T_PubQueue::Element
			>,
		public PAMI::Interface::QueueIterator<
			GccThreadSafeQueue<T_PubQueue>,
			typename T_PubQueue::Element,
			GccThreadSafeQueueIterator<
					GccThreadSafePrivQueue<typename T_PubQueue::Element>,
					typename T_PubQueue::Element
					>
			>
	{
	public:
		typedef typename T_PubQueue::Element Element;
		typedef GccThreadSafeQueueIterator<
			GccThreadSafePrivQueue<typename T_PubQueue::Element>,
			typename T_PubQueue::Element
			> Iterator;

		inline GccThreadSafeQueue() :
		PAMI::Interface::DequeInterface<
			GccThreadSafeQueue<T_PubQueue>,
			typename T_PubQueue::Element
			>(),
		PAMI::Interface::QueueInfoInterface<
			GccThreadSafeQueue<T_PubQueue>,
			typename T_PubQueue::Element
			>(),
		PAMI::Interface::QueueIterator<
			GccThreadSafeQueue<T_PubQueue>,
			typename T_PubQueue::Element,
			GccThreadSafeQueueIterator<
					GccThreadSafePrivQueue<typename T_PubQueue::Element>,
					typename T_PubQueue::Element
					>
			>(),
		_priv_queue(),
		_pub_queue()
		{}

		static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
			return T_PubQueue::checkCtorMm(mm);
		}

		static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
			return T_PubQueue::checkDataMm(mm);
		}

		inline void init(PAMI::Memory::MemoryManager *mm, const char *key) {
			_priv_queue.init();
			_pub_queue.init(mm, key);
		}

		inline void enqueue_impl(Element *e) {
			_pub_queue.enqueue(e);
		}

		inline Element *dequeue_impl() {
			return _pub_queue.dequeue();
		}

		inline void push_impl(Element *element) {
			_pub_queue.push(element);
		}

		inline Element *peek_impl() {
			return _pub_queue.peek();
		}

		inline bool isEmpty_impl() {
			return _pub_queue.isEmpty();
		}

		inline Element *next_impl(Element *reference) {
			return _pub_queue.next(reference);
		}

		inline Element *tail_impl() {
			return _pub_queue.tail();
		}

		inline Element *before_impl(Element *reference) {
			return _pub_queue.before(reference);
		}

		inline void insert_impl(Element *reference, Element *element) {
			_pub_queue.insert(reference, element);
		}

		inline void append_impl(Element *reference, Element *element) {
			_pub_queue.append(reference, element);
		}

		inline void remove_impl(Element *element) {
			_pub_queue.remove(element);
		}

		inline size_t size_impl() {
			return _pub_queue.size();
		}

		inline void removeAll_impl(Element *&head, Element *&tail, size_t &size) {
			_pub_queue.removeAll(head, tail, size);
		}

		inline void dump_impl(const char *str, int n) {
			_pub_queue.dump(str, n);
		}

#ifdef VALIDATE_ON
		inline void validate_impl() {
			_pub_queue.validate();
		}
#endif

		// Iterator implementation

		inline void iter_init_impl(Iterator *iter) {
			iter->parent = iter->next_par = NULL;
			iter->curr = iter->next = NULL;
		}

		inline bool iter_begin_impl(Iterator *iter) {
			// pick up any new work...
			Element *h, *t;
			size_t c;

			_pub_queue.removeAll(h, t, c);
			if (h) {
				_priv_queue.appendAll(h, t, c);
			}
			iter->parent = NULL;
			iter->curr = _priv_queue.peek();
			return (h != NULL);
		}

		inline bool iter_check_impl(Iterator *iter) {
			if (iter->curr == NULL) {
				// done with this pass...
				return false;
			}
			// save next element, so we survive removal.
			iter->next = _priv_queue.nextElem(iter->curr);
			iter->next_par = iter->curr;
			return true;
		}

		inline void iter_end_impl(Iterator *iter) {
			iter->parent = iter->next_par;
			iter->curr = iter->next;
		}

		inline Element *iter_current_impl(Iterator *iter) {
			return iter->curr;
		}

		inline pami_result_t iter_remove_impl(Iterator *iter) {

			pami_result_t rc = _priv_queue.removeIfSafe(iter->parent,
							_pub_queue.removeAll_can_race);
			if (rc == PAMI_SUCCESS) {
				iter->next_par = iter->parent; // keep same parent for next iter
			}
			return rc;
		}

		inline void iter_dump_impl(const char *str, Iterator *iter) {
			PAMI_abortf("iter_dump not implemented");
		}

	protected:
		GccThreadSafePrivQueue<typename T_PubQueue::Element> _priv_queue;
		T_PubQueue _pub_queue;
	}; // class GccThreadSafeQueue
}; // namespace PAMI

#endif // __util_queue_GccThreadSafeQueue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
