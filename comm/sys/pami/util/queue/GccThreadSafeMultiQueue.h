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
///  \file util/queue/GccThreadSafeMultiQueue.h
///  \brief QueueElem and Queue Classes for multi-queue objects
///
#ifndef __util_queue_GccThreadSafeMultiQueue_h__
#define __util_queue_GccThreadSafeMultiQueue_h__

#include <stdio.h>
#include "util/common.h"
#include "Compiler.h"
#include "util/queue/QueueInterface.h"
#include "util/queue/QueueIteratorInterface.h"
#include "components/memory/MemoryManager.h"

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

	template <size_t T_NumElems>
	class GccThreadSafeMultiQueueElement {
	protected:

		typedef struct element {
			GccThreadSafeMultiQueueElement *_next;
		} element_t;

	public:
		inline GccThreadSafeMultiQueueElement() {
			unsigned n;

			for (n = 0; n < T_NumElems; n++) {
				_elem[n]._next = NULL;
			}
		}

		inline ~GccThreadSafeMultiQueueElement() {}

		inline GccThreadSafeMultiQueueElement *next(size_t n) {
			return _elem[n]._next;
		}

		inline GccThreadSafeMultiQueueElement **nextPtr(size_t n) {
			return &_elem[n]._next;
		}

		inline void setNext(GccThreadSafeMultiQueueElement *element, size_t n) {
			_elem[n]._next = element;
		}

	protected:
		element_t _elem[T_NumElems];
	}; // class GccThreadSafeMultiQueueElement

	/// \brief structure used to iterate over queue
	///
	/// '_queue' is a private queue where all active elements are kept.
	/// 'this' is the queue where new work is appended, and that is moved
	/// over to '_queue' periodically. For SE queues the 'parent' (et al.)
	/// fields help allow removal from the middle. Since there is never
	/// any contention for '_queue', removal is easier.
	///
	template <class T_Queue, class T_Element>
	struct GccThreadSafeMultiQueueIterator {
		T_Queue _queue;
		T_Element *parent;
		T_Element *next_par;
		T_Element *curr;
		T_Element *next;
	};

	template <size_t T_NumElems, size_t T_ElemNum>
	class GccThreadSafeMultiQueue :
		public PAMI::Interface::DequeInterface<
			GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
			GccThreadSafeMultiQueueElement<T_NumElems>
			>,
		public PAMI::Interface::QueueInfoInterface<
			GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
			GccThreadSafeMultiQueueElement<T_NumElems>
			>,
		public PAMI::Interface::QueueIterator<
			GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
			GccThreadSafeMultiQueueElement<T_NumElems>,
			GccThreadSafeMultiQueueIterator<
					GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
					GccThreadSafeMultiQueueElement<T_NumElems>
					>
			>
	{
	public:
		typedef GccThreadSafeMultiQueueElement<T_NumElems> Element;
		typedef GccThreadSafeMultiQueueIterator<
			GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
			GccThreadSafeMultiQueueElement<T_NumElems>
			> Iterator;

		inline GccThreadSafeMultiQueue() :
		PAMI::Interface::DequeInterface<
			GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
			GccThreadSafeMultiQueueElement<T_NumElems>
			>(),
		PAMI::Interface::QueueInfoInterface<
			GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
			GccThreadSafeMultiQueueElement<T_NumElems>
			>(),
		PAMI::Interface::QueueIterator<
			GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
			GccThreadSafeMultiQueueElement<T_NumElems>,
			GccThreadSafeMultiQueueIterator<
					GccThreadSafeMultiQueue<T_NumElems,T_ElemNum>,
					GccThreadSafeMultiQueueElement<T_NumElems>
					>
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

		inline void enqueue_impl(Element *e) {
			Element *t, *u;

			e->setNext(NULL, T_ElemNum);
			u = _tail;
			do {
				t = u;
			} while ((u = __sync_val_compare_and_swap(&_tail, t, e)) != t);
			if (t) {
				// PAMI_assert(t->next == NULL);
				t->setNext(e, T_ElemNum); // t->next was NULL...
			} else {
				// q->tail already set to 'e'...
				_head = e; // q->head was NULL...
			}
			//__sync_fetch_and_add(&_size, 1);
		}

		inline Element *dequeue_impl() {
			PAMI_abortf("dequeue not used - only merge");
			return NULL;
		}

		inline void push_impl(Element *element) {
			PAMI_abortf("push not used - only enqueue");
		}

		inline Element *peek_impl() {
			return _head;
		}

		inline bool isEmpty_impl() {
			return(_head == NULL);
		}

		inline Element *next_impl(Element *reference) {
			return reference->next(T_ElemNum);
		}

		inline Element *tail_impl() {
			return _tail;
		}

		inline Element *before_impl(Element *reference) {
			PAMI_abortf("before not supported");
			return NULL;
		}

		inline void insert_impl(Element *reference, Element *element) {
			PAMI_abortf("insert not supported");
		}

		inline void append_impl(Element *reference, Element *element) {
			PAMI_abortf("append not supported");
		}

		inline void remove_impl(Element *element) {
			PAMI_abortf("SE Queue does not support simple remove");
		}

		inline size_t size_impl() {
			return _size;
		}

		inline void dump_impl(const char *str, int n) {
			PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
		}

#ifdef VALIDATE_ON
		inline void validate_impl() {
			PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
		}
#endif

		// Iterator implementation

		inline void iter_init_impl(Iterator *iter) {
			new (&iter->_queue) GccThreadSafeMultiQueue(); // nothing more needed?
			iter->parent = iter->next_par = NULL;
			iter->curr = iter->next = NULL;
		}

		inline bool iter_begin_impl(Iterator *iter) {
			// pick up any new work...
			bool did = __merge(&iter->_queue, this);
			iter->parent = NULL;
			iter->curr = iter->_queue.peek();
			return did;
		}

		inline bool iter_check_impl(Iterator *iter) {
			if (iter->curr == NULL) {
				// done with this pass...
				return false;
			}
			// save next element, so we survive removal.
			iter->next = iter->_queue.nextElem(iter->curr);
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
			Element *n, *e, **h;

			if (iter->parent) h = iter->parent->nextPtr(T_ElemNum);
			else h = &iter->_queue._head;

			e = *h;
			n = e->next(T_ElemNum);
			if (n == NULL) {
				if (iter->_queue._tail != e) {
					// can't remove, an old appender is still appending...
					return PAMI_EAGAIN;
				}
				iter->_queue._tail = iter->parent;
			}
			*h = n;
			e->setNext(NULL, T_ElemNum);
			//main->_size -= 1;
			iter->next_par = iter->parent; // keep same parent for next iter
			return PAMI_SUCCESS;
		}

		inline void iter_dump_impl(const char *str, Iterator *iter) {
			PAMI_abortf("iter_dump not implemented");
		}

	private:
		inline bool __merge(GccThreadSafeMultiQueue<T_NumElems,T_ElemNum> *main,
				GccThreadSafeMultiQueue<T_NumElems,T_ElemNum> *new_work) {
			Element *h, *t, *qt;
			//size_t c;

			h = new_work->peek();
			if (!h) return false;

			new_work->_head = NULL;
			do {
				t = new_work->_tail;
				//c = new_work->_size;
			} while (!__sync_bool_compare_and_swap(&new_work->_tail, t, NULL));

			qt = main->_tail;
			if (qt) {
				// PAMI_assert(qt->_next == NULL);
				qt->setNext(h, T_ElemNum);
			} else {
				// PAMI_assert(main->_head == NULL);
				main->_head = h;
			}
			main->_tail = t;
			//main->_size += c;
			//__sync_fetch_and_add(&new_work->_size, -c);
			return true;
		}

	protected:
		Element *_head;
		Element *_tail;
		size_t    _size;
	}; // class GccThreadSafeMultiQueue
}; // namespace PAMI

#endif // __util_queue_GccThreadSafeMultiQueue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
