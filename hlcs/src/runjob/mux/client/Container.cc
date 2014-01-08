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
#include "mux/client/Container.h"

#include "common/logging.h"

#include "mux/client/Runjob.h"

#include <boost/assert.hpp>

namespace runjob {
namespace mux {
namespace client {

LOG_DECLARE_FILE( runjob::mux::log );

Container::Container(
        boost::asio::io_service& io_service
        ) :
    _strand( io_service ),
    _clients()
{

}

Container::~Container()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

void
Container::add(
        const Runjob::Ptr& client,
        const AddHandler& handler
        )
{
    _strand.post(
            boost::bind(
                &Container::addImpl,
                shared_from_this(),
                client,
                handler
                )
            );
}

void
Container::addImpl(
        const Runjob::Ptr& client,
        const AddHandler& handler
        )
{
    // convert to weak_ptr
    Runjob::WeakPtr weak(client);

    // do the insert
    LOG_TRACE_MSG( "adding client " << client->getId() );
    BOOST_ASSERT(
            _clients.insert(
                std::make_pair(
                    client->getId(),
                    weak
                    )
                ).second
            );

    // invoke callback
    handler();
}

void
Container::remove(
        const Id& id
        )
{
    _strand.post(
            boost::bind(
                &Container::removeImpl,
                shared_from_this(),
                id
                )
            );
}

void
Container::removeImpl(
        const Id& id
        )
{
    Map::const_iterator i = _clients.find(id);
    LOG_TRACE_MSG( "removing client " << id );
    if (i != _clients.end()) {
        (void)_clients.erase(i);
    } else {
        LOG_WARN_MSG( "could not find client " << id << " in container" );
    }
}

void
Container::find(
        const Id& id,
        const FindHandler& handler
        )
{
    _strand.post(
            boost::bind(
                &Container::findImpl,
                shared_from_this(),
                id,
                handler
                )
            );
}

void
Container::findImpl(
        const Id& id,
        const FindHandler& handler
        )
{
    Runjob::Ptr result;

    // find client
    Map::iterator i = _clients.find(id);
    if (i != _clients.end()) {
        // convert to shared_ptr
        result = i->second.lock();
    }

    handler( result );
}

void
Container::get(
        const GetHandler& handler
        )
{
    _strand.post(
            boost::bind(
                &Container::getImpl,
                shared_from_this(),
                handler
                )
            );
}

void
Container::getImpl(
        const GetHandler& handler
        )
{
    handler( _clients );
}

} // client
} // mux
} // runjob
