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
 * \file components/memory/heap/HeapMemoryManager.h
 * \brief ???
 */

#ifndef __components_memory_heap_HeapMemoryManager_h__
#define __components_memory_heap_HeapMemoryManager_h__

#include <cstdlib>

#include "components/memory/MemoryManager.h"
#include "components/memory/shmem/SharedMemoryManager.h"

namespace PAMI {
namespace Memory {

class HeapMemoryManager : public MemoryManager {
private:
	inline void MM_RESET_STATS() {
		_num_allocs = 0;
		_num_frees = 0;
		_total_bytes = 0;
		_freed_bytes = 0;
	}
	inline void MM_DUMP_STATS() {
		fprintf(stderr, "%s: "
			"%zd allocs, %zd frees, total %zd, freed %zd\n",
			getName(), _num_allocs, _num_frees,
			_total_bytes, _freed_bytes);
	}
public:

	inline HeapMemoryManager() :
	MemoryManager() {
		COMPILE_TIME_ASSERT(sizeof(HeapMemoryManager) <= sizeof(SharedMemoryManager));
		_attrs = PAMI_MM_PROCSCOPE;
#ifdef MM_DEBUG // set/unset in MemoryManager.h
		_debug = setup_debug();
		MM_RESET_STATS();
		// since currently this can't track number of bytes freed,
		// don't bother trying to track current/max bytes.
#else // !MM_DEBUG
		if (__global.mapping.task() == 0 && getenv("PAMI_MM_DEBUG")) {
			fprintf(stderr, "WARNING: env PAMI_MM_DEBUG is set "
				"but MM_DEBUG support was not compiled-in\n");
		}
#endif // !MM_DEBUG
	}

	~HeapMemoryManager() {
		// this is only called from process exit,
		// so no need to actually free - the memory
		// is all being reclaimed by the OS.
#ifdef MM_DEBUG
		if (_debug) {
			MM_DUMP_STATS();
			// if (_debug > 1) _meta.dump(getName()); // no meta data to dump
		}
#endif // MM_DEBUG
	}

	inline const char *getName() { return "/HeapMemoryManager"; }

	inline pami_result_t init(MemoryManager *mm, size_t bytes,
				  size_t alignment, size_t new_align,
				  unsigned attrs = 0, const char *key = NULL,
				  MM_INIT_FN *init_fn = NULL, void *cookie = NULL) {
                //unused warnings with -pedantic
                (void)mm;(void)bytes;(void)alignment;(void)new_align;
                (void)attrs;(void)key;(void)init_fn;(void)cookie;

		PAMI_abortf("HeapMemoryManager cannot be init()");
		return PAMI_ERROR;
	}

	inline pami_result_t init2(MemoryManager *mm, void *buf, size_t bytes,
				  const char *key = NULL,
				  size_t new_align = 1,
				  unsigned attrs = 0) {
                (void)mm;(void)bytes;(void)new_align;
                (void)attrs;(void)key;(void)buf;
                PAMI_abortf("HeapMemoryManager cannot be init()");
		return PAMI_ERROR;
	}

	inline pami_result_t reset(bool force = false) {
                (void)force;
#ifdef MM_DEBUG
		if (_debug) {
			MM_DUMP_STATS();
			MM_RESET_STATS();
		}
#endif // MM_DEBUG
		// can't actually release any memory... yet.
		return PAMI_SUCCESS;
	}

	inline pami_result_t memalign(void **memptr, size_t alignment, size_t bytes,
				      const char *key = NULL,
				      MM_INIT_FN *init_fn = NULL, void *cookie = NULL) {
		if (alignment < _alignment) { alignment = _alignment; }
#ifdef USE_MEMALIGN
		int rc = posix_memalign(memptr, alignment, bytes);
		if (rc != 0) {
#ifdef MM_DEBUG
			if (_debug) {
				dump("ENOMEM");
			}
#endif // MM_DEBUG
			return PAMI_ERROR;
		}
#else
		void *ptr = malloc(bytes + alignment);
		if (!ptr) { return PAMI_ERROR; }
		*memptr = (void *)(((uintptr_t)ptr + alignment - 1) &
				   ~(alignment - 1));
		// can't free this! users ptr is not the one malloc knows about...
#endif
		memset(*memptr, 0, bytes);	// needed for 1 proc/node, when
		// simulating shared_mm...
		if (init_fn) {
			init_fn(*memptr, bytes, key, _attrs, cookie);
		}
#ifdef MM_DEBUG
		if (_debug) {
			++_num_allocs;
			_total_bytes += bytes;
		}
#endif // MM_DEBUG
		return PAMI_SUCCESS;
	}


	inline void free(void *mem) {
#ifdef MM_DEBUG
		if (_debug) {
			++_num_frees;
			// _freed_bytes += bytes;
		}
#endif // MM_DEBUG
#ifdef USE_MEMALIGN
		std::free(mem);
#endif
	}

	inline size_t available(size_t alignment) {
                (void)alignment;
		// how to tell how much is available???
		return (size_t)-1;
	}

	inline void dump(const char *str) {
		if (str) {
			fprintf(stderr, "%s: %s %x\n", str, getName(), _attrs);
		} else {
			fprintf(stderr, "%s %x\n", getName(), _attrs);
		}
#ifdef MM_DEBUG
		if (_debug) {
			MM_DUMP_STATS();
		}
#endif // MM_DEBUG
	}

protected:
#ifdef MM_DEBUG
	long _debug;
	size_t _num_allocs;
	size_t _num_frees;
	size_t _total_bytes;
	size_t _freed_bytes;
#endif // MM_DEBUG
}; // class HeapMemoryManager

}; // namespace Memory
}; // namespace PAMI

#endif // __pami_components_memory_heap_heapmemorymanager_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments --max-instatement-indent=79
// astyle options --min-conditional-indent=2 --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements --align-pointer=name --lineend=linux
//
