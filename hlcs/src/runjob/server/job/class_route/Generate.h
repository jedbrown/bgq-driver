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
#ifndef RUNJOB_SERVER_JOB_CLASS_ROUTE_GENERATE_H
#define RUNJOB_SERVER_JOB_CLASS_ROUTE_GENERATE_H

#include "common/fwd.h"
#include "common/Uci.h"

#include "server/job/class_route/fwd.h"

#include "server/job/fwd.h"

#include "server/block/fwd.h"

#include "server/fwd.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include <map>

#include <stdint.h>

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

/*!
 * \brief Generate class route information for each node in a job.
 *
 * Each node's class route is calculated by taking the job's shape, --np,
 * and --ranks-per-node information into account. The information sent
 * to each I/O node is unique based on the compute nodes it manages.
 */
class Generate
{
public:
    /*!
     * \brief I/O node to compute class route mapping.
     */
    typedef std::multimap<Uci, ClassRoute> Container;

public:
    /*!
     * \brief ctor.
     */
    Generate(
            const boost::shared_ptr<Job>& job   //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Generate();

    /*!
     * \brief Add the class route information for the compute nodes an I/O node manages.
     */
    void add(
            const Uci&  node,                                                   //!< [in] I/O node location
            const boost::shared_ptr<bgcios::jobctl::SetupJobMessage>& message   //!< [in] message
            ) const;

private:
    void calculateJobShape();
    
    void subBlock();

    void fullBlock();

    void midplane(
            const block::Midplane& midplane
            );

    void set(
            const boost::shared_ptr<block::ComputeNode>& node,
            const Coordinates& coordinates
            );
            
    void pickJobLeader();

private:
    Container _container;
    boost::scoped_ptr<Coordinates> _jobLeader;
    const boost::shared_ptr<Job> _job;
    boost::scoped_ptr<Np> _np;
    boost::scoped_ptr<Rectangle> _world;
    boost::scoped_ptr<Mapping> _mapping;
};

} // class_route
} // job
} // server
} // runjob

#endif
