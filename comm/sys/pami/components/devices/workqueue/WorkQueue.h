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
 * \file components/devices/workqueue/WorkQueue.h
 * \brief ???
 */

#ifndef __components_devices_workqueue_WorkQueue_h__
#define __components_devices_workqueue_WorkQueue_h__

#include "Platform.h"
#include "PipeWorkQueue.h"
#include "math/math_coremath.h"
#include <pami.h>
#include "util/common.h"

namespace PAMI
{
  namespace Device
  {
    namespace WorkQueue
    {
      ///
      /// \brief Defines the interfaces and operations for producer/consumer work queues.
      ///
      class _BaseWorkQueue
      {
        public:

          ///
          /// \brief Default constructor.
          ///
          /// Only one consumer of this work queue is allowed.
          ///
          inline _BaseWorkQueue () :
            _producers (1),
            _producer  (0),
            _consumers (1),
            _consumer  (0)
          {}

          ///
          /// \brief Clone constructor.
          ///
          /// Used to create a second work queue object identical to the input.
          ///
          /// \param[in] obj     Work queue object
          ///
          _BaseWorkQueue (_BaseWorkQueue & obj) :
            _producers (obj._producers),
            _producer  (obj._producer),
            _consumers (obj._consumers),
            _consumer  (obj._consumer)
          {}


#if 0
          ///
          /// \brief Multiple consumer constructor.
          ///
          /// Multiple consumers of this work queue is allowed.
          ///
          /// \param[in] consumers Number of consumers that will read from this work queue.
          ///
          inline _BaseWorkQueue (unsigned consumers) :
            _consumers (consumers)
          {}

          ///
          /// \brief Virtual destructors make compilers happy.
          ///
          virtual ~_BaseWorkQueue ();
#endif

          ///
          /// \brief Reset this work queue.
          ///
          void reset ();

          ///
          /// \brief Dump work queue statistics to stderr.
          ///
          /// \param[in] prefix Optional character string to prefix.
          ///
          void dump (const char * prefix = NULL);

          ///
          /// \brief Number of producers accessor method.
          ///
          /// \return Number of producers of this work queue
          ///
          inline unsigned getProducers () { return _producers; }

          ///
          /// \brief Number of producers specifier method.
          ///
          /// \param[in] producers Number of producers of this work queue
          /// \param[in] producer  Producer id of this core for this work queue
          ///
          inline void setProducers (unsigned producers, unsigned producer)
          {
            PAMI_assert_debugf(producers > 0, "WorkQueue requires at least one producer, got %d\n", producers);
            PAMI_assert_debugf(producers <= PAMI_MAX_PROC_PER_NODE, "WorkQueue producers must be less than %d, got %d\n", PAMI_MAX_PROC_PER_NODE, producers);
            PAMI_assert_debugf(producer < producers, "WorkQueue producer number must be less than producers (%d !< %d)\n", producer, producers);

            _producers = producers;
            _producer  = producer;
          }

          ///
          /// \brief Number of consumers accessor method.
          ///
          /// \return Number of consumers of this work queue
          ///
          inline unsigned getConsumers () { return _consumers; }

          ///
          /// \brief Number of consumers specifier method.
          ///
          /// \param[in] consumers Number of consumers of this work queue
          /// \param[in] consumer  Consumer id of this core for this work queue
          ///
          inline void setConsumers (unsigned consumers, unsigned consumer)
          {
            PAMI_assert_debugf(consumers > 0, "WorkQueue requires at least one consumer, got %d\n", consumers);
            PAMI_assert_debugf(consumers <= PAMI_MAX_PROC_PER_NODE, "WorkQueue consumers must be less than %d, got %d\n", PAMI_MAX_PROC_PER_NODE, consumers);
            PAMI_assert_debugf(consumer < consumers, "WorkQueue consumer number must be less than consumers (%d !< %d)\n", consumer, consumers);

            _consumers = consumers;
            _consumer  = consumer;
          }

          size_t bytesAvailableToProduce (unsigned producer);

          ///
          /// \brief Return the maximum number of bytes that can be produced into this work queue.
          ///
          /// Bytes must be produced into the memory location returned by bufferToProduce() and then
          /// this work queue \b must be updated with produceBytes().
          ///
          /// \see bufferToProduce
          /// \see produceBytes
          ///
          /// \return Number of bytes that may be produced.
          ///
          size_t bytesAvailableToProduce ();

