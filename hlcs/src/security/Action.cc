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
#include <hlcs/include/security/Action.h>

#include <boost/algorithm/string.hpp>

#include <boost/lexical_cast.hpp>

namespace hlcs {
namespace security {

std::ostream&
operator<<(
        std::ostream& os, 
        Action::Type a
        )
{
    switch (a) {
        case Action::Create:
            os << "Create";
            break;
        case Action::Read:
            os << "Read";
            break;
        case Action::Update:
            os << "Update";
            break;
        case Action::Delete:
            os << "Delete";
            break;
        case Action::Execute:
            os << "Execute";
            break;
        default:
            os << "Invalid";
            break;
    }

    return os;
}

std::istream&
operator>>(
        std::istream& stream,
        Action::Type& action
        )
{
    std::string value;
    stream >> value;

    // loop through all possible action values comparing each string until
    // we find one that matches
    action = Action::Invalid;
    for ( unsigned int i = 0; i < Action::Invalid && action == Action::Invalid; ++i ) {
        const std::string s = boost::lexical_cast<std::string>( static_cast<Action::Type>(i) );

        // case insensitive equals comparison
        if ( boost::iequals( s, value ) ) {
            action = static_cast<Action::Type>( i );
        }
    }

    if ( action == Action::Invalid ) {
        stream.setstate( std::ios::failbit );
    }

    return stream;
}

} // security
} // hlcs
