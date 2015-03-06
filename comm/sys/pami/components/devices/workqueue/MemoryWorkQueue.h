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
 * \file components/devices/workqueue/MemoryWorkQueue.h
 * \brief ???
 */

#ifndef __components_devices_workqueue_MemoryWorkQueue_h__
#define __components_devices_workqueue_MemoryWorkQueue_h__

#include "components/devices/workqueue/WorkQueue.h"

namespace PAMI
{
  namespace Device
  {
    namespace WorkQueue
    {
      ///
      /// \brief Work queue implementation of a contiguous memory buffer.
      ///
      class MemoryWorkQueue : public WorkQueue
      {
        public:

          ///
          /// \brief Single-consumer memory work queue constructor.
          ///
          /// Only one consumer of this work queue is allowed.
          ///
          /// \param[in] buffer Memory location of the input/output buffer
          /// \param[in] length Length in bytes of the memory buffer
          /// \param[in] input  \c True if this memory work queue is an input
          ///                   buffer and already contains data available to consume.
          ///
          MemoryWorkQueue (char * buffer, size_t length, bool input) :
            WorkQueue (),
            _buffer (buffer),
            _length (length),
            _produced (0),
            _consumed (0),
            _input (input)
          {
            if (input) _produced = length;
          }

          ///
          /// \brief Clone constructor.
          ///
          /// Used to create a second local memory wrapper object of the same
          /// local memory resource.
          ///
          /// \see WorkQueue(WorkQueue &)
          ///
          /// \param[in] obj     Memory work queue object
          ///
          MemoryWorkQueue (MemoryWorkQueue & obj) :
            WorkQueue (obj),
            _buffer (obj._buffer),
            _length (obj._length),
            _produced (0),
            _consumed (0),
            _input (obj._input)
          {}

#if 0
          ///
          /// \brief Multiple-consumer memory work queue constructor.
          ///
          /// Multiple consumers of this work queue is allowed.
          ///
          /// \note Currently asserts if the number of consumers is greater than one.
          ///
          /// \param[in] buffer    Memory location of the input/output buffer
          /// \param[in] length    Length in bytes of the memory buffer
          /// \param[in] input     \c True if this memory work queue is an input
          ///                      buffer and already contains data available to consume.
          /// \param[in] consumers Number of consumers of this memory work queue.
          ///
          MemoryWorkQueue (char * buffer, size_t length, bool input, unsigned consumers) :
            WorkQueue (consumers),
            _buffer (buffer),
            _length (length),
            _produced (0),
            _consumed (0),
            _input (input)
          {
            PAMI_assert_debug(_consumers == 1);
            if (input) _produced = length;
          }
#endif
          ///
          /// \brief Virtual destructors make compilers happy.
          ///
          virtual ~MemoryWorkQueue ();

          ///
          /// \brief Reset this memory work queue.
          ///
          /// Sets the number of bytes consumed to zero and, if this is an
          /// input memory work queue, sets the number of bytes produced to
          /// the memory buffer length. Otherwise, sets the number of bytes
          /// produced to zero.
          ///
          inline void reset ()
          {
            _consumed = 0;
            if (_input)
              _produced = _length;
            else
              _produced = 0;
          }

          ///
          /// \brief Dump work queue statistics to stderr.
          ///
          /// \param[in] prefix Optional character string to prefix.
          ///
          inline void dump (const char * prefix = NULL)
          {
            if (prefix == NULL)
              fprintf (stderr, "MemoryWorkQueue::dump() _buffer = %p, _length = %zu, _produced = %zu, _consumed = %zu, _input = %d\n",
                       _buffer, _length, _produced, _consumed, _input);
            else
              fprintf (stderr, "%s MemoryWorkQueue::dump() _buffer = %p, _length = %zu, _produced = %zu, _consumed = %zu, _input = %d\n",
                       prefix, _buffer, _length, _produced, _consumed, _input);
          }

          inline size_t bytesAvailableToProduce (unsigned producer)
          {
            PAMI_abortf("%s<%d>\n",__FILE__,__LINE__);
            return 0;
          }

          ///
          /// \brief Return the maximum number of bytes that can be produced into this work queue.
          ///
          /// The number of bytes that may be produced is the length of the memory
          /// buffer less the number of bytes already produced. Input memory work
          /// queue will always return \c 0 because the number of bytes produced is
          /// initialized to the memory buffer length.
          ///
          /// Bytes must be produced into the memory location returned by bufferToProduce() and then
          /// this work queue \b must be updated with produceBytes().
          ///
          /// \see bufferToProduce
          /// \see produceBytes
          ///
          /// \return Number of bytes that may be produced.
          ///
          inline size_t bytesAvailableToProduce ()
          {
            //fprintf (stderr, "MemoryWorkQueue::bytesAvailableToProduce() -- (_length=%d - _produced=%d) = %d\n", _length, _produced, _length - _produced);
            return _length - _produced;
          }

          inline size_t bytesAvailableToConsume (unsigned consumer)
          {
            PAMI_assert_debug(0);
            return 0;
          }

          ///
          /// \brief Return the maximum number of bytes that can be consumed from this work queue.
          ///
          /// Consuming from work queues with multiple consumers must specify the consumer id.
          ///
          /// Bytes must be consumed into the memory location returned by bufferToConsume() and then
          /// this work queue \b must be updated with consumeBytes().
          ///
          /// \see bufferToConsume
          /// \see consumeBytes
          ///
          /// \return Number of bytes that may be consumed.
          ///
          inline size_t bytesAvailableToConsume ()
          {
            //fprintf (stderr, "MemoryWorkQueue::bytesAvailableToConsume() -- (_produced=%d - _consumed=%d) = %d\n", _produced, _consumed, _produced - _consumed);
            return _produced - _consumed;
          }

          inline size_t getBytesProduced (unsigned producer)
          {
            PAMI_assert_debug(0);
            return 0;
          }

          inline size_t getBytesProduced ()
          {
            return _produced;
          }

          inline size_t getBytesConsumed (unsigned consumer)
          {
            PAMI_assert_debug(0);
            return 0;
          }

          inline size_t getBytesConsumed ()
          {
            return _consumed;
          }

          ///
          /// \brief Get the length in bytes of the underlying memory buffer.
          ///
          /// \note The length of the memory buffer is difference than the number
          ///       of bytes consumed or produced.
          ///
          /// \return Memory buffer length in bytes
          ///
          inline size_t getLength ()
          {
            return _length;
          }

        protected:

          inline char * bufferToProduce (unsigned producer)
          {
            PAMI_assert_debug(0);
            return NULL;
          }

          inline char * bufferToProduce ()
          {
            return &_buffer[_produced];
          }

          inline void produceBytes (size_t bytes, unsigned producer)
          {
            PAMI_assert_debug(0);
          }

          inline void produceBytes (size_t bytes)
          {
            _produced += bytes;
          }

          inline char * bufferToConsume (unsigned consumer)
          {
            PAMI_assert_debug(0);
            return NULL;
          }

          inline char * bufferToConsume ()
          {
            return &_buffer[_consumed];
          }

          inline void consumeBytes (size_t bytes, unsigned consumer)
          {
            PAMI_assert_debug(0);
          }

          inline void consumeBytes (size_t bytes)
          {
            _consumed += bytes;
          }

          char * _buffer;
          size_t _length;
          size_t _produced;
          size_t _consumed;
          bool   _input;
      };
    };
  };
};

#endif // __components_devices_workqueue_memoryworkqueue_h__
