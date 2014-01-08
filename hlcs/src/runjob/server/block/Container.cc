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
#include "server/block/Container.h"

#include "common/logging.h"
#include "common/Exception.h"

#include "server/block/Compute.h"
#include "server/block/Io.h"
#include "server/block/IoLink.h"
#include "server/block/IoNode.h"

#include "server/cios/Connection.h"

#include "server/job/Container.h"

#include "server/Options.h"
#include "server/Server.h"

#include <bgq_util/include/Location.h>

#include <control/include/bgqconfig/xml/BGQBlockXML.h>
#include <control/include/bgqconfig/xml/BGQMachineXML.h>
#include <control/include/bgqconfig/BGQBlockNodeConfig.h>

#include <db/include/api/BGQDBlib.h>

#include <algorithm>

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace block {

Container::Ptr
Container::create(
        const Server::Ptr& server
        )
{
    const Ptr result(
            new Container( server )
            );

    return result;
}

Container::Container(
        const Server::Ptr& server
        ) :
    _io_service( server->getIoService() ),
    _strand( _io_service ),
    _server( server ),
    _blocks(),
    _io()
{
    
}

void
Container::loadMachine(
        const LoadMachineCallback& callback
        )
{
    boost::shared_ptr<BGQMachineXML> machine;

    // extract machine XML from database
    LOG_INFO_MSG( "extracting machine description" );
    std::stringstream xml;
    const BGQDB::STATUS result = BGQDB::getMachineXML(xml);
    if ( result != BGQDB::OK ) {
        LOG_ERROR_MSG( "could not get machine XML description: " << result );

        callback( machine, error_code::database_error );
        return;
    }

    // create bgqconfig object
    LOG_DEBUG_MSG( "creating machine XML object" );
    try {
        machine.reset( BGQMachineXML::create(xml) );
    } catch ( const XMLException& e ) {
        LOG_TRACE_MSG( xml.str() );
        LOG_ERROR_MSG( e.what() );

        callback( machine, error_code::database_error );
        return;
    }

    // ensure we got something
    if ( !machine ) {
        LOG_ERROR_MSG( "could not create machine  XML" );
        
        callback( machine, error_code::database_error );
        return;
    }

    // get number of compute and I/O nodes
    const size_t compute(
            machine->_midplanes.size() * 
            bgq::util::Location::NodeBoardsOnMidplane * 
            bgq::util::Location::ComputeCardsOnNodeBoard
            );
    const size_t io(
            machine->_ioBoards.size() * bgq::util::Location::ComputeCardsOnIoBoard
            );

    // ensure we have something
    if ( compute == 0 ) {
        LOG_ERROR_MSG( "invalid number of compute nodes (" << compute << ") from machine XML" );
        
        callback( machine, error_code::database_error );
        return;
    } else if ( io == 0 ) {
        LOG_ERROR_MSG( "invalid number of io nodes (" << io << ") from machine XML" );
        
        callback( machine, error_code::database_error );
        return;
    }

    // iterate through I/O links
    LOG_DEBUG_MSG( "machine has " << machine->_ioLinks.size() << " I/O links" );
    BOOST_FOREACH( BGQMachineIOLink* link, machine->_ioLinks ) {
        if ( link->_enabled ) {
            LOG_TRACE_MSG( "link " << link->_computeNode << " <--> " << link->_ioNode );
        } else {
            LOG_TRACE_MSG( "link " << link->_computeNode << " <--> " << link->_ioNode << " disabled" );
        }
    }

    callback( machine, error_code::success );
}

Container::~Container()
{

}

void
Container::find(
        const std::string& id,
        const FindCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Container::findImpl,
                this,
                id,
                callback
                )
            );
}

void
Container::get(
        const GetCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Container::getImpl,
                this,
                callback
                )
            );
}

void
Container::findImpl(
        const std::string& id,
        const FindCallback& callback
        )
{
    Compute::Ptr result;

    const Blocks::const_iterator iterator = std::find_if(
            _blocks.begin(),
            _blocks.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Block::name,
                    _1
                    ),
                id
                )
            );

    if ( iterator != _blocks.end() ) {
        result = boost::dynamic_pointer_cast<Compute>( *iterator );
    }

    callback( result );
}

void
Container::add(
        const Block::Ptr& block
        )
{
    // ensure block does not exist already
    const Blocks::const_iterator iterator = std::find_if(
            _blocks.begin(),
            _blocks.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Block::name,
                    _1
                    ),
                block->name()
                )
            );

    if ( iterator != _blocks.end() ) {
        LOG_RUNJOB_EXCEPTION( error_code::block_already_exists, block->name() );
    }

    LOG_INFO_MSG( "added" );
    _blocks.push_back(block);
}

