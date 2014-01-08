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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits>
#include <map>
#include <memory>

#include "hwi/include/common/compiler_support.h"
#include "bgpm/include/bgpm.h"

using namespace std;

// This allocator exists to partition memory in user state, but
// allow map nodes to be allocated in a signal handler where
// malloc cannot be used.
// Note: objects assigned to one map cannot be shared with another list because
//       they are allocated by different allocator objects.
//       The allocator is not thread safe and each instance can only be used by one thread.
// Also, the 1st instance of this object must remain for the lifetime of any copies
// (copies use same buffer and reference the same # of elements used)
//
// WARNING:  I don't know if the copy constructors are correct. Looking at the map it
// seems like the MyAddrAllocator<U> is used temporarily (created and destroyed), so it
// doesn't seem to make sense to copy the data.  But, if anything got added to the list
// on the copied type - it might mess up the original space.
template<typename T, const size_t MAX_SIZE>
class MapAddrAllocator
{
public:
    void *dbuffer;
    int   used;
    bool  orig;

public:
    // type definitions
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;

    // rebind allocator to type U
    template <class U>
    struct rebind {
       typedef MapAddrAllocator<U, MAX_SIZE> other;
    };

    // return address of values
    pointer address (reference value) const {
       return &value;
    }
    const_pointer address (const_reference value) const {
       return &value;
    }


    // constructors and destructor
    MapAddrAllocator() throw()
    : dbuffer(NULL), used(0), orig(true)
    {
        dbuffer = malloc(MAX_SIZE * sizeof(T));
        //fprintf(stderr, _AT_ " MapAddrAllocator Tsize=%ld, dbuffer=%p\n", sizeof(T), dbuffer);
        assert(dbuffer);
    }

    ~MapAddrAllocator() throw()
    {
        //fprintf(stderr, _AT_ " ~MapAddrAllocator dbuffer=%p, orig=%d\n", dbuffer, orig);
        if (orig) {
            free(dbuffer);
            dbuffer = NULL;
        }
    }


    MapAddrAllocator(const MapAddrAllocator&s) throw()
    : dbuffer(s.dbuffer), used(s.used), orig(false)
    {
        //dbuffer = malloc(MAX_SIZE * sizeof(U));
        //fprintf(stderr, _AT_ " MapAddrAllocator(copy) Tsize=%ld, dbuffer=%p, src.dbuffer=%p\n", sizeof(T), dbuffer, s.dbuffer);
        //assert(dbuffer);
    }

    template <class U>
    MapAddrAllocator (const MapAddrAllocator<U, MAX_SIZE>&s) throw()
    : dbuffer(s.dbuffer), used(s.used), orig(false)
    {
        //dbuffer = malloc(MAX_SIZE * sizeof(U));
        //fprintf(stderr, _AT_ " MapAddrAllocator(copy) Usize=%ld, dbuffer=%p, src.dbuffer=%p\n", sizeof(U), dbuffer, s.dbuffer);
        //assert(dbuffer);
    }


private:
    // Disable these

    void operator=(const MapAddrAllocator&) throw();


public:

    // return maximum number of elements that can be allocated
    size_type max_size() const throw()
    {
        return MAX_SIZE;
    }

    // allocate but don't initialize num elements of type T
    pointer allocate (size_type num, const void* = 0)
    {
        if (used >= (int)MAX_SIZE) {
            throw std::bad_alloc();
        }
        pointer ptr = reinterpret_cast<pointer>(&((T*)dbuffer)[used]);
        used++;
        //fprintf(stderr, _AT_ " allocate %ld elements of size %ld; totalsize=%ld. Allocated %p\n",
        //        num, sizeof(T), num*sizeof(T), ptr);
        return ptr;
    }

    // initialize elements of allocated storage p with value value
    void construct (pointer p, const T& value) {
        new((void*)p)T(value);
    }

    // destroy elements of initialized storage p
    void destroy (pointer p) {
        p->~T();
    }

    // deallocate storage p of deleted elements
    // We will never deallocate.
    void deallocate (pointer p, size_type num) {
        //fprintf(stderr, _AT_ " deallocate %p of size %ld\n", p, sizeof(num));
        //used--;
        //::operator delete((void*)p);
    }

};

// return that all specializations of this allocator are interchangeable
template <class T, const size_t MAX_SIZE>
bool operator== (const MapAddrAllocator<T, MAX_SIZE>&l,
                const MapAddrAllocator<T, MAX_SIZE>&r) throw() {
    return false;
}
template <class T, const size_t MAX_SIZE>
bool operator!= (const MapAddrAllocator<T, MAX_SIZE>&l,
                const MapAddrAllocator<T, MAX_SIZE>&r) throw() {
    return true;
}

