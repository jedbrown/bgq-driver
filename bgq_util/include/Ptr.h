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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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
/*********************************************************************/

#ifndef PTR_HEADER
#define PTR_HEADER

#include <cassert>
#include <iostream>
#include <iterator>

// The class Obj is used to implement smart pointers 
// with reference counting and works in conjunction with the
// Ptr class. Your class should inherit from Obj to
// include a copyCount field. Ptr has the constructors,
// destructors and assignment operators defined to increment
// or decrement the object copy count. Note that the objects
// automatically destroy themselves when the counts drops to
// zero.

class Obj {
public:
  Obj() : _copyCount(0) {}
  Obj(const Obj&) : _copyCount(0) {}
  virtual ~Obj() {}

  void increment() { 
    _copyCount++; 
  }
  void decrement() { 
    if (--_copyCount == 0) { 
      delete this; // objects commit suicide
    }
  }

  // mainly for debugging purposes
  unsigned int copyCount() const { return _copyCount; }

private:
  unsigned int _copyCount;
 
};

template <class T>
class Ptr {
//  friend std::ostream &operator<< <>(std::ostream &os, const Ptr<T>& ptr);
public:
  typedef T value_type;

  explicit Ptr(T* ptr = 0) : _ptr(ptr) { 
    if (_ptr) _ptr->increment(); 
  }

  Ptr(const Ptr& ptr) : _ptr(ptr.getAddress()) { 
    if (_ptr) _ptr->increment(); 
  }

  template <class U>
  Ptr(const Ptr<U>& ptr) : _ptr(ptr.getAddress()) { 
    if (_ptr) _ptr->increment(); 
  }

  virtual ~Ptr() { 
    if (_ptr) _ptr->decrement(); 
  }

  
  const Ptr& operator=(const Ptr& ptr) { 
    if (_ptr != ptr.getAddress()) { 
      if (_ptr) _ptr->decrement(); 
      if ((_ptr = ptr.getAddress())) _ptr->increment();
    } 
    return *this;
  } 
  
  
  template <class U>
  const Ptr& operator=(const Ptr<U>& ptr) {   
    if (_ptr != ptr.getAddress()) { 
      if (_ptr) _ptr->decrement(); 
      if ((_ptr = ptr.getAddress())) _ptr->increment();
    } 
    return *this;
  }

  const Ptr& operator=(T* ptr) { 
    if (_ptr) _ptr->decrement(); 
    if ((_ptr = ptr)) _ptr->increment(); 
    return *this; 
  }
  
  template <class U>
  const Ptr& operator=(U* ptr) { 
    if (_ptr) _ptr->decrement(); 
    if (_ptr = ptr) _ptr->increment(); 
    return *this;
  }

  operator T*() const { assert (_ptr != 0); return _ptr; }

  T* operator->() const { assert (_ptr != 0); return _ptr; }
  T& operator*() const { assert (_ptr != 0); return *_ptr; }
  int isNull() const { return (_ptr == 0); }

  T* getAddress() const { return _ptr; }

protected:
  T* _ptr;

};

template <class T>
inline std::ostream &operator<<(std::ostream &os, const Ptr<T>& ptr)
{
  if (ptr._ptr) {
    os << "[" << ptr._ptr << ", " << ptr._ptr->copyCount() << "]";
  } else {
    os << "[NULL]";
  }

  return os;
}

#endif
