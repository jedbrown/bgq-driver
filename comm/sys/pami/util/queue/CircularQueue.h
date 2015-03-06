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
///  \file util/queue/CircularQueue.h
///  \brief Circular Queue Classes
///
#ifndef __util_queue_CircularQueue_h__
#define __util_queue_CircularQueue_h__

#include <stdio.h>

#include "components/memory/MemoryManager.h"

#include "util/common.h"
#include "util/queue/QueueInterface.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x)
#endif

namespace PAMI
{
  //
  // template specialization of queue element
  //
  class CircularQueue;

  namespace Interface
  {
    template <>
    class QueueElement<CircularQueue>
    {
      public:
        inline QueueElement ()
        {
          //TRACE_ERR((stderr, "template specialization\n"));
        };

        inline void set (QueueElement * previous, QueueElement * next)
        {
          _prev = previous;
          _next = next;
        }

        inline void setPrev (QueueElement * element)
        {
          _prev = element;
        }

        inline void setNext (QueueElement * element)
        {
          _next = element;
        }

        inline QueueElement * prev ()
        {
          return _prev;
        }

        inline QueueElement * next ()
        {
          return _next;
        }

      protected:

        QueueElement * _prev;
        QueueElement * _next;
    };
  };

  ///
  /// \brief Circular linked list implementation of PAMI::QueueInterface
  ///
  /// The circular queue class is a space-efficient queue implementation - only
  /// a single \c head pointer is maintained. The "tail" element of the
  /// circular queue is the "previous" element of the "head" element.
  ///
  class CircularQueue : public PAMI::Interface::QueueInterface<CircularQueue>
  {
    public:

      typedef Interface::QueueElement<CircularQueue> Element;

      inline CircularQueue() :
          PAMI::Interface::QueueInterface<CircularQueue> (),
          _head (NULL)
      {
      };

	static bool checkCtorMm(PAMI::Memory::MemoryManager *mm) {
                (void)mm;
		return true;
	}

	static bool checkDataMm(PAMI::Memory::MemoryManager *mm) {
                (void)mm;
		return true;
	}

      inline void init (PAMI::Memory::MemoryManager * mm, const char *key)
      {
        (void)mm;(void)key;
      };

      /// \copydoc PAMI::Interface::QueueInterface::enqueue
      inline void enqueue_impl (CircularQueue * queue)
      {
        if (unlikely(queue->_head == NULL))
        {
          // No elements in the reference queue -> nothing to do
          return;
        }
        else if (unlikely(_head == NULL))
        {
          // No elements in this queue
          _head = queue->_head;
        }
        else
        {
          // Some elements in both queues
          CircularQueue::Element * t0 = _head->prev();
          CircularQueue::Element * t1 = queue->_head->prev();

          t0->setNext(queue->_head);
          t1->setNext(_head);
          queue->_head->setPrev(t0);
          _head->setPrev(t1);
        }
        queue->_head = NULL;
      };

      /// \copydoc PAMI::Interface::QueueInterface::enqueue
      inline void enqueue_impl (CircularQueue::Element * element)
      {
        if (unlikely(_head == NULL))
        {
          // No elements in the queue
          element->set (element, element);
          _head = element;
        }
        else if (unlikely(_head->next() == _head))
        {
          // One element in the queue
          element->set (_head, _head);
          _head->set (element, element);
        }
        else
        {
          // More than one element in the queue
          element->set (_head->prev(), _head);
          _head->prev()->setNext(element);
          _head->setPrev (element);
        }
      };

      /// \copydoc PAMI::Interface::QueueInterface::dequeue
      inline CircularQueue::Element * dequeue_impl ()
      {
        if (unlikely(_head == NULL))
        {
          // No elements in the queue
          return NULL;
        }

        CircularQueue::Element * element = _head;
        if (unlikely(_head->next() == _head))
        {
          // One element in the queue
          _head = NULL;
        }
        else
        {
          // More than one element in the queue
          _head = _head->next();
          _head->setPrev (element->prev());
          element->prev()->setNext (_head);
        }

        return element;
      };

      /// \copydoc PAMI::Interface::QueueInterface::push
      inline void push_impl (CircularQueue::Element * element)
      {
        if (unlikely(_head == NULL))
        {
          // No elements in the queue
          _head = element;
          element->set (element, element);
          return;
        }

        element->set (_head->prev(), _head);
        _head->prev()->setNext (element);
        _head->setPrev (element);
        _head = element;
      };

      /// \copydoc PAMI::Interface::QueueInterface::peek
      inline CircularQueue::Element * peek_impl ()
      {
        return _head;
      };

      /// \copydoc PAMI::Interface::QueueInterface::isEmpty
      inline bool isEmpty_impl ()
      {
        return (_head == NULL);
      };

      /// \copydoc PAMI::Interface::QueueInterface::next
      inline CircularQueue::Element * next_impl (CircularQueue::Element * reference)
      {
        CircularQueue::Element * next = reference->next();

        if (unlikely(next == _head))
          return NULL;

        return next;
      };

    private:

      CircularQueue::Element * _head;

  }; // class PAMI::CircularQueue
}; // namespace PAMI

#endif // __util_queue_CircularQueue_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
