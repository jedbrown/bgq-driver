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
/// \file common/PipeWorkQueueInterface.h
/// \brief PAMI pipeworkqueue implementation.
///
#ifndef __common_PipeWorkQueueInterface_h__
#define __common_PipeWorkQueueInterface_h__

#include <pami.h>
#include "components/memory/MemoryManager.h"
#include "common/type/TypeCode.h"
#include "common/MultisendInterface.h"

namespace PAMI {
namespace Interface {

template <class T_PipeWorkQueue>
class PipeWorkQueue {
public:
	inline PipeWorkQueue() {}

	///
	/// \brief Clone constructor.
	///
	/// Used to create a second local memory wrapper object of the same
	/// shared memory resource.
	///
	/// \see WorkQueue(WorkQueue &)
	///
	/// \param[in] obj	Shared work queue object
	///
	PipeWorkQueue(T_PipeWorkQueue &obj) {}

	///
	/// \brief Virtual destructors make compilers happy.
	///
	~PipeWorkQueue() { }

	///
	/// \brief Configure for Shared Circular Buffer variety.
	///
	/// Only one consumer and producer are allowed.
	/// Creates a circular buffer of specified size in shared memory.
	/// Buffer size must be power-of-two.
	///
	/// \param[in] mm	Memory to use
	/// \param[in] key	Unique id for memory allocation
	/// \param[in] bufsize	Size of data buffer to allocate
	///
	inline void configure(PAMI::Memory::MemoryManager *mm, const char *key, size_t bufsize);

	///
	/// \brief Configure for User-supplied Circular Buffer variety.
	///
	/// Only one consumer and producer are allowed.
	/// Uses the provided buffer as a circular buffer of specified size.
	/// Buffer size must be power-of-two.
	/// Assumes the caller has placed buffer and (this) in appropriate memory
	/// for desired use - i.e. all in shared memory if to be used beyond this process.
	///
	/// \param[in] buffer	Buffer to use
	/// \param[in] bufsize	Size of buffer
	///
	inline void configure(char *buffer, size_t bufsize);

	///
	/// \brief Configure for Memeory (flat buffer) variety.
	///
	/// Only one consumer and producer are allowed. Still supports pipelining.
	/// Sets up a flat buffer of specified maximum size with an arbitrary "initial fill".
	/// Assumes the caller has placed buffer and (this) in appropriate memory
	/// for desired use - i.e. all in shared memory if to be used beyond this process.
	///
	/// \param[in] mm	System dependent methods
	/// \param[in] buffer	Buffer to use
	/// \param[in] bufsize	Size of buffer
	/// \param[in] bufinit	Amount of data initially in buffer
	///
	inline void configure(char *buffer, size_t bufsize, size_t bufinit,
				PAMI::Type::TypeCode *prod_dt = NULL,
				PAMI::Type::TypeCode *cons_dt = NULL);


	///
	/// \brief Reset this shared memory work queue.
	///
	/// Sets the number of bytes produced and the number of bytes
	/// consumed by each consumer to zero.
	///
	inline void reset();
	inline void reset_nosync();
	inline void barrier_reset(unsigned participants, bool master);

	///
	/// \brief Dump shared memory work queue statistics to stderr.
	///
	/// \param[in] prefix	Optional character string to prefix.
	///
	inline void dump(const char *prefix = NULL);

	///
	/// \brief Export
	///
	/// Produces information about the PipeWorkQueue into the opaque buffer "export".
	/// This info is suitable for sharing with other processes such that those processes
	/// can then construct a PipeWorkQueue which accesses the same data stream.
	///
	/// \param[in] wq	Opaque memory for PipeWorkQueue
	/// \param[out] export	Opaque memory to export into
	/// \return	success of the export operation
	///
	inline pami_result_t exp(pami_pipeworkqueue_ext_t *exp);

	///
	/// \brief Import
	///
	/// Takes the results of an export of a PipeWorkQueue on a different process and
	/// constructs a new PipeWorkQueue which the local process may use to access the
	/// data stream.
	///
	/// The resulting PipeWorkQueue may consume data, but that is a local-only operation.
	/// The producer has no knowledge of data consumed. There can be only one producer.
	/// There may be multiple consumers, but the producer does not know about them.
	///
	/// TODO: can this work for circular buffers? does it need to, since those are
	/// normally shared memory and thus already permit inter-process communication.
	///
	/// \param[in] import	Opaque memory into which an export was done.
	/// \param[out] wq	Opaque memory for new PipeWorkQueue
	/// \return	success of the import operation
	///
	inline pami_result_t import(pami_pipeworkqueue_ext_t *import);

