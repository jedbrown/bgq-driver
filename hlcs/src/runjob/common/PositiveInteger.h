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
#ifndef RUNJOB_CLIENT_OPTIONS_POSITIVE_INTEGER_H
#define RUNJOB_CLIENT_OPTIONS_POSITIVE_INTEGER_H

#include <boost/serialization/access.hpp>

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

namespace runjob {

/*!
 * \brief Lexically convert a string to a positive integer.
 * \ingroup argument_parsing
 */
template <typename T>
class PositiveInteger
{
public:
    typedef T Type;

public:
    /*!
     * \brief ctor.
     */
    PositiveInteger(
            T value = 0
            ) :
        _value( value )
    {

    }

    /*!
     * \brief dtor.
     */
    virtual ~PositiveInteger() { }

    /*!
     * \brief conversion to value;
     */
    operator T() const { return _value; }

    /*!
     * \brief conversion to type.
     */
    T get() const { return _value; }

    /*!
     * \brief
     */
    virtual bool allowZero() const { return false; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _value;
    }

private:
    T _value;
};

/*!
 * \copydoc PositiveInteger
 * \ingroup argument_parsing
 */
template <typename T>
class PositiveIntegerWithZero : public PositiveInteger<T>
{
public:
    PositiveIntegerWithZero(
            T value = 0
            ) : PositiveInteger<T>( value )
    {

    }

    bool allowZero() const { return true; }
};

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
template <typename T>
std::ostream&
operator<<(
        std::ostream& stream,
        const PositiveInteger<T>& integer
        )
{
    stream << integer.get();

    return stream;
}

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
template <typename T>
std::istream&
operator>>(
        std::istream& stream,
        PositiveInteger<T>& integer
        )
{
    // get integer value from stream
    T value;
    stream >> value;

    // if conversion failed, punt
    if ( !stream ) return stream;

    if ( value < 0 ) {
        std::cerr << value << " is not a positive integer" << std::endl;
        stream.setstate( std::ios::failbit );
    } else if ( !integer.allowZero() && value == 0 ) {
        std::cerr << value << " is not a positive integer" << std::endl;
        stream.setstate( std::ios::failbit );
    }

    integer = PositiveInteger<T>( value );

    return stream;
}

} // runjob

#endif
