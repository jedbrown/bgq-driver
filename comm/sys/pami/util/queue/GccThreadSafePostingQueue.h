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
///  \file util/queue/GccThreadSafePostingQueue.h
///  \brief QueueElem and Queue Classes for multi-queue objects
///
#ifndef __util_queue_GccThreadSafePostingQueue_h__
#define __util_queue_GccThreadSafePostingQueue_h__

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

	template <class T_PubQueue>
	class GccThreadSafePostingQueue :
		public PAMI::Interface::DequeInterface<
			GccThreadSafePostingQueue<T_PubQueue>,
			typename T_PubQueue::Element
			>,
		public PAMI::Interface::QueueInfoInterface<
			GccThreadSafePostingQueue<T_PubQueue>,
			typename T_PubQueue::Element
			>,
		public PAMI::Interface::QueueIterator<
			GccThreadSafePostingQueue<T_PubQueue>,
			typename T_PubQueue::Element,
			typename T_PubQueue::Iterator
			>
	{
	public:
		static const size_t ALIGNMENT = sizeof(void *);
		typedef typename T_PubQueue::Element Element;
		typedef typename T_PubQueue::Iterator Iterator;

		inline GccThreadSafePostingQueue() :
		PAMI::Interface::DequeInterface<
			GccThreadSafePostingQueue<T_PubQueue>,
			typename T_PubQueue::Element
			>(),
		PAMI::Interface::QueueInfoInterface<
			GccThreadSafePostingQueue<T_PubQueue>,
			typename T_PubQueue::Element
			>(),
		PAMI::Interface::QueueIterator<
			GccThreadSafePostingQueue<T_PubQueue>,
			typename T_PubQueue::Element,
			typename T_PubQueue::Iterator
			>(),
		_pub_queue()
		{}

		static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
			return T_PubQueue::checkCtorMm(mm);
		}

		static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
			return T_PubQueue::checkDataMm(mm);
		}

		inline void init(PAMI::Memory::MemoryManager *mm, const char *key) {
			_pub_queue.init(mm, key);
		}

		inline void enqueue_impl(Element *e) {
			Memory::sync();	// traditional producer-side msync
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

		// This is a destructive iterator, as needed by a Posting queue.

		inline void iter_init_impl(Iterator *iter) {
			iter->curr = iter->next = NULL;
		}

		inline bool iter_begin_impl(Iterator *iter) {
			// pick up any new work...
			Element *h, *t;
			size_t c;

			_pub_queue.removeAll(h, t, c);
			iter->curr = h;
			return (h != NULL);
		}

		inline bool iter_check_impl(Iterator *iter) {
			if (iter->curr == NULL) {
				// done with this pass...
				return false;
			}
			// save next element, so we survive removal.
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
			return PAMI_SUCCESS;
		}

		inline void iter_dump_impl(const char *str, Iterator *iter) {
			PAMI_abortf("iter_dump not implemented");
		}

	protected:
		T_PubQueue _pub_queue;
	}; // class GccThreadSafePostingQueue
}; // namespace PAMI

#endif // __util_queue_GccThreadSafeQueue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments --max-instatement-indent=79
// astyle options --min-conditional-indent=2 --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements --align-pointer=name --lineend=linux
//
