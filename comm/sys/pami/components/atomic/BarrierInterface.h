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
#ifndef __components_atomic_BarrierInterface_h__
#define __components_atomic_BarrierInterface_h__

////////////////////////////////////////////////////////////////////////
///  \file components/atomic/BarrierInterface.h
///  \brief Barrier Objects for Hardware and Software Barrieres
///
///  This object is a portability layer that abstracts local barriers
///  - Access to the hardware barriers
///  - Access to the software barriers
///  - Enter methods provided
///  - Allocation/Deallocation handled by constructor/destructor
///
///  Definitions:
///  - Hardware Barrier:  a barrier that is assisted by hardware
///  - Software Barrier, ie an atomic memory barrier
///
///  Namespace:  PAMI, the messaging namespace
///  Notes:  This is currently indended for use only by the lock manager
///
////////////////////////////////////////////////////////////////////////

/// \page pami_atomic_examples
///
/// How to use a Barrier atomic object:
///
/// // following changes with desired type of barrier...
/// #include "collectives/components/atomic/bgp/LockBoxBarrier.h"
/// typedef PAMI::Barrier::BGP::LockBoxNodeCoreBarrier MY_BARRIER;
///
/// MY_BARRIER _barrier;
/// pami_result_t status;
/// _barrier.init(sysdep, status); // get 'sysdep' wherever you can...
/// PAMI_assert(status == PAMI_SUCCESS);
///
/// _barrier.enter(); // perform actual barrier
///
///
namespace PAMI
{
  namespace Barrier
  {
    ///
    /// \brief Barrier object interface
    ///
    /// This is an interface class and may not be directly instantiated.
    ///
    /// \param T  Barrier interface implementation class
    ///
    template <class T>
    class Interface
    {
      private:

        inline Interface () {};

      protected:

        ///
        /// \brief  Construct a barrier
        ///
        /// \param [in] participants Total number of participants in the barrier
        /// \param [in] master       Caller is the master participant of the barrier
        ///
        inline Interface (size_t participants, bool master) {};

        inline ~Interface () {};

      public:

        static const bool indirect = false;

        ///
        /// \brief Enter a local blocking barrier operation
        ///
        /// Does not return until all participants have entered the barrier.
        ///
        /// Semantically equivalent to the following:
        ///
        /// \code
        /// MyBarrier barrier;
        /// barrier.init(...);
        ///
        /// barrier.begin();
        /// while (barrier.poll());
        /// barier.end();
        /// \endcode
        ///
        inline void enter ();

        ///
        /// \brief  Enter a blocking barrier operation
        ///
        /// Does not return until all participants have entered the barrier.
        ///
        /// The functor object is invoked for each internal poll of the
        /// barrier resource.
        ///
        /// A "functor", or "function object", is simply a class that
        /// implements the '()' operator.
        ///
        /// http://en.wikipedia.org/wiki/Function_object
        /// http://en.wikipedia.org/wiki/Function_object#Performance
        /// \par
        ///      "An advantage of function objects in C++ is performance
        ///       because, unlike a function pointer, a function object can
        ///       be inlined."
        ///
        template <class T_Functor>
        inline void enter (T_Functor & functor);

        ///
        /// \brief Begin a non-blocking barrier operation and return.
        ///
        inline void begin ();

        ///
        /// \brief  Poll an initialized non-blocking barrier
        ///
        /// \retval true  After the poll the barrier is active
        /// \retval false After the poll the barrier is now complete
        ///
        inline bool poll ();

        ///
        /// \brief End a non-blocking barrier operation and return.
        ///
        inline void end ();

        ///
        /// \brief  Debug routine to dump state of a barrier
        ///
        inline void dump (const char * string = NULL);

    }; // class PAMI::Barrier::Interface<T>

    template <class T>
    inline void Interface<T>::enter ()
    {
      static_cast<T*>(this)->enter_impl();
    }

    template <class T>
    template <class T_Functor>
    inline void Interface<T>::enter (T_Functor & functor)
    {
      static_cast<T*>(this)->enter_impl(functor);
    }

    template <class T>
    inline void Interface<T>::begin ()
    {
      static_cast<T*>(this)->begin_impl();
    }

    template <class T>
    inline bool Interface<T>::poll ()
    {
      return static_cast<T*>(this)->poll_impl();
    }

    template <class T>
    inline void Interface<T>::end ()
    {
      static_cast<T*>(this)->end_impl();
    }

    template <class T>
    inline void Interface<T>::dump (const char *string)
    {
      static_cast<T*>(this)->dump_impl(string);
    }
  }; // namespace Barrier
}; // namespace PAMI

#endif // __components_atomic_BarrierInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
