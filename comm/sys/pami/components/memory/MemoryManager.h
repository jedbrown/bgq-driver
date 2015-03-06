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
/// \file components/memory/MemoryManager.h
/// \brief Base memory manager class
///
#ifndef __components_memory_MemoryManager_h__
#define __components_memory_MemoryManager_h__

#include <pami.h>
#include "util/common.h"
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fnmatch.h>
#include "NativeAtomics.h"
#include "Memory.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

#define MM_DEBUG

// need a "name" for each MemoryManager, in order to debug effectively.
// But HeapMemoryManager and SharedMemoryManager debug is still useful.

////////////////////////////////////////////////////////////////////////////////
/// \env{pami,PAMI_MM_DEBUG}
/// Enable collection and display of MemoryManager
/// statistics. Statistics and allocations are dumped on exit
/// (when MemoryManager destructor(s) are called). Some allocation
/// failures also result in a dump.
///
/// - "1"     - dump statistics of all MemoryManagers.
/// - "2"     - dump statistics and (remaining) allocations.
/// - [level:]pattern - (see fnmatch(3)) dump MemoryManagers
///           that match pattern (by name, i.e. key used to init()),
///           occording to level as previously described.
///           Default level is "1".
///
/// \todo add more debug levels to pattern syntax
////////////////////////////////////////////////////////////////////////////////


namespace PAMI {
namespace Memory {

	static const unsigned int PAMI_MM_PROCSCOPE = 1;
	static const unsigned int PAMI_MM_NODESCOPE = 2;
	static const unsigned int PAMI_MM_SCOPE = (PAMI_MM_PROCSCOPE | PAMI_MM_NODESCOPE);
	// how do platforms add more?
	static const unsigned int PAMI_MM_L2ATOMIC  = 4; // BGQ-specific
	static const unsigned int PAMI_MM_WACREGION = 8; // BGQ-specific

	static const unsigned int MMKEYSIZE = 128;
	typedef void MM_INIT_FN(void *mem, size_t bytes, const char *key, unsigned attrs, void *cookie);

// internal class, templatized to allow varied use
class MemoryManager {
public:

	// is this neded? do we always ensure memory is zero?
	static void memzero(void *mem, size_t bytes, const char *key,
					unsigned attrs, void *cookie) {
          (void)key;(void)attrs;(void)cookie;
		memset(mem, 0, bytes);
	}
protected:
	/// \brief compute the padding needed to provide user with aligned addr
	///
	/// \param[in] base	Start address of raw chunk
	/// \param[in] off	Addition (meta) data needed
	/// \param[in] align	Alignment required (power of two - NOT zero!)
	///
	static inline size_t getPadding(void *base, size_t off, size_t align) {
		return (((size_t)base + off + (align - 1)) & ~(align - 1)) -
		       (size_t)base;
	}

	class MemoryManagerSync {
	public:
		static const size_t ALIGNMENT = PAMI::Atomic::NativeAtomic::ALIGNMENT;
		MemoryManagerSync() { }

		inline size_t addRef() {
			return _ref_count.fetch_and_add(1);
		}
		inline size_t rmRef() {
			return _ref_count.fetch_and_sub(1);
		}
		inline size_t refCount() {
			return _ref_count.fetch();
		}
		inline void initDone() { _init_done = 1; }

		// must be called without lock!
		inline void waitDone() {
			while (_init_done == 0) { }
		}
	private:
		PAMI::Atomic::NativeAtomic _ref_count;
		volatile size_t _init_done;
	}; // class MemoryManagerSync

	class MemoryManagerKey {
	public:
		MemoryManagerKey() { }

		inline void key(const char *key) {
			if (key) {
				strncpy((char *)_key, key, MMKEYSIZE);
			} else {
				// whatever this does, _key[0] must not be 0...
				snprintf((char *)_key, MMKEYSIZE, "/pami-%d-%p",
					 getpid(), this);
			}
			// assert(!isFree());
		}
		inline char *key() { return (char *)_key; }

		inline bool isMatch(const char *key) {
			return (strncmp((char *)_key, key, MMKEYSIZE) == 0);
		}
		inline void freeKey() { _key[0] = '\0'; }
		inline bool isFree() { return (_key[0] == '\0'); }
	private:
		volatile char _key[MMKEYSIZE];
	}; // class MemoryManagerKey

