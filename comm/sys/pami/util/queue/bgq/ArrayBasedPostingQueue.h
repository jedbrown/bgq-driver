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
///  \file util/queue/bgq/ArrayBasedPostingQueue.h
///  \brief Fast Aarray Based Queue Classe for GenericDevice Posting
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
#ifndef __util_queue_bgq_ArrayBasedPostingQueue_h__
#define __util_queue_bgq_ArrayBasedPostingQueue_h__

#include <stdio.h>

#include "components/memory/MemoryManager.h"

#include "util/common.h"
#include "util/queue/QueueInterface.h"
#include "util/queue/QueueIteratorInterface.h"
#include "util/queue/Queue.h"
#include "spi/include/l2/atomic.h"
#include "spi/include/l1p/flush.h"
#include "Global.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)
#endif

#define DEFAULT_SIZE         2048

#define L2_ATOMIC_FULL        0x8000000000000000UL
#define L2_ATOMIC_EMPTY       0x8000000000000000UL

namespace PAMI {
	///
	/// \brief A fast array based queue based on L2 atomics. All calls
	/// except enqueue are not thread safe and must be called from one
	/// thread. In this algorithm each thread tries to enqueue to the
	/// array based queue via bounded-load-increment and if that fails
	/// it enqueues to the overflow queue. A clear disadvandage of this
	/// method is that once the overflow queue has elements, the
	/// algorithm will not use fast queue till the producers stop
	/// enqueueing.
	///
	///
	template <class T_Queue, class T_Element>
	struct ArrayBasedPostingQueueIterator {
		uint64_t head;
		uint64_t tail;
		T_Element *curr;
		T_Element *next;
	};

	// T_Queue must be thread-safe!
	template <class T_Mutex, class T_Queue>
	class
    __attribute__((__aligned__(L1D_CACHE_LINE_SIZE))) // xl requires the attribute after the class keyword for some reason; still works for gnu
  ArrayBasedPostingQueue :
		public PAMI::Interface::DequeInterface<
				ArrayBasedPostingQueue<T_Mutex, T_Queue>,
				Queue::Element >,
		public PAMI::Interface::QueueInfoInterface<
				ArrayBasedPostingQueue<T_Mutex, T_Queue>,
				Queue::Element >,
		public PAMI::Interface::QueueIterator<
				ArrayBasedPostingQueue<T_Mutex, T_Queue>,
				Queue::Element,
				ArrayBasedPostingQueueIterator<
					ArrayBasedPostingQueue<T_Mutex, T_Queue>,
					Queue::Element> >
	{
	private:

		struct ArrayBasedPostingQueue_s {
			volatile uint64_t Consumer;	// not used atomically
			volatile uint64_t Producer;
			volatile uint64_t UpperBound;
			volatile uint64_t Flush;	// contents not used
		};

	public:
		const static bool removeAll_can_race = false;
		const static size_t ALIGNMENT = L1D_CACHE_LINE_SIZE;
		typedef typename T_Queue::Element  Element;
		typedef ArrayBasedPostingQueueIterator<
			ArrayBasedPostingQueue<T_Mutex, T_Queue>, Element > Iterator;

		inline ArrayBasedPostingQueue() :
		_array_q(),
		_ovf_mtx(),
		_overflow(),
		_array(NULL)
		{
		}

		static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
			return (T_Queue::checkCtorMm(mm) &&
				T_Mutex::checkCtorMm(mm) &&
				(mm->attrs() & PAMI::Memory::PAMI_MM_L2ATOMIC) != 0);
		}

