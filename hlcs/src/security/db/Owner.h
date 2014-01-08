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
#ifndef HLCS_SECURITY_DB_OWNER_H
#define HLCS_SECURITY_DB_OWNER_H

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
class Owner
{
public:
    /*!
     * \brief ctor.
     */
    Owner(
            const cxxdb::ConnectionPtr& connection  //!< [in]
            );

    /*!
     * \brief
     *
     * \throws exception::DatabaseError
     * \throws exception::ObjectNotFound
     * \throws std::invalid_argument if the job name is not convertible to an int64_t
     */
    bool execute(
            const Object& object,
            const bgq::utility::UserId& user
            ) const;

private:
    void __attribute__ ((visibility("hidden"))) prepareJobStatement(
        const cxxdb::ConnectionPtr& connection
        );
    
    void __attribute__ ((visibility("hidden"))) prepareBlockStatement(
        const cxxdb::ConnectionPtr& connection
        );

private:
    cxxdb::QueryStatementPtr _job;
    cxxdb::QueryStatementPtr _block;
};

} // db
} // security
} // hlcs

#endif