	/// \brief Class to hold the meta-data portion of an allocation that is shared
	///
	/// Anonymous (private) chunks do not use this object
	///
	/// Can't use util/queue/*Queue.h since they all depend on MemoryManager.
	///
	class MemoryManagerAlloc :	public MemoryManagerSync,
					public MemoryManagerKey {
	public:
		static const size_t ALIGNMENT = MemoryManagerSync::ALIGNMENT;
		MemoryManagerAlloc() { }
		virtual ~MemoryManagerAlloc() { }

		inline size_t offset() { return _offset; }
		inline void *userMem() { return (void *)-1; }
		inline size_t userSize() { return _userSize; }
		inline size_t rawSize() { return _rawSize; }

		inline void setMem(void *mem, size_t len, size_t aln) {
                  (void)mem;(void)len;(void)aln;
                }
		inline void setMem(size_t off, size_t len, size_t pad) {
			_userSize = len;
			_offset = off;
			_rawSize = len + pad;
		}

		// returns PAMI_SUCCESS if last reference gone, otherwise
		// PAMI_EAGAIN (potentially, PAMI_ERROR but not currently used).
		// caller holds lock...
		inline pami_result_t free(void (*func)(MemoryManagerAlloc *m) = NULL) {
			// this destroys VFT? Can't do this anyway.
			// memset(this, 0, sizeof(*this));

			pami_result_t rc = PAMI_EAGAIN; // i.e. EBUSY
			// Meta data in shared memory, or same memory as allocation...
			if (rmRef() == 1) {
				if (func) { func(this); }
				freeKey();
				rc = PAMI_SUCCESS;
			}
			return rc;
		}
	private:
		volatile size_t _offset; // _base + _offset = user memory chunk
		volatile size_t _userSize;
		volatile size_t _rawSize;
	}; // class MemoryManagerAlloc

	class MemoryManagerOSShmAlloc : public MemoryManagerKey {
	public:
		static const size_t ALIGNMENT = sizeof(void *);
		MemoryManagerOSShmAlloc() { }
		virtual ~MemoryManagerOSShmAlloc() { }

		inline size_t offset() { return (size_t)-1; }
		inline void *userMem() { return (char *)_mem + _pad; }
		inline size_t userSize() { return _userSize; }
		inline void *rawMem() { return _mem; }
		inline size_t rawSize() { return _size; }

		inline void setMem(size_t off, size_t len, size_t pad) {
                  (void)off;(void)len;(void)pad;
                }
		inline void setMem(void *mem, size_t len, size_t aln) {
			_mem = mem;
			_size = maxSize(len, aln); // assumed "real" length of 'mem'...
			// (caller should have used maxSize() too)
			_pad = getPadding(mem, sizeof(MemoryManagerSync), aln);
			_userSize = len;
		}

		// returns PAMI_SUCCESS if last reference gone, otherwise
		// PAMI_EAGAIN (potentially, PAMI_ERROR but not currently used).
		// caller holds lock...
		inline pami_result_t free(void (*func)(MemoryManagerOSShmAlloc *m) = NULL) {
			pami_result_t rc = PAMI_EAGAIN; // i.e. EBUSY
			// Meta data in private memory, but allocation is shared...
			if (sync() && rmRef() == 1) { // last one out...
				if (func) { func(this); }
				// zero memory so that next use is zeroed?
				// memset(rawAddress(), 0, size());
				rc = PAMI_SUCCESS;
			}
			freeKey();
			return rc;
		}

		static size_t maxSize(size_t size, size_t align) {
			return sizeof(MemoryManagerSync) + align + size;
		}

		inline void initDone() { sync()->initDone(); }
		inline void waitDone() { sync()->waitDone(); }
		inline size_t addRef() { return sync()->addRef(); }
		inline size_t rmRef() { return sync()->rmRef(); }
	private:
		inline MemoryManagerSync *sync() { return (MemoryManagerSync *)_mem; }
		void *_mem;
		size_t _size;
		size_t _userSize;
		size_t _pad;
	}; // class MemoryManagerOSShmAlloc

	/// \brief Class to contain the root header of a MemoryManager
	///
	/// Can't use util/queue/*Queue.h or components/atomic/*... since they all
	/// depend on MemoryManager.
	///
	class MemoryManagerHeader {
	public:
		static const size_t ALIGNMENT = PAMI::Atomic::NativeAtomic::ALIGNMENT;
		MemoryManagerHeader() :
		_mutex(),
		_offset(0),
		_nmetas(0) {
		}
		virtual ~MemoryManagerHeader() { }

		// caller holds lock!
		inline size_t offset() { return _offset; }
		inline size_t offset(size_t off) {
			size_t o = _offset;
			_offset = off;
			return o;
		}

		inline void acquire() {
			while (_mutex.fetch_and_add(1) != 0) { ; }
		}

		inline void release() {
			Memory::sync();
			_mutex.set(0);
		}

