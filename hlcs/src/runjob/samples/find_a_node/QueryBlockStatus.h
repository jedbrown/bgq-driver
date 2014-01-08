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
#ifndef RUNJOB_SAMPLE_FIND_A_NODE_QUERY_BLOCK_STATUS_H_
#define RUNJOB_SAMPLE_FIND_A_NODE_QUERY_BLOCK_STATUS_H_

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/BGQDBlib.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <string>
#include <sstream>

namespace find_a_node {

class QueryBlockStatus
{
public:
    /*!
     * \brief ctor.
     */
    QueryBlockStatus(
            const std::string& id   //!< [in]
            );

    void execute();

    const std::string& id() const { return _id; }
    BGQDB::BLOCK_STATUS status() const { return _status; }
    const boost::posix_time::ptime& modified() const { return _modified; }

private:
    cxxdb::ConnectionPtr _connection;
    cxxdb::QueryStatementPtr _statement;
    const std::string _id;
    std::ostringstream _sql;
    BGQDB::BLOCK_STATUS _status;
    boost::posix_time::ptime _modified;
};

} // find_a_node

#endif
