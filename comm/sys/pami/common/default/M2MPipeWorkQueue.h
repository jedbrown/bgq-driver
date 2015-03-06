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
 * \file common/default/M2MPipeWorkQueue.h
 * \brief Implementation of M2MPipeWorkQueue
 */

#ifndef __common_default_M2MPipeWorkQueue_h__
#define __common_default_M2MPipeWorkQueue_h__

#include "Arch.h"
#include "common/M2MPipeWorkQueueInterface.h"
#include "util/common.h"

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x


namespace PAMI
{
  enum M2MType {
    M2M_SINGLE      = 0,
    M2M_VECTOR_INT  = 1,
    M2M_VECTOR_LONG = 2
  };

  template <typename T_Int, bool T_Single>
    class M2MPipeWorkQueueT : public Interface::M2MPipeWorkQueue<PAMI::M2MPipeWorkQueueT<T_Int, T_Single> >
  {
///
/// \brief Work queue implementation of a flat many2manyshared indexed memory buffer.
///
/// \note May not both produce and consume on a single M2M PWQ because there is a
/// single "_bytes" member which can not track both operations.   Also, this means
/// that the M2M PWQ may NOT be initialized with partially full byte counts.
/// This was a result of intentionally trying to minimize the state data tracked down
/// to a single size_t per access point.


    public:
    M2MPipeWorkQueueT<T_Int, T_Single>() :
          Interface::M2MPipeWorkQueue<PAMI::M2MPipeWorkQueueT<T_Int, T_Single> >(),
          _buffer(NULL),
          _offsets(NULL),
          _bytes(NULL),
          _dgspCounts(NULL),
          _dgsp(PAMI_TYPE_BYTE), /// \assume PAMI_TYPE_BYTE
          _indexCount(0),
          _sizeOfDgsp(1),    /// \assume PAMI_TYPE_BYTE
          _byteCount(0),
	  _nactive(0)
      {
      }

