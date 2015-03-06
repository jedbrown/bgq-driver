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
/// \file common/WakeupManagerInterface.h
/// \brief PAMI wakeup manager interface.
///
#ifndef __common_WakeupManagerInterface_h__
#define __common_WakeupManagerInterface_h__

#include <pami.h>

namespace PAMI {
namespace Interface {
        /// \brief WakeupManager class
        ///
        /// General usage is as follows: Typically only one vector per thread/context.
        /// The vector is registered with all work objects (e.g. PipeWorkQueues).
        /// In the advance loop, one pass is made to see if any work was done, and if
        /// no work happened then the thread will sleep on this vector. When other
        /// threads perform related work, they will wakeup this thread by calling
        /// wake() on the vector that was registered with the particular work object.
        /// It is up to the implementation to avoid scenarios where work units
        /// that depend on eachother are handled on the same context/thread.
        /// In such cases the work objects should not have a registered vector.
        /// Also, the case where no other contexts have work objects (that would
        /// wakeup this context) must be handled such that the sleep is not performed.
        ///
        /// Implementations must ensure that any wakeup before a call to sleep
        /// results in the sleep returning immediately (without sleeping).
        ///
        template <class T_WakeupManager>
        class WakeupManager {
        public:
                inline WakeupManager() {}

                /// \brief Initialize/allocate wakeup vectors
                ///
                /// \param[in] num	Number of vectors to create
                /// \param[in] key	Unique value to identify this set of vectors
                ///
                inline pami_result_t init(int num, int key);

                /// \brief Get wakeup vector based on index
                ///
                /// \param[in] idx	Wakeup vector index
                ///
                inline void *getWakeupVec(int idx);

                /// \brief Wakeup anyone awaiting on a vector
                ///
                /// \param[in] v	Wakeup vector to wake
                /// \return	PAMI_SUCCESS = wakeup sent
                ///		PAMI_ERROR = internal errors
                ///
                inline pami_result_t wakeup(void *v);

                /// \brief Reset a vector
                ///
                /// Resets a vector such that the next poll
                /// should indicate not ready (PAMI_EAGAIN).
                /// This is used to avoid repeated advance loops
                /// for wakeup conditions already being serviced.
                /// As an example, suppose other thread(s)/context(s)
                /// resulted in 5 units of work, each performing a wakeup.
                /// Instead of performing 5 iterations of the advance
                /// in this context, when one iteration is enough to
                /// perform the work, this clears the wakeup so that
                /// after one iteration of advance the loop will sleep.
                ///
                /// \param[in] v	Wakeup vector to clear
                /// \return	PAMI_SUCCESS = cleared
                ///		PAMI_ERROR = internal errors
                ///
                inline pami_result_t clear(void *v);

                /// \brief Wait for (sleep until) a wakeup
                ///
                /// Wakeup condition is cleared upon returnPAMI_SUCCESS
                /// (as if clear() called).
                ///
                /// \param[in] v	Wakeup vector to sleep on
                /// \return	PAMI_SUCCESS = woke up
                ///		PAMI_ERROR = internal errors
                ///
                inline pami_result_t sleep(void *v);

                /// \brief Check if wakeup has happened
                ///
                /// Wakeup condition is cleared upon return PAMI_SUCCESS
                /// (as if clear() called).
                ///
                /// \param[in] v	Wakeup vector to check
                /// \return	PAMI_SUCCESS = wakeup occurred
                ///		PAMI_EAGAIN = no wakeup (yet)
                ///		PAMI_ERROR = internal errors
                ///
                inline pami_result_t trySleep(void *v);

                /// \brief Check if wakeup has happened
                ///
                /// Requires subsequent trySleep() or sleep() in order to
                /// clear condition (if return was PAMI_SUCCESS).
                ///
                /// It is generally expected that only one thread
                /// would poll a wakeup vector. Otherwise, a race
                /// between pollers exits.
                ///
                /// \param[in] v	Wakeup vector to check
                /// \return	PAMI_SUCCESS = wakeup condition exists
                ///		PAMI_EAGAIN = no wakeup condition (yet)
                ///		PAMI_ERROR = internal errors
                ///
                inline pami_result_t poll(void *v);

        }; // class Interface::WakeupManager

        template <class T_WakeupManager>
        pami_result_t WakeupManager<T_WakeupManager>::init(int num, int key)
        {
            return static_cast<T_WakeupManager*>(this)->init_impl(num, key);
        }

        template <class T_WakeupManager>
        void *WakeupManager<T_WakeupManager>::getWakeupVec(int idx)
        {
            return static_cast<T_WakeupManager*>(this)->getWakeupVec_impl(idx);
        }

        template <class T_WakeupManager>
        pami_result_t WakeupManager<T_WakeupManager>::wakeup(void *v)
        {
            return static_cast<T_WakeupManager*>(this)->wakeup_impl(v);
        }

        template <class T_WakeupManager>
        pami_result_t WakeupManager<T_WakeupManager>::clear(void *v)
        {
            return static_cast<T_WakeupManager*>(this)->clear_impl(v);
        }

        template <class T_WakeupManager>
        pami_result_t WakeupManager<T_WakeupManager>::sleep(void *v)
        {
            return static_cast<T_WakeupManager*>(this)->sleep_impl(v);
        }

        template <class T_WakeupManager>
        pami_result_t WakeupManager<T_WakeupManager>::trySleep(void *v)
        {
            return static_cast<T_WakeupManager*>(this)->trySleep_impl(v);
        }

        template <class T_WakeupManager>
        pami_result_t WakeupManager<T_WakeupManager>::poll(void *v)
        {
            return static_cast<T_WakeupManager*>(this)->poll_impl(v);
        }

}; // namespace Interface
}; // namespace PAMI

#endif // __common_WakeupManagerInterface_h__
