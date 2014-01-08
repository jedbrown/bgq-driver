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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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
#ifndef RUNJOB_EXCEPTION_H
#define RUNJOB_EXCEPTION_H

#include "common/error.h"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>
#include <boost/exception/info.hpp>

#include <string>

/*!
 * \brief Log a message then throw an exception.
 */
#define LOG_RUNJOB_EXCEPTION(error, msg) { \
    LOG_ERROR_MSG( msg ); \
    runjob::Exception e_(error); \
    e_ << msg; \
    ::boost::throw_exception(::boost::enable_error_info(e_) << \
            ::boost::throw_function(BOOST_CURRENT_FUNCTION) << \
            ::boost::throw_file(__FILE__) << \
            ::boost::throw_line(static_cast<int>(__LINE__)) \
    ); \
}

namespace runjob {

/*!
 * \brief Generic exception class for transporting errors.
 */
class Exception : public std::exception
{
public:
    /*!
     * \brief ctor.
     */
    explicit Exception(
            error_code::rc error    //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Exception() throw() { }

    /*!
     * \brief copy ctor
     */
    Exception(
            const Exception& other  //!< [in]
            ) :
        std::exception(),
        _error( other._error ),
        _msg( other._msg )
    {

    }

    /*!
     * \brief Get a descriptive string about the exception.
     */
    const char* what() const throw();

    // getters
    error_code::rc getError() const { return _error; }  //!< Get the error_code.
    std::string& getMessage() { return _msg; }  //!< Get the message.

private:
    const error_code::rc _error;
    std::string _msg;
};

/*!
 * \brief insertion operator.
 */
template<typename T>
Exception&
operator<<(
        Exception& e,           //!< [in]
        const T& type           //!< [in]
        )
{
    e.getMessage().append( boost::lexical_cast<std::string>(type) );
    return e;
}

} // runjob

#endif