          size_t bytesAvailableToConsume (unsigned consumer);

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
          /// \param[in] consumer Consumer id for work queues with multiple consumers
          ///
          /// \return Number of bytes that may be consumed.
          ///
          size_t bytesAvailableToConsume ();

          char * bufferToProduce (unsigned producer);

          ///
          /// \brief Return the memory location to write to produce into this work queue.
          ///
          /// There must be space available in the work queue, determined by a call to
          /// bytesAvailableToProduce(), in order to write to this memory location!
          ///
          /// After writing to the memory location this work queue \b must be updated
          /// with produceBytes().
          ///
          /// \warning Work queues are lockless and only one core (or thread) may
          ///          produce to work queue. Unpredictable results will occur if
          ///          more than one core (or thread) produces into a work queue.
          ///          You have been warned.
          ///
          /// \see reduce2Q
          /// \see Q2Q
          /// \see bytesAvailableToProduce
          /// \see produceBytes
          ///
          /// \return Producer memory location
          ///
          char * bufferToProduce ();

          void   produceBytes (size_t bytes, unsigned producer);

          ///
          /// \brief Update the number of bytes produced into this work queue
          ///
          /// \warning Work queues are lockless and only one core (or thread) may
          ///          produce to work queue. Unpredictable results will occur if
          ///          more than one core (or thread) produces into a work queue.
          ///          You have been warned.
          ///
          /// \see reduce2Q
          /// \see Q2Q
          /// \see bufferToProduce
          /// \see bytesAvailableToProduce
          /// \see bufferToProduce
          ///
          /// \param[in] bytes Number of bytes that were recently produced into this work queue.
          ///
          void   produceBytes (size_t bytes);

          char * bufferToConsume (unsigned consumer);

          ///
          /// \brief Return the memory location to read to consume from this work queue.
          ///
          /// There must be data available in the work queue, determined by a call to
          /// bytesAvailableToConsume(), in order to read from this memory location!
          ///
          /// After reading from the memory location this work queue \b must be updated
          /// with consumeBytes().
          ///
          /// \see bytesAvailableToConsume
          /// \see consumeBytes
          ///
          /// \return Consumer memory location
          ///
          char * bufferToConsume ();

          void   consumeBytes (size_t bytes, unsigned consumer);

          ///
          /// \brief Update the number of bytes consumed from this work queue
          ///
          /// \see bytesAvailableToConsume
          /// \see bufferToConsume
          ///
          /// \param[in] bytes    Number of bytes that were recently consumed from this work queue.
          ///
          void   consumeBytes (size_t bytes);

          size_t getBytesProduced (unsigned producer);

          size_t getBytesProduced ();

          size_t getBytesConsumed (unsigned consumer);

          size_t getBytesConsumed ();

        protected:

          /// Number of producers for this work queue
          unsigned _producers;

          /// Producer id of this core for this work queue.
          unsigned _producer;

          /// Number of consumers for this work queue
          unsigned _consumers;

          /// Consumer id of this core for this work queue.
          unsigned _consumer;
      }; /* class _BaseWorkQueue */

      ///
      /// \brief Defines the interfaces and operations for producer/consumer work queues.
      ///
      /// This version uses virtual functions and defines a byte interface
      ///
      ///
      class WorkQueue : public _BaseWorkQueue
      {
        public:

          ///
          /// \brief Default constructor.
          ///
          /// Only one consumer of this work queue is allowed.
          ///
          inline WorkQueue () : _BaseWorkQueue()
          {}

          ///
          /// \brief Clone constructor.
          ///
          /// Used to create a second work queue object identical to the input.
          ///
          /// \param[in] obj     Work queue object
          ///
          WorkQueue (WorkQueue & obj) : _BaseWorkQueue(obj)
          {}


#if 0
          ///
          /// \brief Multiple consumer constructor.
          ///
          /// Multiple consumers of this work queue is allowed.
          ///
          /// \param[in] consumers Number of consumers that will read from this work queue.
          ///
          inline WorkQueue (unsigned consumers) : _BaseWorkQueue(consumers)
          {}
#endif
          ///
          /// \brief Virtual destructors make compilers happy.
          ///
          virtual ~WorkQueue ();

          ///
          /// \brief Reset this work queue.
          ///
          virtual void reset () = 0;