		inline size_t numMetas() { return _nmetas; }
		inline void setMeta(size_t x) { if (x >= _nmetas) { _nmetas = x + 1; } }
	private:
		PAMI::Atomic::NativeAtomic _mutex;
		volatile size_t _offset; // simple free-space handling
		volatile size_t _nmetas; // number of _metas[] allocated globally
	}; // class MemoryManagerHeader

// meta[0] has 8, meta[1] has 32, meta[2] has 128, etc...
#define MM_META_NUM(idx)	((8UL << idx) << idx)
#define MMMAX_N_META	4

	template <class T_MMAlloc = MemoryManagerAlloc>
	class MemoryManagerMeta {
	private:

		inline void __mark(size_t z) {
			_my_metas[z / (sizeof(*_my_metas) * 8)] |=
				((size_t)1 << (z % (sizeof(*_my_metas) * 8)));
		}

		inline void __unMark(size_t z) {
			_my_metas[z / (sizeof(*_my_metas) * 8)] &=
				~((size_t)1 << (z % (sizeof(*_my_metas) * 8)));
		}

		inline bool __isMarked(size_t z) {
			return (_my_metas[z / (sizeof(*_my_metas) * 8)] &
				((size_t)1 << (z % (sizeof(*_my_metas) * 8)))) != 0;
		}

		/// Caller declares size_t x = 0, y = 0, z = 0;
		/// before starting iteration. Return value of NULL is end.
		///
		template <bool fill, bool want>
		inline T_MMAlloc *__locateOne(size_t &x, size_t &y, size_t &z) {
			size_t max = (want ? MMMAX_N_META : _metahdr->numMetas());
			for (; x < max; ++x, y = 0) {
				if (fill && _getMeta(x) != PAMI_SUCCESS) { return NULL; }
				if (!fill && _metas[x] == NULL) { return NULL; }
				for (; y < MM_META_NUM(x); ++y, ++z) {
					if (want && _metas[x][y].isFree()) {
						return &_metas[x][y];
					}
					if (!want && !_metas[x][y].isFree()) {
						return &_metas[x][y];
					}
				}
			}
			return NULL;
		}
		// Caller of __locateOne() must call this between iterations.
		inline void __locateSkip(size_t &x, size_t &y, size_t &z) {
			++y;
			++z;
		}

		inline pami_result_t _metaAlloc(void **pptr, size_t align, size_t len, char tag) {
			pami_result_t rc;
			if (_meta_key_len) {
				_meta_key_fmt[_meta_key_len] = tag; // replaced on each use
				rc = _meta_mm->memalign(pptr, align, len, _meta_key_fmt);
			} else {
				rc = _meta_mm->memalign(pptr, align, len, NULL);
			}
			// PAMI_assertf(rc == PAMI_SUCCESS, "Failed to get memory for meta \"%s\" from \"%s\"", _meta_key_fmt, _meta_mm->getName());
			return rc;
		}

		inline pami_result_t _getMeta(size_t x) {
			pami_result_t rc = PAMI_SUCCESS;
			if (_metas[x] == NULL) {
				rc = _metaAlloc((void **)&_metas[x], _metas[x]->ALIGNMENT,
						MM_META_NUM(x) * sizeof(*_metas[x]),
						"0123456789"[x]);
				if (rc == PAMI_SUCCESS) { _metahdr->setMeta(x); }
			}
			return rc;
		}

		inline pami_result_t _freeAll() {
			if (!_metahdr) { return PAMI_SUCCESS; }
			size_t x = 0, y = 0, z = 0;
			size_t nf = 0; // "Not Freed"...
			T_MMAlloc *m;
			while ((m = __locateOne<false, false>(x, y, z))) {
				if (!__isMarked(z) || free(m) != PAMI_SUCCESS) {
					++nf;
				}
				__locateSkip(x, y, z);
			}
			if (nf == 0) { // got them all...
				_metahdr->offset(0);
			}
			return PAMI_SUCCESS;
		}
	public:
		static  size_t MAX_NUM_META() {
			size_t x;
			size_t y = 0;
			for (x = 0; x < MMMAX_N_META; ++x) {
				y += MM_META_NUM(x);
			}
			return y;
		}

		MemoryManagerMeta() :
		_meta_mm(NULL),
		_metahdr(NULL),
		_meta_key_fmt(),
		_meta_key_len(0),
		_my_metas(NULL),
		_last_z(0) {
			memset(_metas, 0, sizeof(_metas));
		}

		virtual ~MemoryManagerMeta() {
			if (!_metahdr) { return; }
			acquire(); // no one else should be here...
			_freeAll();
			release(); // no one else should be here...
			if (_meta_mm->base()) { // still there?
				if (_metahdr) {
					_meta_mm->free(_metahdr); // destroys mutex
					_metahdr = NULL;
				}
				int x;
				for (x = 0; x < MMMAX_N_META; ++x) {
					if (_metas[x]) {
						if (!_pre_alloc) { _meta_mm->free(_metas[x]); }
						_metas[x] = NULL;
					}
				}
			}
		}

