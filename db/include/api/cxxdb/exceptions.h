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

#ifndef CXXDB_EXCEPTIONS_H_
#define CXXDB_EXCEPTIONS_H_

#include "diagnostics.h"

#include <boost/exception/all.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>

#define CXXDB_THROW_EXCEPTION( x ) \
    ::boost::throw_exception(::boost::enable_error_info(x) << \
    ::boost::throw_function(BOOST_CURRENT_FUNCTION) << \
    ::boost::throw_file(__FILE__) << \
    ::boost::throw_line((int)__LINE__) << \
    ::cxxdb::stack_error_info( cxxdb::gatherStack() ) \
    )

namespace cxxdb {

typedef boost::error_info<struct tag_stack_str,std::string> stack_error_info;

std::string gatherStack();

/*! \brief Exception thrown when a database error occurs. */
class DatabaseException : public std::runtime_error
{
public:

    DatabaseException(
            const std::string& sql_function_name,
            SQLRETURN sqlrc,
            const DiagnosticRecords& diagnostic_records
        );


    const std::string& getSqlFunctionName() const  { return _sql_function_name; }
    SQLRETURN getSqlrc() const  { return _sqlrc; }
    const DiagnosticRecords& getDiagnosticRecords() const  { return _diagnostic_records; }


    ~DatabaseException() throw() { /* Nothing to do */ }


private:

    std::string _sql_function_name;
    SQLRETURN _sqlrc;
    DiagnosticRecords _diagnostic_records;
};


/*! \brief Exception thrown if a method is called on an invalidated object. */
class InvalidObjectException : public std::logic_error
{
public:

    InvalidObjectException(
            const std::string& object_type,
            const std::string& function_name
        );

};


/*! \brief Exception thrown when a value is too big for field. */
class ValueTooBig : public std::range_error
{
public:
    ValueTooBig(
            const std::string& msg
        );
};


/*! \brief Exception thrown when a method is called that's not valid for the field type. */
class WrongType : public std::logic_error
{
public:
    WrongType(
            const std::string& msg
        );
};


} // namespace cxxdb

#endif
