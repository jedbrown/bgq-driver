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
#ifndef RUNJOB_SERVER_DATABASE_DELETE_H
#define RUNJOB_SERVER_DATABASE_DELETE_H

#include "server/job/ExitStatus.h"

#include <db/include/api/job/Operations.h>
#include <db/include/api/job/types.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/function.hpp>

namespace runjob {
namespace server {
namespace database {

/*!
 * \brief Delete a job from the BGQJob table.
 */
class Delete 
{
public:
    /*!
     * \brief Callback type.
     */
    typedef boost::function<void()> Callback;

public:
    /*!
     * \brief ctor.
     */
    explicit Delete(
            boost::asio::io_service& io_service,                        //!< [in]
            const boost::shared_ptr<BGQDB::job::Operations>& operations //!< [in]
            );

    /*!
     * \brief Execute the prepared statement.
     */
    void execute(
            BGQDB::job::Id id,              //!< [in]
            const job::ExitStatus& exit,    //!< [in]
            const Callback& callback        //!< [in]
            );

private:
    void executeImpl(
            BGQDB::job::Id id,
            const job::ExitStatus& exit,
            const Callback& callback
            );

    const boost::shared_ptr<BGQDB::job::Operations> _operations;
    boost::asio::io_service::strand _strand;
};

} // database
} // server
} // runjob

#endif
