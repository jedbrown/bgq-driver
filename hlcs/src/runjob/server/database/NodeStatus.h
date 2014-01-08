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
#ifndef RUNJOB_SERVER_DATABASE_NODE_STATUS_H
#define RUNJOB_SERVER_DATABASE_NODE_STATUS_H

#include "server/fwd.h"

#include "server/block/Compute.h"

#include "server/database/fwd.h"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <sstream>
#include <vector>

namespace runjob {
namespace server {
namespace database {

/*!
 * \brief
 */
class NodeStatus
{
public:
    /*!
     * \brief ctor.
     */
    NodeStatus(
            const block::Compute::Ptr& block,
            const JobInfo& info
            );

    /*!
     * \brief dtor.
     */
    ~NodeStatus();

    /*!
     * \brief
     */
    const block::Compute::Nodes& get() const { return _nodes; }

    /*!
     * \brief
     */
    void execute();

private:
    void subBlock();

    void smallBlock();

    void largeBlock();

protected:
    std::ostringstream _sql;
    const cxxdb::ConnectionPtr _connection;
    cxxdb::UpdateStatementPtr _statement;
    const boost::shared_ptr<block::Compute> _block;
    const JobInfo& _info;
    block::Compute::Nodes _nodes;
};

} // database
} // server
} // runjob

#endif