      /// \brief Configure for Many to Many (indexed flat buffer) access.
      ///
      /// Only one consumer OR producer at a time is allowed.
      ///
      /// Sets up a flat buffer for indexed access by many to many.
      ///
      /// Each index (offset) in the buffer may be consumed or produced independently.
      ///
      /// Each index specifies maximum size with an arbitrary "initial fill".   An initial
      /// fill of 0 implies a producer-only PWQ.   A FULL initial fill implies a consumer-only
      /// PWQ.  Its the users responsibility to only call consumer or producer
      /// interfaces or unpredictable results may occur.
      ///
      /// The interface is similar to PAMI::PipwWorkQueue's except that an index" will be
      /// required for all consume/produce calls.
      ///
      /// Warning! The PWQ retains and uses the input arrays, it does NOT copy them.  It
      /// may change the contents of these arrays.
      ///
      /// \param[in]  buffer        Buffer to use
      /// \param[in]  indexcount    Number of indexed access points to the pwq
      /// \param[in]  dgsp          Memory layout of each buffer unit
      /// \param[in]  offsets       Array[indexcount] of byte offsets for each indexed access point
      /// \param[in]  dgspcounts    Array[indexcount] of data type (dgsp) counts
      /// \param[in]  bufinit       Array[indexcount] of bytes initially in buffer (storage may be modified)
      ///
      /// \note bufinit must be empty (0) for producer PWQ or full (size of dgsp type * dsgpcounts[index]) for consumer PWQ.
      ///
      inline void configure_impl(char *buffer, size_t indexcount,
                                 pami_type_t *dgsp, void *offsets, void *dgspcounts, void *bufinit)
      {
        /// \todo 'real' dgsp is unimplemented now, so assume PAMI_TYPE_BYTE
        /// \todo why pami_type_t*? Copied from PAMI::PipeWorkQueue
        //PAMI_assert(*dgsp == PAMI_TYPE_BYTE);
        PAMI::Type::TypeCode * tempType = (PAMI::Type::TypeCode *) *dgsp;
        _sizeOfDgsp = tempType->GetExtent();

        _indexCount = indexcount;
        _buffer     = buffer;
        _offsets    = (T_Int *) offsets;
        _bytes      = (T_Int *) bufinit;
        _dgspCounts = (T_Int *) dgspcounts;
        _dgsp       = *dgsp;

        _nactive = 0;
        for (size_t i=0; i < indexcount; ++i)
          if (_dgspCounts[i] > 0)
            _nactive ++;

        COMPILE_TIME_ASSERT(T_Single == 0);
      }
      /// \brief Configure for Many to Many (indexed flat buffer) access.
      ///
      /// Only one consumer OR producer at a time is allowed.
      ///
      /// Sets up a flat buffer for indexed access by many to many.
      ///
      /// Each index (offset) in the buffer may be consumed or produced independently.
      ///
      /// Each index specifies maximum size with an arbitrary "initial fill".   An initial
      /// fill of 0 implies a producer-only PWQ.   A FULL initial fill implies a consumer-only
      /// PWQ.  Its the users responsibility to only call consumer or producer
      /// interfaces or unpredictable results may occur.
      ///
      /// The interface is similar to PAMI::PipwWorkQueue's except that an index" will be
      /// required for all consume/produce calls.
      ///
      /// Warning! The PWQ retains and uses the input arrays, it does NOT copy them.  It
      /// may change the contents of these arrays.
      ///
      /// \param[in]  buffer        Buffer to use
      /// \param[in]  indexcount    Number of indexed access points to the pwq (bufinit array only)
      /// \param[in]  dgsp          Memory layout of each buffer unit
      /// \param[in]  offset        Byte offset for each indexed access point
      /// \param[in]  dgspcount     Data type (dgsp) countfor each indexed access point
      /// \param[in]  bufinit       Array[indexcount] of bytes initially in buffer (storage may be modified)
      ///
      /// \note bufinit must be empty (0) for producer PWQ or full (size of dgsp type * dsgpcounts[index]) for consumer PWQ.
      ///
      inline void configure_impl(char *buffer, size_t indexcount,
                                 pami_type_t *dgsp, size_t offset, size_t dgspcount, size_t *bufinit)
      {
        /// \todo 'real' dgsp is unimplemented now, so assume PAMI_TYPE_BYTE
        /// \todo why pami_type_t*? Copied from PAMI::PipeWorkQueue
        //PAMI_assert(*dgsp == PAMI_TYPE_BYTE);
        PAMI::Type::TypeCode * tempType = (PAMI::Type::TypeCode *) *dgsp;
        _sizeOfDgsp = tempType->GetExtent();

        _indexCount = indexcount;
        _buffer     = buffer;
        _bytes      = bufinit;
        _dgsp       = *dgsp;
        // Only a single offset and count, point our normal array at this single value.
        _offsets    = NULL;
        _byteCount  = dgspcount * _sizeOfDgsp;
        _dgspCount  = dgspcount;
        _dgspCounts = &_dgspCount;
        _nactive    = indexcount;
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::configure()indexcount %zu, offset %zu, dgspcount %zu, bufinit %zu\n",this, indexcount,  offset,  dgspcount,  bufinit[0]));
        #if 0
        size_t size = _byteCount * _indexCount;
        for(size_t i=0; i < size; ++i)
          {
          TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::configure() buffer[%zu]=%u\n",this, i, buffer[i]));
          }
        #endif

        COMPILE_TIME_ASSERT(T_Single == 1);
      }

      ///
      /// \brief Clone constructor.
      ///
      /// Used to create a second local memory wrapper object of the same
      /// shared memory resource.
      ///
      /// \see WorkQueue(WorkQueue &)
      ///
      /// \param[in] obj     Shared work queue object
      ///
      M2MPipeWorkQueueT<T_Int, T_Single>(M2MPipeWorkQueueT &obj) :
      Interface::M2MPipeWorkQueue<PAMI::M2MPipeWorkQueueT<T_Int, T_Single> >(),
          _buffer(obj._buffer),
          _offsets(obj._offsets),
          _bytes(obj._bytes),
          _dgspCounts(obj._dgspCounts),
          _dgsp(obj._dgsp),
          _indexCount(obj._indexCount),
          _sizeOfDgsp(obj._sizeOfDgsp),
	  _byteCount(obj._byteCount),
	  _nactive(obj._nactive)
      {
      }

      ///
      /// \brief Virtual destructors make compilers happy.
      ///
      inline void operator delete(void * p)
      {
      }
      ~M2MPipeWorkQueueT<T_Int, T_Single>()
      {
      }

      ///
      /// \brief Reset this shared memory work queue.
      ///
      /// Sets the number of bytes produced and the number of bytes
      /// consumed by each consumer to zero.
      ///
      inline void reset_impl()
      {
      }

      ///
      /// \brief Dump shared memory work queue statistics to stderr.
      ///
      /// \param[in] prefix Optional character string to prefix.
      ///
      inline void dump_impl(const char *prefix = NULL)
      {

        if (prefix == NULL)
          {
            prefix = "";
          }

        fprintf(stderr, "%s dump(%p)\n",
                prefix, this);
      }

