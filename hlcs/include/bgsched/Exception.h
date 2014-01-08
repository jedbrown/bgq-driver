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

/*!
 * \file bgsched/Exception.h
 * \brief ExceptionInfo class definition.
 * \ingroup Exceptions
 * \defgroup Exceptions Exception grouping
 */

#ifndef BGSCHED_EXCEPTION_H_
#define BGSCHED_EXCEPTION_H_

#include <bgsched/EnumWrapper.h>

#include <boost/lexical_cast.hpp>

#include <stdexcept>

namespace bgsched {

/*!
 * \addtogroup Exceptions
 * @{
 */

/*!
 * \brief Abstract exception class with configurable behavior via policy classes.
 */
template <
    typename Errors,        //!< Exposes a Values type
    typename Exception      //!< Exposes a ctor taking a std::string
>
class ExceptionInfo : public Exception
{
public:
    /*!
     * \brief Propagate Value type of template class.
     */
    typedef typename Errors::Value Value;

    /*!
     *
     */
    ExceptionInfo(
            typename Errors::Value error,   //!< [in] Error value
            const std::string& what         //!< [in] Descriptive string
            ) :
        Exception(Errors::toString(error, what)),
        _error(error)
    {

    }

    /*!
     *
     */
    ~ExceptionInfo() throw() { }

    /*!
     * \brief Get the error code.
     */
    const EnumWrapper<typename Errors::Value>& getError() const { return _error; }

private:
    EnumWrapper<typename Errors::Value> _error;   //!< Error code.
};

/*!
 * \brief Reports when a runtime error occurs.
 */
template <
    typename Errors     //! [in] Exposes a Values type
>
class RuntimeError : public ExceptionInfo<Errors, std::runtime_error>
{
public:
    /*!
     * \copydoc ExceptionInfo::ExceptionInfo
     */
    RuntimeError(typename Errors::Value error, const std::string& what = std::string()) :
        ExceptionInfo<Errors, std::runtime_error>(error, what)
    {

    }
};

/*!
 * \brief Reports when an internal logic assumption has been violated.
 */
template <
    typename Errors     //!< Exposes a Values type
>
class LogicError : public ExceptionInfo<Errors, std::logic_error>
{
public:
    /*!
     * \copydoc ExceptionInfo::ExceptionInfo
     */
    LogicError(typename Errors::Value error, const std::string& what = std::string()) :
        ExceptionInfo<Errors, std::logic_error>(error, what)
    {

    }
};

/*!
 * \brief Reports when arguments are invalid.
 */
template <
    typename Errors     //!< [in] Exposes a Values type
>
class InvalidArgument : public ExceptionInfo<Errors, std::invalid_argument>
{
public:
    /*!
     * \copydoc ExceptionInfo::ExceptionInfo
     */
    InvalidArgument(typename Errors::Value error, const std::string& what = std::string()) :
        ExceptionInfo<Errors, std::invalid_argument>(error, what)
    {

    }
};

//!< @}

} // namespace bgsched

#endif
