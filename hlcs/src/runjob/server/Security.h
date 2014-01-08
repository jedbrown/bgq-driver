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
#ifndef RUNJOB_SERVER_SECURITY_H
#define RUNJOB_SERVER_SECURITY_H

#include "server/fwd.h"

#include <db/include/api/cxxdb/fwd.h>

#include <db/include/api/job/types.h>

#include <hlcs/include/security/Enforcer.h>

#include <utility/include/UserId.h>

#include <boost/enable_shared_from_this.hpp>

#include <boost/asio/strand.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace runjob {
namespace server {

/*!
 * \brief Central location for security enforcement.
 *
 * This class maintains two hlcs::security::Enforcer objects. One uses
 * a persistent database connection to validate the appropriate block authority
 * for starting jobs. The other Enforcer is used for any other job related
 * command.
 */
class Security : public boost::enable_shared_from_this<Security>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Security> Ptr;

    /*!
     * \brief
     */
    typedef boost::function<void(bool)> ExecuteBlockCallback;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server    //!< [in]
            );

    /*!
     * \brief Validate an action on a job.
     */
    bool validate(
            const bgq::utility::UserId::ConstPtr& user, //!< [in]
            hlcs::security::Action::Type action,        //!< [in]
            BGQDB::job::Id job                          //!< [in]
            );

    /*!
     * \brief Validate an action on a block
     */
    void executeBlock(
            const bgq::utility::UserId::ConstPtr& user,
            const std::string& block,
            const ExecuteBlockCallback& callback
            )
    {
        _strand.post(
                boost::bind(
                    &Security::executeBlockImpl,
                    shared_from_this(),
                    user,
                    block,
                    callback
                    )
                );
    }

    /*!
     * \brief Grant authority.
     */
    void grant(
            const std::string& user,
            hlcs::security::Action::Type action,            //!< [in]
            BGQDB::job::Id job,                             //!< [in]
            const bgq::utility::UserId::ConstPtr& granter   //!< [in]
            );

    /*!
     * \brief Revoke authority.
     */
    void revoke(
            const std::string& user,
            hlcs::security::Action::Type action,            //!< [in]
            BGQDB::job::Id job,                             //!< [in]
            const bgq::utility::UserId::ConstPtr& revoker   //!< [in]
            );

private:
    Security(
            const boost::shared_ptr<Server>& server    //!< [in]
            );

    void executeBlockImpl(
            const bgq::utility::UserId::ConstPtr& user,
            const std::string& block,
            const ExecuteBlockCallback& callback
            );

private:
    const cxxdb::ConnectionPtr _database;
    hlcs::security::Enforcer _blockEnforcer;
    hlcs::security::Enforcer _commandEnforcer;
    boost::asio::strand _strand;
};

} // server
} // runjob

#endif