		inline pami_result_t reset() {
			acquire();
			pami_result_t rc = _freeAll();
			if (rc == PAMI_SUCCESS && _metahdr->offset() != 0) {
				rc = PAMI_ERROR;
			}
			release();
			return rc;
		}

		inline void dump(const char *str) {
			size_t x = 0, y = 0, z = 0;
			T_MMAlloc *m;
			if (!_metahdr) { return; }
			while ((m = __locateOne<true, false>(x, y, z))) {
				// dump "my" flag too?
				fprintf(stderr, "%s: _metas[%zd][%zd]: "
					"\"%s\" %p %zd (%zd/%zd)\n",
					str, x, y,
					m->key(),
					m->userMem(),
					m->offset(),
					m->userSize(),
					m->rawSize()
				       );
				__locateSkip(x, y, z);
			}
		}

		inline void init(MemoryManager *mm, const char *key) {
			_meta_mm = mm;
			_pre_alloc = (mm == shared_mm); // minimize small allocs
			if (key) {
				_meta_key_len = strlen(key);
				strncpy(_meta_key_fmt, key, MMKEYSIZE);
				if (_meta_key_len + 3 >= MMKEYSIZE) {
					_meta_key_len = MMKEYSIZE - 3;
					// hope for the best...
				}
				_meta_key_fmt[_meta_key_len++] = '-';
				_meta_key_fmt[_meta_key_len + 1] = '\0';
			}
			pami_result_t rc;
			if (_pre_alloc) {
				rc = _metaAlloc((void **)&_metahdr, _metahdr->ALIGNMENT,
						sizeof(*_metahdr) +
						MAX_NUM_META() * sizeof(*_metas[0]),
						'a');
				PAMI_assertf(rc == PAMI_SUCCESS,
					     "Failed to get memory for meta data");
				int x;
				T_MMAlloc *mp = (T_MMAlloc *)(_metahdr + 1);
				for (x = 0; x < MMMAX_N_META; ++x) {
					_metas[x] = mp;
					_metahdr->setMeta(x);
					mp += MM_META_NUM(x);
				}
			} else {
				rc = _metaAlloc((void **)&_metahdr, _metahdr->ALIGNMENT, sizeof(*_metahdr), 'h');
				PAMI_assertf(rc == PAMI_SUCCESS,
					     "Failed to get memory for _metahdr");
			}
			// one bit per entry in _metas[*]...
			rc = heap_mm->memalign((void **)&_my_metas, sizeof(*_my_metas),
					       MAX_NUM_META() / 8);
			PAMI_assertf(rc == PAMI_SUCCESS,
				     "Failed to get memory for _my_metas");
			memset(_my_metas, 0, MAX_NUM_META() / 8);
			//new (_metahdr) MemoryManagerHeader(); // can? should?
		}

		inline void acquire() { _metahdr->acquire(); }
		inline void release() { _metahdr->release(); }

		inline size_t spaceUsed() {
			return _metahdr->offset();
		}

		/// \brief Prepare to make an allocation
		///
		/// Setup numbers to reflect requested allocation.
		///
		///    offset ---+
		///              v
		/// +------------+------------------------+
		/// | padding    |  (user allocation)     |
		/// +------------+------------------------+
		///              |<-------- bytes ------->|
		///
		/// Note: these numbers may not represent available memory.
		/// Caller verifies available space then calls allocSpace().
		///
		///  \param[in] base		Base of mm (for setting alignment)
		///  \param[in] alignment	User alignment request
		///  \param[in] bytes		User alloc request size
		///  \param[out] offset		Offset of user allocation
		///  \param[out] padding	Padding of 'offset'
		///
		inline void getSpace(void *base, size_t alignment, size_t bytes,
				     size_t &offset, size_t &padding) {
			// fancier freespace handling TBD...
			size_t padded = getPadding(base, _metahdr->offset(), alignment);
			padding = padded - _metahdr->offset();
			offset = padded;
		}
		/// \brief Commit an allocation
		///
		///  \param[in] offset	From getSpace()
		///  \param[in] bytes	User requested allocation size
		///  \param[in] padding	From getSpace()
		///
		inline size_t allocSpace(size_t offset, size_t bytes, size_t padding) {
			// fancier freespace handling TBD...
			return _metahdr->offset(offset + bytes);
		}

