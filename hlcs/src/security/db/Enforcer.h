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
#ifndef HLCS_SECURITY_DB_ENFORCER_H
#define HLCS_SECURITY_DB_ENFORCER_H

#include "Owner.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <hlcs/include/security/Action.h>
#include <hlcs/include/security/Object.h>

#include <utility/include/UserId.h>

namespace hlcs {
namespace security {
namespace db {

/*!
 * \brief
 */
class Enforcer
{
public:
    /*!
     * \brief ctor.
     */
    Enforcer(
            const cxxdb::ConnectionPtr& connection = cxxdb::ConnectionPtr() //!< [in]
            );

    /*!
     * \brief
     */
    bool validate(
            const Object& object,
            Action::Type action,
            const bgq::utility::UserId& user
            ) const;

private:
    void __attribute__ ((visibility("hidden"))) prepareStatement();

private:
    cxxdb::ConnectionPtr _connection;
    cxxdb::QueryStatementPtr _job;
    cxxdb::QueryStatementPtr _block;
    boost::shared_ptr<Owner> _owner;
};

} // db
} // security
} // hlcs

#endif
