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
#ifndef SECURITY_DB_LOG_H
#define SECURITY_DB_LOG_H

#include <db/include/api/cxxdb/cxxdb.h>

#include <hlcs/include/security/Authority.h>
#include <hlcs/include/security/Object.h>

#include <utility/include/UserId.h>

#include <iostream>

namespace hlcs {
namespace security {
namespace db {

class Log
{
public:
    struct Action
    {
        enum Type {
            Grant,
            Revoke
        };

        static char toChar(
                Type t
                );
    };

public:
    /*!
     * \brief ctor.
     */
    Log(
            const cxxdb::ConnectionPtr& connection, //!< [in]
            const Object& object,                   //!< [in]
            Action::Type action                     //!< [in]
       );

    /*!
     * \brief
     *
     * \throws cxxdb::DatabaseException
     * \throws cxxdb::WrongType
     * \throws cxxdb::ValueTooBig
     */
    void operator()(
            const Authority& authority,             //!< [in]
            const bgq::utility::UserId& user        //!< [in]
            );
private:
    const cxxdb::ConnectionPtr _connection;
    cxxdb::UpdateStatementPtr _statement;
    const Object _object;
};


/*!
 * \brief Object insertion operator.
 */
std::ostream&
operator<<(std::ostream&, Log::Action::Type);

} // db
} // security
} // hlcs

#endif

