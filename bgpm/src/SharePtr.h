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

#ifndef _BGPM_SHAREDPTR_H_  // Prevent multiple inclusion
#define _BGPM_SHAREDPTR_H_

namespace bgpm {



//! SharedPtr Template
/*!
  Create a pointer to keep reference counts for objects shared by
  multiple process threads. The referenced object will be deleted when
  all SharedPtrs are destroyed.

  Note: The pointer itself cannot be shared between threads, it's that the object pointed to is
  shared between threads.

  Some attributes:
  - This class only tracks the object lifetime.  It does not lock against access or update.
  - This class relies on "Intrusive" reference counting. That is, the target object requires
    two required thread safe mutable methods:
    - void AddRef();  - add reference
    - bool RmvRef();  - remove reference - which returns true if this is the last use of the object.
                      - when true is returned, the SharedPtr will delete the pointed to object.

*/
template <class T>
class SharedPtr
{
public:
    //! construct/assign from raw pointer
    //! We assume that constructed from a raw pointer, there is no other pointer existing to the
    //! same source object - that is this is the 1st pointer to the source object and all later
    //! references will be smart pointers.
    explicit SharedPtr(T *pObj) : ptr(pObj) {
        if (ptr) ptr->AddRef();
    }
    SharedPtr & operator=(T *pObj) {
        if (pObj != ptr) {
            RemoveCurrent();
            ptr=pObj;
            if (ptr) ptr->AddRef();
            mbar();
        }
        return(*this);
    }

    // construct/assign from another sharedptr
    SharedPtr() : ptr(NULL) {};
    ~SharedPtr() { RemoveCurrent(); }
    explicit SharedPtr(const SharedPtr & src) {
        ptr=src.ptr;
        if (ptr) ptr->AddRef();
        mbar();
    }
    SharedPtr & operator=(const SharedPtr & src) {
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
    inline friend bool operator==(const SharedPtr & lhs, const T *rhs) { return(lhs.ptr == rhs); }
    inline friend bool operator!=(const SharedPtr & lhs, const T *rhs) { return(lhs.ptr != rhs); }
    inline friend bool operator==(const T *lhs, const SharedPtr & rhs) { return(rhs.ptr == lhs); }
    inline friend bool operator!=(const T *lhs, const SharedPtr & rhs) { return(rhs.ptr != lhs); }

    //  derived class c++ ambiguity busters
    template <class U> inline friend bool operator==(const SharedPtr & lhs, const U *rhs) { return(lhs.ptr == rhs); }
    template <class U> inline friend bool operator!=(const SharedPtr & lhs, const U *rhs) { return(lhs.ptr != rhs); }
    template <class U> inline friend bool operator==(const U *lhs, const SharedPtr & rhs) { return(rhs.ptr == lhs); }
    template <class U> inline friend bool operator!=(const U *lhs, const SharedPtr & rhs) { return(rhs.ptr != lhs); }

    // different raw ptr type ambiguity busters
    template <class U> inline bool operator==(const SharedPtr<U> & rhs) { return(ptr == rhs.ptr); }
    template <class U> inline bool operator!=(const SharedPtr<U> & rhs) { return(ptr != rhs.ptr); }


private:
    T  *ptr;

    void RemoveCurrent() {
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