          ///
          /// \brief Dump work queue statistics to stderr.
          ///
          /// \param[in] prefix Optional character string to prefix.
          ///
          virtual void dump (const char * prefix = NULL) = 0;

          ///
          /// \brief Consume from the input work queue and produce into this work queue.
          ///
          /// \param[in] in       Input work queue which will be consumed
          /// \param[in] consumer Consumer id to use when consuming from the input work queue.
          /// \param[in] func     Math function to invoke to consume-produce the input work unit.
          /// \param[in] dtshift  Data type size of the elements operated upon by the math function.
          ///
          /// \warning Work queues are lockless and only one core (or thread) may
          ///          produce to work queue. Unpredictable results will occur if
          ///          more than one core (or thread) produces into a work queue.
          ///          You have been warned.
          ///
          /// \see reduce2Q
          /// \see bufferToProduce
          /// \see produceBytes
          ///
          /// \return Number of bytes produced into this work queue.
          ///
          inline unsigned Q2Q (WorkQueue & in,
                               coremath1   func,
                               int         dtshift)
          {
            unsigned nbytes = 0;

            unsigned bytes_to_consume = in.bytesAvailableToConsume ();
            unsigned bytes_to_produce = bytesAvailableToProduce ();

            unsigned n = MIN(bytes_to_produce,bytes_to_consume);

            if (n > 0)
            {
              char * dst = bufferToProduce ();
              char * buf = in.bufferToConsume ();
              unsigned count = n >> dtshift;

              func (dst, buf, count);

              nbytes = count << dtshift;

              produceBytes (nbytes);
              in.consumeBytes (nbytes);
            }

            return nbytes;
          }
          // Q2Q using PipeWorkQueue as 'in' (should use template?)
          inline unsigned Q2Q (PAMI::PipeWorkQueue & in,
                               coremath1   func,
                               int         dtshift)
          {
            unsigned nbytes = 0;

            unsigned bytes_to_consume = in.bytesAvailableToConsume ();
            unsigned bytes_to_produce = bytesAvailableToProduce ();

            unsigned n = MIN(bytes_to_produce,bytes_to_consume);

            if (n > 0)
            {
              char * dst = bufferToProduce ();
              char * buf = in.bufferToConsume ();
              unsigned count = n >> dtshift;

              func (dst, buf, count);

              nbytes = count << dtshift;

              produceBytes (nbytes);
              in.consumeBytes (nbytes);
            }

            return nbytes;
          }
          // reverse-direction Q2Q, using PipeWorkQueue as 'out'
          inline unsigned Q2Qr (PAMI::PipeWorkQueue &out,
                               coremath1   func,
                               int         dtshift)
          {
            unsigned nbytes = 0;

            unsigned bytes_to_consume = bytesAvailableToConsume ();
            unsigned bytes_to_produce = out.bytesAvailableToProduce ();

            unsigned n = MIN(bytes_to_produce,bytes_to_consume);

            if (n > 0)
            {
              char * dst = out.bufferToProduce ();
              char * buf = bufferToConsume ();
              unsigned count = n >> dtshift;

              func (dst, buf, count);

              nbytes = count << dtshift;

              out.produceBytes (nbytes);
              consumeBytes (nbytes);
            }

            return nbytes;
          }

          ///
          /// \brief Consume from the input work queues and produce into this work queue.
          ///
          /// \param[in] in0       First input work queue which will be consumed
          /// \param[in] consumer0 Consumer id to use when consuming from the first input work queue.
          /// \param[in] in1       Second input work queue which will be consumed
          /// \param[in] consumer1 Consumer id to use when consuming from the second input work queue.
          /// \param[in] func     Math function to invoke to consume-produce the input work unit.
          /// \param[in] dtshift  Data type size of the elements operated upon by the math function.
          ///
          /// \warning Work queues are lockless and only one core (or thread) may
          ///          produce to work queue. Unpredictable results will occur if
          ///          more than one core (or thread) produces into a work queue.
          ///          You have been warned.
          ///
          /// \see Q2Q
          /// \see bufferToProduce
          /// \see produceBytes
          ///
          /// \return Number of bytes produced into this work queue.
          ///
          inline unsigned reduce2Q (WorkQueue & in0,
                                    WorkQueue & in1,
                                    coremath    func,
                                    int         dtshift)
          {
            unsigned nbytes = 0;

            unsigned in0_bytes_to_consume = in0.bytesAvailableToConsume ();
            unsigned in1_bytes_to_consume = in1.bytesAvailableToConsume ();
            unsigned     bytes_to_consume = MIN(in0_bytes_to_consume,in1_bytes_to_consume);

            unsigned     bytes_to_produce = bytesAvailableToProduce ();
            unsigned     bytes_to_reduce  = MIN(bytes_to_produce,bytes_to_consume);

            if (bytes_to_reduce > 0)
            {
              char * dst  = bufferToProduce ();
              void * buf[] = { in0.bufferToConsume (),
                               in1.bufferToConsume () };

              unsigned count = bytes_to_reduce >> dtshift;

              func (dst, buf, 2, count);

              nbytes = count << dtshift;

              produceBytes (nbytes);
              in0.consumeBytes (nbytes);
              in1.consumeBytes (nbytes);
            }

            return nbytes;
          }

