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
#include <hlcs/include/security/Object.h>

#include <boost/algorithm/string.hpp>

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <exception>
#include <stdexcept>

namespace hlcs {
namespace security {

Object::Object(
        Type type,
        const std::string& name
        ) :
    _type( type ),
    _name( name )
{

}

void
Object::validate(
        Action::Type action
        ) const
{
    if ( _type == Job) {
        switch ( action ) {
            case Action::Read:
            case Action::Execute:
                break;
            default:
                BOOST_THROW_EXCEPTION(
                        std::logic_error(
                            boost::lexical_cast<std::string>(action) + " action not supported for " +
                            boost::lexical_cast<std::string>(_type) + " objects"
                            )
                        );
        }
    } else if ( _type == Block ) {
        // all actions are supported
    } else if ( _type == Hardware ) {
        switch ( action ) {
            case Action::Read:
            case Action::Execute:
                break;
            default:
                BOOST_THROW_EXCEPTION(
                        std::logic_error(
                            boost::lexical_cast<std::string>(action) + " action not supported for " +
                            boost::lexical_cast<std::string>(_type) + " objects"
                            )
                        );
        }
    } else {
        BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    boost::lexical_cast<std::string>(_type) + " objects are not supported."
                    )
                );
    }
}

std::ostream&
operator<<(
        std::ostream& os, 
        Object::Type o
        )
{
    switch ( o ) {
        case Object::Job:
            os << "job";
            break;
        case Object::Block:
            os << "block";
            break;
        case Object::Hardware:
            os << "hardware";
            break;
        default:
            os << "invalid";
            break;
    }

    return os;
}

std::istream&
operator>>(
        std::istream& stream,
        Object::Type& object
        )
{
    std::string value;
    stream >> value;

    // loop through all possible object values comparing each string until
    // we find one that matches
    object = Object::Invalid;
    for ( unsigned int i = 0; i < Object::Invalid && object == Object::Invalid; ++i ) {
        const std::string s = boost::lexical_cast<std::string>( static_cast<Object::Type>(i) );

        // case insensitive equals comparison
        if ( boost::iequals( s, value ) ) {
            object = static_cast<Object::Type>( i );
        }
    }

    if ( object == Object::Invalid ) {
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

} // security
} // hlcs