		static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
			return true; // no mm will be used in init()...
		}

		inline void init(PAMI::Memory::MemoryManager *mm, const char *key) {
			pami_result_t rc;

			//Verify counter array is 32-byte aligned
			//printf ("Array Based Queue Initialized, atomics at address %p\n", &_array_q);
			PAMI_assert( (((uintptr_t)&_array_q) & (ALIGNMENT - 1)) == 0 );

			L2_AtomicStore(&_array_q.Consumer, 0);
			L2_AtomicStore(&_array_q.Producer, 0);
			L2_AtomicStore(&_array_q.UpperBound, DEFAULT_SIZE);

			rc = __global.heap_mm->memalign ((void **)&_array,
					L1D_CACHE_LINE_SIZE,
					sizeof(*_array) * DEFAULT_SIZE);
			PAMI_assert(rc == PAMI_SUCCESS);
			memset((void*)_array, 0, sizeof(*_array) * DEFAULT_SIZE);
		}

		inline void array_queue(uint64_t pos, Element *elm) {
			uint64_t qi = pos & (DEFAULT_SIZE - 1);
			_array[qi] = elm;
		}

		inline volatile Element *array_queue(uint64_t pos) {
			uint64_t qi = pos & (DEFAULT_SIZE - 1);
			return _array[qi];
		}

		/// \copydoc PAMI::Interface::QueueInterface::enqueue
		inline void enqueue_impl(Element *element) __attribute__((__always_inline__)) {
			L1P_FlushRequests();
			uint64_t index = L2_AtomicLoadIncrementBounded(&_array_q.Producer);
			if (index != L2_ATOMIC_FULL) {
				array_queue(index,element);
				return;
			}
			_ovf_mtx.acquire();
			// must check again to avoid race
			if ((index = L2_AtomicLoadIncrementBounded(&_array_q.Producer)) != L2_ATOMIC_FULL) {
				array_queue(index,element);
			} else {
				_overflow.enqueue((Queue::Element *)element);
			}
			_ovf_mtx.release();
		}

			/// \copydoc PAMI::Interface::QueueInterface::dequeue
		inline Element *dequeue_impl() {
			PAMI_abortf("dequeue not supported");
			return NULL;
		}

		/// \copydoc PAMI::Interface::QueueInterface::push (implemented as an enqueue)
		inline void push_impl(Element *element) {
			PAMI_abortf("push not supported");
		}

		/// \copydoc PAMI::Interface::QueueInterface::peek
		inline Element *peek_impl() {
			PAMI_abortf("peek not supported");
			return NULL;
		}

		/// \copydoc PAMI::Interface::QueueInterface::isEmpty
		inline bool isEmpty_impl() {
			PAMI_abortf("isEmpty not supported");
			return true;
		}

		/// \copydoc PAMI::Interface::QueueInterface::next
		inline Element *next_impl(Element *reference) {
			PAMI_abortf("next not supported");
			return NULL;
		}

		/// \copydoc PAMI::Interface::QueueInterface::removeAll
		inline void removeAll_impl(Element *&head, Element *&tail, size_t &size) {
			PAMI_abortf("removeAll not supported");
		}

		/// \copydoc PAMI::Interface::QueueInterface::appendAll
		inline void appendAll_impl(Element *head, Element *tail, size_t size) {
			PAMI_abortf("appendAll not supported");
		}

		/// \copydoc PAMI::Interface::DequeInterface::tail
		inline Element *tail_impl() {
			PAMI_abortf("tail not supported");
			return NULL;
		}

		/// \copydoc PAMI::Interface::DequeInterface::before
		inline Element *before_impl(Element *reference) {
			PAMI_abortf("before not supported");
			return NULL;
		}

		/// \copydoc PAMI::Interface::DequeInterface::insert
		inline void insert_impl(Element *reference,
					Element *element) {
			PAMI_abortf("insert not supported");
		}

		/// \copydoc PAMI::Interface::DequeInterface::append
		inline void append_impl(Element *reference,
					Element *element) {
			PAMI_abortf("append not supported");
		}

		/// \copydoc PAMI::Interface::DequeInterface::remove
		inline void remove_impl(Element *element) {
			PAMI_abortf("remove not supported");
		}

		/// \copydoc PAMI::Interface::QueueInfoInterface::size
		inline size_t size_impl() {
			PAMI_abortf("size not supported");
			return 0;
		}

		/// \copydoc PAMI::Interface::QueueInfoInterface::dump
		inline void dump_impl(const char *str, int n) {
			fprintf(stderr, "%s: PAMI::ArrayBasedPostingQueue "
				"%zd %zd %zd\n", str,
				_array_q.Producer,
				_array_q.Consumer,
				_array_q.UpperBound);
			_overflow.dump(str, n);
		}

