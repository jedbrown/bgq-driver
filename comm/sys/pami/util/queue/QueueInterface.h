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
///  \file util/queue/QueueInterface.h
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
#ifndef __util_queue_QueueInterface_h__
#define __util_queue_QueueInterface_h__

#define COMPILE_DEPRECATED_QUEUE_INTERFACES

namespace PAMI
{
  namespace Interface
  {
    ///
    /// \brief Basic queue element class
    ///
    /// Queue interface implementations must use template specialization to
    /// provide a different queue element implementation that is optimized for
    /// the specific queue implementation.
    ///
    /// \code
    /// namespace PAMI
    /// {
    ///   // Forward declaration of the queue class in order to spectialize
    ///   // the template definition of the "Interface::QueueElement<T>" class.
    ///   class ExampleQueue;
    ///
    ///   namespace Interface
    ///   {
    ///     template <>
    ///     class QueueElement<ExampleQueue>
    ///     {
    ///       public:
    ///         inline QueueElement () :
    ///           prev (NULL),
    ///           next (NULL)
    ///         { };
    ///
    ///       protected:
    ///         QueueElement * prev;
    ///         QueueElement * next;
    ///
    ///         // This friend class declaration allows the "ExampleQueue"
    ///         // class to directly access the protected and private data
    ///         // members of this queue element class.
    ///         friend class ExampleQueue;
    ///     };
    ///   };
    ///
    ///   class ExampleQueue : public Interface::QueueInterface<ExampleQueue>
    ///   {
    ///     public:
    ///       // This typedef allows users of the "ExampleQueue" class to refer
    ///       // to the queue elements as 'ExampleQueue::Element" instead of
    ///       // the complicated template type.
    ///       typedef Interface::QueueElement<ExampleQueue> Element;
    ///
    ///       inline ExampleQueue() :
    ///         PAMI::Interface::QueueInterface<ExampleQueue> (),
    ///         _head (NULL),
    ///         _tail (NULL)
    ///       { };
    ///
    ///       inline ExampleQueue::Element * next_impl (ExampleQueue::Element * reference)
    ///       {
    ///         return reference->next;
    ///       };
    ///
    ///       // . . .
    ///
    ///     private:
    ///       Element _head;
    ///       Element _tail;
    ///   };
    /// };
    ///
    /// \endcode
    ///
    /// \tparam T_Queue
    ///
    template <class T_Queue>
    class QueueElement
    {
      protected:
        inline QueueElement ()
        {
          // This class constructor should never be invoked. All queue
          // interface implementations must provide a template specialization
          // of this queue element class.
          //COMPILE_TIME_ASSERT(0 == 1);
        };
    };


    template < class T_Queue, class T_Element = QueueElement<T_Queue> >
    class QueueInterface
    {
      protected:

        ///
        /// \brief  Queue interface constructor
        ///
        inline QueueInterface () { };

      public:

        ///
        /// \brief Add an entire queue to the back of this queue
        ///
        /// All elements from the reference queue will be added, in order, to
        /// the end of this queue.  The reference queue will be empty after the
        /// enqueue operation.
        ///
        /// \param[in] queue  Queue to add
        ///
        inline void enqueue (T_Queue * queue);

        ///
        /// \brief Add a queue element to the back of the queuei
        ///
        /// \param[in] element  Queue element to add
        ///
        inline void enqueue (T_Element * element) __attribute__((__always_inline__));

        ///
        /// \brief Remove a queue element from the front of the queue
        ///
        /// The 'dequeue' operation is identical to the 'pop' operation and the
        /// pop method is implemented as an alias to the dequeue method.
        ///
        /// \see enqueue
        /// \see pop
        ///
        /// \return The previous front queue element of the queue
        ///
        inline T_Element * dequeue ();

        ///
        /// \brief Add a queue element to the front of the queue
        ///
        /// \param[in] element  Queue element to add
        ///
        inline void push (T_Element * element);

        ///
        /// \brief Remove a queue element from the front of the queue
        ///
        /// The 'pop' operation is identical to the 'dequeue' operation and the
        /// pop method is implemented as an alias to the dequeue method.
        ///
        /// \see dequeue
        /// \see push
        ///
        /// \return The previous front queue element of the queue
        ///
        inline T_Element * pop () { return dequeue(); };

