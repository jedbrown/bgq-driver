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

#include "AbstractResponder.hpp"

#include "exception.hpp"
#include "Request.hpp"
#include "Response.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/exception/diagnostic_information.hpp>

#include <string>
#include <vector>


using boost::bind;
using boost::lexical_cast;

using std::runtime_error;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "capena-http" );


namespace capena {
namespace server {


AbstractResponder::AbstractResponder(
        RequestPtr request_ptr
    ) :
        _request_ptr(request_ptr)
{
    // Nothing to do.
}


void AbstractResponder::initialize(
        boost::asio::io_service& io_service,
        NotifyStatusHeadersFn notify_status_headers_fn,
        NotifyDataFn notify_data_fn,
        ResponseComplete *response_complete_out
    )
{
    _strand_ptr.reset( new boost::asio::strand( io_service ) );

    _response_ptr.reset( new Response(
            notify_status_headers_fn,
            notify_data_fn
        ) );

    _process();

    *response_complete_out = (_response_ptr->isComplete() ? ResponseComplete::COMPLETE : ResponseComplete::CONTINUE);
}


void AbstractResponder::postRequestData(
        const std::string& data,
        DataContinuesIndicator data_continues
    )
{
    _strand_ptr->post( bind(
            &AbstractResponder::_postRequestDataImpl,
            shared_from_this(),
            data,
            data_continues
        ) );
}


void AbstractResponder::notifyDisconnect()
{
    LOG_DEBUG_MSG( "Notified of disconnect (ignored)." );
}


void AbstractResponder::_postRequestDataImpl(
        const std::string& data,
        DataContinuesIndicator data_continues
    )
{
    _request_ptr->postData( data, data_continues );

    if ( data_continues == DataContinuesIndicator::END_OF_DATA ) {

        _process();

    }
}


void AbstractResponder::_process()
{
    try {

        _processRequest();

    } catch ( std::exception& e ) {

        _response_ptr->setException( e );

    }
}


AbstractResponder::~AbstractResponder()
{
    LOG_DEBUG_MSG( "Destroying responder." );
}


} } // namespace capena::server
