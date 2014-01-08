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
#ifndef RUNJOB_SAMPLE_FIND_A_NODE_PLUGIN_H_
#define RUNJOB_SAMPLE_FIND_A_NODE_PLUGIN_H_

#include "Configuration.h"
#include "QueryBlockStatus.h"

#include <hlcs/include/bgsched/runjob/Plugin.h>

#include <utility/include/Properties.h>

#include <boost/thread/mutex.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/scoped_ptr.hpp>

#include <iosfwd>

namespace find_a_node {

class Nodes;

/*!
 * \brief Find a node runjob plugin for compiler test buckets.
 */
class Plugin : public bgsched::runjob::Plugin
{
public:
    Plugin();
    
    ~Plugin();

    void execute(
            bgsched::runjob::Verify& data    //!< [in]
            );

    void execute(
            const bgsched::runjob::Started& data  //!< [in]
            );
    
    void execute(
            const bgsched::runjob::Terminated& data //!< [in]
            );

    const Configuration& configuration() const { return _configuration; }

private:
    void getBlockInformation();
    void getNodeInformation();

private:
    boost::mutex _mutex;
    Configuration _configuration;
    std::string _nodeboard;
    boost::scoped_ptr<Nodes> _nodes;
    boost::scoped_ptr<QueryBlockStatus> _block;
    boost::posix_time::ptime _blockModified;
};

} // find_a_node

#endif