		// Find existing, possibly from other thread (requires filling
		// of all space).
		// caller holds lock
		inline T_MMAlloc *find(const char *key) {
			size_t x = 0, y = 0, z = 0;
			T_MMAlloc *m;
			while ((m = __locateOne<true, false>(x, y, z))) {
				if (m->isMatch(key)) {
					_last_z = z;
					m->addRef();
					__mark(z);
					return m;
				}
				__locateSkip(x, y, z);
			}
			return NULL;
		}

		// only for MemoryManager, T_MMAlloc == MemoryManagerAlloc
		// caller holds lock
		inline T_MMAlloc *find(size_t off) {
			size_t x = 0, y = 0, z = 0;
			T_MMAlloc *m;
			while ((m = __locateOne<false, false>(x, y, z))) {
				if (_metas[x][y].offset() == off) {
					_last_z = z;
					return m;
				}
				__locateSkip(x, y, z);
			}
			return NULL;
		}

		// only for SharedMemoryManager, T_MMAlloc == MemoryManagerOSShmAlloc
		// caller holds lock
		inline T_MMAlloc *find(void *mem) {
			size_t x = 0, y = 0, z = 0;
			T_MMAlloc *m;
			while ((m = __locateOne<false, false>(x, y, z))) {
				if (_metas[x][y].userMem() == mem) {
					_last_z = z;
					return m;
				}
				__locateSkip(x, y, z);
			}
			return NULL;
		}

		// allocates more space if needed/possible.
		// caller holds lock
		inline T_MMAlloc *findFree(void *mem, size_t len, size_t aln,
					   const char *key) {
			size_t x = 0, y = 0, z = 0;
			T_MMAlloc *m = __locateOne<true, true>(x, y, z);
			if (m) {
				m->setMem(mem, len, aln);
				m->key(key);
				m->addRef();
				__mark(z);
			}
			return m;
		}

		// allocates more space if needed/possible.
		// caller holds lock
		inline T_MMAlloc *findFree(void *base, size_t max, size_t len, size_t aln,
					   const char *key) {
			COMPILE_TIME_ASSERT(MMMAX_N_META <= 10); // for "0123456789"[x]...

			size_t x = 0, y = 0, z = 0;
			size_t pad, off;
			// just get initial figures for allocation.
			getSpace(base, aln, len, off, pad);
			T_MMAlloc *m = __locateOne<true, true>(x, y, z);
			if (m) {
				m->setMem(off, len, pad);
				if (m->offset() + len > max) {
					return NULL;
				}
				// now, "commit" the allocation...
				allocSpace(off, len, pad);
				m->key(key);
				m->addRef();
				__mark(z);
			}
			return m;
		}

		// caller holds lock
		inline void forAllActive(void (*func)(T_MMAlloc *m, void *cookie),
					 void *cookie = NULL) {
			size_t x = 0, y = 0, z = 0;
			T_MMAlloc *m;
			while ((m = __locateOne<true, false>(x, y, z))) {
				func(m, cookie);
				__locateSkip(x, y, z);
			}
		}

		// 'm' must have been immediate result of find()...
		// returns PAMI_SUCCESS if last reference gone, otherwise
		// PAMI_EAGAIN (potentially, PAMI_ERROR but not currently used).
		inline pami_result_t free(T_MMAlloc *m, void (*func)(T_MMAlloc *m) = NULL) {
			size_t z = _last_z;
			__unMark(z);
			return m->free(func);
		}
	private:
		MemoryManager *_meta_mm; // mm for meta data (same scope as parent)
		bool _pre_alloc;
		MemoryManagerHeader *_metahdr;
		char _meta_key_fmt[MMKEYSIZE];
		size_t _meta_key_len;
		T_MMAlloc *_metas[MMMAX_N_META];
		uint64_t *_my_metas;
		size_t _last_z;
	}; // class MemoryManagerMeta

public:

	static MemoryManager *heap_mm; // reference to __global.heap_mm without circ deps.
	static MemoryManager *shared_mm; // reference to __global.shared_mm without circ deps.
	static MemoryManager *shm_mm; // reference to __global.mm without circ deps.

	///
	/// \brief Empty base memory manager constructor
	///
	inline MemoryManager() :
	_base(NULL),
	_size(0),
	_enabled(false),
	_attrs(0),
	_alignment(sizeof(void *)),
	_pmm(NULL) {
		TRACE_ERR((stderr, "%s, this = %p\n", __PRETTY_FUNCTION__, this));
	}

	virtual ~MemoryManager() {
		//_pmm = NULL;
		_enabled = false;
		_base = NULL;
	}

	inline void enable() { _enabled = true; }
	inline void disable() { _enabled = false; }
	inline unsigned attrs() { return _attrs; }
	inline void attrs(unsigned attrs) { _attrs |= attrs; }
	inline MemoryManager *getParent() { return _pmm; }

