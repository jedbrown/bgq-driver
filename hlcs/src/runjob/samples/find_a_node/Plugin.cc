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
#include "Plugin.h"

#include "Hostname.h"
#include "logging.h"
#include "QueryBlockStatus.h"
#include "Nodes.h"

#include <algorithm>
#include <iostream>
#include <iterator>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( find_a_node::log );

namespace find_a_node {

Plugin::Plugin() :
    bgsched::runjob::Plugin(),
    _mutex(),
    _configuration(),
    _nodeboard(),
    _nodes(),
    _block(),
    _blockModified( boost::posix_time::not_a_date_time )
{
    LOG_DEBUG_MSG( "Hello from find a node runjob plugin" );

    try {
        this->getBlockInformation();
        this->getNodeInformation();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        throw;
    }
}

void
Plugin::getBlockInformation()
{
    const std::string section( "block" );
    const std::string block = _configuration.getValue( section, "name" );
    _nodeboard = _configuration.getValue( section, "nodeboard" );
    LOG_INFO_MSG( "block:     " << block );
    LOG_INFO_MSG( "nodeboard: " << _nodeboard );

    // get initial block status
    _block.reset(
            new QueryBlockStatus( block )
            );
}

void
Plugin::getNodeInformation()
{
    const Hostname name;
    LOG_DEBUG_MSG( "host: " << std::string(name) );
    _nodes.reset(
            new Nodes(
                _configuration.getValue( name, "nodes")
                )
            );
}

Plugin::~Plugin()
{
    LOG_DEBUG_MSG( "Goodbye from find a node runjob plugin" );
}

void
Plugin::execute(
        bgsched::runjob::Verify& verify
        )
{
    if ( !verify.block().empty() ) {
        if ( verify.block() != _block->id() ) {
            LOG_DEBUG_MSG( "skipping find a node for pid " << verify.pid() << " request on block " << verify.block() );
            return;
        }
    }

    boost::lock_guard<boost::mutex> lock( _mutex );
    (void)verify;

    // get block status
    _block->execute();
    if ( _block->status() != BGQDB::INITIALIZED ) {
        verify.deny_job( "block " + _block->id() + " is not initialized" );
        return;
    }

    // check if block time stamp is newer than what we remember
    if ( _blockModified == boost::posix_time::not_a_date_time ) {
        _blockModified = _block->modified();
        LOG_DEBUG_MSG( "set initial time stamp to " << _blockModified );
    } else if ( _blockModified < _block->modified() ) {
        // newer time stamp, reset any unavailable nodes
        LOG_DEBUG_MSG( "resetting unavailable nodes" );
        _nodes->reset();
    } else {
        // no change
        LOG_DEBUG_MSG( "block has not changed" );
    }

    try {
        Node& node = _nodes->available();
        node.setPid( verify.pid() );
        LOG_DEBUG_MSG( "selected " << node.location() );

        // set block resource information
        verify.block( _block->id() );
        verify.shape( bgsched::runjob::Shape().a(1).b(1).c(1).d(1).e(1) );
        verify.corner(
                bgsched::runjob::Corner().location( _nodeboard + "-" + node.location() )
                );
        verify.deny_job( bgsched::runjob::Verify::DenyJob::No );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        verify.deny_job( e.what() );
    }

    return;
}

void
Plugin::execute(
        const bgsched::runjob::Started& data
        )
{
    boost::lock_guard<boost::mutex> lock( _mutex );
    try {
        Node& node = _nodes->find( data.pid() );
        (void)node;
        LOG_INFO_MSG( "job " << data.job() << " started with pid " << data.pid() );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "could not find pid " << data.pid() << ": " << e.what() );
    }
}

void
Plugin::execute(
        const bgsched::runjob::Terminated& data
        )
{
    boost::lock_guard<boost::mutex> lock( _mutex );
    try {
        Node& node = _nodes->find( data.pid() );
        if ( data.kill_timeout() ) {
            LOG_DEBUG_MSG( node.location() << " unavailable" );
            node.reset( Status::Unavailable );
        } else {
            LOG_DEBUG_MSG( node.location() << " available" );
            node.reset( Status::Available );
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( "could not find pid " << data.pid() << ": " << e.what() );
    }
}

} // find_a_node

extern "C"
{

bgsched::runjob::Plugin*
create()
{
    return new find_a_node::Plugin();
}

void
destroy(bgsched::runjob::Plugin* p)
{
    delete p;
}

} // extern "C"
