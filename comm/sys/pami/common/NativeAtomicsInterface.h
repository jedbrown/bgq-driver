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
 * \file common/NativeAtomicsInterface.h
 * \brief ???
 */

#ifndef __common_NativeAtomicsInterface_h__
#define __common_NativeAtomicsInterface_h__

#include "util/common.h"

namespace PAMI {
namespace Interface {

///
/// \brief Interface for native-atomic counter objects
///
/// This is a base classs for creating platform-(and compiler-)independent
/// implementations of __sync_fetch_and_add() et al.
/// For simplicity, all atomic objects are of type 'size_t'.
///
///
/// \param T_Object  Atomic counter object derived class
///

template <class T_Object>
class NativeAtomic {	// a.k.a. "GccAtomics"...
protected:

	NativeAtomic() {
		ENFORCE_STATIC_CONST_CLASS_MEMBER(T_Object,ALIGNMENT);
	}
	~NativeAtomic() { }
public:
	///
	/// \brief Fetch the atomic counter object value
	///
	/// \attention All atomic counter object derived classes \b must
	///            implement the fetch_impl() method.
	///
	/// \return Atomic counter object value
	///
	inline size_t fetch();
	inline void set(size_t val);

	///
	/// \brief Fetch, then add a value to the atomic object
	///
	/// \attention All atomic object derived classes \b must
	///            implement the fetch_and_add_impl() method.
	///
	/// \param[in] val	Value to be added
	/// \return Atomic object previous value
	///
	inline size_t fetch_and_add(size_t val);

	///
	/// \brief Fetch, then subtract a value from the atomic object
	///
	/// \attention All atomic counter object derived classes \b must
	///            implement the fetch_and_dec_impl() method.
	///
	/// \return Atomic counter object value
	///
	inline size_t fetch_and_sub(size_t val);
	inline size_t fetch_and_or(size_t val);
	inline size_t fetch_and_and(size_t val);
	inline size_t fetch_and_xor(size_t val);
	inline size_t fetch_and_nand(size_t val);

	inline size_t add_and_fetch(size_t val);
	inline size_t sub_and_fetch(size_t val);
	inline size_t or_and_fetch(size_t val);
	inline size_t and_and_fetch(size_t val);
	inline size_t xor_and_fetch(size_t val);
	inline size_t nand_and_fetch(size_t val);

	///
	/// \brief Fetch, then clear the atomic counter object value
	///
	/// \attention All atomic counter object derived classes \b must
	///            implement the fetch_and_clear_impl() method.
	///
	/// \return Atomic counter object value
	///
	inline size_t val_compare_and_swap(size_t oldv, size_t newv);
	inline bool bool_compare_and_swap(size_t oldv, size_t newv);
	inline void synchronize();
	inline size_t lock_test_and_set(size_t val);
	inline void lock_release();
	inline volatile size_t *return_atom();

}; // class NativeAtomics

template <class T_Object>
size_t NativeAtomic<T_Object>::fetch() {
	return static_cast<T_Object *>(this)->fetch_impl();
}
template <class T_Object>
void NativeAtomic<T_Object>::set(size_t val) {
	static_cast<T_Object *>(this)->set_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::fetch_and_add(size_t val) {
	return static_cast<T_Object *>(this)->fetch_and_add_impl(val);
}

template <class T_Object>
size_t NativeAtomic<T_Object>::fetch_and_sub(size_t val) {
	return static_cast<T_Object *>(this)->fetch_and_sub_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::fetch_and_or(size_t val) {
	return static_cast<T_Object *>(this)->fetch_and_or_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::fetch_and_and(size_t val) {
	return static_cast<T_Object *>(this)->fetch_and_and_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::fetch_and_xor(size_t val) {
	return static_cast<T_Object *>(this)->fetch_and_xor_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::fetch_and_nand(size_t val) {
	return static_cast<T_Object *>(this)->fetch_and_nand_impl(val);
}

template <class T_Object>
size_t NativeAtomic<T_Object>::add_and_fetch(size_t val) {
	return static_cast<T_Object *>(this)->add_and_fetch_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::sub_and_fetch(size_t val) {
	return static_cast<T_Object *>(this)->sub_and_fetch_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::or_and_fetch(size_t val) {
	return static_cast<T_Object *>(this)->or_and_fetch_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::and_and_fetch(size_t val) {
	return static_cast<T_Object *>(this)->and_and_fetch_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::xor_and_fetch(size_t val) {
	return static_cast<T_Object *>(this)->xor_and_fetch_impl(val);
}
template <class T_Object>
size_t NativeAtomic<T_Object>::nand_and_fetch(size_t val) {
	return static_cast<T_Object *>(this)->nand_and_fetch_impl(val);
}

template <class T_Object>
size_t NativeAtomic<T_Object>::val_compare_and_swap(size_t oldv, size_t newv) {
	return static_cast<T_Object *>(this)->val_compare_and_swap_impl(oldv, newv);
}
template <class T_Object>
bool NativeAtomic<T_Object>::bool_compare_and_swap(size_t oldv, size_t newv) {
	return static_cast<T_Object *>(this)->bool_compare_and_swap_impl(oldv, newv);
}
template <class T_Object>
void NativeAtomic<T_Object>::synchronize() {
	static_cast<T_Object *>(this)->synchronize_impl();
}
template <class T_Object>
size_t NativeAtomic<T_Object>::lock_test_and_set(size_t val) {
	return static_cast<T_Object *>(this)->lock_test_and_set_impl(val);
}
template <class T_Object>
void NativeAtomic<T_Object>::lock_release() {
	static_cast<T_Object *>(this)->lock_release_impl();
}
template <class T_Object>
volatile size_t *NativeAtomic<T_Object>::return_atom() {
	return static_cast<T_Object *>(this)->return_atom_impl();
}

}; // PAMI::Interface namespace
};     // PAMI namespace

#endif // __common_NativeAtomicsInterface_h__
