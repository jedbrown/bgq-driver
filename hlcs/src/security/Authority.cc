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
#include <hlcs/include/security/Authority.h>

#include <boost/assert.hpp>

namespace hlcs {
namespace security {

Authority::Authority(
        const std::string& user,
        Action::Type action
        ) :
    _user( user ),
    _action( action )
{

}

std::ostream&
operator<<(
        std::ostream& os,
        const Authority& authority
        )
{
    os << authority.user() << ": " << authority.action();
    return os;
}

std::ostream&
operator<<(
        std::ostream& os,
        const Authority::Source::Type& source
        )
{
    switch ( source ) {
        case Authority::Source::Granted: os << "Granted"; break;
        case Authority::Source::Properties: os << "Properties"; break;
        default: BOOST_ASSERT( !"unhandled source" );
    }

    return os;
}

} // security
} // hlcs