        ///
        /// \brief Access the front element of the queue without removing
        /// \return The front element of the queue
        ///
        inline T_Element * peek ();

        ///
        /// \brief  Query the queue empty state
        /// \retval true  Queue is empty
        /// \retval false Queue is not empty
        ///
        inline bool isEmpty();

        ///
        /// \brief Access the next queue element in queue from the reference queue element
        /// \param[in] reference Queue element to query
        /// \return The next queue element in the queue
        ///
        inline T_Element * next (T_Element * reference);

        ///
        /// \brief Removal all elements and return.
	///
	/// This is used to merge the queue contents onto another (private) queue.
	///
        /// \param[out] head	previous head of queue or NULL if empty
        /// \param[out] tail	previous tail of queue if head not NULL
        /// \param[out] size	previous size of queue if head not NULL
	///
	inline void removeAll(T_Element *&head, T_Element *&tail, size_t &size);

        ///
        /// \brief Append all elements and return.
	///
	/// This is used to merge the queue contents onto another (private) queue.
	///
        /// \param[out] head	head of elements to append
        /// \param[out] tail	tail of elements to append
        /// \param[out] size	size of elements to append
	///
	inline void appendAll(T_Element *head, T_Element *tail, size_t size);

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
        ///
        /// \brief Add an element to the tail of the queuei
        /// \deprecated
        /// \param[in] element  Queue element to push onto the tail of the queue
        ///
        inline void pushTail (T_Element * element)
        {
          enqueue (element);
        };

        ///
        /// \brief Add an element to the head of the queue
        /// \deprecated
        /// \param[in] element  Queue element to push onto the head of the queue
        ///
        inline void pushHead (T_Element * element)
        {
          push (element);
        };

        ///
        /// \brief Remove an element from the head of the queue
        /// \deprecated
        /// \return The removed head element of the queue
        ///
        inline T_Element * popHead ()
        {
          return pop ();
        };

        ///
        /// \brief Remove an element from the tail of the queue
        /// \deprecated
        /// \return The removed tail element of the queue
        ///
        inline T_Element * popTail ();

        ///
        /// \brief Access the head element of the queue without removing
        /// \deprecated
        /// \see PAMI::Interface::QueueInterface::peek
        /// \see PAMI::Interface::DequeInterface::head
        /// \return The head element of the queue
        ///
        inline T_Element * peekHead ()
        {
          return peek ();
        };

        ///
        /// \brief Access the tail element of the queue without removing
        /// \deprecated
        /// \see PAMI::Interface::DequeInterface::tail
        /// \return The tail element of the queue
        ///
        inline T_Element * peekTail ();

        ///
        /// \brief Get next element in queue after this one
        /// \deprecated
        /// \param[in] element Element to take 'next' of
        /// \return The next element in list/queue order
        ///
        inline T_Element * nextElem (T_Element * element)
        {
          return next (element);
        };

#endif
    }; // class PAMI::Interface::QueueInterface

    ///
    /// \brief Double-ended queue, a.k.a. "deque", interface
    ///
    /// The deque interface inherits from the simple queue interface to allow
    /// interface implementation classes to optimize the simple queue
    /// operation in addition to the double-ended queue operations.
    ///
    /// However, all of the simple queue operations may be emulated by one or
    /// more double-ended queue operations. These emulation should be inlined
    /// by the compiler.
    ///
    ///   enqueue(e) == append(tail(),e)
    ///   dequeue(e) == remove(head())
    ///   pop()      == remove(head())
    ///   push(e)    == insert(head(),e)
    ///   peek()     == head()
    ///   next(e)    == after(e)
    ///
    template < class T_Queue, class T_Element = QueueElement<T_Queue> >
    class DequeInterface : public QueueInterface<T_Queue, T_Element>
    {
      protected:

        ///
        /// \brief  Double-ended queue interface constructor
        ///
        inline DequeInterface () :
            QueueInterface<T_Queue, T_Element> ()
        {};

      public:

        ///
        /// \brief Return the queue element at the head (front) of the queue
        ///
        /// The 'head' operation is identical to the 'peek' operation and the
        /// head method is implemented as an alias to the peek method.
        ///
        /// \note  The queue element is not removed from the queue
        ///
        /// \see peek
        /// \see tail
        ///
        /// \return Queue element at the head of the queue, \c NULL if empty
        ///
        inline T_Element * head ();

