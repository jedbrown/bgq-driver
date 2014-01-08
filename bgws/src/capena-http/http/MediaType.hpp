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

#ifndef CAPENA_HTTP_MEDIA_TYPE_HPP_
#define CAPENA_HTTP_MEDIA_TYPE_HPP_


#include <iosfwd>
#include <map>
#include <string>


namespace capena {
namespace http {


/*! \brief Handling media type strings, parsing, etc.
 *
 *  See RFC 2616 section 3.7 for definition of Media Types. It's used in the Content-Type and Accept header fields.
 */
class MediaType
{
public:

    typedef std::map<std::string,std::string> Parameters;


    static const MediaType JSON;
    static const MediaType JSON_UTF8;


    static MediaType parse( const std::string& str );


    MediaType()  { /* Nothing to do */ }

    MediaType(
            const std::string& type, //!< [copy]
            const std::string& subtype, //!< [copy]
            const Parameters& parameters = Parameters() //!< [copy]
        );

    const std::string& getType() const  { return _type; }
    const std::string& getSubtype() const  { return _subtype; }
    const Parameters& getParameters() const  { return _parameters; }

    //! MediaTypes are equivalent if the type and subtype are the same.
    bool equivalent( const MediaType& other ) const;


private:

    std::string _type;
    std::string _subtype;
    Parameters _parameters;
};


std::ostream& operator<<( std::ostream& os, const MediaType& ct );


} } // namespace capena::http

#endif
