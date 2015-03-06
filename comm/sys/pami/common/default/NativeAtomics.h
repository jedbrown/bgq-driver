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
 * \file common/default/NativeAtomics.h
 * \brief ???
 */

#ifndef __common_default_NativeAtomics_h__
#define __common_default_NativeAtomics_h__

#include "Platform.h"
#include "common/NativeAtomicsInterface.h"

#ifndef PAMI_NATIVE_ATOMIC_ALIGN
/// \brief Constant for aligning declarations the base atomic (internal only?)
#define PAMI_NATIVE_ATOMIC_ALIGN	sizeof(size_t)
#endif // ! PAMI_NATIVE_ATOMIC_ALIGN

namespace PAMI {
namespace Atomic {

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

class NativeAtomic : public PAMI::Interface::NativeAtomic<NativeAtomic> {
public:
	/// \brief Constant for aligning allocations of this class
	static const size_t ALIGNMENT = PAMI_NATIVE_ATOMIC_ALIGN;

	NativeAtomic() { }
	~NativeAtomic() { }

	inline size_t fetch_impl() {
		return _atom;
	}
	inline void set_impl(size_t val) {
		_atom = val;
	}

	inline size_t fetch_and_add_impl(size_t val) {
		return __sync_fetch_and_add(&_atom, val);
	}

	inline size_t fetch_and_sub_impl(size_t val) {
		return __sync_fetch_and_sub(&_atom, val);
	}
	inline size_t fetch_and_or_impl(size_t val) {
		return __sync_fetch_and_or(&_atom, val);
	}
	inline size_t fetch_and_and_impl(size_t val) {
		return __sync_fetch_and_and(&_atom, val);
	}
	inline size_t fetch_and_xor_impl(size_t val) {
		return __sync_fetch_and_xor(&_atom, val);
	}
	inline size_t fetch_and_nand_impl(size_t val) {
		return __sync_fetch_and_nand(&_atom, val);
	}

	inline size_t add_and_fetch_impl(size_t val) {
		return __sync_add_and_fetch(&_atom, val);
	}
	inline size_t sub_and_fetch_impl(size_t val) {
		return __sync_sub_and_fetch(&_atom, val);
	}
	inline size_t or_and_fetch_impl(size_t val) {
		return __sync_or_and_fetch(&_atom, val);
	}
	inline size_t and_and_fetch_impl(size_t val) {
		return __sync_and_and_fetch(&_atom, val);
	}
	inline size_t xor_and_fetch_impl(size_t val) {
		return __sync_xor_and_fetch(&_atom, val);
	}
	inline size_t nand_and_fetch_impl(size_t val) {
		return __sync_nand_and_fetch(&_atom, val);
	}

	inline size_t val_compare_and_swap_impl(size_t oldv, size_t newv) {
		return __sync_val_compare_and_swap(&_atom, oldv, newv);
	}
	inline bool bool_compare_and_swap_impl(size_t oldv, size_t newv) {
		return __sync_bool_compare_and_swap(&_atom, oldv, newv);
	}
	inline void synchronize_impl() {
		__sync_synchronize();
	}
	inline size_t lock_test_and_set_impl(size_t val) {
		return __sync_lock_test_and_set(&_atom, val);
	}
	inline void lock_release_impl() {
		__sync_lock_release(&_atom);
	}

	inline volatile size_t *return_atom_impl() {
		return &_atom;
	}

private:
	volatile size_t _atom __attribute__((__aligned__(PAMI_NATIVE_ATOMIC_ALIGN)));
}; // class NativeAtomic

}; // PAMI::Atomic namespace
};     // PAMI namespace

#endif // __common_default_NativeAtomics_h__
