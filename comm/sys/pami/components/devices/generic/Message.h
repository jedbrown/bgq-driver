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

#ifndef __components_devices_generic_Message_h__
#define __components_devices_generic_Message_h__

///  \file components/devices/generic/Message.h
///  \brief Generic Device base class for Messages
///

#include <pami.h>
#include "GenericDevicePlatform.h"
#include "common/MultisendInterface.h"

namespace PAMI {
namespace Device {

/// \brief Message Status
///
/// The only required status value is Done (and of course !Done).
/// When the message status is found to be Done (in advance), the
/// message will be dequeued and the completion callback invoked.
/// Also, the message's QS will be examined to see if another message
/// should be started.
///
enum MessageStatus {
        Uninitialized = 0,	///< status for uninitialized message
        Initialized,		///< status for initialized message
        Active,			///< status for active message
        Done			///< status for completed message
};

namespace Generic {

/// \brief Base Class for Messages
///
/// Messages must be able to exist on two queues at the same time.
/// This requires two actual QueueElem structs, handled by MultiQueueElem.
/// In fact, this class is templatized by number of queue elements and thus
/// is general for any number of queues.
///
/// Requires the typedef GenericDeviceMessageQueue for the queue
/// used to hold messages. This is a two-piece queue such that an
/// object may be queued two places at the same time.
///
class GenericMessage : public GenericDeviceMessageQueue::Element {
public:
        //////////////////////////////////////////////////////////////////////
        ///  \brief Constructor
        //////////////////////////////////////////////////////////////////////
        GenericMessage(GenericDeviceMessageQueue *QS, pami_callback_t cb,
                                                size_t client, size_t context) :
        GenericDeviceMessageQueue::Element(),
        _status(Uninitialized),
        _QS(QS),
        _client(client),
        _context(context),
        _cb(cb)
        {
        }

        virtual ~GenericMessage() {}

        /// \note This is required to make "C" programs link successfully with virtual destructors
        inline void operator delete(void * p) { (void)p;PAMI_abort(); }

        /// \brief get client associated with message
        /// \return	client for message posting/completion
        size_t getClientId() { return _client; }

        /// \brief get context ID associated with message
        /// \return	Context ID for message posting/completion
        size_t getContextId() { return _context; }

        ///  \brief Query function to determine message state
        ///  \return	message status
        ///
        inline MessageStatus getStatus() {return _status;}

        /// \brief Set message status
        ///
        /// \param[in] status	Message status to set
        ///
        inline void setStatus(MessageStatus status) {_status = status;}

        /// \brief     Returns the done status of the message
        ///
        /// \return	true is message is Done
        ///
        inline bool isDone() {return (getStatus() == Done);}

        ///  \brief Sets the message completion callback
        ///
        /// \param[in] cb	Callback to use for message completion
        ///
        void setCallback(pami_callback_t cb) {_cb = cb;}

        ///  \brief Executes the message completion callback
        ///
        /// \param[in] ctx	The context object on which completion is called
        /// \param[in] err	Optional error status (default is success)
        ///
        void executeCallback(pami_context_t ctx, pami_result_t err = PAMI_SUCCESS) {
                if(_cb.function) _cb.function(ctx, _cb.clientdata, err);
        }

        /// \brief accessor for sub-device linked to message
        ///
        /// Returns reference to device object which contains "send queues".
        /// This may not be the actual sub-device paired with the message.
        ///
        /// \return	Reference to sub-device
        ///
        inline GenericDeviceMessageQueue *getQS() { return _QS; }

        /// \brief virtual wrapper starting the next message.
        ///
        /// Used during message complete to start the next message.
        /// Each message class must implement the appropriate wrapper.
        /// This must be virtual since the actual implementation is specific
        /// to the device and message. Certain device base classes
        /// (SubDeviceSuppt.h) provide a __postNext() which this function
        /// may call directly.
        ///
        /// \param[in] devQueued	msg is already queued on device send queue
        /// \return	context on which message completed, or NULL
        ///
        virtual pami_context_t postNext(bool devQueued) = 0;

protected:
        MessageStatus _status;		///< current message status
        GenericDeviceMessageQueue *_QS; ///< send queue associated with message
        size_t _client;			///< client ID for message
        size_t _context;		///< context ID for message
        pami_callback_t _cb;		///< completion callback
}; /* class GenericMessage */
}; /* namespace Generic */
}; /* namespace Device */
}; /* namespace PAMI */

#endif /* __components_devices_generic_message_h__ */
