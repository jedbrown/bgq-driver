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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#ifndef MMCS_SERVER_ENV_TOKEN
#define MMCS_SERVER_ENV_TOKEN

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace mmcs {
namespace server {
namespace env {

/*!
 * \brief RAII implementation for a callback.
 *
 * This class is expected to be owned by a shared_ptr, its destructor will invoke 
 * the provided callback.
 *
 * \see NodeBoard
 * \see Optical
 */
class Token : private boost::noncopyable
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Token> Ptr;

    /*!
     * \brief
     */
    typedef boost::function<void()> Callback;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            const std::string& name,    //!< [in] descriptive name for logging purposes
            const Callback& callback    //!< [in] callback to invoke upon destruction
            );

    /*!
     * \brief dtor.
     */
    ~Token();

private:
    /*!
     * \brief ctor.
     */
    Token(
            const std::string& name,
            const Callback& callback
         );

private:
    const std::string _name;
    const Callback _callback;
};

} // env
} // server
} // mmcs

#endif
