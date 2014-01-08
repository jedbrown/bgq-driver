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

#ifndef BGQ_HLCS_SECURITY_EXCEPTION_H_
#define BGQ_HLCS_SECURITY_EXCEPTION_H_
/*!
 * \file hlcs/include/security/exception.h
 * \brief exception types.
 */


#include <stdexcept>


namespace hlcs {
namespace security {
namespace exception {


/*!
 * \brief Generic permission denied type.
 */
class NotAuthorized : public std::runtime_error
{
public:
    /*!
     * \brief ctor.
     */
    NotAuthorized( const std::string& what ) : std::runtime_error( what )  { /* Nothing to do. */ }
};

/*!
 * \brief Object not found in database.
 */
class ObjectNotFound : public std::runtime_error
{
public:
    /*!
     * \brief ctor.
     */
    ObjectNotFound( const std::string& what ) : std::runtime_error( what ) { /* nothing to do. */ }
};

/*!
 * \brief Generic database error.
 */
class DatabaseError : public std::runtime_error
{
public:
    /*!
     * \brief ctor.
     */
    DatabaseError( const std::string& what ) : std::runtime_error( what ) { /* nothing to do. */ }
};



} } } // namespace hlcs::security::exception


#endif
