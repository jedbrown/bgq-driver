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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include <bgsched/IOBlockFilter.h>

#include "bgsched/IOBlockFilterImpl.h"

using namespace bgsched;
using namespace std;

namespace bgsched {

IOBlockFilter::IOBlockFilter() :
    _impl(new Impl())
{
     // Nothing to do
}

IOBlockFilter::Pimpl
IOBlockFilter::getPimpl() const
{
    return _impl;
}

void
IOBlockFilter::setStatuses(
        const Statuses* statusesPtr
        )
{
    return _impl->setStatuses(statusesPtr);
}

void
IOBlockFilter::setName(
        const string& name
        )
{
    return _impl->setName(name);
}

void
IOBlockFilter::setOwner(
        const string& owner
        )
{
    return _impl->setOwner(owner);
}

void
IOBlockFilter::setUser(
        const string& user
        )
{
    return _impl->setUser(user);
}

void
IOBlockFilter::setExtendedInfo(
        bool extendedInfo
        )
{
    return _impl->setExtendedInfo(extendedInfo);
}

const IOBlockFilter::Statuses
IOBlockFilter::getStatuses() const
{
    return _impl->getStatuses();
}

const string&
IOBlockFilter::getName() const
{
    return _impl->getName();
}

const string&
IOBlockFilter::getOwner() const
{
    return _impl->getOwner();
}

const string&
IOBlockFilter::getUser() const
{
    return _impl->getUser();
}

bool
IOBlockFilter::getExtendedInfo() const
{
    return _impl->getExtendedInfo();
}


} // namespace bgsched
