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
/**
 * \file components/atomic/BoundedCounterInterface.h
 * \brief Interface for Bounded Counters
 */

#ifndef __components_atomic_BoundedCounterInterface_h__
#define __components_atomic_BoundedCounterInterface_h__

namespace PAMI {
namespace BoundedCounter {
///
/// \brief Atomic bounded counter interface class
///
/// This is an interface class and may not be directly instantiated.
///
/// \tparam T  Atomic bounded counter implementation class
///
/// \note This interface represents a specialized use of atomics.
///       It is typically backed by special hardware, and so a software-
///       only implementation is not likely to perform well. Classes that
///       use bounded counters may not perform well on platforms that lack
///       hardware support.
///
template <class T>
class Interface {
protected:

	inline Interface() {}
	inline ~Interface() {}

public:

	///
	/// \brief Returns the 'bound error' value
	///
	/// The 'bound error' value is used to determine if a particular
	/// *_bounded operation was successful.
	///
	/// \attention All bounded counter interface derived classes \b must
	///            contain a public static const data member named
	///            'size_t bound_error'.
	///
	/// C++ code using templates to specify the model may statically
	/// access the 'bound_error' constant.
	///
	static const size_t getBoundError() {
		return T::bound_error;
	}

	///
	/// \brief Fetch the current value of the counter
	///
	/// \return	Current value of counter
	///
	inline size_t fetch() {
		return static_cast<T *>(this)->fetch_impl();
	}

	///
	/// \brief Fetch, then bounded increment the atomic counter object value
	///
	/// The following code demonstrates how to test for bounded condition:
	///
	/// \code
	/// MyBoundedCounter counter;
	/// if (counter.fetch_and_inc_bounded() == MyBoundedCounter::bound_error)
	/// {
	///    // .... increment was bounded.
	/// }
	/// \endcode
	///
	/// \attention All bounded atomic counter object derived classes
	///            \b must implement the fetch_and_inc_bounded_impl()
	///            method.
	///
	/// \return Previous counter value, or T::bound_error if the increment
	///         operation was bounded
	///
	inline size_t fetch_and_inc_bounded() {
		return static_cast<T *>(this)->fetch_and_inc_bounded_impl();
	}

	///
	/// \brief Fetch, then bounded decrement the atomic counter object value
	///
	/// The following code demonstrates how to test for bounded condition:
	///
	/// \code
	/// MyBoundedCounter counter;
	/// if (counter.fetch_and_dec_bounded() == MyBoundedCounter::bound_error)
	/// {
	///    // .... decrement was bounded.
	/// }
	/// \endcode
	///
	/// \attention All bounded atomic counter object derived classes
	///            \b must implement the fetch_and_dec_bounded_impl()
	///            method.
	///
	/// \return Previous counter value, or T::bound_error if the decrement
	///         operation was bounded
	///
	inline bool fetch_and_dec_bounded(size_t &value) {
		return static_cast<T *>(this)->fetch_and_dec_bounded_impl(value);
	}

	///
	/// \brief Fetch the current value of the upper bound
	///
	/// \return	Current value in upper bound
	///
	inline size_t bound_upper_fetch() {
		return static_cast<T *>(this)->bound_upper_fetch_impl();
	}

	///
	/// \brief Clear value of upper bound
	///
	inline void bound_upper_clear() {
		static_cast<T *>(this)->bound_upper_clear_impl();
	}

	///
	/// \brief Set value of upper bound
	///
	/// \param[in] value	New value for upper bound
	///
	inline void bound_upper_set(size_t value) {
		static_cast<T *>(this)->bound_upper_set_impl(value);
	}

	///
	/// \brief Fetch the current value of the upper bound and increment it
	///
	/// \return	Current value in upper bound, before increment
	///
	inline size_t bound_upper_fetch_and_inc() {
		return static_cast<T *>(this)->bound_upper_fetch_and_inc_impl();
	}

	///
	/// \brief Fetch the current value of the lower bound
	///
	/// \return	Current value in lower bound
	///
	inline size_t bound_lower_fetch() {
		return static_cast<T *>(this)->bound_lower_fetch_impl();
	}

	///
	/// \brief Clear value of lower bound
	///
	inline void bound_lower_clear() {
		static_cast<T *>(this)->bound_lower_clear_impl();
	}

	///
	/// \brief Set value of lower bound
	///
	/// \param[in] value	New value for lower bound
	///
	inline void bound_lower_set(size_t value) {
		static_cast<T *>(this)->bound_lower_set_impl(value);
	}

	///
	/// \brief Fetch the current value of the lower bound and decrement it
	///
	/// \return	Current value in lower bound, before increment
	///
	inline size_t bound_lower_fetch_and_dec() {
		return static_cast<T *>(this)->bound_lower_fetch_and_dec_impl();
	}

	static const bool indirect = false;

}; // PAMI::BoundedCounter::Interface class
};   // PAMI::BoundedCounter namespace
};     // PAMI namespace

#endif // __components_atomic_BoundedCounterInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments
// astyle options --max-instatement-indent=79 --min-conditional-indent=2
// astyle options --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements
// astyle options --align-pointer=name --lineend=linux
//
