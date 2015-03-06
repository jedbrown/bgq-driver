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
/// \file common/M2MPipeWorkQueueInterface.h
/// \brief PAMI pipeworkqueue implementation.
///
#ifndef __common_M2MPipeWorkQueueInterface_h__
#define __common_M2MPipeWorkQueueInterface_h__

#include <pami.h>
#include "components/memory/MemoryManager.h"
#include "common/MultisendInterface.h"


namespace PAMI {
namespace Interface {

template <class T_PipeWorkQueue>
class M2MPipeWorkQueue {
public:
	inline M2MPipeWorkQueue() {}

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
	M2MPipeWorkQueue(T_PipeWorkQueue &obj) {}

	///
	/// \brief Virtual destructors make compilers happy.
	///
	~M2MPipeWorkQueue() { }

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
	/// \param[out] unused        REMOVE? Consistent with PAMI::PipeWorkQueue?
	/// \param[in]  buffer        Buffer to use
	/// \param[in]  indexcount    Number of indexed access points to the pwq
	/// \param[in]  dgsp          Memory layout of each buffer unit
	/// \param[in]  offsets       Array[indexcount] of byte offsets for each indexed access point
	/// \param[in]  dgspcounts    Array[indexcount] of data type (dgsp) counts
	/// \param[in]  bufinit       Array[indexcount] of bytes initially in buffer (storage may be modified)
	///
	/// \note bufinit must be empty (0) for producer PWQ or full (size of dgsp type * dsgpcounts[index]) for consumer PWQ.
	///
	inline void configure(char *buffer, size_t indexcount,
			      pami_type_t *dgsp, void *offsets, void *dgspcounts, void *bufinit);

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
	/// \param[out] unused        REMOVE? Consistent with PAMI::PipeWorkQueue?
	/// \param[in]  buffer        Buffer to use
	/// \param[in]  indexcount    Number of indexed access points to the pwq (bufinit array only)
	/// \param[in]  dgsp          Memory layout of each buffer unit
	/// \param[in]  offset        Byte offset for each indexed access point
	/// \param[in]  dgspcount     Data type (dgsp) countfor each indexed access point
	/// \param[in]  bufinit       Array[indexcount] of bytes initially in buffer (storage may be modified)
	///
	/// \note bufinit must be empty (0) for producer PWQ or full (size of dgsp type * dsgpcounts[index]) for consumer PWQ.
	///
	inline void configure(char *buffer, size_t indexcount,
			      pami_type_t *dgsp, size_t offset, size_t dgspcount, size_t *bufinit);
	///
	/// \brief Reset this shared memory work queue.
	///
	/// Sets the number of bytes produced and the number of bytes
	/// consumed by each consumer to zero.
	///
	inline void reset();
	inline void barrier_reset(unsigned participants, bool master);

	///
	/// \brief Dump shared memory work queue statistics to stderr.
	///
	/// \param[in] prefix Optional character string to prefix.
	///
	inline void dump(const char *prefix = NULL);

	///
	/// \brief Export
	///
	/// Produces information about the PipeWorkQueue into the opaque buffer "export".
	/// This info is suitable for sharing with other processes such that those processes
	/// can then construct a PipeWorkQueue which accesses the same data stream.
	///
	/// \param[in] wq             Opaque memory for PipeWorkQueue
	/// \param[out] export        Opaque memory to export into
	/// \return   success of the export operation
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
	/// \param[in] import        Opaque memory into which an export was done.
	/// \param[out] wq           Opaque memory for new PipeWorkQueue
	/// \return   success of the import operation
	///
	inline pami_result_t import(pami_pipeworkqueue_ext_t *import);

	/// \brief register a wakeup for the consumer side of the PipeWorkQueue
	///
	/// \param[in] vec  Opaque wakeup vector parameter
	///
	inline void setConsumerWakeup(void *vec);

	/// \brief register a wakeup for the producer side of the PipeWorkQueue
	///
	/// \param[in] vec  Opaque wakeup vector parameter
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
	/// \param[in] word1  First piece of info
	/// \param[in] word2  Second piece of info
	inline void setConsumerUserInfo(void *word1, void *word2);