	///
	/// \brief Allocate an aligned buffer of the memory.
	///
	/// NOTE, this is not efficient for small allocations. Users that are
	/// allocating large numbers of small chunks should allocate one
	/// large block and then sub-divide into small chunks.
	///
	/// The initializer function is called only once on a given chunk of memory,
	/// by the first caller to allocate with a given key.
	///
	/// This does NOT support the freeing of memory
	///
	/// \param[out] memptr    Pointer to the allocated memory.
	/// \param[in]  alignment Requested buffer alignment - must be a power of 2.
	/// \param[in]  bytes     Number of bytes to allocate.
	/// \param[in]  key       (opt) Shared identifier for allocation
	/// \param[in]  init_fn   (opt) Initializer
	/// \param[in]  cookie    (opt) Opaque data for initializer
	///
	virtual pami_result_t memalign(void **memptr, size_t alignment, size_t bytes,
				       const char *key = NULL,
				       MM_INIT_FN *init_fn = NULL, void *cookie = NULL) = 0;
        inline void * malloc(size_t bytes) {
          pami_result_t     rc;
          void   *memptr;
          size_t  align=16;
          rc = this->memalign(&memptr, align, bytes);
          if(rc != PAMI_SUCCESS)
            return NULL;
          return memptr;
        }

	virtual void free(void *mem) = 0;
	virtual size_t available(size_t alignment = 1) = 0;

	virtual const char *getName() = 0;
	///
	/// \brief Intialize a memory manager from another memory manager
	///
	/// \param[in] mm        MemoryManager providing buffer
	/// \param[in] bytes     Number of bytes of memory to manage
	/// \param[in] alignment (opt) alignment of memory chunk
	/// \param[in] new_align (opt) Default/minimum alignment for mm
	/// \param[in] attrs     (opt) Attributes added to new mm
	/// \param[in] key	 (opt) Unique identifier for memory
	/// \param[in] init_fn	 (opt) Initializer function
	/// \param[in] cookie	 (opt) Opaque data for init_fn
	///
	virtual pami_result_t init(MemoryManager *mm,
				   size_t bytes, size_t alignment = 1,
				   size_t new_align = 1,
				   unsigned attrs = 0, const char *key = NULL,
				   MM_INIT_FN *init_fn = NULL, void *cookie = NULL) = 0;
	///
	/// \brief Intialize a memory manager from a buffer
	///
	/// \param[in] mm        MemoryManager that provided buffer
	/// \param[in] buf       Buffer, must have been obtained from mm
	/// \param[in] bytes     Number of bytes (length of buffer)
	/// \param[in] key	 (opt) (information only) Name for new mm
	/// \param[in] new_align (opt) Default/minimum alignment for new mm
	/// \param[in] attrs     (opt) Attributes added to new mm
	///
	virtual pami_result_t init2(MemoryManager *mm,
				  void *buf, size_t bytes,
				  const char *key = NULL,
				  size_t new_align = 1,
				  unsigned attrs = 0) = 0;
	///
	/// \brief Reset a memory manager
	///
	/// Forceably release all user memory. (caution!)
	/// If PAMI_MM_DEBUG is set, will print then reset statistics.
	///
	virtual pami_result_t reset(bool force = false) = 0;

	virtual void dump(const char *str = NULL) = 0;
	///
	/// \brief Return the size of the managed memory buffer
	///
	/// \return    Number of bytes in the memory buffer
	///
	inline size_t size() {
		TRACE_ERR((stderr, "%s %zu\n", __PRETTY_FUNCTION__, _size));
		PAMI_assert_debug(_base != NULL);
		return _size;
	}

	///
	/// \brief Return the base address of the managed memory buffer
	///
	/// \return    base address of the memory buffer
	///
	inline void *base() {
		return _base;
	}

protected:
#ifdef MM_DEBUG
	inline long setup_debug() {
		char *s = getenv("PAMI_MM_DEBUG");
		if (!s) return 0;
		char *t;
		long v = strtol(s, &t, 0);
		if (*t == '\0') {	// entire string was number
			if (t == s) v = 1;
			return v;
		}
		if (t != s && *t == ':') {
			++t;	// skip past ':' and use 't' to match
		} else {
			t = s;	// entire string is to be matched
			v = 1;	// default to level "1"
		}
		if (fnmatch(t, getName(), 0) == 0) {
			return v;
		} else {
			return 0;
		}
	}
#endif // MM_DEBUG

