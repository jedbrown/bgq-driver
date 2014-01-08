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


/*! \brief HTTP URI query string handling, parsing, etc. */
class Query
{
public:

    typedef std::pair<std::string,std::string> Parameter; // name=value pairs.
    typedef std::vector<Parameter> Parameters;
    typedef std::vector<std::string> Arguments;


    //! \throws InvalidQueryStringError if the query string is not a valid query string
    static Query parse( const std::string& query_string_escaped );


    explicit Query(
            const Parameters& parameters = Parameters()
        );

    const Parameters& getParameters() const  { return _parameters; }

    // If any parameters, returns ?name=value&name=value for each pair, where name and value are escaped. If no values, empty string.
    std::string calcString() const;

    // Calculates args like --name value for each pair, could be used with an option parser like boost::program_options.
    Arguments calcArguments() const;


private:

    Parameters _parameters;

};


/*! \brief Exception for when a HTTP URI query string cannot be parsed because it's not valid. */
class InvalidQueryStringError : public std::invalid_argument
{
public:
    InvalidQueryStringError( const std::string& query_string_escaped );
};


} } } // namespace capena::http::uri

#endif