	/// \brief register a wakeup for the consumer side of the PipeWorkQueue
	///
	/// \param[in] vec	Opaque wakeup vector parameter
	///
	inline void setConsumerWakeup(void *vec);

	/// \brief register a wakeup for the producer side of the PipeWorkQueue
	///
	/// \param[in] vec	Opaque wakeup vector parameter
	///
	inline void setProducerWakeup(void *vec);

	/// \brief register user-defined info for consumer
	///
	/// Cooperating consumer/producers may share information using
	/// these interfaces. The information is static, i.e. after a
	/// call to setConsumerUserInfo() with values A and B, all subsequent
	/// calls to getConsumerUserInfo() will return values A and B
	/// (until another call to setConsumerUserInfo() is made).
	///
	/// "Producer" and "Consumer" here are just convenience references,
	/// either side (or potentially third-parties) may use these
	/// methods as desired to set/get the info.
	///
	/// \param[in] word1	First piece of info
	/// \param[in] word2	Second piece of info
	inline void setConsumerUserInfo(void *word1, void *word2);

	/// \brief register user-defined info for producer
	/// \param[in] word1	First piece of info
	/// \param[in] word2	Second piece of info
	inline void setProducerUserInfo(void *word1, void *word2);

	/// \brief get user-defined info for consumer
	/// \param[in] word1	First piece of info
	/// \param[in] word2	Second piece of info
	inline void getConsumerUserInfo(void **word1, void **word2);

	/// \brief register user-defined info for producer
	/// \param[in] word1	First piece of info
	/// \param[in] word2	Second piece of info
	inline void getProducerUserInfo(void **word1, void **word2);

	///
	/// \brief Return the maximum number of bytes that can be produced into this work queue.
	///
	/// Bytes must be produced into the memory location returned by bufferToProduce() and then
	/// this work queue \b must be updated with produceBytes().
	///
	/// The number of bytes that may be produced is calculated by determining
	/// the difference between the \b minimum number of bytes consumed by all
	/// consumers and the number of bytes produced.
	///
	/// This must only be called with serious intent to produce! If this
	/// routine returns non-zero, the caller MUST produceBytes() in a timely
	/// manner. Never returns more than one packet.
	///
	/// \see bufferToProduce
	/// \see produceBytes
	///
	/// \return Number of bytes that may be produced.
	///
	inline size_t bytesAvailableToProduce();

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
	inline size_t bytesAvailableToConsume();

	/// \brief raw accessor for total number of bytes produced since reset()
	///
	/// \return number of bytes produced
	///
	inline size_t getBytesProduced();

	/// \brief raw accessor for total number of bytes consumed since reset()
	///
	/// \return number of bytes consumed
	///
	inline size_t getBytesConsumed();

	/// \brief current position for producing into buffer
	///
	/// \return location in buffer to produce into
	///
	inline char *bufferToProduce();

	/// \brief notify workqueue that bytes have been produced
	///
	/// \param[in] bytes	Number of bytes that were produced
	///
	inline void produceBytes(size_t bytes);

	/// \brief current position for consuming from buffer
	///
	/// \return location in buffer to consume from
	///
	inline char *bufferToConsume();

	/// \brief notify workqueue that bytes have been consumed
	///
	/// \param[in] bytes	Number of bytes that were consumed
	///
	inline void consumeBytes(size_t bytes);

