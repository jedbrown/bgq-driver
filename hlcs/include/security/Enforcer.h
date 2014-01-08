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
 * \file security/Enforcer.h
 * \brief Enforcer class definition.
 */

#ifndef BGQ_HLCS_SECURITY_ENFORCER_H_
#define BGQ_HLCS_SECURITY_ENFORCER_H_

#include <utility/include/Properties.h>
#include <utility/include/UserId.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <hlcs/include/security/Action.h>
#include <hlcs/include/security/Object.h>

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace hlcs {
namespace security {

/*!
 * \brief Policy enforcement.
 *
 * \section thread_safety Thread Safety
 * When not using a persistent database connection, it is safe to invoke members from distinct 
 * Enforcer objects from multiple threads.
 *
 *  \pre BGQDB::init was successful.
 */
class Enforcer
{
public:
    /*!
     * \brief Constructor.
     *
     * \throws std::invalid_argument if !properties
     *
     * \note to prepare the SQL queries once, pass a cxxdb::ConnectionPtr parameter. If the
     * default value is used, a new connection will be obtained from the connection pool for
     * each call to Enforcer::validate.
     */
    explicit Enforcer(
            bgq::utility::Properties::ConstPtr properties,                  //!< [in]
            const cxxdb::ConnectionPtr& database = cxxdb::ConnectionPtr()   //!< [in]
            );

    /*!
     * \brief Destructor.
     */
    ~Enforcer();

    /*!
     * \brief validate an action on an object.
     *
     * \returns true if the action is allowed.
     * \returns false otherwise.
     *
     * \throws std::invalid_argument for unsupported object and action combinations
     * \throws ObjectNotFound if object does not exist
     */
    bool validate(
            const Object& object,               //!< [in] object to act upon
            Action::Type action,                //!< [in] action on the object
            const bgq::utility::UserId& user    //!< [in] user making the request
            ) const;

private:
    /*!
     * \brief Implementation type.
     */
    class __attribute__ ((visibility("hidden"))) Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::scoped_ptr<Impl> Pimpl;

private:
    const Pimpl _impl;
};

} // security
} // hlcs

#endif
