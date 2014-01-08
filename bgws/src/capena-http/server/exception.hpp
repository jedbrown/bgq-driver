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

#ifndef CAPENA_SERVER_EXCEPTION_HPP_
#define CAPENA_SERVER_EXCEPTION_HPP_


#include "fwd.hpp"

#include "../http/http.hpp"

#include <stdexcept>
#include <string>
#include <vector>


namespace capena {
namespace server {

/*! \brief Exceptions that result in HTTP responses. */
namespace exception {


/*! \brief Base class for HTTP errors.
 *
 *  The application can throw an Error to generate an HTTP response when an error occurs.
 *
 *  By default, the Content-Type will be text, this can be overridden in _setHeaders.
 *
 *  The what_str will be the entity-body unless the response_str is not empty.
 *
 *  For example, if you want responses to use JSON you'd
 *  override _setHeaders to set the Content-Type to JSON
 *  and set the response_str to the JSON string.
 *
 */
class Error : public std::runtime_error
{
public:

    /*! \brief Constructor */
    Error(
            http::Status http_status, //!< The HTTP status
            const std::string& what_str, //!< Error message string
            const std::string& response_str = std::string() //!< Optional value for the response.
        );

    //! Set the response string.
    void setResponseStr( const std::string& response_str )  { _response_str = response_str; }

    /*! \brief Update the response with the values from this Error.
     *
     *  Sets the status in the response to the HTTP status,
     *  calls _setHeaders(),
     *  notifies headers complete,
     *  sets the entity-body to response_str if set or what_str
     */
    void updateResponse( Response& r ) const;


    //! \brief Get the HTTP status value.
    http::Status getHttpStatus() const  { return _http_status; }

    //! \brief Get the response string, may be empty.
    const std::string& getResponseStr() const  { return _response_str; }


    ~Error() throw() { /* Nothing to do */ }


protected:

    /*! \brief Override to set headers in the response, sets no headers by default. */
    virtual void _setHeaders( Response& /*r*/ ) const { /* Do nothing by default */ }


private:
    http::Status _http_status;
    std::string _response_str;
};


/*! \brief Bad request HTTP error */
class BadRequest : public Error
{
public:
    BadRequest( const std::string& str ) : Error( http::Status::BadRequest, str ) { /* nothing to do */ }
};


/*! \brief Method not allowed HTTP error */
class MethodNotAllowed : public Error
{
public:
    MethodNotAllowed( const http::Methods& allowed_methods );

    ~MethodNotAllowed() throw() { /* Nothing to do */ }

protected:
    void _setHeaders( Response& r ) const; // override

private:
    std::string _methods;
};


/*! \brief Unsupported Media Type HTTP error */
class UnsupportedMediaType : public Error
{
public:
    UnsupportedMediaType( const std::string& content_type );
};


/*! \brief 404 Not Found HTTP error */
class NotFound : public Error
{
public:
    NotFound( const std::string& str ) : Error( http::Status::NotFound, str )  { /* Nothing to do */ }

protected:
    void _setHeaders( Response& r ) const; // override
};


} } } // namespace capena::server::exception


#endif
