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

#ifndef CAPENA_HTTP_HTTP_UTILITY_HPP_
#define CAPENA_HTTP_HTTP_UTILITY_HPP_


#include <iosfwd>
#include <string>


namespace capena {
namespace http {
namespace uri {


/*! URL-escapes a string to the stream,
 * ' ' becomes + and non-alnum are converted to %xx escape sequences. */
void escTo(
        const std::string& str,
        std::ostream& os
    );


    /*! Escape a part of a URL. */
std::string escape( const std::string& part );

    /*! Unescape a URL part. */
std::string unescape( const std::string& part );


/*! \brief HTTP form encoding/decoding. */
namespace form {


    /* Does encoding in application/x-www-form-urlencoded. */
void encodeTo(
        const std::string& str,
        std::ostream& os
    );

    /* Does encoding in application/x-www-form-urlencoded. */
std::string encode( const std::string& part );

    /* Does decoding in application/x-www-form-urlencoded. */
std::string decode( const std::string& part );

} // namespace capena::http:uri::form


} } } // namespace capena::http:uri

#endif
