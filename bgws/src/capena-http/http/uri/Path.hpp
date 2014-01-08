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

#ifndef CAPENA_HTTP_URL_PATH_HPP_
#define CAPENA_HTTP_URL_PATH_HPP_


#include <iosfwd>
#include <string>
#include <vector>


namespace capena {
namespace http {
namespace uri {


/*! \brief HTTP URI path handling, parsing and comparing. */
class Path : public std::vector<std::string>
{
public:

    /*! \brief Parses a URL path.
     *
     *  A URL path contains parts separated by '/'.
     *  Each part can contain escaped characters, + for space, %xx for any other character.
     *  A part can be empty.
     *
     */
    explicit Path( const std::string& escaped_url_string = std::string() );

    /*! \brief Create from parts.
     *
     *  Note that these are not escaped strings.
     */
    Path(
            std::vector<std::string>::const_iterator start,
            std::vector<std::string>::const_iterator end
        );


    /*! \brief Escape to a string.
     *
     *  The string is like /part[/part...].
     *  Where each part is escaped according to URL rules.
     */
    std::string toString() const;

    /*! \brief Append the new part to the path.
     *
     *  \note new_part should not be escaped.
     */
    Path operator/( const std::string& new_part ) const;

    /*! \brief Concatenate two paths.
     */
    Path operator/( const Path& other ) const;

    /*! \brief Add part to path.
     *
     *  \note new_part should not be escaped.
     */
    Path& operator/=( const std::string& new_part );

    /*! \brief Calculates the parent of this URL.
     */
    Path calcParent() const;

    /*! \brief Returns true iff other is a descendant of this path. */
    bool isDescendantOf( const Path& other ) const;

    /*! \brief Returns true iff other is an ancestor of this path. */
    bool isAncestorOf( const Path& other ) const;

    /*! \brief Returns true iff other is a direct child of this path. */
    bool isChildOf( const Path& other ) const;

    /*! \brief Returns true iff other is the parent of this path. */
    bool isParentOf( const Path& other ) const;


private:

    void _handlePathPartComplete( std::string& new_part_in_out );

};


std::ostream& operator<<( std::ostream& os, const Path& url_path );


} } } // namespace capena::http::uri


#endif