	void *_base;
	size_t _size;
	bool   _enabled;
	unsigned _attrs;
	size_t _alignment;
	MemoryManager *_pmm; // parent mm
}; // class MemoryManager

class GenMemoryManager : public MemoryManager {
private:
	inline void MM_RESET_STATS() {
		_num_allocs = 0;
		_num_frees = 0;
		_loc_bytes = 0;
		_rep_bytes = 0;
		_fre_bytes = 0;
	}
	inline void MM_DUMP_STATS() {
		fprintf(stderr, "%s(%p, %zd): %zd allocs, %zd frees, "
			"local %zd, repeat %zd, freed %zd\n",
			_name, _base, _size,
			_num_allocs, _num_frees,
			_loc_bytes, _rep_bytes, _fre_bytes);
	}
public:
	static size_t MAX_META_SIZE() {
		size_t y = MemoryManagerMeta<MemoryManagerAlloc>::MAX_NUM_META();
		return y * sizeof(MemoryManagerAlloc) + sizeof(MemoryManagerHeader);
	}

	GenMemoryManager() :
	MemoryManager(),
	_meta() {
#ifdef MM_DEBUG // set/unset in MemoryManager.h
		MM_RESET_STATS();
#endif // MM_DEBUG
	}
	virtual ~GenMemoryManager() {
#ifdef MM_DEBUG
		if (_debug) {
			MM_DUMP_STATS();
			if (_debug > 1) _meta.dump(_name);
		}
#endif // MM_DEBUG
		if (_base) {
			if (_pmm->base()) {
				_pmm->free(_base);
			}
		}
	}

	inline const char *getName() { return _name; }

	inline pami_result_t reset(bool force = false) {
#ifdef MM_DEBUG
		if (_debug) {
			MM_DUMP_STATS();
			MM_RESET_STATS();
		}
#endif // MM_DEBUG
		// "reset" - discard arena
		pami_result_t rc = PAMI_SUCCESS;
		if (force) {
			// need to coordinate and synchronize... TBD
			rc = _meta.reset();
			memset(_base, 0, _size);
		}
		return rc;
	}

	inline pami_result_t init2(MemoryManager *mm,
				  void *buf, size_t bytes,
				  const char *key = NULL,
				  size_t new_align = 1,
				  unsigned attrs = 0) {
		PAMI_assert_debugf(!(new_align & (new_align - 1)), "%zd: new_align must be power of two", new_align);
		PAMI_assert_debugf(mm && _enabled == false, "Trying to re-init MemoryManager");
		if (new_align < _alignment) { new_align = _alignment; }
		if (attrs & PAMI_MM_SCOPE) {
			// can't change scope
			return PAMI_INVAL;
		}
		_pmm = mm;
		if (key) {
			strncpy(_name, key, sizeof(_name));
		} else {
			sprintf(_name, "%s-%p", mm->getName(), this);
		}
#ifdef MM_DEBUG // set/unset in MemoryManager.h
		_debug = setup_debug();
#endif // MM_DEBUG
		_attrs = mm->attrs() | attrs;
		_base = buf;
		_size = bytes;
		_alignment = new_align;
		// have a problem when creating shm_mm,
		// can't use itself for meta data allocs.
		MemoryManager *mmm = (mm->attrs() & PAMI_MM_NODESCOPE) ?
				     (this == shm_mm ? shared_mm : shm_mm) : heap_mm;
		_meta.init(mmm, _name);
		_enabled = true;
		return PAMI_SUCCESS;
	}

	inline pami_result_t init(MemoryManager *mm,
				  size_t bytes, size_t alignment = 1,
				  size_t new_align = 1,
				  unsigned attrs = 0, const char *key = NULL,
				  MM_INIT_FN *init_fn = NULL, void *cookie = NULL) {
		// assert 'key' to be non-NULL?
		PAMI_assert_debugf(!(alignment & (alignment - 1)), "%zd: alignment must be power of two", alignment);
		PAMI_assert_debugf(!(new_align & (new_align - 1)), "%zd: new_align must be power of two", new_align);
		PAMI_assert_debugf(mm && _enabled == false, "Trying to re-init MemoryManager");
		if (new_align < _alignment) { new_align = _alignment; }
		if (attrs & PAMI_MM_SCOPE) {
			// can't change scope
			return PAMI_INVAL;
		}
#if 0
		// minimal alignment for MemoryManagerAlloc
		if (alignment < T_Alloc::ALIGNMENT) {
			alignment = T_Alloc::ALIGNMENT;
		}
#endif
		_pmm = mm;
		// make new allocation...
		if (key) {
			strncpy(_name, key, sizeof(_name));
		} else {
			_name[0] = '\0';
		}
		pami_result_t rc;
		rc = mm->memalign((void **)&_base, alignment, bytes,
				  _name, init_fn, cookie);
		if (rc != PAMI_SUCCESS) {
			return rc;
		}
#ifdef MM_DEBUG // set/unset in MemoryManager.h
		_debug = setup_debug();
#endif // MM_DEBUG
		_attrs = mm->attrs() | attrs;
		_size = bytes;
		_alignment = new_align;
		// have a problem when creating shm_mm,
		// can't use itself for meta data allocs.
		MemoryManager *mmm = (mm->attrs() & PAMI_MM_NODESCOPE) ?
				     (this == shm_mm ? shared_mm : shm_mm) : heap_mm;
		_meta.init(mmm, _name);
		_enabled = true;
		return PAMI_SUCCESS;
	}

