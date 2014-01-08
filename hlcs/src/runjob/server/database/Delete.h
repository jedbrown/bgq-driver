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

#include <boost/asio.hpp>

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
     * \brief ctor.
     */
    explicit Delete(
            const boost::shared_ptr<BGQDB::job::Operations>& operations    //!< [in]
            );

    /*!
     * \brief Execute the prepared statement.
     */
    void execute(
            BGQDB::job::Id id,          //!< [in]
            const job::ExitStatus& exit //!< [in]
            );

private:
    const boost::shared_ptr<BGQDB::job::Operations> _operations;
};

} // database
} // server
} // runjob

#endif
