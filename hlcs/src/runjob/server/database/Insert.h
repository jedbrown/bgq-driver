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
#ifndef RUNJOB_SERVER_DATABASE_INSERT_H
#define RUNJOB_SERVER_DATABASE_INSERT_H

#include <db/include/api/job/Operations.h>

namespace runjob {
namespace server {

class Job;

namespace database {

/*!
 * \brief Insert a job into the BGQJob table.
 */
class Insert
{
public:
    /*!
     * \brief ctor.
     */
    Insert(
            const boost::shared_ptr<BGQDB::job::Operations>& operations    //!< [in]
          );

    /*!
     * \brief Execute the prepared statement.
     */
    void execute(
            const boost::shared_ptr<Job>& job,          //!< [in]
            size_t nodesUsed                            //!< [in]
            );

private:
    const boost::shared_ptr<BGQDB::job::Operations> _operations;
    const size_t _nodesPerMidplane;
};

} // database
} // server
} // runjob

#endif
