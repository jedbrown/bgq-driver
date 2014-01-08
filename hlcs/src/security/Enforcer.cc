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
#include <hlcs/include/security/Enforcer.h>

#include "EnforcerImpl.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "security" );

namespace hlcs {
namespace security {

Enforcer::Enforcer(
        bgq::utility::Properties::ConstPtr properties,
        const cxxdb::ConnectionPtr& database
        ) :
    _impl( new Enforcer::Impl(properties, database) )
{

}

Enforcer::~Enforcer()
{

}

bool
Enforcer::validate(
        const Object& object,
        Action::Type action,
        const bgq::utility::UserId& user
        ) const
{
    return _impl->validate( object, action, user );
}

} // security
} // hlcs
