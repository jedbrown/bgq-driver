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

#ifndef CAPENA_SERVER_ABSTRACT_RESPONDER_HPP_
#define CAPENA_SERVER_ABSTRACT_RESPONDER_HPP_


#include "fwd.hpp"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>


namespace capena {
namespace server {


/*! \brief The application implements this class to generate a response for a request.
 *
 * The response for the request, when the response has a body,
 * is complete only when the AbstractResponder goes away,
 * so don't keep a reference to the AbstractResponder longer than necessary.
 *
 * Each Responder has its own strand, which you can get using _getStrand().
 * Applications that want to use callbacks must wrap them in this strand.
 *
 */
class AbstractResponder :
    public boost::enable_shared_from_this<AbstractResponder>,
    boost::noncopyable
{

public:

    enum class ResponseComplete {
        COMPLETE,
        CONTINUE
    };


    AbstractResponder(
            RequestPtr request_ptr
        );

    void initialize(
            boost::asio::io_service& io_service, //!< [ref]
            NotifyStatusHeadersFn notify_status_headers_fn, //!< [copy]
            NotifyDataFn notify_data_fn, //!< [copy]
            ResponseComplete *response_complete_out //!< [out] if COMPLETE, response is complete.
        );

    void postRequestData(
            const std::string& data, //!< [copy]
            DataContinuesIndicator data_continues
        );


    /* \brief Called if the client disconnects. */
    virtual void notifyDisconnect();


    virtual ~AbstractResponder();


protected:

    /*! \brief Get the strand for the responder. */
    boost::asio::strand& _getStrand()  { return *_strand_ptr; }

    /*! \brief Get the request info (method, URI, headers, and body).
     *
     * Note that the application must wait until _processRequest() is called because
     * the request is not available until after the library calls initialize().
     */
    const Request& _getRequest() const  { return *_request_ptr; }

    /*! \brief Get the response to be filled in. */
    Response& _getResponse() { return *_response_ptr; }


    /*! \brief Called to process the request.
     *
     * This library will call _processRequest() to process the request and generate the response.
     *
     * _processRequest() may be called once or twice, depending on the request and the response.
     *
     * The first time _processRequest() is called,
     * the Request has the headers filled in, but it may not have the body.
     * If _getRequest().isComplete() returns true then the request is complete
     * and _processRequest will not be called again.
     *
     * If the application is able to generate a complete response from the headers
     * then _processRequest() will not be called again even if there is a body.
     *
     * If the application throws an exception when _processRequest is called then
     * then _processRequest() will not be called again even if there is a body.
     *
     * Otherwise, _processRequest() will be called again when the full body contents have been received.
     *
     * This function can throw an exception.
     * Any exception will be converted to an HTTP response.
     * If the exception is a capena::server::exception:Error it can respond with an HTTP error with a specific code.
     * If the exception is anything else then the HTTP error is 500 Internal Server Error.
     */
    virtual void _processRequest() =0;


private:

    RequestPtr _request_ptr;
    ResponsePtr _response_ptr;

    boost::shared_ptr<boost::asio::strand> _strand_ptr;


    void _postRequestDataImpl(
            const std::string& data, //!< [copy]
            DataContinuesIndicator data_continues
        );

    void _process();
};


} } // namespace capena::server


#endif
