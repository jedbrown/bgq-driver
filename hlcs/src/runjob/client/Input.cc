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
#include "client/Input.h"

#include "client/FileInput.h"
#include "client/MuxConnection.h"
#include "client/StreamInput.h"

#include "common/logging.h"

#include <boost/bind.hpp>

#include <sys/stat.h>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {

Input::Ptr
Input::create(
        boost::asio::io_service& io_service,
        const boost::weak_ptr<MuxConnection>& mux,
        const int fd
        )
{
    Ptr result;
    try {
        result.reset(
                new StreamInput( io_service, mux, fd )
                );
    } catch ( const boost::system::system_error& e ) {
        LOG_TRACE_MSG( "could not create StreamInput for descriptor " << fd );
        LOG_TRACE_MSG( e.what() );
        LOG_TRACE_MSG( "assuming descriptor " << fd << " is a file" );
        result.reset(
                new FileInput( mux, fd )
                );
    }

    return result;
}

Input::Input(
        const boost::weak_ptr<MuxConnection>& mux
        ) :
    _mux( mux )
{

}

Input::~Input()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
}

} // client
} // runjob
