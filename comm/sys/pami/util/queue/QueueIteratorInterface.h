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
///  \file util/queue/QueueIteratorInterface.h
///  \brief Queue Iterator Interface class
///
#ifndef __util_queue_QueueIteratorInterface_h__
#define __util_queue_QueueIteratorInterface_h__

namespace PAMI {
namespace Interface {

///
/// \brief Basic queue iterator class
///
/// Example Use:
///
///	T_Queue myqueue;
///	...
///	T_Iterator iter;
///	iter_init(&iter, myqueue);
///	...
///	for (iter_begin(&iter); iter_check(&iter); iter_end(&iter)) {
///		T_Queue::Element *elem = iter_current(&iter);
///		...
///		if (iter_remove(&iter) == PAMI_SUCCESS) free(elem);
///		...
///	}
///
template <class T_Queue, class T_Element, typename T_Iterator>
class QueueIterator {
public:
	inline QueueIterator() {
	}

	/// \brief initialize iterator for a given queue
	///
	/// \param[out] iter	Iterator data structure
	/// \param[in] que	Queue that will be iterated over
	///
	inline void iter_init(T_Iterator *iter);

	/// \brief Begin an iteration loop
	///
	/// Resets iterator from any possible previous state.
	///
	/// \param[in] iter	Iterator data structure
	/// \return	True if new work discovered (e.g. merge done)
	///
	inline bool iter_begin(T_Iterator *iter);

	/// \brief test if iteration loop may continue (element exists)
	///
	/// \param[in] iter	Iterator data structure
	/// \return	True if another (current) element exists
	///
	inline bool iter_check(T_Iterator *iter);

	/// \brief end iteration loop (not end of all iteration!)
	///
	/// \param[in] iter	Iterator data structure
	///
	inline void iter_end(T_Iterator *iter);

	/// \brief remove current element in iteration
	///
	/// \param[in] iter	Iterator data structure
	/// \return	PAMI_SUCCESS if element was removed,
	///		otherwise caller must arrange additional attempts
	///
	inline pami_result_t iter_remove(T_Iterator *iter);

	/// \brief get reference to current element in iteration
	///
	/// \param[in] iter	Iterator data structure
	///
	inline T_Element *iter_current(T_Iterator *iter);

	/// \brief dump iterator and associated objects
	///
	/// \param[in] str	(optional) String to prefix to outout
	/// \param[in] iter	Iterator data structure
	///
	inline void iter_dump(const char *str, T_Iterator *iter);

}; // class QueueIterator

}; // namespace PAMI::Interface
}; // namespace PAMI

// ----------------------------------------------------------------------------
//
// PAMI::Interface::QueueIterator<T_Queue,T_Element,T_Iterator> method definitions
//
// ----------------------------------------------------------------------------

template <class T_Queue, class T_Element, typename T_Iterator>
inline void PAMI::Interface::QueueIterator<T_Queue,T_Element,T_Iterator>::iter_init(T_Iterator *iter) {
	static_cast<T_Queue *>(this)->iter_init_impl(iter);
}

template <class T_Queue, class T_Element, typename T_Iterator>
inline bool PAMI::Interface::QueueIterator<T_Queue,T_Element,T_Iterator>::iter_begin(T_Iterator *iter) {
	return static_cast<T_Queue *>(this)->iter_begin_impl(iter);
}

template <class T_Queue, class T_Element, typename T_Iterator>
inline bool PAMI::Interface::QueueIterator<T_Queue,T_Element,T_Iterator>::iter_check(T_Iterator *iter) {
	return static_cast<T_Queue *>(this)->iter_check_impl(iter);
}

template <class T_Queue, class T_Element, typename T_Iterator>
inline void PAMI::Interface::QueueIterator<T_Queue,T_Element,T_Iterator>::iter_end(T_Iterator *iter) {
	static_cast<T_Queue *>(this)->iter_end_impl(iter);
}

template <class T_Queue, class T_Element, typename T_Iterator>
inline pami_result_t PAMI::Interface::QueueIterator<T_Queue,T_Element,T_Iterator>::iter_remove(T_Iterator *iter) {
	return static_cast<T_Queue *>(this)->iter_remove_impl(iter);
}

template <class T_Queue, class T_Element, typename T_Iterator>
inline T_Element *PAMI::Interface::QueueIterator<T_Queue,T_Element,T_Iterator>::iter_current(T_Iterator *iter) {
	return static_cast<T_Queue *>(this)->iter_current_impl(iter);
}

template <class T_Queue, class T_Element, typename T_Iterator>
inline void PAMI::Interface::QueueIterator<T_Queue,T_Element,T_Iterator>::iter_dump(const char *str, T_Iterator *iter) {
	static_cast<T_Queue *>(this)->iter_dump_impl(str, iter);
}

#endif // __util_queue_QueueIteratorInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