	/// \brief register user-defined info for producer
	/// \param[in] word1  First piece of info
	/// \param[in] word2  Second piece of info
	inline void setProducerUserInfo(void *word1, void *word2);

	/// \brief get user-defined info for consumer
	/// \param[in] word1  First piece of info
	/// \param[in] word2  Second piece of info
	inline void getConsumerUserInfo(void **word1, void **word2);

	/// \brief register user-defined info for producer
	/// \param[in] word1  First piece of info
	/// \param[in] word2  Second piece of info
	inline void getProducerUserInfo(void **word1, void **word2);

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
	inline size_t bytesAvailableToProduce(size_t index);

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
	inline size_t bytesAvailableToConsume(size_t index);

	/// \brief raw accessor for total number of bytes produced since reset()
	///
	/// \param[in] index of access point
	///
	/// \return number of bytes produced
	///
	inline size_t getBytesProduced(size_t index);

	/// \brief raw accessor for total number of bytes consumed since reset()
	///
	/// \param[in] index of access point
	///
	/// \return number of bytes consumed
	///
	inline size_t getBytesConsumed(size_t index);

	/// \brief current position for producing into buffer
	///
	/// \param[in] index of access point
	///
	/// \return location in buffer to produce into
	///
	inline char *bufferToProduce(size_t index);

	/// \brief notify workqueue that bytes have been produced
	///
	/// \param[in] index of access point
	/// \param[in] number of bytes that were produced
	///
	inline void produceBytes(size_t index, size_t bytes);

	/// \brief current position for consuming from buffer
	///
	/// \param[in] index of access point
	///
	/// \return location in buffer to consume from
	///
	inline char *bufferToConsume(size_t index);

	/// \brief notify workqueue that bytes have been consumed
	///
	/// \param[in] index of access point
	/// \param[in] number of bytes that were consumed
	///
	inline void consumeBytes(size_t index, size_t bytes);

	inline char   * getBufferBase (size_t index);
	
	inline size_t   getTotalBytes (size_t index);


	/// \brief current position for producing into buffer
	/// \brief is workqueue ready for action
	///
	/// \return boolean indicate workqueue readiness
	///
	inline bool available();

	/// \brief Number of indices
	///
	///  \return number of indices
	inline 	size_t numIndices ();

