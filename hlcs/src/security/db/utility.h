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
#ifndef SECURITY_DB_UTILITY_H
#define SECURITY_DB_UTILITY_H

#include <hlcs/include/security/Types.h>

namespace hlcs {
namespace security {
namespace db {

/*!
 * \brief convert an Object::Type into a string suitable for a database query
 */
__attribute__ ((visibility ("hidden")))
const char* toString(
        Object::Type object     //!< [in]
        );

/*!
 * \brief convert an Action::Type into a string suitable for a database query
 */
__attribute__ ((visibility ("hidden")))
const char* toString(
        Action::Type action     //!< [in]
        );

/*!
 * \brief convert a char to an Action::Type
 */
__attribute__ ((visibility ("hidden")))
Action::Type fromChar(
        char action //!< [in]
        );

} // db
} // security
} // hlcs

#endif
