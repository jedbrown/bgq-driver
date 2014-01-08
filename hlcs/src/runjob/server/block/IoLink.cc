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
#include "server/block/IoLink.h"

#include "server/block/IoNode.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"

#include <string>

namespace runjob {
namespace server {
namespace block {

LOG_DECLARE_FILE( runjob::server::log );

IoLink::IoLink(
        const IoNode::Ptr& io,
        const std::string& compute
      ) :
    _io( io ),
    _compute( compute )
{
    if ( BG_UCI_GET_COMPONENT(_compute.get()) != BG_UCI_Component_ComputeCardOnNodeBoard ) {
        LOG_RUNJOB_EXCEPTION( error_code::compute_node_invalid, compute << " is not a compute node location" );
    }
}

IoLink::~IoLink()
{

}

} // block
} // server
} // runjob