	/// \brief In an alltoallv for example several indices can be
	///  zero. Number of active indices is the number of nonzero indices. In an
	///  alltoall or a fixed sized M2mPipeWorkQueue this is the same as the
	///  index count.
	///
	///  \return number of active indices
	inline 	size_t numActive ();

}; // end class PAMI::Pipeworkqueue::M2MPipeworkqueue

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::reset() {
	return static_cast<T_PipeWorkQueue *>(this)->reset_impl();
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::barrier_reset(unsigned participants, bool master) {
	return static_cast<T_PipeWorkQueue *>(this)->barrier_reset_impl(participants, master);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::configure(char *buffer, size_t indexcount,
						pami_type_t *dgsp, void *offsets,
						void *dgspcounts, void *bufinit) {
	return static_cast<T_PipeWorkQueue *>(this)->configure_impl(buffer, indexcount, dgsp, offsets,
								    dgspcounts, bufinit);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::configure(char *buffer, size_t indexcount,
						pami_type_t *dgsp, size_t offset,
						size_t dgspcount, size_t *bufinit) {
	return static_cast<T_PipeWorkQueue *>(this)->configure_impl(buffer, indexcount, dgsp, offset,
								    dgspcount, bufinit);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::dump(const char *prefix) {
	return static_cast<T_PipeWorkQueue *>(this)->dump_impl(prefix);
}

template <class T_PipeWorkQueue>
pami_result_t M2MPipeWorkQueue<T_PipeWorkQueue>::exp(pami_pipeworkqueue_ext_t *exp) {
	return static_cast<T_PipeWorkQueue *>(this)->exp_impl(exp);
}

template <class T_PipeWorkQueue>
pami_result_t M2MPipeWorkQueue<T_PipeWorkQueue>::import(pami_pipeworkqueue_ext_t *import) {
	return static_cast<T_PipeWorkQueue *>(this)->import_impl(import);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::setConsumerWakeup(void *vec) {
	return static_cast<T_PipeWorkQueue *>(this)->setConsumerWakeup_impl(vec);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::setProducerWakeup(void *vec) {
	return static_cast<T_PipeWorkQueue *>(this)->setProducerWakeup_impl(vec);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::setConsumerUserInfo(void *word1, void *word2) {
	static_cast<T_PipeWorkQueue *>(this)->setConsumerUserInfo_impl(word1, word2);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::setProducerUserInfo(void *word1, void *word2) {
	static_cast<T_PipeWorkQueue *>(this)->setProducerUserInfo_impl(word1, word2);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::getConsumerUserInfo(void **word1, void **word2) {
	static_cast<T_PipeWorkQueue *>(this)->getConsumerUserInfo_impl(word1, word2);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::getProducerUserInfo(void **word1, void **word2) {
	static_cast<T_PipeWorkQueue *>(this)->getProducerUserInfo_impl(word1, word2);
}

template <class T_PipeWorkQueue>
size_t M2MPipeWorkQueue<T_PipeWorkQueue>::bytesAvailableToProduce(size_t index) {
	return static_cast<T_PipeWorkQueue *>(this)->bytesAvailableToProduce_impl(index);
}

template <class T_PipeWorkQueue>
size_t M2MPipeWorkQueue<T_PipeWorkQueue>::bytesAvailableToConsume(size_t index) {
	return static_cast<T_PipeWorkQueue *>(this)->bytesAvailableToConsume_impl(index);
}

template <class T_PipeWorkQueue>
size_t M2MPipeWorkQueue<T_PipeWorkQueue>::getBytesProduced(size_t index) {
	return static_cast<T_PipeWorkQueue *>(this)->getBytesProduced_impl(index);
}

template <class T_PipeWorkQueue>
size_t M2MPipeWorkQueue<T_PipeWorkQueue>::getBytesConsumed(size_t index) {
	return static_cast<T_PipeWorkQueue *>(this)->getBytesConsumed_impl(index);
}

template <class T_PipeWorkQueue>
char *M2MPipeWorkQueue<T_PipeWorkQueue>::bufferToProduce(size_t index) {
	return static_cast<T_PipeWorkQueue *>(this)->bufferToProduce_impl(index);
}

template <class T_PipeWorkQueue>
void M2MPipeWorkQueue<T_PipeWorkQueue>::produceBytes(size_t index, size_t bytes) {
	return static_cast<T_PipeWorkQueue *>(this)->produceBytes_impl(index, bytes);
}

template <class T_PipeWorkQueue>
char *M2MPipeWorkQueue<T_PipeWorkQueue>::bufferToConsume(size_t index) {
	return static_cast<T_PipeWorkQueue *>(this)->bufferToConsume_impl(index);
}

template <class T_PipeWorkQueue>
char* M2MPipeWorkQueue<T_PipeWorkQueue>::getBufferBase(size_t index) {
	return static_cast<T_PipeWorkQueue *>(this)->getBufferBase_impl(index);
}

template <class T_PipeWorkQueue>
size_t M2MPipeWorkQueue<T_PipeWorkQueue>::getTotalBytes(size_t index) {
	return static_cast<T_PipeWorkQueue *>(this)->getTotalBytes_impl(index);
}

template <class T_PipeWorkQueue>
bool M2MPipeWorkQueue<T_PipeWorkQueue>::available() {
	return static_cast<T_PipeWorkQueue *>(this)->available_impl();
}

template <class T_PipeWorkQueue>
size_t M2MPipeWorkQueue<T_PipeWorkQueue>::numIndices() {
	return static_cast<T_PipeWorkQueue *>(this)->numIndices_impl();
}

template <class T_PipeWorkQueue>
size_t M2MPipeWorkQueue<T_PipeWorkQueue>::numActive() {
	return static_cast<T_PipeWorkQueue *>(this)->numActive_impl();
}

}; // end namespace Interface
}; // end namespace PAMI

#endif // __common_M2MPipeWorkQueueInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments
// astyle options --max-instatement-indent=79 --min-conditional-indent=2
// astyle options --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements
// astyle options --align-pointer=name --lineend=linux
//