          /// PipeWorkQueue version... "this" is the SharedWorkQueue...
          inline unsigned reduce2Q (PAMI::PipeWorkQueue &in,
                                    PAMI::PipeWorkQueue &out,
                                    coremath    func,
                                    int         dtshift)
          {
            unsigned nbytes = 0;

            unsigned in0_bytes_to_consume = in.bytesAvailableToConsume ();
            unsigned in1_bytes_to_consume = bytesAvailableToConsume ();
            unsigned     bytes_to_consume = MIN(in0_bytes_to_consume,in1_bytes_to_consume);

            unsigned     bytes_to_produce = out.bytesAvailableToProduce ();
            unsigned     bytes_to_reduce  = MIN(bytes_to_produce,bytes_to_consume);

            if (bytes_to_reduce > 0)
            {
              char * dst  = out.bufferToProduce ();
              void * buf[] = { in.bufferToConsume (),
                               bufferToConsume () };

              unsigned count = bytes_to_reduce >> dtshift;

              func (dst, buf, 2, count);

              nbytes = count << dtshift;

              out.produceBytes (nbytes);
              in.consumeBytes (nbytes);
              consumeBytes (nbytes);
            }

            return nbytes;
          }

          inline unsigned reduceInPlace (WorkQueue & in,
                                         coremath    func,
                                         int         dtshift)
          {
            unsigned nbytes = 0;

            unsigned bytes_to_produce =    bytesAvailableToProduce ();
            unsigned bytes_to_consume = in.bytesAvailableToConsume ();
            unsigned bytes_to_reduce  = MIN(bytes_to_consume,bytes_to_produce);

            if (bytes_to_reduce > 0)
            {
              void * buf[] = { bufferToProduce (),
                               in.bufferToConsume () };
              unsigned count = bytes_to_reduce >> dtshift;

              func (buf[0], buf, 2, count);

              nbytes = count << dtshift;

              produceBytes (nbytes);
              in.consumeBytes (nbytes);
            }

            return nbytes;
          }

          // reduceInPlace using PipeWorkQueue as 'in' (should use template?)
          inline unsigned reduceInPlace (PAMI::PipeWorkQueue & in,
                                         coremath    func,
                                         int         dtshift)
          {
            unsigned nbytes = 0;

            unsigned bytes_to_produce =    bytesAvailableToProduce ();
            unsigned bytes_to_consume = in.bytesAvailableToConsume ();
            unsigned bytes_to_reduce  = MIN(bytes_to_consume,bytes_to_produce);

            if (bytes_to_reduce > 0)
            {
              void * buf[] = { bufferToProduce (),
                               in.bufferToConsume () };
              unsigned count = bytes_to_reduce >> dtshift;

              func (buf[0], buf, 2, count);

              nbytes = count << dtshift;

              produceBytes (nbytes);
              in.consumeBytes (nbytes);
            }

            return nbytes;
          }

          virtual size_t bytesAvailableToProduce (unsigned producer) = 0;

          ///
          /// \brief Return the maximum number of bytes that can be produced into this work queue.
          ///
          /// Bytes must be produced into the memory location returned by bufferToProduce() and then
          /// this work queue \b must be updated with produceBytes().
          ///
          /// \see bufferToProduce
          /// \see produceBytes
          ///
          /// \return Number of bytes that may be produced.
          ///
          virtual size_t bytesAvailableToProduce () = 0;

