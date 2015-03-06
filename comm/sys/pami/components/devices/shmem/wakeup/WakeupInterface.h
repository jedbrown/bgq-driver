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
/// \file components/devices/shmem/wakeup/WakeupInterface.h
/// \brief ???
///
/// The wakeup component may be used to cause a process to pause execution
/// until a wakeup event is posted to the wakeup object. The definition of the
/// term 'pause' is implementation dependent.
///
/// A class that implements the wakeup interface may be used as a template
/// parameter to classes that code to the wakeup interface.
///
#ifndef __components_devices_shmem_wakeup_WakeupInterface_h__
#define __components_devices_shmem_wakeup_WakeupInterface_h__

#include <pami.h>

namespace PAMI
{
  namespace Wakeup
  {
    template <class T_Wakeup>
    class Interface
    {
      protected:

        inline Interface () {};

        inline Interface (T_Wakeup & clone) {};

        inline ~Interface () {};

      public:

        template <class T_Region>
        class Region
        {
          protected:

            template <typename T>
            inline T & array (size_t position)
            {
              return static_cast<T_Region*>(this)->operator[] (position);
            };

          public:

            ///
            /// \brief Wakeup region "clone" initializer interface
            ///
            /// \param [in] original Wakeup region to clone
            ///
            inline void init (T_Region & original);

            ///
            /// \brief Wakeup region allocation initializer interface
            ///
            /// \param [in] mm     Memory manager for wakeup region allocation
            /// \param [in] key    Unique key for memory manager allocation
            /// \param [in] count  Number of wakeup region elements
            /// \param [in] wakeup Wakeup parent for the wakeup region
            ///
            template <class T_MemoryManager>
            inline void init (T_MemoryManager * mm,
                              char            * key,
                              size_t            count,
                              T_Wakeup        * wakeup);
        };

        ///
        /// \brief Allocate and initialize an array of wakeup objects
        ///
        /// One wakeup object is created for each local task. For example, the
        /// first element in the allocated wakeup array is dedicated to peer 0.
        ///
        /// \see PAMI::Interface::Mapping::Node::nodePeers()
        /// \see PAMI::Interface::Mapping::Node::task2peer()
        ///
        /// \param [in] peers	Number of peer processes to manage
        ///
        static T_Wakeup * generate (size_t peers, const char * key);

        ///
        /// \brief Create a wakeup event to resume execution of any waiters
        ///
        inline pami_result_t poke ();

#if 0
        ///
        /// \brief Wait for (sleep until) a wakeup event
        ///
        /// The special "prepare to sleep" functor class is used to perform
        /// any work after the sleep is initialized, yet before the sleep
        /// is executed.
        ///
        /// Conceptually, the logic flow is:
        /// \code
        /// MyWakeup wu;
        /// wu.sleep(functor)
        /// -> initiateSleep()
        /// -> result = functor()
        /// -> if (result == eagain) completeSleep()
        /// \endcode
        ///
        /// \see PAMI::Interface::WakeupPrepare
        ///
        /// \tparam     T_Prepare Class that implements the WakeupPrepare interface
        /// \param [in] prepare   WakeupPrepare object
        ///
        template <class T_Prepare>
        inline pami_result_t sleep (T_Prepare & prepare)
        {
          return static_cast<T_Wakeup*>(this)->sleep_impl (prepare);
        };
#endif
    }; // class PAMI::Wakeup::Interface
#if 0
    template <class T_Prepare>
    class WakeupPrepare
    {
      public:

        ///
        /// \brief Perform any related work prior to the completion of the sleep
        ///
        /// \see PAMI::Interface::Wakeup::sleep()
        /// \see PAMI::Wakeup::Prepare::Noop
        ///
        /// \retval true  No work to do; ready to sleep
        /// \retval false Work was required; \b not ready to sleep
        ///
        inline bool operator()()
        {
          return static_cast<T_Prepare*>(this)->prepareToSleepFunctor_impl();
        }
    }; // class Interface::WakeupPrepare
#endif

    template <class T_Wakeup>
    template <class T_Region>
    void Interface<T_Wakeup>::Region<T_Region>::init (T_Region & original)
    {
      static_cast<T_Region*>(this)->init_impl(original);
    };

    template <class T_Wakeup>
    template <class T_Region>
    template <class T_MemoryManager>
    void Interface<T_Wakeup>::Region<T_Region>::init (T_MemoryManager * mm,
                                                      char            * key,
                                                      size_t            count,
                                                      T_Wakeup        * wakeup)
    {
      static_cast<T_Region*>(this)->init_impl(mm, key, count, wakeup);
    };

    template <class T_Wakeup>
    T_Wakeup * Interface<T_Wakeup>::generate (size_t peers, const char * key)
    {
      return T_Wakeup::generate_impl (peers, key);
    };

    template <class T_Wakeup>
    pami_result_t Interface<T_Wakeup>::poke ()
    {
      return static_cast<T_Wakeup*>(this)->poke_impl ();
    };
  }; // namespace PAMI::Wakeup
}; // namespace PAMI

#endif // __common_WakeupInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
