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
///  \file util/queue/bgq/ArrayBasedQueue.h
///  \brief Fast Aarray Based Queue Classes
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
#ifndef __util_queue_bgq_ArrayBasedQueue_h__
#define __util_queue_bgq_ArrayBasedQueue_h__

#include <execinfo.h>
#include <stdio.h>
#include <malloc.h>

#include "components/memory/MemoryManager.h"

#include "util/common.h"
#include "util/queue/QueueInterface.h"
#include "util/queue/QueueIteratorInterface.h"
#include "util/queue/Queue.h"
#include "spi/include/l2/atomic.h"
#include "Global.h"
#include "Memory.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)
#endif

#define DEFAULT_SIZE         2048

#ifndef L1D_CACHE_LINE_SIZE
#define L1D_CACHE_LINE_SIZE  64
#endif

#define L2_ATOMIC_FULL        0x8000000000000000UL
#define L2_ATOMIC_EMPTY       0x8000000000000000UL

#define USE_GUARDED_WC_FLUSH 0

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
  template <class T_Mutex>
    class
      __attribute__((__aligned__(L1D_CACHE_LINE_SIZE))) // xl requires the attribute after the class keyword for some reason; still works for gnu
    ArrayBasedQueue : public PAMI::Interface::DequeInterface<ArrayBasedQueue<T_Mutex>,
    Queue::Element >,
    public PAMI::Interface::QueueInfoInterface<ArrayBasedQueue<T_Mutex>,
    Queue::Element >,
    public PAMI::Interface::QueueIterator<ArrayBasedQueue<T_Mutex>,
    Queue::Element,
    BasicQueueIterator<ArrayBasedQueue<T_Mutex>, Queue::Element> > {

  public:
    const static bool removeAll_can_race = false;
    typedef Queue::Element  Element;
    typedef BasicQueueIterator<ArrayBasedQueue<T_Mutex>, Element > Iterator;

  protected:

    ///
    ///  \brief Move queue elments from array/overflow queues to
    ///  private queue. This routine must be called from only one
    ///  thread.
    ///
    inline bool  advance() {
      bool newwork = false;
      int tid = Kernel_ProcessorThreadID();
      uint64_t index = *_tailLoadAddress[tid];
      uint64_t head  = _headCounter;

      while ( head < index ) {
	//fprintf(stderr, "Dequeueing index %ld, counter address %lx\n", index, (uint64_t)&_atomicCounters[0]);
	uint64_t qindex = head & (DEFAULT_SIZE - 1);
	head ++;

	//Wait till producer updates this
	while (_queueArray[qindex] == NULL);

	Element *element = (Element *)_queueArray[qindex];
	_queueArray[qindex] = NULL; //Mark the element as unused
	Memory::sync();
	//fprintf(stderr, "Found element %ld\n", index);

	_privateq.enqueue((Element *)element);
	newwork = true;

	//Increment the queue bound to permit another enqueue
	*_boundAddress[tid] = 1;  //Store increment operation
      }
      _headCounter = head;

      if (!_overflowq.isEmpty()) {
	_mutex.acquire();
	Queue::Element *head;
	Queue::Element *tail;
	size_t size;
	_overflowq.removeAll(head, tail, size);
	if (head) {
	  _privateq.appendAll (head,  tail,  size);
	  newwork = true;
	}
	_mutex.release();
      }

      return newwork;
    }

  public:
    inline ArrayBasedQueue() :  _overflowq(), _privateq(), _mutex()
    {
      _wakeup            = 0;
      _queueArray        = NULL;

      _atomicCounters    = NULL;
      _tailAddress[0]    = NULL;
      _tailAddress[1]    = NULL;
      _tailAddress[2]    = NULL;
      _tailAddress[3]    = NULL;

      _tailLoadAddress[0]    = NULL;
      _tailLoadAddress[1]    = NULL;
      _tailLoadAddress[2]    = NULL;
      _tailLoadAddress[3]    = NULL;

      _headCounter       = 0;
      _boundAddress[0]   = NULL;
      _boundAddress[1]   = NULL;
      _boundAddress[2]   = NULL;
      _boundAddress[3]   = NULL;
    }

    static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
      return true;
    }

    static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
      return true; // no mm will be used in init()...
    }

    inline void init(PAMI::Memory::MemoryManager *mm, const char *key)
      {
	//_mutex.init(); // in-place mutex
	_overflowq.init(mm, key);
	_privateq.init(mm, key);

	uint64_t *buffer;
	void *_buffer = NULL;
	int rc = 0;
	rc = __global.l2atomicFactory.__procscoped_mm.memalign(&_buffer,
							       L1D_CACHE_LINE_SIZE, L1D_CACHE_LINE_SIZE);
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to allocate L2 Atomic Counter");
	buffer = (uint64_t*)_buffer;
	//Verify counter array is 32-byte aligned

	//printf ("Array Based Queue Initialized, atomics at address %lx\n", (uint64_t)buffer);

	_atomicCounters = buffer;
	PAMI_assert( (((uint64_t)_atomicCounters) & 0x3f) == 0 );

	L2_AtomicStore(&_atomicCounters[0], 0);   //Consumer
	L2_AtomicStore(&_atomicCounters[1], 0);   //Producer
	L2_AtomicStore(&_atomicCounters[2], DEFAULT_SIZE); //Upper bound

	uint64_t tid = 0;
	for (tid = 0; tid < 4UL; tid ++)
	  _tailAddress[tid] =  (volatile uint64_t *)
	    (((Kernel_L2AtomicsBaseAddress() +
	       ((((uint64_t) &_atomicCounters[1]) << 5) & ~0xfful)) |
	      (tid << 6)) +
	     (4UL << 3)); /*bounded increment*/

	for (tid = 0; tid < 4UL; tid ++)
	  _tailLoadAddress[tid] =  (volatile uint64_t *)
	    (((Kernel_L2AtomicsBaseAddress() +
	       ((((uint64_t) &_atomicCounters[1]) << 5) & ~0xfful)) |
	      (tid << 6)) +
	     (0UL << 3)); /*Load counter*/

	for (tid = 0; tid < 4UL; tid ++)
	  _boundAddress[tid] =  (volatile uint64_t *)
	    (((Kernel_L2AtomicsBaseAddress() +
	       ((((uint64_t) &_atomicCounters[2]) << 5) & ~0xfful)) |
	      (tid << 6)) +
	     (2UL << 3)); /*Store add*/

#if USE_GUARDED_WC_FLUSH
	for (tid = 0; tid < 4UL; tid ++)
	  _flushAddress[tid] =  (volatile uint64_t *)
	    (((Kernel_L2AtomicsBaseAddress() +
	       ((((uint64_t) &_atomicCounters[3]) << 5) & ~0xfful)) |
	      (tid << 6)) +
	     (0UL << 3)); /*direct store*/
#endif

	_queueArray = (volatile Element * volatile *) memalign (L1D_CACHE_LINE_SIZE,
								sizeof(Element*) * DEFAULT_SIZE);
	PAMI_assert (_queueArray != NULL);
	memset ((void*)_queueArray, 0, sizeof(Element*) * DEFAULT_SIZE);
      }

    /// \copydoc PAMI::Interface::QueueInterface::enqueue
    inline void enqueue_impl(Element *element)
      {
	//printf("Calling enqueue\n");
	int tid = Kernel_ProcessorThreadID();
#if USE_GUARDED_WC_FLUSH
	*_flushAddress[tid] = 0; //Store 0 to the flush address to
	                         //flush all stores. Low overhead
	                         //non-blocking write fence
#endif
	//mbar();
	uint64_t index = 0;
	if ( likely (_overflowq.isEmpty() &&
		     ((index = *_tailAddress[tid]) != L2_ATOMIC_FULL)) )
	  {
	    uint64_t qindex = index & (DEFAULT_SIZE - 1);
	    //PAMI_assert ( _queueArray[qindex] == NULL);
	    //printf("Atomic increment of counter %lx returned index %lu\n", (uint64_t)&_atomicCounters[1], index);
	    _queueArray[qindex] = element;
	    //printf("After enqueue\n");
	    _wakeup = 1;
	    return;
	  }

	_mutex.acquire();
	_overflowq.enqueue((Queue::Element *)element);
	_mutex.release();
      }

      /// \copydoc PAMI::Interface::QueueInterface::dequeue
    inline Element *dequeue_impl()
      {
	advance();
	return 	_privateq.dequeue();
      }

    /// \copydoc PAMI::Interface::QueueInterface::push (implemented as an enqueue)
    inline void push_impl(Element *element)
      {
	enqueue_impl (element);
      }

    /// \copydoc PAMI::Interface::QueueInterface::peek
    inline Element *peek_impl()
      {
	advance();
	return _privateq.peek();
      }

    /// \copydoc PAMI::Interface::QueueInterface::isEmpty
    inline bool isEmpty_impl()
      {
	advance();

	return _privateq.isEmpty();
      }

    /// \copydoc PAMI::Interface::QueueInterface::next
    inline Element *next_impl(Element *reference)
      {
	advance();
	return _privateq.next(reference);
      }

    /// \copydoc PAMI::Interface::QueueInterface::removeAll
    inline void removeAll_impl(Element *&head, Element *&tail, size_t &size)
      {
	advance();
	_privateq.removeAll(head, tail, size);
      }

    /// \copydoc PAMI::Interface::QueueInterface::appendAll
    inline void appendAll_impl(Element *head, Element *tail, size_t size)
      {
	advance();
	_privateq.appendAll(head, tail, size);
      }

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
    /// \copydoc PAMI::Interface::QueueInterface::popTail
    inline Element *popTail_impl()
      {
	PAMI_abort();
	return NULL;
      }

    /// \copydoc PAMI::Interface::QueueInterface::peekTail
    inline Element *peekTail_impl()
      {
	PAMI_abort();
	return NULL;
      }
