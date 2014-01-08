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
#ifndef RUNJOB_SAMPLES_FIND_A_NODE_NODES_H
#define RUNJOB_SAMPLES_FIND_A_NODE_NODES_H

#include "Node.h"

#include <boost/algorithm/string/trim.hpp>

#include <string>
#include <vector>

namespace find_a_node {

class Nodes
{
public:
    explicit Nodes(
            const std::string& list //!< [in] comma separated list of compute node locations (J00,J01,J02)
            );

    Node& available();

    Node& find(
            pid_t pid
            );

    /*!
     * \brief Reset any Unavailable nodes.
     */
    void reset();

private:
    typedef std::vector<Node> Container;

private:
    Container _nodes;
};

} // find_a_node

#endif

