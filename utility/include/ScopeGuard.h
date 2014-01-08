/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2007, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/*!
 * \file ScopeGuard.h
 * \brief \link bgq::utility::ScopeGuard ScopeGuard\endlink definition and implementation.
 */

#ifndef BGQ_UTILITY_SCOPE_GUARD_H_
#define BGQ_UTILITY_SCOPE_GUARD_H_

#include <boost/function.hpp>
#include <boost/utility.hpp>

namespace bgq {
namespace utility {

/*!
 * \brief Exception safe RAII wrapper around a functor
 *
 * Example usage:
 *
 * \include test/ScopeGuard/sample.cc
 */
class ScopeGuard : boost::noncopyable
{
private:
    boost::function<void()> _f;     //!< function pointer.
    bool _trigger;                  //!< true if we should call the functor in our dtor.

public:
    /*!
     * \brief Create a ScopeGuard from any callable entity.
     */
    template <typename F>
    explicit ScopeGuard(
            F f     //!< [in] function pointer to call in the destructor.
            ) : 
        _f(f),
        _trigger(true)
    {

    }

    /*!
     * \brief dtor
     */
    ~ScopeGuard() {
        if ( !_trigger ) return;

        // we cannot throw an exception from here, so use a try/catch block
        try {
            _f();
        } catch (...) {

        }
    }

    /*!
     * \brief release this guard
     */
    void dismiss() {
        _trigger = false;
    }
};

} // utility
} // bgq

#endif