        ///
        /// \brief Return the queue element at the tail (end) of the queue
        ///
        /// \note  The queue element is not removed from the queue
        ///
        /// \see head
        ///
        /// \return Queue element at the tail of the queue, \c NULL if empty
        ///
        inline T_Element * tail ();

        ///
        /// \brief Return the queue element before the reference queue element
        /// \note  The queue element is not removed from the queue
        /// \param[in] reference  Queue element in a queue
        /// \return Queue element before the reference queue element, \c NULL
        ///         if the reference queue element is the head queue element
        ///
        inline T_Element * before (T_Element * reference);

        ///
        /// \brief Return the queue element after the reference queue element
        ///
        /// The 'after' operation is identical to the 'next' operation and the
        /// after method is implemented as an alias to the next method.
        ///
        /// \note  The queue element is not removed from the queue
        ///
        /// \see next
        ///
        /// \param[in] reference  Queue element in a queue
        ///
        /// \return Queue element after the reference queue element, \c NULL
        ///         if the reference queue element is the tail queue element
        ///
        inline T_Element * after (T_Element * reference);

        ///
        /// \brief Insert the queue element before the reference queue element
        /// \param[in] reference  Queue element in a queue
        /// \param[in] element    Queue element to insert
        ///
        inline void insert (T_Element * reference, T_Element * element);

        ///
        /// \brief Append the queue element after the reference queue element
        /// \param[in] reference  Queue element in a queue
        /// \param[in] element    Queue element to append
        ///
        inline void append (T_Element * reference, T_Element * element);

        ///
        /// \brief Remove the queue element from the queue
        /// \param[in] element    Queue element to remove
        ///
        inline void remove (T_Element * element);

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
        ///
        /// \brief Remove element from queue
        /// \deprecated
        /// \param[in] element Element to be removed
        ///
        inline void deleteElem (T_Element * element);
#endif
    }; // class PAMI::Interface::DequeInterface

    template < class T_Queue, class T_Element = QueueElement<T_Queue> >
    class QueueInfoInterface
    {
      protected:

        ///
        /// \brief  Queue informational interface constructor
        ///
        inline QueueInfoInterface () {};

      public:

        ///
        /// \brief Query the size of the queue
        /// \return Number of elements in the queue
        ///
        inline size_t size ();

        ///
        /// \brief Write the queue state to stdout
        /// \param[in] str A string to prepend to the output
        /// \param[in] n   An integer to print and append to the 'str'
        ///
        inline void dump (const char * str, int n);

#ifdef VALIDATE_ON
        ///
        /// \brief Validate queue for errors, inconsistencies, etc.
        ///
        /// This is a diagnostics tool.
        ///
        inline void validate ();
#endif

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
        ///
        /// \brief Insert element into the queue at specified position
        /// \deprecated
        ///
        /// \param[in] element  Element to insert
        /// \param[in] position Insert position relative to head (0 = before head)
        ///
        /// Should this be 'deprecated' in a different interface class?
        ///
        inline void insertElem (T_Element * element, size_t position);
#endif
    }; // class PAMI::Interface::QueueInfoInterface
  };   // namespace PAMI::Interface
};     // namespace PAMI

// ----------------------------------------------------------------------------
//
// PAMI::Interface::QueueInterface<T_Queue,T_Element> method definitions
//
// ----------------------------------------------------------------------------

template <class T_Queue, class T_Element>
inline void PAMI::Interface::QueueInterface<T_Queue, T_Element>::enqueue (T_Queue * queue)
{
  static_cast<T_Queue *>(this)->enqueue_impl (queue);
}

template <class T_Queue, class T_Element>
inline void PAMI::Interface::QueueInterface<T_Queue, T_Element>::enqueue (T_Element * element)
{
  static_cast<T_Queue *>(this)->enqueue_impl (element);
}

template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::QueueInterface<T_Queue, T_Element>::dequeue ()
{
  return static_cast<T_Queue *>(this)->dequeue_impl ();
}

template <class T_Queue, class T_Element>
inline void PAMI::Interface::QueueInterface<T_Queue, T_Element>::push (T_Element * element)
{
  static_cast<T_Queue *>(this)->push_impl (element);
}

template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::QueueInterface<T_Queue, T_Element>::peek ()
{
  return static_cast<T_Queue *>(this)->peek_impl ();
}