          virtual size_t bytesAvailableToConsume (unsigned consumer) = 0;

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
          /// \param[in] consumer Consumer id for work queues with multiple consumers
          ///
          /// \return Number of bytes that may be consumed.
          ///
          virtual size_t bytesAvailableToConsume () = 0;

          virtual char * bufferToProduce (unsigned producer) = 0;

          ///
          /// \brief Return the memory location to write to produce into this work queue.
          ///
          /// There must be space available in the work queue, determined by a call to
          /// bytesAvailableToProduce(), in order to write to this memory location!
          ///
          /// After writing to the memory location this work queue \b must be updated
          /// with produceBytes().
          ///
          /// \warning Work queues are lockless and only one core (or thread) may
          ///          produce to work queue. Unpredictable results will occur if
          ///          more than one core (or thread) produces into a work queue.
          ///          You have been warned.
          ///
          /// \see reduce2Q
          /// \see Q2Q
          /// \see bytesAvailableToProduce
          /// \see produceBytes
          ///
          /// \return Producer memory location
          ///
          virtual char * bufferToProduce () = 0;

          virtual void   produceBytes (size_t bytes, unsigned producer) = 0;

          ///
          /// \brief Update the number of bytes produced into this work queue
          ///
          /// \warning Work queues are lockless and only one core (or thread) may
          ///          produce to work queue. Unpredictable results will occur if
          ///          more than one core (or thread) produces into a work queue.
          ///          You have been warned.
          ///
          /// \see reduce2Q
          /// \see Q2Q
          /// \see bufferToProduce
          /// \see bytesAvailableToProduce
          /// \see bufferToProduce
          ///
          /// \param[in] bytes Number of bytes that were recently produced into this work queue.
          ///
          virtual void   produceBytes (size_t bytes) = 0;

          virtual char * bufferToConsume (unsigned consumer) = 0;

          ///
          /// \brief Return the memory location to read to consume from this work queue.
          ///
          /// There must be data available in the work queue, determined by a call to
          /// bytesAvailableToConsume(), in order to read from this memory location!
          ///
          /// After reading from the memory location this work queue \b must be updated
          /// with consumeBytes().
          ///
          /// \see bytesAvailableToConsume
          /// \see consumeBytes
          ///
          /// \return Consumer memory location
          ///
          virtual char * bufferToConsume () = 0;

          virtual void   consumeBytes (size_t bytes, unsigned consumer) = 0;

          ///
          /// \brief Update the number of bytes consumed from this work queue
          ///
          /// \see bytesAvailableToConsume
          /// \see bufferToConsume
          ///
          /// \param[in] bytes    Number of bytes that were recently consumed from this work queue.
          ///
          virtual void   consumeBytes (size_t bytes) = 0;

          virtual size_t getBytesProduced (unsigned producer) = 0;

          virtual size_t getBytesProduced () = 0;

          virtual size_t getBytesConsumed (unsigned consumer) = 0;

          virtual size_t getBytesConsumed () = 0;

        protected:

      }; /* class WorkQueue */

      ///
      /// \brief Defines the interfaces and operations for producer/consumer work queues.
      ///
      /// This version does not use virtual functions and defines the same byte interface
      ///
      ///
      class NonVirtWorkQueue : public _BaseWorkQueue
      {
        public:

          ///
          /// \brief Default constructor.
          ///
          /// Only one consumer of this work queue is allowed.
          ///
          inline NonVirtWorkQueue () : _BaseWorkQueue()
          {}

          ///
          /// \brief Clone constructor.
          ///
          /// Used to create a second work queue object identical to the input.
          ///
          /// \param[in] obj     Work queue object
          ///
          NonVirtWorkQueue (NonVirtWorkQueue & obj) : _BaseWorkQueue(obj)
          {}


#if 0
          ///
          /// \brief Multiple consumer constructor.
          ///
          /// Multiple consumers of this work queue is allowed.
          ///
          /// \param[in] consumers Number of consumers that will read from this work queue.
          ///
          inline NonVirtWorkQueue (unsigned consumers) : _BaseWorkQueue(consumers)
          {}
#endif
          ///
          /// \brief Virtual destructors make compilers happy.
          ///
          virtual ~NonVirtWorkQueue ();

        protected:

      }; /* class WorkQueue */

    }; /* namespace WorkQueue */
  }; /* namespace Device */
}; /* namespace PAMI */


#endif // __components_devices_workqueue_workqueue_h__
