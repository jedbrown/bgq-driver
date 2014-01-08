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
#ifndef RUNJOB_CLIENT_OPTIONS_MAXIMUM_LENGTH_STRING_H
#define RUNJOB_CLIENT_OPTIONS_MAXIMUM_LENGTH_STRING_H

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <stdexcept>

namespace runjob {

/*!
 * \brief A trivial wrapper around std::string that throws an exception when its maximum length is exceeded.
 * \ingroup argument_parsing
 *
 * This class exists as a vehicle to enable boost::program_options option validation.
 */
template <size_t Length>
class MaximumLengthString
{
public:
    /*!
     * \brief ctor.
     *
     * \throws std::length_error if the string is longer than Length.
     */
    MaximumLengthString(
            const std::string& s = std::string()
          ) :
        _value( s )
    {
        if ( _value.size() > Length ) {
            throw std::length_error(
                    "string length " +
                    boost::lexical_cast<std::string>( _value.size() ) +
                    " is greater than maximum length " +
                    boost::lexical_cast<std::string>( Length )
                    );
        }
    }

    /*!
     * \brief Get underlying value.
     */
    const std::string& getValue() const { return _value; }

    /*!
     * \brief Conversion to string.
     */
    operator const std::string&() const { return _value; }

private:
    std::string _value;
};

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
template <size_t Length>
std::ostream&
operator<<(
        std::ostream& stream,
        const MaximumLengthString<Length>& string
        )
{
    stream << string.getValue();
    return stream;
}

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
template <size_t Length>
std::istream&
operator>>(
        std::istream& stream,
        MaximumLengthString<Length>& string
        )
{
    // get string value from stream
    std::string value;
    stream >> value;

    // assign value
    try {
        string = MaximumLengthString<Length>( value );
    } catch ( const std::length_error& e ) {
        std::cerr << e.what() << std::endl;
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

} // runjob

#endif
