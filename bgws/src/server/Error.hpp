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

#ifndef BGWS_EXCEPTION_HPP_
#define BGWS_EXCEPTION_HPP_


#include "capena-http/server/exception.hpp"

#include "capena-http/http/http.hpp"

#include "capena-http/server/Response.hpp"

#include <map>
#include <stdexcept>
#include <string>


namespace bgws {


class Error : public capena::server::exception::Error
{
public:

    typedef std::map<std::string,std::string> Data;


    Error(
            const std::string& text,
            const std::string& operation,
            const std::string& id,
            const Data& data,
            capena::http::Status http_status
        );

    const std::string& getOperation() const  { return _operation; }
    const std::string& getId() const  { return _id; }
    const Data& getData() const  { return _data; }

    ~Error() throw ()  { /* Nothing to do */ }

protected:

    void _setHeaders( capena::server::Response& response ) const;


private:

    std::string _operation;
    std::string _id;
    Data _data;

};


} // namespace bgws


#endif