template <class T_Queue, class T_Element>
inline bool PAMI::Interface::QueueInterface<T_Queue, T_Element>::isEmpty()
{
  return static_cast<T_Queue *>(this)->isEmpty_impl ();
}

template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::QueueInterface<T_Queue, T_Element>::next (T_Element * reference)
{
  return static_cast<T_Queue *>(this)->next_impl (reference);
}

template <class T_Queue, class T_Element>
inline void PAMI::Interface::QueueInterface<T_Queue, T_Element>::removeAll(T_Element *&head, T_Element *&tail, size_t &size)
{
	static_cast<T_Queue *>(this)->removeAll_impl(head, tail, size);
}

template <class T_Queue, class T_Element>
inline void PAMI::Interface::QueueInterface<T_Queue, T_Element>::appendAll(T_Element *head, T_Element *tail, size_t size)
{
	static_cast<T_Queue *>(this)->appendAll_impl(head, tail, size);
}

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::QueueInterface<T_Queue, T_Element>::popTail ()
{
  return static_cast<T_Queue *>(this)->popTail_impl ();
}

template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::QueueInterface<T_Queue, T_Element>::peekTail ()
{
  return static_cast<T_Queue *>(this)->peekTail_impl ();
}
#endif

// ----------------------------------------------------------------------------
//
// PAMI::Interface::DequeInterface<T_Queue,T_Element> method definitions
//
// ----------------------------------------------------------------------------

template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::DequeInterface<T_Queue, T_Element>::head ()
{
  return static_cast<T_Queue *>(this)->peek_impl ();
}

template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::DequeInterface<T_Queue, T_Element>::tail ()
{
  return static_cast<T_Queue *>(this)->tail_impl ();
}

template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::DequeInterface<T_Queue, T_Element>::before (T_Element * reference)
{
  return static_cast<T_Queue *>(this)->before_impl (reference);
}

template <class T_Queue, class T_Element>
inline T_Element * PAMI::Interface::DequeInterface<T_Queue, T_Element>::after (T_Element * reference)
{
  return static_cast<T_Queue *>(this)->next_impl (reference);
}

template <class T_Queue, class T_Element>
inline void PAMI::Interface::DequeInterface<T_Queue, T_Element>::insert (T_Element * reference,
                                                                        T_Element * element)
{
  static_cast<T_Queue *>(this)->insert_impl (reference, element);
}

template <class T_Queue, class T_Element>
inline void PAMI::Interface::DequeInterface<T_Queue, T_Element>::append (T_Element * reference,
                                                                        T_Element * element)
{
  static_cast<T_Queue *>(this)->append_impl (reference, element);
}

template <class T_Queue, class T_Element>
inline void PAMI::Interface::DequeInterface<T_Queue, T_Element>::remove (T_Element * element)
{
  static_cast<T_Queue *>(this)->remove_impl (element);
}

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
template <class T_Queue, class T_Element>
inline void PAMI::Interface::DequeInterface<T_Queue, T_Element>::deleteElem (T_Element * element)
{
  static_cast<T_Queue *>(this)->remove_impl (element);
}
#endif

// ----------------------------------------------------------------------------
//
// PAMI::Interface::QueueInfoInterface<T_Queue,T_Element> method definitions
//
// ----------------------------------------------------------------------------

template <class T_Queue, class T_Element>
inline size_t PAMI::Interface::QueueInfoInterface<T_Queue, T_Element>::size ()
{
  return static_cast<T_Queue *>(this)->size_impl ();
}

template <class T_Queue, class T_Element>
inline void PAMI::Interface::QueueInfoInterface<T_Queue, T_Element>::dump (const char * str, int n)
{
  return static_cast<T_Queue *>(this)->dump_impl (str, n);
}

#ifdef VALIDATE_ON
template <class T_Queue, class T_Element>
inline void PAMI::Interface::QueueInfoInterface<T_Queue, T_Element>::validate ()
{
  static_cast<T_Queue *>(this)->validate_impl ();
}
#endif

#ifdef COMPILE_DEPRECATED_QUEUE_INTERFACES
template <class T_Queue, class T_Element>
inline void PAMI::Interface::QueueInfoInterface<T_Queue, T_Element>::insertElem (T_Element * element, size_t position)
{
  static_cast<T_Queue *>(this)->insertElem_impl (element, position);
}
#endif

#endif // __util_queue_QueueInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