	inline pami_result_t memalign(void **memptr, size_t alignment, size_t bytes,
				      const char *key = NULL,
				      MM_INIT_FN *init_fn = NULL, void *cookie = NULL) {
		if (alignment < _alignment) { alignment = _alignment; }
		if (key && strlen(key) >= MMKEYSIZE) {
			return PAMI_INVAL;
		}
		_meta.acquire();
		MemoryManagerAlloc *m;
		if (key && key[0]) {
			// "public" (shared) allocation
			m = _meta.find(key); // does addRef() if found...
			if (m) {
				_meta.release();
				m->waitDone();
				*memptr = (uint8_t *)_base + m->offset();
#ifdef MM_DEBUG
				if (_debug) {
					++_num_allocs;
					size_t alloc_bytes = m->rawSize(); // m->userSize();
					_rep_bytes += alloc_bytes;
				}
#endif // MM_DEBUG
				return PAMI_SUCCESS;
			}
			// lock still held...
		} else {
			if (key) {
				// callers wants to know the unique key we chose...
				snprintf((char *)key, MMKEYSIZE, "/%d-%lx",
					 getpid(), (unsigned long)memptr);
			}
		}
		m = _meta.findFree(_base, _size, bytes, alignment, key);
		if (!m) { // either no space for user or no space for meta...
			_meta.release();
#ifdef MM_DEBUG
			if (_debug) {
				dump("ENOMEM");
			}
#endif // MM_DEBUG
			return PAMI_ERROR;
		}
		_meta.release();
		*memptr = (uint8_t *)_base + m->offset();
		if (init_fn) {
			init_fn(*memptr, bytes, key, _attrs, cookie);
		}
		m->initDone();
#ifdef MM_DEBUG
		if (_debug) {
			++_num_allocs;
			size_t alloc_bytes = m->rawSize(); // m->userSize();
			_loc_bytes += alloc_bytes;
		}
#endif // MM_DEBUG
		return PAMI_SUCCESS;
	}

	inline void free(void *mem) {
		// for now, only top-level mm's actually free...
		// i.e. SharedMemoryManager and HeapMemoryManager.
		// and then only during job exit (dtor).
#ifdef MM_DEBUG
		size_t off = (size_t)mem - (size_t)_base;
		MemoryManagerAlloc *m = _meta.find(off);
		if (m) {
			if (_debug) {
				++_num_frees;
				_fre_bytes += m->rawSize();
			}
			// _meta.free(m); // save it for dtor... someday...
		}
#endif // MM_DEBUG
	}

	///
	/// \brief Return the current maximum number of bytes that may be allocated
	///
	/// \param[in] alignment Optional alignment parameter
	///
	/// \return    Number of bytes available
	///
	inline size_t available(size_t alignment = 1) {
		TRACE_ERR((stderr, "%s(%zu) _size %zu, _offset %zu, this = %p\n", __PRETTY_FUNCTION__, alignment, _size, _offset, this));
		PAMI_assert(_enabled == true);
		PAMI_assert_debug((alignment & (alignment - 1)) == 0);
		if (alignment < _alignment) { alignment = _alignment; }

		return _size - _meta.spaceUsed();
	}

	inline void dump(const char *str) {
		if (str) {
			fprintf(stderr, "%s: ", str);
		}
		fprintf(stderr, "%s::GenMemoryManager %p %zd (%zd) %x\n", _name,
			_base, _size, _meta.spaceUsed(), _attrs);
#ifdef MM_DEBUG
		if (_debug) {
			MM_DUMP_STATS();
		}
#endif // MM_DEBUG
		_meta.dump(_name);
		_pmm->dump(str);
	}
private:
	MemoryManagerMeta<MemoryManagerAlloc> _meta;
	char _name[MMKEYSIZE];
#ifdef MM_DEBUG
	long _debug;
	size_t _num_allocs;
	size_t _num_frees;
	size_t _loc_bytes;
	size_t _rep_bytes;
	size_t _fre_bytes;
#endif // MM_DEBUG
}; // class GenMemoryManager

}; // namespace Memory
}; // namespace PAMI

#endif // __components_memory_MemoryManager_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments --max-instatement-indent=79
// astyle options --min-conditional-indent=2 --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements --align-pointer=name --lineend=linux
//