void
Container::create(
        const std::string& name,
        const boost::shared_ptr<BGQMachineXML>& machine,
        const ResponseCallback& callback
        )
{
    LOGGING_DECLARE_BLOCK_MDC( name );
    if ( !machine ) {
        this->loadMachine(
                _strand.wrap(
                    boost::bind(
                        &Container::createImpl,
                        this,
                        _1,
                        _2,
                        name,
                        callback
                        )
                    )
                );

        return;
    }

    _strand.post(
            boost::bind(
                &Container::createImpl,
                this,
                machine,
                error_code::success,
                name,
                callback
                )
        );
}

void
Container::initialized(
        const std::string& name,
        const ResponseCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Container::initializedImpl,
                this,
                name,
                callback
                )
            );
}

void
Container::terminating(
        const std::string& name,
        const ResponseCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Container::terminatingImpl,
                this,
                name,
                callback
                )
            );
}

void
Container::remove(
        const std::string& name,
        const ResponseCallback& callback
        )
{
    _strand.post(
            boost::bind(
                &Container::removeImpl,
                this,
                name,
                callback
                )
            );
}

void
Container::createImpl(
        const boost::shared_ptr<BGQMachineXML>& machine,
        const error_code::rc error,
        const std::string& name,
        const ResponseCallback& callback
        )
{
    LOGGING_DECLARE_BLOCK_MDC( name );

    if ( error || !machine ) {
        callback( error, "invalid machine XML" );
        return;
    }

    const Blocks::const_iterator block = std::find_if(
            _blocks.begin(),
            _blocks.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Block::name,
                    _1
                    ),
                name
                )
            );
    if ( block != _blocks.end() ) {
        callback( error_code::block_already_exists, "block " + name + " already exists" );
        return;
    }

    // get block xml
    LOG_INFO_MSG( "extracting block description" );
    std::stringstream xml;
    const BGQDB::STATUS result = BGQDB::getBlockXML(xml, name);
    if ( result != BGQDB::OK ) {
        LOG_ERROR_MSG( "could not extract block XML: " << result );
        callback( error_code::block_not_found, "could not get XML definition for block " + name );
        return;
    }

    try {
        // create block XML
        const boost::scoped_ptr<BGQBlockXML> blockXml(
                BGQBlockXML::create( xml, machine.get() )
                );
        if ( !blockXml ) {
            callback( error_code::block_invalid, "could not create XML for block " + name );
            return;
        }

        // create block configuration
        const boost::shared_ptr<BGQBlockNodeConfig> blockConfig(
                new BGQBlockNodeConfig( machine.get(), blockXml.get() )
                );

        if ( blockXml->_ioboards.size() ) {
            this->createIo( name, blockConfig );
        } else if ( blockXml->_midplanes.size() ) {
            this->createCompute( name, machine, blockConfig );
        } else {
            BOOST_ASSERT(!"both I/O boards and midplanes are empty");
        }
        
        callback( error_code::success, "success" );
    } catch ( const XMLException& xml_exception ) {
        callback( error_code::block_invalid, xml_exception.what() );
    } catch ( const Exception& e ) {
        callback( e.getError(), e.what() );
    } catch ( const std::exception& e ) {
        callback( error_code::block_invalid, e.what() );
    }
}

void
Container::createIo(
        const std::string& name,
        const boost::shared_ptr<BGQBlockNodeConfig>& blockConfig
        )
{
    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    const Io::Ptr block(
            Io::create(
                name,
                blockConfig,
                server
                )
            );
    
    // ensure all of the I/O nodes do not already exist
    BOOST_FOREACH( IoNode::Ptr node, block->getNodes() ) {
        const Uci& location = node->getLocation();
        const IoMap::const_iterator result = _io.find( location );
        if ( result != _io.end() ) {
            const IoNode::Ptr previous = result->second;
            LOG_RUNJOB_EXCEPTION(
                    error_code::block_invalid,
                    "node " << location << " already exists in block " << previous->getBlock()
                    );
        }
    }

    this->add( block );

    BOOST_FOREACH( const block::IoNode::Ptr& node, block->getNodes() ) {
        LOG_DEBUG_MSG( "adding " << node->getLocation() );
        BOOST_ASSERT( _io.insert(
                    IoMap::value_type(
                        node->getLocation(),
                        node
                        )
                    ).second
                );
    }
}

