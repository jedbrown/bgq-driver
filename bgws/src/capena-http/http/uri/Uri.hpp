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

#ifndef CAPENA_HTTP_URI_HPP_
#define CAPENA_HTTP_URI_HPP_


#include "Path.hpp"
#include "Query.hpp"

#include <iosfwd>
#include <string>


namespace capena {
namespace http {

/*! \brief HTTP URI handling */
namespace uri {


/*! \brief HTTP URI handling, parse and get the processed pieces.
 *
 * See RFC 2616, section 3.2.
 */
class Uri
{
public:


    enum class Security {
        Secure,
        NotSecure
    };


    /*! \brief Parse an HTTP URL string to a Uri object. */
    static Uri parse( const std::string& uri_str );


    explicit Uri(
            const Path& path, //!< [copied]
            const Query& query = Query() //!< [copied]
        );

    explicit Uri(
            const std::string& host_port, //!< [copied]
            const Path& path = Path(), //!< [copied]
            const Query& query = Query() //!< [copied]
        );

    Uri(
            Security security, //!< [in]
            const std::string& host_port, //!< [copied]
            const Path& path = Path(), //!< [copied]
            const Query& query = Query() //!< [copied]
        );


    bool isAbsolute() const  { return _is_absolute; }
    Security getSecurity() const  { return _security; }
    const std::string& getHostPort() const  { return _host_port; }

    const Path& getPath() const  { return _path; }
    const Query& getQuery() const  { return _query; }

    std::string calcString() const;


private:


    bool _is_absolute;

    Security _security;
    std::string _host_port;

    Path _path;
    Query _query;

};


std::ostream& operator<<( std::ostream& os, Uri::Security security );


} } } // namespace capena::http::uri

#endif
