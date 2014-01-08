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
#include "server/block/IoNode.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"

#include "server/cios/Connection.h"

#include <boost/assert.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace block {

IoNode::IoNode(
        const std::string& block,
        const std::string& location
        ) :
    _block( block ),
    _location( location ),
    _control( ),
    _data( )
{
    // create UCI from location string
    switch (  BG_UCI_GET_COMPONENT(_location.get()) ) {
        case BG_UCI_Component_ComputeCardOnIoBoardOnComputeRack:
        case BG_UCI_Component_ComputeCardOnIoBoardOnIoRack:
            break;
        default:
            LOG_RUNJOB_EXCEPTION( error_code::block_invalid, _location << " is not a compute card on I/O board" );
    }
}

void
IoNode::initialized(
        const cios::Connection::Ptr& control,
        const cios::Connection::Ptr& data
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOG_TRACE_MSG( __FUNCTION__ );
    _control = control;
    _data = data;
}

IoNode::~IoNode()
{

}

} // block
} // server
} // runjob
