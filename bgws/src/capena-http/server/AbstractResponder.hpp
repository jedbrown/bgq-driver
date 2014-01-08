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


/*! \brief Override to generate your response.
 *
 * _processRequest() is called to process the request and generate the response.
 * The application must not call _getRequest() until _processRequest is called
 * (in short, don't call _getRequest() in the constructor).
 *
 * _processRequest() may be called multiple times.
 * The first time it's called, the Request has the headers complete,
 * but it may not have the data.
 * After _processRequest() is called when the Request is complete
 * it will not be called again.
 *
 * The response is complete only when the AbstractResponder goes away,
 * so don't keep it around.
 *
 * Each Responder has a strand, which you can get using _getStrand().
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
            ConnectionPtr connection_ptr, //!< [copy]
            ResponseComplete *response_complete_out //!< [out] if COMPLETE, response is complete.
        );

    void postRequestData(
            const std::string& data, //!< [copy]
            DataContinuesIndicator data_continues
        );


    virtual ~AbstractResponder();


protected:

    /*! \brief Get the strand for the responder. */
    boost::asio::strand& _getStrand()  { return *_strand_ptr; }

    /*! \brief Get the request info (method, URI, headers, and body). */
    const Request& _getRequest() const  { return *_request_ptr; }

    /*! \brief Get the response to be filled in. */
    Response& _getResponse() { return *_response_ptr; }

    /*! \brief Called to process the request. */
    virtual void _processRequest() =0;


private:

    ConnectionPtr _connection_ptr;
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
