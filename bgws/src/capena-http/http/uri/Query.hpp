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

#ifndef CAPENA_HTTP_URI_QUERY_HPP_
#define CAPENA_HTTP_URI_QUERY_HPP_


#include <stdexcept>
#include <string>
#include <vector>


namespace capena {
namespace http {
namespace uri {


class Query
{
public:

    typedef std::pair<std::string,std::string> Parameter; // name=value pairs.
    typedef std::vector<Parameter> Parameters;
    typedef std::vector<std::string> Arguments;


    //! \throws InvalidQueryStringError if the query string is not a valid query string
    static Query parse( const std::string& query_string_escaped );


    Query(
            const Parameters& parameters = Parameters()
        );

    const Parameters& getParameters() const  { return _parameters; }

    // if any parameters, writes ?name=value&name=value for each pair, where name and value are escaped. If no values, empty string.
    std::string calcString() const;

    // calculates args like --name value for each pair, could be used with an option parser like boost::program_options.
    Arguments calcArguments() const;


private:

    Parameters _parameters;

};


class InvalidQueryStringError : public std::invalid_argument
{
public:
    InvalidQueryStringError( const std::string& query_string_escaped );
};


} } } // namespace capena::http::uri

#endif