	/// \brief is workqueue ready for action
	///
	/// \return	boolean indicate workqueue readiness
	///
	inline bool available();

}; // end class PAMI::Pipeworkqueue::Pipeworkqueue

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::configure(PAMI::Memory::MemoryManager *mm,
						const char *key, size_t bufsize) {
	return static_cast<T_PipeWorkQueue *>(this)->configure_impl(mm, key, bufsize);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::reset() {
        return static_cast<T_PipeWorkQueue *>(this)->reset_impl();
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::reset_nosync() {
        return static_cast<T_PipeWorkQueue *>(this)->reset_nosync_impl();
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::barrier_reset(unsigned participants, bool master) {
	return static_cast<T_PipeWorkQueue *>(this)->barrier_reset_impl(participants, master);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::configure(char *buffer, size_t bufsize) {
	return static_cast<T_PipeWorkQueue *>(this)->configure_impl(buffer, bufsize);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::configure(char *buffer, size_t bufsize, size_t bufinit,
				PAMI::Type::TypeCode *prod_dt,
				PAMI::Type::TypeCode *cons_dt) {
	return static_cast<T_PipeWorkQueue *>(this)->configure_impl(buffer, bufsize, bufinit, prod_dt, cons_dt);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::dump(const char *prefix) {
	return static_cast<T_PipeWorkQueue *>(this)->dump_impl(prefix);
}

template <class T_PipeWorkQueue>
pami_result_t PipeWorkQueue<T_PipeWorkQueue>::exp(pami_pipeworkqueue_ext_t *exp) {
	return static_cast<T_PipeWorkQueue *>(this)->exp_impl(exp);
}

template <class T_PipeWorkQueue>
pami_result_t PipeWorkQueue<T_PipeWorkQueue>::import(pami_pipeworkqueue_ext_t *import) {
	return static_cast<T_PipeWorkQueue *>(this)->import_impl(import);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::setConsumerWakeup(void *vec) {
	return static_cast<T_PipeWorkQueue *>(this)->setConsumerWakeup_impl(vec);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::setProducerWakeup(void *vec) {
	return static_cast<T_PipeWorkQueue *>(this)->setProducerWakeup_impl(vec);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::setConsumerUserInfo(void *word1, void *word2) {
	static_cast<T_PipeWorkQueue *>(this)->setConsumerUserInfo_impl(word1, word2);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::setProducerUserInfo(void *word1, void *word2) {
	static_cast<T_PipeWorkQueue *>(this)->setProducerUserInfo_impl(word1, word2);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::getConsumerUserInfo(void **word1, void **word2) {
	static_cast<T_PipeWorkQueue *>(this)->getConsumerUserInfo_impl(word1, word2);
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::getProducerUserInfo(void **word1, void **word2) {
	static_cast<T_PipeWorkQueue *>(this)->getProducerUserInfo_impl(word1, word2);
}

template <class T_PipeWorkQueue>
size_t PipeWorkQueue<T_PipeWorkQueue>::bytesAvailableToProduce() {
	return static_cast<T_PipeWorkQueue *>(this)->bytesAvailableToProduce_impl();
}

template <class T_PipeWorkQueue>
size_t PipeWorkQueue<T_PipeWorkQueue>::bytesAvailableToConsume() {
	return static_cast<T_PipeWorkQueue *>(this)->bytesAvailableToConsume_impl();
}

template <class T_PipeWorkQueue>
size_t PipeWorkQueue<T_PipeWorkQueue>::getBytesProduced() {
	return static_cast<T_PipeWorkQueue *>(this)->getBytesProduced_impl();
}

template <class T_PipeWorkQueue>
size_t PipeWorkQueue<T_PipeWorkQueue>::getBytesConsumed() {
	return static_cast<T_PipeWorkQueue *>(this)->getBytesConsumed_impl();
}

template <class T_PipeWorkQueue>
char *PipeWorkQueue<T_PipeWorkQueue>::bufferToProduce() {
	return static_cast<T_PipeWorkQueue *>(this)->bufferToProduce_impl();
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::produceBytes(size_t bytes) {
	return static_cast<T_PipeWorkQueue *>(this)->produceBytes_impl(bytes);
}

template <class T_PipeWorkQueue>
char *PipeWorkQueue<T_PipeWorkQueue>::bufferToConsume() {
	return static_cast<T_PipeWorkQueue *>(this)->bufferToConsume_impl();
}

template <class T_PipeWorkQueue>
void PipeWorkQueue<T_PipeWorkQueue>::consumeBytes(size_t bytes) {
	return static_cast<T_PipeWorkQueue *>(this)->consumeBytes_impl(bytes);
}

template <class T_PipeWorkQueue>
bool PipeWorkQueue<T_PipeWorkQueue>::available() {
	return static_cast<T_PipeWorkQueue *>(this)->available_impl();
}

}; // end namespace Interface
}; // end namespace PAMI

#endif // __pami_component_pipeworkqueue_h__

//
// astyle info	http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments
// astyle options --max-instatement-indent=79 --min-conditional-indent=2
// astyle options --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements
// astyle options --align-pointer=name --lineend=linux
//
