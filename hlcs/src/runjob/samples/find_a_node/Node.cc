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
#include "Node.h"

#include "logging.h"

#include <boost/throw_exception.hpp>

#include <stdexcept>

LOG_DECLARE_FILE( find_a_node::log );

namespace find_a_node {

Node::Node(
        const std::string& location
        ) :
    _location( location ),
    _status( Status::Available ),
    _pid( 0 )
{
    LOG_DEBUG_MSG( location );
}

void
Node::reset(
        Status status
        )
{
    LOG_DEBUG_MSG( _location << " completed pid " << _pid );
    _pid = 0;
    _status = status;
}

void
Node::setPid(
        pid_t pid
        )
{
    if ( _pid != 0 ) {
        BOOST_THROW_EXCEPTION(
                std::logic_error( "already in use" )
                );
    }

    _pid = pid;
    _status = Status::Busy;
    LOG_DEBUG_MSG( _location << " starting pid " <<  _pid );
}

} // find_a_node
