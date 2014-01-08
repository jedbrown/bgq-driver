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
#ifndef HLCS_SECURITY_DB_GRANT_H
#define HLCS_SECURITY_DB_GRANT_H

#include <hlcs/include/security/Authority.h>
#include <hlcs/include/security/Object.h>

#include <utility/include/UserId.h>

#include <string>

namespace hlcs {
namespace security {
namespace db {

/*!
 * \brief
 */
void __attribute__ ((visibility ("hidden"))) grant(
        const Object& object,
        const Authority& authority,
        const bgq::utility::UserId& user
        );

} // db
} // security 
} // hlcs

#endif
