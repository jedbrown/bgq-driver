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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#include "Token.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

Token::Ptr
Token::create(
        const std::string& name,
        const Callback& callback
        )
{
    const Ptr result( new Token(name, callback) );
    return result;
}

Token::Token(
        const std::string& name,
        const Callback& callback
        ) : 
    _name( name ),
    _callback( callback )
{

}

Token::~Token()
{
    LOG_TRACE_MSG( __FUNCTION__ << " for " << _name );
    if ( !_callback ) return;
    _callback();
}

} // env
} // server
} // mmcs

