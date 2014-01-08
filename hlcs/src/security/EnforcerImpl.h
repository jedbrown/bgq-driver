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
#ifndef BGQ_HLCS_SECURITY_ENFORCER_IMPL_H_
#define BGQ_HLCS_SECURITY_ENFORCER_IMPL_H_

#include "db/Enforcer.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <hlcs/include/security/Enforcer.h>
#include <hlcs/include/security/Types.h>

#include <utility/include/UserId.h>

#include <boost/thread/mutex.hpp>

#include <boost/shared_ptr.hpp>

#include <list>
#include <vector>

namespace hlcs {
namespace security {

class __attribute__ ((visibility("hidden"))) Enforcer::Impl
{
public:
    explicit Impl(
            bgq::utility::Properties::ConstPtr properties,
            const cxxdb::ConnectionPtr& database
            );

    bool validate(
            const Object& object,
            Action::Type action,
            const bgq::utility::UserId& user
            ) const;

private:
    std::string __attribute__ ((visibility("hidden"))) getSectionName(
            Object::Type object
            ) const;

    bool __attribute__ ((visibility("hidden"))) hasAuthority(
            const bgq::utility::UserId& user,
            const Object& object,
            Action::Type action
            ) const;
    
    bool __attribute__ ((visibility("hidden"))) hasAllAuthority(
            const bgq::utility::UserId& user,
            Object::Type object
            ) const;
private:
    bgq::utility::Properties::ConstPtr _properties;     //!< properties file
    boost::shared_ptr<const db::Enforcer> _database;
};

} // security
} // hlcs

#endif
