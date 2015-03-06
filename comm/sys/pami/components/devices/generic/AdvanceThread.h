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

#ifndef __components_devices_generic_AdvanceThread_h__
#define __components_devices_generic_AdvanceThread_h__

#include "GenericDevicePlatform.h"
#include "components/devices/BaseDevice.h"
#include "components/devices/generic/Message.h"
#include "components/memory/MemoryManager.h"

////////////////////////////////////////////////////////////////////////
///  \file components/devices/generic/AdvanceThread.h
///  \brief Generic Device Threads implementation
///
////////////////////////////////////////////////////////////////////////
namespace PAMI {
namespace Device {

        /// \brief Thread Status
        ///
        /// The work function of a thread will only be called if the
        /// status is Ready or OneShot. In the case of OneShot, the
        /// thread will be dequeued first. In the case of Ready, the
        /// return value of the function determines whether the thread
        /// is dequeued. PAMI_EAGAIN keeps the thread on the queue for
        /// subsequent advance calls. Any other value causes the thread
        /// to be dequeue, and PAMI_SUCCESS indicates successful completion.
        ///
        enum ThreadStatus {
                New = 0,  ///< Thread has only been constructed (do not call)
                Idle,   ///< Thread has no work (do not call)
                Ready,    ///< Thread has work, make call to func
                Complete  ///< Thread should be dequeued (do not call)
        };

namespace Generic {

/// \brief Base class for all thread objects posted to generic device
///
/// All objects posted to the generic device via postThread() must
/// inherit from this class.
///
/// This is the basic thread object. It describes a unit of work to be performed
/// during advance. Ctors allow thread to be created empty/idle or
/// with a function and ready to run.
///
class GenericThread : public PAMI::Queue::Element {
public:
        /// \brief Default constructor for thread
        /// \ingroup use_gendev
        ///
        /// Constructs a thread object that is not ready to run.
        ///
        GenericThread() :
        PAMI::Queue::Element(),
        _func(NULL),
        _cookie(NULL),
        _status(New)
        {
        }

        /// \brief Work constructor for thread
        ///
        /// Constructs a thread object that is ready to run,
        /// complete with work function.
        ///
        /// \param[in] func Work function to call
        /// \param[in] cookie Opaque data for work function
        ///
        GenericThread(pami_work_function func, void *cookie) :
        PAMI::Queue::Element(),
        _func(func),
        _cookie(cookie),
        _status(Ready)
        {
        }

        /// \brief Execute work function for thread
        ///
        /// Calls the work function with the context object and opaque data.
        ///
        /// \param[in] context  Context in which thread is being executed
        ///
        inline pami_result_t executeThread(pami_context_t context) {
                return _func(context, _cookie);
        }

        /// \brief Accessor for thread status
        ///
        /// \return Thread status
        ///
        inline ThreadStatus getStatus() { return _status; }

        /// \brief Set thread status
        ///
        /// \param[in] stat New status for thread
        ///
        inline void setStatus(ThreadStatus stat) { _status = stat; }

        /// \brief Set thread work function and opaque data
        ///
        /// Status must still be set to Ready or OneShot in order for
        /// function to be called.
        ///
        /// \param[in] func Function to call from advance
        /// \param[in] cookie Opaque data to pass to function
        ///
        inline void setFunc(pami_work_function func, void *cookie) {
                _func = func;
                _cookie = cookie;
        }
protected:
        pami_work_function _func; ///< work function to call
        void *_cookie;     ///< opaque value to pass to work function
        ThreadStatus _status;  ///< current thread status
}; // class GenericThread

}; /* namespace Generic */
}; /* namespace Device */
}; /* namespace PAMI */

#endif /* __components_devices_generic_thread_h__ */

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=java --indent=force-tab=8 --indent-preprocessor
// astyle options --indent-col1-comments --max-instatement-indent=79
// astyle options --min-conditional-indent=2 --pad-oper --unpad-paren
// astyle options --pad-header --add-brackets --keep-one-line-blocks
// astyle options --keep-one-line-statements --align-pointer=name --lineend=linux
//