      ///
      /// \brief Return the maximum number of bytes that can be produced into this work queue.
      ///
      /// Bytes must be produced into the memory location returned by bufferToProduce() and then
      /// this work queue \b must be updated with produceBytes().
      ///
      /// \see bufferToProduce
      /// \see produceBytes
      ///
      /// \param[in] index of access point
      ///
      /// \return Number of bytes that may be produced.
      ///
      inline size_t bytesAvailableToProduce_impl(size_t index)
      {
        PAMI_assert(index < _indexCount);
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bytesAvailableToProduce()=%zu index %zu, _indexcount %zu, offset %zu, dgspcount %zu, bufinit %zu/%zu\n",this, _dgspCounts[T_Single?0:index]*_sizeOfDgsp - _bytes[index], index, _indexCount,  (size_t)(_offsets?_offsets[0]:-1U),  _dgspCounts[0],  (size_t)_bytes[0], (size_t)_bytes[index]));
        return (T_Single?(_byteCount):(_dgspCounts[index]*_sizeOfDgsp)) - _bytes[index];
      }

      ///
      /// \brief Return the maximum number of bytes that can be consumed from this work queue.
      ///
      /// Bytes must be consumed into the memory location returned by bufferToConsume() and then
      /// this work queue \b must be updated with consumeBytes().
      ///
      /// \see bufferToConsume
      /// \see consumeBytes
      ///
      /// \param[in] index of access point
      ///
      /// \return Number of bytes that may be consumed.
      ///
      inline size_t bytesAvailableToConsume_impl(size_t index)
      {
        PAMI_assert(index < _indexCount);
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bytesAvailableToConsume()=%zu index %zu, _indexcount %zu, offset %zu, dgspcount %zu, bufinit %zu/%zu\n",this, (size_t)_bytes[index], index, _indexCount,  (size_t)(_offsets?_offsets[0]:-1U),  _dgspCounts[0],  (size_t)_bytes[0], (size_t)_bytes[index]));
        return _bytes[index];
      }

      /// \brief raw accessor for total number of bytes produced since reset()
      ///
      /// \param[in] index of access point
      ///
      /// \return number of bytes produced
      ///
      inline size_t getBytesProduced_impl(size_t index)
      {
        PAMI_assert(index < _indexCount);
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bytesProduce()=%zu index %zu, _indexcount %zu, offset %zu, dgspcount %zu, bufinit %zu/%zu\n",this, _bytes[index], index, _indexCount,  _offsets?_offsets[0]:-1U,  _dgspCounts[0],  (size_t)_bytes[0], (size_t)_bytes[index]));
        return _bytes[index];
      }

      /// \brief raw accessor for total number of bytes consumed since reset()
      ///
      /// \param[in] index of access point
      ///
      /// \return number of bytes consumed
      ///
      inline size_t getBytesConsumed_impl(size_t index)
      {
        PAMI_assert(index < _indexCount);
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bytesConsumeed()=%zu index %zu, _indexcount %zu, offset %zu, dgspcount %zu, bufinit %zu/%zu\n",this, _dgspCounts[T_Single?0:index]*_sizeOfDgsp - _bytes[index], index, _indexCount,  _offsets?_offsets[0]:-1U,  _dgspCounts[0],  (size_t)_bytes[0], (size_t)_bytes[index]));
        return (T_Single?(_byteCount):(_dgspCounts[index]*_sizeOfDgsp)) - _bytes[index];
      }

      /// \brief current position for producing into buffer
      ///
      /// \param[in] index of access point
      ///
      /// \return location in buffer to produce into
      ///
      inline char *bufferToProduce_impl(size_t index)
      {
        PAMI_assert(index < _indexCount);
        char *b;
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bufferToProduce()T_Single %u, index %zu, _dgspCounts[0] %zu,_sizeOfDgsp %zu, offsets %zu\n",this,T_Single,index,_dgspCounts[0],_sizeOfDgsp,(size_t)(_offsets?_offsets[0]:-1U)));
        /// \todo Need 'real' dgsp support to find the byte offset in the dgsp type.
        /// Assuming PAMI_TYPE_BYTE now.
        size_t offset = T_Single?_byteCount*index:_offsets[index]*_sizeOfDgsp;
        b = (char *) & _buffer[offset+_bytes[index]];
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bufferToProduce() <%p>buffer\n",this, b));
        return b;
      }

