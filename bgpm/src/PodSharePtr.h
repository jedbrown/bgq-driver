/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef _BGPM_PODSHAREDPTR_H_  // Prevent multiple inclusion
#define _BGPM_PODSHAREDPTR_H_

#include "Debug.h"

namespace bgpm {



//! PodSharedPtr Template
/*!
  Create a shared pointer to keep reference counts for objects shared by
  multiple process threads. The referenced object will be deleted when
  all SharedPtrs are destroyed.

  Note: Locking is tracked in pointed to object.  Thus, assumption that
  when freeing object, all accesses are freeing and won't be race condition
  between gaining access and freeing pointed to object.

  However, make the shared pointer a POD type (pretty ordinary data) type, so
  that it may be used as thread local data.

  Some attributes:
  - This class only tracks the object lifetime.  It does not lock against access or update.
  - This class relies on "Intrusive" reference counting. That is, the target object requires
    three thread safe mutable methods:
    - void AddRef();  - add reference
    - bool RmvRef();  - remove reference - which returns true if this is the last use of the object.
                      - when true is returned, the SharedPtr will delete the pointed to object.

*/
template <class T>
class PodSharedPtr
{
public:

    // Initialize Pointer
    // As a POD object - the pointer is created, but not initialized.
    // Some explicit initialization is required before 1st use.
    void Init() {
        BGPM_EXEMPT_L2;
        ptr = NULL;
        mbar();
    }

    PodSharedPtr & operator=(T *pObj) {
        BGPM_EXEMPT_L2;
        if (pObj != ptr) {
            RemoveCurrent();
            ptr=pObj;
            if (ptr) ptr->AddRef();
            mbar();
        }
        return(*this);
    }

    // Release access
    void Release() {
        BGPM_EXEMPT_L2;
        RemoveCurrent();
        ptr = NULL;
        mbar();
    }

    // assign from another sharedptr
    PodSharedPtr & Assign(const PodSharedPtr & src) {
        BGPM_EXEMPT_L2;
        if (src.ptr != ptr) {
            RemoveCurrent();
            ptr=src.ptr;
            if (ptr) ptr->AddRef();
            mbar();
        }
        return(*this);
    }

    // normal pointer operations
    T & operator*() const { return *ptr; }
    T * operator->() const { return ptr; }

    // pointer Comparisons
    inline friend bool operator==(const PodSharedPtr & lhs, const T *rhs) { return(lhs.ptr == rhs); }
    inline friend bool operator!=(const PodSharedPtr & lhs, const T *rhs) { return(lhs.ptr != rhs); }
    inline friend bool operator==(const T *lhs, const PodSharedPtr & rhs) { return(rhs.ptr == lhs); }
    inline friend bool operator!=(const T *lhs, const PodSharedPtr & rhs) { return(rhs.ptr != lhs); }

    //  derived class c++ ambiguity busters
    template <class U> inline friend bool operator==(const PodSharedPtr & lhs, const U *rhs) { return(lhs.ptr == rhs); }
    template <class U> inline friend bool operator!=(const PodSharedPtr & lhs, const U *rhs) { return(lhs.ptr != rhs); }
    template <class U> inline friend bool operator==(const U *lhs, const PodSharedPtr & rhs) { return(rhs.ptr == lhs); }
    template <class U> inline friend bool operator!=(const U *lhs, const PodSharedPtr & rhs) { return(rhs.ptr != lhs); }

    // different raw ptr type ambiguity busters
    template <class U> inline bool operator==(const PodSharedPtr<U> & rhs) { return(ptr == rhs.ptr); }
    template <class U> inline bool operator!=(const PodSharedPtr<U> & rhs) { return(ptr != rhs.ptr); }


// private:  everything must remain public

    T  *ptr;

    void RemoveCurrent() {
        BGPM_EXEMPT_L2;
        if (ptr) {
            if (ptr->RmvRef()) {
                delete ptr;
                ptr = NULL;
            }
        }
    }

};



}

#endif