#ifdef VALIDATE_ON
		/// \copydoc PAMI::Interface::QueueInfoInterface::validate
		inline void validate_impl() {
			PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);
		}
#endif

		// Iterator implementation:
		// This all works because there is only one thread removing (the iterator),
		// all others only append new work.

		inline void iter_init_impl(Iterator *iter) {
			iter->curr = iter->next = NULL;
			iter->head = iter->tail = 0;
		}

		inline bool iter_begin_impl(Iterator *iter) {
			// _overflow can only be !empty if _array_q is also !empty...
			iter->tail = _array_q.Producer;
			iter->head = _array_q.Consumer; // not needed?
			if (iter->head < iter->tail) ppc_msync(); // conditional too much?
			return (iter->head < iter->tail);
		}

		inline bool iter_check_impl(Iterator *iter) {
			if (iter->head < iter->tail) {
				// msync was done in iter_begin()...
				volatile Element *e = array_queue(iter->head);
				// it is reasonable to wait here since the
				// enqueuer will immediately update the pointer.
#ifndef DO_NOT_BLOCK
				while (e == NULL) {
					// or... do a ldarx/wait_rsv...
					e = array_queue(iter->head);
				}
#else // DO_NOT_BLOCK
				if (e == NULL) {
					_array_q.Consumer = iter->head; // bookmark my place
					return false;
				}
#endif // DO_NOT_BLOCK
				return true;
			}
			if (iter->curr != NULL) {
				// msync was done after removeAll() in iter_end()...
				iter->next = _overflow.nextElem(iter->curr);
				return true;
			}
			return false;
		}

		inline void iter_end_impl(Iterator *iter) {
			if (iter->head < iter->tail) {
				array_queue(iter->head, NULL);
				++iter->head;
				if (iter->head == iter->tail) {
					_array_q.Consumer = iter->head; // bookmark my place
					_ovf_mtx.acquire();
					// must ensure enqueuers did not fill array
					// while we processed this snapshot.
					// Note, the important case is where an
					// enqueuer *noticed* it full and used the
					// overflow queue. Also note, cannot empty
					// the overflow queue unless we emptied array!

					
					Element *t;
					size_t s;
					t = _overflow.peek();
					if (t == NULL || iter->head == _array_q.Producer) {
						uint64_t n = iter->head + DEFAULT_SIZE;
						// is atomic-store needed?
						L2_AtomicStore(&_array_q.UpperBound, n);
						if (t) {
							_overflow.removeAll(iter->curr, t, s);
							ppc_msync();
						}
					}
					_ovf_mtx.release();
				}
			} else {
				iter->curr = iter->next;
			}
		}

		inline Element *iter_current_impl(Iterator *iter) {
			if (iter->head < iter->tail) {
				volatile Element *e = array_queue(iter->head);
				return (Element *)e;
			}
			return iter->curr;
		}

		inline pami_result_t iter_remove_impl(Iterator *iter) {
			return PAMI_SUCCESS;
		}

		inline void iter_dump_impl(const char *str, Iterator *iter) {
			fprintf(stderr, "%s: PAMI::ArrayBasedPostingQueue::Iterator "
				"%ld %ld %p %p\n", str,
				iter->head, iter->tail, iter->curr, iter->next);
			dump_impl(str, 0);
		}

	protected:

		ArrayBasedPostingQueue_s _array_q;
		T_Mutex _ovf_mtx;
		T_Queue _overflow;
		volatile Element * volatile *_array;

	}; // class PAMI::ArrayBasedPostingQueue
}; // namespace PAMI

#endif // __util_queue_bgq_ArrayBasedPostingQueue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
