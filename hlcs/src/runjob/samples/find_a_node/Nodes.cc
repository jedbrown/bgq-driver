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
#include "Nodes.h"

#include "logging.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <algorithm>

LOG_DECLARE_FILE( find_a_node::log );

namespace find_a_node {

Nodes::Nodes(
        const std::string& list
        ) :
    _nodes()
{
    std::vector<std::string> nodes;
    boost::split( nodes, list, boost::is_any_of(",") );
    if ( nodes.empty() ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument("empty node list") );
    }
    LOG_DEBUG_MSG( nodes.size() << " nodes" );

    BOOST_FOREACH( const std::string& i, nodes ) {
        _nodes.push_back( Node(i) );
    }
}

Node&
Nodes::available()
{
    const Container::iterator result = std::find_if(
            _nodes.begin(),
            _nodes.end(),
            boost::bind(
                std::equal_to<Status>(),
                boost::bind(
                    &Node::status,
                    _1
                    ),
                Status::Available
                )
            );
    if ( result == _nodes.end() ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error( "0 nodes available" )
                );
    }

    return *result;
}

Node&
Nodes::find(
        pid_t pid
        )
{
    const Container::iterator result = std::find_if(
            _nodes.begin(),
            _nodes.end(),
            boost::bind(
                std::equal_to<pid_t>(),
                boost::bind(
                    &Node::pid,
                    _1
                    ),
                pid
                )
            );
    if ( result == _nodes.end() ) {
        BOOST_THROW_EXCEPTION(
                std::runtime_error( "not found" )
                );
    }

    return *result;
}

void
Nodes::reset()
{
    BOOST_FOREACH( Node& node, _nodes ) {
        if ( node.status() == Status::Unavailable ) {
            LOG_DEBUG_MSG( "node " << node.location() << " is available" );
            node.reset( Status::Available );
        }
    }
}

} // find_a_node