      /// \brief notify workqueue that bytes have been produced
      ///
      /// \param[in] index of access point
      ///
      /// \return number of bytes that were produced
      ///
      inline void produceBytes_impl(size_t index, size_t bytes)
      {
        PAMI_assert(index < _indexCount);
        _bytes[index] += bytes;
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::produceBytes()=%zu index %zu, _indexcount %zu, offset %zu, dgspcount %zu, bufinit %zu/%zu\n",this, _bytes[index], index, _indexCount,  _offsets?_offsets[0]:-1U,  _dgspCounts[0],  (size_t)_bytes[0], (size_t)_bytes[index]));
      }

      /// \brief current position for consuming from buffer
      ///
      /// \param[in] index of access point
      ///
      /// \return location in buffer to consume from
      ///
      inline char *bufferToConsume_impl(size_t index)
      {
        PAMI_assert(index < _indexCount);
        char *b;
        /// \todo Need 'real' dgsp support to find the byte offset in the dgsp type.
        /// Assuming PAMI_TYPE_BYTE now.
        size_t offset = T_Single?_byteCount*index:_offsets[index]*_sizeOfDgsp;
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bufferToConsume() index %zu, single %u, _indexcount %zu, offset %zu, dgspcount %zu, _sizeOfDgsp %zu, _bytes[index] %zu\n",this, index, (unsigned)T_Single, _indexCount,  offset,  _dgspCounts[T_Single?0:index],_sizeOfDgsp, (size_t)_bytes[index]));
        b = (char *) & _buffer[offset+((T_Single?(_byteCount):(_dgspCounts[index]*_sizeOfDgsp)) - _bytes[index])];
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bufferToConsume() <%p/%p>buffer\n",this, _buffer, b));
        #if 0
        size_t size = bytesAvailableToConsume_impl(index);
        for(size_t i=0; i < size; ++i)
          {
          TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::bufferToConsume() index %zu <%p>buffer[%zu]=%u\n",this, index, b+i, i, b[i]));
          }
        #endif
        return b;
      }

      /// \brief notify workqueue that bytes have been consumed
      ///
      /// \param[in] index of access point
      ///
      /// \return number of bytes that were consumed
      ///
      inline void consumeBytes_impl(size_t index, size_t bytes)
      {
        PAMI_assert(index < _indexCount);
        _bytes[index] -= bytes;
        TRACE_ERR((stderr,  "<%p>M2MPipeWorkQueueT::consumeBytes()=%zu index %zu, _indexcount %zu, offset %zu, dgspcount %zu, bufinit %zu/%zu\n",this, _bytes[index], index, _indexCount,  _offsets?_offsets[0]:-1U,  _dgspCounts[0],  (size_t)_bytes[0], (size_t)_bytes[index]));
      }

      inline char *getBufferBase_impl(size_t index) {
        size_t offset = T_Single?(_byteCount*index):_offsets[index]*_sizeOfDgsp;
        return (char *)&_buffer[offset];
      }

      inline size_t getTotalBytes_impl (size_t index) {
        return (T_Single?(_byteCount):(_dgspCounts[index]*_sizeOfDgsp));
      }


      inline size_t numIndices_impl () {
        return _indexCount;
      }
      
      inline size_t numActive_impl () {
        return _nactive;
      }            

      static inline void compile_time_assert ()
      {
//    COMPILE_TIME_ASSERT(sizeof(export_t) <= sizeof(pami_pipeworkqueue_ext_t));
        COMPILE_TIME_ASSERT(sizeof(M2MPipeWorkQueueT) <= sizeof(pami_pipeworkqueue_t));
      }

    private:
      volatile char *_buffer;     /**< flat buffer */
      T_Int         *_offsets;    /**< array of byte offsets to start of each access point */
      T_Int         *_bytes;      /**< array of byte counts (available to consume) */
      T_Int         *_dgspCounts; /**< array of dgsp count size */
      pami_type_t    _dgsp;       /**< dgsp data type */
      size_t         _indexCount; /**< number of indexed access points to the buffer(size of arrays) */
      size_t         _sizeOfDgsp; /**< byte size of the dgsp data type */
      size_t         _dgspCount;  /**< single dgsp count size */
      size_t         _byteCount;  /**< single element size */
      size_t         _nactive;
  }; // class M2MPipeWorkQueueT

}; /* namespace PAMI */

#endif // __common_default_M2MPipeWorkQueueT_h__
