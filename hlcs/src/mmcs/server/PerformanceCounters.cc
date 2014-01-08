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

#include "PerformanceCounters.h"

#include "BCNodeInfo.h"
#include "BlockControllerBase.h"

#include <utility/include/Log.h>

#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {

PerformanceCounters::PerformanceCounters(
            const BlockPtr& block
        ) :
    _block( block ),
    _container( boost::make_shared<Container>("mmcs") ),
    _otherData()
{

}

void
PerformanceCounters::getBlockSize()
{
    // Count number of nodes that are not used for link training in the block
    const size_t computes = std::count_if(
            _block->getNodes().begin(),
            _block->getNodes().end(),
            boost::bind(
                std::equal_to<bool>(),
                boost::bind(
                    &BCNodeInfo::_linkio,
                    _1
                    ),
                false
                )
            );

    // Set detail based on I/O or compute block
    if ( _block->isIoBlock() ) {
        _otherData = std::string(
                "c0i" + boost::lexical_cast<std::string>( computes )
                );
    } else {
        // Compute number of link training I/O nodes
        const size_t io = _block->getNodes().size() - computes;

        _otherData = std::string(
                "c" + boost::lexical_cast<std::string>( computes ) +
                "i" + boost::lexical_cast<std::string>( io )
                );
    }
}

PerformanceCounters::Container::Timer::Ptr
PerformanceCounters::create()
{
    return _container->create();
}


void
PerformanceCounters::output(
        const unsigned cookie
        )
{
    if ( !cookie ) {
        // When the cookie is zero, this means mmcs_server was restarted
        // and reconnected to this block. We don't want to store
        // performance counters for this scenario
        return;
    }

    // Set other data based on size of block
    this->getBlockSize();

    // Get contents of our container
    Container::StoragePolicy::Container container;
    _container->get( container );

    // Update each entry with the boot cookie
    try {
        const std::string qualifier = boost::lexical_cast<std::string>( cookie );
        BOOST_FOREACH( Container::StoragePolicy::Container::value_type& i, container ) {
            i.setQualifier( qualifier );
            i.setOtherData( _otherData );
            _container->add( i );
        }
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_WARN_MSG( "Could not convert boot cookie (" << cookie << ") to a string" );
    }

    try {
        _container->output();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

} } // namespace mmcs::server
