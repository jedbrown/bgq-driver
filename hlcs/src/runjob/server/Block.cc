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
#include "server/Block.h"

#include "common/logging.h"

#include "server/Server.h"

#include <boost/foreach.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {

Block::Block(
        const std::string& name,
        const boost::shared_ptr<BGQBlockNodeConfig>& block,
        const Server::Ptr& server
        ) :
    _name( name ),
    _id( block->blockId() ),
    _config( block ),
    _server( server )
{
    BOOST_ASSERT( !name.empty() );
    BOOST_ASSERT( block );
    LOG_TRACE_MSG( _name << " creation id " << _id );
}

unsigned
Block::a() const
{
    return _config->aNodeSize();
} 

unsigned
Block::b() const {
    return _config->bNodeSize();
}
    
unsigned
Block::c() const {
    return _config->cNodeSize();
}
    
unsigned
Block::d() const
{ 
    return _config->dNodeSize();
}

unsigned
Block::e() const {
    return _config->eNodeSize();
}

Block::~Block()
{
    LOG_TRACE_MSG( __FUNCTION__ );
}

} // server
} // runjob