void
Container::createCompute(
        const std::string& name,
        const boost::shared_ptr<BGQMachineXML>& machine,
        const boost::shared_ptr<BGQBlockNodeConfig>& blockConfig
        )
{

    const Server::Ptr server( _server.lock() );
    if ( !server ) return;

    // container for I/O links for this compute block
    Io::Links links;

    for (
            BGQBlockNodeConfig::midplane_iterator midplane = blockConfig->midplaneBegin();
            midplane != blockConfig->midplaneEnd();
            ++midplane
        )
    {
        LOG_TRACE_MSG( "looking at midplane " << midplane->posInMachine() );
        BOOST_FOREACH( const BGQNodePos& compute, *midplane->computes() ) {
            const std::string location(
                    midplane->posInMachine() +
                    "-" +
                    boost::lexical_cast<std::string>( compute )
                    );

            BOOST_FOREACH( const BGQMachineIOLink* link, machine->_ioLinks ) {
                if ( !link->_enabled ) continue;
                if ( location != link->_computeNode ) continue;

                LOG_DEBUG_MSG( location << " links to " << link->_ioNode );

                // find the I/O node in our map
                const Uci uci( link->_ioNode );
                const IoMap::iterator io = _io.find( uci );
                if ( io == _io.end() ) {
                    LOG_RUNJOB_EXCEPTION( error_code::block_invalid, location << " has no connected I/O block" );
                }

                links.push_back(
                        IoLink( io->second, link->_computeNode )
                        );
            }
        }
    }

    // ensure this compute block has at least one I/O link
    if ( links.empty() ) {
        LOG_RUNJOB_EXCEPTION( error_code::block_invalid, "block has no I/O links" );
    }

    const Compute::Ptr block(
            Compute::create(
                name,
                blockConfig,
                server,
                links
                )
            );

    this->add( block );
}

void
Container::initializedImpl(
        const std::string& name,
        const ResponseCallback& callback
        )
{
    LOGGING_DECLARE_BLOCK_MDC( name );
    const Blocks::iterator result = std::find_if(
            _blocks.begin(),
            _blocks.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Block::name,
                    _1
                    ),
                name
                )
            );
    if ( result == _blocks.end() ) {
        callback( error_code::block_not_found, "block not found" );
        return;
    }

    if ( Io::Ptr io = boost::dynamic_pointer_cast<Io>(*result) ) {
        try {
            io->initialized();
            callback( error_code::success, "success" );
        } catch ( const Exception e ) {
            callback( e.getError(), e.what() );
        }
    }
}

void
Container::terminatingImpl(
        const std::string& name,
        const ResponseCallback& callback
        )
{
    LOGGING_DECLARE_BLOCK_MDC( name );
    const Blocks::iterator result = std::find_if(
            _blocks.begin(),
            _blocks.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Block::name,
                    _1
                    ),
                name
                )
            );
    if ( result == _blocks.end() ) {
        callback( error_code::block_not_found, "block not found" );
        return;
    }

    if ( Io::Ptr io = boost::dynamic_pointer_cast<Io>(*result) ) {
        io->terminating();
    }

    callback( error_code::success, "success" );
}

void
Container::removeImpl(
        const std::string& name,
        const ResponseCallback& callback
        )
{
    LOGGING_DECLARE_BLOCK_MDC( name );

    const Blocks::iterator result = std::find_if(
            _blocks.begin(),
            _blocks.end(),
            boost::bind(
                std::equal_to<std::string>(),
                boost::bind(
                    &Block::name,
                    _1
                    ),
                name
                )
            );
    if ( result == _blocks.end() ) {
        callback( error_code::block_not_found, "block '" + name + "' not found" );
        return;
    }

    const Server::Ptr server = _server.lock();
    if ( !server ) return;
    
    // remove any jobs still left on compute blocks
    if ( Compute::Ptr compute = boost::dynamic_pointer_cast<Compute>(*result) ) {
        server->getJobs()->eof( compute );
    }

    _blocks.erase( result );
    LOG_INFO_MSG( "removed" );

    // remove I/O nodes if we find them
    for ( IoMap::iterator i = _io.begin(); i != _io.end(); ) {
        const IoNode::Ptr& node = i->second;
        if ( node->getBlock() == name ) {
            if ( const cios::Connection::Ptr control = node->getControl() ) {
                control->stop();
            }
            if ( const cios::Connection::Ptr data = node->getData() ) {
                data->stop();
            }

            // kill any jobs using this node
            server->getJobs()->eof( node->getLocation() );

            LOG_DEBUG_MSG( "removing I/O node " << node->getLocation() );
            _io.erase( i++ );
        } else {
            ++i;
        }
    }

    callback( error_code::success, "success" );
}

void
Container::getImpl(
        const GetCallback& callback
        )
{
    callback( _blocks );
}

} // block
} // server
} // runjob