#endif

    /// \copydoc PAMI::Interface::DequeInterface::tail
    inline Element *tail_impl()
      {
	advance();
	return _privateq.tail();
      }

    /// \copydoc PAMI::Interface::DequeInterface::before
    inline Element *before_impl(Element *reference)
      {
	advance();
	return _privateq.before(reference);
      }

    /// \copydoc PAMI::Interface::DequeInterface::insert
    inline void insert_impl(Element *reference,
			    Element *element)
      {
	advance();
	return _privateq.insert(reference, element);
      }

    /// \copydoc PAMI::Interface::DequeInterface::append
    inline void append_impl(Element *reference,
			    Element *element)
      {
	advance();
	_privateq.append(reference, element);
      }

    /// \copydoc PAMI::Interface::DequeInterface::remove
    inline void remove_impl(Element *element)
      {
	advance();
	_privateq.remove(element);
      }

    /// \copydoc PAMI::Interface::QueueInfoInterface::size
    inline size_t size_impl()
      {
	advance();
	return _privateq.size_impl();
      }

    /// \copydoc PAMI::Interface::QueueInfoInterface::dump
    inline void dump_impl(const char *str, int n)
    {
      fprintf(stderr, "%s: size=%ld locked:%d\n", str,
	      size_impl(), _mutex.isLocked());
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
      bool newwork = advance();

      iter->curr =  (Element *)_privateq.peek();
      return newwork;
    }

    inline bool iter_check_impl(Iterator *iter) {
      if (iter->curr == NULL) {
	// done with this pass...
	return false;
      }
      iter->next = _privateq.nextElem(iter->curr);
      return  true;
    }

    inline void iter_end_impl(Iterator *iter) {
      iter->curr = iter->next;
    }

    inline Element *iter_current_impl(Iterator *iter) {
      return iter->curr;
    }

    inline pami_result_t iter_remove_impl(Iterator *iter) {
      _privateq.remove(iter->curr);
      return PAMI_SUCCESS;
    }

		inline void iter_dump_impl(const char *str, Iterator *iter) {
			PAMI_abortf("iter_dump not implemented");
		}

  protected:

    volatile Element          * volatile           * _queueArray;
    Queue                                            _overflowq;
    volatile uint64_t                              * _tailAddress[4];
    volatile uint64_t                              * _flushAddress[4];
    volatile uint64_t                              * _tailLoadAddress[4];
    volatile uint64_t                              * _boundAddress[4];
    uint64_t                                         _headCounter;
    Queue                                            _privateq;
    T_Mutex                                          _mutex;
    volatile uint64_t                                _wakeup;
    volatile uint64_t                              * _atomicCounters;

//  } __attribute__((__aligned__(L1D_CACHE_LINE_SIZE))); // class PAMI::ArrayBasedQueue
  } ; // class PAMI::ArrayBasedQueue
}; // namespace PAMI

#endif // __util_queue_queue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
