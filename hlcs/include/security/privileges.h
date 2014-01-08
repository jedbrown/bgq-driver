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
#ifndef HLCS_SECURITY_PRIVILEGES_H
#define HLCS_SECURITY_PRIVILEGES_H
/*!
 * \file hlcs/include/security/privileges.h
 * \brief grant, revoke, and list privileges on an Object.
 */

#include <hlcs/include/security/Authorities.h>
#include <hlcs/include/security/Authority.h>
#include <hlcs/include/security/Object.h>

#include <utility/include/Properties.h>
#include <utility/include/UserId.h>

#include <string>

namespace hlcs {
namespace security {

/*!
 * \brief grant the requested action for the user
 *
 * \note this function is reentrant, you can safely invoke it from multiple threads.
 *
 * \pre BGQDB::init was successful.
 *
 * \throws exception::ObjectNotFound
 * \throws exception::DatabaseError
 * \throws std::logic_error if the action is not appropriate for the object
 * \throws std::invalid_argument if the object is not Block or Job
 */
void grant(
        const Object& object,               //!< [in] object to grant authority on
        const Authority& authority,         //!< [in] authority to grant
        const bgq::utility::UserId& user    //!< [in] user responsible for granting authority
        );

/*!
 * \brief revoke the requested action for the user
 *
 * \note this function is reentrant, you can safely invoke it from multiple threads.
 *
 * \pre BGQDB::init was successful.
 *
 * \throws exception::ObjectNotFound
 * \throws exception::DatabaseError
 * \throws std::logic_error if the action is not appropriate for the object
 * \throws std::invalid_argument if the object is not Block or Job
 */
void revoke(
        const Object& object,               //!< [in] object to revoke authority from
        const Authority& authority,         //!< [in] authority to revoke
        const bgq::utility::UserId& user    //!< [in] user responsible for revoking authority
        );

/*!
 * \brief Get the Authority list for an object.
 *
 * \note this function is reentrant, you can safely invoke it from multiple threads.
 *
 * \pre BGQDB::init was successful.
 *
 * \throws exception::ObjectNotFound
 * \throws exception::DatabaseError
 * \throws std::invalid_argument
 */
Authorities list(
        const bgq::utility::Properties& properties, //!< [in]
        const Object& object                        //!< [in]
        );

/*!
 * \brief Get the Authority list for an object excluding any security settings in properties file.
 *
 * \note this function is reentrant, you can safely invoke it from multiple threads.
 *
 * \pre BGQDB::init was successful.
 *
 * \throws exception::ObjectNotFound
 * \throws exception::DatabaseError
 * \throws std::invalid_argument
 */
Authorities list(
        const Object& object                        //!< [in]
        );

} // security
} // hlcs

#endif
