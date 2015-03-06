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
/// \file components/memory/MemoryAllocator.h
/// \brief Common templatized memory allocator class
///
/// The class defined in this file uses C++ templates. C++ templates
/// require all source code to be #include'd from a header file.
///
#ifndef __components_memory_MemoryAllocator_h__
#define __components_memory_MemoryAllocator_h__

#include <vector>

#include "components/memory/MemoryManager.h"
#include "components/atomic/noop/Noop.h"

#include "util/trace.h"

#undef  DO_TRACE_ENTEREXIT
#define DO_TRACE_ENTEREXIT 0
#undef  DO_TRACE_DEBUG    
#define DO_TRACE_DEBUG     0

namespace PAMI
{
  /// \todo Update to use a memory manager template parameter.
  template <unsigned T_ObjSize, unsigned T_ObjAlign, unsigned T_PREALLOC=4, class T_Mutex = PAMI::Mutex::Noop>
  class MemoryAllocator
  {

   protected:

      typedef struct memory_object
      {
        uint8_t                object[T_ObjSize];
        struct memory_object * next;
	
        //Correct padding assuming T_Objsize is not a multiple of T_ObjAlign. 
        //T_ObjAlign - (objsize+sizeof(next))%T_ObjAlign
        //SSS: Using sizeof(int*) instead of a hard coded 8 to have the correct value for 32-bit
        uint8_t                pad[T_ObjAlign - ((T_ObjSize+sizeof(int*)) & (T_ObjAlign-1))];
      } memory_object_t;

  private:
    inline MemoryAllocator (const MemoryAllocator &m) 
    {
// Clang performs syntax checks even for non-intantianated templates.
// This assertion fail needs to be deferred from syntax-checking to a possible instantianation attempt.
      COMPILE_TIME_ASSERT(sizeof(T_Mutex) == 0);
      PAMI_abort();
    }
    inline MemoryAllocator& operator= (const MemoryAllocator &m) 
    {
// Clang performs syntax checks even for non-intantianated templates.
// This assertion fail needs to be deferred from syntax-checking to a possible instantianation attempt.
      COMPILE_TIME_ASSERT(sizeof(T_Mutex) == 0);
      PAMI_abort();
    }
  public:

      inline MemoryAllocator () :
        _mutex (),
        _head (NULL)
      {
        TRACE_FN_ENTER();
        TRACE_FORMAT("<%p> ", this);
        TRACE_FN_EXIT();
      }

	  ~MemoryAllocator()
      {
        while(!_segments.empty())
        {
          PAMI::Memory::MemoryManager::heap_mm->free(_segments.back());
          _segments.pop_back();
        }
      }
	  
	  
      void *internalAllocate () __attribute__((noinline, weak));
    
      inline void * allocateObject ()
      {
        TRACE_FN_ENTER();
        lock ();

        COMPILE_TIME_ASSERT(T_ObjSize > 1);//SSS: Leave this here to catch any compiler misbehavior

        memory_object_t * object = _head;
        if (object != NULL)
        {
          _head  = _head->next;
        }
        else
        {
          object = (memory_object_t*) internalAllocate();
        }

        unlock ();

        TRACE_FORMAT("<%p> _head %p, object %p", this, _head, object);
        TRACE_FN_EXIT();
		
        return (void *) object;
      };

      inline void returnObject (void * object)
      {
        TRACE_FN_ENTER();
        lock ();

        memory_object_t * tmp = (memory_object_t *) object;
        tmp->next = _head;
        _head = tmp;
        TRACE_FORMAT("<%p> _head %p, object %p", this, _head, object);

        unlock ();
        TRACE_FN_EXIT();
      };

      static const size_t objsize = T_ObjSize;

    protected:

      inline void lock ()
      {
        _mutex.acquire();
      }

      inline void unlock ()
      {
        _mutex.release();
      }

      T_Mutex          _mutex;

      memory_object_t *_head;
	  
      std::vector<void *>   _segments;
  };

  template <unsigned T_ObjSize, unsigned T_ObjAlign, unsigned T_PREALLOC, class T_Atomic>
    void *MemoryAllocator<T_ObjSize, T_ObjAlign, T_PREALLOC, T_Atomic>::internalAllocate () 
    {
      TRACE_FN_ENTER();
      memory_object_t *object; 

      // Allocate and construct a new set of objects
      unsigned i;
      pami_result_t rc;
      TRACE_FORMAT("<%p> malloc %6.6zu (%zu * %u) T_ObjSize %u",this,sizeof(memory_object_t) * T_PREALLOC,sizeof(memory_object_t),T_PREALLOC,T_ObjSize);
      rc = PAMI::Memory::MemoryManager::heap_mm->memalign((void **)&object, T_ObjAlign, sizeof(memory_object_t) * T_PREALLOC);
      PAMI_assert_alwaysf(rc==PAMI_SUCCESS, "alloc %zu bytes failed for context\n",sizeof(memory_object_t) * T_PREALLOC);
      // "return" the newly allocated objects to the pool of free objects.
      for (i=1; i<T_PREALLOC; i++) returnObject ((void *) &object[i]);
      TRACE_FN_EXIT();
      _segments.push_back((void *)object);
      return  (void *)object;
    }
  
};
#undef  DO_TRACE_ENTEREXIT
#undef  DO_TRACE_DEBUG    

#endif // __pami_components_memory_memoryallocator_h__
