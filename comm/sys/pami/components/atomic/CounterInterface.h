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
 * \file components/atomic/CounterInterface.h
 * \brief ???
 */

#ifndef __components_atomic_CounterInterface_h__
#define __components_atomic_CounterInterface_h__

namespace PAMI
{
  namespace Counter
  {
    ///
    /// \brief Atomic counter interface class
    ///
    /// This is an interface class and may not be directly instantiated.
    ///
    /// \tparam T  Atomic counter implementation class
    ///
    template <class T>
    class Interface
    {
      protected:

        inline Interface() {}
        inline ~Interface() {}

      public:

        static const bool indirect = false;

        ///
        /// \brief Fetch the atomic counter object value
        ///
        /// \attention All atomic counter object derived classes \b must
        ///            implement the fetch_impl() method.
        ///
        /// \return Atomic counter object value
        ///
        inline size_t fetch();

        ///
        /// \brief Fetch, then increment the atomic counter object value
        ///
        /// \attention All atomic counter object derived classes \b must
        ///            implement the fetch_and_inc_impl() method.
        ///
        /// \return Atomic counter object value
        ///
        inline size_t fetch_and_inc();

        ///
        /// \brief Fetch, then decrement the atomic counter object value
        ///
        /// \attention All atomic counter object derived classes \b must
        ///            implement the fetch_and_dec_impl() method.
        ///
        /// \return Atomic counter object value
        ///
        inline size_t fetch_and_dec();

        ///
        /// \brief Fetch, then clear the atomic counter object value
        ///
        /// \attention All atomic counter object derived classes \b must
        ///            implement the fetch_and_clear_impl() method.
        ///
        /// \return Atomic counter object value
        ///
        inline size_t fetch_and_clear();

        ///
        /// \brief Clear the atomic counter object value
        ///
        /// \attention All atomic counter implementation classes \b must
        ///            implement the clear_impl() method.
        ///
        /// This is needed to make Mutexes deterministic on some implementations
        ///
        inline void clear();

    }; // PAMI::Counter::Interface class

    template <class T>
    inline size_t Interface<T>::fetch()
    {
      return static_cast<T*>(this)->fetch_impl();
    }

    template <class T>
    inline size_t Interface<T>::fetch_and_inc()
    {
      return static_cast<T*>(this)->fetch_and_inc_impl();
    }

    template <class T>
    inline size_t Interface<T>::fetch_and_dec()
    {
      return static_cast<T*>(this)->fetch_and_dec_impl();
    }

    template <class T>
    inline size_t Interface<T>::fetch_and_clear()
    {
      return static_cast<T*>(this)->fetch_and_clear_impl();
    }

    template <class T>
    inline void Interface<T>::clear()
    {
      static_cast<T*>(this)->clear_impl();
    }

  };  // PAMI::Counter namespace
};    // PAMI namespace

#endif // __components_atomic_CounterInterface_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
